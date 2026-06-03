// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "DemoAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class DEMOPROJECT_API UDemoAssetManager : public UAssetManager
{
	GENERATED_BODY()
public:
	static UDemoAssetManager&Get();
protected:
	virtual void StartInitialLoading() override;
};
