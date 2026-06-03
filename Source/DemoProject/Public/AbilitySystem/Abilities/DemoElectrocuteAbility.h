// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DemoGameplayAbility.h"
#include "DemoElectrocuteAbility.generated.h"

/**
 * 
 */
UCLASS()
class DEMOPROJECT_API UDemoElectrocuteAbility : public UDemoGameplayAbility
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void StoreMouseHitInfo(const FHitResult&MouseHitResult);
	UFUNCTION(BlueprintCallable)
	void StoreOwnerVariables();
	UFUNCTION(BlueprintCallable)
	void TraceFirstTarget(const FVector&BeamTargetLocation);
	UFUNCTION(BlueprintCallable)
	void StoreAdditionalTargets(TArray<AActor*>&AdditionalTargets);
	UFUNCTION(BlueprintImplementableEvent)
	void FirstTargetDied(AActor* DeadActor);
	UFUNCTION(BlueprintImplementableEvent)
	void AdditionalTargetDied(AActor* DeadActor);
	
protected:
	UPROPERTY(BlueprintReadWrite,Category="Electrocute")
	TObjectPtr<AActor>MouseHitActor;
	UPROPERTY(BlueprintReadWrite,Category="Electrocute")
	FVector MouseHitLocation;
	UPROPERTY(BlueprintReadWrite,Category="Electrocute")
	TObjectPtr<ACharacter> OwnerCharacter;
	UPROPERTY(BlueprintReadWrite,Category="Electrocute")
	TObjectPtr<APlayerController> OwnerPlayerController;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Electrocute")
	int32 NumShocked=3;
};
