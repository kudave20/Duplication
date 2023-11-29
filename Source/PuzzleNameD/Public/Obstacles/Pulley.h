// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "Pulley.generated.h"

class UCableComponent;
class UPhysicsConstraintComponent;
class AMainCharacter;

UCLASS()
class PUZZLENAMED_API APulley : public AActor
{
	GENERATED_BODY()
	
public:
	APulley();
	virtual void Tick(float DeltaTime) override;
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* Root;
	
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* LeftWheel;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* RightWheel;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* LeftPlate;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* RightPlate;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UCableComponent* LeftCable;
	
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UCableComponent* RightCable;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UCableComponent* CenterCable;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* LeftJoint;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* RightJoint;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UPhysicsConstraintComponent* LeftConstraint;
	
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UPhysicsConstraintComponent* RightConstraint;

	void MovePlate();
	
	bool bIsLeftHeavy = false;
	bool bIsRightHeavy = false;
	
	UPROPERTY()
	UTimelineComponent* LeftElevateTimeline;
	FOnTimelineFloat LeftElevateTrack;

	UPROPERTY()
	UTimelineComponent* RightElevateTimeline;
	FOnTimelineFloat RightElevateTrack;
	
	UPROPERTY(EditAnywhere, Category = "Properties")
	UCurveFloat* ElevateCurve;
	
	void StartElevateLeftPlate();
	void StartElevateRightPlate();
	
	UFUNCTION()
	void ElevateLeftPlate(float ElevateValue);
	
	UFUNCTION()
	void ElevateRightPlate(float ElevateValue);
	
	float LeftPlateOriginZ;
	float RightPlateOriginZ;

	float PreviousLeftLinearForceZ = 0.f;
	float PreviousRightLinearForceZ = 0.f;

	UPROPERTY(EditAnywhere, Category = "Properties")
	float RopeThreshold = 200.f;

	UPROPERTY(EditAnywhere, Category = "Properties")
	float WheelRotateSpeed = 1080.f;
	
public:

};
