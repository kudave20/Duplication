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

protected:
	virtual void BeginPlay() override;
	virtual void OnPreview_Implementation() override;
	virtual void OnPlace_Implementation() override;
	virtual void OnDisappear_Implementation() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, Category = "Material")
	UMaterialInstance* PreviewMaterial;

	UPROPERTY(EditAnywhere, Category = "Material")
	UMaterialInstance* OriginalMaterial;

	UPROPERTY(EditAnywhere, Category = "Properties")
	int32 Mass;

	/*
	* Disappear Effect
	*/
	UPROPERTY(VisibleAnywhere)
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

public:
	FORCEINLINE int32 GetMass() const { return Mass; }
};
