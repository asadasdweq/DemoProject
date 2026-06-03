// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "DemoAIController.generated.h"
class UBlackboardComponent;
class UBehaviorTreeComponent;
/**
 * 
 */
UCLASS()
class DEMOPROJECT_API ADemoAIController : public AAIController
{
	GENERATED_BODY()
public:
	ADemoAIController();
protected:
	
	
	
	
	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;
};
