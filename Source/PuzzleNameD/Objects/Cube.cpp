// Fill out your copyright notice in the Description page of Project Settings.


#include "Cube.h"

void ACube::OnPreview_Implementation()
{
	Super::OnPreview_Implementation();

	Mesh->SetMaterial(0, PreviewMaterial);
}

void ACube::OnPlace_Implementation()
{
	Super::OnPlace_Implementation();

	Mesh->SetMaterial(0, OriginalMaterial);
}
