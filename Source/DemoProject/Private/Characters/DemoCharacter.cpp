// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/DemoCharacter.h"

#include "AbilitySystem/DemoAbilitySystemComponent.h"
#include"DemoProject/DemoProject.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Interaction/Inv_HighLightableStaticMesh.h"
#include "Items/Components/Inv_ItemComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/DemoPlayerController.h"
#include "Player/DemoPlayerState.h"
#include "UI/HUD/DemoHUD.h"
#include "UI/InventoryWidget/Message/Inv_PickUpMessage.h"


ADemoCharacter::ADemoCharacter()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate=FRotator(0.0f,400.f,0.0f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	
	PrimaryActorTick.bCanEverTick = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->bDoCollisionTest=false;


	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>("TopDownCameraComponent");
	TopDownCameraComponent->SetupAttachment(CameraBoom,USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false;

	ChargeProgressBar = CreateDefaultSubobject<UWidgetComponent>("ChargeProgressBar");
	ChargeProgressBar->SetupAttachment(GetRootComponent());
	ChargeProgressBar->SetRelativeLocation(FVector(0.f, 0.f, 140.f));
	ChargeProgressBar->SetWidgetSpace(EWidgetSpace::Screen);
	ChargeProgressBar->SetDrawSize(FVector2D(120.f, 16.f));
	ChargeProgressBar->SetVisibility(false);
	

	PickupSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickupSphere"));
	PickupSphere->SetupAttachment(GetRootComponent());
	PickupSphere->SetSphereRadius(75.f);
	PickupSphere->SetCollisionObjectType(ECC_Pickup);
	PickupSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PickupSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	PickupSphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	PickupSphere->SetGenerateOverlapEvents(true);
	
	CharacterClass = ECharacterClass::Elementalist;
	PlayerWeaponComponent = Weapon;
	
}

void ADemoCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	InitActorAbilityInfo();
	
	AddCharacterAbilities();
	
}
void ADemoCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	InitActorAbilityInfo();
}

void ADemoCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADemoCharacter, bIsCasting);
	DOREPLIFETIME(ADemoCharacter, EquippedWeaponMesh);
}

void ADemoCharacter::SetWeapon(AActor* Actor)
{
	if (!PlayerWeaponComponent)
	{
		PlayerWeaponComponent = Weapon;
	}

	Weapon = PlayerWeaponComponent;
	if (!Actor)
	{
		EquippedWeaponMesh = nullptr;
		ApplyEquippedWeaponMesh();
		return;
	}

	if (const USkeletalMeshComponent* WeaponSkeletalMesh = Actor->FindComponentByClass<USkeletalMeshComponent>())
	{
		EquippedWeaponMesh = WeaponSkeletalMesh->GetSkeletalMeshAsset();
		ApplyEquippedWeaponMesh();
	}
}

void ADemoCharacter::OnRep_EquippedWeaponMesh()
{
	if (!PlayerWeaponComponent)
	{
		PlayerWeaponComponent = Weapon;
	}
	Weapon = PlayerWeaponComponent;
	ApplyEquippedWeaponMesh();
}

void ADemoCharacter::ApplyEquippedWeaponMesh()
{
	if (!PlayerWeaponComponent)
	{
		return;
	}

	PlayerWeaponComponent->SetSkeletalMesh(EquippedWeaponMesh);
}

bool ADemoCharacter::IsCasting_Implementation() const
{
	return bIsCasting;
}

void ADemoCharacter::SetIsCasting_Implementation(bool bInCasting)
{
	bIsCasting = bInCasting;
	if (HasAuthority())
	{
		Client_SetIsCasting(bInCasting);
	}
}

void ADemoCharacter::Client_SetIsCasting_Implementation(bool bInCasting)
{
	bIsCasting = bInCasting;
}

void ADemoCharacter::Client_UpdateFacingTarget_Implementation(const FVector& Target)
{
	if (Implements<UCombatInterface>())
	{
		ICombatInterface::Execute_UpdateFacingTarget(this, Target);
	}
}

void ADemoCharacter::InitActorAbilityInfo()
{
	
	ADemoPlayerState*DemoPlayerState=GetPlayerState<ADemoPlayerState>();
	check(DemoPlayerState);
	DemoPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(DemoPlayerState,this);
	AbilitySystemComponent=DemoPlayerState->GetAbilitySystemComponent();
	AttributeSet=DemoPlayerState->GetAttributeSet();
	
	if (ADemoPlayerController*DemoPlayerController=Cast<ADemoPlayerController>(GetController()))
	{
		if (ADemoHUD*DemoHUD=Cast<ADemoHUD>(DemoPlayerController->GetHUD()))
		{
			DemoHUD->InitOverlay(DemoPlayerController,DemoPlayerState,AbilitySystemComponent,AttributeSet);
		}
	}
	
	InitializeDefaultAttributes();
	
}

void ADemoCharacter::ShowChargeProgressBar()
{
	if (ChargeProgressBar)
	{
		ChargeProgressBar->SetVisibility(true);
	}
	OnChargeProgressBarUpdated(0.f);
}

void ADemoCharacter::UpdateChargeProgressBar(float Percent)
{
	OnChargeProgressBarUpdated(FMath::Clamp(Percent, 0.f, 1.f));
}

void ADemoCharacter::HideChargeProgressBar()
{
	if (ChargeProgressBar)
	{
		ChargeProgressBar->SetVisibility(false);
	}
	OnChargeProgressBarUpdated(0.f);
}

void ADemoCharacter::OnPickupSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor)return;
	UInv_ItemComponent*ItemComponent=OtherActor->FindComponentByClass<UInv_ItemComponent>();
	
	
	
	if (!IsValid(ItemComponent))return;
	
	if (!PickupItems.Contains(OtherActor))
	{
		PickupItems.Add(OtherActor);
		
		if (ADemoPlayerController* PC = Cast<ADemoPlayerController>(GetController()))
		{
			if (PC->GetPickupMessage())
			{
				PC->GetPickupMessage()->ShowPickupMessage(ItemComponent->GetPickupMessage());
			}
		}
	}
	if (UActorComponent*HighLightable=OtherActor->FindComponentByInterface(UInv_Highlightable::StaticClass());IsValid(HighLightable))
	{
		IInv_Highlightable::Execute_HighLight(HighLightable);
	}
	
}

void ADemoCharacter::OnPickupSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor)return;
	UInv_ItemComponent*ItemComponent=OtherActor->FindComponentByClass<UInv_ItemComponent>();
	if (!ItemComponent)return;
	
	
	if (PickupItems.Contains(OtherActor))
	{
		PickupItems.Remove(OtherActor);
		if (ADemoPlayerController* PC = Cast<ADemoPlayerController>(GetController()))
		{
			if (PC->GetPickupMessage())
			{
				PC->GetPickupMessage()->HidePickupMessage();
			}
		}
	}
	
	if (UActorComponent*UnHighLightable=OtherActor->FindComponentByInterface(UInv_Highlightable::StaticClass());IsValid(UnHighLightable))
	{
		IInv_Highlightable::Execute_UnHighLight(UnHighLightable);
	}
}

void ADemoCharacter::BeginPlay()
{
	Super::BeginPlay();
	PickupSphere->OnComponentBeginOverlap.AddDynamic(this,&ADemoCharacter::OnPickupSphereOverlap);
	PickupSphere->OnComponentEndOverlap.AddDynamic(this,&ADemoCharacter::OnPickupSphereEndOverlap);
	
}



