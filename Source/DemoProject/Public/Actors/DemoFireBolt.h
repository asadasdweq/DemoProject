// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "ScalableFloat.h"
#include "DemoFireBolt.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UGameplayEffect;
class UNiagaraSystem;
class USoundBase;

UCLASS()
class DEMOPROJECT_API ADemoFireBolt : public AActor
{
	GENERATED_BODY()

public:
	
	ADemoFireBolt();
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent>ProjectileMovement;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent>Sphere;
	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, 
		AActor* OtherActor, UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnHit();
	
	
	virtual void Destroyed()override;
	bool IsValidOverlap(AActor *OtherActor);
	bool bHit = false;


	UPROPERTY()
	TObjectPtr<USceneComponent> HomingTargetSceneComponent;

	UPROPERTY(EditAnywhere, Category = "Damage")
	TSubclassOf<UGameplayEffect> DamageGameplayEffectClass;
	UPROPERTY(EditAnywhere, Category = "Damage")
	FGameplayTag DamageType;
	UPROPERTY(EditAnywhere, Category = "Damage")
	float BaseDamage;
protected:
	
	virtual void BeginPlay() override;
private:
	
	UPROPERTY(EditAnywhere)
	float LifeSpan = 15.f;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem>ImpactEffect;
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase>ImpactSound;
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase>LoopingSound;
};
