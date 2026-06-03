// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InventoryWidget/Message/Inv_PickUpMessage.h"
#include"InventoryManagement/Components/InventoryComponent.h"
#include "InventoryManagement/Utils/Inv_InventoryStatics.h"
#include "UI/InventoryWidget/Message/Inv_InfoMessage.h"

void UInv_PickUpMessage::NativeConstruct()
{
	Super::NativeConstruct();
	UInventoryComponent *InventoryComponent= UInv_InventoryStatics::GetInventoryComponent(GetOwningPlayer());
	
	if (IsValid(InventoryComponent))
	{
		InventoryComponent->NoRoomInInventory.AddDynamic(this,&UInv_PickUpMessage::OnNoRoom);
	}
}

void UInv_PickUpMessage::OnNoRoom()
{
	if (!IsValid(InfoMessage)) return;
	InfoMessage->SetMessage(FText::FromString("No Room In Inventory."));
}
