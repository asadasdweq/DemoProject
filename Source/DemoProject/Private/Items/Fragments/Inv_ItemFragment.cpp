#include"Items\Fragments\Inv_ItemFragment.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "EquipmentManagement/EquipActor/Inv_EquipActor.h"
#include "UI/InventoryWidget/CompositeBase/Inv_Leaf_Image.h"
#include "UI/InventoryWidget/CompositeBase/Inv_Leaf_LabeledValue.h"
#include "UI/InventoryWidget/CompositeBase/Inv_Leaf_Text.h"
#include"UI\InventoryWidget/CompositeBase/Inv_CompositeBase.h"
void FInv_InventoryItemFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	if (!MatchesWidgetTag(Composite))return;
	Composite->Expand();
		
}

bool FInv_InventoryItemFragment::MatchesWidgetTag(const UInv_CompositeBase* Composite) const
{
	return Composite->GetFragmentTag().MatchesTagExact(GetFragmentTag());
}

void FInv_ImageFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	FInv_InventoryItemFragment::Assimilate(Composite);
	if (!MatchesWidgetTag(Composite))return;
	UInv_Leaf_Image*Image=Cast<UInv_Leaf_Image>(Composite);
	if (!IsValid(Image))return;
	
	Image->SetImage(Icon);
	Image->SetBoxSize(IconDimension);
	Image->SetImageSize(IconDimension);
	

}

void FInv_TextFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	FInv_InventoryItemFragment::Assimilate(Composite);
	if (!MatchesWidgetTag(Composite))return;
	UInv_Leaf_Text*LeafText=Cast<UInv_Leaf_Text>(Composite);
	if (!IsValid(LeafText))return;
	LeafText->SetText(FragmentText);
}

void FInv_LabeledNumberFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	FInv_InventoryItemFragment::Assimilate(Composite);
	if (!MatchesWidgetTag(Composite))return;
	UInv_Leaf_LabeledValue*LabeledValue=Cast<UInv_Leaf_LabeledValue>(Composite);
	LabeledValue->SetText_Label(Text_Label,bCollapseLabel);
	FNumberFormattingOptions Options;
	Options.MinimumFractionalDigits=MinFractionDigits;
	Options.MaximumFractionalDigits=MaxFractionDigits;
	LabeledValue->SetText_Value(FText::AsNumber(Value,&Options),bCollapseValue);
}

void FInv_LabeledNumberFragment::Manifest()
{
	FInv_InventoryItemFragment::Manifest();
	if (bRandomizeOnManifest)
	{
		Value=FMath::RandRange(Min,Max);
	}
	bRandomizeOnManifest=false;
}

void FInv_ConsumeModifier::OnConsume(APlayerController* PC)
{
	APawn* Pawn = PC->GetPawn();
	if (!Pawn)return;
	UAbilitySystemComponent*ASC=UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Pawn);
	if (!ASC)return;
	FGameplayEffectContextHandle ContextHandle=ASC->MakeEffectContext();
	if (!ConsumeGameplayEffectClass)return;
	ContextHandle.AddSourceObject(Pawn);
	FGameplayEffectSpecHandle SpecHandle=ASC->MakeOutgoingSpec(ConsumeGameplayEffectClass,1.f,ContextHandle);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,ConsumeAttributeTag,GetValue());
	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void FInv_ConsumableFragment::OnConsume(APlayerController* PC)
{
	for (auto &Modifier:ConsumeModifiers)
	{
		auto&ModRef=Modifier.GetMutable();
		ModRef.OnConsume(PC);
	}
}

void FInv_ConsumableFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	FInv_InventoryItemFragment::Assimilate(Composite);
	for (const auto &Modifier:ConsumeModifiers)
	{
		const auto &ModRef=Modifier.Get();
		ModRef.Assimilate(Composite);
	}
}

void FInv_ConsumableFragment::Manifest()
{
	FInv_InventoryItemFragment::Manifest();
	for (auto&Modifier:ConsumeModifiers)
	{
		auto&ModRef= Modifier.GetMutable();
		ModRef.Manifest();
	}
}

void FInv_HealthPotionFragment::OnConsume(APlayerController* PC)
{
	FInv_ConsumeModifier::OnConsume(PC);
}

void FInv_ManaPotionFragment::OnConsume(APlayerController* PC)
{
	FInv_ConsumeModifier::OnConsume(PC);
}
//EquipFragment
void FInv_EquipModifier::OnEquip(APlayerController* PC)
{
	if (!PC->HasAuthority())return;
	APawn* Pawn = PC->GetPawn();
	if (!Pawn)return;
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Pawn);
	if (!ASC || !EquipGameplayEffectClass)return;
	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	ContextHandle.AddSourceObject(Pawn);
	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(EquipGameplayEffectClass, 1.f, ContextHandle);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, EquipAttributeTag, GetValue());
	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void FInv_EquipModifier::OnUnequip(APlayerController* PC)
{
	
	if (!PC->HasAuthority())return;
	APawn* Pawn = PC->GetPawn();
	if (!Pawn) return;
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Pawn);
	if (!ASC || !EquipGameplayEffectClass) return;
	ASC->RemoveActiveGameplayEffectBySourceEffect(
		EquipGameplayEffectClass,
		ASC);
}

void FInv_IntelligenceModifier::OnEquip(APlayerController* PC)
{
	FInv_EquipModifier::OnEquip(PC);
}

void FInv_IntelligenceModifier::OnUnequip(APlayerController* PC)
{
	FInv_EquipModifier::OnUnequip(PC);
}

void FInv_VigorModifier::OnEquip(APlayerController* PC)
{
	FInv_EquipModifier::OnEquip(PC);
}

void FInv_VigorModifier::OnUnequip(APlayerController* PC)
{
	FInv_EquipModifier::OnUnequip(PC);
}

void FInv_ResilienceModifier::OnEquip(APlayerController* PC)
{
	FInv_EquipModifier::OnEquip(PC);
}

void FInv_ResilienceModifier::OnUnequip(APlayerController* PC)
{
	FInv_EquipModifier::OnUnequip(PC);
}


void FInv_EquipmentFragment::OnEquip(APlayerController* PC)
{
	if (bEquipped)return;
	bEquipped = true;
	for (auto&Modifier:EquipModifiers)
	{
		auto&ModRef=Modifier.GetMutable();
		ModRef.OnEquip(PC);
	}
}

void FInv_EquipmentFragment::OnUnequip(APlayerController* PC)
{
	if (!bEquipped)return;
	bEquipped = false;
	for (auto&Modifier:EquipModifiers)
	{
		auto&ModRef=Modifier.GetMutable();
		ModRef.OnUnequip(PC);
	}
}

void FInv_EquipmentFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	FInv_InventoryItemFragment::Assimilate(Composite);
	for (const auto&Modifier:EquipModifiers)
	{
		const auto&ModRef= Modifier.Get();
		ModRef.Assimilate(Composite);
	}
}

void FInv_EquipmentFragment::Manifest()
{
	FInv_InventoryItemFragment::Manifest();
	for (auto&Modifier:EquipModifiers)
	{
		auto&ModRef= Modifier.GetMutable();
		ModRef.Manifest();
	}
}

AInv_EquipActor* FInv_EquipmentFragment::SpawnAttachedActor(USkeletalMeshComponent* AttachedMesh) const
{
	if (!IsValid(EquipActorClass)||!IsValid(AttachedMesh))return nullptr;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = AttachedMesh->GetOwner();
	AInv_EquipActor*SpawnedActor=AttachedMesh->GetWorld()->SpawnActor<AInv_EquipActor>(
		EquipActorClass,
		AttachedMesh->GetSocketTransform(SocketAttachPoint),
		SpawnParams);
	if (!IsValid(SpawnedActor)) return nullptr;
	SpawnedActor->SetReplicateMovement(true);
	SpawnedActor->AttachToComponent(AttachedMesh,FAttachmentTransformRules::SnapToTargetNotIncludingScale,SocketAttachPoint);
	SpawnedActor->ForceNetUpdate();
	
	return SpawnedActor;
}

void FInv_EquipmentFragment::DestroyAttachedActor() const
{
	if (EquippedActor.IsValid())
	{
		EquippedActor->Destroy();
	}
}

void FInv_EquipmentFragment::SetEquippedActor(AInv_EquipActor* EquipActor)
{
	EquippedActor = EquipActor;
}
