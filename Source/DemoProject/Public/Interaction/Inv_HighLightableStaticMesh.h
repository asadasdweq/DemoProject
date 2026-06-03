// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inv_Highlightable.h"
#include "Components/StaticMeshComponent.h"
#include "Inv_HighLightableStaticMesh.generated.h"

/**
 * 
 */
UCLASS()
class DEMOPROJECT_API UInv_HighLightableStaticMesh : public UStaticMeshComponent,public IInv_Highlightable
{
	GENERATED_BODY()
public:
	virtual void HighLight_Implementation() override;
	virtual void UnHighLight_Implementation() override;
private:
	UPROPERTY(EditAnywhere,Category="Inventory")
	TObjectPtr<UMaterialInterface> HighLightMaterial;
};
