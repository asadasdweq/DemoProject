// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DemoGameplayAbility.h"
#include "DemoProjectileAbility.generated.h"

class ADemoFireBolt;
/**
 * 
 */
UCLASS()
class DEMOPROJECT_API UDemoProjectileAbility : public UDemoGameplayAbility
{
	GENERATED_BODY()
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	UFUNCTION(BlueprintCallable)
	void SpawnProjectile(const FVector&TargetLocation);
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Projectile")
	TSubclassOf<ADemoFireBolt>ProjectileClass;
	
};
