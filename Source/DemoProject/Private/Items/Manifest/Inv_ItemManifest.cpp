#include"Items\Manifest\Inv_ItemManifest.h"
#include "Items/Fragments/Inv_ItemFragment.h"
#include "Items/Inv_InventoryItem.h"
#include "Items/Components/Inv_ItemComponent.h"
#include "UI/InventoryWidget/CompositeBase/Inv_CompositeBase.h"
#include "Engine/World.h"

UInv_InventoryItem* FInv_ItemManifest::Manifest(UObject* NewOuter)
{
	UInv_InventoryItem* Item = NewObject<UInv_InventoryItem>(NewOuter,UInv_InventoryItem::StaticClass());
	Item->SetItemManifest(*this);
	
	for (auto&Fragment:Item->GetItemManifestMutable().GetFragmentsMutable())
	{
		Fragment.GetMutable().Manifest();
	}
	ClearFragments();
	return Item;
}

void FInv_ItemManifest::AssimilateInventoryFragments(UInv_CompositeBase* Composite) const
{
	const auto &InventoryFragments=GetAllFragmentsOfType<FInv_InventoryItemFragment>();
	for (auto*Fragment:InventoryFragments)
	{
		Composite->ApplyFunction([Fragment](UInv_CompositeBase* Widget)
		{
			Fragment->Assimilate(Widget);
		});
	}
}

void FInv_ItemManifest::SpawnPickupActor(const UObject* WorldContextObject, const FVector& SpawnLocation,
                                         const FRotator& SpawnRotation)
{
	if (!IsValid(WorldContextObject) || !PickupActorClass)return;
	UWorld* World = WorldContextObject->GetWorld();
	if (!IsValid(World)) return;
	
	AActor *SpawnPickupActor = World->SpawnActor<AActor>(PickupActorClass,SpawnLocation,SpawnRotation);
	if (!IsValid(SpawnPickupActor)) return;
	UInv_ItemComponent*ItemComp=SpawnPickupActor->FindComponentByClass<UInv_ItemComponent>();
	if (!IsValid(ItemComp))
	{
		UE_LOG(LogTemp, Error, TEXT("Pickup actor class must contain an Inv_ItemComponent. PickupActorClass=%s SpawnedClass=%s"),
			*GetNameSafe(PickupActorClass.Get()),
			*GetNameSafe(SpawnPickupActor->GetClass()));
		SpawnPickupActor->Destroy();
		return;
	}
	ItemComp->InitItemManifest(*this);
}

void FInv_ItemManifest::ClearFragments()
{
	{
		for (auto&Fragment:Fragments)
		{
			Fragment.Reset();
		}
		Fragments.Empty();
	}
}
