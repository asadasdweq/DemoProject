#include"InventoryManagement/FastArray/Inv_FastArray.h"

#include "InventoryManagement/Components/InventoryComponent.h"
#include"Items/Inv_InventoryItem.h"
#include "Items/Components/Inv_ItemComponent.h"

TArray<UInv_InventoryItem*> FInv_InventoryFastArray::GetAllItems() const
{
	TArray<UInv_InventoryItem*> Results;
	Results.Reserve(Entries.Num());
	for (const auto& Entry : Entries)
	{
		if (!IsValid(Entry.Item))continue;
		Results.Add(Entry.Item);
	}
	return Results;
}

void FInv_InventoryFastArray::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	UInventoryComponent*InventoryComponent=Cast<UInventoryComponent>(OwnerComponent);
	if (!IsValid(InventoryComponent))return;
	for (int32 Index:RemovedIndices)
	{
		InventoryComponent->OnItemRemoved.Broadcast(Entries[Index].Item);
	}
}

void FInv_InventoryFastArray::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	UInventoryComponent*InventoryComponent=Cast<UInventoryComponent>(OwnerComponent);
	if (!IsValid(InventoryComponent))return;
	for (int32 Index:AddedIndices)
	{
		InventoryComponent->OnItemAdded.Broadcast(Entries[Index].Item);
	}
}

UInv_InventoryItem* FInv_InventoryFastArray::AddEntry(UInv_ItemComponent* ItemComponent)
{
	check(OwnerComponent)
	AActor*OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority())
	UInventoryComponent* IC = Cast<UInventoryComponent>(OwnerComponent);
	if (!IsValid(IC))return nullptr;
	FInv_InventoryEntry &NewEntry=Entries.AddDefaulted_GetRef();
	NewEntry.Item=ItemComponent->GetItemManifest().Manifest(OwningActor);
	IC->AddRepSubObj(NewEntry.Item);
	MarkItemDirty(NewEntry);
	return NewEntry.Item;
}


UInv_InventoryItem* FInv_InventoryFastArray::AddEntry(UInv_InventoryItem* Item)
{
	check(OwnerComponent);
	AActor *OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());
	FInv_InventoryEntry &NewEntry=Entries.AddDefaulted_GetRef();//
	NewEntry.Item=Item;
	MarkItemDirty(NewEntry);
	return Item;
}

void FInv_InventoryFastArray::RemoveEntry(UInv_InventoryItem* Item)
{
	for (auto EntryIt=Entries.CreateIterator();EntryIt;++EntryIt)
	{
		FInv_InventoryEntry&Entry=*EntryIt;
		if (Entry.Item==Item)
		{
			EntryIt.RemoveCurrent();
			MarkArrayDirty();
		}
	}
}

void FInv_InventoryFastArray::MarkItemDirtyByItem(UInv_InventoryItem* Item)
{
	for (FInv_InventoryEntry& Entry : Entries)
	{
		if (Entry.Item == Item)
		{
			MarkItemDirty(Entry);
			return;
		}
	}
}

UInv_InventoryItem* FInv_InventoryFastArray::FindFirstItemByType(const FGameplayTag& ItemType)
{
	auto FoundItem=Entries.FindByPredicate([ItemType=ItemType](const FInv_InventoryEntry& Entry)
	{
		return IsValid(Entry.Item)&&Entry.Item->GetItemManifest().GetItemType()==ItemType;
	});
	return FoundItem?FoundItem->Item:nullptr;
}

