// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/DemoElectrocuteAbility.h"

#include "AbilitySystem/DemoAbilitySystemLibrary.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/KismetSystemLibrary.h"

void UDemoElectrocuteAbility::StoreMouseHitInfo(const FHitResult& MouseHitResult)
{
	if (MouseHitResult.bBlockingHit)
	{
		MouseHitLocation=MouseHitResult.ImpactPoint;
		MouseHitActor=MouseHitResult.GetActor();
	}
	else
	{
		CancelAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true);
	}
}

void UDemoElectrocuteAbility::StoreOwnerVariables()
{
	if (CurrentActorInfo)
	{
		OwnerPlayerController=CurrentActorInfo->PlayerController.Get();
		OwnerCharacter=Cast<ACharacter>(CurrentActorInfo->AvatarActor);
	}
}

void UDemoElectrocuteAbility::TraceFirstTarget(const FVector& BeamTargetLocation)
{
	check(OwnerCharacter);
	if (OwnerCharacter->Implements<UCombatInterface>())
	{
		const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(OwnerCharacter);
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(OwnerCharacter);
		FHitResult HitResult;
		UKismetSystemLibrary::SphereTraceSingle(OwnerCharacter,SocketLocation,BeamTargetLocation,10.f
			,TraceTypeQuery1,false,ActorsToIgnore,EDrawDebugTrace::None,HitResult,true);
		if (HitResult.bBlockingHit)
		{
			MouseHitActor=HitResult.GetActor();
			MouseHitLocation=HitResult.ImpactPoint;
		}
		if (ICombatInterface*CombatInterface=Cast<ICombatInterface>(MouseHitActor))
		{
			if (!CombatInterface->GetOnDeathDel().IsAlreadyBound(this,&UDemoElectrocuteAbility::FirstTargetDied))
			{
				CombatInterface->GetOnDeathDel().AddDynamic(this,&UDemoElectrocuteAbility::FirstTargetDied);
			}
		}
	}
}

void UDemoElectrocuteAbility::StoreAdditionalTargets(TArray<AActor*> &AdditionalTargets)
{
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetAvatarActorFromActorInfo());
	ActorsToIgnore.Add(MouseHitActor);
	TArray<AActor*> OverlapActors;
	UDemoAbilitySystemLibrary::GetLivePlayerWithinRadius(GetAvatarActorFromActorInfo()
		,OverlapActors,ActorsToIgnore
		,800.f,MouseHitActor->GetActorLocation());
	int32 NumAdditionalTargets=NumShocked;
	UDemoAbilitySystemLibrary::GetClosestTargets(NumAdditionalTargets,OverlapActors,AdditionalTargets,MouseHitActor->GetActorLocation());
	for (AActor* Target : AdditionalTargets)
	{
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Target))
		{
			if (!CombatInterface->GetOnDeathDel().IsAlreadyBound(this, &UDemoElectrocuteAbility::AdditionalTargetDied))
			{
				CombatInterface->GetOnDeathDel().AddDynamic(this, &UDemoElectrocuteAbility::AdditionalTargetDied);
			}
		}
	}
	
}

