// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGItemDragWidget.h"

#include "RPGInventoryItemWidget.h"
#include "RPGItemMapping.h"
#include "Components/SizeBox.h"
#include "DemoRPG/RPGFunctionLibrary.h"
#include "DemoRPG/Items/RPGItemInstance.h"

void URPGItemDragWidget::SetSize(const FVector2D& NewSize)
{
	if (IsValid(SizeBox))
	{
		SizeBox->SetWidthOverride(NewSize.X);
		SizeBox->SetHeightOverride(NewSize.Y);
	}
	RefreshWidget();
}

void URPGItemDragWidget::SetItemMapping(URPGItemMapping* InItemMapping)
{
	// Unsubscribe from item instance, if we change dragged widget
	if (IsValid(ItemMapping) && IsValid(ItemMapping->GetItemInstance()))
	{
		ItemMapping->GetItemInstance()->OnItemInstanceChangedNative.RemoveAll(this);
		ItemMapping->GetItemInstance()->OnItemInstanceChanged.RemoveAll(this);
	}

	ItemMapping = InItemMapping;
	if (IsValid(InItemMapping) && IsValid(InItemMapping->GetItemInstance()))
	{
		InItemMapping->GetItemInstance()->OnItemInstanceChanged.AddDynamic(this, &URPGItemDragWidget::RefreshWidget);
	}
	RefreshWidget();
}
