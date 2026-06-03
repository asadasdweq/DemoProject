// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/OverlayWidgetController.h"
#include"AbilitySystem/DemoAbilitySystemComponent.h"
#include "AbilitySystem/DemoAttributeSet.h"

void UOverlayWidgetController::BroadcastInitialValue()
{
	Super::BroadcastInitialValue();
	OnHealthChanged.Broadcast(GetDemoAS()->GetHealth());
	OnManaChanged.Broadcast(GetDemoAS()->GetMana());
	OnMaxHealthChanged.Broadcast(GetDemoAS()->GetMaxHealth());
	OnMaxManaChanged.Broadcast(GetDemoAS()->GetMaxMana());
}

void UOverlayWidgetController::BindCallbacksDependencies()
{
	Super::BindCallbacksDependencies();
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetDemoAS()->GetHealthAttribute()).AddLambda
	([this](const FOnAttributeChangeData& Data)
	{
		OnHealthChanged.Broadcast(Data.NewValue);
	});
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetDemoAS()->GetManaAttribute()).AddLambda
	([this](const FOnAttributeChangeData& Data)
	{
		OnManaChanged.Broadcast(Data.NewValue);
	});
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetDemoAS()->GetMaxHealthAttribute()).AddLambda
	([this](const FOnAttributeChangeData& Data)
	{
		OnMaxHealthChanged.Broadcast(Data.NewValue);
	});
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetDemoAS()->GetMaxManaAttribute()).AddLambda
	([this](const FOnAttributeChangeData& Data)
	{
		OnMaxManaChanged.Broadcast(Data.NewValue);
	});
	
	if (GetDemoASC())
	{
		if (GetDemoASC()->bStartupAbilitiesGiven)
		{
			BroadAbilityInfo();
			GetDemoASC()->AbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::BroadAbilityInfo);
		}
		else
		{
			GetDemoASC()->AbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::BroadAbilityInfo);
		}
	}
	
}


