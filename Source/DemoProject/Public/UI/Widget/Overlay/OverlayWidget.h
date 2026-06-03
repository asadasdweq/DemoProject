// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/DemoWidget.h"
#include "OverlayWidget.generated.h"

class USpellGlobe;
class UCanvasPanel;
class UGlobeProgressBar;
/**
 * 
 */
UCLASS()
class DEMOPROJECT_API UOverlayWidget : public UDemoWidget
{
	GENERATED_BODY()
public:
	
protected:
	virtual void WidgetControllerSet()override;
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCanvasPanel>CanvasPanel;
	//HealthManaGlobe
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UGlobeProgressBar>ProgressBar_Health;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UGlobeProgressBar>ProgressBar_Mana;
	//SpellGlobe
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USpellGlobe>SpellGlobe_LMB;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USpellGlobe>SpellGlobe_RMB;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USpellGlobe>SpellGlobe_1;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USpellGlobe>SpellGlobe_2;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USpellGlobe>SpellGlobe_3;
	
	
};
