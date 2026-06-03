// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTask/TargetDataUnderMouse.h"
#include"DemoProject/DemoProject.h"
#include "AbilitySystemComponent.h"

UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	UTargetDataUnderMouse *Obj=NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);
	return Obj;
}

void UTargetDataUnderMouse::Activate()
{
	Super::Activate();
	const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
	if (bIsLocallyControlled)
	{
		SendMouseCursorData();
	}
	else
	{
		const FGameplayAbilitySpecHandle SpecHandle=GetAbilitySpecHandle();
		FPredictionKey ActivationPredictionKey=GetActivationPredictionKey();
		AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey)
		.AddUObject(this, &UTargetDataUnderMouse::OnTargetDataReplicatedCallBack);
		const bool bCalledDelegate=AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey);
		if(!bCalledDelegate)
		{
			SetWaitingOnRemotePlayerData();
		}
	}
}

void UTargetDataUnderMouse::SendMouseCursorData()
{
	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());
	APlayerController *PC=Ability->GetCurrentActorInfo()->PlayerController.Get();
	FHitResult CursorHit;
	PC->GetHitResultUnderCursor(ECC_Target,false,CursorHit);
	FGameplayAbilityTargetDataHandle DataHandle;
	
	FGameplayAbilityTargetData_SingleTargetHit *Data=new FGameplayAbilityTargetData_SingleTargetHit();
	Data->HitResult=CursorHit;
	DataHandle.Add(Data);
	AbilitySystemComponent->ServerSetReplicatedTargetData
	(GetAbilitySpecHandle(),
		GetActivationPredictionKey(),
		DataHandle,
		FGameplayTag(),
		AbilitySystemComponent->ScopedPredictionKey);
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}

void UTargetDataUnderMouse::OnTargetDataReplicatedCallBack(const FGameplayAbilityTargetDataHandle& DataHandle,
	FGameplayTag ActivationTag)
{
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}

