// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RPGContextMenuEntryWidget.h"
#include "DemoRPG/Items/RPGItem.h"
#include "DemoRPG/Items/RPGItemInstance.h"
#include "DemoRPG/Items/Components/RPGInventory.h"
#include "RPGContextMenuWidget.generated.h"

UCLASS()
class DEMORPG_API URPGContextMenuWidget : public URPGFocusWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget), Category = "Context Menu Widget")
	class UVerticalBox* Container;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = "true"), Category = "Context Menu Widget")
	UObject* ContextMenuOperation;

	UFUNCTION(BlueprintImplementableEvent, Category = "Context Menu Widget")
	void OnMenuCreated();

	UFUNCTION(BlueprintCallable, Category = "Context Menu Widget")
	void Close();

	template <typename OwnerT>
	static URPGContextMenuWidget* CreateItemContextMenu(
		OwnerT* Owner,
		const TSubclassOf<URPGContextMenuWidget> ContextMenuWidgetClass,
		const TSubclassOf<URPGContextMenuEntryWidget> ContextMenuEntryWidgetClass,
		class ARPGCharacterInstance* TargetCharacter,
		class ARPGItemInstance* InItemInstance,
		class URPGInventoryMappingContainer* TargetContainer);

	template <typename OwnerT>
	static URPGContextMenuWidget* CreateEquippedItemContextMenu(
		OwnerT* Owner,
		const TSubclassOf<URPGContextMenuWidget> ContextMenuWidgetClass,
		const TSubclassOf<URPGContextMenuEntryWidget> ContextMenuEntryWidgetClass,
		class ARPGCharacterInstance* TargetCharacter,
		class ARPGItemInstance* InItemInstance,
		class URPGInventoryMappingContainer* TargetContainer);

protected:
	virtual void ReceivedRPGFocus(URPGFocusWidget* PreviouslyFocusedWidget) override;
	virtual void DescendantLostRPGFocus(URPGFocusWidget* Descendant, const bool bIsDirectRPGFocusableChild, URPGFocusWidget* WidgetThatReceivedFocus) override;
};

template <typename OwnerT>
URPGContextMenuWidget* URPGContextMenuWidget::CreateItemContextMenu(
	OwnerT* Owner,
	const TSubclassOf<URPGContextMenuWidget> ContextMenuWidgetClass,
	const TSubclassOf<class URPGContextMenuEntryWidget> ContextMenuEntryWidgetClass,
	class ARPGCharacterInstance* TargetCharacter,
	class ARPGItemInstance* InItemInstance,
	class URPGInventoryMappingContainer* TargetContainer)
{
	check(ContextMenuWidgetClass && ContextMenuEntryWidgetClass);
	
	URPGContextMenuWidget* ContextMenu = CreateWidget<URPGContextMenuWidget>(Owner, ContextMenuWidgetClass);
	URPGItemContextMenuOperation* Operation = NewObject<URPGItemContextMenuOperation>(ContextMenu);
	Operation->ContextMenu = ContextMenu;
	Operation->TargetCharacter = TargetCharacter;
	Operation->ItemInstance = InItemInstance;
	Operation->TargetContainer = TargetContainer;
	ContextMenu->ContextMenuOperation = Operation;

	URPGItem* ItemType = InItemInstance->GetItem();
	check(IsValid(ItemType));
	if (ItemType->bStackable && InItemInstance->GetQuantity() > 1)
	{
		URPGContextMenuEntryWidget::CreateSplitItemEntry(ContextMenu, ContextMenuEntryWidgetClass);
	}
	if (InItemInstance->GetComponentByClass(URPGInventory::StaticClass()))
	{
		URPGContextMenuEntryWidget::CreateOpenBagEntry(ContextMenu, ContextMenuEntryWidgetClass);
	}
	const URPGEquippableFeature* EquippableFeature = ItemType->GetFeature<URPGEquippableFeature>();
	if (IsValid(EquippableFeature))
	{
		URPGContextMenuEntryWidget::CreateEquipItemEntry(ContextMenu, ContextMenuEntryWidgetClass);
	}
	
	ContextMenu->OnMenuCreated();
	return ContextMenu;
}

template <typename OwnerT>
URPGContextMenuWidget* URPGContextMenuWidget::CreateEquippedItemContextMenu(
	OwnerT* Owner,
	const TSubclassOf<URPGContextMenuWidget> ContextMenuWidgetClass,
	const TSubclassOf<class URPGContextMenuEntryWidget> ContextMenuEntryWidgetClass,
	class ARPGCharacterInstance* TargetCharacter,
	class ARPGItemInstance* InItemInstance,
	class URPGInventoryMappingContainer* TargetContainer)
{
	check(ContextMenuWidgetClass && ContextMenuEntryWidgetClass);
	
	URPGContextMenuWidget* ContextMenu = CreateWidget<URPGContextMenuWidget>(Owner, ContextMenuWidgetClass);
	URPGItemContextMenuOperation* Operation = NewObject<URPGItemContextMenuOperation>(ContextMenu);
	Operation->ContextMenu = ContextMenu;
	Operation->TargetCharacter = TargetCharacter;
	Operation->ItemInstance = InItemInstance;
	Operation->TargetContainer = TargetContainer;
	ContextMenu->ContextMenuOperation = Operation;

	const URPGItem* ItemType = InItemInstance->GetItem();
	check(IsValid(ItemType));
	URPGContextMenuEntryWidget::CreateUnequipItemEntry(ContextMenu, ContextMenuEntryWidgetClass);
	
	ContextMenu->OnMenuCreated();
	return ContextMenu;
}
