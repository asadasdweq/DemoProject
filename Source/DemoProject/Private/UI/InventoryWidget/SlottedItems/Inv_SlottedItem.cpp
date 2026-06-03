// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InventoryWidget/SlottedItems/Inv_SlottedItem.h"
#include "Items/Inv_InventoryItem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "InventoryManagement/Utils/Inv_InventoryStatics.h"


FReply UInv_SlottedItem::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	OnSlottedItemClicked.Broadcast(GridIndex, InMouseEvent);
	return FReply::Handled();
}

void UInv_SlottedItem::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	UInv_InventoryStatics::ItemHovered(GetOwningPlayer(),InventoryItem.Get());
}

void UInv_SlottedItem::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	UInv_InventoryStatics::ItemUnHovered(GetOwningPlayer());
}

void UInv_SlottedItem::SetImageBrush(const FSlateBrush& Brush) const
{
	Image_Icon->SetBrush(Brush);
}

void UInv_SlottedItem::UpdateStackCount(int32 StackCount)
{
	if (bIsStackable)
	{
		Text_StackCount->SetVisibility(ESlateVisibility::Visible);
		Text_StackCount->SetText(FText::AsNumber(StackCount));
	}
	else
	{
		Text_StackCount->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UInv_SlottedItem::SetInventoryItem(UInv_InventoryItem * Item)
{
	InventoryItem=Item;
}
