// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"
#include "DemoGameplayTags.h"
#include "AbilitySystem/DemoAttributeSet.h"


struct DemoDamageStatic
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MagicalDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MagicalResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Intelligence);
	
	DemoDamageStatic()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDemoAttributeSet,MagicalDamage,Source,false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDemoAttributeSet,PhysicalDamage,Source,false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDemoAttributeSet,Armor,Target,false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDemoAttributeSet,MagicalResistance,Target,false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDemoAttributeSet,Intelligence,Source,false);
		
	}
};
static const DemoDamageStatic&DamageStatics()
{
	static DemoDamageStatic DStatic;
	return DStatic;
}
UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().PhysicalDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().MagicalDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().MagicalResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().IntelligenceDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	TMap<FGameplayTag,FGameplayEffectAttributeCaptureDefinition>TagsToCaptureDefs;
	const FDemoGameplayTags &Tags=FDemoGameplayTags::Get();
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_PhysicalDamage,DamageStatics().PhysicalDamageDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_MagicalDamage,DamageStatics().MagicalDamageDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_Armor,DamageStatics().ArmorDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_MagicalResistance,DamageStatics().MagicalResistanceDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Primary_Intelligence,DamageStatics().IntelligenceDef);
	
	
	const UAbilitySystemComponent*SourceASC=ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent*TargetASC=ExecutionParams.GetTargetAbilitySystemComponent();
	AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;
	const FGameplayEffectSpec&Spec=ExecutionParams.GetOwningSpec();
	FGameplayEffectContextHandle ContextHandle=Spec.GetContext();
	
	const FGameplayTagContainer*SourceTag=Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer*TargetTag=Spec.CapturedTargetTags.GetAggregatedTags();
	
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags=SourceTag;
	EvaluationParameters.TargetTags=TargetTag;
	
	
	float Damage=0.f;
	//Intelligence
	const FGameplayEffectAttributeCaptureDefinition IntelligenceDef=TagsToCaptureDefs[Tags.Attributes_Primary_Intelligence];
	float IntelligenceDamage=0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(IntelligenceDef,EvaluationParameters,IntelligenceDamage);
	IntelligenceDamage=FMath::Clamp(IntelligenceDamage,0.f,100.f);
	//end Intelligence
	for (const TTuple<FGameplayTag,FGameplayTag>&Pair:Tags.DamageTypeToResistance)
	{
		const FGameplayTag DamageType=Pair.Key;
		const FGameplayTag DamageResistance=Pair.Value;
		check(TagsToCaptureDefs.Contains(DamageResistance));
		const FGameplayEffectAttributeCaptureDefinition CaptureDef=TagsToCaptureDefs[DamageResistance];
		

		
		
		float DamageTypeValue=Spec.GetSetByCallerMagnitude(DamageType,false);
		
		float Resistance=0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDef,EvaluationParameters,Resistance);
		Resistance=FMath::Clamp(Resistance,0.f,100.f);
		DamageTypeValue-=Resistance;
		if (DamageTypeValue<=0.f)
		{
			continue;
		}
		Damage += DamageTypeValue;

		if (DamageType==Tags.Attributes_Secondary_MagicalDamage&&Damage>0.f)
		{
			Damage+=IntelligenceDamage;
		}
	}
	const FGameplayModifierEvaluatedData EvaluatedData(UDemoAttributeSet::GetIncomingDamageAttribute(),EGameplayModOp::Additive,Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
	
	
	
}
