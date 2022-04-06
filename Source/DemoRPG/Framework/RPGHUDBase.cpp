#include "RPGHUDBase.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Overlay.h"
#include "DemoRPG/UI/RPGBagWidget.h"
#include "DemoRPG/UI/RPGCursor.h"
#include "DemoRPG/UI/RPGHUDWidget.h"
#include "DemoRPG/UI/RPGTooltip.h"

void ARPGHUDBase::SetCurrentlyFocusedWidget(URPGFocusWidget* WidgetToGiveFocus)
{
	if (CurrentlyFocusedWidget != WidgetToGiveFocus)
	{
		URPGFocusWidget* OldWidget = CurrentlyFocusedWidget;
		CurrentlyFocusedWidget = WidgetToGiveFocus;

		if (IsValid(OldWidget))
		{
			OldWidget->ClearRPGFocus(WidgetToGiveFocus);
		}
		WidgetToGiveFocus->SetRPGFocus(OldWidget);
	}
}

void ARPGHUDBase::ResetRPGFocus()
{
	if (IsValid(HUDWidget))
	{
		SetCurrentlyFocusedWidget(HUDWidget);
	}
}

void ARPGHUDBase::ShowHUDWidget()
{
	if (!IsValid(HUDWidget) && HUDWidgetClass)
	{
		HUDWidget = CreateWidget<URPGHUDWidget>(PlayerOwner.Get(), HUDWidgetClass);
	}
	if (IsValid(HUDWidget))
	{
		HUDWidget->AddToViewport();
	}
}

void ARPGHUDBase::HideHUDWidget()
{
	if (IsValid(HUDWidget))
	{
		HUDWidget->RemoveFromParent();
	}
}

void ARPGHUDBase::AddTooltip(URPGTooltip* Tooltip)
{
	if (IsValid(HUDWidget))
	{
		HUDWidget->TooltipLayer->AddChild(Tooltip);
		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Tooltip->Slot))
		{
			CanvasSlot->SetAutoSize(true);
		}
	}
}

void ARPGHUDBase::BeginPlay()
{
	Super::BeginPlay();
	ShowHUDWidget();
	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(GetOwningPlayerController(), nullptr, EMouseLockMode::DoNotLock, false);
	if (!IsValid(Cursor) && CursorWidgetClass)
	{
		Cursor = CreateWidget<URPGCursor>(PlayerOwner.Get(), CursorWidgetClass);
		PlayerOwner->SetMouseCursorWidget(EMouseCursor::Default, Cursor);
		PlayerOwner->SetMouseCursorWidget(EMouseCursor::ResizeLeftRight, Cursor);
		PlayerOwner->SetMouseCursorWidget(EMouseCursor::GrabHandClosed, Cursor);
	}
}
