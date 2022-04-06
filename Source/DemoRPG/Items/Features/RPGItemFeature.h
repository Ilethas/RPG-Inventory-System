// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DemoRPG/RPGNetworkedObject.h"
#include "RPGItemFeature.generated.h"

class ARPGItemInstance;
UCLASS(Abstract, EditInlineNew, DefaultToInstanced)
class DEMORPG_API URPGItemFeature : public URPGNetworkedObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category = "Item Feature")
	bool HasBeenInitialized() const { return bInitialized; }

	UFUNCTION(BlueprintCallable, Category = "Item Feature")
	void Initialize();

	virtual void OnRemoved();

	virtual void OnItemInstanceConstructed(ARPGItemInstance* ItemInstance);
	
protected:
	UFUNCTION(BlueprintImplementableEvent, Meta = (DisplayName = "On Initialized"), Category = "Item Feature")
	void OnInitialized_BP();
	
	UFUNCTION(BlueprintImplementableEvent, Meta = (DisplayName = "On Removed"), Category = "Item Feature")
	void OnRemoved_BP();
	
	UFUNCTION(BlueprintImplementableEvent, Meta = (DisplayName = "On Item Instance Constructed"), Category = "Item Feature")
	void OnItemInstanceConstructed_BP(ARPGItemInstance* ItemInstance);
	
	virtual void OnInitialized();

private:
	bool bInitialized = false;
};
