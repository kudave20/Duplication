// Fill out your copyright notice in the Description page of Project Settings.


#include "MassCharacter.h"
#include "PuzzleNameD/Objects/ObjectBase.h"

void AMassCharacter::TryDuplicate()
{
	if (DuplicatedMass >= AbilityMass) return;

	AObjectBase* FirstObject = nullptr;
	AObjectBase* SecondObject = nullptr;
	float TargetMass = Super::Duplicate(FirstObject, SecondObject);

	DuplicatedMass += TargetMass;
}

void AMassCharacter::TryDelete()
{
	float TargetMass = Super::Delete();

	DuplicatedMass -= TargetMass;
}

void AMassCharacter::TryClear()
{
	float TargetMass = Super::Clear();

	DuplicatedMass -= TargetMass;
}
