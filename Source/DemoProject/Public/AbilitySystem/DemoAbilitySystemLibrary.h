// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "DemoAbilitySystemLibrary.generated.h"

enum class ECharacterClass : uint8;
class UCharacterClassInfo;
class UGameplayEffect;
class UAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class DEMOPROJECT_API UDemoAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	static bool IsNotFriend(AActor *FirstActor,AActor *SecondActor);
	UFUNCTION(BlueprintCallable)
	static void CauseDamage(UAbilitySystemComponent*SourceASC,UAbilitySystemComponent*TargetASC,
		TSubclassOf<UGameplayEffect>DamageEffectClass,FGameplayTag DamageType,float BaseDamage,int32 AbilityLevel=1);
	
	UFUNCTION(BlueprintCallable)
	static UCharacterClassInfo * GetCharacterClassInfo(const UObject*WorldContextObject);
	UFUNCTION(BlueprintCallable)
	static void InitializeDefaultAttributes(const UObject*WorldContextObject,ECharacterClass CharacterClass,UAbilitySystemComponent*ASC);
	UFUNCTION(BlueprintCallable)
	static void GiveStartupAbility(const UObject*WorldContextObject,UAbilitySystemComponent*ASC,ECharacterClass CharacterClass);
	UFUNCTION(BlueprintCallable, Category = "Mechanics")
	static void GetLivePlayerWithinRadius(const UObject* WorldContextObject,
		TArray<AActor*>&OutOverlappingActors,const TArray<AActor*>&ActorsToIgnore,
		float Radius,const FVector&SphereLocation);
	UFUNCTION(BlueprintCallable, Category = "Mechanics")
	static void GetClosestTargets(int32 MaxTargets,const TArray<AActor*>&Actors,TArray<AActor*>&OutClosestActors,const FVector&Origin);
	
};
