// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UI/InventoryWidget/Utils/Inv_WidgetUtils.h"
#include "Inv_InventoryStatics.generated.h"

class UInv_ItemComponent;
enum class EItemCategory : uint8;
class UInventoryComponent;
class UInv_InventoryItem;
class UInv_HoverItem;
class UDemoInventoryBase;
/**
 * 
 */
UCLASS()
class DEMOPROJECT_API UInv_InventoryStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable,Category="Inventory")
	static UInventoryComponent*GetInventoryComponent(const APlayerController* PlayerController);
	UFUNCTION(BlueprintCallable,Category="Inventory")
	static EItemCategory GetItemCategoryFromItemComponent(UInv_ItemComponent*ItemComponent);
	template<typename T,typename FuncT>
	static void ForEach2D(TArray<T>Array,int32 Index,const FIntPoint&Rang2D,int32 GridColumns,const FuncT&Function);
	
	UFUNCTION(BlueprintCallable,Category="Inventory")
	static void ItemHovered(APlayerController*PC,UInv_InventoryItem*Item);

	UFUNCTION(BlueprintCallable,Category="Inventory")
	static void ItemUnHovered(APlayerController*PC);
	UFUNCTION(BlueprintCallable,Category="Inventory")
	static UInv_HoverItem*GetHoverItem(APlayerController*PC);
	UFUNCTION(BlueprintCallable,Category="Inventory")
	static UDemoInventoryBase*GetInventoryWidget(APlayerController*PC);
};

template <typename T, typename FuncT>
void UInv_InventoryStatics::ForEach2D(TArray<T> Array, int32 Index, const FIntPoint& Rang2D, int32 GridColumns,
	const FuncT& Function)
{
	for (int32 j=0;j<Rang2D.Y;++j)
	{
		for (int32 i=0;i<Rang2D.X;++i)
		{
			const FIntPoint Coordinate=UInv_WidgetUtils::GetPositionFromIndex(Index,GridColumns)+FIntPoint(i,j);
			const int32 TileIndex=UInv_WidgetUtils::GetIndexFromPosition(Coordinate,GridColumns);
			if (Array.IsValidIndex(TileIndex))
			{
				Function(Array[TileIndex]);
			}
		}
	}
}
