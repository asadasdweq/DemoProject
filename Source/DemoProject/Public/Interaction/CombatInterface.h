// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "CombatInterface.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeathSignature, AActor*, DeadActor);
class UNiagaraSystem;
UINTERFACE()
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DEMOPROJECT_API ICombatInterface
{
	GENERATED_BODY()

	
public:
	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable )
	void UpdateFacingTarget(const FVector&Target);
	
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	UAnimMontage*GetHitReactMontage();
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	ECharacterClass GetCharacterClass();
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable )
	FVector GetCombatSocketLocation();
	
	
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	void Die();
	
	virtual FOnDeathSignature&GetOnDeathDel()=0;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsDead() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	AActor* GetAvatar();
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	UAnimMontage*GetAttackMontage() const;

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	UNiagaraSystem* GetBloodEffect();
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	USceneComponent * GetWeapon();
	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
	void SetInShockLoop(bool InShockLoop);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsBeingShocked() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetIsBeingShocked(bool bInShock);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsCasting() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetIsCasting(bool bInCasting);
	
	
};
