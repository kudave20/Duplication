// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacles/Seesaw.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"

ASeesaw::ASeesaw()
{
	PrimaryActorTick.bCanEverTick = false;

	Base = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base"));
	SetRootComponent(Base);

	Board = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Board"));
	Board->SetupAttachment(Base);
	Board->SetSimulatePhysics(true);
	Board->BodyInstance.SleepFamily = ESleepFamily::Custom;
	Board->BodyInstance.CustomSleepThresholdMultiplier = 0.1f;
	Board->BodyInstance.bGenerateWakeEvents = true;

	Joint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("Joint"));
	Joint->SetupAttachment(Board);
	Joint->ComponentName1.ComponentName = FName("Base");
	Joint->ComponentName2.ComponentName = FName("Board");
	Joint->SetDisableCollision(true);
	Joint->SetAngularSwing1Limit(ACM_Locked, 0.f);
	Joint->SetAngularSwing2Limit(ACM_Limited, 40.f);
	Joint->SetAngularTwistLimit(ACM_Locked, 0.f);
	Joint->SetAngularDriveMode(EAngularDriveMode::TwistAndSwing);
	Joint->SetAngularOrientationDrive(true, false);
	Joint->SetAngularVelocityDrive(true, false);
	Joint->SetAngularDriveParams(2.f, 1.f, 0.f);
}
