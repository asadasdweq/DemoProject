// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inv_GridSlot.generated.h"

class UInv_ItemPopUp;
class UImage;
class UInv_InventoryItem;
UENUM(BlueprintType)
enum class EGridSlotSlate:uint8
{
	Unoccupied,
	Occupied,
	Selected,
	GrayedOut
};
/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGridSlotEvent,int32,GridIndex,const FPointerEvent&,MouseEvent);
UCLASS()
class DEMOPROJECT_API UInv_GridSlot : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	TWeakObjectPtr<UInv_InventoryItem>GetInventoryItem()const{return InventoryItem;}
	
	void SetInventoryItem(UInv_InventoryItem* Item);
	
	void SetIndex(int32 Index){TileIndex=Index;}
	int32 GetIndex()const{return TileIndex;}
	void SetUpperLeftIndex(int32 Index){UpperLeftIndex=Index;}
	int32 GetUpperLeftIndex()const{return UpperLeftIndex;}
	void SetStackCount(int32 Count){StackCount=Count;}
	int32 GetStackCount()const{return StackCount;}
	void SetAvailable(bool bIsAvalable){bAvailable=bIsAvalable;}
	bool IsAvailable()const{return bAvailable;}
	//ItemPopUp
	void SetItemPopUp(UInv_ItemPopUp* PopUp);
	UInv_ItemPopUp *GetItemPopUp()const;
	
	
	EGridSlotSlate GetGridSlotSlate()const{return GridSlotSlate;}
	
	void SetUnoccupiedTexture();
	void SetOccupiedTexture();
	void SetSelectedTexture();
	void SetGrayedOutTexture();

	FGridSlotEvent GridSlotClicked;
	FGridSlotEvent GridSlotHovered;
	FGridSlotEvent GridSlotUnhovered;
protected:
	
	
private:
	int32 TileIndex{INDEX_NONE};
	int32 UpperLeftIndex{INDEX_NONE};
	int32 StackCount{0};
	bool bAvailable{true};
	TWeakObjectPtr<UInv_InventoryItem>InventoryItem;
	TWeakObjectPtr<UInv_ItemPopUp>ItemPopUp;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage>Image_GridSlot;
	
	UPROPERTY(EditAnywhere,Category="Inventory")
	FSlateBrush Brush_Unoccupied;
	
	UPROPERTY(EditAnywhere,Category="Inventory")
	FSlateBrush Brush_Occupied;
	
	UPROPERTY(EditAnywhere,Category="Inventory")
	FSlateBrush Brush_Selected;
	
	UPROPERTY(EditAnywhere,Category="Inventory")
	FSlateBrush Brush_GrayedOut;
	
	EGridSlotSlate GridSlotSlate;
	
	
	UFUNCTION()
	void OnItemPopUpDestruct(UUserWidget*Menu);
};
