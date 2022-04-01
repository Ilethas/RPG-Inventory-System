#include "RPGBagWidget.h"

#include "RPGInventoryWidget.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "DemoRPG/Components/RPGInventoryMappingContainer.h"
#include "DemoRPG/Items/RPGItemInstance.h"
#include "DemoRPG/Items/Components/RPGInventory.h"
#include "WidgetComponents/RPGViewportDragWidgetComponent.h"

URPGBagWidget::URPGBagWidget(const FObjectInitializer& ObjectInitializer)
	: URPGWidgetWithComponents(ObjectInitializer)
{
	DragComponent = CreateDefaultSubobject<URPGViewportDragWidgetComponent>(TEXT("DragComponent"));
}

void URPGBagWidget::SetBagItemInstance(ARPGItemInstance* NewBagItemInstance)
{
	URPGInventory* BagInventory = nullptr;
	URPGInventoryMappingContainer* BagMappingContainer = nullptr;
	
	if (IsValid(BagItemInstance))
	{
		BagItemInstance->OnItemInstanceChangedNative.RemoveAll(this);
		BagItemInstance->OnItemInstanceChanged.RemoveAll(this);
	}
	BagItemInstance = NewBagItemInstance;
	if (IsValid(NewBagItemInstance))
	{
		NewBagItemInstance->OnItemInstanceChanged.AddDynamic(this, &URPGBagWidget::RefreshWidget);
		BagInventory = NewBagItemInstance->FindComponentByClass<URPGInventory>();
		BagMappingContainer = NewBagItemInstance->FindComponentByClass<URPGInventoryMappingContainer>();
	}
	if (IsValid(BagInventory) && IsValid(BagMappingContainer) && IsValid(InventoryWidget))
	{
		InventoryWidget->SetInventory(BagInventory);
		InventoryWidget->SetInventoryMappingContainer(BagMappingContainer);
	}
	else
	{
		InventoryWidget->SetInventory(nullptr);
		InventoryWidget->SetInventoryMappingContainer(nullptr);
	}
	RefreshWidget();
}

void URPGBagWidget::PostInitProperties()
{
	Super::PostInitProperties();
	WidgetComponents.Add(DragComponent);
}

FReply URPGBagWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	return FReply::Handled();
}

FReply URPGBagWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
	return FReply::Handled();
}

FReply URPGBagWidget::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseWheel(InGeometry, InMouseEvent);
	return FReply::Handled();
}
