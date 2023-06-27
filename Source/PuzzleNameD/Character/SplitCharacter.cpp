// Fill out your copyright notice in the Description page of Project Settings.


#include "SplitCharacter.h"
#include "PuzzleNameD/Objects/ObjectBase.h"

void ASplitCharacter::TryDuplicate()
{
	AObjectBase* OriginalObject = nullptr;
	AObjectBase* DuplicatedObject = nullptr;
	float TargetMass = Super::Duplicate(OriginalObject, DuplicatedObject);
	float SplitMass = TargetMass / 2;
	if (OriginalObject && DuplicatedObject)
	{
		OriginalObject->SetMass(SplitMass);
		DuplicatedObject->SetMass(SplitMass);
	}
}

void ASplitCharacter::TryDelete()
{
	int32 TargetMass = Super::Delete();


}

void ASplitCharacter::TryClear()
{
	int32 TargetMass = Super::Clear();


}
