// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "MainCharacter.generated.h"

UCLASS()
class PUZZLENAMED_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMainCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = Input)
	class UInputMappingContext* MainContext;

	UPROPERTY(EditAnywhere, Category = Input)
	class UInputAction* MovementAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* GrabAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* DuplicateAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* DeleteAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* ClearAction;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void TryGrab();
	void Grab();
	void Grab(UPrimitiveComponent* Target);
	void Release();
	virtual void TryDuplicate();
	int32 Duplicate();
	virtual void TryDelete();
	int32 Delete();
	virtual void TryClear();
	int32 Clear();

private:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UPhysicsHandleComponent* PhysicsHandle;

	UPROPERTY(EditAnywhere, Category = "Length")
	float ArmLength = 250.f;

	UPROPERTY(EditAnywhere, Category = "Length")
	float DuplicateLength = 150.f;

	float Length;

public:

};
