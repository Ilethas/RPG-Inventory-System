#include "RPGCreateBagFeature.h"

#include "DemoRPG/Components/RPGInventoryMappingContainer.h"
#include "DemoRPG/Items/RPGItemInstance.h"
#include "DemoRPG/Items/Components/RPGInventory.h"
#include "Engine/ActorChannel.h"

void URPGCreateBagFeature::OnItemInstanceConstructed(ARPGItemInstance* ItemInstance)
{
	Super::OnItemInstanceConstructed(ItemInstance);
	if (ItemInstance->HasAuthority())
	{
		URPGInventory* BagInventory = NewObject<URPGInventory>(ItemInstance);
		BagInventory->RegisterComponent();
		ItemInstance->AddInstanceComponent(BagInventory);
		
		URPGInventoryMappingContainer* MappingContainer = NewObject<URPGInventoryMappingContainer>(ItemInstance);
		MappingContainer->SetInventorySlotsPerRow(5);
		MappingContainer->RegisterComponent();
		ItemInstance->AddInstanceComponent(MappingContainer);

		BagInventory->OnInventoryItemAddedNative.AddUObject(MappingContainer, &URPGInventoryMappingContainer::OnItemAdded);
		BagInventory->OnInventoryItemRemovedNative.AddUObject(MappingContainer, &URPGInventoryMappingContainer::OnItemRemoved);
	}
}
