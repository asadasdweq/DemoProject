// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/DemoPlayerController.h"

#include "AbilitySystem/DemoAbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Characters/DemoCharacter.h"
#include "Components/SplineComponent.h"
#include "Input/DemoInputComponent.h"
#include "Interaction/HighlightInterface.h"
#include"GameFramework/Character.h"
#include "InventoryManagement/Components/InventoryComponent.h"
#include "Items/Components/Inv_ItemComponent.h"
#include "UI/Widget/DamageTextComponent.h"
#include"UI/InventoryWidget/Message/Inv_PickUpMessage.h"

ADemoPlayerController::ADemoPlayerController()
{
	bReplicates=true;
	Spline=CreateDefaultSubobject<USplineComponent>("Spline");
}

void ADemoPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CursorTrace();
}

void ADemoPlayerController::ShowDamageNumber_Implementation(float DamageAmount, ACharacter* TargetCharacter)
{
	if (IsValid(TargetCharacter)&&DamageTextComponentClass&&IsLocalController())
	{
		UDamageTextComponent *DamageText=NewObject<UDamageTextComponent>(TargetCharacter,DamageTextComponentClass);
		DamageText->RegisterComponent();
		DamageText->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		DamageText->SetDamageText(DamageAmount);
	}
}
void ADemoPlayerController::PickUpKey()
{
	ADemoCharacter*DemoCharacter=Cast<ADemoCharacter>(GetPawn());
	if (!DemoCharacter) return;
	if (DemoCharacter->GetPickupItems().Num() > 0)
	{
		AActor* NearestItem = nullptr;
		float MinDist = TNumericLimits<float>::Max();
		for (AActor*Item: DemoCharacter->GetPickupItems())
		{
			if (!IsValid(Item))
			{
				continue;
			}

			float Dist = FVector::Dist(Item->GetActorLocation(), GetPawn()->GetActorLocation());
			if (Dist < MinDist)
			{
				MinDist = Dist;
				NearestItem = Item;
			}
			
		}
		if (NearestItem)
		{
			UInv_ItemComponent*ItemComponent=NearestItem->FindComponentByClass<UInv_ItemComponent>();
			if (InventoryComponent.IsValid())
			{
				InventoryComponent->TryAddItem(ItemComponent);
			}
		}
		
	}
	
	
}
void ADemoPlayerController::ToggleInventory()
{
	
	if (!InventoryComponent.IsValid())return;
	InventoryComponent->ToggleInventoryMenu();
}



void ADemoPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(DemoContext);
	UEnhancedInputLocalPlayerSubsystem *Subsystem=ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem)
	{
		Subsystem->AddMappingContext(DemoContext,0);
	}
	bShowMouseCursor=true;
	DefaultMouseCursor=EMouseCursor::Default;
	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
	
	InventoryComponent=FindComponentByClass<UInventoryComponent>();
	
	CreatePickUpMessage();
}

void ADemoPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	UDemoInputComponent*DemoInputComponent=CastChecked<UDemoInputComponent>(InputComponent);
	DemoInputComponent->BindAction(MoveAction,ETriggerEvent::Triggered,this,&ADemoPlayerController::Move);
	DemoInputComponent->BindAction(ShiftAction,ETriggerEvent::Started,this,&ADemoPlayerController::ShiftKeyPressed);
	DemoInputComponent->BindAction(EquipAction,ETriggerEvent::Started,this,&ADemoPlayerController::PickUpKey);
	DemoInputComponent->BindAction(ToggleInventoryAction,ETriggerEvent::Started,this,&ADemoPlayerController::ToggleInventory);
	DemoInputComponent->BindAction(ShiftAction,ETriggerEvent::Completed,this,&ADemoPlayerController::ShiftKeyReleased);
	DemoInputComponent->BindAbilityActions(DemoInputConfig,this,&ThisClass::AbilityInputTagPressed,&ThisClass::AbilityInputTagReleased,&ThisClass::AbilityInputTagHeld);
	
}

void ADemoPlayerController::CreatePickUpMessage()
{
	if (!IsLocalController())return;
	PickUpMessage=CreateWidget<UInv_PickUpMessage>(this,PickUpMessageClass);
	if (IsValid(PickUpMessage))
	{
		PickUpMessage->AddToViewport();
	}
}


void ADemoPlayerController::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility,false,CursorHit);
	if (!CursorHit.bBlockingHit)return;
	LastActor=ThisActor;
	if (IsValid(CursorHit.GetActor())&&CursorHit.GetActor()->Implements<UHighlightInterface>())
	{
		ThisActor=CursorHit.GetActor();
	}
	else
	{
		ThisActor=nullptr;
	}
	if (LastActor!=ThisActor)
	{
		HighlightActor(ThisActor);
		UnHighlightActor(LastActor);
	}
}

void ADemoPlayerController::HighlightActor(TWeakObjectPtr<AActor>Actor)
{
	if (Actor.IsValid()&&Actor.Get()->Implements<UHighlightInterface>())
	{
		IHighlightInterface::Execute_HightlightActor(Actor.Get());
	}
}

void ADemoPlayerController::UnHighlightActor(TWeakObjectPtr<AActor>Actor)
{
	if (Actor.IsValid()&&Actor.Get()->Implements<UHighlightInterface>())
	{
		IHighlightInterface::Execute_UnHightlightActor(Actor.Get());
	}
}

void ADemoPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector=InputActionValue.Get<FVector2D>();
	const FRotator Rotation=GetControlRotation();
	const FRotator YawRotation=FRotator(0.f,Rotation.Yaw,0.f);
	
	const FVector ForwardDirection=FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection=FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	if (APawn*ControlledPawn=Cast<APawn>(GetPawn()))
	{
		ControlledPawn->AddMovementInput(ForwardDirection,InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection,InputAxisVector.X);
	}
	
}

UDemoAbilitySystemComponent* ADemoPlayerController::GetAsc()
{
	if (DemoAbilitySystemComponent==nullptr)
	{		
		DemoAbilitySystemComponent= Cast<UDemoAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Cast<APawn>(GetPawn())));
	}
	return DemoAbilitySystemComponent;
}

void ADemoPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (InventoryComponent.IsValid() && InventoryComponent->IsMenuOpen()) return;
	
	if (GetAsc()) GetAsc()->AbilityInputTagPressed(InputTag);
}

void ADemoPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	
	if (GetAsc()) GetAsc()->AbilityInputTagReleased(InputTag);
}

void ADemoPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (InventoryComponent.IsValid() && InventoryComponent->IsMenuOpen()) return;
	
	if (GetAsc()) GetAsc()->AbilityInputTagHeld(InputTag);
}

void ADemoPlayerController::AutoRun()
{
	if (!bAutoRunning) return;
	if (APawn*ControlledPawn=GetPawn())
	{
		
	}
}
