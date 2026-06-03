// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "DemoHUD.generated.h"
class UAbilitySystemComponent;
class UAttributeSet;
class UDemoWidget;
class UOverlayWidgetController;
struct FWidgetControllerParams;
/**
 * 
 */
UCLASS()
class DEMOPROJECT_API ADemoHUD : public AHUD
{
	GENERATED_BODY()
public:
	UOverlayWidgetController*GetOverlayWidgetController(const FWidgetControllerParams& Params) ;
	void InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS);
private:
	UPROPERTY()
	TObjectPtr<UDemoWidget>OverlayWidget;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UDemoWidget>OverlayWidgetClass;
	UPROPERTY()
	TObjectPtr<UOverlayWidgetController>OverlayWidgetController;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;
};
