// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/DemoWidget.h"
#include "EnemyHealthBar.generated.h"

class UProgressBar;
class UOverlay;
class USizeBox;
/**
 * 
 */
UCLASS()
class DEMOPROJECT_API UEnemyHealthBar : public UDemoWidget
{
	GENERATED_BODY()
public:
	virtual void NativeOnInitialized() override;
	
	
	void SetHealthBarPercent(float Percent);

	
	void SetProgressBarVisible(bool bVisible){bProgressBarVisible=bVisible;};
	UFUNCTION()
	void OnHealthChanged(float NewValue);
	UFUNCTION()
	void OnMaxHealthChanged(float NewValue);
	
protected:
	UFUNCTION(BlueprintImplementableEvent)
	void GlobePercentSet(float Percent);

	FTimerHandle VisibleTimerHandle;
	void HiddenHealthBar();
	
	bool bProgressBarVisible=false;
	virtual void WidgetControllerSet() override;
	float CurrentHealth=0.f;
	float MaxHealth=1.f;
private:
	
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox>SizeBox_Root;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay>Overlay_Root;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UProgressBar>HealthBar;
	
};
