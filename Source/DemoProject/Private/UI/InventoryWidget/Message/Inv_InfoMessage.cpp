// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InventoryWidget/Message/Inv_InfoMessage.h"

#include "Components/TextBlock.h"

void UInv_InfoMessage::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	Text_Message->SetText(FText::GetEmpty());
}

void UInv_InfoMessage::SetMessage(const FText& Message)
{
	Text_Message->SetText(Message);
	if (!bIsMessageActive)
	{
		MessageShow();
	}
	bIsMessageActive = true;
	
	GetWorld()->GetTimerManager().SetTimer(MessageTimer,[this]()
	{
		MessageHide();
		bIsMessageActive = false;
	},MessageLifetime,false);
	
}
