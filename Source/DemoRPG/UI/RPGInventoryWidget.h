// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RPGFocusWidget.h"
#include "RPGInventoryWidget.generated.h"

UCLASS()
class DEMORPG_API URPGInventoryWidget : public URPGFocusWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "Inventory Widget")
	void SetInventory(class URPGInventory* NewInventory);

	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Category = "Inventory Widget")
	class URPGInventory* GetInventory() const { return Inventory; }
	
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "Inventory Widget")
	void SetInventoryMappingContainer(class URPGInventoryMappingContainer* NewInventoryMappingContainer);

	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Category = "Inventory Widget")
	class URPGInventoryMappingContainer* GetInventoryMappingContainer() const { return InventoryMappingContainer; }

protected:
	UPROPERTY(BlueprintSetter = SetInventory, BlueprintGetter = GetInventory, Category = "Inventory Widget")
	class URPGInventory* Inventory;
	
	UPROPERTY(BlueprintSetter = SetInventoryMappingContainer, BlueprintGetter = GetInventoryMappingContainer, Category = "Inventory Widget")
	class URPGInventoryMappingContainer* InventoryMappingContainer;
	
	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget), Category = "Inventory Widget")
	class UVerticalBox* Container;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory Widget")
	TSubclassOf<class URPGInventoryRowWidget> RowWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory Widget")
	TSubclassOf<class URPGInventoryItemWidget> ItemSlotWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory Widget", Meta = (ClampMin = "0.0", UIMin = "0.0"))
	float ItemSlotPadding = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory Widget", Meta = (ClampMin = "0", UIMin = "0"))
	int RowsAmountOverride = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory Widget")
	int DefaultInventorySlotsPerRow = 7;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory Widget")
	float DeterminedItemSlotSize = 0.0f;
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void OnInventoryMappingRemoved(class URPGUIMapping* RemovedUIMapping);
	void OnInventoryMappingAdded(class URPGUIMapping* AddedUIMapping);
	void OnInventoryUISlotChanged(const struct FRPGUISlot OldUISlot, class URPGInventoryMapping* InventoryMapping);
	
	UFUNCTION(BlueprintCallable, Category = "Inventory Widget")
	void CreateInventoryRows(const int RowsAmount, const int SlotsPerRow, const float ItemPadding);

	UFUNCTION(BlueprintCallable, Category = "Inventory Widget")
	void RebuildInventory(const int RowsAmount, const int SlotsPerRow, const float ItemPadding);
	
	void UpdateItemSlot(const FRPGUISlot& InItemSlot, URPGInventoryMapping* InventoryMapping) const;
	class URPGInventoryItemWidget* GetItemWidgetForSlot(const FRPGUISlot& InItemSlot) const;
	class URPGInventoryItemWidget* CreateWidgetForRow(const URPGInventoryRowWidget* InventoryRow, const FRPGUISlot& InItemSlot, const FMargin& PaddingStruct);
	void RebuildInventoryRow(const URPGInventoryRowWidget* InventoryRow, const int RowIndex, const int AmountOfItemSlots, const float ItemPadding);
	void RefreshInventory();
	void ClearItemWidgetsMappings() const;
};
