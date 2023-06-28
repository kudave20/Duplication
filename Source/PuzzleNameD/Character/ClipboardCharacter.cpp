// Fill out your copyright notice in the Description page of Project Settings.


#include "ClipboardCharacter.h"
#include "EnhancedInputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PuzzleNameD/Objects/ObjectBase.h"

void AClipboardCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(EraseAction, ETriggerEvent::Triggered, this, &AClipboardCharacter::Erase);
	}
}

void AClipboardCharacter::TryDuplicate()
{
	if (DuplicatedCount >= AbilityCount) return;

	AObjectBase* FirstObject = nullptr;
	AObjectBase* SecondObject = nullptr;
	float TargetMass = Super::Duplicate(FirstObject, SecondObject);
	if (TargetMass <= 0.0f) return;

	DuplicatedCount++;
	CurrentKeptObject = FirstObject;
	if (SecondObject)
	{
		SecondObject->BecomeDuplicatable(false);
	}
}

void AClipboardCharacter::Erase()
{
	if (CurrentKeptObject == nullptr) return;

	TArray<AActor*> TargetActors;
	UGameplayStatics::GetAllActorsOfClass(this, CurrentKeptObject->GetClass(), TargetActors);
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
	if (!bIsExamining)
	{
		DuplicatedCount = 0;
	}
}
