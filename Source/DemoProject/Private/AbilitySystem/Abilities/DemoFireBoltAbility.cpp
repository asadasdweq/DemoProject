// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/DemoFireBoltAbility.h"
#include"AbilitySystemComponent.h"
#include "AbilitySystem/AbilityTask/TargetDataUnderMouse.h"
#include "Actors/DemoFireBolt.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Interaction/CombatInterface.h"

UDemoFireBoltAbility::UDemoFireBoltAbility()
{

	
	
}

void UDemoFireBoltAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	
	
	
}

void UDemoFireBoltAbility::InputPressed(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
	
	
	/*
	if (!CommitAbilityCooldown(Handle, ActorInfo, ActivationInfo, true))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}


	if (!CommitAbilityCost(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}*/
	/*UTargetDataUnderMouse* TargetTask = UTargetDataUnderMouse::CreateTargetDataUnderMouse(this);
	TargetTask->ValidData.AddDynamic(this, &UDemoFireBoltAbility::OnTargetDataReady);
	TargetTask->ReadyForActivation();*/
	
}

void UDemoFireBoltAbility::SpawnFireBolt(const FVector&FireBoltTargetLocation,AActor*HomingTarget)
{
	const bool bIsServer=GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsServer)return;
	const FVector SocketLocation=ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo());
	FRotator Rotation=(FireBoltTargetLocation - SocketLocation).Rotation();
	Rotation.Pitch=45.f;
	//const FVector Forward=Rotation.Vector();
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SocketLocation);
	SpawnTransform.SetRotation(Rotation.Quaternion());//
	ADemoFireBolt*DemoFireBolt=GetWorld()->SpawnActorDeferred<ADemoFireBolt>(FireBoltClass 
		,SpawnTransform
		,GetOwningActorFromActorInfo()
		,Cast<APawn>(GetOwningActorFromActorInfo())
		, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (DemoFireBolt)
	{
		DemoFireBolt->DamageGameplayEffectClass=DamageGameplayEffectClass;
		DemoFireBolt->BaseDamage=BaseDamage.GetValueAtLevel(1.f);
		DemoFireBolt->DamageType=DamageType;
		DemoFireBolt->SetInstigator(Cast<APawn>(GetOwningActorFromActorInfo()));
		DemoFireBolt->SetOwner(GetAvatarActorFromActorInfo());
	}
	if (HomingTarget&&HomingTarget->Implements<UCombatInterface>())
	{
		DemoFireBolt->ProjectileMovement->HomingTargetComponent = HomingTarget->GetRootComponent();
	}
	else
	{
		DemoFireBolt->HomingTargetSceneComponent=NewObject<USceneComponent>(USceneComponent::StaticClass());
		DemoFireBolt->HomingTargetSceneComponent->SetWorldLocation(FireBoltTargetLocation);
		DemoFireBolt->ProjectileMovement->HomingTargetComponent=DemoFireBolt->HomingTargetSceneComponent;
	}
	DemoFireBolt->ProjectileMovement->HomingAccelerationMagnitude=FMath::RandRange(HomingAccelerationMin, HomingAccelerationMax);
	DemoFireBolt->ProjectileMovement->bIsHomingProjectile=bLaunchHomingProjectile;
	DemoFireBolt->FinishSpawning(SpawnTransform);
}

void UDemoFireBoltAbility::OnTargetDataReady(const FGameplayAbilityTargetDataHandle& DataHandle)
{
	if (!GetAvatarActorFromActorInfo()->HasAuthority())
	{
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
		return;
	}


	if (!CommitAbilityCooldown(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(),true))
	{
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
		return;
	}
	if (!CommitAbilityCost(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo()))
	{
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
		return;
	}
	FVector TargetLocation = DataHandle.Get(0)->GetHitResult()->Location;

	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}
