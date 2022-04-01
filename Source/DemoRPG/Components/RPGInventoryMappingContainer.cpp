#include "RPGInventoryMappingContainer.h"

#include "DemoRPG/RPGTypes.h"
#include "DemoRPG/UI/RPGItemMapping.h"
#include "Net/UnrealNetwork.h"

void URPGInventoryMappingContainer::SetInventorySlotsPerRow(const int NewInventorySlotsPerRow)
{
	InventorySlotsPerRow = NewInventorySlotsPerRow;
}

void URPGInventoryMappingContainer::OnItemAdded(ARPGItemInstance* AddedItem)
{
	const FRPGUISlot NewItemSlot = URPGInventoryMapping::GetFirstUnoccupiedSlot(this, InventorySlotsPerRow);
	URPGInventoryMapping::Construct(AddedItem, this, NewItemSlot);
}

void URPGInventoryMappingContainer::OnItemRemoved(ARPGItemInstance* RemovedItem)
{
	RemoveUIMappingsByPredicate([&](URPGUIMapping* Mapping)
	{
		const URPGInventoryMapping* InventoryMapping = Cast<URPGInventoryMapping>(Mapping);
		return IsValid(InventoryMapping) && InventoryMapping->GetItemInstance() == RemovedItem;
	});
}

void URPGInventoryMappingContainer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(URPGInventoryMappingContainer, InventorySlotsPerRow);
}
