// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DemoRPG/RPGTypes.h"
#include "WidgetComponents/RPGWidgetWithComponents.h"
#include "RPGEquippedItemWidget.generated.h"

class URPGContextMenuEntryWidget;
class URPGContextMenuWidget;
class URPGEquipmentMapping;
class URPGItemDragWidget;
class URPGTooltipWidgetComponent;
UCLASS()
class DEMORPG_API URPGEquippedItemWidget : public URPGWidgetWithComponents
{
	GENERATED_BODY()
public:
	URPGEquippedItemWidget(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipped Item Widget")
	TSubclassOf<URPGItemDragWidget> ItemDragWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipped Item Widget")
	TSubclassOf<URPGContextMenuWidget> ContextMenuWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipped Item Widget")
	TSubclassOf<URPGContextMenuEntryWidget> ContextMenuEntryWidgetClass;
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Equipped Item Widget")
	void RefreshWidget();

	UFUNCTION(BlueprintCallable, Category = "Equipped Item Widget")
	void SetEquipmentMapping(URPGEquipmentMapping* NewEquipmentMapping);

	UFUNCTION(BlueprintPure, Category = "Equipped Item Widget")
	URPGEquipmentMapping* GetEquipmentMapping() const { return EquipmentMapping; }

	UFUNCTION(BlueprintCallable, Category = "Equipped Item Widget")
	void SetItemInstance(ARPGItemInstance* NewItemInstance);

	UFUNCTION(BlueprintPure, Category = "Equipped Item Widget")
	ARPGItemInstance* GetItemInstance() const { return ItemInstance; }

	virtual void PostInitProperties() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipped Item Widget")
	TObjectPtr<URPGTooltipWidgetComponent> TooltipComponent;
	
	UPROPERTY(BlueprintSetter = SetEquipmentMapping, BlueprintGetter = GetEquipmentMapping, Category = "Equipped Item Widget")
	TObjectPtr<URPGEquipmentMapping> EquipmentMapping;
	
	UPROPERTY(BlueprintSetter = SetItemInstance, BlueprintGetter = GetItemInstance, Category = "Equipped Item Widget")
	TObjectPtr<ARPGItemInstance> ItemInstance;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipped Item Widget")
	FName WidgetItemSlot;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipped Item Widget")
	TObjectPtr<UTexture2D> ItemSlotIcon;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:
	void OnEquipmentMappingChanged();
};
