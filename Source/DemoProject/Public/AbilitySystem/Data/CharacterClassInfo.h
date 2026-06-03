// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterClassInfo.generated.h"

class UGameplayAbility;
class UGameplayEffect;

UENUM(BlueprintType)
enum class ECharacterClass:uint8
{
	Elementalist,
	Ranger,
	Warrior
};
USTRUCT(BlueprintType)
struct FCharacterClassDefaultInfo
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly,Category="ClassDefault")
	TSubclassOf<UGameplayEffect>PrimaryAttribute;
	UPROPERTY(EditDefaultsOnly,Category="DefaultAbility")
	TArray<TSubclassOf<UGameplayAbility>>StartupAbilities;
	
};
UCLASS()
class DEMOPROJECT_API UCharacterClassInfo : public UDataAsset
{
	GENERATED_BODY()
public:

	UPROPERTY(EditDefaultsOnly,Category="CharacterClassDefault")
	TMap<ECharacterClass,FCharacterClassDefaultInfo> CharacterClassInfo;
	FCharacterClassDefaultInfo GetDefaultCharacterClassInfo(ECharacterClass CharacterClass);
	
	UPROPERTY(EditDefaultsOnly, Category = "Common Class Defaults")
	TSubclassOf<UGameplayEffect>SecondaryAttributes;
	UPROPERTY(EditDefaultsOnly, Category = "Common Class Defaults")
	TSubclassOf<UGameplayEffect>VitalAttributes;
	UPROPERTY(EditDefaultsOnly, Category = "Common Class Defaults")
	TArray<TSubclassOf<UGameplayAbility>>CommonAbilities;
};
