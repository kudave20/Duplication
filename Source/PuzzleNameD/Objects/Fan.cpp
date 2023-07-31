// Fill out your copyright notice in the Description page of Project Settings.


#include "Fan.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "PuzzleNameD/PuzzleNameD.h"

AFan::AFan() : Super()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent->SetWorldScale3D(FVector(2.0f));

	Blade = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Blade"));
	Blade->SetupAttachment(RootComponent);
	Blade->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	Blade->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

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

	TArray<AActor*> OverlappingCharacters;
	Range->GetOverlappingActors(OverlappingCharacters, ACharacter::StaticClass());
	if (OverlappingCharacters.Num() == 0)
	{
		bIsPushingCharacter = false;
	}
	TArray<UPrimitiveComponent*> OverlappingComponents;
	Range->GetOverlappingComponents(OverlappingComponents);
	if (OverlappingComponents.Num() > 0)
	{
		for (int32 Idx = 0; Idx <= RangeX / DistanceBetweenLines; Idx++)
		{
			for (int32 Index = 0; Index <= RangeZ / DistanceBetweenLines; Index++)
			{
				FHitResult HitResult;
				float X = Idx * DistanceBetweenLines * 2 - RangeX;
				float Z = Index * DistanceBetweenLines * 2 - RangeZ;
				FVector Start = GetActorLocation() + GetActorForwardVector() * X + GetActorUpVector() * Z;
				FCollisionQueryParams CollisionParams;
				CollisionParams.AddIgnoredActor(this);
				GetWorld()->LineTraceSingleByChannel(HitResult, Start, Start + GetActorRightVector() * RangeY * 2, ECC_Interactable, CollisionParams);
				if (HitResult.GetComponent())
				{
					float Power = FMath::Abs(FVector::DotProduct(GetActorRightVector(), -HitResult.ImpactNormal));
					Power *= MaxForce * (1 - HitResult.Time) + MaxForce * 0.5 * FMath::FRand();
					FVector Direction = GetActorRightVector() - HitResult.ImpactNormal;
					Direction.Normalize();
					FVector Force = Direction * Power;
					ACharacter* Character = Cast<ACharacter>(HitResult.GetActor());
					if (Character && Character->GetCharacterMovement())
					{
						if (!bIsPushingCharacter && ((Character->GetActorLocation() - GetActorLocation()) * GetActorRightVector()).Size() > 170.0f)
						{
							Character->GetCharacterMovement()->Velocity = Character->GetVelocity() * GetActorRightVector().GetAbs();
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
		}
	}
}

void AFan::SetCollisionResponse(ECollisionResponse NewResponse)
{
	Super::SetCollisionResponse(NewResponse);

	if (Blade)
	{
		Blade->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, NewResponse);
		Blade->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, NewResponse);
	}
}

void AFan::OnPreview_Implementation()
{
	Super::OnPreview_Implementation();

	if (Blade)
	{
		Blade->SetMaterial(0, BladePreviewMaterial);
	}
	SetActorTickEnabled(false);
}

void AFan::OnPlace_Implementation()
{
	Super::OnPlace_Implementation();

	if (Blade)
	{
		Blade->SetMaterial(0, BladeOriginalMaterial);
	}
	SetActorTickEnabled(true);
}
