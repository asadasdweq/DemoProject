// Fill out your copyright notice in the Description page of Project Settings.


#include "DemoGameplayTags.h"
#include"GameplayTagsManager.h"

FDemoGameplayTags FDemoGameplayTags::GameplayTags;
void FDemoGameplayTags::InitializeNativeGameplayTags()
{
	GameplayTags.Globel_Health=UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Globel.Health"),FString("HealthGlobal"));
	GameplayTags.Globel_Mana=UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Globel.Mana"),FString("ManaGlobal"));
	//InputTag
    GameplayTags.InputTag_LMB=UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("InputTag.LMB"),
        FString("Input Tag for Left mouse Button"));
    GameplayTags.InputTag_RMB=UGameplayTagsManager::Get().AddNativeGameplayTag(
                FName("InputTag.RMB"),
                FString("Input Tag for Right mouse Button"));
    GameplayTags.InputTag_1=UGameplayTagsManager::Get().AddNativeGameplayTag(
                FName("InputTag.1"),
                FString("Input Tag 1"));
   GameplayTags.InputTag_2=UGameplayTagsManager::Get().AddNativeGameplayTag(
                FName("InputTag.2"),
                FString("Input Tag 2"));
    GameplayTags.InputTag_3=UGameplayTagsManager::Get().AddNativeGameplayTag(
                FName("InputTag.3"),
                FString("Input Tag 3"));
   GameplayTags.InputTag_4=UGameplayTagsManager::Get().AddNativeGameplayTag(
                FName("InputTag.4"),
                FString("Input Tag 4"));
	//Vital Attribute
	GameplayTags.Attributes_Vital_Health=UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Vital.Health"),FString("Health"));
	GameplayTags.Attributes_Vital_Mana=UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Vital.Mana"),FString("Mana"));
	
	//Primary Attribute
	GameplayTags.Attributes_Primary_Power=UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Primary.Power"),FString("Power"));
	GameplayTags.Attributes_Primary_Intelligence=UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Primary.Intelligence"),FString("Intelligence"));
	GameplayTags.Attributes_Primary_Vigor=UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Primary.Vigor"),FString("Vigor"));
	GameplayTags.Attributes_Primary_Resilience=UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Primary.Resilience"),FString("Resilience"));
	//Secondary Attribute
	GameplayTags.Attributes_Secondary_MaxHealth=UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.MaxHealth"),FString("MaxHealth"));
	GameplayTags.Attributes_Secondary_MaxMana=UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.MaxMana"),FString("MaxMana"));
	GameplayTags.Attributes_Secondary_Armor=UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.Armor"),FString("Armor"));
	GameplayTags.Attributes_Secondary_MagicalResistance=UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.MagicalResistance"),FString("MagicalResistance"));
	GameplayTags.Attributes_Secondary_PhysicalDamage=UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.PhysicalDamage"),FString("PhysicalDamage"));
	GameplayTags.Attributes_Secondary_MagicalDamage=UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.MagicalDamage"),FString("MagicalDamage"));
	//State
	GameplayTags.State_AbilityInputBlocked=UGameplayTagsManager::Get().AddNativeGameplayTag(FName("State.AbilityInputBlocked"),FString("Actor is blocking ability input"));
	//Abilities PawnAbility
	GameplayTags.Abilities_FireBolt=UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.FireBolt"),FString("FireBolt"));
	GameplayTags.Abilities_Lightning=UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.Lightning"),FString("Lightning"));
	GameplayTags.Abilities_Electrocute=UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.Electrocute"),FString("Electrocute"));
	GameplayTags.Abilities_ChargeRock=UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.ChargeRock"),FString("ChargeRock"));
	
	GameplayTags.Abilities_EnemyAttack=UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.EnemyAttack"),FString("EnemyAttack"));
	
	
	GameplayTags.Abilities_None=UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.None"),FString("None"));
	//EventTag
	GameplayTags.Attack_1=UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attack.1"),FString("Attack 1"));
	GameplayTags.Attack_2=UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attack.2"),FString("Attack 2"));
	GameplayTags.Event_FireBolt=UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Event.FireBolt"),FString("FireBolt"));
	GameplayTags.Event_Lightning=UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Event.Lightning"),FString("Lightning"));
	GameplayTags.Event_Electrocute=UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Event.Electrocute"),FString("Electrocute"));
	GameplayTags.Event_ChargeRock=UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Event.ChargeRock"),FString("ChargeRock"));
	//HitReact
	GameplayTags.Effect_HitReact=UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Effects.HitReact"),FString("HitReact"));
	//CoolDown
	GameplayTags.CoolDown_FireBolt=UGameplayTagsManager::Get().AddNativeGameplayTag(FName("CoolDown.FireBolt"),FString("CoolDownFireBolt"));
	GameplayTags.CoolDown_Lightning=UGameplayTagsManager::Get().AddNativeGameplayTag(FName("CoolDown.Lightning"),FString("CoolDownLightning"));
	GameplayTags.CoolDown_Electrocute=UGameplayTagsManager::Get().AddNativeGameplayTag(FName("CoolDown.Electrocute"),FString("CoolDownElectrocute"));
	GameplayTags.CoolDown_ChargeRock=UGameplayTagsManager::Get().AddNativeGameplayTag(FName("CoolDown.ChargeRock"),FString("CoolDownChargeRock"));
	//DamageTypeToResistance
	GameplayTags.DamageTypeToResistance.Add(GameplayTags.Attributes_Secondary_PhysicalDamage,GameplayTags.Attributes_Secondary_Armor);
	GameplayTags.DamageTypeToResistance.Add(GameplayTags.Attributes_Secondary_MagicalDamage,GameplayTags.Attributes_Secondary_MagicalResistance);
	
	
}
