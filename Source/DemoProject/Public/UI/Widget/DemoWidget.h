// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DemoWidget.generated.h"



/**
 * 
 */
UCLASS()
class DEMOPROJECT_API UDemoWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent)
	void SetWidgetController(UObject*InWidgetController);
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UObject>WidgetController;
	
	
	
	
protected:
	virtual void WidgetControllerSet();
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime)override;
	

	
};
