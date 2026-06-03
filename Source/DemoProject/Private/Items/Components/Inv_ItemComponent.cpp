// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Components/Inv_ItemComponent.h"

#include "Items/Fragments/Inv_ItemFragment.h"
#include "Net/UnrealNetwork.h"


UInv_ItemComponent::UInv_ItemComponent()
{
	
	PrimaryComponentTick.bCanEverTick = false;

	PickupMessage=FString("E  -  Pick Up");
	
	SetIsReplicatedByDefault(true);
	
}

void UInv_ItemComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass,ItemManifest);
}

void UInv_ItemComponent::InitItemManifest(FInv_ItemManifest CopyOfManifest)
{
	ItemManifest=CopyOfManifest;
}

void UInv_ItemComponent::SetStackCount(int32 StackCount)
{
	if (FInv_StackableFragment* StackableFragment = ItemManifest.GetFragmentOfTypeMutable<FInv_StackableFragment>())
	{
		StackableFragment->SetStackCount(FMath::Max(0, StackCount));
	}
}

void UInv_ItemComponent::PickedUp()
{
	OnPickedUp();
	GetOwner()->Destroy();
}









