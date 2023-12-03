// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PUZZLENAMED_API IInteractableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void OnPreview() = 0;
	virtual void OnPlace() = 0;
	virtual void OnDisappear() = 0;

	virtual float MeasureTotalMass(TArray<IInteractableInterface*>& DetectedObjects) = 0;
	virtual void SetCollisionResponse(ECollisionResponse NewResponse) = 0;
	
	virtual UStaticMeshComponent* GetMesh() = 0;
	virtual float GetMass() = 0;
	virtual bool IsDuplicatable() = 0;
	virtual void BecomeDuplicatable(bool bDuplicatable) = 0;
	virtual bool IsGrabbed() = 0;
	virtual void SetIsGrabbed(bool bGrabbed) = 0;
};
