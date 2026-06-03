// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/InventoryWidget/InventoryBase/DemoInventoryBase.h"
#include"GameplayTagContainer.h"
#include "DemoSpatialInventory.generated.h"

class UCanvasPanel;
class UWidgetSwitcher;
class UDemoInventoryGrid;
class UButton;
class UInv_ItemDescription;
class UInv_EquippedGridSlot;
class UInv_EquippedSlottedItem;
/**
 * 
 */
UCLASS()
class DEMOPROJECT_API UDemoSpatialInventory : public UDemoInventoryBase
{
	GENERATED_BODY()
public:
	virtual void NativeOnInitialized() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	
	virtual FInv_SlotAvailabilityResult HasRoomForItem(UInv_ItemComponent* ItemComponent) override;
	virtual void OnItemHovered(UInv_InventoryItem* Item)override;
	virtual void OnItemUnhovered() override;
	virtual bool HasHoverItem()const override;
	virtual UInv_HoverItem* GetHoverItem()const override;
	virtual float GetTileSize()const override;
private:
	
	UPROPERTY()
	TArray<TObjectPtr<UInv_EquippedGridSlot>>EquippedGridSlots;
	UFUNCTION()
	void ShowEquippables();
	UFUNCTION()
	void ShowConsumables();
	UFUNCTION()
	void ShowCraftables();
	UFUNCTION()
	void EquippedGridSlotClicked(UInv_EquippedGridSlot*EquippedGridSlot,const FGameplayTag&EquipmentTypeTag);
	UFUNCTION()
	void EquippedSlottedItemClicked(UInv_EquippedSlottedItem*EquippedSlottedItem);
	void DisableButton(UButton* Button);
	void SetActiveGrid(UDemoInventoryGrid*Grid,UButton*Button);
	bool CanEquipHoverItem(UInv_EquippedGridSlot*EquippedGridSlot,const FGameplayTag&EquipmentTypeTag)const;
	UInv_EquippedGridSlot*FindSlotWithEquippedItem(UInv_InventoryItem*EquippedItem)const;
	void ClearSlotOfItem(UInv_EquippedGridSlot*EquippedGridSlot);
	void RemoveEquippedSlottedItem(UInv_EquippedSlottedItem*EquippedSlottedItem);
	void MakeEquippedSlottedItem(UInv_EquippedSlottedItem*EquippedSlottedItem,UInv_EquippedGridSlot*EquippedGridSlot,UInv_InventoryItem*ItemToEquip);
	
	void BroadcastSlotClickedDelegates(UInv_InventoryItem*ItemToEquip,UInv_InventoryItem*ItemToUnequip)const;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCanvasPanel>CanvasPanel;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWidgetSwitcher>Switcher;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UDemoInventoryGrid>Grid_Equippables;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UDemoInventoryGrid>Grid_Consumables;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UDemoInventoryGrid>Grid_Craftables;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton>Button_Equippables;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton>Button_Consumables;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton>Button_Craftables;
	
	TWeakObjectPtr<UDemoInventoryGrid>ActiveGrid;
	
	UPROPERTY(EditAnywhere,Category="Inventory")
	TSubclassOf<UInv_ItemDescription>ItemDescriptionClass;
	UPROPERTY()
	TObjectPtr<UInv_ItemDescription>ItemDescription;
	
	FTimerHandle DescriptionTimer;
	
	UPROPERTY(EditAnywhere,Category="Inventory")
	float DescriptionTimerDelay=0.5f;
	
	UInv_ItemDescription*GetItemDescription();
	void SetItemDescriptionSizeAndPosition(UInv_ItemDescription*Description,UCanvasPanel*Canvas)const;
};


