#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "AbilitySystem/Abilities/DemoGameplayAbility.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ScalableFloat.h"
#include "DemoChargeRockAbility.generated.h"

class ADemoRollingStone;
class UAudioComponent;
class USoundBase;

UCLASS()
class DEMOPROJECT_API UDemoChargeRockAbility : public UDemoGameplayAbility
{
	GENERATED_BODY()

public:
	UDemoChargeRockAbility();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION(BlueprintCallable, Category = "Charge Rock")
	void StartChargeFromEvent();

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Charge Rock")
	void OnChargeStarted(AActor* AvatarActor, ADemoRollingStone* InPreviewStone);

	UFUNCTION(BlueprintImplementableEvent, Category = "Charge Rock")
	void OnChargeUpdated(float ChargePercent);

	UFUNCTION(BlueprintImplementableEvent, Category = "Charge Rock")
	void OnChargeEnded(bool bReleased);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Charge Rock")
	TSubclassOf<ADemoRollingStone> RollingStoneClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Charge Rock")
	float MaxChargeTime = 2.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Charge Rock")
	float ChargeUpdateInterval = 0.03f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Charge Rock")
	float SpawnHeightAboveAvatar = 170.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Charge Rock")
	float StoneSpawnForwardOffset = 80.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Charge Rock")
	float StoneSpawnVerticalOffset = -45.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Charge Rock")
	float ReleaseTraceDistance = 6000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Charge Rock")
	bool bRotateAvatarToMouseInCpp = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Charge Rock|Sound")
	TObjectPtr<USoundBase> CastingSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Charge Rock|Sound")
	float CastingSoundVolume = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Charge Rock|Sound")
	float CastingSoundPitch = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	FScalableFloat MinBaseDamage;

	UPROPERTY(BlueprintReadOnly, Category = "Charge Rock")
	bool bChargeStarted = false;

private:
	void UpdateCharge();
	void SpawnPreviewStone();
	void ReleaseStone();
	void ReleaseStoneAtLocation(const FVector& TargetLocation, float ReleaseChargePercent = -1.f);
	void SendMouseTargetData();
	void OnReleaseTargetDataReady(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag);
	void UpdateAvatarFacing(const FVector& TargetLocation);
	void DisableAvatarMovement();
	void RestoreAvatarMovement();
	void PlayCastingSound();
	void StopCastingSound();
	FVector GetMouseTargetLocation() const;
	FVector GetStoneSpawnLocation() const;
	float GetChargePercent() const;
	float GetChargedDamage() const;
	void ClearChargeTimer();

	UPROPERTY()
	TObjectPtr<ADemoRollingStone> PreviewStone;

	UPROPERTY()
	TObjectPtr<UAudioComponent> CastingAudioComponent;

	FTimerHandle ChargeTimerHandle;
	float ChargeStartTime = 0.f;
	bool bReleasedStone = false;
	bool bKeepCastingState = false;
	bool bMovementDisabledByChargeRock = false;
	TEnumAsByte<EMovementMode> PreviousMovementMode = MOVE_Walking;
	uint8 PreviousCustomMovementMode = 0;
};
