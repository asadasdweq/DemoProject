// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/DemoWidgetController.h"
#include "AbilitySystem/DemoAbilitySystemComponent.h"
#include "AbilitySystem/DemoAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/DemoPlayerController.h"
#include "Player/DemoPlayerState.h"

void UDemoWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& Params)
{
	AbilitySystemComponent=Params.AbilitySystemComponent;
	PlayerController=Params.PlayerController;
	PlayerState=Params.PlayerState;
	AttributeSet=Params.AttributeSet;
}

void UDemoWidgetController::BroadcastInitialValue()
{
	
}

void UDemoWidgetController::BindCallbacksDependencies()
{
}

void UDemoWidgetController::BroadAbilityInfo()
{
	if (!GetDemoASC()->bStartupAbilitiesGiven)return;
	FForEachAbility ForEachDelegate;
	ForEachDelegate.BindLambda([this](const FGameplayAbilitySpec& AbilitySpec)
	{
		FDemoAbilityInfo Info=AbilityInfo->FindAbilityInfoForTag(DemoAbilitySystemComponent->GetAbilityTagFromSpec(AbilitySpec));
		Info.InputTag = DemoAbilitySystemComponent->GetInputTagFromSpec(AbilitySpec);
		AbilityInfoDelegate.Broadcast(Info);
	});
	GetDemoASC()->ForEachAbility(ForEachDelegate);
}


ADemoPlayerController* UDemoWidgetController::GetDemoPC()
{
	if (DemoPlayerController==nullptr)
	{
		DemoPlayerController=Cast<ADemoPlayerController>(PlayerController);
	}
	return DemoPlayerController;
}

ADemoPlayerState* UDemoWidgetController::GetDemoPS()
{
	if (DemoPlayerState==nullptr)
	{
		DemoPlayerState=Cast<ADemoPlayerState>(PlayerState);
	}
	return DemoPlayerState;
}

UDemoAbilitySystemComponent* UDemoWidgetController::GetDemoASC()
{
	if (DemoAbilitySystemComponent==nullptr)
	{
		DemoAbilitySystemComponent=Cast<UDemoAbilitySystemComponent>(AbilitySystemComponent);
	}
	return DemoAbilitySystemComponent;
}

UDemoAttributeSet* UDemoWidgetController::GetDemoAS()
{
	if (DemoAttributeSet==nullptr)
	{
		DemoAttributeSet=Cast<UDemoAttributeSet>(AttributeSet);
	}
	return DemoAttributeSet;
}
