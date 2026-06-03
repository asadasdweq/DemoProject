// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InventoryWidget/CompositeBase/Inv_Leaf.h"

void UInv_Leaf::ApplyFunction(FuncType Function)
{
	Function(this);
}
