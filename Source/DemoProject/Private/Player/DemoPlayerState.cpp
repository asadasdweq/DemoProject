// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/DemoPlayerState.h"

#include "AbilitySystem/DemoAbilitySystemComponent.h"
#include "AbilitySystem/DemoAttributeSet.h"


ADemoPlayerState::ADemoPlayerState()
{
	SetNetUpdateFrequency(100.f);
	AbilitySystemComponent = CreateDefaultSubobject<UDemoAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	AttributeSet = CreateDefaultSubobject<UDemoAttributeSet>("AttributeSet");
}
UAbilitySystemComponent* ADemoPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
