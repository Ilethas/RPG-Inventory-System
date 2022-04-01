#include "RPGWidgetComponent.h"

#include "RPGWidgetWithComponents.h"

URPGWidgetWithComponents* URPGWidgetComponent::GetOwnerWidget() const
{
	return GetTypedOuter<URPGWidgetWithComponents>();
}

void URPGWidgetComponent::Construct()
{
	if (!bConstructed)
	{
		OnConstructedNative();
		bConstructed = true;
	}
}

void URPGWidgetComponent::Initialize()
{
	if (!bInitialized)
	{
		OnInitializedNative();
		bInitialized = true;
	}
}

void URPGWidgetComponent::OnInitializedNative()
{
	OnInitialized();
}

void URPGWidgetComponent::OnConstructedNative()
{
	OnConstructed();
}

void URPGWidgetComponent::OnRemovedNative()
{
	OnRemoved();
}

void URPGWidgetComponent::TickNative(const FGeometry& MyGeometry, float InDeltaTime)
{
	Tick(MyGeometry, InDeltaTime);
}

FReply URPGWidgetComponent::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return OnMouseButtonDown(InGeometry, InMouseEvent).NativeReply;
}

FReply URPGWidgetComponent::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return OnPreviewMouseButtonDown(InGeometry, InMouseEvent).NativeReply;
}

FReply URPGWidgetComponent::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return OnMouseButtonUp(InGeometry, InMouseEvent).NativeReply;
}

FReply URPGWidgetComponent::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return OnMouseMove(InGeometry, InMouseEvent).NativeReply;
}

void URPGWidgetComponent::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	OnMouseEnter(InGeometry, InMouseEvent);
}

void URPGWidgetComponent::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	OnMouseLeave(InMouseEvent);
}

void URPGWidgetComponent::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& InOutOperation)
{
	OnDragDetected(InGeometry, InMouseEvent, InOutOperation, InOutOperation);
}

void URPGWidgetComponent::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	OnDragEnter(InGeometry, InDragDropEvent, InOperation);
}

void URPGWidgetComponent::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	OnDragLeave(InDragDropEvent, InOperation);
}

bool URPGWidgetComponent::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	return OnDragOver(InGeometry, InDragDropEvent, InOperation);
}

bool URPGWidgetComponent::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	return OnDrop(InGeometry, InDragDropEvent, InOperation);
}

void URPGWidgetComponent::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	OnDragCancelled(InDragDropEvent, InOperation);
}
