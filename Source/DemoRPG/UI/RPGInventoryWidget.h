// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RPGFocusWidget.h"
#include "RPGInventoryWidget.generated.h"

class URPGInventoryItemWidget;
class URPGInventoryMapping;
struct FRPGUISlot;
class URPGInventoryRowWidget;
class UVerticalBox;
class URPGInventoryMappingContainer;
class URPGInventory;
class URPGUIMapping;
UCLASS()
class DEMORPG_API URPGInventoryWidget : public URPGFocusWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "Inventory Widget")
	void SetInventory(URPGInventory* NewInventory);

	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Category = "Inventory Widget")
	URPGInventory* GetInventory() const { return Inventory; }
	
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "Inventory Widget")
	void SetInventoryMappingContainer(URPGInventoryMappingContainer* NewInventoryMappingContainer);

	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Category = "Inventory Widget")
	URPGInventoryMappingContainer* GetInventoryMappingContainer() const { return InventoryMappingContainer; }

protected:
	UPROPERTY(BlueprintSetter = SetInventory, BlueprintGetter = GetInventory, Category = "Inventory Widget")
	TObjectPtr<URPGInventory> Inventory;
	
	UPROPERTY(BlueprintSetter = SetInventoryMappingContainer, BlueprintGetter = GetInventoryMappingContainer, Category = "Inventory Widget")
	TObjectPtr<URPGInventoryMappingContainer> InventoryMappingContainer;
	
	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget), Category = "Inventory Widget")
	TObjectPtr<UVerticalBox> Container;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory Widget")
	TSubclassOf<URPGInventoryRowWidget> RowWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory Widget")
	TSubclassOf<URPGInventoryItemWidget> ItemSlotWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory Widget", Meta = (ClampMin = "0.0", UIMin = "0.0"))
	float ItemSlotPadding = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory Widget", Meta = (ClampMin = "0", UIMin = "0"))
	int RowsAmountOverride = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory Widget")
	int DefaultInventorySlotsPerRow = 7;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory Widget")
	float DeterminedItemSlotSize = 0.0f;
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void OnInventoryMappingRemoved(URPGUIMapping* RemovedUIMapping);
	void OnInventoryMappingAdded(URPGUIMapping* AddedUIMapping);
	void OnInventoryUISlotChanged(const FRPGUISlot OldUISlot, URPGInventoryMapping* InventoryMapping);
	
	UFUNCTION(BlueprintCallable, Category = "Inventory Widget")
	void CreateInventoryRows(const int RowsAmount, const int SlotsPerRow, const float ItemPadding);

	UFUNCTION(BlueprintCallable, Category = "Inventory Widget")
	void RebuildInventory(const int RowsAmount, const int SlotsPerRow, const float ItemPadding);
	
	void UpdateItemSlot(const FRPGUISlot& InItemSlot, URPGInventoryMapping* InventoryMapping) const;
	URPGInventoryItemWidget* GetItemWidgetForSlot(const FRPGUISlot& InItemSlot) const;
	URPGInventoryItemWidget* CreateWidgetForRow(const URPGInventoryRowWidget* InventoryRow, const FRPGUISlot& InItemSlot, const FMargin& PaddingStruct);
	void RebuildInventoryRow(const URPGInventoryRowWidget* InventoryRow, const int RowIndex, const int AmountOfItemSlots, const float ItemPadding);
	void RefreshInventory();
	void ClearItemWidgetsMappings() const;
};
