// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/ObjectBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PuzzleNameD/PuzzleNameD.h"

AObjectBase::AObjectBase()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetSimulatePhysics(true);
	Mesh->BodyInstance.bOverrideMass = true;
	Mesh->BodyInstance.SetMassOverride(1000.0f);
	Mesh->SetLinearDamping(1.0f);
	Mesh->SetCollisionObjectType(ECC_WorldDynamic);
	Mesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECC_Interactable, ECR_Block);

	DisappearTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DisappearTimeline"));
}

void AObjectBase::BeginPlay()
{
	Super::BeginPlay();
}

void AObjectBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float AObjectBase::MeasureTotalMass(TArray<IInteractableInterface*>& DetectedObjects)
{
	UWorld* World = GetWorld();
	if (World == nullptr) return 0.f;

	TotalMass = Mass;

	for (int32 RowCount = 0; RowCount < NumberOfLines; ++RowCount)
	{
		for (int32 ColCount = 0; ColCount < NumberOfLines; ++ColCount)
		{
			FHitResult HitResult;
			float UnitLength = Width / (NumberOfLines - 1);
			FVector XAxisVector = FVector::ForwardVector * UnitLength * (ColCount - (NumberOfLines - 1) * 0.5f);
			FVector YAxisVector = FVector::RightVector * UnitLength * (RowCount - (NumberOfLines - 1) * 0.5f);
			FVector Start = GetActorLocation() + XAxisVector + YAxisVector;
			FVector End = Start + FVector::UpVector * TraceLength;
			
			TArray<AActor*> IgnoredActors;
			UKismetSystemLibrary::LineTraceSingle(this, Start, End, UEngineTypes::ConvertToTraceType(ECC_Interactable), false, IgnoredActors, EDrawDebugTrace::ForOneFrame, HitResult, true);

			// FCollisionQueryParams QueryParams;
			// QueryParams.AddIgnoredActor(this);
			//World->LineTraceSingleByChannel(HitResult, Start, End, ECC_Interactable, QueryParams);

			IInteractableInterface* Object = Cast<IInteractableInterface>(HitResult.GetActor());
			if (Object && !DetectedObjects.Contains(Object))
			{
				DetectedObjects.AddUnique(Object);
				TotalMass += Object->MeasureTotalMass(DetectedObjects);
			}
		}
	}
	
	return TotalMass;
}

void AObjectBase::SetCollisionResponse(ECollisionResponse NewResponse)
{
	Mesh->SetCollisionResponseToAllChannels(NewResponse);
	// Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, NewResponse);
	// Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, NewResponse);
	// Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, NewResponse);
}

void AObjectBase::OnPreview()
{
	if (Mesh)
	{
		for (int32 Idx = 0; Idx < PreviewMaterials.Num(); Idx++)
		{
			Mesh->SetMaterial(Idx, PreviewMaterials[Idx]);
		}
	}
}

void AObjectBase::OnPlace()
{
	if (Mesh)
	{
		for (int32 Idx = 0; Idx < OriginalMaterials.Num(); Idx++)
		{
			Mesh->SetMaterial(Idx, OriginalMaterials[Idx]);
		}
	}
}

void AObjectBase::OnDisappear()
{
	if (GetOwner() && Mesh)
	{
		SetActorEnableCollision(false);
		Mesh->SetEnableGravity(false);
		StartDisappear();
	}
}

void AObjectBase::ClearAll(float DisappearValue)
{
	FVector NewLocation = FMath::Lerp(GetActorLocation(), GetOwner()->GetActorLocation(), DisappearValue);
	FRotator NewRotation = FMath::Lerp(GetActorRotation(), GetOwner()->GetActorRotation(), DisappearValue);
	SetActorLocationAndRotation(NewLocation, NewRotation);
}

void AObjectBase::StartDisappear()
{
	DisappearTrack.BindDynamic(this, &AObjectBase::ClearAll);
	DisappearTimelineFinished.BindDynamic(this, &AObjectBase::DisappearFinished);
	if (DisappearCurve && DisappearTimeline)
	{
		DisappearTimeline->AddInterpFloat(DisappearCurve, DisappearTrack);
		DisappearTimeline->PlayFromStart();
		DisappearTimeline->SetTimelineFinishedFunc(DisappearTimelineFinished);
	}
}

void AObjectBase::DisappearFinished()
{
	Destroy();
}
