// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGFunctionLibrary.h"

#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Characters/RPGCharacterInstance.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/PanelWidget.h"
#include "Components/RPGInventoryMappingContainer.h"
#include "Components/Widget.h"
#include "Items/RPGItem.h"
#include "Items/RPGItemInstance.h"
#include "Items/Components/RPGEquippableItem.h"
#include "Items/Components/RPGInventory.h"
#include "Items/Features/RPGEquippableFeature.h"
#include "UI/RPGItemMapping.h"

bool URPGFunctionLibrary::IsWithEditor()
{
#if WITH_EDITOR
	return true;
#else
	return false;
#endif
}

bool URPGFunctionLibrary::IsDesignTime(UWidget* Widget)
{
#if WITH_EDITOR
	return Widget->IsDesignTime();
#else
	return false;
#endif
}

UUserWidget* URPGFunctionLibrary::GetParentUserWidget(const UUserWidget* Widget)
{
	const UWidget* ParentWidget = Widget;
	while (ParentWidget->GetParent())
	{
		ParentWidget = ParentWidget->GetParent();
	}
	const UObject* WidgetTree = ParentWidget->GetOuter();
	if (IsValid(WidgetTree))
	{
		return Cast<UUserWidget>(WidgetTree->GetOuter());
	}
	return nullptr;
}

bool URPGFunctionLibrary::IsChild(const UUserWidget* Descendant, const UUserWidget* Ancestor)
{
	if (Descendant == Ancestor)
	{
		return false;
	}
	
	const UUserWidget* Parent = Descendant;
	while (Parent)
	{
		if (Parent == Ancestor)
		{
			return true;
		}
		Parent = GetParentUserWidget(Parent);
	}
	return false;
}

FVector2D URPGFunctionLibrary::GetWidgetScreenSize(UObject* WorldContextObject, const UWidget* Widget)
{
	const FGeometry WidgetGeometry = Widget->GetCachedGeometry();
	const FVector2D LocalSize = WidgetGeometry.GetLocalSize();
	const FVector2D ScreenPosition = WidgetGeometry.LocalToAbsolute(FVector2D(0, 0));
	return (WidgetGeometry.LocalToAbsolute(LocalSize) - ScreenPosition) / UWidgetLayoutLibrary::GetViewportScale(WorldContextObject);
}

void URPGFunctionLibrary::MoveItem(ARPGItemInstance* ItemInstance, const bool bForceNewPosition, URPGInventory* TargetInventory, URPGInventoryMappingContainer* TargetContainer, const FRPGUISlot& TargetSlot)
{
	// Sanity checks here.
	// Item with inventory can't contain itself. Also, you can't put a bag into a bag that's inside that bag...
	check(IsValid(TargetInventory));
	URPGInventory* Inventory = ItemInstance->FindComponentByClass<URPGInventory>();
	if (IsValid(Inventory))
	{
		if (Inventory == TargetInventory)
		{
			return;
		}
		ARPGItemInstance* TargetAsItemInstance = Cast<ARPGItemInstance>(TargetInventory->GetOwner());
		if (IsValid(TargetAsItemInstance) && Inventory->ContainsItem(TargetAsItemInstance))
		{
			return;
		}
	}
	
	// Move the item between inventories
	if (ItemInstance->GetTypedItemOwner<URPGInventory>() != TargetInventory)
	{
		if (IsValid(ItemInstance->GetTypedItemOwner<URPGInventory>()))
		{
			ItemInstance->GetTypedItemOwner<URPGInventory>()->RemoveItemInstance(ItemInstance);
		}
		TargetInventory->AddItemInstance(ItemInstance);
	}

	// Position the item in the target inventory accordingly
	if (IsValid(TargetContainer) && TargetSlot.X >= 0 && TargetSlot.Y >= 0)
	{
		FRPGUISlot DestinationSlot = TargetSlot;
		if (!bForceNewPosition)
		{
			const URPGInventoryMapping* ExistingMapping = URPGInventoryMapping::FindMappingForSlot(TargetContainer, DestinationSlot);
			if (IsValid(ExistingMapping) && ExistingMapping->GetItemInstance() != ItemInstance)
			{
				DestinationSlot = URPGInventoryMapping::FindSlotForItem(ItemInstance, TargetContainer, TargetContainer->GetInventorySlotsPerRow(), DestinationSlot);
			}
		}
		URPGInventoryMapping* MappingToAdjust = URPGInventoryMapping::FindMappingForItemTyped<URPGInventoryMapping>(TargetContainer, ItemInstance);
		if (IsValid(MappingToAdjust))
		{
			MappingToAdjust->SetUISlot(DestinationSlot);
		}
	}
}

void URPGFunctionLibrary::SplitItem(ARPGItemInstance* ItemInstance, const int AmountToSplit, URPGInventory* TargetInventory, URPGInventoryMappingContainer* TargetContainer, const FRPGUISlot& TargetSlot)
{
	// No amount to transfer, nothing else to do
	if (AmountToSplit <= 0)
	{
		return;
	}
	// Transfer the entire stack to the desired slot in target inventory
	else if (AmountToSplit >= ItemInstance->GetQuantity())
	{
		MoveItem(ItemInstance, false, TargetInventory, TargetContainer, TargetSlot);
	}
	// Transfer only a certain amount to the target inventory (creating a new item instance)
	else
	{
		ItemInstance->SetQuantity(ItemInstance->GetQuantity() - AmountToSplit);
		ARPGItemInstance* NewItemInstance = ARPGItemInstance::Construct(TargetInventory, ItemInstance->GetItem(), AmountToSplit);
		TargetInventory->AddItemInstance(NewItemInstance);
		MoveItem(NewItemInstance, false, TargetInventory, TargetContainer, TargetSlot);
	}
}

void URPGFunctionLibrary::ShowEquippedItemInUI(ARPGItemInstance* ItemInstance)
{
	check(ItemInstance->GetItem());
	const URPGEquippableFeature* EquippableFeature = ItemInstance->GetItem()->GetFeature<URPGEquippableFeature>();
	if (!IsValid(EquippableFeature))
	{
		return;
	}

	const URPGEquippableItem* EquippableComponent = ItemInstance->FindComponentByClass<URPGEquippableItem>();
	if (IsValid(EquippableComponent) && IsValid(EquippableComponent->GetEquippingCharacter()))
	{
		const ARPGCharacterInstance* EquippingCharacter = EquippableComponent->GetEquippingCharacter();
		const URPGEquipmentMapping* EquipmentMapping = URPGItemMapping::FindMappingForItemTyped<URPGEquipmentMapping>(EquippingCharacter->InventoryMappingContainer, ItemInstance);
		if (!IsValid(EquipmentMapping))
		{
			URPGEquipmentMapping::Construct(ItemInstance, EquippingCharacter->InventoryMappingContainer, EquippingCharacter->GetEquippedItemSlot(ItemInstance));
		}
	}
}

void URPGFunctionLibrary::HideEquippedItemInUI(ARPGItemInstance* ItemInstance)
{
	const URPGEquippableItem* EquippableComponent = ItemInstance->FindComponentByClass<URPGEquippableItem>();
	if (IsValid(EquippableComponent) && IsValid(EquippableComponent->GetEquippingCharacter()))
	{
		const ARPGCharacterInstance* EquippingCharacter = EquippableComponent->GetEquippingCharacter();
		EquippingCharacter->InventoryMappingContainer->RemoveUIMappingsByPredicate([&](URPGUIMapping* Mapping)
		{
			const URPGEquipmentMapping* EquipmentMapping = Cast<URPGEquipmentMapping>(Mapping);
			return IsValid(EquipmentMapping) && EquipmentMapping->GetItemInstance() == ItemInstance;
		});
	}
}

void URPGFunctionLibrary::ShowItemInInventory(ARPGItemInstance* ItemInstance, URPGInventoryMappingContainer* TargetContainer, const FRPGUISlot& TargetSlot)
{
	const URPGInventoryMapping* InventoryMapping = URPGItemMapping::FindMappingForItemTyped<URPGInventoryMapping>(TargetContainer, ItemInstance);
	if (!IsValid(InventoryMapping))
	{
		const FRPGUISlot NewItemSlot = URPGInventoryMapping::GetFirstUnoccupiedSlot(TargetContainer, TargetContainer->GetInventorySlotsPerRow(), TargetSlot);
		URPGInventoryMapping::Construct(ItemInstance, TargetContainer, NewItemSlot);
	}
}

void URPGFunctionLibrary::HideItemInInventory(ARPGItemInstance* ItemInstance, URPGInventoryMappingContainer* TargetContainer)
{
	TargetContainer->RemoveUIMappingsByPredicate([&](const URPGUIMapping* UIMapping)
	{
		const URPGInventoryMapping* InventoryMapping = Cast<URPGInventoryMapping>(UIMapping);
		if (IsValid(InventoryMapping))
		{
			return InventoryMapping->GetItemInstance() == ItemInstance;
		}
		return false;
	});
}

void URPGFunctionLibrary::SetWidgetLayerPosition(UWidget* Widget, const FVector2D Position)
{
	const UPanelWidget* WidgetParent = Widget->GetParent();
	UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(Widget->Slot);
	if (IsValid(CanvasPanelSlot) && IsValid(WidgetParent))
	{
		const FGeometry WidgetGeometry = Widget->GetCachedGeometry();
		const FGeometry ParentGeometry = WidgetParent->GetCachedGeometry();
		
		const FVector2D ParentSize = ParentGeometry.GetLocalSize();
		const FVector2D WidgetSize = WidgetGeometry.GetAbsoluteSize();
		
		FVector2D FinalPosition = ParentGeometry.AbsoluteToLocal(Position);
		const FVector2D WidgetCorner = ParentGeometry.AbsoluteToLocal(Position + WidgetSize);
		FinalPosition.X = FMath::Max(0.0f, FinalPosition.X - FMath::Max(0.0f, WidgetCorner.X - ParentSize.X));
		FinalPosition.Y = FMath::Max(0.0f, FinalPosition.Y - FMath::Max(0.0f, WidgetCorner.Y - ParentSize.Y));
		
		const FMargin FinalPadding =
		{
			static_cast<float>(FinalPosition.X),
			static_cast<float>(FinalPosition.Y),
			0.0f,
			0.0f
		};
		CanvasPanelSlot->SetOffsets(FinalPadding);
	}
}

FName URPGFunctionLibrary::GetGameplayTagShortName(const FGameplayTag& InTag)
{
	const FString TagName = InTag.GetTagName().ToString();
	TArray<FString> SplitElements;
	TagName.ParseIntoArray(SplitElements, TEXT("."));
	if (SplitElements.Num() > 0)
	{
		return FName(SplitElements[SplitElements.Num() - 1]);
	}
	return NAME_None;
}
