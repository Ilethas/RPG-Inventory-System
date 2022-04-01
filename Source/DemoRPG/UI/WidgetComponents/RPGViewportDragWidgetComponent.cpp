#include "RPGViewportDragWidgetComponent.h"

#include "RPGWidgetWithComponents.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Components/PanelWidget.h"
#include "DemoRPG/RPGFunctionLibrary.h"

FReply URPGViewportDragWidgetComponent::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (const UUserWidget* Owner = GetOwnerWidget())
	{
		DragOffset = Owner->GetCachedGeometry().GetAbsolutePosition() - InMouseEvent.GetScreenSpacePosition();
		bIsDragged = true;
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply URPGViewportDragWidgetComponent::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	DragOffset = FVector2D::ZeroVector;
	bIsDragged = false;
	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

FReply URPGViewportDragWidgetComponent::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
}

void URPGViewportDragWidgetComponent::TickNative(const FGeometry& MyGeometry, float InDeltaTime)
{
	if (bIsDragged)
	{
		UUserWidget* Owner = GetOwnerWidget();
		if (IsValid(Owner))
		{
			const TSet<FKey> PressedMouseButtons = FSlateApplication::Get().GetPressedMouseButtons();
			if (!PressedMouseButtons.Contains(EKeys::LeftMouseButton))
			{
				bIsDragged = false;
				return;
			}

			const FVector2D CursorPosition = FSlateApplication::Get().GetCursorPos();
			URPGFunctionLibrary::SetWidgetLayerPosition(Owner, CursorPosition + DragOffset);
		}
	}
	Super::TickNative(MyGeometry, InDeltaTime);
}
