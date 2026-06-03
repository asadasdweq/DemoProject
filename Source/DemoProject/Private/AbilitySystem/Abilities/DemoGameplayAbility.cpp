// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/DemoGameplayAbility.h"


bool UDemoGameplayAbility::HasActivationOwnedTag(const FGameplayTag& Tag) const
{
	return ActivationOwnedTags.HasTagExact(Tag);
}

float UDemoGameplayAbility::GetDamageValue() const
{
	return BaseDamage.GetValueAtLevel(1.f);
}
