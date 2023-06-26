// Fill out your copyright notice in the Description page of Project Settings.


#include "MassCharacter.h"

void AMassCharacter::TryDuplicate()
{
	if (DuplicatedMass >= AbilityMass) return;

	int32 TargetMass = Super::Duplicate();

	DuplicatedMass += TargetMass;
}

void AMassCharacter::TryDelete()
{
	int32 TargetMass = Super::Delete();

	DuplicatedMass -= TargetMass;
}

void AMassCharacter::TryClear()
{
	int32 TargetMass = Super::Clear();

	DuplicatedMass -= TargetMass;
}
