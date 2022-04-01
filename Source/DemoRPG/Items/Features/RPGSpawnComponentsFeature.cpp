#include "RPGSpawnComponentsFeature.h"

#include "DemoRPG/Components/RPGInventoryMappingContainer.h"
#include "DemoRPG/Items/RPGItemInstance.h"
#include "DemoRPG/Items/Components/RPGInventory.h"
#include "Engine/ActorChannel.h"

void URPGSpawnComponentsFeature::OnItemInstanceConstructed(ARPGItemInstance* ItemInstance)
{
	Super::OnItemInstanceConstructed(ItemInstance);
	if (ItemInstance->HasAuthority())
	{
		for (const TSubclassOf<UActorComponent>& ComponentClass : ComponentsToSpawn)
		{
			if (ComponentClass)
			{
				UActorComponent* Component = NewObject<UActorComponent>(ItemInstance, ComponentClass);
				Component->RegisterComponent();
				ItemInstance->AddInstanceComponent(Component);
			}
		}
	}
}
