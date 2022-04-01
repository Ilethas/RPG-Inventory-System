// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGHUD.h"

#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "DemoRPG/RPGFunctionLibrary.h"
#include "DemoRPG/UI/RPGBagWidget.h"
#include "DemoRPG/UI/RPGCursor.h"
#include "DemoRPG/UI/RPGFocusWidget.h"
#include "DemoRPG/UI/RPGHUDWidget.h"
#include "DemoRPG/UI/RPGInventoryScreenWidget.h"
#include "DemoRPG/UI/RPGSplitItemWidget.h"

void ARPGHUD::ShowCursorOnDrop(UDragDropOperation* Operation)
{
	PlayerOwner->bShowMouseCursor = true;
	if (IsValid(Cursor))
	{
		Cursor->CursorImage->SetRenderOpacity(1.0f);
	}
}

void ARPGHUD::HideCursorOnDrag(UDragDropOperation* Operation)
{
	if (IsValid(Cursor))
	{
		Cursor->CursorImage->SetRenderOpacity(0.0f);
	}
}

void ARPGHUD::ShowSplitItemWidget(class ARPGItemInstance* ItemInstance, class URPGInventory* TargetInventory, class URPGInventoryMappingContainer* TargetContainer, const FRPGUISlot& TargetSlot)
{
	if (!IsValid(SplitItemWidget))
	{
		SplitItemWidget = CreateWidget<URPGSplitItemWidget>(PlayerOwner, SplitItemWidgetClass);
	}
	SplitItemWidget->SetItemInstance(ItemInstance);
	SplitItemWidget->TargetInventory = TargetInventory;
	SplitItemWidget->TargetContainer = TargetContainer;
	SplitItemWidget->TargetSlot = TargetSlot;
	SplitItemWidget->AddToViewport();
}

void ARPGHUD::HideSplitItemWidget()
{
	if (IsValid(SplitItemWidget))
	{
		SplitItemWidget->RemoveFromViewport();
		SplitItemWidget->SetItemInstance(nullptr);
		SplitItemWidget->TargetInventory = nullptr;
		SplitItemWidget->TargetContainer = nullptr;
		SplitItemWidget->TargetSlot = FRPGUISlot(-1, -1);
	}
}

void ARPGHUD::ShowBagWidget(ARPGItemInstance* BagItem)
{
	if (!IsValid(HUDWidget) || !IsValid(HUDWidget->BagLayer))
	{
		return;
	}
	URPGBagWidget* BagWidget = nullptr;
	{
		for (int i = 0; i < HUDWidget->BagLayer->GetChildrenCount(); i++)
		{
			 if (URPGBagWidget* Widget = Cast<URPGBagWidget>(HUDWidget->BagLayer->GetChildAt(i)))
			 {
				 if (Widget->GetBagItemInstance() == BagItem)
				 {
				 	BagWidget = Widget;
				 	break;
				 }
			 }
		}
	}
	if (IsValid(BagWidget))
	{
		BagWidget->RemoveFromParent();
	}
	else if (IsValid(BagWidgetClass))
	{
		BagWidget = CreateWidget<URPGBagWidget>(PlayerOwner, BagWidgetClass);
		BagWidget->SetBagItemInstance(BagItem);
	}

	if (IsValid(BagWidget))
	{
		const FVector2D CursorPosition = FSlateApplication::Get().GetCursorPos();
		HUDWidget->BagLayer->AddChild(BagWidget);
		if (UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(BagWidget->Slot))
		{
			CanvasPanelSlot->SetAutoSize(true);
			URPGFunctionLibrary::SetWidgetLayerPosition(BagWidget, CursorPosition);
			BagWidget->SetVisibility(ESlateVisibility::Visible);
			SetCurrentlyFocusedWidget(BagWidget);
		}
	}
}

void ARPGHUD::ShowInventoryScreen()
{
	if (IsValid(HUDWidget))
	{
		HUDWidget->InventoryScreen->SetVisibility(ESlateVisibility::Visible);
	}
}

void ARPGHUD::HideInventoryScreen()
{
	if (IsValid(HUDWidget))
	{
		HUDWidget->InventoryScreen->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void ARPGHUD::ToggleInventoryScreen()
{
	if (IsValid(HUDWidget))
	{
		if (HUDWidget->InventoryScreen->IsVisible())
		{
			HUDWidget->InventoryScreen->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			HUDWidget->InventoryScreen->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void ARPGHUD::SetCurrentlyFocusedWidget(URPGFocusWidget* WidgetToGiveFocus)
{
	Super::SetCurrentlyFocusedWidget(WidgetToGiveFocus);
}

void ARPGHUD::BeginPlay()
{
	Super::BeginPlay();
}
