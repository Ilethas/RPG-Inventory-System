#include "RPGFocusWidget.h"

#include "Components/PanelWidget.h"
#include "DemoRPG/RPGFunctionLibrary.h"
#include "DemoRPG/Framework/RPGHUD.h"

void URPGFocusWidget::SetRPGFocus(URPGFocusWidget* PreviouslyFocusedWidget)
{
	if (!bHasRPGFocus)
	{
		bHasRPGFocus = true;
		ReceivedRPGFocus(PreviouslyFocusedWidget);
	}
}

void URPGFocusWidget::ClearRPGFocus(URPGFocusWidget* WidgetThatReceivedFocus)
{
	if (bHasRPGFocus)
	{
		bHasRPGFocus = false;
		LostRPGFocus(WidgetThatReceivedFocus);

		URPGFocusWidget* ParentFocusWidget = GetFocusableParent();
		while (IsValid(ParentFocusWidget))
		{
			ParentFocusWidget->DescendantLostRPGFocus(this, GetFocusableParent() == ParentFocusWidget, WidgetThatReceivedFocus);
			ParentFocusWidget = ParentFocusWidget->GetFocusableParent();
		}
	}
}

bool URPGFocusWidget::HasRPGFocus() const
{
	return bHasRPGFocus;
}

void URPGFocusWidget::SetRPGFocusable(const bool bNewFocusableState)
{
	if (bIsRPGFocusable != bNewFocusableState)
	{
		bIsRPGFocusable = bNewFocusableState;
		
	}
}

URPGFocusWidget* URPGFocusWidget::GetFocusableParent() const
{
	UUserWidget* Widget = URPGFunctionLibrary::GetParentUserWidget(this);
	while (Widget)
	{
		URPGFocusWidget* FocusWidget = Cast<URPGFocusWidget>(Widget);
		if (IsValid(FocusWidget) && FocusWidget->IsRPGFocusable())
		{
			return FocusWidget;
		}
		Widget = URPGFunctionLibrary::GetParentUserWidget(Widget);
	}
	return nullptr;
}

void URPGFocusWidget::ReceivedRPGFocus(URPGFocusWidget* PreviouslyFocusedWidget)
{
	ReceivedRPGFocus_BP(PreviouslyFocusedWidget);
}

void URPGFocusWidget::LostRPGFocus(URPGFocusWidget* WidgetThatReceivedFocus)
{
	LostRPGFocus_BP(WidgetThatReceivedFocus);
}

void URPGFocusWidget::DescendantLostRPGFocus(URPGFocusWidget* Descendant, const bool bIsDirectRPGFocusableChild, URPGFocusWidget* WidgetThatReceivedFocus)
{
	DescendantLostRPGFocus_BP(Descendant, bIsDirectRPGFocusableChild, WidgetThatReceivedFocus);
}

FReply URPGFocusWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	ARPGHUD* HUD = Cast<ARPGHUD>(GetOwningPlayer()->GetHUD());
	if (IsValid(HUD))
	{
		HUD->SetCurrentlyFocusedWidget(this);
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}
