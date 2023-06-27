// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MainCharacter.h"
#include "SplitCharacter.generated.h"

/**
 * 
 */
UCLASS()
class PUZZLENAMED_API ASplitCharacter : public AMainCharacter
{
	GENERATED_BODY()
	
protected:
	virtual void TryDuplicate() override;
	virtual void TryDelete() override;
	virtual void TryClear() override;
};
