// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "Pulley.generated.h"

class UCableComponent;
class UPhysicsConstraintComponent;
class AMainCharacter;
class IInteractableInterface;

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

	void CalculateMass();
	void MovePlate(float DeltaTime);

	bool bIsLeftHeavy;
	bool bIsRightHeavy;

	float LeftTotalMass = 0.f;
	float RightTotalMass = 0.f;

	FVector LeftPlateOriginLocation;
	FVector LeftPlateHighestLocation;
	FVector LeftPlateLowestLocation;

	FVector RightPlateOriginLocation;
	FVector RightPlateHighestLocation;
	FVector RightPlateLowestLocation;

	FVector LeftPlateLocation;
	FVector RightPlateLocation;
	
	UPROPERTY(EditAnywhere, Category = "Properties")
	float Width = 30.f;

	UPROPERTY(EditAnywhere, Category = "Properties")
	float TraceLength = 1.f;

	UPROPERTY(EditAnywhere, Category = "Properties")
	int32 NumberOfLines = 5;

	UPROPERTY(EditAnywhere, Category = "Properties")
	float RopeThreshold = 200.f;
	
	UPROPERTY(EditAnywhere, Category = "Properties")
	float WheelRotateSpeed = 1080.f;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UTimelineComponent* ElevateTimeline;
	FOnTimelineFloat ElevateTrack;

	UPROPERTY(EditAnywhere, Category = "Properties")
	UCurveFloat* ElevateCurve;

	void StartLiftLeft();
	void StartLiftRight();
	
	UFUNCTION()
	void Elevate(float ElevateValue);
	
public:

};
