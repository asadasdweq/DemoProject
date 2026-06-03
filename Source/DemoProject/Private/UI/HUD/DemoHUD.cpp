// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/DemoHUD.h"
#include "UI/Widget/DemoWidget.h"
#include "Blueprint/UserWidget.h"
#include "UI/Widget/Overlay/OverlayWidget.h"
#include "UI/WidgetController/OverlayWidgetController.h"

UOverlayWidgetController* ADemoHUD::GetOverlayWidgetController(const FWidgetControllerParams& Params)
{
	if (OverlayWidgetController == nullptr)
	{
		OverlayWidgetController=NewObject<UOverlayWidgetController>(this,OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(Params);
		OverlayWidgetController->BindCallbacksDependencies();
	}
	return OverlayWidgetController;
}

void ADemoHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	check(OverlayWidgetControllerClass);
	check(OverlayWidgetClass);
	UUserWidget*Widget=CreateWidget<UUserWidget>(GetWorld(),OverlayWidgetClass);
	OverlayWidget=Cast<UOverlayWidget>(Widget);
	const FWidgetControllerParams Params(PC,PS,ASC,AS);
	UOverlayWidgetController*WidgetController=GetOverlayWidgetController(Params);
	OverlayWidget->SetWidgetController(WidgetController);
	WidgetController->BroadcastInitialValue();
	Widget->AddToViewport();
}
