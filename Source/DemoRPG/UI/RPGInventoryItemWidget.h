// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DemoRPG/RPGTypes.h"
#include "WidgetComponents/RPGWidgetWithComponents.h"
#include "RPGInventoryItemWidget.generated.h"

class URPGEquipmentMapping;
UCLASS()
class DEMORPG_API URPGInventoryItemWidget : public URPGWidgetWithComponents
{
	GENERATED_BODY()
public:
	URPGInventoryItemWidget(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(BlueprintReadOnly, Category = "Inventory Item Widget")
	class URPGInventoryWidget* OwnerInventoryWidget;
	
	UPROPERTY(BlueprintReadOnly, Category = "Inventory Item Widget")
	FRPGUISlot UISlot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory Item Widget")
	TSubclassOf<class URPGItemDragWidget> ItemDragWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory Item Widget")
	TSubclassOf<class URPGContextMenuWidget> ContextMenuWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory Item Widget")
	TSubclassOf<class URPGContextMenuEntryWidget> ContextMenuEntryWidgetClass;
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Inventory Item Widget")
	void RefreshWidget();

	UFUNCTION(BlueprintCallable, Category = "Inventory Item Widget")
	void SetInventoryMapping(class URPGInventoryMapping* NewInventoryMapping);

	UFUNCTION(BlueprintPure, Category = "Inventory Item Widget")
	class URPGInventoryMapping* GetInventoryMapping() const { return InventoryMapping; }

	UFUNCTION(BlueprintCallable, Category = "Inventory Item Widget")
	void SetItemInstance(class ARPGItemInstance* NewItemInstance);

	UFUNCTION(BlueprintPure, Category = "Inventory Item Widget")
	class ARPGItemInstance* GetItemInstance() const { return ItemInstance; }

	virtual void PostInitProperties() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory Item Widget")
	class URPGTooltipWidgetComponent* TooltipComponent;
	
	UPROPERTY(BlueprintSetter = SetInventoryMapping, BlueprintGetter = GetInventoryMapping, Category = "Inventory Item Widget")
	class URPGInventoryMapping* InventoryMapping;
	
	UPROPERTY(BlueprintSetter = SetItemInstance, BlueprintGetter = GetItemInstance, Category = "Inventory Item Widget")
	class ARPGItemInstance* ItemInstance;

	void OnInventoryMappingChanged();
	void HandleOnDrop(URPGInventoryMapping* Payload);
	void HandleOnDrop(URPGEquipmentMapping* Payload);

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
};
