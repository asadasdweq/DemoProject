#include "AbilitySystem/Abilities/DemoChargeRockAbility.h"

#include "Abilities/GameplayAbilityTargetTypes.h"
#include "Actors/DemoRollingStone.h"
#include "AbilitySystemComponent.h"
#include "Components/AudioComponent.h"
#include "DemoGameplayTags.h"
#include "DemoProject/DemoProject.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Characters/DemoCharacter.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

namespace
{
	void EncodeChargePercentIntoHitResult(FHitResult& HitResult, const float ChargePercent)
	{
		HitResult.TraceStart.X = FMath::Clamp(ChargePercent, 0.f, 1.f) * 1000.f;
	}

	float DecodeChargePercentFromHitResult(const FHitResult& HitResult)
	{
		return FMath::Clamp(static_cast<float>(HitResult.TraceStart.X) / 1000.f, 0.f, 1.f);
	}
}

UDemoChargeRockAbility::UDemoChargeRockAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UDemoChargeRockAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	bReleasedStone = false;
	bChargeStarted = false;
	bKeepCastingState = false;
	bMovementDisabledByChargeRock = false;

	OnChargeStarted(GetAvatarActorFromActorInfo(), PreviewStone);

	if (!ActorInfo->IsLocallyControlled() && ActorInfo->AbilitySystemComponent.IsValid())
	{
		ActorInfo->AbilitySystemComponent->AbilityTargetDataSetDelegate(Handle, ActivationInfo.GetActivationPredictionKey())
			.AddUObject(this, &ThisClass::OnReleaseTargetDataReady);
	}
}

void UDemoChargeRockAbility::InputReleased(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);

	if (!bChargeStarted)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (ActorInfo && ActorInfo->IsLocallyControlled())
	{
		SendMouseTargetData();
		if (!GetAvatarActorFromActorInfo()->HasAuthority() && IsValid(PreviewStone))
		{
			PreviewStone->Destroy();
			PreviewStone = nullptr;
		}
	}

	if (GetAvatarActorFromActorInfo()->HasAuthority() && (!ActorInfo || !ActorInfo->IsLocallyControlled()))
	{
		return;
	}

	if (GetAvatarActorFromActorInfo()->HasAuthority())
	{
		ReleaseStone();
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}

void UDemoChargeRockAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ClearChargeTimer();
	bChargeStarted = false;
	bKeepCastingState = false;
	StopCastingSound();
	RestoreAvatarMovement();

	if (bWasCancelled && IsValid(PreviewStone))
	{
		PreviewStone->Destroy();
		PreviewStone = nullptr;
	}

	if (ADemoCharacter* DemoCharacter = Cast<ADemoCharacter>(GetAvatarActorFromActorInfo()))
	{
		DemoCharacter->HideChargeProgressBar();
	}
	if (GetAvatarActorFromActorInfo() && GetAvatarActorFromActorInfo()->Implements<UCombatInterface>())
	{
		ICombatInterface::Execute_SetIsCasting(GetAvatarActorFromActorInfo(), false);
	}
	OnChargeEnded(bReleasedStone);

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UDemoChargeRockAbility::StartChargeFromEvent()
{
	if (bChargeStarted || !GetWorld())
	{
		return;
	}

	bChargeStarted = true;
	bKeepCastingState = false;
	ChargeStartTime = GetWorld()->GetTimeSeconds();
	DisableAvatarMovement();
	PlayCastingSound();

	SpawnPreviewStone();
	if (ADemoCharacter* DemoCharacter = Cast<ADemoCharacter>(GetAvatarActorFromActorInfo()))
	{
		DemoCharacter->ShowChargeProgressBar();
	}
	UpdateCharge();

	GetWorld()->GetTimerManager().SetTimer(ChargeTimerHandle, this, &ThisClass::UpdateCharge, ChargeUpdateInterval, true);

	if (GetAvatarActorFromActorInfo() && GetAvatarActorFromActorInfo()->Implements<UCombatInterface>())
	{
		ICombatInterface::Execute_SetIsCasting(GetAvatarActorFromActorInfo(), true);
	}
	bKeepCastingState = true;
}

void UDemoChargeRockAbility::UpdateCharge()
{
	if (!bChargeStarted)
	{
		return;
	}

	const float ChargePercent = GetChargePercent();
	const FVector TargetLocation = GetMouseTargetLocation();

	UpdateAvatarFacing(TargetLocation);

	if (IsValid(PreviewStone))
	{
		PreviewStone->SetActorLocation(GetStoneSpawnLocation());
		PreviewStone->SetChargePercent(ChargePercent);
	}

	OnChargeUpdated(ChargePercent);
	if (ADemoCharacter* DemoCharacter = Cast<ADemoCharacter>(GetAvatarActorFromActorInfo()))
	{
		if (bKeepCastingState && !ICombatInterface::Execute_IsCasting(DemoCharacter))
		{
			ICombatInterface::Execute_SetIsCasting(DemoCharacter, true);
		}
		DemoCharacter->UpdateChargeProgressBar(ChargePercent);
	}
}

void UDemoChargeRockAbility::SpawnPreviewStone()
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	const bool bIsLocallyControlled = CurrentActorInfo && CurrentActorInfo->IsLocallyControlled();

	if (!AvatarActor || !RollingStoneClass || !bIsLocallyControlled)
	{
		return;
	}

	const FVector SpawnLocation = GetStoneSpawnLocation();
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SpawnLocation);

	PreviewStone = GetWorld()->SpawnActorDeferred<ADemoRollingStone>(
		RollingStoneClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(GetOwningActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (PreviewStone)
	{
		PreviewStone->SetReplicates(false);
		PreviewStone->SetReplicateMovement(false);
		PreviewStone->SetActorEnableCollision(false);
		PreviewStone->DamageGameplayEffectClass = DamageGameplayEffectClass;
		PreviewStone->DamageType = DamageType.IsValid() ? DamageType : FDemoGameplayTags::Get().Attributes_Secondary_PhysicalDamage;
		PreviewStone->BaseDamage = MinBaseDamage.GetValueAtLevel(GetAbilityLevel());
		PreviewStone->SetOwner(AvatarActor);
		PreviewStone->SetInstigator(Cast<APawn>(AvatarActor));
		PreviewStone->bSpawnDestroyEffect = false;
		PreviewStone->FinishSpawning(SpawnTransform);
	}
}

void UDemoChargeRockAbility::ReleaseStone()
{
	ReleaseStoneAtLocation(GetMouseTargetLocation());
}

void UDemoChargeRockAbility::ReleaseStoneAtLocation(const FVector& TargetLocation, float ReleaseChargePercent)
{
	if (bReleasedStone)
	{
		return;
	}

	const float FinalChargePercent = ReleaseChargePercent >= 0.f ? FMath::Clamp(ReleaseChargePercent, 0.f, 1.f) : GetChargePercent();
	bReleasedStone = true;
	ClearChargeTimer();
	UpdateCharge();

	if (IsValid(PreviewStone))
	{
		PreviewStone->Destroy();
		PreviewStone = nullptr;
	}

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor || !AvatarActor->HasAuthority() || !RollingStoneClass)
	{
		return;
	}

	const FVector StoneLocation = GetStoneSpawnLocation();
	FVector AdjustedTargetLocation = TargetLocation;
	AdjustedTargetLocation.Z = StoneLocation.Z;
	const FVector Direction = (AdjustedTargetLocation - StoneLocation).GetSafeNormal2D();

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(StoneLocation);

	ADemoRollingStone* LaunchedStone = GetWorld()->SpawnActorDeferred<ADemoRollingStone>(
		RollingStoneClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(AvatarActor),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (!LaunchedStone)
	{
		return;
	}

	LaunchedStone->SetReplicates(true);
	LaunchedStone->SetReplicateMovement(true);
	LaunchedStone->SetActorEnableCollision(true);
	LaunchedStone->DamageGameplayEffectClass = DamageGameplayEffectClass;
	LaunchedStone->DamageType = DamageType.IsValid() ? DamageType : FDemoGameplayTags::Get().Attributes_Secondary_PhysicalDamage;
	const float MinDamage = MinBaseDamage.GetValueAtLevel(GetAbilityLevel());
	const float MaxDamage = BaseDamage.GetValueAtLevel(GetAbilityLevel());
	LaunchedStone->BaseDamage = FMath::Lerp(MinDamage, MaxDamage, FinalChargePercent);
	LaunchedStone->SetOwner(AvatarActor);
	LaunchedStone->SetInstigator(Cast<APawn>(AvatarActor));
	LaunchedStone->SetChargePercent(FinalChargePercent);
	LaunchedStone->FinishSpawning(SpawnTransform);
	LaunchedStone->LaunchStone(Direction);
}

void UDemoChargeRockAbility::SendMouseTargetData()
{
	if (!CurrentActorInfo || !CurrentActorInfo->AbilitySystemComponent.IsValid())
	{
		return;
	}

	FScopedPredictionWindow ScopedPrediction(CurrentActorInfo->AbilitySystemComponent.Get());
	FGameplayAbilityTargetDataHandle DataHandle;

	FHitResult CursorHit;
	if (APlayerController* PlayerController = CurrentActorInfo->PlayerController.Get())
	{
		PlayerController->GetHitResultUnderCursor(ECC_Target, false, CursorHit);
	}
	if (!CursorHit.bBlockingHit)
	{
		CursorHit.Location = GetMouseTargetLocation();
		CursorHit.ImpactPoint = CursorHit.Location;
	}
	EncodeChargePercentIntoHitResult(CursorHit, GetChargePercent());

	FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit();
	TargetData->HitResult = CursorHit;
	DataHandle.Add(TargetData);

	CurrentActorInfo->AbilitySystemComponent->ServerSetReplicatedTargetData(
		GetCurrentAbilitySpecHandle(),
		GetCurrentActivationInfo().GetActivationPredictionKey(),
		DataHandle,
		FGameplayTag(),
		CurrentActorInfo->AbilitySystemComponent->ScopedPredictionKey);

	if (GetAvatarActorFromActorInfo()->HasAuthority())
	{
		ReleaseStoneAtLocation(CursorHit.ImpactPoint, DecodeChargePercentFromHitResult(CursorHit));
	}
}

void UDemoChargeRockAbility::OnReleaseTargetDataReady(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag)
{
	if (CurrentActorInfo && CurrentActorInfo->AbilitySystemComponent.IsValid())
	{
		CurrentActorInfo->AbilitySystemComponent->ConsumeClientReplicatedTargetData(
			GetCurrentAbilitySpecHandle(),
			GetCurrentActivationInfo().GetActivationPredictionKey());
	}

	const FHitResult* HitResult = DataHandle.Num() > 0 && DataHandle.Get(0) ? DataHandle.Get(0)->GetHitResult() : nullptr;
	FVector TargetLocation = GetMouseTargetLocation();
	if (HitResult)
	{
		TargetLocation = FVector(HitResult->ImpactPoint);
	}
	const float ReleaseChargePercent = HitResult ? DecodeChargePercentFromHitResult(*HitResult) : -1.f;
	ReleaseStoneAtLocation(TargetLocation, ReleaseChargePercent);
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

FVector UDemoChargeRockAbility::GetMouseTargetLocation() const
{
	const AActor* AvatarActor = GetAvatarActorFromActorInfo();
	FVector TargetLocation = AvatarActor ? AvatarActor->GetActorLocation() + AvatarActor->GetActorForwardVector() * ReleaseTraceDistance : FVector::ZeroVector;

	const APlayerController* PlayerController = CurrentActorInfo ? CurrentActorInfo->PlayerController.Get() : nullptr;
	if (!PlayerController)
	{
		return TargetLocation;
	}

	FHitResult CursorHit;
	if (PlayerController->GetHitResultUnderCursor(ECC_Target, false, CursorHit) && CursorHit.bBlockingHit)
	{
		TargetLocation = CursorHit.ImpactPoint;
		return TargetLocation;
	}

	if (PlayerController->GetHitResultUnderCursor(ECC_Visibility, false, CursorHit) && CursorHit.bBlockingHit)
	{
		TargetLocation = CursorHit.ImpactPoint;
		return TargetLocation;
	}

	FVector WorldLocation;
	FVector WorldDirection;
	if (PlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
	{
		const float PlaneZ = AvatarActor ? AvatarActor->GetActorLocation().Z : 0.f;
		if (!FMath::IsNearlyZero(WorldDirection.Z))
		{
			const float DistanceToPlane = (PlaneZ - WorldLocation.Z) / WorldDirection.Z;
			if (DistanceToPlane > 0.f)
			{
				TargetLocation = WorldLocation + WorldDirection * DistanceToPlane;
			}
		}
	}

	return TargetLocation;
}

void UDemoChargeRockAbility::UpdateAvatarFacing(const FVector& TargetLocation)
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		return;
	}

	if (AvatarActor->Implements<UCombatInterface>())
	{
		ICombatInterface::Execute_UpdateFacingTarget(AvatarActor, TargetLocation);
	}

	if (bRotateAvatarToMouseInCpp)
	{
		const FVector Direction = (TargetLocation - AvatarActor->GetActorLocation()).GetSafeNormal2D();
		if (!Direction.IsNearlyZero())
		{
			AvatarActor->SetActorRotation(Direction.Rotation());
		}
	}

	if (ADemoCharacter* DemoCharacter = Cast<ADemoCharacter>(AvatarActor))
	{
		if (DemoCharacter->HasAuthority())
		{
			DemoCharacter->Client_UpdateFacingTarget(TargetLocation);
		}
	}
}

void UDemoChargeRockAbility::DisableAvatarMovement()
{
	ACharacter* AvatarCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!AvatarCharacter)
	{
		return;
	}

	UCharacterMovementComponent* MovementComponent = AvatarCharacter->GetCharacterMovement();
	if (!MovementComponent || bMovementDisabledByChargeRock)
	{
		return;
	}

	PreviousMovementMode = MovementComponent->MovementMode;
	PreviousCustomMovementMode = MovementComponent->CustomMovementMode;
	MovementComponent->StopMovementImmediately();
	MovementComponent->DisableMovement();
	bMovementDisabledByChargeRock = true;
}

void UDemoChargeRockAbility::RestoreAvatarMovement()
{
	ACharacter* AvatarCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!AvatarCharacter)
	{
		return;
	}

	UCharacterMovementComponent* MovementComponent = AvatarCharacter->GetCharacterMovement();
	if (!MovementComponent || !bMovementDisabledByChargeRock)
	{
		return;
	}

	MovementComponent->SetMovementMode(PreviousMovementMode, PreviousCustomMovementMode);
	bMovementDisabledByChargeRock = false;
}

void UDemoChargeRockAbility::PlayCastingSound()
{
	if (!CastingSound || CastingAudioComponent)
	{
		return;
	}

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		return;
	}

	CastingAudioComponent = UGameplayStatics::SpawnSoundAttached(
		CastingSound,
		AvatarActor->GetRootComponent(),
		NAME_None,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		EAttachLocation::KeepRelativeOffset,
		true,
		CastingSoundVolume,
		CastingSoundPitch);
}

void UDemoChargeRockAbility::StopCastingSound()
{
	if (CastingAudioComponent)
	{
		CastingAudioComponent->Stop();
		CastingAudioComponent = nullptr;
	}
}

FVector UDemoChargeRockAbility::GetStoneSpawnLocation() const
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		return FVector::ZeroVector;
	}

	if (AvatarActor->Implements<UCombatInterface>())
	{
		const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(AvatarActor);
		return SocketLocation + AvatarActor->GetActorForwardVector() * StoneSpawnForwardOffset + FVector(0.f, 0.f, StoneSpawnVerticalOffset);
	}

	return AvatarActor->GetActorLocation() + FVector(0.f, 0.f, SpawnHeightAboveAvatar);
}

float UDemoChargeRockAbility::GetChargePercent() const
{
	if (!GetWorld() || MaxChargeTime <= 0.f)
	{
		return 1.f;
	}

	return FMath::Clamp((GetWorld()->GetTimeSeconds() - ChargeStartTime) / MaxChargeTime, 0.f, 1.f);
}

float UDemoChargeRockAbility::GetChargedDamage() const
{
	const float MinDamage = MinBaseDamage.GetValueAtLevel(GetAbilityLevel());
	const float MaxDamage = BaseDamage.GetValueAtLevel(GetAbilityLevel());
	return FMath::Lerp(MinDamage, MaxDamage, GetChargePercent());
}

void UDemoChargeRockAbility::ClearChargeTimer()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(ChargeTimerHandle);
	}
}
