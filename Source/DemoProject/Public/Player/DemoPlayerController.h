// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include"GameplayTagContainer.h"
#include "DemoPlayerController.generated.h"

class UInv_PickUpMessage;
class UInventoryComponent;
class UDamageTextComponent;
class USplineComponent;
class UNiagaraSystem;
class UDemoAbilitySystemComponent;
class UDemoInputConfig;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;
/**
 * 
 */

enum class ETargetingStatus :uint8
{
	TargetingEnemy,
	TargetingNonEnemy,
	NotTargeting
};
UCLASS()
class DEMOPROJECT_API ADemoPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	ADemoPlayerController();
	virtual void Tick( float DeltaTime )override;
	UFUNCTION(Client,Reliable)
	void ShowDamageNumber(float DamageAmount,ACharacter*TargetCharacter);
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void ToggleInventory();
	
	
	UInv_PickUpMessage* GetPickupMessage()const{return PickUpMessage;}
protected:
	virtual void BeginPlay()override;
	virtual void SetupInputComponent() override;
private:
	void CreatePickUpMessage();
	
	UPROPERTY(EditAnywhere,Category="Input")
	TObjectPtr<UInputMappingContext>DemoContext;
	UPROPERTY(EditAnywhere,Category="Input")
	TObjectPtr<UInputAction>MoveAction;
	UPROPERTY(EditAnywhere,Category="Input")
	TObjectPtr<UInputAction>ShiftAction;
	UPROPERTY(EditDefaultsOnly,Category="InventoryInput")
	TObjectPtr<UInputAction>EquipAction;
	UPROPERTY(EditDefaultsOnly,Category="InventoryInput")
	TObjectPtr<UInputAction>ToggleInventoryAction;
	
	UPROPERTY()
	TObjectPtr<UInv_PickUpMessage>PickUpMessage;
	UPROPERTY(EditDefaultsOnly,Category="InventoryComponent")
	TSubclassOf<UInv_PickUpMessage>PickUpMessageClass;
	void PickUpKey();
	TWeakObjectPtr<UInventoryComponent>InventoryComponent;
	
	void CursorTrace();
	static void HighlightActor(TWeakObjectPtr<AActor> Actor);
	static void UnHighlightActor(TWeakObjectPtr<AActor> Actor);
	
	void Move(const FInputActionValue&InputActionValue);
	
	//AbilityInputActionConfig
	UPROPERTY(EditAnywhere,Category="Input")
	TObjectPtr<UDemoInputConfig>DemoInputConfig;
	//AbilitySystem
	UDemoAbilitySystemComponent*GetAsc();
	UPROPERTY()
	TObjectPtr<UDemoAbilitySystemComponent>DemoAbilitySystemComponent;
	//AbilityInput
	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);
	void ShiftKeyPressed(){bShiftKeyDown=true;};
	void ShiftKeyReleased(){bShiftKeyDown=false;};
	bool bShiftKeyDown = false;

	FVector CachedDestination=FVector::ZeroVector;
	float FollowTime = 0.f;
	float ShortPressThreshold = 0.5f;
	bool bAutoRunning = false;
	void AutoRun();
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageTextComponent>DamageTextComponentClass;
	
	ETargetingStatus TargetingStatus = ETargetingStatus::NotTargeting;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem>ClickedNiagaraSystem;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent>Spline;

	TWeakObjectPtr<AActor> LastActor;
	TWeakObjectPtr<AActor> ThisActor;
};
