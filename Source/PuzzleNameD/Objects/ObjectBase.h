// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PuzzleNameD/Interfaces/InteractableInterface.h"
#include "Components/TimelineComponent.h"
#include "ObjectBase.generated.h"

UCLASS()
class PUZZLENAMED_API AObjectBase : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	AObjectBase();
	virtual void Tick(float DeltaTime) override;
	virtual void SetCollisionResponse(ECollisionResponse NewResponse);

protected:
	virtual void BeginPlay() override;
	virtual void OnPreview_Implementation() override;
	virtual void OnPlace_Implementation() override;
	virtual void OnDisappear_Implementation() override;

	UPROPERTY(EditAnywhere, Category = "Material")
	TArray<UMaterialInstance*> PreviewMaterials;

	UPROPERTY(EditAnywhere, Category = "Material")
	TArray<UMaterialInstance*> OriginalMaterials;

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UBoxComponent* AreaBox;

	UPROPERTY(EditAnywhere, Category = "Properties")
	float Mass;

	/*
	* Disappear Effect
	*/
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UTimelineComponent* DisappearTimeline;
	FOnTimelineFloat DisappearTrack;
	FOnTimelineEvent DisappearTimelineFinished;

	UPROPERTY(EditAnywhere, Category = "Curves")
	UCurveFloat* DisappearCurve;

	UFUNCTION()
	void ClearAll(float DisappearValue);
	void StartDisappear();

	UFUNCTION()
	void DisappearFinished();

	UPROPERTY()
	TArray<AObjectBase*> DuplicatedObjects;

	bool bIsDuplicatable = true;

	FTimerHandle DuplicateTimer;

public:
	FORCEINLINE UStaticMeshComponent* GetMesh() const { return Mesh; }
	FORCEINLINE float GetMass() const { return Mass; }
	FORCEINLINE void SetMass(float InMass) { Mass = InMass; }
	FORCEINLINE bool IsDuplicatable() const { return bIsDuplicatable; }
	FORCEINLINE void BecomeDuplicatable(bool bDuplicatable) { bIsDuplicatable = bDuplicatable; }
	FORCEINLINE FTimerHandle& GetDuplicateTimer() { return DuplicateTimer; }
};
