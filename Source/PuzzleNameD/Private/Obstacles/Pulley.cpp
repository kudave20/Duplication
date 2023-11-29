// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacles/Pulley.h"
#include "CableComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"

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
	LeftPlate->SetSimulatePhysics(true);
	LeftPlate->BodyInstance.bOverrideMass = true;
	LeftPlate->BodyInstance.SetMassOverride(5000.f);
	LeftPlate->BodyInstance.bLockXTranslation = true;
	LeftPlate->BodyInstance.bLockYTranslation = true;
	LeftPlate->BodyInstance.bLockZTranslation = false;
	LeftPlate->BodyInstance.bLockXRotation = true;
	LeftPlate->BodyInstance.bLockYRotation = true;
	LeftPlate->BodyInstance.bLockZRotation = true;

	RightPlate = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightPlate"));
	RightPlate->SetupAttachment(RootComponent);
	RightPlate->SetSimulatePhysics(true);
	RightPlate->BodyInstance.bOverrideMass = true;
	RightPlate->BodyInstance.SetMassOverride(5000.f);
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

	LeftJoint = CreateDefaultSubobject<USceneComponent>(TEXT("LeftJoint"));
	LeftJoint->SetupAttachment(LeftWheel);

	RightJoint = CreateDefaultSubobject<USceneComponent>(TEXT("RightJoint"));
	RightJoint->SetupAttachment(RightWheel);

	LeftConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("LeftConstraint"));
	LeftConstraint->SetupAttachment(RootComponent);
	LeftConstraint->ComponentName1.ComponentName = FName("LeftJoint");
	LeftConstraint->ComponentName2.ComponentName = FName("LeftPlate");
	LeftConstraint->SetDisableCollision(true);
	LeftConstraint->SetLinearZLimit(LCM_Free, 0.f);
	LeftConstraint->SetAngularSwing1Limit(ACM_Locked, 0.f);
	LeftConstraint->SetAngularSwing2Limit(ACM_Locked, 0.f);
	LeftConstraint->SetAngularTwistLimit(ACM_Locked, 0.f);
	LeftConstraint->SetLinearVelocityTarget(FVector::ZeroVector);

	RightConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("RightConstraint"));
	RightConstraint->SetupAttachment(RootComponent);
	RightConstraint->ComponentName1.ComponentName = FName("RightJoint");
	RightConstraint->ComponentName2.ComponentName = FName("RightPlate");
	RightConstraint->SetDisableCollision(true);
	RightConstraint->SetLinearZLimit(LCM_Free, 0.f);
	RightConstraint->SetAngularSwing1Limit(ACM_Locked, 0.f);
	RightConstraint->SetAngularSwing2Limit(ACM_Locked, 0.f);
	RightConstraint->SetAngularTwistLimit(ACM_Locked, 0.f);
	RightConstraint->SetLinearVelocityTarget(FVector::ZeroVector);

	LeftElevateTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("LeftElevateTimeline"));
	RightElevateTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("RightElevateTimeline"));
}

void APulley::BeginPlay()
{
	Super::BeginPlay();

	if (LeftPlate == nullptr || RightPlate == nullptr || LeftConstraint == nullptr || RightConstraint == nullptr) return;
	
	LeftPlateOriginZ = LeftPlate->GetComponentLocation().Z;
	RightPlateOriginZ = RightPlate->GetComponentLocation().Z;
	
	LeftConstraint->SetLinearPositionDrive(false, false, true);
	LeftConstraint->SetLinearVelocityDrive(false, false, true);
	LeftConstraint->SetLinearDriveParams(3000.f, 50.f, 0.f);
	LeftConstraint->SetLinearPositionTarget(FVector::ZeroVector);
	LeftConstraint->SetLinearVelocityTarget(FVector::ZeroVector);
	RightConstraint->SetLinearPositionDrive(false, false, true);
	RightConstraint->SetLinearVelocityDrive(false, false, true);
	RightConstraint->SetLinearDriveParams(3000.f, 50.f, 0.f);
	RightConstraint->SetLinearPositionTarget(FVector::ZeroVector);
	RightConstraint->SetLinearVelocityTarget(FVector::ZeroVector);
	LeftPlate->WakeRigidBody();
	RightPlate->WakeRigidBody();
}

void APulley::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MovePlate();
}

void APulley::MovePlate()
{
	if (LeftConstraint == nullptr || RightConstraint == nullptr || LeftPlate == nullptr || RightPlate == nullptr) return;

	if (PreviousLeftLinearForceZ > PreviousRightLinearForceZ + 2000.f && !bIsLeftHeavy && LeftPlate->GetComponentLocation().Z > LeftPlateOriginZ + 5.f)
	{
		StartElevateRightPlate();
		LeftPlate->WakeRigidBody();
		RightPlate->WakeRigidBody();
		bIsLeftHeavy = true;
		bIsRightHeavy = false;
		return;
	}
	if (PreviousLeftLinearForceZ + 2000.f < PreviousRightLinearForceZ && !bIsRightHeavy && RightPlate->GetComponentLocation().Z > RightPlateOriginZ - RopeThreshold + 5.f)
	{
		StartElevateLeftPlate();
		LeftPlate->WakeRigidBody();
		RightPlate->WakeRigidBody();
		bIsRightHeavy = true;
		bIsLeftHeavy = false;
		return;
	}
	
	FVector LeftLinearForce;
	FVector LeftAngularForce;
	LeftConstraint->GetConstraintForce(LeftLinearForce, LeftAngularForce);
	
	FVector RightLinearForce;
	FVector RightAngularForce;
	RightConstraint->GetConstraintForce(RightLinearForce, RightAngularForce);

	if (PreviousLeftLinearForceZ != LeftLinearForce.Z && LeftPlate->GetComponentVelocity().Size() == 0.f)
	{
		PreviousLeftLinearForceZ = LeftLinearForce.Z;
	}
	if (PreviousRightLinearForceZ != RightLinearForce.Z && RightPlate->GetComponentVelocity().Size() == 0.f)
	{
		PreviousRightLinearForceZ = RightLinearForce.Z;
	}
}

void APulley::StartElevateLeftPlate()
{
	LeftElevateTrack.BindDynamic(this, &APulley::ElevateLeftPlate);
	if (ElevateCurve && LeftElevateTimeline)
	{
		LeftElevateTimeline->AddInterpFloat(ElevateCurve, LeftElevateTrack);
		LeftElevateTimeline->PlayFromStart();
	}
}

void APulley::StartElevateRightPlate()
{
	RightElevateTrack.BindDynamic(this, &APulley::ElevateRightPlate);
	if (ElevateCurve && RightElevateTimeline)
	{
		RightElevateTimeline->AddInterpFloat(ElevateCurve, RightElevateTrack);
		RightElevateTimeline->PlayFromStart();
	}
}

void APulley::ElevateLeftPlate(float ElevateValue)
{
	if (LeftConstraint == nullptr || RightConstraint == nullptr || LeftWheel == nullptr || RightWheel == nullptr) return;
	
	const float LeftCurrentZ = ElevateValue * RopeThreshold;
	const float RightCurrentZ = ElevateValue * -RopeThreshold;
	LeftConstraint->SetLinearPositionTarget(FVector(0.f, 0.f, LeftCurrentZ));
	RightConstraint->SetLinearPositionTarget(FVector(0.f, 0.f, RightCurrentZ));
	const FRotator WheelDeltaRotation(0.f, 0.f, ElevateValue * WheelRotateSpeed);
	LeftWheel->SetRelativeRotation(WheelDeltaRotation);
	RightWheel->SetRelativeRotation(WheelDeltaRotation);
}

void APulley::ElevateRightPlate(float ElevateValue)
{
	if (LeftConstraint == nullptr || RightConstraint == nullptr || LeftWheel == nullptr || RightWheel == nullptr) return;
	
	const float LeftCurrentZ = (1 - ElevateValue) * RopeThreshold;
	const float RightCurrentZ = (1 - ElevateValue) * -RopeThreshold;
	LeftConstraint->SetLinearPositionTarget(FVector(0.f, 0.f, LeftCurrentZ));
	RightConstraint->SetLinearPositionTarget(FVector(0.f, 0.f, RightCurrentZ));
	const FRotator WheelDeltaRotation(0.f, 0.f, -ElevateValue * WheelRotateSpeed);
	LeftWheel->SetRelativeRotation(WheelDeltaRotation);
	RightWheel->SetRelativeRotation(WheelDeltaRotation);
}
