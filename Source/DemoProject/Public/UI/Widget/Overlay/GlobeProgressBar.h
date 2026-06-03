// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/DemoWidget.h"
#include "GameplayTagContainer.h"
#include "GlobeProgressBar.generated.h"

class UProgressBar;
class UImage;
class UOverlay;
class USizeBox;
/**
 * 
 */
UCLASS()
class DEMOPROJECT_API UGlobeProgressBar : public UDemoWidget
{
	GENERATED_BODY()
public:
	void SetProgressBarPercent(float Percent);
	void SetGlobalTag(FGameplayTag Tag);
	FGameplayTag GetGlobalTag()const{return GlobalTag;};
protected:
	UFUNCTION(BlueprintImplementableEvent)
	void GlobePercentSet(float Percent);
	virtual void WidgetControllerSet() override;
	UFUNCTION()
	void OnHealthChanged(const float NewValue);
	UFUNCTION()
	void OnManaChanged(const float NewValue);
	UFUNCTION()
	void OnMaxHealthChanged(const float NewValue);
	UFUNCTION()
	void OnMaxManaChanged(const float NewValue);
	float CurrentHealth=0.f;
	float MaxHealth=1.f;
	float CurrentMana=0.f;
	float MaxMana=1.f;
	
private:
	
	FGameplayTag GlobalTag;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox>SizeBox_Root;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay>Overlay_Root;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage>Image_Background;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage>Image_Glass;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UProgressBar>ProgressBar;
	
};
