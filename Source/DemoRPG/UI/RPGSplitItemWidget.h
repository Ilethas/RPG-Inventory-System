// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RPGFocusWidget.h"
#include "DemoRPG/RPGTypes.h"
#include "RPGSplitItemWidget.generated.h"

/**
 * 
 */
UCLASS()
class DEMORPG_API URPGSplitItemWidget : public URPGFocusWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Category = "Split Item Widget")
	class URPGInventory* TargetInventory;
	
	UPROPERTY(BlueprintReadOnly, Category = "Split Item Widget")
	class URPGInventoryMappingContainer* TargetContainer;

	UPROPERTY(BlueprintReadOnly, Category = "Split Item Widget")
	FRPGUISlot TargetSlot = FRPGUISlot(-1, -1);
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Split Item Widget")
	void RefreshWidget();
	
	UFUNCTION(BlueprintCallable, Category = "Split Item Widget")
	void SetItemInstance(class ARPGItemInstance* NewItemInstance);

	UFUNCTION(BlueprintPure, Category = "Split Item Widget")
	class ARPGItemInstance* GetItemInstance() const { return ItemInstance; }

	UFUNCTION(BlueprintCallable, Category = "Split Item Widget")
	void PerformSplit(const int AmountToSplit) const;

protected:
	UPROPERTY(BlueprintSetter = SetItemInstance, BlueprintGetter = GetItemInstance, Category = "Split Item Widget")
	class ARPGItemInstance* ItemInstance;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
};
