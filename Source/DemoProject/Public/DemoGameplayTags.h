// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

struct FDemoGameplayTags
{
public:
	static const FDemoGameplayTags &Get(){return GameplayTags;}
	static void InitializeNativeGameplayTags();
	//OverlayGlobalTag
	FGameplayTag Globel_Health;
	FGameplayTag Globel_Mana;
    //inputTag
	FGameplayTag InputTag_LMB;
	FGameplayTag InputTag_RMB;
	FGameplayTag InputTag_1;
	FGameplayTag InputTag_2;
	FGameplayTag InputTag_3;
	FGameplayTag InputTag_4;
	//Attribute
	//Vital Attribute
	FGameplayTag Attributes_Vital_Health;
	
	FGameplayTag Attributes_Vital_Mana;
	
	//PrimaryAttribute
	FGameplayTag Attributes_Primary_Power;
	FGameplayTag Attributes_Primary_Intelligence;
	FGameplayTag Attributes_Primary_Vigor;
	FGameplayTag Attributes_Primary_Resilience;
	//SecondaryAttribute
	FGameplayTag Attributes_Secondary_MaxHealth;
	FGameplayTag Attributes_Secondary_MaxMana;
	FGameplayTag Attributes_Secondary_Armor;
	FGameplayTag Attributes_Secondary_MagicalResistance;
	FGameplayTag Attributes_Secondary_PhysicalDamage;
	FGameplayTag Attributes_Secondary_MagicalDamage;
	
	//HitReact
	FGameplayTag Effect_HitReact;
	//State
	FGameplayTag State_AbilityInputBlocked;
	//Ability
	FGameplayTag Abilities_FireBolt;
	FGameplayTag Abilities_Lightning;
	FGameplayTag Abilities_Electrocute;
	FGameplayTag Abilities_ChargeRock;
	
	FGameplayTag Abilities_EnemyAttack;
	
	FGameplayTag Abilities_None;
	//EventTag
	FGameplayTag Attack_1;
	FGameplayTag Attack_2;
	FGameplayTag Event_FireBolt;
	FGameplayTag Event_Lightning;
	FGameplayTag Event_Electrocute;
	FGameplayTag Event_ChargeRock;
	
	//CoolDown
    FGameplayTag CoolDown_FireBolt;
	FGameplayTag CoolDown_Lightning;
	FGameplayTag CoolDown_Electrocute;
	FGameplayTag CoolDown_ChargeRock;
	//DamageType
	TMap<FGameplayTag,FGameplayTag> DamageTypeToResistance;
	

private:
	static FDemoGameplayTags GameplayTags; 
};
