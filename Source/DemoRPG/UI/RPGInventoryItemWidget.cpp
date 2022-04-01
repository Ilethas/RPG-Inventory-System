// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGInventoryItemWidget.h"

#include "RPGContextMenuWidget.h"
#include "RPGItemDragWidget.h"
#include "RPGInventoryWidget.h"
#include "RPGItemMapping.h"
#include "Blueprint/DragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/VerticalBox.h"
#include "DemoRPG/RPGFunctionLibrary.h"
#include "DemoRPG/RPGPlayerController.h"
#include "DemoRPG/Characters/RPGCharacterInstance.h"
#include "DemoRPG/Components/RPGInventoryMappingContainer.h"
#include "DemoRPG/Framework/RPGHUD.h"
#include "DemoRPG/Items/RPGItem.h"
#include "DemoRPG/Items/RPGItemInstance.h"
#include "DemoRPG/Items/Features/RPGEquippableFeature.h"
#include "WidgetComponents/RPGTooltipWidgetComponent.h"

URPGInventoryItemWidget::URPGInventoryItemWidget(const FObjectInitializer& ObjectInitializer)
	: URPGWidgetWithComponents(ObjectInitializer)
{
	TooltipComponent = CreateDefaultSubobject<URPGTooltipWidgetComponent>(TEXT("TooltipComponent"));
}

void URPGInventoryItemWidget::SetInventoryMapping(URPGInventoryMapping* NewInventoryMapping)
{
	ARPGItemInstance* MappingItem = nullptr;
	if (IsValid(InventoryMapping))
	{
		InventoryMapping->OnUIMappingChangedNative.RemoveAll(this);
	}
	InventoryMapping = NewInventoryMapping;
	if (IsValid(NewInventoryMapping))
	{
		MappingItem = NewInventoryMapping->GetItemInstance();
		InventoryMapping->OnUIMappingChangedNative.AddUObject(this, &URPGInventoryItemWidget::OnInventoryMappingChanged);
	}
	SetItemInstance(MappingItem);
	RefreshWidget();
}

void URPGInventoryItemWidget::SetItemInstance(ARPGItemInstance* NewItemInstance)
{
	if (IsValid(ItemInstance))
	{
		ItemInstance->OnItemInstanceChangedNative.RemoveAll(this);
		ItemInstance->OnItemInstanceChanged.RemoveAll(this);
	}
	ItemInstance = NewItemInstance;
	if (IsValid(NewItemInstance))
	{
		NewItemInstance->OnItemInstanceChangedNative.AddUObject(this, &URPGInventoryItemWidget::RefreshWidget);
	}
}

void URPGInventoryItemWidget::PostInitProperties()
{
	Super::PostInitProperties();
	WidgetComponents.Add(TooltipComponent);
}

void URPGInventoryItemWidget::OnInventoryMappingChanged()
{
	ARPGItemInstance* MappingItem = InventoryMapping->GetItemInstance();
	if (ItemInstance != MappingItem)
	{
		SetItemInstance(MappingItem);
		RefreshWidget();
	}
}

void URPGInventoryItemWidget::HandleOnDrop(URPGInventoryMapping* Payload)
{
	ARPGPlayerController* Controller = Cast<ARPGPlayerController>(GetOwningPlayer());
	if (IsValid(Controller) && IsValid(OwnerInventoryWidget))
	{
		// Shift-drop item should trigger item splitting
		const FModifierKeysState ModifierKeys = FSlateApplication::Get().GetModifierKeys();
		if (ModifierKeys.IsLeftShiftDown() || ModifierKeys.IsRightShiftDown())
		{
			ARPGHUD* HUD = Cast<ARPGHUD>(Controller->GetHUD());
			if (IsValid(HUD))
			{
				HUD->ShowSplitItemWidget(Payload->GetItemInstance(), OwnerInventoryWidget->GetInventory(), OwnerInventoryWidget->GetInventoryMappingContainer(), UISlot);
			}
		}
		else
		{
			if (IsValid(InventoryMapping) && IsValid(InventoryMapping->GetItemInstance()) && IsValid(Payload->GetItemInstance()->GetItem()))
			{
				const bool bIsItemStackFull = InventoryMapping->GetItemInstance()->GetQuantity() >= InventoryMapping->GetItemInstance()->GetItem()->MaxStackSize;
				const bool bIsTheSameKind = Payload->GetItemInstance()->GetItem() == InventoryMapping->GetItemInstance()->GetItem();
				const bool bIsTheSameInstance = Payload->GetItemInstance() == InventoryMapping->GetItemInstance();

				if (!bIsTheSameInstance)
				{
					if (bIsTheSameKind && !bIsItemStackFull)
					{
						Controller->CombineItemStacks(Payload->GetItemInstance(), InventoryMapping->GetItemInstance());
					}
					else
					{
						URPGInventoryMappingContainer* Container = Cast<URPGInventoryMappingContainer>(Payload->GetOwnerContainer());
						if (IsValid(Container))
						{
							Controller->MoveItem(InventoryMapping->GetItemInstance(), true, Payload->GetItemInstance()->GetTypedItemOwner<URPGInventory>(), Container, Payload->GetUISlot());
							Controller->MoveItem(Payload->GetItemInstance(), true, OwnerInventoryWidget->GetInventory(), OwnerInventoryWidget->GetInventoryMappingContainer(), UISlot);
						}
					}
				}
			}
			else
			{
				if (IsValid(Payload->GetItemInstance()))
				{
					Controller->MoveItem(Payload->GetItemInstance(), true, OwnerInventoryWidget->GetInventory(), OwnerInventoryWidget->GetInventoryMappingContainer(), UISlot);
				}
			}
		}
	}
}

void URPGInventoryItemWidget::HandleOnDrop(URPGEquipmentMapping* Payload)
{
	ARPGPlayerController* Controller = Cast<ARPGPlayerController>(GetOwningPlayer());
	if (IsValid(Controller))
	{
		ARPGCharacterInstance* CharacterInstance = Controller->GetSelectedCharacter();
		URPGInventory* Inventory = OwnerInventoryWidget->GetInventory();
		URPGInventoryMappingContainer* UIContainer = OwnerInventoryWidget->GetInventoryMappingContainer();
		
		Controller->UnequipItem(CharacterInstance, Payload->GetItemInstance(), Inventory, UIContainer, UISlot);
	}
}

FReply URPGInventoryItemWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	FEventReply DetectDragIfPressed = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
	return DetectDragIfPressed.NativeReply;
}

void URPGInventoryItemWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	if (!(ItemDragWidgetClass && IsValid(InventoryMapping)))
	{
		return;
	}
	
	URPGItemDragWidget* DragWidget = CreateWidget<URPGItemDragWidget>(this, ItemDragWidgetClass);
	DragWidget->SetSize(URPGFunctionLibrary::GetWidgetScreenSize(this, this));
	DragWidget->SetItemMapping(InventoryMapping);
	
	OutOperation = NewObject<UDragDropOperation>(this);
	OutOperation->DefaultDragVisual = DragWidget;
	OutOperation->Pivot = EDragPivot::CenterCenter;
	OutOperation->Payload = InventoryMapping;

	ARPGHUD* HUD = Cast<ARPGHUD>(GetOwningPlayer()->GetHUD());
	if (IsValid(HUD))
	{
		HUD->HideCursorOnDrag(OutOperation);
		OutOperation->OnDrop.AddDynamic(HUD, &ARPGHUD::ShowCursorOnDrop);
		OutOperation->OnDragCancelled.AddDynamic(HUD, &ARPGHUD::ShowCursorOnDrop);
	}
}

bool URPGInventoryItemWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	URPGInventoryMapping* InventoryMappingPayload = Cast<URPGInventoryMapping>(InOperation->Payload);
	if (IsValid(InventoryMappingPayload))
	{
		HandleOnDrop(InventoryMappingPayload);
		return true;
	}
	URPGEquipmentMapping* EquipmentMappingPayload = Cast<URPGEquipmentMapping>(InOperation->Payload);
	if (IsValid(EquipmentMappingPayload))
	{
		HandleOnDrop(EquipmentMappingPayload);
		return true;
	}
	return true;
}

FReply URPGInventoryItemWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton && ContextMenuWidgetClass && IsValid(InventoryMapping) &&
		IsValid(InventoryMapping->GetItemInstance()) && IsValid(OwnerInventoryWidget))
	{
		URPGContextMenuWidget* ContextMenu = URPGContextMenuWidget::CreateItemContextMenu(
			this,
			ContextMenuWidgetClass,
			ContextMenuEntryWidgetClass,
			GetOwningPlayer<ARPGPlayerController>()->GetSelectedCharacter(),
			InventoryMapping->GetItemInstance(),
			OwnerInventoryWidget->GetInventoryMappingContainer());
		if (IsValid(ContextMenu->Container) && ContextMenu->Container->GetChildrenCount() <= 0)
		{
			return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
		}
		else
		{
			const FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(this) * UWidgetLayoutLibrary::GetViewportScale(this);
			ContextMenu->AddToViewport();
			ContextMenu->SetPositionInViewport(MousePosition, true);
			ARPGHUD* HUD = GetOwningPlayer()->GetHUD<ARPGHUD>();
			if (IsValid(HUD))
			{
				HUD->SetCurrentlyFocusedWidget(ContextMenu);
			}
		}
	}
	return FReply::Handled();
}

FReply URPGInventoryItemWidget::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDoubleClick(InGeometry, InMouseEvent);
	if (IsValid(InventoryMapping) && IsValid(InventoryMapping->GetItemInstance()))
	{
		const URPGInventory* BagComponent = InventoryMapping->GetItemInstance()->FindComponentByClass<URPGInventory>();
		if (IsValid(BagComponent))
		{
			ARPGHUD* HUD = Cast<ARPGHUD>(GetOwningPlayer()->GetHUD());
			if (IsValid(HUD))
			{
				HUD->ShowBagWidget(InventoryMapping->GetItemInstance());
			}
		}

		check(InventoryMapping->GetItemInstance()->GetItem());
		const URPGEquippableFeature* EquippableFeature = InventoryMapping->GetItemInstance()->GetItem()->GetFeature<URPGEquippableFeature>();
		if (IsValid(EquippableFeature))
		{
			ARPGPlayerController* Controller = GetOwningPlayer<ARPGPlayerController>();
			ARPGCharacterInstance* SelectedCharacter = Controller->GetSelectedCharacter();

			const FName EquipmentSlot = SelectedCharacter->FindSlotForItem(ItemInstance);
			Controller->EquipItem(SelectedCharacter, InventoryMapping->GetItemInstance(), EquipmentSlot);
		}
	}
	return FReply::Handled();
}
