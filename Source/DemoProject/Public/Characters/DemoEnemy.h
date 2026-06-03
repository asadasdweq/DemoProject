// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DemoCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "Interaction/HighlightInterface.h"
#include "DemoEnemy.generated.h"

class ADemoAIController;
class UBehaviorTree;
class UWidgetComponent;

UCLASS()
class DEMOPROJECT_API ADemoEnemy : public ADemoCharacterBase,public IHighlightInterface,public IEnemyInterface
{
	GENERATED_BODY()

public:
	
	ADemoEnemy();
	
	virtual void PossessedBy(AController* NewController) override;
	virtual void InitActorAbilityInfo() override;
	virtual void InitializeDefaultAttributes()const override;
	//Highlight Interface
	virtual void HightlightActor_Implementation() override;
	virtual void UnHightlightActor_Implementation() override;
	//End Highlight Interface

	//EnemyInterface
	virtual void SetCombatTarget_Implementation(AActor* InCombatTarget) override;
	virtual AActor* GetCombatTarget_Implementation() override;
	//EndEnemyInterface
	//CombatInterface
	virtual void Die_Implementation() override;
	//End CombatInterface
	
	
	void HitReactTagChanged(const FGameplayTag CallBackTag,int32 NewCount);
	UPROPERTY(BlueprintReadOnly,Category="Combat")
	bool bHitReact=false;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Combat")
	float LifeSpan=15.f;
	FOnAttributeChanged OnHealthChanged;
	FOnAttributeChanged OnMaxHealthChanged;

	
	
	UPROPERTY(BlueprintReadOnly,Category="Combat")
	TObjectPtr<AActor>CombatTarget;
protected:
	
	virtual void BeginPlay() override;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TObjectPtr<UWidgetComponent>HealthBar;
	
	UPROPERTY(EditAnywhere,Category="AI")
	TObjectPtr<UBehaviorTree>BehaviorTree;
	UPROPERTY()
	TObjectPtr<ADemoAIController>DemoAIController;
	


};
