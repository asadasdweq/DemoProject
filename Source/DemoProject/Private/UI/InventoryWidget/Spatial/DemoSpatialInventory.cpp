// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InventoryWidget/Spatial/DemoSpatialInventory.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"
#include"UI/InventoryWidget/Spatial/DemoInventoryGrid.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/WidgetSwitcher.h"
#include "InventoryManagement/Utils/Inv_InventoryStatics.h"
#include "Items/Inv_InventoryItem.h"
#include "UI/InventoryWidget/CompositeBase/Inv_ItemDescription.h"
#include "UI/InventoryWidget/GridSlots/Inv_EquippedGridSlot.h"
#include "UI/InventoryWidget/HoverItem/Inv_HoverItem.h"
#include "UI/InventoryWidget/SlottedItems/Inv_EquippedSlottedItem.h"
#include"InventoryManagement/Components/InventoryComponent.h"

void UDemoSpatialInventory::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	Button_Equippables->OnClicked.AddDynamic(this,&ThisClass::UDemoSpatialInventory::ShowEquippables);
	Button_Consumables->OnClicked.AddDynamic(this,&ThisClass::UDemoSpatialInventory::ShowConsumables);
	Button_Craftables->OnClicked.AddDynamic(this,&ThisClass::UDemoSpatialInventory::ShowCraftables);
	
	Grid_Equippables->SetOwningCanvas(CanvasPanel);
	Grid_Consumables->SetOwningCanvas(CanvasPanel);
	Grid_Craftables->SetOwningCanvas(CanvasPanel);
	ShowEquippables();
	GetOwningPlayer()->SetMouseCursorWidget(EMouseCursor::Default,nullptr);
	
	WidgetTree->ForEachWidget([this](UWidget* Widget)
	{
		UInv_EquippedGridSlot*EquippedGridSlot=Cast<UInv_EquippedGridSlot>(Widget);
		if (IsValid(EquippedGridSlot))
		{
			EquippedGridSlots.Add(EquippedGridSlot);
			EquippedGridSlot->EquippedGridSlotClicked.AddDynamic(this,&ThisClass::EquippedGridSlotClicked);
		}
	});
}
void UDemoSpatialInventory::EquippedGridSlotClicked(UInv_EquippedGridSlot* EquippedGridSlot,
	const FGameplayTag& EquipmentTypeTag)
{
	if (!CanEquipHoverItem(EquippedGridSlot,EquipmentTypeTag)) return;
	UInv_HoverItem*HoverItem=GetHoverItem();
	const float TileSize=UInv_InventoryStatics::GetInventoryWidget(GetOwningPlayer())->GetTileSize();
	
	UInv_EquippedSlottedItem*EquippedSlottedItem=EquippedGridSlot->OnItemEquipped(
	HoverItem->GetInventoryItem(),
	EquipmentTypeTag,
	TileSize);
	EquippedSlottedItem->OnEquippedSlottedItemClicked.AddDynamic(this,&ThisClass::EquippedSlottedItemClicked);
	
	UInventoryComponent* InventoryComponent=UInv_InventoryStatics::GetInventoryComponent(GetOwningPlayer());
	check(IsValid(InventoryComponent));
	InventoryComponent->Server_EquipSlotClicked(HoverItem->GetInventoryItem(),nullptr);
	
	if (GetOwningPlayer()->GetNetMode()!=NM_DedicatedServer)
	{
		InventoryComponent->OnItemEquipped.Broadcast(HoverItem->GetInventoryItem());
	}
	Grid_Equippables->ClearHoverItem();
}

void UDemoSpatialInventory::EquippedSlottedItemClicked(UInv_EquippedSlottedItem* EquippedSlottedItem)
{
	UInv_InventoryStatics::ItemUnHovered(GetOwningPlayer());//Remove the item description
	
	if (IsValid(GetHoverItem())&&GetHoverItem()->IsStackable())return;
	UInv_InventoryItem*ItemToEquip=IsValid(GetHoverItem())?GetHoverItem()->GetInventoryItem():nullptr;
	UInv_InventoryItem*ItemToUnequip=EquippedSlottedItem->GetInventoryItem();
	UInv_EquippedGridSlot*EquippedGridSlot=FindSlotWithEquippedItem(ItemToUnequip);
	
	ClearSlotOfItem(EquippedGridSlot);
	Grid_Equippables->AssignHoverItem(ItemToUnequip);
	RemoveEquippedSlottedItem(EquippedSlottedItem);
	MakeEquippedSlottedItem(EquippedSlottedItem,EquippedGridSlot,ItemToEquip);
	
	BroadcastSlotClickedDelegates(ItemToEquip,ItemToUnequip);
}
FReply UDemoSpatialInventory::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	ActiveGrid->DropItem();
	return FReply::Handled();
}

void UDemoSpatialInventory::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (!IsValid(ItemDescription))return;
	SetItemDescriptionSizeAndPosition(ItemDescription,CanvasPanel);
}
UInv_ItemDescription* UDemoSpatialInventory::GetItemDescription()
{
	if (!IsValid(ItemDescription))
	{
		ItemDescription=CreateWidget<UInv_ItemDescription>(GetOwningPlayer(),ItemDescriptionClass);
		CanvasPanel->AddChild(ItemDescription);
	}
	return ItemDescription;
}

void UDemoSpatialInventory::SetItemDescriptionSizeAndPosition(UInv_ItemDescription* Description,
	UCanvasPanel* Canvas) const
{
	UCanvasPanelSlot *ItemDescriptionCPS=UWidgetLayoutLibrary::SlotAsCanvasSlot(Description);
	if (!IsValid(ItemDescriptionCPS)) return;
	const FVector2D ItemDescriptionSize=Description->GetBoxSize();
	ItemDescriptionCPS->SetSize(ItemDescriptionSize);
	
	FVector2D ClampedPosition=UInv_WidgetUtils::GetClampedWidgetPosition(
		UInv_WidgetUtils::GetWidgetSize(CanvasPanel),
		ItemDescriptionSize,
		UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer()));
	
	ItemDescriptionCPS->SetPosition(ClampedPosition);
}

FInv_SlotAvailabilityResult UDemoSpatialInventory::HasRoomForItem(UInv_ItemComponent* ItemComponent)
{
	switch (UInv_InventoryStatics::GetItemCategoryFromItemComponent(ItemComponent))
	{
	    case EItemCategory::Equippable:
		return Grid_Equippables->HasRoomForItem(ItemComponent);
		case EItemCategory::Consumable:
		return Grid_Consumables->HasRoomForItem(ItemComponent);
		case EItemCategory::Craftable:
		return Grid_Craftables->HasRoomForItem(ItemComponent);
		default:
		UE_LOG(LogTemp,Error,TEXT("ItemComponent doesn't have Valid Item Category"));
		return FInv_SlotAvailabilityResult();
	}
}

void UDemoSpatialInventory::OnItemHovered(UInv_InventoryItem* Item)
{
	const auto&Manifest=Item->GetItemManifest();
	UInv_ItemDescription*DescriptionWidget=GetItemDescription();
	
	
	
	DescriptionWidget->SetVisibility(ESlateVisibility::Collapsed);
	GetOwningPlayer()->GetWorldTimerManager().ClearTimer(DescriptionTimer);
	FTimerDelegate DescriptionTimerDelegate;
	DescriptionTimerDelegate.BindLambda([this,&Manifest,DescriptionWidget]()
	{
		Manifest.AssimilateInventoryFragments(DescriptionWidget);
		DescriptionWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
	});
	GetOwningPlayer()->GetWorldTimerManager().SetTimer(DescriptionTimer,DescriptionTimerDelegate,DescriptionTimerDelay,false);
}

void UDemoSpatialInventory::OnItemUnhovered()
{
	GetItemDescription()->SetVisibility(ESlateVisibility::Collapsed);
	GetOwningPlayer()->GetWorldTimerManager().ClearTimer(DescriptionTimer);
}

bool UDemoSpatialInventory::HasHoverItem() const
{
	if (Grid_Equippables->HasHoverItem()) return true;
	if (Grid_Consumables->HasHoverItem())return true;
	if (Grid_Craftables->HasHoverItem())return true;
	return false;
}

UInv_HoverItem* UDemoSpatialInventory::GetHoverItem() const
{
	if (!ActiveGrid.IsValid()) return nullptr;
	return ActiveGrid->GetHoverItem();
}

float UDemoSpatialInventory::GetTileSize() const
{
	return Grid_Equippables->GetTileSize();
}

void UDemoSpatialInventory::ShowEquippables()
{
	SetActiveGrid(Grid_Equippables,Button_Equippables);
}

void UDemoSpatialInventory::ShowConsumables()
{
	SetActiveGrid(Grid_Consumables,Button_Consumables);
}

void UDemoSpatialInventory::ShowCraftables()
{
	SetActiveGrid(Grid_Craftables,Button_Craftables);
}





void UDemoSpatialInventory::DisableButton(UButton* Button)
{
	Button_Equippables->SetIsEnabled(true);
	Button_Consumables->SetIsEnabled(true);
	Button_Craftables->SetIsEnabled(true);
	Button->SetIsEnabled(false);
}

void UDemoSpatialInventory::SetActiveGrid(UDemoInventoryGrid* Grid, UButton* Button)
{
	if (ActiveGrid.IsValid())
	{
		ActiveGrid->HideCursor();
		ActiveGrid->OnHide();
	}
	ActiveGrid=Grid;
	if (ActiveGrid.IsValid()) Grid->ShowCursor();
	DisableButton(Button);
	Switcher->SetActiveWidget(Grid);
}

bool UDemoSpatialInventory::CanEquipHoverItem(UInv_EquippedGridSlot* EquippedGridSlot,
	const FGameplayTag& EquipmentTypeTag) const
{
	if (!IsValid(EquippedGridSlot)||EquippedGridSlot->GetInventoryItem().IsValid()) return false;
	UInv_HoverItem*HoverItem=GetHoverItem();
	if (!IsValid(HoverItem))return false;
	
	UInv_InventoryItem*HeldItem=HoverItem->GetInventoryItem();
	
	return HasHoverItem()&&
		IsValid(HeldItem)&&
			!HoverItem->IsStackable()&&
				HeldItem->GetItemManifest().GetItemCategory()==EItemCategory::Equippable&&
					HeldItem->GetItemManifest().GetItemType().MatchesTag(EquipmentTypeTag);
}

UInv_EquippedGridSlot* UDemoSpatialInventory::FindSlotWithEquippedItem(UInv_InventoryItem* EquippedItem) const
{
	auto *FoundEquippedGridSlot=EquippedGridSlots.FindByPredicate([EquippedItem](const UInv_EquippedGridSlot* GridSlot)
	{
		return GridSlot->GetInventoryItem()==EquippedItem;
	});
	return FoundEquippedGridSlot?*FoundEquippedGridSlot:nullptr;
}

void UDemoSpatialInventory::ClearSlotOfItem(UInv_EquippedGridSlot* EquippedGridSlot)
{
	if (IsValid(EquippedGridSlot))
	{
		EquippedGridSlot->SetEquippedSlottedItem(nullptr);
		EquippedGridSlot->SetInventoryItem(nullptr);
	}
}

void UDemoSpatialInventory::RemoveEquippedSlottedItem(UInv_EquippedSlottedItem* EquippedSlottedItem)
{
	if (!IsValid(EquippedSlottedItem))return;
	
	if (EquippedSlottedItem->OnEquippedSlottedItemClicked.IsAlreadyBound(this,&ThisClass::EquippedSlottedItemClicked))
	{
		EquippedSlottedItem->OnEquippedSlottedItemClicked.RemoveDynamic(this,&ThisClass::EquippedSlottedItemClicked);
	}
	EquippedSlottedItem->RemoveFromParent();
}

void UDemoSpatialInventory::MakeEquippedSlottedItem(UInv_EquippedSlottedItem* EquippedSlottedItem,
	UInv_EquippedGridSlot* EquippedGridSlot, UInv_InventoryItem* ItemToEquip)
{
	if (!IsValid(EquippedGridSlot))return;
	UInv_EquippedSlottedItem*SlottedItem=EquippedGridSlot->OnItemEquipped(ItemToEquip,
		EquippedSlottedItem->GetEquipmentTypeTag(),
		UInv_InventoryStatics::GetInventoryWidget(GetOwningPlayer())->GetTileSize());
	if (IsValid(SlottedItem)) SlottedItem->OnEquippedSlottedItemClicked.AddDynamic(this,&ThisClass::EquippedSlottedItemClicked);
	EquippedGridSlot->SetEquippedSlottedItem(SlottedItem);
}

void UDemoSpatialInventory::BroadcastSlotClickedDelegates(UInv_InventoryItem* ItemToEquip,
	UInv_InventoryItem* ItemToUnequip) const
{
	UInventoryComponent*InventoryComponent=UInv_InventoryStatics::GetInventoryComponent(GetOwningPlayer());
	check(IsValid(InventoryComponent));
	InventoryComponent->Server_EquipSlotClicked(ItemToEquip,ItemToUnequip);
	
	if (GetOwningPlayer()->GetNetMode()!=NM_DedicatedServer)
	{
		InventoryComponent->OnItemEquipped.Broadcast(ItemToEquip);
		InventoryComponent->OnItemUnequipped.Broadcast(ItemToUnequip);
	}
}
