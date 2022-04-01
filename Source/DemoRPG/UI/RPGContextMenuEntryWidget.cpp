// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGContextMenuEntryWidget.h"

#include "RPGContextMenuWidget.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "DemoRPG/RPGPlayerController.h"
#include "DemoRPG/Characters/RPGCharacterInstance.h"
#include "DemoRPG/Framework/RPGHUD.h"
#include "DemoRPG/Items/RPGItemInstance.h"

#define LOCTEXT_NAMESPACE "ContextMenu"

void URPGItemContextMenuOperation::SplitItem()
{
	if (IsValid(ContextMenu))
	{
		ARPGHUD* HUD = Cast<ARPGHUD>(ContextMenu->GetOwningPlayer()->GetHUD());
		if (IsValid(HUD))
		{
			HUD->ShowSplitItemWidget(ItemInstance, ItemInstance->GetTypedItemOwner<URPGInventory>(), TargetContainer, FRPGUISlot(-1, -1));
		}
		ContextMenu->Close();
	}
}

void URPGItemContextMenuOperation::OpenBag()
{
	if (IsValid(ContextMenu))
	{
		ARPGHUD* HUD = Cast<ARPGHUD>(ContextMenu->GetOwningPlayer()->GetHUD());
		if (IsValid(HUD))
		{
			HUD->ShowBagWidget(ItemInstance);
		}
		ContextMenu->Close();
	}
}

void URPGItemContextMenuOperation::EquipItem()
{
	if (IsValid(ContextMenu))
	{
		if (IsValid(TargetCharacter))
		{
			const FName EquipmentSlot = TargetCharacter->FindSlotForItem(ItemInstance);
			TargetCharacter->GetWorld()->GetFirstPlayerController<ARPGPlayerController>()->EquipItem(TargetCharacter, ItemInstance, EquipmentSlot);
		}
		ContextMenu->Close();
	}
}

void URPGItemContextMenuOperation::UnequipItem()
{
	if (IsValid(ContextMenu))
	{
		if (IsValid(TargetCharacter))
		{
			ARPGPlayerController* Controller = TargetCharacter->GetWorld()->GetFirstPlayerController<ARPGPlayerController>();
			ARPGCharacterInstance* CharacterInstance = Controller->GetSelectedCharacter();	
			URPGInventory* Inventory = CharacterInstance->InventoryComponent;
			URPGInventoryMappingContainer* UIContainer = CharacterInstance->InventoryMappingContainer;

			Controller->UnequipItem(CharacterInstance, ItemInstance, Inventory, UIContainer, { -1, -1 });
		}
		ContextMenu->Close();
	}
}

URPGContextMenuEntryWidget* URPGContextMenuEntryWidget::CreateSplitItemEntry(
	URPGContextMenuWidget* ContextMenu,
	const TSubclassOf<URPGContextMenuEntryWidget> ContextMenuEntryWidgetClass)
{
	URPGItemContextMenuOperation* Operation = Cast<URPGItemContextMenuOperation>(ContextMenu->ContextMenuOperation);
	if (IsValid(Operation) && ContextMenuEntryWidgetClass)
	{
		URPGContextMenuEntryWidget* Entry = CreateWidget<URPGContextMenuEntryWidget>(ContextMenu, ContextMenuEntryWidgetClass);
		Entry->OwnerContextMenu = ContextMenu;
		Entry->OnClickedNative.AddUObject(Operation, &URPGItemContextMenuOperation::SplitItem);

		if (IsValid(Entry->EntryText))
		{
			Entry->EntryText->SetText(LOCTEXT("ContextMenuSplitItemText", "Split"));
		}
		if (IsValid(ContextMenu->Container))
		{
			ContextMenu->Container->AddChild(Entry);
		}
		return Entry;
	}
	return nullptr;
}

URPGContextMenuEntryWidget* URPGContextMenuEntryWidget::CreateOpenBagEntry(
	URPGContextMenuWidget* ContextMenu,
	const TSubclassOf<URPGContextMenuEntryWidget> ContextMenuEntryWidgetClass)
{
	URPGItemContextMenuOperation* Operation = Cast<URPGItemContextMenuOperation>(ContextMenu->ContextMenuOperation);
	if (IsValid(Operation) && ContextMenuEntryWidgetClass)
	{
		URPGContextMenuEntryWidget* Entry = CreateWidget<URPGContextMenuEntryWidget>(ContextMenu, ContextMenuEntryWidgetClass);
		Entry->OwnerContextMenu = ContextMenu;
		Entry->OnClickedNative.AddUObject(Operation, &URPGItemContextMenuOperation::OpenBag);

		if (IsValid(Entry->EntryText))
		{
			Entry->EntryText->SetText(LOCTEXT("ContextMenuOpenBagText", "Open"));
		}
		if (IsValid(ContextMenu->Container))
		{
			ContextMenu->Container->AddChild(Entry);
		}
		return Entry;
	}
	return nullptr;
}

URPGContextMenuEntryWidget* URPGContextMenuEntryWidget::CreateEquipItemEntry(URPGContextMenuWidget* ContextMenu, const TSubclassOf<URPGContextMenuEntryWidget> ContextMenuEntryWidgetClass)
{
	URPGItemContextMenuOperation* Operation = Cast<URPGItemContextMenuOperation>(ContextMenu->ContextMenuOperation);
	if (IsValid(Operation) && ContextMenuEntryWidgetClass)
	{
		URPGContextMenuEntryWidget* Entry = CreateWidget<URPGContextMenuEntryWidget>(ContextMenu, ContextMenuEntryWidgetClass);
		Entry->OwnerContextMenu = ContextMenu;
		Entry->OnClickedNative.AddUObject(Operation, &URPGItemContextMenuOperation::EquipItem);

		if (IsValid(Entry->EntryText))
		{
			Entry->EntryText->SetText(LOCTEXT("ContextMenuEquipText", "Equip"));
		}
		if (IsValid(ContextMenu->Container))
		{
			ContextMenu->Container->AddChild(Entry);
		}
		return Entry;
	}
	return nullptr;
}

URPGContextMenuEntryWidget* URPGContextMenuEntryWidget::CreateUnequipItemEntry(URPGContextMenuWidget* ContextMenu, const TSubclassOf<URPGContextMenuEntryWidget> ContextMenuEntryWidgetClass)
{
	URPGItemContextMenuOperation* Operation = Cast<URPGItemContextMenuOperation>(ContextMenu->ContextMenuOperation);
	if (IsValid(Operation) && ContextMenuEntryWidgetClass)
	{
		URPGContextMenuEntryWidget* Entry = CreateWidget<URPGContextMenuEntryWidget>(ContextMenu, ContextMenuEntryWidgetClass);
		Entry->OwnerContextMenu = ContextMenu;
		Entry->OnClickedNative.AddUObject(Operation, &URPGItemContextMenuOperation::UnequipItem);

		if (IsValid(Entry->EntryText))
		{
			Entry->EntryText->SetText(LOCTEXT("ContextMenuUnequipText", "Unequip"));
		}
		if (IsValid(ContextMenu->Container))
		{
			ContextMenu->Container->AddChild(Entry);
		}
		return Entry;
	}
	return nullptr;
}

#undef LOCTEXT_NAMESPACE
