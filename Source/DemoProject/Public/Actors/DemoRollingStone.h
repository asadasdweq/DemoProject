#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "DemoRollingStone.generated.h"

class UGameplayEffect;
class UNiagaraSystem;
class UProjectileMovementComponent;
class USoundBase;
class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class DEMOPROJECT_API ADemoRollingStone : public AActor
{
	GENERATED_BODY()

public:
	ADemoRollingStone();

	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Destroyed() override;

	void SetChargePercent(float InChargePercent);
	void LaunchStone(const FVector& Direction);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> Sphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> StoneMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	TSubclassOf<UGameplayEffect> DamageGameplayEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	FGameplayTag DamageType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	float BaseDamage = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Charge")
	float MinStoneScale = 0.4f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Charge")
	float MaxStoneScale = 1.6f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float RollSpeed = 900.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float MaxRollDistance = 1600.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	TObjectPtr<UNiagaraSystem> DestroyEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	TObjectPtr<USoundBase> DestroySound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	bool bSpawnDestroyEffect = true;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		FVector NormalImpulse, const FHitResult& Hit);

private:
	bool IsValidDamageTarget(AActor* OtherActor) const;
	void ApplyChargeScale();
	void StopStone();

	UFUNCTION()
	void OnRep_ChargePercent();

	UPROPERTY()
	TArray<TObjectPtr<AActor>> DamagedActors;

	UPROPERTY(ReplicatedUsing=OnRep_ChargePercent)
	float ChargePercent = 0.f;

	FVector LaunchLocation = FVector::ZeroVector;
	bool bLaunched = false;
};
