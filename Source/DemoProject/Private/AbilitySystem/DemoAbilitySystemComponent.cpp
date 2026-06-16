// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/DemoAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/DemoGameplayAbility.h"
#include "DemoGameplayTags.h"

void UDemoAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())return;
	
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec &AbilitySpec:GetActivatableAbilities())
	{
		
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			if (IsAbilityInputBlocked(AbilitySpec))
			{
				continue;
			}
			AbilitySpecInputPressed(AbilitySpec);
			if (AbilitySpec.IsActive())
			{
				TArray<UGameplayAbility*> Instances = AbilitySpec.GetAbilityInstances();
				if (Instances.Num() == 0)
				{
					continue;
				}

				const FGameplayAbilityActivationInfo& ActivationInfo = Instances.Last()->GetCurrentActivationInfoRef();

				FPredictionKey OriginalPredictionKey = ActivationInfo.GetActivationPredictionKey();
                
				InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, AbilitySpec.Handle, OriginalPredictionKey);
			}
			
		}
	}
	
}

void UDemoAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	
	if (!InputTag.IsValid())return;
	
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec:GetActivatableAbilities())
	{
	   if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
	   {
	   	if (IsAbilityInputBlocked(AbilitySpec))
	   	{
	   		continue;
	   	}
	   	AbilitySpecInputPressed(AbilitySpec);
	   	if(!AbilitySpec.IsActive())
	   	{
	   		TryActivateAbility(AbilitySpec.Handle);
	   	}
	   }
	}
}

void UDemoAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())return;
	
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec &AbilitySpec:GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)&&AbilitySpec.IsActive())
		{
			AbilitySpecInputReleased(AbilitySpec);
			
			TArray<UGameplayAbility*> Instances = AbilitySpec.GetAbilityInstances();
			if (Instances.Num() == 0)
			{
				continue;
			}

			const FGameplayAbilityActivationInfo& ActivationInfo = Instances.Last()->GetCurrentActivationInfoRef();

			FPredictionKey OriginalPredictionKey = ActivationInfo.GetActivationPredictionKey();

			InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, AbilitySpec.Handle, OriginalPredictionKey);
		}
	}
	
}

void UDemoAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	for(const TSubclassOf<UGameplayAbility> AbilityClass: StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		if(const UDemoGameplayAbility * DemoAbility=Cast<UDemoGameplayAbility>(AbilitySpec.Ability))
		{
			AbilitySpec.DynamicAbilityTags.AddTag(DemoAbility->StartupInputTag);
			
			GiveAbility(AbilitySpec);
		}
	}
	bStartupAbilitiesGiven = true;
	AbilitiesGivenDelegate.Broadcast();
}

void UDemoAbilitySystemComponent::ForEachAbility(const FForEachAbility& Delegate)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (const FGameplayAbilitySpec& AbilitySpec:GetActivatableAbilities())
	{
		if (!Delegate.ExecuteIfBound(AbilitySpec))
		{
			UE_LOG(LogTemp, Error, TEXT("No bound delegate function in %hs"), __FUNCTION__);
		}
	}
}

FGameplayTag UDemoAbilitySystemComponent::GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability)
	{
		for (FGameplayTag Tag:AbilitySpec.Ability.Get()->AbilityTags)
		{
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities"))))
			{
				return Tag;
			}
		}
	}
	return FGameplayTag();
}

FGameplayTag UDemoAbilitySystemComponent::GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (FGameplayTag Tag:AbilitySpec.DynamicAbilityTags)
	{
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("InputTag"))))
		{
			return Tag;
		}
	}
	return FGameplayTag();
}

void UDemoAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();
	if(!bStartupAbilitiesGiven)
	{
		bStartupAbilitiesGiven = true;
		AbilitiesGivenDelegate.Broadcast();
	}
    
}

bool UDemoAbilitySystemComponent::IsAbilityInputBlocked(const FGameplayAbilitySpec& AbilitySpec) const
{
	const FGameplayTag AbilityInputBlockedTag = FDemoGameplayTags::Get().State_AbilityInputBlocked;
	if (!HasMatchingGameplayTag(AbilityInputBlockedTag))
	{
		return false;
	}

	const UDemoGameplayAbility* Ability = Cast<UDemoGameplayAbility>(AbilitySpec.Ability);
	const bool bAllowCurrentBlockingAbilityInput =
		AbilitySpec.IsActive() && Ability && Ability->HasActivationOwnedTag(AbilityInputBlockedTag);

	return !bAllowCurrentBlockingAbilityInput;
}
