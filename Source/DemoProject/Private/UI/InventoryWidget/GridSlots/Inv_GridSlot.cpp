// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InventoryWidget/GridSlots/Inv_GridSlot.h"
#include"Items/Inv_InventoryItem.h"
#include "Components/Image.h"
#include "UI/InventoryWidget/ItemPopUp/Inv_ItemPopUp.h"

void UInv_GridSlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	GridSlotHovered.Broadcast(TileIndex,InMouseEvent);
}

void UInv_GridSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	GridSlotUnhovered.Broadcast(TileIndex,InMouseEvent);
}

FReply UInv_GridSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	
	GridSlotClicked.Broadcast(TileIndex,InMouseEvent);
	return FReply::Handled();
}

void UInv_GridSlot::SetInventoryItem(UInv_InventoryItem* Item)
{
	InventoryItem=Item;
}

void UInv_GridSlot::SetItemPopUp(UInv_ItemPopUp* PopUp)
{
	ItemPopUp=PopUp;
	ItemPopUp->SetGridIndex(GetIndex());
	ItemPopUp->OnNativeDestruct.AddUObject(this,&ThisClass::OnItemPopUpDestruct);
}

UInv_ItemPopUp* UInv_GridSlot::GetItemPopUp() const
{
	return ItemPopUp.Get();
}



void UInv_GridSlot::SetUnoccupiedTexture()
{
	GridSlotSlate=EGridSlotSlate::Unoccupied;
	Image_GridSlot->SetBrush(Brush_Unoccupied);
}

void UInv_GridSlot::SetOccupiedTexture()
{
	GridSlotSlate=EGridSlotSlate::Occupied;
	Image_GridSlot->SetBrush(Brush_Occupied);
}

void UInv_GridSlot::SetSelectedTexture()
{
	GridSlotSlate=EGridSlotSlate::Selected;
	Image_GridSlot->SetBrush(Brush_Selected);
}

void UInv_GridSlot::SetGrayedOutTexture()
{
	GridSlotSlate=EGridSlotSlate::GrayedOut;
	Image_GridSlot->SetBrush(Brush_GrayedOut);
}

void UInv_GridSlot::OnItemPopUpDestruct(UUserWidget* Menu)
{
	ItemPopUp.Reset();
}
