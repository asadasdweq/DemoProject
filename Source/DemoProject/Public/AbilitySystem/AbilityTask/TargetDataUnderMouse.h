// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "TargetDataUnderMouse.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTargetDataUnderMouseDelegate, const FGameplayAbilityTargetDataHandle&, DataHandle);
/**
 * 
 */
UCLASS()
class DEMOPROJECT_API UTargetDataUnderMouse : public UAbilityTask
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category="AbilityTask",meta=(DisplayName="TargetDataUnderMouse",HidePin="OwningAbility",DefaultToSelf="OwningAbility",BlueprintInternalUseOnly=true))
	static UTargetDataUnderMouse*CreateTargetDataUnderMouse(UGameplayAbility*OwningAbility);
	
	UPROPERTY(BlueprintAssignable)
	FTargetDataUnderMouseDelegate ValidData;
	
private:
	virtual void Activate() override;
	void SendMouseCursorData();
	void OnTargetDataReplicatedCallBack(const FGameplayAbilityTargetDataHandle&DataHandle,FGameplayTag ActivationTag);
};
