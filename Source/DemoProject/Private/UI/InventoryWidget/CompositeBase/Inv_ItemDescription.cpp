// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InventoryWidget/CompositeBase/Inv_ItemDescription.h"

#include "Components/SizeBox.h"

FVector2D UInv_ItemDescription::GetBoxSize() const
{
	return SizeBox->GetDesiredSize();
}
