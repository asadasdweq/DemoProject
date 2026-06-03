// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inv_PickUpMessage.generated.h"

class UInv_InfoMessage;
/**
 * 
 */
UCLASS()
class DEMOPROJECT_API UInv_PickUpMessage : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct()override;
	
	UFUNCTION(BlueprintImplementableEvent,Category="Inventory")
	void ShowPickupMessage(const FString&Message);
	
	UFUNCTION(BlueprintImplementableEvent,Category="Inventory")
	void HidePickupMessage();
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UInv_InfoMessage>InfoMessage;
	
	UFUNCTION()
	void OnNoRoom();
};
