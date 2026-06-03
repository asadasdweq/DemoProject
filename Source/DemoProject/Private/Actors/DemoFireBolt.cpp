// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/DemoFireBolt.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/DemoAbilitySystemLibrary.h"
#include "DemoProject/DemoProject.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"


ADemoFireBolt::ADemoFireBolt()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates=true;
	
	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(Sphere);
	Sphere->SetCollisionObjectType(ECC_Projectile);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->InitialSpeed = 550.f;
	ProjectileMovement->MaxSpeed = 550.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;
}
void ADemoFireBolt::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(LifeSpan);
	SetReplicateMovement(true);
	Sphere->OnComponentBeginOverlap.AddDynamic(this,&ThisClass::OnSphereOverlap);
}

void ADemoFireBolt::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValidOverlap(OtherActor))return;
	if (bHit) return;
	bHit = true;
	if (HasAuthority())
	{
		if (UAbilitySystemComponent*TargetASC=UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			AActor *SourceActor=GetOwner();
			if (!SourceActor) return;
			UDemoAbilitySystemLibrary::CauseDamage(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor)
				,TargetASC
				,DamageGameplayEffectClass
				,DamageType
				,BaseDamage);
		}
		Multicast_OnHit();
		SetLifeSpan(0.1f);
		
	}
	
	bHit = true;
}

void ADemoFireBolt::Multicast_OnHit_Implementation()
{
	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
}



void ADemoFireBolt::Destroyed()
{
	Super::Destroyed();
}

bool ADemoFireBolt::IsValidOverlap(AActor* OtherActor)
{
	AActor*SourceActor=GetOwner();
	if (!SourceActor) return false;
	if (OtherActor==SourceActor)return false;
	if (!UDemoAbilitySystemLibrary::IsNotFriend(OtherActor,SourceActor)) return false;
    if (UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor)==nullptr)return false;
	return true;
}





