// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RPGTooltip.h"
#include "RPGItemTooltip.generated.h"

class ARPGItemInstance;
UCLASS()
class DEMORPG_API URPGItemTooltip : public URPGTooltip
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Item Tooltip")
	void RefreshWidget();
	
	UFUNCTION(BlueprintCallable, Category = "Item Tooltip")
	void SetItemInstance(ARPGItemInstance* NewItemInstance);

	UFUNCTION(BlueprintPure, Category = "Item Tooltip")
	ARPGItemInstance* GetItemInstance() const { return ItemInstance; }
	
protected:
	UPROPERTY(BlueprintReadOnly, Category = "Item Tooltip")
	TObjectPtr<ARPGItemInstance> ItemInstance;
};
