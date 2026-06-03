// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/DemoAbilitySystemLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Engine/OverlapResult.h"
#include "Game/DemoGameModeBase.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"


bool UDemoAbilitySystemLibrary::IsNotFriend(AActor* FirstActor, AActor* SecondActor)
{
	const bool bFirstActorIsPlayer=FirstActor->ActorHasTag(FName("Player"));
	const bool bSecondActorIsPlayer=SecondActor->ActorHasTag(FName("Player"));
	const bool bFirstActorIsEnemy=FirstActor->ActorHasTag(FName("Enemy"));
	const bool bSecondActorIsEnemy=SecondActor->ActorHasTag(FName("Enemy"));
	const bool bBothIsPlayer=bFirstActorIsPlayer&&bSecondActorIsPlayer;
	const bool bBothIsEnemy=bFirstActorIsEnemy&&bSecondActorIsEnemy;
	const bool bIsFriend=bBothIsPlayer||bBothIsEnemy;
	return !bIsFriend;
}

void UDemoAbilitySystemLibrary::CauseDamage(UAbilitySystemComponent* SourceASC, UAbilitySystemComponent* TargetASC,
                                            TSubclassOf<UGameplayEffect>DamageEffectClass, FGameplayTag DamageType, float BaseDamage, int32 AbilityLevel)
{
	if (!SourceASC||!TargetASC||!DamageEffectClass)return;
	const FGameplayEffectContextHandle ContextHandle=SourceASC->MakeEffectContext();
	const FGameplayEffectSpecHandle SpecHandle=SourceASC->MakeOutgoingSpec(DamageEffectClass,AbilityLevel,ContextHandle);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,DamageType,BaseDamage);
	SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(),TargetASC);
}

UCharacterClassInfo* UDemoAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	const ADemoGameModeBase*DemoGameModeBase = Cast<ADemoGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (DemoGameModeBase==nullptr) return nullptr;
	return DemoGameModeBase->CharacterClassInfo;
}

void UDemoAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject,
	ECharacterClass CharacterClass, UAbilitySystemComponent* ASC)
{
	AActor*AvatarActor=ASC->GetAvatarActor();
	UCharacterClassInfo* CharacterClassInfo=GetCharacterClassInfo(WorldContextObject);
	FCharacterClassDefaultInfo ClassDefaultInfo=CharacterClassInfo->GetDefaultCharacterClassInfo(CharacterClass);
	
	FGameplayEffectContextHandle PrimaryContextHandle=ASC->MakeEffectContext();
	PrimaryContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle PrimarySpecHandle=ASC->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttribute,1,PrimaryContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*PrimarySpecHandle.Data.Get());
	
	FGameplayEffectContextHandle SecondaryContextHandle=ASC->MakeEffectContext();
	SecondaryContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle SecondarySpecHandle=ASC->MakeOutgoingSpec(CharacterClassInfo->SecondaryAttributes,1,SecondaryContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondarySpecHandle.Data.Get());
	
	FGameplayEffectContextHandle VitalContextHandle=ASC->MakeEffectContext();
	VitalContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle VitalSpecHandle=ASC->MakeOutgoingSpec(CharacterClassInfo->VitalAttributes,1,VitalContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalSpecHandle.Data.Get());
	
	
}

void UDemoAbilitySystemLibrary::GiveStartupAbility(const UObject* WorldContextObject, UAbilitySystemComponent* ASC,
                                                   ECharacterClass CharacterClass)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (CharacterClassInfo==nullptr)return;
	for (TSubclassOf<UGameplayAbility>StartupCommonAbility:CharacterClassInfo->CommonAbilities)
	{
		FGameplayAbilitySpec AbilitySpec=FGameplayAbilitySpec(StartupCommonAbility,1);
		ASC->GiveAbility(AbilitySpec);
	}
	const FCharacterClassDefaultInfo&CharacterClassDefaultInfo=CharacterClassInfo->GetDefaultCharacterClassInfo(CharacterClass);
	
	for (TSubclassOf<UGameplayAbility>StartupAbility:CharacterClassDefaultInfo.StartupAbilities)
	{

		FGameplayAbilitySpec AbilitySpec=FGameplayAbilitySpec(StartupAbility,1);
		ASC->GiveAbility(AbilitySpec);
	}
	
}

void UDemoAbilitySystemLibrary::GetLivePlayerWithinRadius(const UObject* WorldContextObject,
	TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float Radius,
	const FVector& SphereLocation)
{
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorsToIgnore);
	// query scene to see what we hit
	TArray<FOverlapResult> Overlaps;
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		World->OverlapMultiByObjectType(Overlaps, SphereLocation, FQuat::Identity, 
			FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects), 
			FCollisionShape::MakeSphere(Radius), SphereParams);
		for (FOverlapResult& Overlap : Overlaps)
		{
			if (Overlap.GetActor()->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(Overlap.GetActor()))
			{
				OutOverlappingActors.AddUnique(ICombatInterface::Execute_GetAvatar(Overlap.GetActor()));
			}
		}
	}
}

void UDemoAbilitySystemLibrary::GetClosestTargets(int32 MaxTargets, const TArray<AActor*>& Actors,
	TArray<AActor*>&OutClosestActors, const FVector& Origin)
{
	if (Actors.Num()<MaxTargets)
	{
		OutClosestActors=Actors;
		return;
	}
	TArray<AActor*>ActorsToCheck=Actors;
	int32 NumTargetsFound=0;
	while (NumTargetsFound<MaxTargets)
	{
		if (ActorsToCheck.Num()==0)break;
		double MinDistance=TNumericLimits<double>::Max();
		AActor*ClosetActor;
		for (AActor*PotentialActor:ActorsToCheck)
		{
			const double Distance=(PotentialActor->GetActorLocation()-Origin).Length();
			if(Distance<MinDistance)
			{
				MinDistance=Distance;
				ClosetActor=PotentialActor;
			}
		}
		ActorsToCheck.Remove(ClosetActor);
		OutClosestActors.AddUnique(ClosetActor);
		++NumTargetsFound;
	}
}
