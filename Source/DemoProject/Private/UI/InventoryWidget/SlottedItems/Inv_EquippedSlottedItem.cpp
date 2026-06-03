// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InventoryWidget/SlottedItems/Inv_EquippedSlottedItem.h"

FReply UInv_EquippedSlottedItem::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	OnEquippedSlottedItemClicked.Broadcast(this);
	return FReply::Handled();
}
