// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RPGButtonWidget.h"
#include "DemoRPG/Items/Components/RPGEquippableItem.h"
#include "RPGContextMenuEntryWidget.generated.h"

class UTextBlock;
class URPGInventoryMappingContainer;
class URPGContextMenuWidget;
UCLASS()
class DEMORPG_API URPGContextMenuOperation : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Category = "Item Context Menu Operation")
	TObjectPtr<URPGContextMenuWidget> ContextMenu;
};

UCLASS()
class DEMORPG_API URPGItemContextMenuOperation : public URPGContextMenuOperation
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Category = "Item Context Menu Operation")
	TObjectPtr<ARPGCharacterInstance> TargetCharacter;
	
	UPROPERTY(BlueprintReadOnly, Category = "Item Context Menu Operation")
	TObjectPtr<ARPGItemInstance> ItemInstance;

	UPROPERTY(BlueprintReadOnly, Category = "Item Context Menu Operation")
	TObjectPtr<URPGInventoryMappingContainer> TargetContainer;

	UFUNCTION(BlueprintCallable, Category = "Item Context Menu Operation")
	void SplitItem();

	UFUNCTION(BlueprintCallable, Category = "Item Context Menu Operation")
	void OpenBag();

	UFUNCTION(BlueprintCallable, Category = "Item Context Menu Operation")
	void EquipItem();

	UFUNCTION(BlueprintCallable, Category = "Item Context Menu Operation")
	void UnequipItem();
};

UCLASS()
class DEMORPG_API URPGContextMenuEntryWidget : public URPGButtonWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = "true"), Category = "Context Menu Entry Widget")
	TObjectPtr<URPGContextMenuWidget> OwnerContextMenu;
	
	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget), Category = "Context Menu Entry Widget")
	TObjectPtr<UTextBlock> EntryText;

	static URPGContextMenuEntryWidget* CreateSplitItemEntry(
		URPGContextMenuWidget* ContextMenu,
		const TSubclassOf<URPGContextMenuEntryWidget> ContextMenuEntryWidgetClass);

	static URPGContextMenuEntryWidget* CreateOpenBagEntry(
		URPGContextMenuWidget* ContextMenu,
		const TSubclassOf<URPGContextMenuEntryWidget> ContextMenuEntryWidgetClass);

	static URPGContextMenuEntryWidget* CreateEquipItemEntry(
		URPGContextMenuWidget* ContextMenu,
		const TSubclassOf<URPGContextMenuEntryWidget> ContextMenuEntryWidgetClass);

	static URPGContextMenuEntryWidget* CreateUnequipItemEntry(
		URPGContextMenuWidget* ContextMenu,
		const TSubclassOf<URPGContextMenuEntryWidget> ContextMenuEntryWidgetClass);
};
