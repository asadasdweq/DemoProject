// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "DemoAbilitySystemComponent.generated.h"
DECLARE_MULTICAST_DELEGATE(FAbilitiesGiven);
DECLARE_DELEGATE_OneParam(FForEachAbility,const FGameplayAbilitySpec&);
/**
 * 
 */
UCLASS()
class DEMOPROJECT_API UDemoAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>&StartupAbilities);
	
	void ForEachAbility(const FForEachAbility &Delegate);
	bool bStartupAbilitiesGiven=false;
	FAbilitiesGiven AbilitiesGivenDelegate;
	
	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	
protected:
	virtual void OnRep_ActivateAbilities() override;
	bool IsAbilityInputBlocked(const FGameplayAbilitySpec& AbilitySpec) const;
};
