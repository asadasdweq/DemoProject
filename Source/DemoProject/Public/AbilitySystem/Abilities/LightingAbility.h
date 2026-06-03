// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DemoGameplayAbility.h"
#include "LightingAbility.generated.h"

/**
 * 
 */
UCLASS()
class DEMOPROJECT_API ULightingAbility : public UDemoGameplayAbility
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void SpawnLightningEffect(const FVector & SpawnLocation);
	
protected:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Lightning")
	TSubclassOf<AActor>LightningEffectClass;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Lightning")
	float LightningRadius=150.f;
	
	
	
private:
};
