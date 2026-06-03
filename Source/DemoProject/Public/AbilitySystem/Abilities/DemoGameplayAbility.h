// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "DemoGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class DEMOPROJECT_API UDemoGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	FGameplayTag StartupInputTag;

	bool HasActivationOwnedTag(const FGameplayTag& Tag) const;
	
protected:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Damage")
	TSubclassOf<UGameplayEffect> DamageGameplayEffectClass;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Damage")
	FGameplayTag DamageType;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Damage")
	FScalableFloat BaseDamage;
	UFUNCTION(BlueprintCallable)
	float GetDamageValue() const;
};
