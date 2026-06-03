// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "DemoCharacterBase.generated.h"
class UNiagaraSystem;
class UGameplayEffect;
class UAbilitySystemComponent;
class UAttributeSet;
class UGameplayAbility;
UCLASS()
class DEMOPROJECT_API ADemoCharacterBase : public ACharacter,public IAbilitySystemInterface,public ICombatInterface
{
	GENERATED_BODY()

public:

	ADemoCharacterBase();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet()const{return AttributeSet;};
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void InitActorAbilityInfo();
	virtual void InitializeDefaultAttributes()const;

	void SetHasWeapon(bool HasWeapon);
	virtual void SetWeapon(AActor*Actor);
	
	/*CombatInterface*/
	virtual FVector GetCombatSocketLocation_Implementation() override;
	virtual UAnimMontage*GetHitReactMontage_Implementation() override;
	virtual ECharacterClass GetCharacterClass_Implementation() override;
	virtual void Die_Implementation() override;
	virtual FOnDeathSignature&GetOnDeathDel()override;

	virtual bool IsDead_Implementation() const override;
	virtual AActor*GetAvatar_Implementation() override;
	//End
	virtual UAnimMontage*GetAttackMontage_Implementation() const override;
	virtual UNiagaraSystem* GetBloodEffect_Implementation() override;
	virtual USceneComponent * GetWeapon_Implementation()override;
	virtual void SetIsBeingShocked_Implementation(bool bInShock) override;
	virtual bool IsBeingShocked_Implementation()const override;
	
	/*EndCombatInterface*/
	UFUNCTION(NetMulticast,Reliable)
	virtual void MulticastHandleDeath();
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Combat")
	TObjectPtr<UAnimMontage> AttackMontage;
	UPROPERTY(Replicated,BlueprintReadOnly)
	bool bIsBeingShocked=false;
	
	
	//Delegate
	FOnDeathSignature OnDeathDelegate;
	
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<USkeletalMeshComponent>Weapon;
	UPROPERTY(Replicated, BlueprintReadOnly, Category="Combat")
	bool bHasWeapon=false;
	
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TSubclassOf<UGameplayEffect> PrimaryAttributes;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TSubclassOf<UGameplayEffect> VitalAttributes;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TSubclassOf<UGameplayEffect> SecondaryAttributes;
	void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffect,float Level)const;
	
	void AddCharacterAbilities();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USoundBase* DeathSound;
	UPROPERTY(BlueprintReadOnly)
	bool bDead = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UNiagaraSystem *BloodEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category="CharacterClas")
	ECharacterClass CharacterClass=ECharacterClass::Warrior;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float BaseWalkSpeed=600.f;

	void Dissolve();
	UFUNCTION(BlueprintImplementableEvent)
	void StartDissolveTimeline(UMaterialInstanceDynamic*DynamicMaterialInstance);
	UFUNCTION(BlueprintImplementableEvent)
	void StartWeaponDissolveTimeline(UMaterialInstanceDynamic*DynamicMaterialInstance);
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance>DissolveMaterialInstance;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance>WeaponDissolveMaterialInstance;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent>AbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UAttributeSet>AttributeSet;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	FName WeaponSocketName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	FName HandSocketName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float MaxWeaponSocketDistanceFromHand = 500.f;
	
private:
	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>>StartupAbilities;
	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>>StartupPassiveAbilities;
	UPROPERTY(EditAnywhere, Category = "HitReact")
	TObjectPtr<UAnimMontage> HitReactMontage;

};
