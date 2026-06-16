// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "DemoAttributeSet.generated.h"
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
struct FGameplayEffectModCallbackData;


USTRUCT()
struct FEffectProperties
{
	GENERATED_BODY()
	FEffectProperties(){}
	FGameplayEffectContextHandle EffectContextHandle;
	UPROPERTY()
	UAbilitySystemComponent* SourceASC = nullptr;
	UPROPERTY()
	AActor* SourceAvatarActor = nullptr;
	UPROPERTY()
	AController* SourceController = nullptr;
	UPROPERTY()
	ACharacter* SourceCharacter = nullptr;
	UPROPERTY()
	UAbilitySystemComponent* TargetASC = nullptr;
	UPROPERTY()
	AActor* TargetAvatarActor = nullptr;
	UPROPERTY()
	AController* TargetController = nullptr;
	UPROPERTY()
	ACharacter* TargetCharacter = nullptr;
};
/**
 * 
 */
UCLASS()
class DEMOPROJECT_API UDemoAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	UDemoAttributeSet();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	
	
	/*Vital Attributes*/
	UPROPERTY(BlueprintReadWrite,ReplicatedUsing=OnRep_Health,Category="VatalAttributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UDemoAttributeSet, Health)
	UPROPERTY(BlueprintReadWrite,ReplicatedUsing=OnRep_Mana,Category="VatalAttributes")
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UDemoAttributeSet, Mana)
	
	/*PrimaryAttributes*/
	UPROPERTY(BlueprintReadWrite,ReplicatedUsing=OnRep_Power,Category="PrimaryAttributes")
	FGameplayAttributeData Power;
	ATTRIBUTE_ACCESSORS(UDemoAttributeSet, Power)
	UPROPERTY(BlueprintReadWrite,ReplicatedUsing=OnRep_Intelligence,Category="PrimaryAttributes")
	FGameplayAttributeData Intelligence;
	ATTRIBUTE_ACCESSORS(UDemoAttributeSet, Intelligence)
	UPROPERTY(BlueprintReadWrite,ReplicatedUsing=OnRep_Vigor,Category="PrimaryAttributes")
	FGameplayAttributeData Vigor;
	ATTRIBUTE_ACCESSORS(UDemoAttributeSet, Vigor)
	UPROPERTY(BlueprintReadWrite,ReplicatedUsing=OnRep_Resilience,Category="PrimaryAttributes")
	FGameplayAttributeData Resilience;
	ATTRIBUTE_ACCESSORS(UDemoAttributeSet, Resilience)
	//SecondaryAttributes
	UPROPERTY(BlueprintReadWrite,ReplicatedUsing=OnRep_MaxHealth,Category="PrimaryAttributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UDemoAttributeSet, MaxHealth)
	UPROPERTY(BlueprintReadWrite,ReplicatedUsing=OnRep_MaxMana,Category="PrimaryAttributes")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UDemoAttributeSet, MaxMana)
	UPROPERTY(BlueprintReadWrite,ReplicatedUsing=OnRep_Armor,Category="PrimaryAttributes")
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(UDemoAttributeSet, Armor)
	UPROPERTY(BlueprintReadWrite,ReplicatedUsing=OnRep_PhysicalDamage,Category="PrimaryAttributes")
	FGameplayAttributeData PhysicalDamage;
	ATTRIBUTE_ACCESSORS(UDemoAttributeSet, PhysicalDamage)
	UPROPERTY(BlueprintReadWrite,ReplicatedUsing=OnRep_MagicalDamage,Category="PrimaryAttributes")
	FGameplayAttributeData MagicalDamage;
	ATTRIBUTE_ACCESSORS(UDemoAttributeSet, MagicalDamage)
	UPROPERTY(BlueprintReadWrite,ReplicatedUsing=OnRep_MagicalResistance,Category="PrimaryAttributes")
	FGameplayAttributeData MagicalResistance;
	ATTRIBUTE_ACCESSORS(UDemoAttributeSet, MagicalResistance)
	//IncomingDamage
	UPROPERTY(BlueprintReadOnly, Category = "Meta Attributes")
	FGameplayAttributeData IncomingDamage;
	ATTRIBUTE_ACCESSORS(UDemoAttributeSet, IncomingDamage);
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData&OldHealth) const;
	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldMana) const;
	UFUNCTION()
	void OnRep_Power(const FGameplayAttributeData& OldPower) const;
	UFUNCTION()
	void OnRep_Vigor(const FGameplayAttributeData& OldVigor) const;
	UFUNCTION()
	void OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const;
	UFUNCTION()
	void OnRep_Resilience(const FGameplayAttributeData& OldResilience) const;
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;
	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const;
	UFUNCTION()
	void OnRep_Armor(const FGameplayAttributeData& OldArmor) const;
	UFUNCTION()
	void OnRep_PhysicalDamage(const FGameplayAttributeData& OldPhysicalDamage) const;
	UFUNCTION()
	void OnRep_MagicalDamage(const FGameplayAttributeData& OldMagicalDamage) const;
	UFUNCTION()
	void OnRep_MagicalResistance(const FGameplayAttributeData& OldMagicalResistance) const;
private:
	void SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const;
	void HandleIncomingDamage(const FEffectProperties&Props);
	void ShowDamageText(const FEffectProperties&Props,float Damage);
};


