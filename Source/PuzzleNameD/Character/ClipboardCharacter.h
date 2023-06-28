// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MainCharacter.h"
#include "ClipboardCharacter.generated.h"

/**
 * 
 */
UCLASS()
class PUZZLENAMED_API AClipboardCharacter : public AMainCharacter
{
	GENERATED_BODY()
	
public:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* EraseAction;

	virtual void TryDuplicate() override;
	void Erase();

private:
	UPROPERTY(EditAnywhere, Category = "Ability")
	int32 AbilityCount;

	UPROPERTY(VisibleAnywhere)
	int32 DuplicatedCount;

	UPROPERTY()
	AObjectBase* CurrentKeptObject;
};
