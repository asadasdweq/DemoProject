// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/DemoEnemy.h"
#include "AbilitySystemComponent.h"
#include "DemoGameplayTags.h"
#include "AbilitySystem/DemoAbilitySystemComponent.h"
#include "AbilitySystem/DemoAbilitySystemLibrary.h"
#include "AbilitySystem/DemoAttributeSet.h"
#include "AI/DemoAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/Widget/DemoWidget.h"
#include "UI/Widget/Enemy/EnemyHealthBar.h"


ADemoEnemy::ADemoEnemy()
{
	PrimaryActorTick.bCanEverTick = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	AbilitySystemComponent =CreateDefaultSubobject<UDemoAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	
	AttributeSet = CreateDefaultSubobject<UDemoAttributeSet>("AttributeSet");
	
	HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	HealthBar->SetupAttachment(GetRootComponent());
	BaseWalkSpeed = 250.f;
	bHasWeapon=true;
}

void ADemoEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (!HasAuthority())return;
	DemoAIController=Cast<ADemoAIController>(NewController);
	DemoAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	DemoAIController->RunBehaviorTree(BehaviorTree);
	DemoAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"),false);
	DemoAIController->GetBlackboardComponent()->SetValueAsBool(FName("RangedAttacker"),CharacterClass!=ECharacterClass::Warrior);
}

void ADemoEnemy::InitActorAbilityInfo()
{
	Super::InitActorAbilityInfo();
	GetAbilitySystemComponent()->InitAbilityActorInfo(this,this);
	Cast<UDemoAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
	if (HasAuthority())
	{
		InitializeDefaultAttributes();
	}
}

void ADemoEnemy::InitializeDefaultAttributes() const
{
	UDemoAbilitySystemLibrary::InitializeDefaultAttributes(this,CharacterClass,AbilitySystemComponent);
	
}

void ADemoEnemy::HightlightActor_Implementation()
{
	GetMesh()->SetRenderCustomDepth(true);
	Weapon->SetRenderCustomDepth(true);
}

void ADemoEnemy::UnHightlightActor_Implementation()
{
	GetMesh()->SetRenderCustomDepth(false);
	Weapon->SetRenderCustomDepth(false);
}

void ADemoEnemy::SetCombatTarget_Implementation(AActor* InCombatTarget)
{
	CombatTarget=InCombatTarget;
}

AActor* ADemoEnemy::GetCombatTarget_Implementation()
{
	return CombatTarget;
}

void ADemoEnemy::Die_Implementation()
{
	SetLifeSpan(LifeSpan);
	if (DemoAIController) DemoAIController->GetBlackboardComponent()->SetValueAsBool(FName("Dead"),true);
	Super::Die_Implementation();
}

void ADemoEnemy::HitReactTagChanged(const FGameplayTag CallBackTag, int32 NewCount)
{
	bHitReact=NewCount>0;
	GetCharacterMovement()->MaxWalkSpeed = bHitReact ? 0.f : BaseWalkSpeed;
	if (DemoAIController&&DemoAIController->GetBlackboardComponent())
	{
		DemoAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"),bHitReact);
	}
	
}


void ADemoEnemy::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	InitActorAbilityInfo();
	if (HasAuthority())
	{
		UDemoAbilitySystemLibrary::GiveStartupAbility(this,AbilitySystemComponent,CharacterClass);
	}
	if (UDemoWidget*DemoWidget=Cast<UDemoWidget>(HealthBar->GetUserWidgetObject()))
	{
		if (UEnemyHealthBar *EnemyHealthBar=Cast<UEnemyHealthBar>(DemoWidget))
		{
			EnemyHealthBar->SetWidgetController(this);
		}
	}
	const UDemoAttributeSet*DemoAS=Cast<UDemoAttributeSet>(AttributeSet);
	if (DemoAS)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(DemoAS->GetHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnHealthChanged.Broadcast(Data.NewValue);
		});
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(DemoAS->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
				{
					OnMaxHealthChanged.Broadcast(Data.NewValue);
				});
		AbilitySystemComponent->RegisterGameplayTagEvent(FDemoGameplayTags::Get().Effect_HitReact,EGameplayTagEventType::NewOrRemoved)
		.AddUObject(this,&ThisClass::HitReactTagChanged);
		
		
		OnHealthChanged.Broadcast(DemoAS->GetHealth());
		OnMaxHealthChanged.Broadcast(DemoAS->GetMaxHealth());
	}
	
	
}


