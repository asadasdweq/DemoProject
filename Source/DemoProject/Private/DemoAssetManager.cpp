// Fill out your copyright notice in the Description page of Project Settings.


#include "DemoAssetManager.h"
#include "DemoGameplayTags.h"

UDemoAssetManager& UDemoAssetManager::Get()
{
	check(GEngine);
	return *Cast<UDemoAssetManager>(GEngine->AssetManager);
}

void UDemoAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	FDemoGameplayTags::InitializeNativeGameplayTags();
}
