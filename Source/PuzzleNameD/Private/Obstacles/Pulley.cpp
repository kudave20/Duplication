// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacles/Pulley.h"
#include "CableComponent.h"
#include "Interfaces/InteractableInterface.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PuzzleNameD/PuzzleNameD.h"

APulley::APulley()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	LeftWheel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftWheel"));
	LeftWheel->SetupAttachment(RootComponent);
	LeftWheel->SetEnableGravity(false);

	RightWheel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightWheel"));
	RightWheel->SetupAttachment(RootComponent);
	RightWheel->SetEnableGravity(false);

	LeftPlate = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftPlate"));
	LeftPlate->SetupAttachment(RootComponent);
	LeftPlate->BodyInstance.bLockXTranslation = true;
	LeftPlate->BodyInstance.bLockYTranslation = true;
	LeftPlate->BodyInstance.bLockZTranslation = false;
	LeftPlate->BodyInstance.bLockXRotation = true;
	LeftPlate->BodyInstance.bLockYRotation = true;
	LeftPlate->BodyInstance.bLockZRotation = true;

	RightPlate = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightPlate"));
	RightPlate->SetupAttachment(RootComponent);
	RightPlate->BodyInstance.bLockXTranslation = true;
	RightPlate->BodyInstance.bLockYTranslation = true;
	RightPlate->BodyInstance.bLockZTranslation = false;
	RightPlate->BodyInstance.bLockXRotation = true;
	RightPlate->BodyInstance.bLockYRotation = true;
	RightPlate->BodyInstance.bLockZRotation = true;

	LeftCable = CreateDefaultSubobject<UCableComponent>(TEXT("LeftCable"));
	LeftCable->SetupAttachment(RootComponent);
	LeftCable->AttachEndTo.ComponentProperty = FName("LeftPlate");
	LeftCable->CableLength = 1.f;
	LeftCable->EndLocation = FVector::ZeroVector;

	RightCable = CreateDefaultSubobject<UCableComponent>(TEXT("RightCable"));
	RightCable->SetupAttachment(RootComponent);
	RightCable->AttachEndTo.ComponentProperty = FName("RightPlate");
	RightCable->CableLength = 1.f;
	RightCable->EndLocation = FVector::ZeroVector;

	CenterCable = CreateDefaultSubobject<UCableComponent>(TEXT("CenterCable"));
	CenterCable->SetupAttachment(RootComponent);
	CenterCable->AttachEndTo.ComponentProperty = FName("RightWheel");
	CenterCable->CableLength = 1.f;
	CenterCable->EndLocation = FVector::UpVector * 30.f;

	ElevateTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("ElevateTimeline"));
}

void APulley::BeginPlay()
{
	Super::BeginPlay();

	if (LeftPlate)
	{
		LeftPlateOriginLocation = LeftPlate->GetComponentLocation();
		LeftPlateHighestLocation = LeftPlateOriginLocation + FVector::UpVector * RopeThreshold * 0.5f;
		LeftPlateLowestLocation = LeftPlateOriginLocation - FVector::UpVector * RopeThreshold * 0.5f;
	}
	if (RightPlate)
	{
		RightPlateOriginLocation = RightPlate->GetComponentLocation();
		RightPlateHighestLocation = RightPlateOriginLocation + FVector::UpVector * RopeThreshold * 0.5f;
		RightPlateLowestLocation = RightPlateOriginLocation - FVector::UpVector * RopeThreshold * 0.5f;
	}
}

void APulley::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	CalculateMass();
	MovePlate();
}

void APulley::CalculateMass()
{
	if (LeftPlate == nullptr || RightPlate == nullptr) return;

	UWorld* World = GetWorld();
	if (World == nullptr) return;

	LeftTotalMass = 0.f;
	RightTotalMass = 0.f;

	TArray<IInteractableInterface*> LeftDetectedObjects;
	TArray<IInteractableInterface*> RightDetectedObjects;

	for (int32 RowCount = 0; RowCount < NumberOfLines; ++RowCount)
	{
		for (int32 ColCount = 0; ColCount < NumberOfLines; ++ColCount)
		{
			FHitResult LeftHitResult;
			float UnitLength = Width / (NumberOfLines - 1);
			FVector XAxisVector = FVector::ForwardVector * UnitLength * (ColCount - (NumberOfLines - 1) * 0.5f);
			FVector YAxisVector = FVector::RightVector * UnitLength * (RowCount - (NumberOfLines - 1) * 0.5f);
			FVector Start = LeftPlate->GetComponentLocation() + XAxisVector + YAxisVector;
			FVector End = Start + FVector::UpVector * TraceLength;
			
			TArray<AActor*> IgnoredActors;
			UKismetSystemLibrary::LineTraceSingle(this, Start, End, UEngineTypes::ConvertToTraceType(ECC_Interactable), false, IgnoredActors, EDrawDebugTrace::ForOneFrame, LeftHitResult, true);

			//World->LineTraceSingleByChannel(LeftHitResult, Start, End, ECC_Interactable);
			
			IInteractableInterface* Object = Cast<IInteractableInterface>(LeftHitResult.GetActor());
			if (Object && !Object->IsGrabbed() && !LeftDetectedObjects.Contains(Object))
			{
				LeftDetectedObjects.AddUnique(Object);
				LeftTotalMass += Object->MeasureTotalMass(LeftDetectedObjects);
			}
		}
	}

	for (int32 RowCount = 0; RowCount < NumberOfLines; ++RowCount)
	{
		for (int32 ColCount = 0; ColCount < NumberOfLines; ++ColCount)
		{
			FHitResult RightHitResult;
			float UnitLength = Width / (NumberOfLines - 1);
			FVector XAxisVector = FVector::ForwardVector * UnitLength * (ColCount - (NumberOfLines - 1) * 0.5f);
			FVector YAxisVector = FVector::RightVector * UnitLength * (RowCount - (NumberOfLines - 1) * 0.5f);
			FVector Start = RightPlate->GetComponentLocation() + XAxisVector + YAxisVector;
			FVector End = Start + FVector::UpVector * TraceLength;

			TArray<AActor*> IgnoredActors;
			UKismetSystemLibrary::LineTraceSingle(this, Start, End, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility), false, IgnoredActors, EDrawDebugTrace::ForOneFrame, RightHitResult, true);

			//World->LineTraceSingleByChannel(RightHitResult, Start, End, ECC_Interactable);
			
			IInteractableInterface* Object = Cast<IInteractableInterface>(RightHitResult.GetActor());
			if (Object && !Object->IsGrabbed() && !RightDetectedObjects.Contains(Object))
			{
				RightTotalMass += Object->MeasureTotalMass(RightDetectedObjects);
				RightDetectedObjects.AddUnique(Object);
			}
		}
	}
}

void APulley::MovePlate()
{
	if (LeftPlate == nullptr || RightPlate == nullptr) return;
	
	if (LeftTotalMass > RightTotalMass && !bIsLeftHeavy && LeftPlate->GetComponentLocation().Z > LeftPlateLowestLocation.Z)
	{
		LeftPlateLocation = LeftPlate->GetComponentLocation();
		RightPlateLocation = RightPlate->GetComponentLocation();
		StartLiftRight();
		bIsLeftHeavy = true;
		bIsRightHeavy = false;
	}
	if (LeftTotalMass < RightTotalMass && !bIsRightHeavy && RightPlate->GetComponentLocation().Z > RightPlateLowestLocation.Z)
	{
		LeftPlateLocation = LeftPlate->GetComponentLocation();
		RightPlateLocation = RightPlate->GetComponentLocation();
		StartLiftLeft();
		bIsLeftHeavy = false;
		bIsRightHeavy = true;
	}
}

void APulley::StartLiftLeft()
{
	ElevateTrack.BindDynamic(this, &APulley::Elevate);
	if (ElevateCurve && ElevateTimeline)
	{
		ElevateTimeline->AddInterpFloat(ElevateCurve, ElevateTrack);
		const float TimelineLength = ElevateTimeline->GetTimelineLength();
		const float Time = FMath::Clamp((LeftPlateLocation.Z - LeftPlateLowestLocation.Z) / RopeThreshold * TimelineLength, 0.f, TimelineLength);
		ElevateTimeline->SetPlaybackPosition(Time, false);
		ElevateTimeline->Play();
	}
}

void APulley::StartLiftRight()
{
	ElevateTrack.BindDynamic(this, &APulley::Elevate);
	if (ElevateCurve && ElevateTimeline)
	{
		ElevateTimeline->AddInterpFloat(ElevateCurve, ElevateTrack);
		const float TimelineLength = ElevateTimeline->GetTimelineLength();
		const float Time = FMath::Clamp((RightPlateHighestLocation.Z - RightPlateLocation.Z) / RopeThreshold * TimelineLength, 0.f, TimelineLength);
		ElevateTimeline->SetPlaybackPosition(Time, false);
		ElevateTimeline->Reverse();
	}
}

void APulley::Elevate(float ElevateValue)
{
	if (LeftPlate == nullptr || RightPlate == nullptr) return;
	
	FVector LeftLocation = FMath::Lerp(LeftPlateLowestLocation, LeftPlateHighestLocation, ElevateValue);
	FVector RightLocation = FMath::Lerp(RightPlateHighestLocation, RightPlateLowestLocation, ElevateValue);
	LeftPlate->SetWorldLocation(LeftLocation);
	RightPlate->SetWorldLocation(RightLocation);
}