// Fill out your copyright notice in the Description page of Project Settings.


#include "TimeCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "PuzzleNameD/Objects/ObjectBase.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"

void ATimeCharacter::TryDuplicate()
{
	AObjectBase* FirstObject = nullptr;
	AObjectBase* SecondObject = nullptr;
	float TargetMass = Super::Duplicate(FirstObject, SecondObject);
	if (FirstObject == nullptr) return;

	AActor* OriginalActor = FirstObject->GetOwner() ? FirstObject->GetOwner() : FirstObject;
	AObjectBase* OriginalObject = Cast<AObjectBase>(OriginalActor);
	if (OriginalObject && !OriginalObject->GetDuplicateTimer().IsValid())
	{
		FTimerDelegate DuplicateDelegate = FTimerDelegate::CreateUObject(this, &ATimeCharacter::DuplicateTimerFinished, OriginalObject);
		GetWorldTimerManager().SetTimer(
			OriginalObject->GetDuplicateTimer(),
			DuplicateDelegate,
			AbilityTime,
			false
		);
	}
}

void ATimeCharacter::DuplicateTimerFinished(AObjectBase* OriginalObject)
{
	if (OriginalObject == nullptr || PhysicsHandle == nullptr) return;

	UPrimitiveComponent* Component = PhysicsHandle->GrabbedComponent;
	if (Component && Component->GetOwner() && Component->GetOwner()->IsA(OriginalObject->GetClass()))
	{
		PhysicsHandle->ReleaseComponent();
		Component->GetOwner()->Destroy();
	}

	TArray<AActor*> TargetActors;
	UGameplayStatics::GetAllActorsOfClass(this, OriginalObject->GetClass(), TargetActors);
	for (AActor* TargetActor : TargetActors)
	{
		if (TargetActor && TargetActor->GetOwner())
		{
			AObjectBase* TargetObject = Cast<AObjectBase>(TargetActor);
			if (TargetObject)
			{
				IInteractableInterface::Execute_OnDisappear(TargetObject);
			}
		}
	}
	OriginalObject->GetDuplicateTimer().Invalidate();
}
