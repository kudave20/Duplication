// Fill out your copyright notice in the Description page of Project Settings.


#include "ClipboardCharacter.h"
#include "EnhancedInputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PuzzleNameD/Objects/ObjectBase.h"
#include "Camera/CameraComponent.h"
#include "Components/PostProcessComponent.h"

void AClipboardCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(PasteAction, ETriggerEvent::Triggered, this, &AClipboardCharacter::Paste);
		EnhancedInputComponent->BindAction(EraseAction, ETriggerEvent::Triggered, this, &AClipboardCharacter::Erase);
	}
}

void AClipboardCharacter::TryDuplicate()
{
	if (ClipboardCount > 0 || PostProcess->bEnabled) return;

	FHitResult HitResult;
	FVector Start = Camera->GetComponentLocation();
	FVector End = Start + Camera->GetComponentRotation().Vector() * ArmLength;
	GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECollisionChannel::ECC_Visibility
	);

	AObjectBase* TargetObject = Cast<AObjectBase>(HitResult.GetActor());
	if (TargetObject && TargetObject->IsDuplicatable())
	{
		CurrentKeptObject = TargetObject;
		ClipboardCount = AbilityCount;
	}
}

void AClipboardCharacter::Paste()
{
	if (CurrentKeptObject == nullptr || ClipboardCount == 0 || PostProcess->bEnabled) return;
	
	Length = DuplicateLength;
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = CurrentKeptObject;
	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(
		CurrentKeptObject->GetClass(), 
		Camera->GetComponentLocation() + Camera->GetComponentRotation().Vector() * DuplicateLength, 
		CurrentKeptObject->GetActorRotation(), 
		SpawnParams);
	if (SpawnedActor)
	{
		SpawnedActor->SetActorEnableCollision(false);
		IInteractableInterface::Execute_OnPreview(SpawnedActor);
		UPrimitiveComponent* TargetComponent = Cast<UPrimitiveComponent>(SpawnedActor->GetRootComponent());
		Grab(TargetComponent);
		AObjectBase* SpawnedObject = Cast<AObjectBase>(SpawnedActor);
		if (SpawnedObject)
		{
			InteractableObjects.Emplace(SpawnedObject);
			SpawnedObject->BecomeDuplicatable(false);
			ClipboardCount--;
		}
	}
}

void AClipboardCharacter::Erase()
{
	if (CurrentKeptObject == nullptr || PostProcess->bEnabled) return;

	/*TArray<AActor*> TargetActors;
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
	}*/
	ClipboardCount = 0;
}
