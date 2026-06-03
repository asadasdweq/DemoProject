// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DemoCharacterBase.h"
#include "DemoCharacter.generated.h"
class UCameraComponent;
class USpringArmComponent;
class USphereComponent;
class USkeletalMesh;
class UWidgetComponent;


UCLASS()
class DEMOPROJECT_API ADemoCharacter : public ADemoCharacterBase
{
	GENERATED_BODY()

public:
	
	ADemoCharacter();
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void SetWeapon(AActor* Actor) override;

	UFUNCTION()
	void OnRep_EquippedWeaponMesh();

	// CombatInterface
	virtual bool IsCasting_Implementation() const override;
	virtual void SetIsCasting_Implementation(bool bInCasting) override;

	UFUNCTION(Client, Reliable)
	void Client_SetIsCasting(bool bInCasting);

	UFUNCTION(Client, Reliable)
	void Client_UpdateFacingTarget(const FVector& Target);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	TArray<AActor*>GetPickupItems(){return PickupItems;};

	UFUNCTION(BlueprintCallable, Category = "Charge")
	void ShowChargeProgressBar();

	UFUNCTION(BlueprintCallable, Category = "Charge")
	void UpdateChargeProgressBar(float Percent);

	UFUNCTION(BlueprintCallable, Category = "Charge")
	void HideChargeProgressBar();
	
	UFUNCTION()
	virtual void OnPickupSphereOverlap(UPrimitiveComponent* OverlappedComponent, 
		AActor* OtherActor, UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnPickupSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
protected:
	
	virtual void BeginPlay() override;
	virtual void InitActorAbilityInfo() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Charge")
	void OnChargeProgressBarUpdated(float Percent);
	

	


private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent>TopDownCameraComponent;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent>CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Charge", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetComponent> ChargeProgressBar;
	
	UPROPERTY(VisibleAnywhere, Category = "Pickup")
	USphereComponent* PickupSphere;
	UPROPERTY(VisibleAnywhere, Category = "Pickup")
	TArray<AActor*> PickupItems;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bIsCasting = false;

	UPROPERTY()
	TObjectPtr<USkeletalMeshComponent> PlayerWeaponComponent;

	UPROPERTY(ReplicatedUsing=OnRep_EquippedWeaponMesh)
	TObjectPtr<USkeletalMesh> EquippedWeaponMesh;

	void ApplyEquippedWeaponMesh();
	
	
};




