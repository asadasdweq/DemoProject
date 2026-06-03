// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/LightingAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/DemoAbilitySystemLibrary.h"



void ULightingAbility::SpawnLightningEffect(const FVector &SpawnLocation)
{
	const bool bIsServer=GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsServer)return;
	
	
	AActor* Caster = GetAvatarActorFromActorInfo();
	UAbilitySystemComponent*SourceASC=UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Caster);

	TArray<AActor*> ActorsToCauseDamage;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Caster);
	UDemoAbilitySystemLibrary::GetLivePlayerWithinRadius(this,ActorsToCauseDamage,ActorsToIgnore,LightningRadius,SpawnLocation);
	if (ActorsToCauseDamage.Num() > 0)
	{
		for (AActor* Actor : ActorsToCauseDamage)
		{
			if (!Actor)continue;
			if (UAbilitySystemComponent*TargetASC=UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor))
			{
				UDemoAbilitySystemLibrary::CauseDamage(SourceASC,TargetASC,DamageGameplayEffectClass,DamageType,BaseDamage.GetValueAtLevel(1.f));
			}
		}
	}
	if (LightningEffectClass) 
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Instigator=Cast<APawn>(Caster);

		GetWorld()->SpawnActor<AActor>(
			LightningEffectClass,	
			SpawnLocation,			
			FRotator::ZeroRotator,	
			SpawnParams				
		);
	}
}

 
