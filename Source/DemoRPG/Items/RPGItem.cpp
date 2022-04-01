// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGItem.h"

#include "DemoRPG/RPGAssetManager.h"
#include "Engine/ActorChannel.h"
#include "Features/RPGItemFeature.h"
#include "Net/UnrealNetwork.h"

URPGItemFeature* URPGItem::GetFeature(const TSubclassOf<URPGItemFeature> FeatureClass)
{
	for (URPGItemFeature* Feature : ItemFeatures)
	{
		if (Feature->GetClass()->IsChildOf(FeatureClass.Get()))
		{
			return Feature;
		}
	}
	return nullptr;
}

void URPGItem::AddFeature(URPGItemFeature* Feature)
{
	ItemFeatures.Add(Feature);
	Feature->Initialize();
}

void URPGItem::RemoveFeature(URPGItemFeature* Feature)
{
	const int RemovedItemsCount = ItemFeatures.Remove(Feature);
	if (RemovedItemsCount > 0)
	{
		Feature->OnRemoved();
	}
}

#if WITH_EDITOR
void URPGItem::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	const FName ChangedPropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(URPGItem, bStackable))
	{
		if (!bStackable)
		{
			MaxStackSize = 1;
		}
		else
		{
			MaxStackSize = FMath::Max(2, MaxStackSize);
		}
	}
}
#endif

void URPGItem::PostInitProperties()
{
	Super::PostInitProperties();
	if (GIsEditor && !GIsPlayInEditorWorld)
	{
		return;
	}
	for (URPGItemFeature* Component : ItemFeatures)
	{
		Component->Initialize();
	}
}

bool URPGItem::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (URPGItemFeature* Feature : ItemFeatures)
	{
		if (IsValid(Feature))
		{
			bWroteSomething |= Feature->ReplicateSubobjects(Channel, Bunch, RepFlags);		// Lets the component add subobjects before replicating its own properties.
			bWroteSomething |= Channel->ReplicateSubobject(Feature, *Bunch, *RepFlags);	// (this makes those subobjects 'supported', and from here on those objects may have reference replicated)		
		}
	}
	return bWroteSomething;
}

void URPGItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(URPGItem, Icon);
	DOREPLIFETIME(URPGItem, Name);
	DOREPLIFETIME(URPGItem, Description);
	DOREPLIFETIME(URPGItem, bStackable);
	DOREPLIFETIME(URPGItem, MaxStackSize);
	DOREPLIFETIME(URPGItem, ItemTags);
	DOREPLIFETIME(URPGItem, ItemFeatures);
}

FPrimaryAssetId URPGItem::GetPrimaryAssetId() const
{
	// This is a DataAsset and not a blueprint so we can just use the raw FName
	// For blueprints you need to handle stripping the _C suffix
	return FPrimaryAssetId(URPGAssetManager::ItemType, GetFName());
}

void URPGItem::OnRep_ItemChanged() const
{
	OnItemChangedNative.Broadcast();
	OnItemChanged.Broadcast();
}

void URPGItem::OnRep_ItemComponents() const
{
	OnRep_ItemChanged();
}
