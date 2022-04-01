// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGContextMenuWidget.h"

#include "Components/VerticalBox.h"
#include "DemoRPG/RPGFunctionLibrary.h"
#include "DemoRPG/Framework/RPGHUD.h"

void URPGContextMenuWidget::Close()
{
	RemoveFromViewport();
}

void URPGContextMenuWidget::ReceivedRPGFocus(URPGFocusWidget* PreviouslyFocusedWidget)
{
	if (IsValid(Container) && Container->GetChildrenCount() > 0)
	{
		URPGFocusWidget* FirstMenuEntry = Cast<URPGFocusWidget>(Container->GetChildAt(0));
		if (IsValid(FirstMenuEntry))
		{
			ARPGHUD* HUD = Cast<ARPGHUD>(GetOwningPlayer()->GetHUD());
			if (IsValid(HUD))
			{
				HUD->SetCurrentlyFocusedWidget(FirstMenuEntry);
			}
		}
	}
	Super::ReceivedRPGFocus(PreviouslyFocusedWidget);
}

void URPGContextMenuWidget::DescendantLostRPGFocus(URPGFocusWidget* Descendant, const bool bIsDirectRPGFocusableChild, URPGFocusWidget* WidgetThatReceivedFocus)
{
	if (!URPGFunctionLibrary::IsChild(WidgetThatReceivedFocus, this))
	{
		Close();
	}
	Super::DescendantLostRPGFocus(Descendant, bIsDirectRPGFocusableChild, WidgetThatReceivedFocus);
}
