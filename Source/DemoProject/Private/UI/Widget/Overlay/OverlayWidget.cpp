// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/Overlay/OverlayWidget.h"
#include "DemoGameplayTags.h"
#include "UI/Widget/Overlay/GlobeProgressBar.h"
#include "UI/Widget/Overlay/SpellGlobe.h"
#include"AbilitySystem/DemoAbilitySystemComponent.h"
#include "UI/WidgetController/OverlayWidgetController.h"


void UOverlayWidget::WidgetControllerSet()
{
	
	const FDemoGameplayTags DemoGameplayTags=FDemoGameplayTags::Get();
	
	//HealthManaGlobe
	ProgressBar_Health->SetGlobalTag(DemoGameplayTags.Globel_Health);
	ProgressBar_Mana->SetGlobalTag(DemoGameplayTags.Globel_Mana);
	ProgressBar_Health->SetWidgetController(WidgetController);
	ProgressBar_Mana->SetWidgetController(WidgetController);
	//SpellGlobe
	SpellGlobe_LMB->SetInputTag(DemoGameplayTags.InputTag_LMB);
	SpellGlobe_RMB->SetInputTag(DemoGameplayTags.InputTag_RMB);
	SpellGlobe_1->SetInputTag(DemoGameplayTags.InputTag_1);
	SpellGlobe_2->SetInputTag(DemoGameplayTags.InputTag_2);
	SpellGlobe_3->SetInputTag(DemoGameplayTags.InputTag_3);
	
	SpellGlobe_LMB->SetWidgetController(WidgetController);
	SpellGlobe_RMB->SetWidgetController(WidgetController);
	SpellGlobe_1->SetWidgetController(WidgetController);
	SpellGlobe_2->SetWidgetController(WidgetController);
	SpellGlobe_3->SetWidgetController(WidgetController);

	UOverlayWidgetController*OverlayWidgetController=Cast<UOverlayWidgetController>(WidgetController);

	OverlayWidgetController->BroadAbilityInfo();
	
	
}
