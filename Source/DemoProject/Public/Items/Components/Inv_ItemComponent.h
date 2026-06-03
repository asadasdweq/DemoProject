// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include"Items/Manifest/Inv_ItemManifest.h"
#include "Inv_ItemComponent.generated.h"




UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent),Blueprintable)
class DEMOPROJECT_API UInv_ItemComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UInv_ItemComponent();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	void InitItemManifest(FInv_ItemManifest CopyOfManifest);
	FInv_ItemManifest GetItemManifest() const{return ItemManifest;};
	FInv_ItemManifest& GetItemManifestMutable(){return ItemManifest;}
	void SetStackCount(int32 StackCount);

	FString GetPickupMessage()const {return PickupMessage;}
	void PickedUp();
protected:
	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable,Category = "Inventory")
	void OnPickedUp();

private:
	UPROPERTY(Replicated,EditAnywhere,Category="Inventory")
	FInv_ItemManifest ItemManifest;
	
	UPROPERTY(EditAnywhere,Category="Inventory")
	FString PickupMessage;
};
