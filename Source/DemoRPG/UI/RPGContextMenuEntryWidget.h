// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RPGButtonWidget.h"
#include "DemoRPG/Items/Components/RPGEquippableItem.h"
#include "RPGContextMenuEntryWidget.generated.h"

UCLASS()
class DEMORPG_API URPGContextMenuOperation : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Category = "Item Context Menu Operation")
	class URPGContextMenuWidget* ContextMenu;
};

UCLASS()
class DEMORPG_API URPGItemContextMenuOperation : public URPGContextMenuOperation
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Category = "Item Context Menu Operation")
	ARPGCharacterInstance* TargetCharacter;
	
	UPROPERTY(BlueprintReadOnly, Category = "Item Context Menu Operation")
	class ARPGItemInstance* ItemInstance;

	UPROPERTY(BlueprintReadOnly, Category = "Item Context Menu Operation")
	class URPGInventoryMappingContainer* TargetContainer;

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
	class URPGContextMenuWidget* OwnerContextMenu;
	
	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget), Category = "Context Menu Entry Widget")
	class UTextBlock* EntryText;

	static URPGContextMenuEntryWidget* CreateSplitItemEntry(
		class URPGContextMenuWidget* ContextMenu,
		const TSubclassOf<URPGContextMenuEntryWidget> ContextMenuEntryWidgetClass);

	static URPGContextMenuEntryWidget* CreateOpenBagEntry(
		class URPGContextMenuWidget* ContextMenu,
		const TSubclassOf<URPGContextMenuEntryWidget> ContextMenuEntryWidgetClass);

	static URPGContextMenuEntryWidget* CreateEquipItemEntry(
		class URPGContextMenuWidget* ContextMenu,
		const TSubclassOf<URPGContextMenuEntryWidget> ContextMenuEntryWidgetClass);

	static URPGContextMenuEntryWidget* CreateUnequipItemEntry(
		class URPGContextMenuWidget* ContextMenu,
		const TSubclassOf<URPGContextMenuEntryWidget> ContextMenuEntryWidgetClass);
};
