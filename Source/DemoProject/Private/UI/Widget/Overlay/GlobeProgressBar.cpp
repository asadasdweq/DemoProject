// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/Overlay/GlobeProgressBar.h"
#include"Components/ProgressBar.h"
#include "DemoGameplayTags.h"
#include "UI/WidgetController/OverlayWidgetController.h"


void UGlobeProgressBar::SetProgressBarPercent(float Percent)
{
	if (ProgressBar)
	{
		//ProgressBar->SetPercent(Percent);
		//GEngine->AddOnScreenDebugMessage(1,5,FColor::Black,FString::SanitizeFloat(Percent));

		const float SafePercent = FMath::Clamp(Percent, 0.0f, 1.0f);

		ProgressBar->SetPercent(SafePercent);
		
	

		ProgressBar->InvalidateLayoutAndVolatility();
		GlobePercentSet(SafePercent);
		
		//GEngine->AddOnScreenDebugMessage(1, 5, FColor::Green, FString::SanitizeFloat(SafePercent));
	}
}

void UGlobeProgressBar::SetGlobalTag(FGameplayTag Tag)
{
	GlobalTag=Tag;
}



void UGlobeProgressBar::WidgetControllerSet()
{
	
	UOverlayWidgetController*OverlayWidgetController=Cast<UOverlayWidgetController>(WidgetController);
	const FDemoGameplayTags GameplayTags=FDemoGameplayTags::Get();
	if (!OverlayWidgetController)return;
	if (GlobalTag==GameplayTags.Globel_Health)
	{
		OverlayWidgetController->OnHealthChanged.AddDynamic(this,&UGlobeProgressBar::OnHealthChanged);
		OverlayWidgetController->OnMaxHealthChanged.AddDynamic(this,&UGlobeProgressBar::OnMaxHealthChanged);
		
	}
	if (GlobalTag==GameplayTags.Globel_Mana)
	{
		OverlayWidgetController->OnManaChanged.AddDynamic(this,&UGlobeProgressBar::OnManaChanged);
		OverlayWidgetController->OnMaxManaChanged.AddDynamic(this,&UGlobeProgressBar::OnMaxManaChanged);
		
	}
	
}

void UGlobeProgressBar::OnHealthChanged(const float NewValue)
{
	CurrentHealth=NewValue;
	const float Percentage=CurrentHealth/MaxHealth;
	SetProgressBarPercent(Percentage);
}

void UGlobeProgressBar::OnManaChanged(const float NewValue)
{
	CurrentMana=NewValue;
	const float Percentage=CurrentMana/MaxMana;
	SetProgressBarPercent(Percentage);
}

void UGlobeProgressBar::OnMaxHealthChanged(const float NewValue)
{
	MaxHealth=NewValue;
	const float Percentage=CurrentHealth/MaxHealth;
	SetProgressBarPercent(Percentage);
}

void UGlobeProgressBar::OnMaxManaChanged(const float NewValue)
{
	MaxMana=NewValue;
	const float Percentage=CurrentMana/MaxMana;
	SetProgressBarPercent(Percentage);
}
