// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGSplitItemWidget.h"

#include "DemoRPG/RPGPlayerController.h"
#include "DemoRPG/Components/RPGInventoryMappingContainer.h"
#include "DemoRPG/Items/RPGItemInstance.h"
#include "DemoRPG/Items/Components/RPGInventory.h"

void URPGSplitItemWidget::SetItemInstance(ARPGItemInstance* NewItemInstance)
{
	if (IsValid(ItemInstance))
	{
		ItemInstance->OnItemInstanceChangedNative.RemoveAll(this);
		ItemInstance->OnItemInstanceChanged.RemoveAll(this);
	}
	ItemInstance = NewItemInstance;
	if (IsValid(NewItemInstance))
	{
		NewItemInstance->OnItemInstanceChanged.AddDynamic(this, &URPGSplitItemWidget::RefreshWidget);
	}
	RefreshWidget();
}

void URPGSplitItemWidget::PerformSplit(const int AmountToSplit) const
{
	if (IsValid(TargetInventory) && IsValid(TargetContainer) && IsValid(ItemInstance) && AmountToSplit > 0)
	{
		ARPGPlayerController* PlayerController = Cast<ARPGPlayerController>(GetOwningPlayer());
		if (IsValid(PlayerController))
		{
			PlayerController->SplitItem(ItemInstance, AmountToSplit, TargetInventory, TargetContainer, TargetSlot);
		}
	}
}

FReply URPGSplitItemWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	return FReply::Handled();
}

FReply URPGSplitItemWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
	return FReply::Handled();
}

FReply URPGSplitItemWidget::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseWheel(InGeometry, InMouseEvent);
	return FReply::Handled();
}
