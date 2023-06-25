// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectBase.h"

AObjectBase::AObjectBase()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetSimulatePhysics(true);
}

void AObjectBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AObjectBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

