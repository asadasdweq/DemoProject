// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/DemoWidget.h"

void UDemoWidget::SetWidgetController_Implementation(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	
	WidgetControllerSet();
}



void UDemoWidget::WidgetControllerSet()
{
}

void UDemoWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}
