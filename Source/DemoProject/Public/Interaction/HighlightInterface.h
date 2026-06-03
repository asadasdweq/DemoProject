// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HighlightInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UHighlightInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DEMOPROJECT_API IHighlightInterface
{
	GENERATED_BODY()

	
public:
	UFUNCTION(BlueprintNativeEvent)
	void HightlightActor();
	UFUNCTION(BlueprintNativeEvent)
	void UnHightlightActor();
	
};
