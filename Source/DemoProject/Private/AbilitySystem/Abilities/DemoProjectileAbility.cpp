// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/DemoProjectileAbility.h"

#include "Actors/DemoFireBolt.h"
#include "Interaction/CombatInterface.h"

void UDemoProjectileAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                             const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                             const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UDemoProjectileAbility::SpawnProjectile(const FVector& TargetLocation)
{
	const bool bIsServer=GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsServer)return;
	
	const FVector SocketLocation=ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo());
	FRotator Rotation=(TargetLocation - SocketLocation).Rotation();
	
	//const FVector Forward=Rotation.Vector();
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SocketLocation);
	SpawnTransform.SetRotation(Rotation.Quaternion());//
	ADemoFireBolt*Projectile=GetWorld()->SpawnActorDeferred<ADemoFireBolt>(ProjectileClass 
		,SpawnTransform
		,GetOwningActorFromActorInfo()
		,Cast<APawn>(GetOwningActorFromActorInfo())
		, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (Projectile)
	{
		Projectile->DamageGameplayEffectClass=DamageGameplayEffectClass;
		Projectile->BaseDamage=BaseDamage.GetValueAtLevel(1.f);
		Projectile->DamageType=DamageType;
		Projectile->SetInstigator(Cast<APawn>(GetOwningActorFromActorInfo()));
		Projectile->SetOwner(GetAvatarActorFromActorInfo());
	}
	Projectile->FinishSpawning(SpawnTransform);
}
