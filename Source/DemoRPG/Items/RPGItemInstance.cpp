#include "RPGItemInstance.h"

#include "RPGItem.h"
#include "Features/RPGItemFeature.h"
#include "Net/UnrealNetwork.h"

int ARPGItemInstance::ARPGItemInstance_Index = 0;

ARPGItemInstance::ARPGItemInstance()
{
	bReplicates = true;
	bAlwaysRelevant = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

void ARPGItemInstance::SetItem(URPGItem* NewItem)
{
	if (HasAuthority())
	{
		Item = NewItem;
		OnRep_Item();
	}
}

void ARPGItemInstance::SetQuantity(const int NewQuantity)
{
	if (HasAuthority())
	{
		Quantity = NewQuantity;
		OnRep_ItemInstance();
	}
}

void ARPGItemInstance::SetItemOwner(UObject* NewItemOwner)
{
	ItemOwner = NewItemOwner;
}

void ARPGItemInstance::OnRep_ItemInstance()
{
	OnItemInstanceChangedNative.Broadcast();
	OnItemInstanceChanged.Broadcast();
}

ARPGItemInstance* ARPGItemInstance::Construct(UObject* NewItemOwner, URPGItem* ItemType, const int ItemQuantity)
{
	ARPGItemInstance* ItemInstance = nullptr;
	UWorld* const World = GEngine->GetWorldFromContextObject(NewItemOwner, EGetWorldErrorMode::LogAndReturnNull);
	if (IsValid(World))
	{
		FActorSpawnParameters Parameters;
		Parameters.bNoFail = true;
		ItemInstance = World->SpawnActor<ARPGItemInstance>(Parameters);
		
#if WITH_EDITOR
		ItemInstance->SetActorLabel(FString::Format(TEXT("{0}_{1}"), { ItemType->Name.ToString(), ARPGItemInstance_Index++ }));
#endif
		
		if (AActor* ActorOwner = Cast<AActor>(NewItemOwner))
		{
			ItemInstance->AttachToActor(ActorOwner, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		}
		else if (const UActorComponent* ComponentOwner = Cast<UActorComponent>(NewItemOwner))
		{
			ItemInstance->AttachToActor(ComponentOwner->GetOwner(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		}
		ItemInstance->Quantity = ItemQuantity;
		ItemInstance->SetItem(ItemType);
		ItemInstance->SetItemOwner(NewItemOwner);
	}
	return ItemInstance;
}

void ARPGItemInstance::OnSubobjectCreatedFromReplication(UObject* NewSubobject)
{
	Super::OnSubobjectCreatedFromReplication(NewSubobject);
	if (UActorComponent* Component = Cast<UActorComponent>(NewSubobject))
	{
		AddInstanceComponent(Component);
	}
}

void ARPGItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ARPGItemInstance, ItemOwner);
	DOREPLIFETIME(ARPGItemInstance, Item);
	DOREPLIFETIME(ARPGItemInstance, Quantity);
}

void ARPGItemInstance::OnRep_Item()
{
	if (HasAuthority())
	{
		// Remove all old item components, since instance's item type has changed
		TInlineComponentArray<UActorComponent*> Components(this);
		GetComponents(Components);
		for (UActorComponent* Component : Components)
		{
			if (Component != RootComponent)
			{
				Component->DestroyComponent();
			}
		}
	
		// Notify the new item type's components about the instance 
		if (IsValid(Item))
		{
			Item->ForEachItemFeature([&](URPGItemFeature* Feature)
			{
				if (IsValid(Feature))
				{
					Feature->OnItemInstanceConstructed(this);
				}
				return false;
			});
		}
	}
	OnRep_ItemInstance();
}

void ARPGItemInstance::BeginPlay()
{
	Super::BeginPlay();
	if (!HasAuthority())
	{
#if WITH_EDITOR
		SetActorLabel(FString::Format(TEXT("{0}_{1}"), { Item->Name.ToString(), ARPGItemInstance_Index++ }));
#endif
	}
}
