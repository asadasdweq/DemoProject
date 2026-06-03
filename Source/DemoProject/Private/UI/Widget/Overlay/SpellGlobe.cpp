// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/Overlay/SpellGlobe.h"


#include"AbilitySystem/Data/AbilityInfo.h"
#include "Components/Image.h"
#include "UI/WidgetController/OverlayWidgetController.h"

void USpellGlobe::WidgetControllerSet()
{
	
	if (UOverlayWidgetController*OverlayWidgetController=Cast<UOverlayWidgetController>(WidgetController))
	{
		OverlayWidgetController->AbilityInfoDelegate.AddDynamic(this,&USpellGlobe::ReceiveAbilityInfo);
	}
	
}



void USpellGlobe::SetInputTag(FGameplayTag Tag)
{
	InputTag = Tag;
}

void USpellGlobe::ReceiveAbilityInfo(const FDemoAbilityInfo&Info)
{
	if (Info.InputTag!=InputTag) return;
	CooldownTag=Info.CooldownTag;
	
	FSlateBrush BackGroundBrush;
	BackGroundBrush.SetResourceObject(Info.BackgroundMaterial);
	BackGroundBrush.DrawAs=ESlateBrushDrawType::Image;
	FSlateBrush Brush;
	Image_Background->SetBrush(BackGroundBrush);
	FSlateBrush IconBrush;
	IconBrush.SetResourceObject(Info.Icon);
	IconBrush.DrawAs=ESlateBrushDrawType::Image;
	Image_Icon->SetBrush(IconBrush);
	
	CooldownTagReceived();
}
