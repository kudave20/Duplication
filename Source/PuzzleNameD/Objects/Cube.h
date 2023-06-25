// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PuzzleNameD/Interfaces/InteractableInterface.h"
#include "Cube.generated.h"

UCLASS()
class PUZZLENAMED_API ACube : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:
	ACube();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Mesh;

public:

};
