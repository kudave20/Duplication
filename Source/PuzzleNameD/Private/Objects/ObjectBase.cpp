// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/ObjectBase.h"
#include "PuzzleNameD/PuzzleNameD.h"

AObjectBase::AObjectBase()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetSimulatePhysics(true);
	Mesh->BodyInstance.bOverrideMass = true;
	Mesh->BodyInstance.SetMassOverride(1000.0f);
	Mesh->SetLinearDamping(1.0f);
	Mesh->SetCollisionObjectType(ECC_WorldDynamic);
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECC_Interactable, ECollisionResponse::ECR_Block);
	//Mesh->BodyInstance.bLockXRotation = true;

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

void AObjectBase::SetCollisionResponse(ECollisionResponse NewResponse)
{
	Mesh->SetCollisionResponseToAllChannels(NewResponse);
	/*Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, NewResponse);
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, NewResponse);
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, NewResponse);*/
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
