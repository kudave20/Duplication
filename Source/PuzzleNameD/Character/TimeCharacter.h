// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MainCharacter.h"
#include "TimeCharacter.generated.h"

/**
 * 
 */
UCLASS()
class PUZZLENAMED_API ATimeCharacter : public AMainCharacter
{
	GENERATED_BODY()

protected:
	virtual void TryDuplicate() override;

private:
	UPROPERTY(EditAnywhere, Category = "Ability")
	float AbilityTime;

	float CurrentTime;

	void DuplicateTimerFinished(AObjectBase* OriginalObject);
};
