// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DemoWidgetController.h"
#include "OverlayWidgetController.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChanged,float,NewValue);
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class DEMOPROJECT_API UOverlayWidgetController : public UDemoWidgetController
{
	GENERATED_BODY()
public:
	virtual void BroadcastInitialValue()override;
	virtual void BindCallbacksDependencies() override;
	
	
	
	FOnAttributeChanged OnHealthChanged;
	FOnAttributeChanged OnManaChanged;
	FOnAttributeChanged OnMaxHealthChanged;
	FOnAttributeChanged OnMaxManaChanged;
};
