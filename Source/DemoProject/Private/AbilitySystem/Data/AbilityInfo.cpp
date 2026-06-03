// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/AbilityInfo.h"

FDemoAbilityInfo UAbilityInfo::FindAbilityInfoForTag(const FGameplayTag& Tag) const
{
	for (const FDemoAbilityInfo&Info:AbilityInformation)
	{
		if (Info.AbilityTag==Tag)
		{
			return Info;
		}
	}
	return FDemoAbilityInfo();
}
