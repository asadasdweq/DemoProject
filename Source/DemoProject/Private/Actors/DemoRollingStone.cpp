#include "Actors/DemoRollingStone.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/DemoAbilitySystemLibrary.h"
#include "Components/SphereComponent.h"
#include "DemoProject/DemoProject.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraFunctionLibrary.h"

ADemoRollingStone::ADemoRollingStone()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(Sphere);
	Sphere->SetCollisionObjectType(ECC_Projectile);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Sphere->SetNotifyRigidBodyCollision(true);

	StoneMesh = CreateDefaultSubobject<UStaticMeshComponent>("StoneMesh");
	StoneMesh->SetupAttachment(Sphere);
	StoneMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->InitialSpeed = 0.f;
	ProjectileMovement->MaxSpeed = RollSpeed;
	ProjectileMovement->ProjectileGravityScale = 0.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->StopMovementImmediately();
}

void ADemoRollingStone::BeginPlay()
{
	Super::BeginPlay();

	SetReplicateMovement(true);
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereOverlap);
	Sphere->OnComponentHit.AddDynamic(this, &ThisClass::OnSphereHit);
	ApplyChargeScale();
}

void ADemoRollingStone::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADemoRollingStone, ChargePercent);
}

void ADemoRollingStone::Destroyed()
{
	if (bSpawnDestroyEffect)
	{
		if (DestroySound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, DestroySound, GetActorLocation());
		}
		if (DestroyEffect)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, DestroyEffect, GetActorLocation(), GetActorRotation());
		}
	}

	Super::Destroyed();
}

void ADemoRollingStone::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!HasAuthority() || !bLaunched)
	{
		return;
	}

	if (FVector::DistSquared(LaunchLocation, GetActorLocation()) >= FMath::Square(MaxRollDistance))
	{
		Destroy();
	}
}

void ADemoRollingStone::SetChargePercent(float InChargePercent)
{
	ChargePercent = FMath::Clamp(InChargePercent, 0.f, 1.f);
	ApplyChargeScale();
}

void ADemoRollingStone::ApplyChargeScale()
{
	const float StoneScale = FMath::Lerp(MinStoneScale, MaxStoneScale, ChargePercent);
	SetActorScale3D(FVector(StoneScale));
}

void ADemoRollingStone::OnRep_ChargePercent()
{
	ApplyChargeScale();
}

void ADemoRollingStone::LaunchStone(const FVector& Direction)
{
	if (!HasAuthority())
	{
		return;
	}

	SetReplicates(true);
	SetReplicateMovement(true);
	SetActorEnableCollision(true);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ForceNetUpdate();

	const FVector LaunchDirection = Direction.GetSafeNormal2D();
	if (LaunchDirection.IsNearlyZero())
	{
		StopStone();
		return;
	}

	bLaunched = true;
	LaunchLocation = GetActorLocation();
	SetActorRotation(LaunchDirection.Rotation());
	ProjectileMovement->MaxSpeed = RollSpeed;
	ProjectileMovement->Velocity = LaunchDirection * RollSpeed;
}

void ADemoRollingStone::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority() || !bLaunched || !IsValidDamageTarget(OtherActor))
	{
		return;
	}

	DamagedActors.AddUnique(OtherActor);

	if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
	{
		if (UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
		{
			UDemoAbilitySystemLibrary::CauseDamage(SourceASC, TargetASC, DamageGameplayEffectClass, DamageType, BaseDamage);
		}
	}
}

void ADemoRollingStone::OnSphereHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (!HasAuthority() || !bLaunched)
	{
		return;
	}

	if (!OtherActor || OtherActor == GetOwner() || !OtherActor->ActorHasTag(FName("Enemy")))
	{
		StopStone();
	}
}

bool ADemoRollingStone::IsValidDamageTarget(AActor* OtherActor) const
{
	if (!OtherActor || OtherActor == GetOwner() || DamagedActors.Contains(OtherActor))
	{
		return false;
	}

	if (!UDemoAbilitySystemLibrary::IsNotFriend(OtherActor, GetOwner()))
	{
		return false;
	}

	return UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor) != nullptr;
}

void ADemoRollingStone::StopStone()
{
	bLaunched = false;
	ProjectileMovement->StopMovementImmediately();
	Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetLifeSpan(2.f);
}
