// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AsyncTask/WaitCooldownChanged.h"
#include"AbilitySystemComponent.h"
UWaitCooldownChanged* UWaitCooldownChanged::WaitCooldownChanged(UAbilitySystemComponent* AbilitySystemComponent,
	const FGameplayTag &InCooldownTag)
{
	UWaitCooldownChanged *WaitCooldownChanged=NewObject<UWaitCooldownChanged>();
	WaitCooldownChanged->ASC=AbilitySystemComponent;
	WaitCooldownChanged->CooldownTag=InCooldownTag;
	if (!IsValid(AbilitySystemComponent)||!InCooldownTag.IsValid())
	{
		WaitCooldownChanged->EndTask();
		return nullptr;
	}
	AbilitySystemComponent->RegisterGameplayTagEvent(InCooldownTag,EGameplayTagEventType::NewOrRemoved).AddUObject(WaitCooldownChanged
		,&UWaitCooldownChanged::CooldownTagChanged);
	AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(WaitCooldownChanged,&UWaitCooldownChanged::OnActiveEffectAdded);
	
	return WaitCooldownChanged;
}

void UWaitCooldownChanged::EndTask()
{
	if (!IsValid(ASC))return;
	ASC->RegisterGameplayTagEvent(CooldownTag,EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
	SetReadyToDestroy();
	MarkAsGarbage();
}

void UWaitCooldownChanged::CooldownTagChanged(const FGameplayTag InCooldownTag, int32 NewCount)
{
	if (NewCount==0)
	{
		CooldownEnd.Broadcast(0.f);
	}
}

void UWaitCooldownChanged::OnActiveEffectAdded(UAbilitySystemComponent* TargetASC, const FGameplayEffectSpec& SpecApplied,
	FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer AssetTag;
	SpecApplied.GetAllAssetTags(AssetTag);
	FGameplayTagContainer GrantedTag;
	SpecApplied.GetAllGrantedTags(GrantedTag);
	if (AssetTag.HasTagExact(CooldownTag)||GrantedTag.HasTagExact(CooldownTag))
	{
		FGameplayEffectQuery GameplayEffect=FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTag.GetSingleTagContainer());
		TArray<float>TimesRemaining=ASC->GetActiveEffectsTimeRemaining(GameplayEffect);
		if (TimesRemaining.Num()>0)
		{
			float TimeRemaining=TimesRemaining[0];
			for (int32 i=1;i<TimesRemaining.Num();i++)
			{
				if (TimesRemaining[i]>TimeRemaining)
				{
					TimeRemaining=TimesRemaining[i];
				}
			}
			CooldownStart.Broadcast(TimeRemaining);
		}
		
	}
}
