// Fill out your copyright notice in the Description page of Project Settings.


#include "ClearField.h"
#include "PuzzleNameD/Character/MainCharacter.h"

AClearField::AClearField()
{
	PrimaryActorTick.bCanEverTick = false;

	Field = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Field"));
	SetRootComponent(Field);
	Field->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Field->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	Field->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	Field->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void AClearField::BeginPlay()
{
	Super::BeginPlay();
	
	if (Field)
	{
		Field->OnComponentBeginOverlap.AddDynamic(this, &AClearField::OnOverlapBegin);
	}
}

void AClearField::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AClearField::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	MainCharacter = MainCharacter == nullptr ? Cast<AMainCharacter>(OtherActor) : MainCharacter;
	if (MainCharacter)
	{
		MainCharacter->ClearAll();
	}
}
