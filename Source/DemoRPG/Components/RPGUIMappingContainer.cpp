// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGUIMappingContainer.h"

#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"

void URPGUIMapping::SetOwnerContainer(URPGUIMappingContainer* NewOwnerContainer)
{
	OwnerContainer = NewOwnerContainer;
	// Rename(nullptr, NewOwnerContainer ? NewOwnerContainer : static_cast<UObject*>(GetTransientPackage()));
}

void URPGUIMapping::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(URPGUIMapping, OwnerContainer);
}

void URPGUIMapping::OnRep_UIMapping()
{
	OnUIMappingChangedNative.Broadcast();
	OnUIMappingChanged.Broadcast();
}

void FRPGUIMappingEntry::PreReplicatedRemove(const FRPGItemUIMappingArray& InArraySerializer)
{
	if (IsValid(OwnerContainer) && IsValid(UIMapping))
	{
		OwnerContainer->OnUIMappingRemoved_Internal(UIMapping);
	}
}

void FRPGUIMappingEntry::PostReplicatedAdd(const FRPGItemUIMappingArray& InArraySerializer)
{
	if (!bWasPostReplicatedAddCalled && IsValid(OwnerContainer) && IsValid(UIMapping))
	{
		OwnerContainer->OnUIMappingAdded_Internal(UIMapping);
	}
}

void FRPGUIMappingEntry::PostReplicatedChange(const FRPGItemUIMappingArray& InArraySerializer)
{
	if (IsValid(OwnerContainer) && IsValid(UIMapping))
	{
		if (!bWasPostReplicatedAddCalled)
		{
			OwnerContainer->OnUIMappingAdded_Internal(UIMapping);
		}
		else
		{
			OwnerContainer->OnUIMappingChanged_Internal(UIMapping);
		}
	}
}

bool FRPGItemUIMappingArray::NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
{
	return FastArrayDeltaSerialize<FRPGUIMappingEntry, FRPGItemUIMappingArray>(UIMappingEntries, DeltaParams, *this);
}

URPGUIMappingContainer::URPGUIMappingContainer()
{
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = false;
}

FRPGUIMappingEntry& URPGUIMappingContainer::AddUIMapping(URPGUIMapping* UIMapping)
{
	checkf(GetOwnerRole() == ROLE_Authority, TEXT("Should only be called on server"));
	
	const int Index = UIMappingArray.UIMappingEntries.Add(FRPGUIMappingEntry());
	FRPGUIMappingEntry& Entry = UIMappingArray.UIMappingEntries[Index];
	Entry.OwnerContainer = this;
	Entry.UIMapping = UIMapping;
	UIMapping->SetOwnerContainer(this);
	
	UIMappingArray.MarkItemDirty(Entry);
	Entry.PostReplicatedAdd(UIMappingArray);
	return Entry;
}

void URPGUIMappingContainer::RemoveUIMapping(URPGUIMapping* UIMapping)
{
	RemoveUIMappingsByPredicate([&](const URPGUIMapping* Element) { return Element == UIMapping; });
}

TArray<URPGUIMapping*> URPGUIMappingContainer::GetUIMappings(const TSubclassOf<URPGUIMapping> UIMappingClass) const
{
	TArray<URPGUIMapping*> Result;
	for (const FRPGUIMappingEntry& Entry : UIMappingArray.UIMappingEntries)
	{
		if (Entry.UIMapping->StaticClass()->IsChildOf(UIMappingClass))
		{
			Result.Add(Entry.UIMapping);
		}
	}
	return Result;
}

bool URPGUIMappingContainer::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (const FRPGUIMappingEntry& Entry : UIMappingArray.UIMappingEntries)
	{
		if (IsValid(Entry.UIMapping))
		{
			bWroteSomething |= Entry.UIMapping->ReplicateSubobjects(Channel, Bunch, RepFlags);		// Lets the component add subobjects before replicating its own properties.
			bWroteSomething |= Channel->ReplicateSubobject(Entry.UIMapping, *Bunch, *RepFlags);	// (this makes those subobjects 'supported', and from here on those objects may have reference replicated)		
		}
	}
	return bWroteSomething;
}

void URPGUIMappingContainer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(URPGUIMappingContainer, UIMappingArray);
}

void URPGUIMappingContainer::OnUIMappingRemoved_Internal(URPGUIMapping* RemovedUIMapping)
{
	OnUIMappingRemovedNative.Broadcast(RemovedUIMapping);
	OnUIMappingRemoved.Broadcast(RemovedUIMapping);
}

void URPGUIMappingContainer::OnUIMappingAdded_Internal(URPGUIMapping* AddedUIMapping)
{
	OnUIMappingAddedNative.Broadcast(AddedUIMapping);
	OnUIMappingAdded.Broadcast(AddedUIMapping);
}

void URPGUIMappingContainer::OnUIMappingChanged_Internal(URPGUIMapping* ChangedUIMapping)
{
	// OnUIMappingChangedNative.Broadcast(ChangedUIMapping);
	// OnUIMappingChanged.Broadcast(ChangedUIMapping);
}
