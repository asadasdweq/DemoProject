// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EnemyInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UEnemyInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DEMOPROJECT_API IEnemyInterface
{
	GENERATED_BODY()

	
public:
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	void SetCombatTarget(AActor*InCombatTarget);
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	AActor* GetCombatTarget();
};
