// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/Inv_HighLightableStaticMesh.h"

void UInv_HighLightableStaticMesh::HighLight_Implementation()
{
	SetOverlayMaterial(HighLightMaterial);
}

void UInv_HighLightableStaticMesh::UnHighLight_Implementation()
{
	SetOverlayMaterial(nullptr);
}
