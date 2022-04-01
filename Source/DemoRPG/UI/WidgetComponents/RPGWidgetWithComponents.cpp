#include "RPGWidgetWithComponents.h"

#include "Components/WidgetComponent.h"
#include "RPGWidgetComponent.h"

void URPGWidgetWithComponents::AddWidgetComponent(URPGWidgetComponent* Component)
{
	WidgetComponents.Add(Component);
	Component->Construct();
	Component->Initialize();
}

void URPGWidgetWithComponents::RemoveWidgetComponent(URPGWidgetComponent* Component)
{
	const int RemovedItemsCount = WidgetComponents.Remove(Component);
	if (RemovedItemsCount > 0)
	{
		Component->OnRemovedNative();
	}
}

URPGWidgetComponent* URPGWidgetWithComponents::GetComponent_BP(const TSubclassOf<URPGWidgetComponent> ComponentClass)
{
	for (URPGWidgetComponent* WidgetComponent : WidgetComponents)
	{
		if (WidgetComponent->GetClass()->IsChildOf(ComponentClass.Get()))
		{
			return WidgetComponent;
		}
	}
	return nullptr;
}

void URPGWidgetWithComponents::PostInitProperties()
{
	Super::PostInitProperties();
	WidgetComponents.Remove(nullptr);
}

void URPGWidgetWithComponents::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	for (URPGWidgetComponent* Component : WidgetComponents)
	{
		if (IsValid(Component))
		{
			Component->Initialize();
		}
	}
}

void URPGWidgetWithComponents::NativeConstruct()
{
	Super::NativeConstruct();
	for (URPGWidgetComponent* Component : WidgetComponents)
	{
		if (IsValid(Component))
		{
			Component->Construct();
		}
	}
}

void URPGWidgetWithComponents::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	for (URPGWidgetComponent* Component : WidgetComponents)
	{
		if (IsValid(Component))
		{
			Component->TickNative(MyGeometry, InDeltaTime);
		}
	}
}

FReply URPGWidgetWithComponents::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	bool bHandled = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent).IsEventHandled();
	for (URPGWidgetComponent* Component : WidgetComponents)
	{
		if (IsValid(Component))
		{
			bHandled |= Component->NativeOnMouseButtonDown(InGeometry, InMouseEvent).IsEventHandled();
		}
	}
	return bHandled ? FReply::Handled() : FReply::Unhandled();
}

FReply URPGWidgetWithComponents::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	bool bHandled = Super::NativeOnPreviewMouseButtonDown(InGeometry, InMouseEvent).IsEventHandled();
	for (URPGWidgetComponent* Component : WidgetComponents)
	{
		if (IsValid(Component))
		{
			bHandled |= Component->NativeOnPreviewMouseButtonDown(InGeometry, InMouseEvent).IsEventHandled();
		}
	}
	return bHandled ? FReply::Handled() : FReply::Unhandled();
}

FReply URPGWidgetWithComponents::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	bool bHandled = Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent).IsEventHandled();
	for (URPGWidgetComponent* Component : WidgetComponents)
	{
		if (IsValid(Component))
		{
			bHandled |= Component->NativeOnMouseButtonUp(InGeometry, InMouseEvent).IsEventHandled();
		}
	}
	return bHandled ? FReply::Handled() : FReply::Unhandled();
}

FReply URPGWidgetWithComponents::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	bool bHandled = Super::NativeOnMouseMove(InGeometry, InMouseEvent).IsEventHandled();
	for (URPGWidgetComponent* Component : WidgetComponents)
	{
		if (IsValid(Component))
		{
			bHandled |= Component->NativeOnMouseMove(InGeometry, InMouseEvent).IsEventHandled();
		}
	}
	return bHandled ? FReply::Handled() : FReply::Unhandled();
}

void URPGWidgetWithComponents::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	for (URPGWidgetComponent* Component : WidgetComponents)
	{
		if (IsValid(Component))
		{
			Component->NativeOnMouseEnter(InGeometry, InMouseEvent);
		}
	}
}

void URPGWidgetWithComponents::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	for (URPGWidgetComponent* Component : WidgetComponents)
	{
		if (IsValid(Component))
		{
			Component->NativeOnMouseLeave(InMouseEvent);
		}
	}
}

void URPGWidgetWithComponents::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	for (URPGWidgetComponent* Component : WidgetComponents)
	{
		if (IsValid(Component))
		{
			Component->NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
		}
	}
}

bool URPGWidgetWithComponents::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	bool bHandled = Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);
	for (URPGWidgetComponent* Component : WidgetComponents)
	{
		if (IsValid(Component))
		{
			bHandled |= Component->NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);
		}
	}
	return bHandled;
}

bool URPGWidgetWithComponents::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	bool bHandled = Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	for (URPGWidgetComponent* Component : WidgetComponents)
	{
		if (IsValid(Component))
		{
			bHandled |= Component->NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
		}
	}
	return bHandled;
}

void URPGWidgetWithComponents::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);
	for (URPGWidgetComponent* Component : WidgetComponents)
	{
		if (IsValid(Component))
		{
			Component->NativeOnDragCancelled(InDragDropEvent, InOperation);
		}
	}
}

void URPGWidgetWithComponents::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);
	for (URPGWidgetComponent* Component : WidgetComponents)
	{
		if (IsValid(Component))
		{
			Component->NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);
		}
	}
}

void URPGWidgetWithComponents::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);
	for (URPGWidgetComponent* Component : WidgetComponents)
	{
		if (IsValid(Component))
		{
			Component->NativeOnDragLeave(InDragDropEvent, InOperation);
		}
	}
}
