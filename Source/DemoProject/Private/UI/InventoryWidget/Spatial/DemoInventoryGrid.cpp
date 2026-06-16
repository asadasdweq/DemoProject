// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InventoryWidget/Spatial/DemoInventoryGrid.h"


#include "InventoryManagement/Components/InventoryComponent.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "InventoryManagement/Utils/Inv_InventoryStatics.h"
#include "Items/Inv_InventoryItem.h"
#include "Items/Components/Inv_ItemComponent.h"
#include "Items/Fragments/Inv_ItemFragment.h"
#include"UI/InventoryWidget/GridSlots/Inv_GridSlot.h"
#include "UI/InventoryWidget/Utils/Inv_WidgetUtils.h"
#include"UI/InventoryWidget/SlottedItems/Inv_SlottedItem.h"
#include "Items/Fragments/Inv_FragmentTags.h"
#include "UI/InventoryWidget/HoverItem/Inv_HoverItem.h"
#include"UI/InventoryWidget/ItemPopUp/Inv_ItemPopUp.h"

void UDemoInventoryGrid::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	ConstructGrid();
	InventoryComponent=UInv_InventoryStatics::GetInventoryComponent(GetOwningPlayer());
	InventoryComponent->OnItemAdded.AddDynamic(this,&ThisClass::AddItem);
	InventoryComponent->OnStackChanged.AddDynamic(this,&ThisClass::AddStacks);
	
}

void UDemoInventoryGrid::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	const FVector2D CanvasPosition=UInv_WidgetUtils::GetWidgetPosition(CanvasPanel);
	
	const FVector2D MousePosition=UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer());
	if (CursorExitedCanvas(CanvasPosition,UInv_WidgetUtils::GetWidgetSize(CanvasPanel),MousePosition))
	{
		return;
	}
	UpdateTileParameters(CanvasPosition,MousePosition);
	
}

void UDemoInventoryGrid::ShowCursor()
{
	if (!IsValid(GetOwningPlayer())) return;
	GetOwningPlayer()->SetMouseCursorWidget(EMouseCursor::Default,GetVisibleCursorWidget());
}

void UDemoInventoryGrid::HideCursor()
{
	if (!IsValid(GetOwningPlayer())) return;
	GetOwningPlayer()->SetMouseCursorWidget(EMouseCursor::Default,GetHiddenCursorWidget());
}

void UDemoInventoryGrid::UpdateTileParameters(const FVector2D& CanvasPosition, const FVector2D& MousePosition)
{
	if (!bMouseWithinCanvas)return;
	const FIntPoint HoveredTileCoordinates=CalculateHoveredCoordinates(CanvasPosition,MousePosition);
	LastTileParameters=TileParameters;
	TileParameters.TileCoordinates=HoveredTileCoordinates;
	TileParameters.TileIndex=UInv_WidgetUtils::GetIndexFromPosition(HoveredTileCoordinates,Columns);
	TileParameters.TileQuadrant=CalculateTileQuadrant(CanvasPosition,MousePosition);
	
	OnTileParametersUpdate(TileParameters);
}

FIntPoint UDemoInventoryGrid::CalculateHoveredCoordinates(const FVector2D& CanvasPosition,
	const FVector2D& MousePosition) const
{
	const FIntPoint Coordinates{
		static_cast<int32>(FMath::FloorToInt((MousePosition.X-CanvasPosition.X)/TileSize))
		,static_cast<int32>(FMath::FloorToInt((MousePosition.Y-CanvasPosition.Y)/TileSize))
	};
	return Coordinates;
}

EInv_TileQuadrant UDemoInventoryGrid::CalculateTileQuadrant(const FVector2D& CanvasPosition,
	const FVector2D& MousePosition) const
{
	const float TileLocalX=FMath::Fmod(MousePosition.X-CanvasPosition.X,TileSize);
	const float TileLocalY=FMath::Fmod(MousePosition.Y-CanvasPosition.Y,TileSize);
	const bool bIsLeft=TileLocalX<TileSize/2;
	const bool bIsTop=TileLocalY<TileSize/2;
	
	EInv_TileQuadrant HoveredTileQuadrant{EInv_TileQuadrant::None};
	if (bIsTop && bIsLeft) HoveredTileQuadrant=EInv_TileQuadrant::TopLeft;
	else if (bIsTop&&!bIsLeft)HoveredTileQuadrant=EInv_TileQuadrant::TopRight;
	else if (!bIsTop&&bIsLeft)HoveredTileQuadrant=EInv_TileQuadrant::BottomLeft;
	else if (!bIsTop&&!bIsLeft)HoveredTileQuadrant=EInv_TileQuadrant::BottomRight;
	
	return HoveredTileQuadrant;
	
}

void UDemoInventoryGrid::OnTileParametersUpdate(const FInv_TileParameters& Parameters)
{
	if (!IsValid(HoverItem)) return;
	const FIntPoint Dimensions=HoverItem->GetGridDimension();
	const FIntPoint StartingCoordinate=CalculateStartingCoordinate(Parameters.TileCoordinates,Dimensions,Parameters.TileQuadrant);
	ItemDropIndex=UInv_WidgetUtils::GetIndexFromPosition(StartingCoordinate,Columns);
	//TODO:SpaceResult
	CurrentQueryResult=CheckHoverPosition(StartingCoordinate,Dimensions);
	if (CurrentQueryResult.bHasSpace)
	{
		HighlightSlots(ItemDropIndex,Dimensions);
		return;
	}
	UnHighlightSlots(LastHighlightedIndex,LastHighlightedDimensions);
	
	if (CurrentQueryResult.ValidItem.IsValid()&&GridSlots.IsValidIndex(CurrentQueryResult.UpperLeftIndex))
	{
		const FInv_GridFragment*GridFragment=GetFragment<FInv_GridFragment>(CurrentQueryResult.ValidItem.Get(),FragmentTags::GridFragment);
		if (!GridFragment) return;
		ChangeHoverType(CurrentQueryResult.UpperLeftIndex,GridFragment->GetGridSize(),EGridSlotSlate::GrayedOut);
	}
}

FIntPoint UDemoInventoryGrid::CalculateStartingCoordinate(const FIntPoint& Coordinate, const FIntPoint& Dimensions,
	const EInv_TileQuadrant Quadrant) const
{
	const int32 HasEvenWidth=Dimensions.X%2==0?1:0;
	const int32 HasEvenHeight=Dimensions.Y%2==0?1:0;
	
	FIntPoint StartingCoord;
	switch (Quadrant)
	{
		case EInv_TileQuadrant::TopLeft:
		StartingCoord.X=Coordinate.X-FMath::FloorToInt(0.5f*Dimensions.X);
		StartingCoord.Y=Coordinate.Y-FMath::FloorToInt(0.5f*Dimensions.Y);
		break;
		case EInv_TileQuadrant::TopRight:
		StartingCoord.X=Coordinate.X-FMath::FloorToInt(0.5f*Dimensions.X)+HasEvenWidth;
		StartingCoord.Y=Coordinate.Y-FMath::FloorToInt(0.5f*Dimensions.Y);
		break;
	    case EInv_TileQuadrant::BottomLeft:
		StartingCoord.X=Coordinate.X-FMath::FloorToInt(0.5f*Dimensions.X);
		StartingCoord.Y=Coordinate.Y-FMath::FloorToInt(0.5f*Dimensions.Y)+HasEvenHeight;
		break;
		case EInv_TileQuadrant::BottomRight:
		StartingCoord.X=Coordinate.X-FMath::FloorToInt(0.5f*Dimensions.X)+HasEvenWidth;
		StartingCoord.Y=Coordinate.Y-FMath::FloorToInt(0.5f*Dimensions.Y)+HasEvenHeight;
		break;
		default:
		UE_LOG(LogTemp, Warning,TEXT("Invalid Quadrant"));
		return FIntPoint(-1,-1);
	}
	return StartingCoord;
}

FInv_SpaceQueryResult UDemoInventoryGrid::CheckHoverPosition(const FIntPoint& Position, const FIntPoint& Dimensions)
{
	FInv_SpaceQueryResult Result;
	
	if (!IsInGridBounds(UInv_WidgetUtils::GetIndexFromPosition(Position,Columns),Dimensions)) return Result;
	Result.bHasSpace=true;
	TSet<int32>OccupiedUpperLeftIndices;
	UInv_InventoryStatics::ForEach2D(GridSlots,UInv_WidgetUtils::GetIndexFromPosition(Position,Columns),Dimensions,
		Columns,[&](const UInv_GridSlot*GridSlot)
	{
		if (GridSlot->GetInventoryItem().IsValid())
		{
			OccupiedUpperLeftIndices.Add(GridSlot->GetUpperLeftIndex());
			Result.bHasSpace=false;
		}
	});
	if (OccupiedUpperLeftIndices.Num()==1)
	{
		const int32 Index=*OccupiedUpperLeftIndices.CreateConstIterator();
		Result.UpperLeftIndex=GridSlots[Index]->GetUpperLeftIndex();
		Result.ValidItem=GridSlots[Index]->GetInventoryItem();
	}
	return Result;
	
}

FInv_SlotAvailabilityResult UDemoInventoryGrid::HasRoomForItem(const UInv_ItemComponent* ItemComponent)
{
	return HasRoomForItem(ItemComponent->GetItemManifest());
}

void UDemoInventoryGrid::SetOwningCanvas(UCanvasPanel* OwningCanvas)
{
	OwningCanvasPanel=OwningCanvas;
}

bool UDemoInventoryGrid::HasHoverItem() const
{
	return IsValid(HoverItem);
}

UInv_HoverItem* UDemoInventoryGrid::GetHoverItem() const
{
	return HoverItem;
}

void UDemoInventoryGrid::AssignHoverItem(UInv_InventoryItem* Item)
{
	if (!IsValid(HoverItem))
	{
		HoverItem=CreateWidget<UInv_HoverItem>(GetOwningPlayer(),HoverItemClass);
	}
	const FInv_GridFragment*GridFragment=GetFragment<FInv_GridFragment>(Item,FragmentTags::GridFragment);
	const FInv_ImageFragment*ImageFragment=GetFragment<FInv_ImageFragment>(Item,FragmentTags::IconFragment);
	const FVector2D DrawSize=GetDrawSize(GridFragment);
	
	FSlateBrush Brush;
	Brush.SetResourceObject(ImageFragment->GetIcon());
	Brush.DrawAs=ESlateBrushDrawType::Image;
	Brush.ImageSize=DrawSize*UWidgetLayoutLibrary::GetViewportScale(this);
	
	HoverItem->SetImageBrush(Brush);
	HoverItem->SetGridDimension(GridFragment->GetGridSize());
	HoverItem->SetInventoryItem(Item);
	HoverItem->SetIsStackable(Item->IsStackable());
	
	GetOwningPlayer()->SetMouseCursorWidget(EMouseCursor::Default,HoverItem);
}

void UDemoInventoryGrid::ClearHoverItem()
{
	if (!IsValid(HoverItem)) return;
	
	HoverItem->SetInventoryItem(nullptr);
	HoverItem->SetIsStackable(false);
	HoverItem->SetPreviousGridIndex(INDEX_NONE);
	HoverItem->UpdateStackCount(0);
	HoverItem->SetImageBrush(FSlateNoResource());
	HoverItem->RemoveFromParent();
	HoverItem=nullptr;
	ShowCursor();
}

void UDemoInventoryGrid::DropItem()
{
	if (!IsValid(HoverItem))return;
	if (!IsValid(HoverItem->GetInventoryItem()))return;
	
	const int32 DropStackCount = HoverItem->IsStackable() ? HoverItem->GetStackCount() : 1;
	InventoryComponent->Server_DropItem(HoverItem->GetInventoryItem(),DropStackCount);
	
	ClearHoverItem();
	ShowCursor();
}

void UDemoInventoryGrid::OnHide()
{
	PutHoverItemBack();
}

FInv_SlotAvailabilityResult UDemoInventoryGrid::HasRoomForItem(const UInv_InventoryItem* InventoryItem,
                                                               const int32 StackAmountOverride)
{
	return HasRoomForItem(InventoryItem->GetItemManifest(),StackAmountOverride);
}

FInv_SlotAvailabilityResult UDemoInventoryGrid::HasRoomForItem(const FInv_ItemManifest& ItemManifest,
	const int32 StackAmountOverride)
{
	FInv_SlotAvailabilityResult Result;
	const FInv_StackableFragment*StackableFragment=ItemManifest.GetFragmentOfType<FInv_StackableFragment>();
	Result.bStackable=StackableFragment!=nullptr;
	
	const int32 MaxStackSize=StackableFragment?StackableFragment->GetMaxStackSize():1;
	int32 AmountToFill=StackableFragment?StackableFragment->GetStackCount():1;
	if (StackAmountOverride!=-1&&Result.bStackable)
	{
		AmountToFill=StackAmountOverride;
	}
	TSet<int32>CheckIndices;
	for (const auto&GridSlot:GridSlots)
	{
		if (AmountToFill==0)break;
		if (IsIndexClaimed(CheckIndices,GridSlot->GetIndex()))continue;
		if (!IsInGridBounds(GridSlot->GetIndex(),GetItemDimensions(ItemManifest)))continue;
		TSet<int32>TentativelyClaimed;
		if (!HasRoomAtIndex(GridSlot,GetItemDimensions(ItemManifest),CheckIndices,TentativelyClaimed,ItemManifest.GetItemType(),MaxStackSize))
		{
			continue;
		}
		const int32 AmountToFillInSlot = DetermineFillAmountForSlot(Result.bStackable,MaxStackSize,AmountToFill,GridSlot);
		if (AmountToFillInSlot==0)continue;
		CheckIndices.Append(TentativelyClaimed);
		Result.TotalRoomToFill+=AmountToFillInSlot;
		Result.SlotAvailabilities.Emplace(
			FInv_SlotAvailability{HasValidItem(GridSlot)?GridSlot->GetUpperLeftIndex():GridSlot->GetIndex(),
				Result.bStackable?AmountToFillInSlot:0,
				HasValidItem(GridSlot)
			});
		AmountToFill-=AmountToFillInSlot;
		Result.Remainder=AmountToFill;
		if (AmountToFill==0)return Result;
	}
	return Result;
	
	
}



bool UDemoInventoryGrid::MatchesCategory(const UInv_InventoryItem* InventoryItem) const
{
	return InventoryItem->GetItemManifest().GetItemCategory()==ItemCategory;
}

void UDemoInventoryGrid::AddItem(UInv_InventoryItem* InventoryItem)
{
	if (!MatchesCategory(InventoryItem)) return;
	FInv_SlotAvailabilityResult Result=HasRoomForItem(InventoryItem);
	AddItemToIndices(Result,InventoryItem);
}
void UDemoInventoryGrid::AddItemToIndices(const FInv_SlotAvailabilityResult& Result, UInv_InventoryItem* InventoryItem)
{
	for (const auto&Availability:Result.SlotAvailabilities)
	{
		AddItemAtIndex(InventoryItem,Availability.Index,Result.bStackable,Availability.AmountToFill);
		UpdateGridSlots(InventoryItem,Availability.Index,Result.bStackable,Availability.AmountToFill);
		
	}
}



void UDemoInventoryGrid::AddItemAtIndex(UInv_InventoryItem* Item, const int32 Index, const bool bStackable,
                                        const int32 StackAmount)
{
	const FInv_GridFragment*GridFragment=GetFragment<FInv_GridFragment>(Item,FragmentTags::GridFragment);
	const FInv_ImageFragment*ImageFragment=GetFragment<FInv_ImageFragment>(Item,FragmentTags::IconFragment);
	if (!GridFragment||!ImageFragment)return;
	UInv_SlottedItem*SlottedItem=CreateSlottedItem(Item,bStackable,StackAmount,GridFragment,ImageFragment,Index);
	AddSlottedItemToCanvas(Index,GridFragment,SlottedItem);
	SlottedItems.Add(Index,SlottedItem);
}
void UDemoInventoryGrid::OnSlottedItemClicked(int32 GridIndex, const FPointerEvent& MouseEvent)
{
	UInv_InventoryStatics::ItemUnHovered(GetOwningPlayer());
	check(GridSlots.IsValidIndex(GridIndex));
	UInv_InventoryItem*ClickedInventoryItem=GridSlots[GridIndex]->GetInventoryItem().Get();
	if (!IsValid(HoverItem)&&IsLeftClick(MouseEvent))
	{
		PickUp(ClickedInventoryItem,GridIndex);
		return;
	}
	if (IsRightClick(MouseEvent))
	{
		CreateItemPopUp(GridIndex);
		return;
	}
	if (IsSameStackable(ClickedInventoryItem))
	{
		const int32 ClickedStackCount=GridSlots[GridIndex]->GetStackCount();
		const FInv_StackableFragment*StackableFragment=ClickedInventoryItem->GetItemManifest().GetFragmentOfType<FInv_StackableFragment>();
		const int32 MaxStackSize=StackableFragment->GetMaxStackSize();
		const int32 RoomInClickedSlot=MaxStackSize-ClickedStackCount;
		const int32 HoverStackCount=HoverItem->GetStackCount();
		if (ShouldSwapStackCounts(RoomInClickedSlot,HoverStackCount,MaxStackSize))
		{

			SwapStackCounts(ClickedStackCount,HoverStackCount,GridIndex);
			return;
		}
		if (ShouldConsumeHoverItemStacks(HoverStackCount,RoomInClickedSlot))
		{

			ConsumeHoverItemStacks(ClickedStackCount,HoverStackCount,GridIndex);
			return;
		}
		if (ShouldFillInStack(RoomInClickedSlot,HoverStackCount))
		{

			FillInStack(RoomInClickedSlot,HoverStackCount-RoomInClickedSlot,GridIndex);
			return;
		}
		
		if (RoomInClickedSlot==0) return;
	}
	

	if (CurrentQueryResult.ValidItem.IsValid())
	{
		SwapWithHoverItem(ClickedInventoryItem,GridIndex);
	}
}
void UDemoInventoryGrid::PickUp(UInv_InventoryItem* ClickedInventoryItem, const int32 GridIndex)
{
	AssignHoverItem(ClickedInventoryItem,GridIndex,GridIndex);
	RemoveItemFromGrid(ClickedInventoryItem,GridIndex);
}

void UDemoInventoryGrid::AssignHoverItem(UInv_InventoryItem* Item, const int32 GridIndex, const int32 PreviousGridIndex)
{
	AssignHoverItem(Item);
	HoverItem->SetPreviousGridIndex(PreviousGridIndex);
	HoverItem->UpdateStackCount(Item->IsStackable()?GridSlots[GridIndex]->GetStackCount():0);
}

void UDemoInventoryGrid::RemoveItemFromGrid(UInv_InventoryItem* InventoryItem, const int32 GridIndex)
{
	const FInv_GridFragment*GridFragment=GetFragment<FInv_GridFragment>(InventoryItem,FragmentTags::GridFragment);
	if (!GridFragment)return;
	UInv_InventoryStatics::ForEach2D(GridSlots,GridIndex,GridFragment->GetGridSize(),Columns,[](UInv_GridSlot*GridSlot)
	{
		GridSlot->SetInventoryItem(nullptr);
		GridSlot->SetUpperLeftIndex(INDEX_NONE);
		GridSlot->SetUnoccupiedTexture();
		GridSlot->SetAvailable(true);
		GridSlot->SetStackCount(0);
	});
	if (SlottedItems.Contains(GridIndex))
	{
		TObjectPtr<UInv_SlottedItem>FoundSlottedItem;
		SlottedItems.RemoveAndCopyValue(GridIndex,FoundSlottedItem);
		FoundSlottedItem->RemoveFromParent();
	}
}

void UDemoInventoryGrid::CreateItemPopUp(const int32 GridIndex)
{
	UInv_InventoryItem*RightClickedItem=GridSlots[GridIndex]->GetInventoryItem().Get();
	if (!IsValid(RightClickedItem))return;
	if (IsValid(GridSlots[GridIndex]->GetItemPopUp()))return;
	
	ItemPopUp=CreateWidget<UInv_ItemPopUp>(this,ItemPopUpClass);
	GridSlots[GridIndex]->SetItemPopUp(ItemPopUp);
	
	OwningCanvasPanel->AddChild(ItemPopUp);
	UCanvasPanelSlot*CanvasSlot=UWidgetLayoutLibrary::SlotAsCanvasSlot(ItemPopUp);
	const FVector2D MousePosition=UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer());
	CanvasSlot->SetPosition(MousePosition-ItemPopUpOffset);
	CanvasSlot->SetSize(ItemPopUp->GetBoxSize());
	
	const int32 SliderMax=GridSlots[GridIndex]->GetStackCount()-1;
	if (RightClickedItem->IsStackable()&&SliderMax>0)
	{
		ItemPopUp->OnSplit.BindDynamic(this,&ThisClass::OnPopUpMenuSplit);
		ItemPopUp->SetSliderParams(SliderMax,FMath::Max(1,GridSlots[GridIndex]->GetStackCount()/2));
	}
	else
	{
		ItemPopUp->CollapseSplitButton();
	}
	ItemPopUp->OnDrop.BindDynamic(this,&ThisClass::OnPopUpMenuDrop);
	if (RightClickedItem->IsConsumable())
	{
		ItemPopUp->OnConsume.BindDynamic(this,&ThisClass::OnPopUpMenuConsume);
	}
	else
	{
		ItemPopUp->CollapseConsumeButton();
	}
	
}



bool UDemoInventoryGrid::CursorExitedCanvas(const FVector2D& BoundaryPos, const FVector2D& BoundarySize,
                                            const FVector2D& Location)
{
	bLastMouseWithinCanvas=bMouseWithinCanvas;
	bMouseWithinCanvas=UInv_WidgetUtils::IsWithinBounds(BoundaryPos,BoundarySize,Location);
	if (!bMouseWithinCanvas&&bLastMouseWithinCanvas)
	{
		UnHighlightSlots(LastHighlightedIndex,LastHighlightedDimensions);
	}
	return false;
}

void UDemoInventoryGrid::HighlightSlots(const int32 Index, const FIntPoint& Dimensions)
{
	if (!bMouseWithinCanvas) return;
	UnHighlightSlots(LastHighlightedIndex,LastHighlightedDimensions);
	UInv_InventoryStatics::ForEach2D(GridSlots,Index,Dimensions,Columns,[]
		(UInv_GridSlot*GridSlot)
	{
		GridSlot->SetOccupiedTexture();
	});
	LastHighlightedDimensions=Dimensions;
	LastHighlightedIndex=Index;
}

void UDemoInventoryGrid::UnHighlightSlots(const int32 Index, const FIntPoint& Dimensions)
{
	UInv_InventoryStatics::ForEach2D(GridSlots,Index,Dimensions,Columns,[]
		(UInv_GridSlot*GridSlot)
	{
		if (GridSlot->IsAvailable())
		{
			GridSlot->SetUnoccupiedTexture();
		}
		else
		{
			GridSlot->SetOccupiedTexture();
		}
	});
}

void UDemoInventoryGrid::ChangeHoverType(const int32 Index, const FIntPoint& Dimensions, EGridSlotSlate GridSlotSlate)
{
	UnHighlightSlots(LastHighlightedIndex,LastHighlightedDimensions);
	UInv_InventoryStatics::ForEach2D(GridSlots,Index,Dimensions,Columns,[State=GridSlotSlate](UInv_GridSlot*GridSlot)
	{
		switch(State)
		{
		case EGridSlotSlate::Occupied:
			GridSlot->SetOccupiedTexture();
			break;
		case EGridSlotSlate::Unoccupied:
			GridSlot->SetUnoccupiedTexture();
			break;
		case EGridSlotSlate::GrayedOut:
			GridSlot->SetGrayedOutTexture();
			break;
		case EGridSlotSlate::Selected:
			GridSlot->SetSelectedTexture();
			break;
		}
	});
	
	LastHighlightedIndex=Index;
	LastHighlightedDimensions=Dimensions;
}

void UDemoInventoryGrid::PutDownOnIndex(const int32 Index)
{
	AddItemAtIndex(HoverItem->GetInventoryItem(),Index,HoverItem->IsStackable(),HoverItem->GetStackCount());
	UpdateGridSlots(HoverItem->GetInventoryItem(),Index,HoverItem->IsStackable(),HoverItem->GetStackCount());
	ClearHoverItem();
}

UUserWidget* UDemoInventoryGrid::GetVisibleCursorWidget()
{
	if (!IsValid(GetOwningPlayer())) return nullptr;
	if (!IsValid(VisibleCursorWidget))
	{
		VisibleCursorWidget=CreateWidget<UUserWidget>(GetOwningPlayer(),VisibleCursorWidgetClass);
	}
	return VisibleCursorWidget;
}

UUserWidget* UDemoInventoryGrid::GetHiddenCursorWidget()
{
	if (!IsValid(GetOwningPlayer())) return nullptr;
	if (!IsValid(HiddenCursorWidget))
	{
		HiddenCursorWidget=CreateWidget<UUserWidget>(GetOwningPlayer(),HiddenCursorWidgetClass);
	}
	return HiddenCursorWidget;
}

bool UDemoInventoryGrid::IsSameStackable(const UInv_InventoryItem* ClickedInventoryItem) const
{
	const bool bIsSameItem=ClickedInventoryItem==HoverItem->GetInventoryItem();
	const bool bIsStackable=ClickedInventoryItem->IsStackable();
	return bIsSameItem && bIsStackable&&ClickedInventoryItem->GetItemManifest().GetItemType().MatchesTagExact(HoverItem->GetItemType());
}

void UDemoInventoryGrid::SwapWithHoverItem(UInv_InventoryItem* ClickedInventoryItem, const int32 GridIndex)
{
	if (!IsValid(HoverItem)) return;
	UInv_InventoryItem *TempInventoryItem=HoverItem->GetInventoryItem();
	const int32 TempStackCount=HoverItem->GetStackCount();
	const bool bTempIsStackable=HoverItem->IsStackable();
	

	AssignHoverItem(ClickedInventoryItem,GridIndex,HoverItem->GetPreviousGridIndex());
	RemoveItemFromGrid(ClickedInventoryItem,GridIndex);
	AddItemAtIndex(TempInventoryItem,ItemDropIndex,bTempIsStackable,TempStackCount);
	UpdateGridSlots(TempInventoryItem,ItemDropIndex,bTempIsStackable,TempStackCount);
}

bool UDemoInventoryGrid::ShouldSwapStackCounts(const int32 RoomInClickedSlot, const int32 HoverStackCount,
	const int32 MaxStackSize) const
{
	return RoomInClickedSlot==0&&HoverStackCount<MaxStackSize;
}

void UDemoInventoryGrid::SwapStackCounts(const int32 ClickedStackCount, const int32 HoverStackCount, const int32 Index)
{
	UInv_GridSlot*GridSlot=GridSlots[Index];
	GridSlot->SetStackCount(HoverStackCount);
	UInv_SlottedItem*ClickedItem=SlottedItems.FindChecked(Index);
	ClickedItem->UpdateStackCount(HoverStackCount);
	HoverItem->UpdateStackCount(ClickedStackCount);
}

bool UDemoInventoryGrid::ShouldConsumeHoverItemStacks(const int32 HoveredStackCount,
	const int32 RoomInClickedSlot) const
{
	return RoomInClickedSlot>=HoveredStackCount;
}

void UDemoInventoryGrid::ConsumeHoverItemStacks(const int32 ClickedStackCount, const int32 HoverStackCount,
	const int32 Index)
{
	const int32 AmountTransfer=HoverStackCount;
	const int32 NewClickedStackCount=ClickedStackCount+AmountTransfer;
	GridSlots[Index]->SetStackCount(NewClickedStackCount);
	SlottedItems.FindChecked(Index)->UpdateStackCount(NewClickedStackCount);
	ClearHoverItem();
	ShowCursor();
	
	const FInv_GridFragment*GridFragment=GridSlots[Index]->GetInventoryItem()->GetItemManifest().GetFragmentOfType<FInv_GridFragment>();
	const FIntPoint Dimensions=GridFragment?GridFragment->GetGridSize():FIntPoint(1,1);
	HighlightSlots(Index,Dimensions);
}

bool UDemoInventoryGrid::ShouldFillInStack(const int32 RoomInClickedSlot, const int32 HoverStackCount) const
{
	return RoomInClickedSlot<HoverStackCount;
}

void UDemoInventoryGrid::FillInStack(const int32 FillAmount, const int32 Remainder, const int32 Index)
{
	UInv_GridSlot*GridSlot=GridSlots[Index];
	const int32 NewStackCount=GridSlot->GetStackCount()+FillAmount;
	GridSlots[Index]->SetStackCount(NewStackCount);
	
	UInv_SlottedItem*ClickedSlottedItem=SlottedItems.FindChecked(Index);
	ClickedSlottedItem->UpdateStackCount(NewStackCount);
	HoverItem->UpdateStackCount(Remainder);
}

void UDemoInventoryGrid::PutHoverItemBack()
{
	if (!IsValid(HoverItem))return;
	FInv_SlotAvailabilityResult Result=HasRoomForItem(HoverItem->GetInventoryItem(),HoverItem->GetStackCount());
	Result.Item=HoverItem->GetInventoryItem();
	AddStacks(Result);
	ClearHoverItem();
}

void UDemoInventoryGrid::UpdateGridSlots(UInv_InventoryItem* NewItem, const int32 Index, bool bStackableItem,
                                         const int32 StackAmount)
{
	check(GridSlots.IsValidIndex(Index));
	if (bStackableItem)
	{
		GridSlots[Index]->SetStackCount(StackAmount);
	}
	const FInv_GridFragment* GridFragment=GetFragment<FInv_GridFragment>(NewItem,FragmentTags::GridFragment);
	const FIntPoint Dimensions= GridFragment?GridFragment->GetGridSize():FIntPoint(1,1);
	UInv_InventoryStatics::ForEach2D(GridSlots,Index,Dimensions,Columns,
		[&](UInv_GridSlot*GridSlot)
		{
			GridSlot->SetInventoryItem(NewItem);
			GridSlot->SetUpperLeftIndex(Index);
			GridSlot->SetOccupiedTexture();
			GridSlot->SetAvailable(false);
		});
}
UInv_SlottedItem* UDemoInventoryGrid::CreateSlottedItem(UInv_InventoryItem* Item, const bool bStackable,
	const int32 StackAmount, const FInv_GridFragment* GridFragment, const FInv_ImageFragment* ImageFragment,
	const int32 Index)
{
	UInv_SlottedItem*SlottedItem=CreateWidget<UInv_SlottedItem>(GetOwningPlayer(),SlottedItemClass);
	SlottedItem->SetInventoryItem(Item);
	SetSlottedItemImage(SlottedItem,GridFragment,ImageFragment);
	SlottedItem->SetGridIndex(Index);
	SlottedItem->SetIsStackable(bStackable);
	
	const int32 StackUpdateAmount=bStackable?StackAmount:0;
	SlottedItem->UpdateStackCount(StackUpdateAmount);

	SlottedItem->OnSlottedItemClicked.AddDynamic(this,&ThisClass::OnSlottedItemClicked);
	
	return SlottedItem;
}

void UDemoInventoryGrid::AddSlottedItemToCanvas(const int32 Index, const FInv_GridFragment* GridFragment,
	UInv_SlottedItem* SlottedItem) const
{
	CanvasPanel->AddChild(SlottedItem);
	UCanvasPanelSlot*CanvasSlot=UWidgetLayoutLibrary::SlotAsCanvasSlot(SlottedItem);
	CanvasSlot->SetSize(GetDrawSize(GridFragment));
	const FVector2D DrawPos=UInv_WidgetUtils::GetPositionFromIndex(Index,Columns)*TileSize;
	const FVector2D DrawPosWithPadding=DrawPos+FVector2D(GridFragment->GetGridPadding());
	CanvasSlot->SetPosition(DrawPosWithPadding);
}

void UDemoInventoryGrid::SetSlottedItemImage(const UInv_SlottedItem* SlottedItem, const FInv_GridFragment* GridFragment,
                                             const FInv_ImageFragment* ImageFragment) const
{
	FSlateBrush Brush;
	Brush.SetResourceObject(ImageFragment->GetIcon());
	Brush.DrawAs=ESlateBrushDrawType::Image;
	Brush.ImageSize=GetDrawSize(GridFragment);
	SlottedItem->SetImageBrush(Brush);
}

FVector2D UDemoInventoryGrid::GetDrawSize(const FInv_GridFragment* GridFragment) const
{
	const float IconTileWidth=TileSize-GridFragment->GetGridPadding()*2;
	FVector2D IconSize=GridFragment->GetGridSize()*IconTileWidth;
	return IconSize;
}

bool UDemoInventoryGrid::IsIndexClaimed(const TSet<int32> CheckIndices, const int32 Index)const
{
	return CheckIndices.Contains(Index);
}

bool UDemoInventoryGrid::IsInGridBounds(const int32 StartIndex, const FIntPoint& ItemDimension)
{
	if (StartIndex<0||StartIndex>=GridSlots.Num()) return false;
	const int32 EndColumn=(StartIndex%Columns)+ItemDimension.X;
	const int32 EndRow=(StartIndex/Columns)+ItemDimension.Y;
	return EndColumn<=Columns&&EndRow<=Rows;
}

FIntPoint UDemoInventoryGrid::GetItemDimensions(const FInv_ItemManifest& Manifest)
{
	const FInv_GridFragment*GridFragment=Manifest.GetFragmentOfType<FInv_GridFragment>();
	return GridFragment->GetGridSize();
}

bool UDemoInventoryGrid::HasRoomAtIndex(const UInv_GridSlot* GridSlot, const FIntPoint& Dimensions,
	const TSet<int32>& CheckedIndices, TSet<int32>& OutTentativelyClaimed, const FGameplayTag& ItemType,
	const int32 MaxStackSize)
{
	bool bHasRoomAtIndex=true;
	UInv_InventoryStatics::ForEach2D(GridSlots,GridSlot->GetIndex(),Dimensions,Columns,
		[&](const UInv_GridSlot* SubGridSlot)
	{
		if (CheckSlotConstraints(GridSlot,SubGridSlot,CheckedIndices,OutTentativelyClaimed,ItemType,MaxStackSize))
		{
			OutTentativelyClaimed.Add(SubGridSlot->GetIndex());
		}
		else
		{
			bHasRoomAtIndex=false;
		}
	});
	
	
	
	return bHasRoomAtIndex;
}

bool UDemoInventoryGrid::CheckSlotConstraints(const UInv_GridSlot* GridSlot, const UInv_GridSlot* SubGridSlot,
	const TSet<int32>& CheckedIndices, TSet<int32>& OutTentativelyClaimed, const FGameplayTag& ItemType,
	const int32 MaxStackSize) const
{
	if (IsIndexClaimed(CheckedIndices,SubGridSlot->GetIndex()))return false;
	if (!HasValidItem(SubGridSlot))
	{
		OutTentativelyClaimed.Add(SubGridSlot->GetIndex());
		return true;
	}
	if (!IsUpperleftSlot(GridSlot,SubGridSlot))return false;
	
	UInv_InventoryItem*SubItem=SubGridSlot->GetInventoryItem().Get();
	if (!SubItem->IsStackable())return false;
	if (!DoesItemTypeMatch(SubItem,ItemType))return false;
	if (GridSlot->GetStackCount()>=MaxStackSize)return false;
	
	return true;
}

bool UDemoInventoryGrid::HasValidItem(const UInv_GridSlot* GridSlot) const
{
	return GridSlot->GetInventoryItem().IsValid();
}

bool UDemoInventoryGrid::IsUpperleftSlot(const UInv_GridSlot* GridSlot, const UInv_GridSlot* SubGridSlot) const
{
	return GridSlot->GetIndex()==SubGridSlot->GetUpperLeftIndex();
}

bool UDemoInventoryGrid::DoesItemTypeMatch(const UInv_InventoryItem* InventoryItem, const FGameplayTag& ItemType)const
{
	return InventoryItem->GetItemManifest().GetItemType().MatchesTagExact(ItemType);
}

int32 UDemoInventoryGrid::DetermineFillAmountForSlot(const bool bStackable, const int32 MaxStackSize,
	const int32 AmountToFill, UInv_GridSlot* GridSlot) const
{
	const int32 RoomInSlot=MaxStackSize-GetStackAmount(GridSlot);
	return bStackable ? FMath::Min(AmountToFill,RoomInSlot):1;
}

int32 UDemoInventoryGrid::GetStackAmount(const UInv_GridSlot* GridSlot) const
{
	int32 CurrentSlotStackCount=GridSlot->GetStackCount();
	if (const int32 UpperLeftIndex=GridSlot->GetUpperLeftIndex();UpperLeftIndex!=INDEX_NONE)
	{
		UInv_GridSlot*UpperLeftGridSlot=GridSlots[UpperLeftIndex];
		CurrentSlotStackCount=UpperLeftGridSlot->GetStackCount();
	}
	return CurrentSlotStackCount;
}

bool UDemoInventoryGrid::IsRightClick(const FPointerEvent& MouseEvent) const
{
	return MouseEvent.GetEffectingButton()==EKeys::RightMouseButton;
}

bool UDemoInventoryGrid::IsLeftClick(const FPointerEvent& MouseEvent) const
{
	return MouseEvent.GetEffectingButton()==EKeys::LeftMouseButton;
}



void UDemoInventoryGrid::AddStacks(const FInv_SlotAvailabilityResult& Result)
{
	if (!MatchesCategory(Result.Item.Get()))return;
	for (const auto &Availability:Result.SlotAvailabilities)
	{
		if (Availability.bItemAtIndex)
		{
			const auto&GridSlot=GridSlots[Availability.Index];
			const auto &SlottedItem=SlottedItems.FindChecked(Availability.Index);
			SlottedItem->UpdateStackCount(GridSlot->GetStackCount()+Availability.AmountToFill);
			GridSlot->SetStackCount(GridSlot->GetStackCount()+Availability.AmountToFill);
		}
		else
		{
			AddItemAtIndex(Result.Item.Get(),Availability.Index,Result.bStackable,Availability.AmountToFill);
			UpdateGridSlots(Result.Item.Get(),Availability.Index,Result.bStackable,Availability.AmountToFill);
		}
	}
}




void UDemoInventoryGrid::OnPopUpMenuSplit(int32 SplitAmount, int32 Index)
{
	UInv_InventoryItem*RightClickedItem=GridSlots[Index]->GetInventoryItem().Get();
	if (!IsValid(RightClickedItem)) return;
	if (!RightClickedItem->IsStackable()) return;
	
	const int32 UpperLeftIndex=GridSlots[Index]->GetUpperLeftIndex();
	UInv_GridSlot*UpperLeftGridSlot=GridSlots[UpperLeftIndex];
	const int32 StackCount=UpperLeftGridSlot->GetStackCount();
	const int32 NewStackCount=StackCount-SplitAmount;
	
	UpperLeftGridSlot->SetStackCount(NewStackCount);
	SlottedItems.FindChecked(UpperLeftIndex)->UpdateStackCount(NewStackCount);
	AssignHoverItem(RightClickedItem,UpperLeftIndex,UpperLeftIndex);
	HoverItem->UpdateStackCount(SplitAmount);
}

void UDemoInventoryGrid::OnPopUpMenuDrop(int32 Index)
{
	UInv_InventoryItem *RightClickedItem=GridSlots[Index]->GetInventoryItem().Get();
	if (!IsValid(RightClickedItem)) return;
	PickUp(RightClickedItem,Index);
	DropItem();
	
}

void UDemoInventoryGrid::OnPopUpMenuConsume(int32 Index)
{
	UInv_InventoryItem*RightClickedItem=GridSlots[Index]->GetInventoryItem().Get();
	if (!IsValid(RightClickedItem)) return;
	const int32 UpperLeftIndex=GridSlots[Index]->GetUpperLeftIndex();
	UInv_GridSlot*UpperLeftGridSlot=GridSlots[UpperLeftIndex];
	const int32 NewStackCount=UpperLeftGridSlot->GetStackCount()-1;
	UpperLeftGridSlot->SetStackCount(NewStackCount);
	SlottedItems.FindChecked(UpperLeftIndex)->UpdateStackCount(NewStackCount);
	InventoryComponent->Server_ConsumeItem(RightClickedItem);
	if (NewStackCount<=0)
	{
		RemoveItemFromGrid(RightClickedItem,UpperLeftIndex);
	}
}


void UDemoInventoryGrid::ConstructGrid()
{
	GridSlots.Reserve(Rows*Columns);
	for (int32 j=0;j<Rows;++j)
	{
		for (int32 i=0;i<Columns;++i)
		{
			UInv_GridSlot*GridSlot=CreateWidget<UInv_GridSlot>(this,GridSlotClass);
			CanvasPanel->AddChild(GridSlot);
			const FIntPoint TilePosition(i,j);
			GridSlot->SetIndex(UInv_WidgetUtils::GetIndexFromPosition(TilePosition,Columns));
			UCanvasPanelSlot*GridCPS=UWidgetLayoutLibrary::SlotAsCanvasSlot(GridSlot);
			
			GridCPS->SetSize(FVector2D(TileSize));
			GridCPS->SetPosition(TilePosition*TileSize);
			GridSlots.Add(GridSlot);
			
			GridSlot->GridSlotClicked.AddDynamic(this,&UDemoInventoryGrid::OnGridSlotClicked);
			GridSlot->GridSlotHovered.AddDynamic(this,&UDemoInventoryGrid::OnGridSlotHovered);
			GridSlot->GridSlotUnhovered.AddDynamic(this,&UDemoInventoryGrid::OnGridSlotUnhovered);
		}
	}
}
void UDemoInventoryGrid::OnGridSlotClicked(int32 GridIndex, const FPointerEvent& MouseEvent)
{
	if (!IsValid(HoverItem)) return;
	
	if (!GridSlots.IsValidIndex(ItemDropIndex)) return;
	
	if (CurrentQueryResult.ValidItem.IsValid()&&GridSlots.IsValidIndex(CurrentQueryResult.UpperLeftIndex))
	{
		OnSlottedItemClicked(CurrentQueryResult.UpperLeftIndex,MouseEvent);
		return;
	}
	if (!IsInGridBounds(ItemDropIndex,HoverItem->GetGridDimension()))return;
	
	auto GridSlot=GridSlots[ItemDropIndex];
	if (!GridSlot->GetInventoryItem().IsValid())
	{
		PutDownOnIndex(ItemDropIndex);
	}
}

void UDemoInventoryGrid::OnGridSlotHovered(int32 GridIndex, const FPointerEvent& MouseEvent)
{
	if (IsValid(HoverItem)) return;
	UInv_GridSlot*GridSlot=GridSlots[GridIndex];
	if (GridSlot->IsAvailable())
	{
		GridSlot->SetOccupiedTexture();
	}
}

void UDemoInventoryGrid::OnGridSlotUnhovered(int32 GridIndex, const FPointerEvent& MouseEvent)
{
	if (IsValid(HoverItem)) return;
	UInv_GridSlot*GridSlot=GridSlots[GridIndex];
	if (GridSlot->IsAvailable())
	{
		GridSlot->SetUnoccupiedTexture();
	}
}


