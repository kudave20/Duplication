// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectBase.h"

AObjectBase::AObjectBase()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetSimulatePhysics(true);

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

void AObjectBase::OnPreview_Implementation()
{
	IInteractableInterface::OnPreview_Implementation();

	if (Mesh)
	{
		Mesh->SetMaterial(0, PreviewMaterial);
	}
}

void AObjectBase::OnPlace_Implementation()
{
	IInteractableInterface::OnPlace_Implementation();

	if (Mesh)
	{
		Mesh->SetMaterial(0, OriginalMaterial);
	}
}

void AObjectBase::OnDisappear_Implementation()
{
	IInteractableInterface::OnDisappear_Implementation();

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
