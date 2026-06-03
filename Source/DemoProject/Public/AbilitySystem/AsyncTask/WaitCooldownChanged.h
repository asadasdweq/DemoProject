// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include"GameplayTagContainer.h"
#include"ActiveGameplayEffectHandle.h"
#include "WaitCooldownChanged.generated.h"

class UAbilitySystemComponent;
struct FGameplayEffectSpec;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCooldownChangedSignature, float, TimeRemaining);
UCLASS(BlueprintType, meta = (ExposedAsyncProxy="AsyncTask"))
class DEMOPROJECT_API UWaitCooldownChanged : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FCooldownChangedSignature CooldownStart;
	UPROPERTY(BlueprintAssignable)
	FCooldownChangedSignature CooldownEnd;
	UFUNCTION(BlueprintCallable,meta=(BlueprintInternalUseOnly="true"))
	static UWaitCooldownChanged* WaitCooldownChanged(UAbilitySystemComponent*AbilitySystemComponent, const FGameplayTag &InCooldownTag);
	UFUNCTION(BlueprintCallable)
	void EndTask();
protected:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent>ASC;
	
	FGameplayTag CooldownTag;
	
	void CooldownTagChanged(const FGameplayTag InCooldownTag,int32 NewCount);
	
	void OnActiveEffectAdded(UAbilitySystemComponent*TargetASC,const FGameplayEffectSpec&SpecApplied,FActiveGameplayEffectHandle ActiveEffectHandle);
		
};
