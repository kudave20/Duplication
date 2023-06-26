// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectBase.h"
#include "GeometryCollection/GeometryCollectionComponent.h"

AObjectBase::AObjectBase()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetSimulatePhysics(true);

	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection"));
	GeometryCollection->SetupAttachment(Mesh);
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

	Destroy();
}

