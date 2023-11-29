// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Fan.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PuzzleNameD/PuzzleNameD.h"

AFan::AFan() : Super()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent->SetWorldScale3D(FVector(2.0f));

	Blade = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Blade"));
	Blade->SetupAttachment(RootComponent);
	Blade->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Range = CreateDefaultSubobject<UBoxComponent>(TEXT("Range"));
	Range->SetupAttachment(RootComponent);
	Range->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Range->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	Range->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Range->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	Range->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void AFan::BeginPlay()
{
	Super::BeginPlay();

	RangeX = Range->GetScaledBoxExtent().X;
	RangeY = Range->GetScaledBoxExtent().Y;
	RangeZ = Range->GetScaledBoxExtent().Z;
}

void AFan::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Blade)
	{
		Blade->AddLocalRotation(FRotator(20.0f, 0.0f, 0.0f));
	}
	Push();
}

void AFan::Push()
{
	if (Range == nullptr) return;

	/*
	TArray<AActor*> OverlappingCharacters;
	Range->GetOverlappingActors(OverlappingCharacters, ACharacter::StaticClass());
	if (OverlappingCharacters.Num() == 0)
	{
		bIsPushingCharacter = false;
	}
	*/
	TArray<UPrimitiveComponent*> OverlappingComponents;
	Range->GetOverlappingComponents(OverlappingComponents);
	if (OverlappingComponents.Num() > 0)
	{
		FVector StartPoint = Range->GetComponentLocation() - GetActorForwardVector() * RangeX - GetActorForwardVector() * StartRadius * Tangent;
		FHitResult CenterHitResult;
		FVector CenterStart = Range->GetComponentLocation() -GetActorForwardVector() * RangeX;
		FVector CenterDirection = CenterStart - StartPoint;
		FVector CenterEnd = CenterStart + CenterDirection * (RangeX * 2 / (StartRadius * Tangent));
		//TArray<AActor*> CenterIgnoredActors;
		//UKismetSystemLibrary::LineTraceSingle(this, CenterStart, CenterEnd, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility), false, CenterIgnoredActors, EDrawDebugTrace::ForOneFrame, CenterHitResult, true);
		GetWorld()->LineTraceSingleByChannel(CenterHitResult, CenterStart, CenterEnd, ECC_Interactable);
		for (int32 DegreeCount = 0; DegreeCount < 360.0f / DegreeBetweenLines; DegreeCount++)
		{
			for (int32 LineCount = 1; LineCount <= UnitNumberOfLines; LineCount++)
			{
				FHitResult HitResult;
				FVector DeltaVector = GetActorUpVector().RotateAngleAxis(DegreeCount * DegreeBetweenLines, GetActorForwardVector());
				FVector Start = Range->GetComponentLocation() - GetActorForwardVector() * RangeX + DeltaVector * float(LineCount) / UnitNumberOfLines * StartRadius;
				FVector Direction = Start - StartPoint;
				FVector End = Start + Direction * (RangeX * 2 / (StartRadius * Tangent));
				//TArray<AActor*> IgnoredActors;
				//UKismetSystemLibrary::LineTraceSingle(this, Start, End, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility), false, IgnoredActors, EDrawDebugTrace::ForOneFrame, HitResult, true);
				GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Interactable);
				if (HitResult.GetComponent())
				{
					float Power = MaxForce * (1 - HitResult.Time) + MaxForce * 0.5f * FMath::FRand();
					FVector NormalizedForce = End - Start;
					NormalizedForce.Normalize();
					FVector Force = NormalizedForce * Power;
					ACharacter* Character = Cast<ACharacter>(HitResult.GetActor());
					if (Character && Character->GetCharacterMovement())
					{
						/*if (!bIsPushingCharacter && ((Character->GetActorLocation() - GetActorLocation()) * GetActorForwardVector()).Size() > 170.0f)
						{
							Character->GetCharacterMovement()->Velocity = Character->GetVelocity() * GetActorForwardVector().GetAbs();
							bIsPushingCharacter = true;
						}*/
						Character->GetCharacterMovement()->AddForce(Force);
					}
					if (HitResult.GetComponent()->IsSimulatingPhysics())
					{
						HitResult.GetComponent()->AddForceAtLocation(Force, HitResult.ImpactPoint);
					}
				}
			}
		}
		if (CenterHitResult.GetComponent())
		{
			float Power = MaxForce * (1 - CenterHitResult.Time) + MaxForce * 0.5f * FMath::FRand();
			FVector Force = GetActorForwardVector() * Power;
			ACharacter* Character = Cast<ACharacter>(CenterHitResult.GetActor());
			if (Character && Character->GetCharacterMovement())
			{
				/*if (!bIsPushingCharacter && ((Character->GetActorLocation() - GetActorLocation()) * GetActorForwardVector()).Size() > 170.0f)
				{
					Character->GetCharacterMovement()->Velocity = Character->GetVelocity() * GetActorForwardVector().GetAbs();
					bIsPushingCharacter = true;
				}*/
				Character->GetCharacterMovement()->AddForce(Force);
			}
			if (CenterHitResult.GetComponent()->IsSimulatingPhysics())
			{
				CenterHitResult.GetComponent()->AddForceAtLocation(Force, CenterHitResult.ImpactPoint);
			}
		}
		/*for (int32 Idx = 0; Idx < RangeY / DistanceBetweenLines; Idx++)
		{
			for (int32 Index = 0; Index < RangeZ / DistanceBetweenLines; Index++)
			{
				FHitResult HitResult;
				float Y = Idx * DistanceBetweenLines * 2 - RangeY;
				float Z = Index * DistanceBetweenLines * 2 - RangeZ;
				FVector Start = Range->GetComponentLocation() - GetActorForwardVector() * RangeX + GetActorRightVector() * Y + GetActorUpVector() * Z;
				FVector End = Start + GetActorForwardVector() * RangeX * 2;
				//FVector End = Start + GetActorForwardVector() * RangeX * 2 + ((GetActorUpVector().Rotation() - GetActorRightVector().Rotation()) * RadialIndex * 0.5f).Vector() * 200.0f;
				FCollisionQueryParams CollisionParams;
				CollisionParams.AddIgnoredActor(this);
				TArray<AActor*> IgnoredActors;
				//UKismetSystemLibrary::LineTraceSingle(this, Start, End, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility), false, IgnoredActors, EDrawDebugTrace::ForOneFrame, HitResult, true);
				//GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility, CollisionParams);
				if (HitResult.GetComponent())
				{
					float Power = FMath::Abs(FVector::DotProduct(GetActorForwardVector(), -HitResult.ImpactNormal));
					Power *= MaxForce * (1 - HitResult.Time) + MaxForce * 0.5 * FMath::FRand();
					FVector Direction = GetActorForwardVector() - HitResult.ImpactNormal;
					Direction.Normalize();
					FVector Force = Direction * Power;

					float Power = MaxForce * (1 - HitResult.Time) + MaxForce * 0.5f * FMath::FRand();
					FVector Direction = End - Start;
					Direction.Normalize();
					FVector Force = Direction * Power;

					ACharacter* Character = Cast<ACharacter>(HitResult.GetActor());
					if (Character && Character->GetCharacterMovement())
					{
						if (!bIsPushingCharacter && ((Character->GetActorLocation() - GetActorLocation()) * GetActorForwardVector()).Size() > 170.0f)
						{
							Character->GetCharacterMovement()->Velocity = Character->GetVelocity() * GetActorForwardVector().GetAbs();
							bIsPushingCharacter = true;
						}
						Character->GetCharacterMovement()->AddForce(Force);
					}
					if (HitResult.GetComponent()->IsSimulatingPhysics())
					{
						HitResult.GetComponent()->AddForceAtLocation(Force, HitResult.ImpactPoint);
					}
				}
			}
		}*/
	}
}

void AFan::SetCollisionResponse(ECollisionResponse NewResponse)
{
	Super::SetCollisionResponse(NewResponse);

	if (Blade)
	{
		Blade->SetCollisionResponseToAllChannels(NewResponse);
		/*Blade->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, NewResponse);
		Blade->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, NewResponse);
		Blade->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, NewResponse);*/
	}
}

void AFan::OnPreview()
{
	Super::OnPreview();

	if (Blade)
	{
		Blade->SetMaterial(0, BladePreviewMaterial);
	}
	SetActorTickEnabled(false);
}

void AFan::OnPlace()
{
	Super::OnPlace();

	if (Blade)
	{
		Blade->SetMaterial(0, BladeOriginalMaterial);
	}
	SetActorTickEnabled(true);
}
