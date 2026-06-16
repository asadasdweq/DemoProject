// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryManagement/Components/InventoryComponent.h"
#include"UI/InventoryWidget/InventoryBase/DemoInventoryBase.h"
#include "Blueprint/UserWidget.h"
#include "Items/Inv_InventoryItem.h"
#include "Items/Components/Inv_ItemComponent.h"
#include "Items/Fragments/Inv_ItemFragment.h"
#include"Items/Manifest/Inv_ItemManifest.h"
#include "Net/UnrealNetwork.h"


UInventoryComponent::UInventoryComponent():InventoryList(this)
{
	
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
	bReplicateUsingRegisteredSubObjectList=true;
	bInventoryMenuOpen=false;
	
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass,InventoryList);
}

void UInventoryComponent::ToggleInventoryMenu()
{
	if (bInventoryMenuOpen)
	{
		CloseInventoryMenu();
		InventoryMenu->GetOwningPlayer()->SetMouseCursorWidget(EMouseCursor::Default,nullptr);	
	}
	else
	{
		OpenInventoryMenu();
	}
	
}

void UInventoryComponent::AddRepSubObj(UObject* SubObj)
{
	if (IsUsingRegisteredSubObjectList()&&IsReadyForReplication()&&IsValid(SubObj))
	{
		AddReplicatedSubObject(SubObj);
	}
}

void UInventoryComponent::SpawnDroppedItem(UInv_InventoryItem* Item, int32 StackCount)
{
	const APawn*OwningPawn=OwningController->GetPawn();
	FVector RotatedForward=OwningPawn->GetActorForwardVector();
	RotatedForward=RotatedForward.RotateAngleAxis(FMath::RandRange(DropSpawnAngleMin,DropSpawnAngleMax),FVector::UpVector);
	FVector SpawnLocation=OwningPawn->GetActorLocation()+RotatedForward*FMath::RandRange(DropSpawnDistanceMin,DropSpawnDistanceMax);
	SpawnLocation.Z-=RelativeSpawnElevation;
	const FRotator SpawnRotation=FRotator::ZeroRotator;
	FInv_ItemManifest &ItemManifest=Item->GetItemManifestMutable();
	if (FInv_StackableFragment*StackableFragment=ItemManifest.GetFragmentOfTypeMutable<FInv_StackableFragment>())
	{
		StackableFragment->SetStackCount(StackCount);
	}
	ItemManifest.SpawnPickupActor(this,SpawnLocation,SpawnRotation);
}


void UInventoryComponent::TryAddItem(UInv_ItemComponent* ItemComponent)
{
	if (!IsValid(ItemComponent))
	{
		return;
	}
	
	if (!IsValid(InventoryMenu))
	{
		if (GetOwner() && GetOwner()->HasAuthority())
		{
			Server_TryAddItem_Implementation(ItemComponent);
		}
		else
		{
			Server_TryAddItem(ItemComponent);
		}
		return;
	}
	
	FInv_SlotAvailabilityResult Result=InventoryMenu->HasRoomForItem(ItemComponent);
	UInv_InventoryItem*FoundItem=InventoryList.FindFirstItemByType(ItemComponent->GetItemManifest().GetItemType());
	Result.Item=FoundItem;
	if (Result.TotalRoomToFill==0)
	{
		NoRoomInInventory.Broadcast();
		return;
	}
	if (Result.Item.IsValid()&&Result.bStackable)
	{

		OnStackChanged.Broadcast(Result);
		
	}
	
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		Server_TryAddItem_Implementation(ItemComponent);
	}
	else
	{
		Server_TryAddItem(ItemComponent);
	}
}

void UInventoryComponent::Server_TryAddItem_Implementation(UInv_ItemComponent* ItemComponent)
{
	if (!IsValid(ItemComponent) || !IsValid(ItemComponent->GetOwner()))
	{
		return;
	}

	APawn* OwningPawn = OwningController.IsValid() ? OwningController->GetPawn() : nullptr;
	if (!IsValid(OwningPawn))
	{
		return;
	}

	const float MaxPickupDistanceSquared = FMath::Square(PickupValidationDistance);
	if (FVector::DistSquared(OwningPawn->GetActorLocation(), ItemComponent->GetOwner()->GetActorLocation()) > MaxPickupDistanceSquared)
	{
		return;
	}

	FInv_ItemManifest& PickupManifest = ItemComponent->GetItemManifestMutable();
	FInv_StackableFragment* PickupStackableFragment = PickupManifest.GetFragmentOfTypeMutable<FInv_StackableFragment>();
	const bool bStackable = PickupStackableFragment != nullptr;
	const FGameplayTag ItemType = PickupManifest.GetItemType();
	int32 RemainingStackCount = bStackable ? FMath::Max(1, PickupStackableFragment->GetStackCount()) : 1;

	if (bStackable)
	{
		for (UInv_InventoryItem* InventoryItem : InventoryList.GetAllItems())
		{
			if (RemainingStackCount <= 0)
			{
				break;
			}

			if (!IsValid(InventoryItem) || InventoryItem->GetItemManifest().GetItemType() != ItemType)
			{
				continue;
			}

			FInv_StackableFragment* InventoryStackableFragment = InventoryItem->GetItemManifestMutable().GetFragmentOfTypeMutable<FInv_StackableFragment>();
			if (!InventoryStackableFragment)
			{
				continue;
			}

			const int32 CurrentStackCount = InventoryItem->GetStackCount();
			const int32 AvailableRoom = FMath::Max(0, InventoryStackableFragment->GetMaxStackSize() - CurrentStackCount);
			const int32 AmountToFill = FMath::Min(AvailableRoom, RemainingStackCount);
			if (AmountToFill <= 0)
			{
				continue;
			}

			InventoryItem->SetTotalStackCount(CurrentStackCount + AmountToFill);
			InventoryList.MarkItemDirtyByItem(InventoryItem);
			RemainingStackCount -= AmountToFill;
		}
	}

	if (RemainingStackCount <= 0)
	{
		ItemComponent->PickedUp();
		return;
	}

	const int32 NewEntryStackCount = bStackable
		? FMath::Min(RemainingStackCount, PickupStackableFragment->GetMaxStackSize())
		: 0;
	
	UInv_InventoryItem* NewItem = InventoryList.AddEntry(ItemComponent);
	if (!IsValid(NewItem))
	{
		return;
	}

	NewItem->SetTotalStackCount(NewEntryStackCount);
	InventoryList.MarkItemDirtyByItem(NewItem);
	
	if (GetOwner()->GetNetMode()==NM_Standalone||GetOwner()->GetNetMode()==NM_ListenServer)
	{
		OnItemAdded.Broadcast(NewItem);
	}

	if (!bStackable)
	{
		ItemComponent->PickedUp();
		return;
	}

	RemainingStackCount -= NewEntryStackCount;
	if (RemainingStackCount <= 0)
	{
		ItemComponent->PickedUp();
	}
	else
	{
		ItemComponent->SetStackCount(RemainingStackCount);
		ItemComponent->GetOwner()->ForceNetUpdate();
	}
}

void UInventoryComponent::Server_EquipSlotClicked_Implementation(UInv_InventoryItem* ItemToEquip,
	UInv_InventoryItem* ItemToUnequip)
{
	Multicast_EquipSlotClicked(ItemToEquip,ItemToUnequip);
}

void UInventoryComponent::Multicast_EquipSlotClicked_Implementation(UInv_InventoryItem* ItemToEquip,
	UInv_InventoryItem* ItemToUnequip)
{
	OnItemUnequipped.Broadcast(ItemToUnequip);
	OnItemEquipped.Broadcast(ItemToEquip);
}

void UInventoryComponent::Server_DropItem_Implementation(UInv_InventoryItem* Item, int32 StackCount)
{
	if (!IsValid(Item))
	{
		return;
	}

	const bool bStackable = Item->IsStackable();
	if (bStackable && StackCount <= 0)
	{
		return;
	}

	StackCount = bStackable ? FMath::Min(StackCount, Item->GetStackCount()) : 1;
	const int32 NewStackCount=Item->GetStackCount()-StackCount;
	if (NewStackCount<=0)
	{
		InventoryList.RemoveEntry(Item);
	}
	else
	{
		Item->SetTotalStackCount(NewStackCount);
		InventoryList.MarkItemDirtyByItem(Item);
	}
	SpawnDroppedItem(Item,StackCount);
}

void UInventoryComponent::Server_ConsumeItem_Implementation(UInv_InventoryItem* Item)
{
	if (!IsValid(Item))
	{
		return;
	}

	const int32 NewStackCount=Item->GetStackCount()-1;
	if(NewStackCount<=0)
	{
		InventoryList.RemoveEntry(Item);
	}
	else
	{
		Item->SetTotalStackCount(NewStackCount);
		InventoryList.MarkItemDirtyByItem(Item);
	}
	if (FInv_ConsumableFragment*ConsumableFragment=Item->GetItemManifestMutable().GetFragmentOfTypeMutable<FInv_ConsumableFragment>())
	{
		ConsumableFragment->OnConsume(OwningController.Get());
	}
}

void UInventoryComponent::Server_AddNewItem_Implementation(UInv_ItemComponent* ItemComponent, int32 StackCount)
{
	if (!IsValid(ItemComponent))
	{
		return;
	}
	
	UInv_InventoryItem*NewItem=InventoryList.AddEntry(ItemComponent);
	if (!IsValid(NewItem))
	{
		return;
	}
	
	NewItem->SetTotalStackCount(StackCount);
	InventoryList.MarkItemDirtyByItem(NewItem);
	if (GetOwner()->GetNetMode()==NM_Standalone||GetOwner()->GetNetMode()==NM_ListenServer)
	{
		OnItemAdded.Broadcast(NewItem);
	}
	
	ItemComponent->PickedUp();
}


void UInventoryComponent::Server_AddStacksToItem_Implementation(UInv_ItemComponent* ItemComponent, int32 StackCount,
	int32 Remainder)
{
	if (!IsValid(ItemComponent))
	{
		return;
	}

	const FGameplayTag ItemType=ItemComponent->GetItemManifest().GetItemType();
	UInv_InventoryItem*InventoryItem=InventoryList.FindFirstItemByType(ItemType);
	if (IsValid(InventoryItem))
	{
		if (FInv_StackableFragment* StackableFragment = InventoryItem->GetItemManifestMutable().GetFragmentOfTypeMutable<FInv_StackableFragment>())
		{
			const int32 AmountAdded = FMath::Clamp(StackCount - Remainder, 0, StackableFragment->GetMaxStackSize() - InventoryItem->GetStackCount());
			if (AmountAdded > 0)
			{
				InventoryItem->SetTotalStackCount(InventoryItem->GetStackCount() + AmountAdded);
				InventoryList.MarkItemDirtyByItem(InventoryItem);
			}
		}
	}
	
	if (Remainder==0)
	{
		ItemComponent->PickedUp();
	}
	else
	{
		ItemComponent->SetStackCount(Remainder);
		ItemComponent->GetOwner()->ForceNetUpdate();
	}
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	ConstructInventory();
	
}

void UInventoryComponent::ConstructInventory()
{
	OwningController=Cast<APlayerController>(GetOwner());
	checkf(OwningController.IsValid(),TEXT("InventoryComponent No Owner"));
	if (!OwningController->IsLocalController())return;
	InventoryMenu=CreateWidget<UDemoInventoryBase>(OwningController.Get(),InventoryMenuClass);
	InventoryMenu->AddToViewport();
	
	CloseInventoryMenu();
	
}

void UInventoryComponent::OpenInventoryMenu()
{
	if(!IsValid(InventoryMenu)) return;
	InventoryMenu->SetVisibility(ESlateVisibility::Visible);
	bInventoryMenuOpen=true;
	/*
	if (!OwningController.IsValid()) return;
	FInputModeGameAndUI InputMode;
	OwningController->SetInputMode(InputMode);
	OwningController->SetShowMouseCursor(true);*/
}

void UInventoryComponent::CloseInventoryMenu()
{
	if(!IsValid(InventoryMenu)) return;
	InventoryMenu->SetVisibility(ESlateVisibility::Collapsed);
	bInventoryMenuOpen=false;
	
	/*
	if (!OwningController.IsValid()) return;
	FInputModeGameOnly InputMode;
	OwningController->SetInputMode(InputMode);
	OwningController->SetShowMouseCursor(false);*/
}




