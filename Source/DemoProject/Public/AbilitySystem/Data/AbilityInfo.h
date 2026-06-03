// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "AbilityInfo.generated.h"

class UGameplayAbility;

USTRUCT(BlueprintType)
struct FDemoAbilityInfo
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FGameplayTag AbilityTag=FGameplayTag();

	//UPROPERTY(BlueprintReadOnly)
	//FGameplayTag StatusTag = FGameplayTag();
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag InputTag = FGameplayTag();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag CooldownTag = FGameplayTag();

	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	//FGameplayTag AbilityType = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTexture2D>Icon = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UMaterialInterface>BackgroundMaterial = nullptr;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	//int32 LevelRequirment=1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayAbility>Ability;
};
/**
 * 
 */
UCLASS()
class DEMOPROJECT_API UAbilityInfo : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="AbilityInfo")
	TArray<FDemoAbilityInfo> AbilityInformation;
	FDemoAbilityInfo FindAbilityInfoForTag(const FGameplayTag& Tag) const;
};
