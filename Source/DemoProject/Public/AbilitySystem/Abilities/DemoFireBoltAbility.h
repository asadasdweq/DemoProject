// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DemoGameplayAbility.h"
#include "DemoFireBoltAbility.generated.h"

class ADemoFireBolt;
/**
 * 
 */
UCLASS()
class DEMOPROJECT_API UDemoFireBoltAbility : public UDemoGameplayAbility
{
	GENERATED_BODY()
public:
	UDemoFireBoltAbility();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, 
	const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	UFUNCTION(BlueprintCallable)
	void SpawnFireBolt(const FVector&FireBoltTargetLocation,AActor*HomingTarget);
protected:

	UFUNCTION()
	void OnTargetDataReady(const FGameplayAbilityTargetDataHandle& DataHandle);
	
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Spawn Actor")
	TSubclassOf<ADemoFireBolt>FireBoltClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "FireBolt")
	float HomingAccelerationMin = 6500.f;

	UPROPERTY(EditDefaultsOnly, Category = "FireBolt")
	float HomingAccelerationMax = 7000.f;

	UPROPERTY(EditDefaultsOnly, Category = "FireBolt")
	bool bLaunchHomingProjectile = true;
};
