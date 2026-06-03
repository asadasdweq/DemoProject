// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/Enemy/EnemyHealthBar.h"
#include "Characters/DemoEnemy.h"
#include "Components/ProgressBar.h"
#include "Math/UnitConversion.h"

void UEnemyHealthBar::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	bProgressBarVisible=true;
	if (!bProgressBarVisible)
	{
		SetVisibility(ESlateVisibility::Hidden);
	}
}

void UEnemyHealthBar::SetHealthBarPercent(float Percent)
{
	if (HealthBar)
	{

		const float SafePercent = FMath::Clamp(Percent, 0.0f, 1.0f);

		HealthBar->SetPercent(SafePercent);

		HealthBar->InvalidateLayoutAndVolatility();
		GlobePercentSet(SafePercent);
		
		
		SetProgressBarVisible(true);
		SetVisibility(ESlateVisibility::Visible);
		if (bProgressBarVisible)
		{
			GetWorld()->GetTimerManager().ClearTimer(VisibleTimerHandle);

			GetWorld()->GetTimerManager().SetTimer(
				VisibleTimerHandle,
				this,
				&UEnemyHealthBar::HiddenHealthBar,
				2.0f,
				false);
		}
	}
	
}



void UEnemyHealthBar::OnHealthChanged(float NewValue)
{
	CurrentHealth=NewValue;
	const float Percentage = CurrentHealth/MaxHealth;
	SetHealthBarPercent(Percentage);
	
}

void UEnemyHealthBar::OnMaxHealthChanged(float NewValue)
{
	MaxHealth=NewValue;
	const float Percentage= CurrentHealth/MaxHealth;
	SetHealthBarPercent(Percentage);
}


void UEnemyHealthBar::HiddenHealthBar()
{
	SetVisibility(ESlateVisibility::Hidden);
	SetProgressBarVisible(false);
}

void UEnemyHealthBar::WidgetControllerSet()
{
	if (ADemoEnemy*EnemyWidgetController=Cast<ADemoEnemy>(WidgetController))
	{
		EnemyWidgetController->OnHealthChanged.AddDynamic(this,&ThisClass::OnHealthChanged);
		EnemyWidgetController->OnMaxHealthChanged.AddDynamic(this,&ThisClass::OnMaxHealthChanged);
	}
}
