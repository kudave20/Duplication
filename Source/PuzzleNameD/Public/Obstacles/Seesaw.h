// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Seesaw.generated.h"

class UPhysicsConstraintComponent;

UCLASS()
class PUZZLENAMED_API ASeesaw : public AActor
{
	GENERATED_BODY()
	
public:
	ASeesaw();

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Base;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Board;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UPhysicsConstraintComponent> Joint;
	
public:

};
