// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inv_SlottedItem.generated.h"
class UInv_InventoryItem;
class UTextBlock;
class UImage;
/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSlottedItemClicked,int32,GridIndex,const FPointerEvent&,MouseEvent);
UCLASS()
class DEMOPROJECT_API UInv_SlottedItem : public UUserWidget
{
	GENERATED_BODY()
public:
	
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	
	
	bool IsStackable() const {return bIsStackable;}
	void SetIsStackable(bool bStackable){bIsStackable=bStackable;}
	
	UImage *GetImageIcon()const{return Image_Icon;}
	
	void SetGridIndex(int32 Index) { GridIndex=Index;}
	int32 GetGridIndex(){return GridIndex;}
	
	void SetGridDimensions(const FIntPoint& Dimension){GridDimension=Dimension;}
	FIntPoint GetGridDimensions(){return GridDimension;}
	void SetImageBrush(const FSlateBrush&Brush)const;
	
	void UpdateStackCount(int32 StackCount);
	
	void SetInventoryItem(UInv_InventoryItem* Item);
	UInv_InventoryItem*GetInventoryItem()const {return InventoryItem.Get();}
	
	FSlottedItemClicked OnSlottedItemClicked;
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage>Image_Icon;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock>Text_StackCount;
	
	int32 GridIndex;
	
	FIntPoint GridDimension;
	
	TWeakObjectPtr<UInv_InventoryItem>InventoryItem;
	
	bool bIsStackable{false};
};
