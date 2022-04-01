// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGInventoryWidget.h"

#include "RPGInventoryItemWidget.h"
#include "RPGInventoryRowWidget.h"
#include "RPGItemMapping.h"
#include "Components/HorizontalBox.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "DemoRPG/RPGFunctionLibrary.h"
#include "DemoRPG/Components/RPGInventoryMappingContainer.h"

void URPGInventoryWidget::SetInventory(URPGInventory* NewInventory)
{
	Inventory = NewInventory;
}

void URPGInventoryWidget::SetInventoryMappingContainer(URPGInventoryMappingContainer* NewInventoryMappingContainer)
{
	// Unsubscribe all bindings from previous inventory component
	if (IsValid(InventoryMappingContainer))
	{
		InventoryMappingContainer->OnUIMappingRemovedNative.RemoveAll(this);
		InventoryMappingContainer->OnUIMappingAddedNative.RemoveAll(this);
		InventoryMappingContainer->ForEachUIMapping([this](URPGUIMapping* UIMapping)
		{
			URPGInventoryMapping* InventoryMapping = Cast<URPGInventoryMapping>(UIMapping);
			if (IsValid(InventoryMapping))
			{
				InventoryMapping->OnInventoryMappingUISlotChangedNative.RemoveAll(this);
				InventoryMapping->OnInventoryMappingUISlotChanged.RemoveAll(this);
				UpdateItemSlot(InventoryMapping->GetUISlot(), nullptr);
			}
			return false;
		});
	}

	// Bind to mappings from the new inventory component
	InventoryMappingContainer = NewInventoryMappingContainer;
	ClearItemWidgetsMappings();
	RefreshInventory();
	if (IsValid(NewInventoryMappingContainer))
	{
		NewInventoryMappingContainer->OnUIMappingRemovedNative.AddUObject(this, &URPGInventoryWidget::OnInventoryMappingRemoved);
		NewInventoryMappingContainer->OnUIMappingAddedNative.AddUObject(this, &URPGInventoryWidget::OnInventoryMappingAdded);
		NewInventoryMappingContainer->ForEachUIMapping([this](URPGUIMapping* UIMapping)
		{
			URPGInventoryMapping* InventoryMapping = Cast<URPGInventoryMapping>(UIMapping);
			if (IsValid(InventoryMapping))
			{
				InventoryMapping->OnInventoryMappingUISlotChangedNative.AddUObject(this, &URPGInventoryWidget::OnInventoryUISlotChanged);
				UpdateItemSlot(InventoryMapping->GetUISlot(), InventoryMapping);
			}
			return false;
		});
	}
}

void URPGInventoryWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (DeterminedItemSlotSize != 0.0f)
	{
		return;
	}

	if (IsValid(Container))
	{
		const FVector2D RowSize = URPGFunctionLibrary::GetWidgetScreenSize(this, Container);
		if (RowSize.X > 0)
		{
			const int SlotsAmount = IsValid(InventoryMappingContainer) ? InventoryMappingContainer->GetInventorySlotsPerRow() : DefaultInventorySlotsPerRow;
			DeterminedItemSlotSize = (RowSize.X - ItemSlotPadding * (SlotsAmount + 1)) / SlotsAmount;
		}
	}
	RefreshInventory();
}

void URPGInventoryWidget::OnInventoryMappingRemoved(URPGUIMapping* RemovedUIMapping)
{
	URPGInventoryMapping* InventoryMapping = Cast<URPGInventoryMapping>(RemovedUIMapping);
	if (IsValid(InventoryMapping))
	{
		const URPGInventoryItemWidget* ItemWidget = GetItemWidgetForSlot(InventoryMapping->GetUISlot());
		if (IsValid(ItemWidget) && ItemWidget->GetInventoryMapping() == RemovedUIMapping)
		{
			InventoryMapping->OnInventoryMappingUISlotChangedNative.RemoveAll(this);
			InventoryMapping->OnInventoryMappingUISlotChanged.RemoveAll(this);
			UpdateItemSlot(InventoryMapping->GetUISlot(), nullptr);
		}
	}
	RefreshInventory();
}

void URPGInventoryWidget::OnInventoryMappingAdded(URPGUIMapping* AddedUIMapping)
{
	RefreshInventory();
	
	URPGInventoryMapping* InventoryMapping = Cast<URPGInventoryMapping>(AddedUIMapping);
	if (IsValid(InventoryMapping))
	{
		InventoryMapping->OnInventoryMappingUISlotChangedNative.AddUObject(this, &URPGInventoryWidget::OnInventoryUISlotChanged);
		UpdateItemSlot(InventoryMapping->GetUISlot(), InventoryMapping);
	}
}

void URPGInventoryWidget::OnInventoryUISlotChanged(const FRPGUISlot OldUISlot, URPGInventoryMapping* InventoryMapping)
{
	RefreshInventory();
	
	// Only clear Item Widget if there is no UI Mapping for its slot
	if (!URPGInventoryMapping::FindMappingForSlot(InventoryMappingContainer, OldUISlot))
	{
		UpdateItemSlot(OldUISlot, nullptr);
	}
	UpdateItemSlot(InventoryMapping->GetUISlot(), InventoryMapping);
}

void URPGInventoryWidget::CreateInventoryRows(const int RowsAmount, const int SlotsPerRow, const float ItemPadding)
{
	if (!(RowWidgetClass  && ItemSlotWidgetClass && Container))
	{
		return;
	}

	const int CurrentRowsAmount = Container->GetChildrenCount();
	for (int i = 0; i < RowsAmount; i++)
	{
		URPGInventoryRowWidget* InventoryRow = CreateWidget<URPGInventoryRowWidget>(this, RowWidgetClass);
		UVerticalBoxSlot* VerticalBoxSlot = Container->AddChildToVerticalBox(InventoryRow);
		VerticalBoxSlot->SetHorizontalAlignment(HAlign_Center);
		
		if (IsValid(InventoryRow->Container))
		{
			for (int j = 0; j < SlotsPerRow; j++)
			{
				const FMargin PaddingStruct =
				{
					j == 0 ? ItemPadding : ItemPadding / 2.0f,
					CurrentRowsAmount + i == 0 ? ItemPadding : ItemPadding / 2.0f,
					j == SlotsPerRow - 1 ? ItemPadding : ItemPadding / 2.0f,
					ItemPadding / 2.0f,
				};
				CreateWidgetForRow(InventoryRow, FRPGUISlot(j, CurrentRowsAmount + i), PaddingStruct);
			}
		}
	}
}

void URPGInventoryWidget::RebuildInventory(const int RowsAmount, const int SlotsPerRow, const float ItemPadding)
{
	if (!IsValid(Container))
	{
		return;
	}
	
	const int ChildrenCount = Container->GetChildrenCount();
	if (ChildrenCount > RowsAmount)
	{
		// Reduce the amount of rows
		for (int i = ChildrenCount - 1; i >= RowsAmount; i--)
		{
			Container->GetChildAt(i)->RemoveFromParent();
		}
	}
	else if (ChildrenCount < RowsAmount)
	{
		// Spawn more rows
		CreateInventoryRows(RowsAmount - ChildrenCount, SlotsPerRow, ItemPadding);
	}
	
	// Make sure the amount of slots for unchanged rows is correct
	for (int i = 0; i < FMath::Min(ChildrenCount, RowsAmount); i++)
	{
		const URPGInventoryRowWidget* InventoryRow = Cast<URPGInventoryRowWidget>(Container->GetChildAt(i));
		if (IsValid(InventoryRow))
		{
			RebuildInventoryRow(InventoryRow, i, SlotsPerRow, ItemPadding);
		}
	}
}

void URPGInventoryWidget::UpdateItemSlot(const FRPGUISlot& InItemSlot, URPGInventoryMapping* InventoryMapping) const
{
	URPGInventoryItemWidget* ItemWidget = GetItemWidgetForSlot(InItemSlot);
	if (IsValid(ItemWidget))
	{
		ItemWidget->SetInventoryMapping(InventoryMapping);
	}
}

URPGInventoryItemWidget* URPGInventoryWidget::GetItemWidgetForSlot(const FRPGUISlot& InItemSlot) const
{
	if (!IsValid(Container) || InItemSlot.Y >= Container->GetChildrenCount())
	{
		return nullptr;
	}

	const URPGInventoryRowWidget* InventoryRow = Cast<URPGInventoryRowWidget>(Container->GetChildAt(InItemSlot.Y));
	if (IsValid(InventoryRow->Container) && InItemSlot.X < InventoryRow->Container->GetChildrenCount())
	{
		return Cast<URPGInventoryItemWidget>(InventoryRow->Container->GetChildAt(InItemSlot.X));
	}
	return nullptr;
}

URPGInventoryItemWidget* URPGInventoryWidget::CreateWidgetForRow(const URPGInventoryRowWidget* InventoryRow, const FRPGUISlot& InItemSlot, const FMargin& PaddingStruct)
{
	if (IsValid(InventoryRow->Container))
	{
		URPGInventoryItemWidget* ItemSlotWidget = CreateWidget<URPGInventoryItemWidget>(this, ItemSlotWidgetClass);
		ItemSlotWidget->OwnerInventoryWidget = this;
		ItemSlotWidget->UISlot = InItemSlot;
		InventoryRow->Container->AddChildToHorizontalBox(ItemSlotWidget);
		ItemSlotWidget->SetPadding(PaddingStruct);

		// Assign a UI binding to the new item widget, if possible
		if (IsValid(InventoryMappingContainer))
		{
			InventoryMappingContainer->ForEachUIMapping([&](URPGUIMapping* UIMapping)
			{
				URPGInventoryMapping* InventoryMapping = Cast<URPGInventoryMapping>(UIMapping);
				if (IsValid(InventoryMapping) && InventoryMapping->GetUISlot() == ItemSlotWidget->UISlot)
				{
					ItemSlotWidget->SetInventoryMapping(InventoryMapping);
					return true;
				}
				return false;
			});
		}
		return ItemSlotWidget;
	}
	return nullptr;
}

void URPGInventoryWidget::RebuildInventoryRow(const URPGInventoryRowWidget* InventoryRow, const int RowIndex, const int AmountOfItemSlots, const float ItemPadding)
{
	if (IsValid(InventoryRow->Container))
	{
		const int ItemSlotsCount = InventoryRow->Container->GetChildrenCount();
		if (ItemSlotsCount < AmountOfItemSlots)
		{
			// Row should have more item slots
			const int AmountToCreate = AmountOfItemSlots - ItemSlotsCount;
			for (int i = 0; i < AmountToCreate; i++)
			{
				const FMargin PaddingStruct =
				{
					i == 0 ? ItemPadding : ItemPadding / 2.0f,
					RowIndex == 0 ? ItemPadding : ItemPadding / 2.0f,
					i == AmountToCreate - 1 ? ItemPadding : ItemPadding / 2.0f,
					ItemPadding / 2.0f,
				};
				CreateWidgetForRow(InventoryRow, FRPGUISlot(ItemSlotsCount + i, RowIndex), PaddingStruct);
			}
		}
		else if (ItemSlotsCount > AmountOfItemSlots)
		{
			// Row should have less item slots
			for (int i = ItemSlotsCount - 1; i >= AmountOfItemSlots; i--)
			{
				InventoryRow->Container->GetChildAt(i)->RemoveFromParent();
			}
			
			// Adjust padding of the last item slot
			if (AmountOfItemSlots > 0)
			{
				UUserWidget* LastItem = Cast<UUserWidget>(InventoryRow->Container->GetChildAt(AmountOfItemSlots - 1));
				if (IsValid(LastItem))
				{
					FMargin AdjustedPadding = LastItem->Padding;
					AdjustedPadding.Right = ItemPadding;
					LastItem->SetPadding(AdjustedPadding);
				}
			}
		}
	}
}

void URPGInventoryWidget::RefreshInventory()
{
	if (DeterminedItemSlotSize != 0.0f)
	{
		int RowsAmount;
		if (RowsAmountOverride != 0)
		{
			RowsAmount = RowsAmountOverride;
		}
		else
		{
			RowsAmount = URPGFunctionLibrary::GetWidgetScreenSize(this, this).Y / (DeterminedItemSlotSize + ItemSlotPadding);
		}
		if (IsValid(InventoryMappingContainer))
		{
			InventoryMappingContainer->ForEachUIMapping([&](const URPGUIMapping* UIMapping)
			{
				const URPGInventoryMapping* InventoryMapping = Cast<URPGInventoryMapping>(UIMapping);
				if (IsValid(InventoryMapping))
				{
					// We add +2 to UISlot because it holds indices. So for slot (0, 5) we'd like to have at least 7 rows
					RowsAmount = FMath::Max(RowsAmount, InventoryMapping->GetUISlot().Y + 2);
				}
				return false;
			});
			RebuildInventory(RowsAmount, InventoryMappingContainer->GetInventorySlotsPerRow(), ItemSlotPadding);
		}
		else
		{
			RebuildInventory(RowsAmount, DefaultInventorySlotsPerRow, ItemSlotPadding);
		}
	}
}

void URPGInventoryWidget::ClearItemWidgetsMappings() const
{
	if (IsValid(Container))
	{
		for (int i = 0; i < Container->GetChildrenCount(); i++)
		{
			const URPGInventoryRowWidget* InventoryRow = Cast<URPGInventoryRowWidget>(Container->GetChildAt(i));
			if (IsValid(InventoryRow->Container))
			{
				for (int j = 0; j < InventoryRow->Container->GetChildrenCount(); j++)
				{
					URPGInventoryItemWidget* ItemWidget = Cast<URPGInventoryItemWidget>(InventoryRow->Container->GetChildAt(j));
					if (IsValid(ItemWidget))
					{
						ItemWidget->SetInventoryMapping(nullptr);
					}
				}
			}
		}
	}
}
