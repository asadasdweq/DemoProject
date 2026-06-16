// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/DemoCharacterBase.h"
#include"AbilitySystemComponent.h"
#include "AbilitySystem/DemoAbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values


ADemoCharacterBase::ADemoCharacterBase()
{
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera,ECR_Ignore);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera,ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	CombatSocketComponent = CreateDefaultSubobject<USceneComponent>("CombatSocketComponent");
	CombatSocketComponent->SetupAttachment(Weapon);
	
	
	
}

UAbilitySystemComponent* ADemoCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ADemoCharacterBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADemoCharacterBase, bIsBeingShocked);
	DOREPLIFETIME(ADemoCharacterBase, bHasWeapon);
}

void ADemoCharacterBase::InitActorAbilityInfo()
{
	
}

FVector ADemoCharacterBase::GetCombatSocketLocation_Implementation()
{
	FVector FallbackLocation = GetActorLocation();
	if (USkeletalMeshComponent* CharacterMesh = GetMesh())
	{
		if (HandSocketName != NAME_None && CharacterMesh->DoesSocketExist(HandSocketName))
		{
			FallbackLocation = CharacterMesh->GetSocketLocation(HandSocketName);
		}
		else
		{
			FallbackLocation = CharacterMesh->GetComponentLocation();
		}
	}

	if (bHasWeapon && IsValid(Weapon) && WeaponSocketName != NAME_None && Weapon->DoesSocketExist(WeaponSocketName))
	{
		const FVector WeaponSocketLocation = Weapon->GetSocketLocation(WeaponSocketName);
		const float MaxDistanceSquared = FMath::Square(MaxWeaponSocketDistanceFromHand);
		if (FVector::DistSquared(WeaponSocketLocation, FallbackLocation) <= MaxDistanceSquared)
		{
			return WeaponSocketLocation;
		}

		UE_LOG(LogTemp, Warning, TEXT("Invalid weapon combat socket. Weapon=%s Socket=%s WeaponLocation=%s FallbackLocation=%s"),
			*GetNameSafe(Weapon),
			*WeaponSocketName.ToString(),
			*WeaponSocketLocation.ToString(),
			*FallbackLocation.ToString());
	}

	return FallbackLocation;
}

UAnimMontage* ADemoCharacterBase::GetHitReactMontage_Implementation()
{
	return HitReactMontage;
}

ECharacterClass ADemoCharacterBase::GetCharacterClass_Implementation()
{
	return CharacterClass;
}

void ADemoCharacterBase::Die_Implementation()
{
	Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld,true));
	MulticastHandleDeath();
	
}

FOnDeathSignature& ADemoCharacterBase::GetOnDeathDel()
{
	return OnDeathDelegate;
}

bool ADemoCharacterBase::IsDead_Implementation() const
{
	return bDead;
}

AActor* ADemoCharacterBase::GetAvatar_Implementation()
{
	return this;
}

UAnimMontage* ADemoCharacterBase::GetAttackMontage_Implementation() const
{
	return AttackMontage;
}

UNiagaraSystem* ADemoCharacterBase::GetBloodEffect_Implementation()
{
	return BloodEffect;
}

USceneComponent* ADemoCharacterBase::GetWeapon_Implementation()
{
	UpdateCombatSocketComponentAttachment();
	if (CombatSocketComponent)
	{
		return CombatSocketComponent;
	}
	if (bHasWeapon) return Weapon;
	return GetMesh();
}

void ADemoCharacterBase::SetIsBeingShocked_Implementation(bool bInShock)
{
	bIsBeingShocked=bInShock;
}

bool ADemoCharacterBase::IsBeingShocked_Implementation() const
{
	return bIsBeingShocked;
}

void ADemoCharacterBase::MulticastHandleDeath_Implementation()
{
	UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation(),GetActorRotation());

	Weapon->SetSimulatePhysics(true);
	Weapon->SetEnableGravity(true);
	Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldDynamic,ECR_Block);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Dissolve();
	bDead=true;

	OnDeathDelegate.Broadcast(this);
}


void ADemoCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADemoCharacterBase::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffect, float Level) const
{
	check(IsValid(GetAbilitySystemComponent()));
	check(GameplayEffect)
	FGameplayEffectContextHandle ContextHandle=GetAbilitySystemComponent()->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle EffectSpecHandle=GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffect,Level,ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*EffectSpecHandle.Data.Get(),GetAbilitySystemComponent());
}

void ADemoCharacterBase::InitializeDefaultAttributes() const
{
	ApplyEffectToSelf(PrimaryAttributes,1.f);
	ApplyEffectToSelf(SecondaryAttributes,1.f);
	ApplyEffectToSelf(VitalAttributes,1.f);
}

void ADemoCharacterBase::SetHasWeapon(bool HasWeapon)
{
	bHasWeapon=HasWeapon;
	UpdateCombatSocketComponentAttachment();
}

void ADemoCharacterBase::SetWeapon(AActor* Actor)
{
	if (!Actor)
	{
		return;
	}
	USkeletalMeshComponent*WeaponSkeletalMesh=Actor->FindComponentByClass<USkeletalMeshComponent>();
	if (WeaponSkeletalMesh)
	{
		Weapon=WeaponSkeletalMesh;
		UpdateCombatSocketComponentAttachment();
	}
}

void ADemoCharacterBase::UpdateCombatSocketComponentAttachment()
{
	if (!CombatSocketComponent)
	{
		return;
	}

	USceneComponent* AttachParent = nullptr;
	FName AttachSocket = NAME_None;

	if (bHasWeapon && IsValid(Weapon) && WeaponSocketName != NAME_None && Weapon->DoesSocketExist(WeaponSocketName))
	{
		AttachParent = Weapon;
		AttachSocket = WeaponSocketName;
	}
	else if (USkeletalMeshComponent* CharacterMesh = GetMesh())
	{
		AttachParent = CharacterMesh;
		if (HandSocketName != NAME_None && CharacterMesh->DoesSocketExist(HandSocketName))
		{
			AttachSocket = HandSocketName;
		}
	}

	if (!AttachParent)
	{
		CombatSocketComponent->SetWorldLocation(GetActorLocation());
		return;
	}

	CombatSocketComponent->AttachToComponent(AttachParent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, AttachSocket);
}

void ADemoCharacterBase::AddCharacterAbilities() 
{
	UDemoAbilitySystemComponent*DemoAbilitySystemComponent=CastChecked<UDemoAbilitySystemComponent>(AbilitySystemComponent);
	if (!HasAuthority())return;
	DemoAbilitySystemComponent->AddCharacterAbilities(StartupAbilities);
}

void ADemoCharacterBase::Dissolve()
{
	if (IsValid(DissolveMaterialInstance))
	{
		UMaterialInstanceDynamic*DynamicMaterialInstance=UMaterialInstanceDynamic::Create(DissolveMaterialInstance,this);
		GetMesh()->SetMaterial(0,DynamicMaterialInstance);
		StartDissolveTimeline(DynamicMaterialInstance);
	}
	if (IsValid(WeaponDissolveMaterialInstance))
	{
		UMaterialInstanceDynamic*DynamicMaterialInstance=UMaterialInstanceDynamic::Create(WeaponDissolveMaterialInstance,this);
		Weapon->SetMaterial(0,DynamicMaterialInstance);
		StartWeaponDissolveTimeline(DynamicMaterialInstance);
	}
	
}



