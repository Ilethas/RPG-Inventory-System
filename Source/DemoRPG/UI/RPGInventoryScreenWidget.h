// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RPGFocusWidget.h"
#include "RPGInventoryScreenWidget.generated.h"

class URPGEquippedItemWidget;
class URPGUIMapping;
/**
 * 
 */
UCLASS()
class DEMORPG_API URPGInventoryScreenWidget : public URPGFocusWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = HUD)
	void RefreshWidget();
	
	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget), Category = HUD)
	class URPGInventoryWidget* CharacterInventory;
	
	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget), Category = HUD)
	class URPGInventoryWidget* PartyStash;

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = HUD)
	void SetCurrentCharacter(class ARPGCharacterInstance* NewCharacter);
	
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Category = HUD)
	class ARPGCharacterInstance* GetCurrentCharacter() const { return CurrentCharacter; }

protected:
	UPROPERTY(BlueprintSetter = SetCurrentCharacter, BlueprintGetter = GetCurrentCharacter, Category = HUD)
	class ARPGCharacterInstance* CurrentCharacter;

	UPROPERTY(BlueprintReadWrite, Category = HUD)
	TMap<FName, URPGEquippedItemWidget*> ItemSlotWidgets;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:
	void OnEquipmentMappingRemoved(URPGUIMapping* AddedUIMapping);
	void OnEquipmentMappingAdded(URPGUIMapping* AddedUIMapping);
};
