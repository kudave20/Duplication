// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "MainCharacter.generated.h"

class UCameraComponent;
class UPostProcessComponent;
class UPhysicsHandleComponent;
class UInputMappingContext;
class UInputAction;
class IInteractableInterface;

UCLASS()
class PUZZLENAMED_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMainCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void ClearAll();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UPostProcessComponent> PostProcess;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UPhysicsHandleComponent> PhysicsHandle;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputMappingContext> MainContext;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> MovementAction;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> GrabAction;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> DuplicateAction;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> DeleteAction;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> ClearAction;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> ExamineAction;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> SnapLeftAction;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> SnapRightAction;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> SnapUpAction;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> SnapDownAction;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void TryGrab();
	void Grab();
	void Grab(UPrimitiveComponent* Target);
	void Release();
	virtual void TryDuplicate();
	void Duplicate();
	virtual void TryDelete();
	void Delete();
	virtual void TryClear();
	void Clear();
	void Examine();
	void SnapLeft();
	void SnapRight();
	void SnapUp();
	void SnapDown();

	UPROPERTY(EditAnywhere, Category = "Length")
	float ArmLength = 250.f;

	UPROPERTY(EditAnywhere, Category = "Length")
	float DuplicateLength = 150.f; 
	
	float Length;

	UPROPERTY()
	TArray<IInteractableInterface*> InteractableObjects;

private:
	float YawWhenGrabbed;
	float RollWhenSnapped;

public:

};
