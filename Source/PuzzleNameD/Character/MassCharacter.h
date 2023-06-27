// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MainCharacter.h"
#include "MassCharacter.generated.h"

/**
 * 
 */
UCLASS()
class PUZZLENAMED_API AMassCharacter : public AMainCharacter
{
	GENERATED_BODY()
	
protected:
	virtual void TryDuplicate() override;
	virtual void TryDelete() override;
	virtual void TryClear() override;

private:
	UPROPERTY(EditAnywhere, Category = "Ability")
	float AbilityMass;

	float DuplicatedMass;
};
