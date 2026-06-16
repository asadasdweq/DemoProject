// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "GameplayEffectTypes.h" 
#include"DemoAbilityTypes.generated.h"

USTRUCT(BlueprintType)
struct FDemoGameplayEffectContext:public FGameplayEffectContext
{
	GENERATED_BODY()
	
};

USTRUCT()
struct DEMOPROJECT_API FGameplayAbilityTargetData_ChargeRock : public FGameplayAbilityTargetData
{
	GENERATED_BODY()

	UPROPERTY()
	FVector_NetQuantize TargetLocation = FVector::ZeroVector;

	UPROPERTY()
	float ChargePercent = 0.f;

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return StaticStruct();
	}

	bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
	{
		TargetLocation.NetSerialize(Ar, Map, bOutSuccess);
		Ar << ChargePercent;
		ChargePercent = FMath::Clamp(ChargePercent, 0.f, 1.f);
		return true;
	}
};

template<>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_ChargeRock> : public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_ChargeRock>
{
	enum
	{
		WithNetSerializer = true
	};
};

