// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RPGUIMappingContainer.h"
#include "RPGInventoryMappingContainer.generated.h"

UCLASS(DefaultToInstanced)
class DEMORPG_API URPGInventoryMappingContainer : public URPGUIMappingContainer
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "Inventory Mapping Container")
	void SetInventorySlotsPerRow(const int NewInventorySlotsPerRow);
	
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Category = "Inventory Mapping Container")
	int GetInventorySlotsPerRow() const { return InventorySlotsPerRow; }

	void OnItemAdded(class ARPGItemInstance* AddedItem);
	void OnItemRemoved(class ARPGItemInstance* RemovedItem);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:
	UPROPERTY(BlueprintGetter = GetInventorySlotsPerRow, BlueprintSetter = SetInventorySlotsPerRow, Category = "Inventory Mapping Container", Replicated)
	int InventorySlotsPerRow = 7;
};
