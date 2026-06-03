// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryManagement/Utils/Inv_InventoryStatics.h"
#include"Items/Components/Inv_ItemComponent.h"
#include "InventoryManagement/Components/InventoryComponent.h"
#include"UI/InventoryWidget/InventoryBase/DemoInventoryBase.h"


UInventoryComponent* UInv_InventoryStatics::GetInventoryComponent(const APlayerController* PlayerController)
{
	if (!IsValid(PlayerController)) return nullptr;
	UInventoryComponent* InventoryComponent=PlayerController->FindComponentByClass<UInventoryComponent>();
	return InventoryComponent;
}

EItemCategory UInv_InventoryStatics::GetItemCategoryFromItemComponent(UInv_ItemComponent* ItemComponent)
{
	if (!IsValid(ItemComponent)) return EItemCategory::None;
	return ItemComponent->GetItemManifest().GetItemCategory();
}

void UInv_InventoryStatics::ItemHovered(APlayerController* PC, UInv_InventoryItem* Item)
{
	UInventoryComponent* InventoryComponent=GetInventoryComponent(PC);
	if (!IsValid(InventoryComponent)) return;
	UDemoInventoryBase*InventoryBase=InventoryComponent->GetInventoryMenu();
	if (!IsValid(InventoryBase))return;
	if (InventoryBase->HasHoverItem())return;
	InventoryBase->OnItemHovered(Item);
}

void UInv_InventoryStatics::ItemUnHovered(APlayerController* PC)
{
	UInventoryComponent* IC = GetInventoryComponent(PC);
	if (!IsValid(IC)) return;
	UDemoInventoryBase*InventoryBase=IC->GetInventoryMenu();
	if (!IsValid(InventoryBase))return;
	InventoryBase->OnItemUnhovered();
}

UInv_HoverItem* UInv_InventoryStatics::GetHoverItem(APlayerController* PC)
{
	UInventoryComponent* IC = GetInventoryComponent(PC);
	if (!IsValid(IC)) return nullptr;
	UDemoInventoryBase*InventoryBase=IC->GetInventoryMenu();
	if (!IsValid(InventoryBase))return nullptr;
	return InventoryBase->GetHoverItem();
}

UDemoInventoryBase* UInv_InventoryStatics::GetInventoryWidget(APlayerController* PC)
{
	UInventoryComponent* IC = GetInventoryComponent(PC);
	if (!IsValid(IC)) return nullptr;
	return IC->GetInventoryMenu();
}





