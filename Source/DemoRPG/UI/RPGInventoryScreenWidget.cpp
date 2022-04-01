// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGInventoryScreenWidget.h"

#include "RPGEquippedItemWidget.h"
#include "RPGItemMapping.h"
#include "DemoRPG/Characters/RPGCharacterInstance.h"
#include "DemoRPG/Components/RPGInventoryMappingContainer.h"

void URPGInventoryScreenWidget::SetCurrentCharacter(ARPGCharacterInstance* NewCharacter)
{
	if (IsValid(CurrentCharacter))
	{
		CurrentCharacter->InventoryMappingContainer->OnUIMappingAddedNative.RemoveAll(this);
		CurrentCharacter->InventoryMappingContainer->OnUIMappingRemovedNative.RemoveAll(this);
	}
	CurrentCharacter = NewCharacter;
	if (IsValid(NewCharacter))
	{
		NewCharacter->InventoryMappingContainer->OnUIMappingAddedNative.AddUObject(this, &URPGInventoryScreenWidget::OnEquipmentMappingAdded);
		NewCharacter->InventoryMappingContainer->OnUIMappingRemovedNative.AddUObject(this, &URPGInventoryScreenWidget::OnEquipmentMappingRemoved);
	}
	
	// Clear Equipment Mappings for item slots
	for (const TPair<FName, URPGEquippedItemWidget*>& Pair : ItemSlotWidgets)
	{
		Pair.Value->SetEquipmentMapping(nullptr);
	}

	// Set Equipment Mappings for slots that have items in them for the currently selected character
	if (IsValid(NewCharacter))
	{
		NewCharacter->InventoryMappingContainer->ForEachUIMapping([this](URPGUIMapping* UIMapping)
		{
			URPGEquipmentMapping* EquipmentMapping = Cast<URPGEquipmentMapping>(UIMapping);
			if (IsValid(EquipmentMapping))
			{
				URPGEquippedItemWidget** FoundWidget = ItemSlotWidgets.Find(EquipmentMapping->GetItemSlot());
				if (FoundWidget)
				{
					(*FoundWidget)->SetEquipmentMapping(EquipmentMapping);
				}
			}
			return false;
		});
	}
	RefreshWidget();
}

FReply URPGInventoryScreenWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	return FReply::Handled();
}

FReply URPGInventoryScreenWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
	return FReply::Handled();
}

FReply URPGInventoryScreenWidget::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseWheel(InGeometry, InMouseEvent);
	return FReply::Handled();
}

void URPGInventoryScreenWidget::OnEquipmentMappingRemoved(URPGUIMapping* AddedUIMapping)
{
	const URPGEquipmentMapping* EquipmentMapping = Cast<URPGEquipmentMapping>(AddedUIMapping);
	if (IsValid(EquipmentMapping))
	{
		URPGEquippedItemWidget** FoundWidget = ItemSlotWidgets.Find(EquipmentMapping->GetItemSlot());
		if (FoundWidget)
		{
			(*FoundWidget)->SetEquipmentMapping(nullptr);
		}
	}
}

void URPGInventoryScreenWidget::OnEquipmentMappingAdded(URPGUIMapping* AddedUIMapping)
{
	URPGEquipmentMapping* EquipmentMapping = Cast<URPGEquipmentMapping>(AddedUIMapping);
	if (IsValid(EquipmentMapping))
	{
		URPGEquippedItemWidget** FoundWidget = ItemSlotWidgets.Find(EquipmentMapping->GetItemSlot());
		if (FoundWidget)
		{
			(*FoundWidget)->SetEquipmentMapping(EquipmentMapping);
		}
	}
}
