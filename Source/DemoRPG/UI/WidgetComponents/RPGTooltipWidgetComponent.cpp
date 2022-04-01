#include "RPGTooltipWidgetComponent.h"

#include "RPGWidgetWithComponents.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Components/PanelWidget.h"
#include "DemoRPG/RPGFunctionLibrary.h"
#include "DemoRPG/Framework/RPGHUDBase.h"
#include "DemoRPG/UI/RPGTooltip.h"

void URPGTooltipWidgetComponent::SetTooltipDelay(const float NewDelay)
{
	TooltipDelay = NewDelay;
	if (IsValid(TooltipWidget))
	{
		TooltipWidget->TooltipDelay = NewDelay;
	}
}

void URPGTooltipWidgetComponent::SetTooltipOffset(const FVector2D& NewTooltipOffset)
{
	TooltipOffset = NewTooltipOffset;
}

void URPGTooltipWidgetComponent::BeginDestroy()
{
	Super::BeginDestroy();
	if (IsValid(TooltipWidget))
	{
		TooltipWidget->RemoveFromParent();
		TooltipWidget = nullptr;
	}
}

void URPGTooltipWidgetComponent::ShowTooltip() const
{
	if (IsValid(TooltipWidget))
	{
		TooltipWidget->ShowTooltip();
	}
}

void URPGTooltipWidgetComponent::HideTooltip() const
{
	if (IsValid(TooltipWidget))
	{
		TooltipWidget->HideTooltip();
	}
}

void URPGTooltipWidgetComponent::OnConstructedNative()
{
	Super::OnConstructedNative();
	URPGWidgetWithComponents* ParentWidget = GetOwnerWidget();
	if (!IsValid(TooltipWidget) && IsValid(ParentWidget) && TooltipWidgetClass)
	{
		TooltipWidget = CreateWidget<URPGTooltip>(ParentWidget, TooltipWidgetClass);
		TooltipWidget->TooltipDelay = TooltipDelay;
		ARPGHUDBase* HUD = Cast<ARPGHUDBase>(GetOwnerWidget()->GetOwningPlayer()->GetHUD());
		if (IsValid(HUD))
		{
			HUD->AddTooltip(TooltipWidget);
		}
		TooltipWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void URPGTooltipWidgetComponent::OnRemovedNative()
{
	Super::OnRemovedNative();
	if (IsValid(TooltipWidget))
	{
		TooltipWidget->RemoveFromParent();
		TooltipWidget = nullptr;
	}
}

FReply URPGTooltipWidgetComponent::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (IsValid(TooltipWidget))
	{
		URPGFunctionLibrary::SetWidgetLayerPosition(TooltipWidget, InMouseEvent.GetScreenSpacePosition() + TooltipOffset);
	}
	return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
}

void URPGTooltipWidgetComponent::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	if (IsValid(TooltipWidget))
	{
		URPGFunctionLibrary::SetWidgetLayerPosition(TooltipWidget, InMouseEvent.GetScreenSpacePosition() + TooltipOffset);
		ShowTooltip();
	}
}

void URPGTooltipWidgetComponent::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	if (IsValid(TooltipWidget))
	{
		HideTooltip();
	}
}
