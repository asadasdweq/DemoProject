// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/Inv_GridTypes.h"
#include "DemoInventoryBase.generated.h"

class UInv_ItemComponent;
class UInv_HoverItem;
struct FInv_SlotAvailabilityResult;
/**
 * 
 */
UCLASS()
class DEMOPROJECT_API UDemoInventoryBase : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual FInv_SlotAvailabilityResult HasRoomForItem(UInv_ItemComponent* ItemComponent){return FInv_SlotAvailabilityResult();}
	
	virtual void OnItemHovered(UInv_InventoryItem*InventoryItem){}
	virtual void OnItemUnhovered(){}
	virtual bool HasHoverItem()const{return false;}
	virtual UInv_HoverItem* GetHoverItem()const{return nullptr;}
	virtual float GetTileSize()const{return 0.f;}
};
