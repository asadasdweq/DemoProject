// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryManagement/FastArray/Inv_FastArray.h"
#include "InventoryComponent.generated.h"


struct FInv_SlotAvailabilityResult;
struct FInv_InventoryFastArray;
class UInv_ItemComponent;
class UDemoInventoryBase;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryItemChanged,UInv_InventoryItem*,Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStackChanged,const FInv_SlotAvailabilityResult&,Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNoRoomInInventory);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemEquipStatusChanged,UInv_InventoryItem*,Item);
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent),Blueprintable)
class DEMOPROJECT_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UInventoryComponent();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	void ToggleInventoryMenu();
	void AddRepSubObj(UObject*SubObj);
	void SpawnDroppedItem(UInv_InventoryItem*Item,int32 StackCount);
	UDemoInventoryBase*GetInventoryMenu() const{return InventoryMenu;};
	bool IsMenuOpen()const{return bInventoryMenuOpen;};
	UFUNCTION(BlueprintCallable,BlueprintAuthorityOnly,Category="Inventory")
	void TryAddItem(UInv_ItemComponent*ItemComponent);
	
	UFUNCTION(Server,Reliable)
	void Server_TryAddItem(UInv_ItemComponent*ItemComponent);
	
	UFUNCTION(Server,Reliable)
	void Server_AddNewItem(UInv_ItemComponent*ItemComponent,int32 StackCount);
	UFUNCTION(Server,Reliable)
	void Server_AddStacksToItem(UInv_ItemComponent*ItemComponent,int32 StackCount,int32 Remainder);
	UFUNCTION(Server,Reliable)
	void Server_DropItem(UInv_InventoryItem*Item,int32 StackCount);
	UFUNCTION(Server,Reliable)
	void Server_ConsumeItem(UInv_InventoryItem*Item);
	UFUNCTION(Server,Reliable)
	void Server_EquipSlotClicked(UInv_InventoryItem*ItemToEquip,UInv_InventoryItem*ItemToUnequip);
	UFUNCTION(NetMulticast,Reliable)
	void Multicast_EquipSlotClicked(UInv_InventoryItem*ItemToEquip,UInv_InventoryItem*ItemToUnequip);
	/*
	 * 委托
	 */
	FInventoryItemChanged OnItemAdded;
	FOnStackChanged OnStackChanged;
	FInventoryItemChanged OnItemRemoved;
	FNoRoomInInventory NoRoomInInventory;
	FItemEquipStatusChanged OnItemEquipped;
	FItemEquipStatusChanged OnItemUnequipped;
	
protected:
	
	virtual void BeginPlay() override;

	bool bInventoryMenuOpen;
	void OpenInventoryMenu();
	void CloseInventoryMenu();
private:
	TWeakObjectPtr<APlayerController>OwningController;
	
	UPROPERTY(Replicated)
	FInv_InventoryFastArray InventoryList;
	
	
	UPROPERTY()
	TObjectPtr<UDemoInventoryBase> InventoryMenu;
	UPROPERTY(EditAnywhere,Category="Inventory")
	TSubclassOf<UDemoInventoryBase>InventoryMenuClass;
	void ConstructInventory();
	
	UPROPERTY(EditAnywhere,Category="Inventory")
	float DropSpawnAngleMin=-85.f;
	UPROPERTY(EditAnywhere,Category="Inventory")
	float DropSpawnAngleMax=85.f;
	UPROPERTY(EditAnywhere,Category="Inventory")
	float DropSpawnDistanceMin=10.f;
	UPROPERTY(EditAnywhere,Category="Inventory")
	float DropSpawnDistanceMax=50.f;
	
	UPROPERTY(EditAnywhere,Category="Inventory")
	float RelativeSpawnElevation=70.f;

	UPROPERTY(EditAnywhere,Category="Inventory")
	float PickupValidationDistance=250.f;
};


