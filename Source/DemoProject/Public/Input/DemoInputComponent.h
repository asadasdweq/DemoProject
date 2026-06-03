// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DemoInputConfig.h"
#include"GameplayTagContainer.h"
#include "EnhancedInputComponent.h"
#include "DemoInputComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DEMOPROJECT_API UDemoInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	template<class UserClass,typename PressedFuncType,typename ReleasedFuncType,typename HeldFuncType>
	void BindAbilityActions(const UDemoInputConfig*InputConfig,UserClass* Object,PressedFuncType PressedFunc,ReleasedFuncType ReleasedFunc,HeldFuncType HeldFunc);
};

template <class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
void UDemoInputComponent::BindAbilityActions(const UDemoInputConfig* InputConfig, UserClass* Object,
	PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, HeldFuncType HeldFunc)
{
	check(InputConfig);
	for (const FDemoInputAction&Action:InputConfig->AbilityInputActions)
	{
		if (Action.InputAction&&Action.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindAction(Action.InputAction,ETriggerEvent::Started,Object,PressedFunc,Action.InputTag);
			}
			if (ReleasedFunc)
			{
				BindAction(Action.InputAction,ETriggerEvent::Completed,Object,ReleasedFunc,Action.InputTag);
			}
			if (HeldFunc)
			{
				BindAction(Action.InputAction,ETriggerEvent::Triggered,Object,HeldFunc,Action.InputTag);
			}
			
			
		}
	}
}
