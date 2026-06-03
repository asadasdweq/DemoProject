// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/DemoInputConfig.h"

const UInputAction* UDemoInputConfig::FindAbilityInputActionForTag(const FGameplayTag& Tag, bool bNotFound) const
{
	for (const FDemoInputAction &Action:AbilityInputActions)
	{
		if (Action.InputAction&&Action.InputTag==Tag)
		{
			return Action.InputAction;
		}
		if (bNotFound)
		{
			UE_LOG(LogTemp,Error,TEXT("Cant't find input action for tag %s"),*Tag.ToString());
		}
	}
	return nullptr;
}
