// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "DemoInputConfig.generated.h"

class UInputAction;
/**
 * 
 */
USTRUCT(BlueprintType)
struct FDemoInputAction
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputTag=FGameplayTag();
	UPROPERTY(EditDefaultsOnly)
	const UInputAction* InputAction=nullptr;
};

UCLASS()
class DEMOPROJECT_API UDemoInputConfig : public UDataAsset
{
	GENERATED_BODY()
public:
	const UInputAction *FindAbilityInputActionForTag(const FGameplayTag & Tag,bool bNotFound=false) const;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TArray<FDemoInputAction> AbilityInputActions;
};
