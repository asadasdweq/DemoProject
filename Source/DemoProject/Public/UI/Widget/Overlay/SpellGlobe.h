// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/DemoWidget.h"
#include "GameplayTagContainer.h"
#include "SpellGlobe.generated.h"

class UImage;
struct FGameplayTag;
struct FDemoAbilityInfo;
/**
 * 
 */
UCLASS()
class DEMOPROJECT_API USpellGlobe : public UDemoWidget
{
	GENERATED_BODY()
public:
	void SetInputTag(FGameplayTag Tag);
	
	UFUNCTION()
	void ReceiveAbilityInfo(const FDemoAbilityInfo&Info);
	
	
protected:
	virtual void WidgetControllerSet() override;
	UFUNCTION(BlueprintImplementableEvent)
	void CooldownTagReceived();
	UPROPERTY()
	FGameplayTag InputTag;
	UPROPERTY(BlueprintReadOnly,Category="Cooldown")
	FGameplayTag CooldownTag;
	
	UPROPERTY(BlueprintReadOnly,meta=(BindWidget))
	TObjectPtr<UImage>Image_Background;
	UPROPERTY(BlueprintReadOnly,meta=(BindWidget))
	TObjectPtr<UImage>Image_Icon;
private:
	
	
};
