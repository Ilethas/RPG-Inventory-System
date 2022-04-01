#include "RPGEquippedItemWidget.h"

#include "RPGContextMenuWidget.h"
#include "RPGInventoryWidget.h"
#include "RPGItemDragWidget.h"
#include "RPGItemMapping.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/VerticalBox.h"
#include "DemoRPG/RPGFunctionLibrary.h"
#include "DemoRPG/RPGPlayerController.h"
#include "DemoRPG/Characters/RPGCharacterInstance.h"
#include "DemoRPG/Framework/RPGHUD.h"
#include "DemoRPG/Items/RPGItem.h"
#include "DemoRPG/Items/RPGItemInstance.h"
#include "DemoRPG/Items/Features/RPGEquippableFeature.h"
#include "WidgetComponents/RPGTooltipWidgetComponent.h"

URPGEquippedItemWidget::URPGEquippedItemWidget(const FObjectInitializer& ObjectInitializer)
	: URPGWidgetWithComponents(ObjectInitializer)
{
	TooltipComponent = CreateDefaultSubobject<URPGTooltipWidgetComponent>(TEXT("TooltipComponent"));
}

void URPGEquippedItemWidget::SetEquipmentMapping(URPGEquipmentMapping* NewEquipmentMapping)
{
	ARPGItemInstance* MappingItem = nullptr;
	if (IsValid(EquipmentMapping))
	{
		EquipmentMapping->OnUIMappingChangedNative.RemoveAll(this);
	}
	EquipmentMapping = NewEquipmentMapping;
	if (IsValid(NewEquipmentMapping))
	{
		MappingItem = NewEquipmentMapping->GetItemInstance();
		EquipmentMapping->OnUIMappingChangedNative.AddUObject(this, &URPGEquippedItemWidget::OnEquipmentMappingChanged);
	}
	SetItemInstance(MappingItem);
	RefreshWidget();
}

void URPGEquippedItemWidget::SetItemInstance(ARPGItemInstance* NewItemInstance)
{
	if (IsValid(ItemInstance))
	{
		ItemInstance->OnItemInstanceChangedNative.RemoveAll(this);
		ItemInstance->OnItemInstanceChanged.RemoveAll(this);
	}
	ItemInstance = NewItemInstance;
	if (IsValid(NewItemInstance))
	{
		NewItemInstance->OnItemInstanceChangedNative.AddUObject(this, &URPGEquippedItemWidget::RefreshWidget);
	}
}

void URPGEquippedItemWidget::PostInitProperties()
{
	Super::PostInitProperties();
	WidgetComponents.Add(TooltipComponent);
}

FReply URPGEquippedItemWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	FEventReply DetectDragIfPressed = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
	return DetectDragIfPressed.NativeReply;
}

void URPGEquippedItemWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	if (!(ItemDragWidgetClass && IsValid(EquipmentMapping)))
	{
		return;
	}
	
	URPGItemDragWidget* DragWidget = CreateWidget<URPGItemDragWidget>(this, ItemDragWidgetClass);
	DragWidget->SetSize(URPGFunctionLibrary::GetWidgetScreenSize(this, this));
	DragWidget->SetItemMapping(EquipmentMapping);
	
	OutOperation = NewObject<UDragDropOperation>(this);
	OutOperation->DefaultDragVisual = DragWidget;
	OutOperation->Pivot = EDragPivot::CenterCenter;
	OutOperation->Payload = EquipmentMapping;
	
	ARPGHUD* HUD = GetOwningPlayer()->GetHUD<ARPGHUD>();
	if (IsValid(HUD))
	{
		HUD->HideCursorOnDrag(OutOperation);
		OutOperation->OnDrop.AddDynamic(HUD, &ARPGHUD::ShowCursorOnDrop);
		OutOperation->OnDragCancelled.AddDynamic(HUD, &ARPGHUD::ShowCursorOnDrop);
	}
}

bool URPGEquippedItemWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	URPGInventoryMapping* InventoryMappingPayload = Cast<URPGInventoryMapping>(InOperation->Payload);
	if (IsValid(InventoryMappingPayload))
	{
		check(InventoryMappingPayload->GetItemInstance()->GetItem());
		const URPGEquippableFeature* EquippableFeature = InventoryMappingPayload->GetItemInstance()->GetItem()->GetFeature<URPGEquippableFeature>();
		if (IsValid(EquippableFeature) && EquippableFeature->ItemSlots.Contains(WidgetItemSlot))
		{
			ARPGPlayerController* Controller = GetOwningPlayer<ARPGPlayerController>();
			ARPGCharacterInstance* SelectedCharacter = Controller->GetSelectedCharacter();
			
			Controller->EquipItem(SelectedCharacter, InventoryMappingPayload->GetItemInstance(), WidgetItemSlot);
		}
	}
	
	// URPGEquipmentMapping* EquipmentMappingPayload = Cast<URPGEquipmentMapping>(InOperation->Payload);
	// if (IsValid(EquipmentMappingPayload))
	// {
	// 	check(EquipmentMappingPayload->GetItemInstance()->GetItem());
	// 	const URPGEquippableFeature* EquippableFeature = EquipmentMappingPayload->GetItemInstance()->GetItem()->GetFeature<URPGEquippableFeature>();
	// 	if (IsValid(EquippableFeature) && EquippableFeature->ItemSlots.Contains(WidgetItemSlot))
	// 	{
	// 		ARPGPlayerController* Controller = GetOwningPlayer<ARPGPlayerController>();
	// 		ARPGCharacterInstance* SelectedCharacter = Controller->GetSelectedCharacter();
	//
	// 		Controller->EquipItem(SelectedCharacter, EquipmentMappingPayload->GetItemInstance(), WidgetItemSlot);
	// 	}
	// }
	return true;
}

FReply URPGEquippedItemWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
	ARPGCharacterInstance* SelectedCharacter = GetOwningPlayer<ARPGPlayerController>()->GetSelectedCharacter();
	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton && ContextMenuWidgetClass && IsValid(EquipmentMapping) &&
		IsValid(EquipmentMapping->GetItemInstance()) && IsValid(SelectedCharacter))
	{
		URPGContextMenuWidget* ContextMenu = URPGContextMenuWidget::CreateEquippedItemContextMenu(
			this,
			ContextMenuWidgetClass,
			ContextMenuEntryWidgetClass,
			SelectedCharacter,
			EquipmentMapping->GetItemInstance(),
			SelectedCharacter->InventoryMappingContainer);
		if (IsValid(ContextMenu->Container) && ContextMenu->Container->GetChildrenCount() <= 0)
		{
			return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
		}
		else
		{
			const FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(this) * UWidgetLayoutLibrary::GetViewportScale(this);
			ContextMenu->AddToViewport();
			ContextMenu->SetPositionInViewport(MousePosition, true);
			ARPGHUD* HUD = Cast<ARPGHUD>(GetOwningPlayer()->GetHUD());
			if (IsValid(HUD))
			{
				HUD->SetCurrentlyFocusedWidget(ContextMenu);
			}
		}
	}
	return FReply::Handled();
}

FReply URPGEquippedItemWidget::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDoubleClick(InGeometry, InMouseEvent);
	ARPGPlayerController* Controller = GetOwningPlayer<ARPGPlayerController>();
	if (IsValid(EquipmentMapping))
	{
		ARPGCharacterInstance* CharacterInstance = Controller->GetSelectedCharacter();	
		URPGInventory* Inventory = CharacterInstance->InventoryComponent;
		URPGInventoryMappingContainer* UIContainer = CharacterInstance->InventoryMappingContainer;
		
		Controller->UnequipItem(CharacterInstance, EquipmentMapping->GetItemInstance(), Inventory, UIContainer, { -1, -1 });
	}
	return FReply::Handled();
}

void URPGEquippedItemWidget::OnEquipmentMappingChanged()
{
	ARPGItemInstance* MappingItem = EquipmentMapping->GetItemInstance();
	if (ItemInstance != MappingItem)
	{
		SetItemInstance(MappingItem);
		RefreshWidget();
	}
}
