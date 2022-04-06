// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGItemMapping.h"

#include "Net/UnrealNetwork.h"

void URPGItemMapping::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(URPGItemMapping, ItemInstance);
}

void URPGItemMapping::OnRep_ItemInstance()
{
	OnRep_UIMapping();
}

void URPGInventoryMapping::OnRep_UISlot(const FRPGUISlot OldUISlot)
{
	OnInventoryMappingUISlotChangedNative.Broadcast(OldUISlot, this);
	OnInventoryMappingUISlotChanged.Broadcast(OldUISlot, this);
	OnRep_UIMapping();
}

void URPGInventoryMapping::SetUISlot(const FRPGUISlot& NewUISlot)
{
	const FRPGUISlot OldUISlot = UISlot;
	UISlot = NewUISlot;
	OnRep_UISlot(OldUISlot);
}

URPGInventoryMapping* URPGInventoryMapping::Construct(ARPGItemInstance* Instance, URPGUIMappingContainer* Container, const FRPGUISlot& InItemSlot)
{
	URPGInventoryMapping* Mapping = NewObject<URPGInventoryMapping>(Container);
	Mapping->ItemInstance = Instance;
	Mapping->UISlot = InItemSlot;
	Container->AddUIMapping(Mapping);
	return Mapping;
}

FRPGUISlot URPGInventoryMapping::GetFirstUnoccupiedSlot_BP(const URPGUIMappingContainer* Container, const int SlotsPerRow, const FRPGUISlot& StartingSlot)
{
	return GetFirstUnoccupiedSlot(Container, SlotsPerRow, StartingSlot);
}

FRPGUISlot URPGInventoryMapping::GetFirstUnoccupiedSlot(const URPGUIMappingContainer* Container, const int SlotsPerRow, const FRPGUISlot& StartingSlot)
{
	return FindSlotForItem(nullptr, Container, SlotsPerRow, StartingSlot);
}

FRPGUISlot URPGInventoryMapping::FindSlotForItem(const ARPGItemInstance* InItemInstance, const URPGUIMappingContainer* Container, const int SlotsPerRow, const FRPGUISlot& StartingSlot)
{
	FRPGUISlot Result = StartingSlot;
	TArray<URPGInventoryMapping*> InventoryMappings = Container->GetUIMappingsTyped<URPGInventoryMapping>();
	InventoryMappings.Sort([](const URPGInventoryMapping& A, const URPGInventoryMapping& B)
	{
		if (A.UISlot.Y == B.UISlot.Y)
		{
			return A.UISlot.X < B.UISlot.X;
		}
		return  A.UISlot.Y < B.UISlot.Y;
	});
	for (const URPGInventoryMapping* Mapping : InventoryMappings)
	{
		if (Result.X == Mapping->UISlot.X && Result.Y == Mapping->UISlot.Y && Mapping->GetItemInstance() != InItemInstance)
		{
			Result.X += 1;
			if (Result.X % SlotsPerRow == 0)
			{
				Result.X = 0;
				Result.Y += 1;
			}
		}
	}
	return Result;
}

URPGInventoryMapping* URPGInventoryMapping::FindMappingForSlot(URPGUIMappingContainer* Container, const FRPGUISlot& TargetUISlot)
{
	URPGInventoryMapping* Result = nullptr;
	Container->ForEachUIMapping([&](URPGUIMapping* UIMapping)
	{
		URPGInventoryMapping* InventoryMapping = Cast<URPGInventoryMapping>(UIMapping);
		if (IsValid(InventoryMapping) && InventoryMapping->UISlot == TargetUISlot)
		{
			Result = InventoryMapping;
			return true;
		}
		return false;
	});
	return Result;
}

void URPGInventoryMapping::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(URPGInventoryMapping, UISlot);
}

void URPGEquipmentMapping::SetItemSlot(const FName NewItemSlot)
{
	const FName OldItemSlot = ItemSlot;
	ItemSlot = NewItemSlot;
	OnRep_ItemSlot(OldItemSlot);
}

URPGEquipmentMapping* URPGEquipmentMapping::Construct(ARPGItemInstance* Instance, URPGUIMappingContainer* Container, const FName InItemSlot)
{
	URPGEquipmentMapping* Mapping = NewObject<URPGEquipmentMapping>(Container);
	Mapping->ItemInstance = Instance;
	Mapping->ItemSlot = InItemSlot;
	Container->AddUIMapping(Mapping);
	return Mapping;
}

void URPGEquipmentMapping::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(URPGEquipmentMapping, ItemSlot);
}

void URPGEquipmentMapping::OnRep_ItemSlot(const FName OldItemSlot)
{
	OnEquipmentMappingItemSlotChangedNative.Broadcast(OldItemSlot, this);
	OnEquipmentMappingItemSlotChanged.Broadcast(OldItemSlot, this);
	OnRep_UIMapping();
}
