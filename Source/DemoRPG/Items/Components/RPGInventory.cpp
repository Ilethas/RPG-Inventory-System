// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGInventory.h"

#include "DemoRPG/Items/RPGItem.h"
#include "DemoRPG/Items/RPGItemInstance.h"
#include "Net/UnrealNetwork.h"

#define LOCTEXT_NAMESPACE "InventoryComponent"

// Sets default values for this component's properties
URPGInventory::URPGInventory()
{
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = false;
}

void FRPGItemInstanceArrayEntry::PreReplicatedRemove(const FRPGItemInstanceArray& InArraySerializer)
{
	if (IsValid(ItemInstance) && IsValid(ItemInstance->GetTypedItemOwner<URPGInventory>()))
	{
		ItemInstance->GetTypedItemOwner<URPGInventory>()->OnItemInstanceRemoved_Internal(ItemInstance);
	}
}

void FRPGItemInstanceArrayEntry::PostReplicatedAdd(const FRPGItemInstanceArray& InArraySerializer)
{
	if (!bWasPostReplicatedAddCalled && IsValid(ItemInstance) && IsValid(ItemInstance->GetTypedItemOwner<URPGInventory>()))
	{
		ItemInstance->GetTypedItemOwner<URPGInventory>()->OnItemInstanceAdded_Internal(ItemInstance);
	}
}

void FRPGItemInstanceArrayEntry::PostReplicatedChange(const FRPGItemInstanceArray& InArraySerializer)
{
	if (IsValid(ItemInstance) && IsValid(ItemInstance->GetTypedItemOwner<URPGInventory>()))
	{
		if (!bWasPostReplicatedAddCalled)
		{
			ItemInstance->GetTypedItemOwner<URPGInventory>()->OnItemInstanceAdded_Internal(ItemInstance);
		}
		else
		{
			ItemInstance->GetTypedItemOwner<URPGInventory>()->OnItemInstanceChanged_Internal(ItemInstance);
		}
	}
}

bool FRPGItemInstanceArray::NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
{
	return FastArrayDeltaSerialize<FRPGItemInstanceArrayEntry, FRPGItemInstanceArray>(InstanceEntries, DeltaParams, *this);
}

FAddRemoveItemResult FAddRemoveItemResult::AllItemsAdded(const int Amount, TArray<class ARPGItemInstance*> ItemInstances)
{
	FAddRemoveItemResult AddResult;
	AddResult.ResultType = EAddRemoveItemResultType::AllItemsAdded;
	AddResult.AmountToChange = Amount;
	AddResult.AmountChanged = Amount;
	AddResult.ItemInstances = MoveTemp(ItemInstances);
	return AddResult;
}

FAddRemoveItemResult FAddRemoveItemResult::SomeItemsRemoved(const int AmountToRemove, const int AmountRemoved, TArray<class ARPGItemInstance*> ItemInstances, const FText& ErrorMessage)
{
	FAddRemoveItemResult RemoveResult;
	RemoveResult.ResultType = EAddRemoveItemResultType::SomeItemsRemoved;
	RemoveResult.AmountToChange = AmountToRemove;
	RemoveResult.AmountChanged = AmountRemoved;
	RemoveResult.ErrorMessage = ErrorMessage;
	RemoveResult.ItemInstances = MoveTemp(ItemInstances);
	return RemoveResult;
}

FAddRemoveItemResult FAddRemoveItemResult::AllItemsRemoved(const int Amount, TArray<class ARPGItemInstance*> ItemInstances)
{
	FAddRemoveItemResult RemoveResult;
	RemoveResult.ResultType = EAddRemoveItemResultType::SomeItemsRemoved;
	RemoveResult.AmountToChange = Amount;
	RemoveResult.AmountChanged = Amount;
	RemoveResult.ItemInstances = MoveTemp(ItemInstances);
	return RemoveResult;
}

bool URPGInventory::ContainsItem(ARPGItemInstance* ItemInstance, const bool bRecursive)
{
	for (const FRPGItemInstanceArrayEntry& Entry : ItemInstanceArray.InstanceEntries)
	{
		if (Entry.ItemInstance == ItemInstance)
		{
			return true;
		}
		if (bRecursive)
		{
			URPGInventory* Inventory = Entry.ItemInstance->FindComponentByClass<URPGInventory>();
			if (IsValid(Inventory) && Inventory->ContainsItem(ItemInstance))
			{
				return true;
			}
		}
	}
	return false;
}

void URPGInventory::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(URPGInventory, ItemInstanceArray);
}

FAddRemoveItemResult URPGInventory::AddItem(URPGItem* Item, const int Quantity)
{
	checkf(GetOwnerRole() == ROLE_Authority, TEXT("Should only be called on server"));
	
	// Item is not stackable, so we add an instance of the item *Quantity* times.
	if (!Item->bStackable)
	{
		TArray<ARPGItemInstance*> AddedItems;
		for (int i = 0; i < Quantity; i++)
		{
			ARPGItemInstance* ItemInstance = ARPGItemInstance::Construct(this, Item, 1);
			
			const int Index = ItemInstanceArray.InstanceEntries.Add(ItemInstance);
			ItemInstanceArray.MarkItemDirty(ItemInstanceArray.InstanceEntries[Index]);
			ItemInstanceArray.InstanceEntries[Index].PostReplicatedAdd(ItemInstanceArray);
			AddedItems.Add(ItemInstance);
		}
		return FAddRemoveItemResult::AllItemsAdded(Quantity, MoveTemp(AddedItems));
	}
	
	// Item is stackable, so we search for stacks in inventory, fill them up to their capacity and create new stacks if necessary.
	TArray<FRPGItemInstanceArrayEntry> ItemsFound = ItemInstanceArray.InstanceEntries.FilterByPredicate([&](const FRPGItemInstanceArrayEntry& Entry)
	{
		check(Entry.ItemInstance);
		return Entry.ItemInstance->GetItem() == Item;
	});
	
	int AmountRemaining = Quantity;
	for (const FRPGItemInstanceArrayEntry& Entry : ItemsFound)
	{
		const int AmountToFillStack = FMath::Max(0, Item->MaxStackSize - Entry.ItemInstance->GetQuantity());
		Entry.ItemInstance->SetQuantity(Entry.ItemInstance->GetQuantity() + AmountToFillStack);
		AmountRemaining -= AmountToFillStack;
	}
	
	TArray<ARPGItemInstance*> AddedItems;
	while (AmountRemaining > 0)
	{
		ARPGItemInstance* ItemInstance = ARPGItemInstance::Construct(this, Item, FMath::Min(Item->MaxStackSize, AmountRemaining));
		
		const int Index = ItemInstanceArray.InstanceEntries.Add(ItemInstance);
		ItemInstanceArray.MarkItemDirty(ItemInstanceArray.InstanceEntries[Index]);
		ItemInstanceArray.InstanceEntries[Index].PostReplicatedAdd(ItemInstanceArray);
		AddedItems.Add(ItemInstance);
	
		AmountRemaining -= ItemInstance->GetQuantity();
	}
	return FAddRemoveItemResult::AllItemsAdded(Quantity, MoveTemp(AddedItems));
}

FAddRemoveItemResult URPGInventory::RemoveItem(URPGItem* Item, const int Quantity)
{
	checkf(GetOwnerRole() == ROLE_Authority, TEXT("Should only be called on server"));
	
	TArray<ARPGItemInstance*> RemovedItems;
	int AmountRemaining = Quantity;
	for (int i = ItemInstanceArray.InstanceEntries.Num() - 1; i >= 0; i--)
	{
		FRPGItemInstanceArrayEntry& Entry = ItemInstanceArray.InstanceEntries[i];
		check(IsValid(Entry.ItemInstance));
		
		if (AmountRemaining <= 0)
		{
			break;
		}
		const int AmountToRemove = FMath::Min(Entry.ItemInstance->GetQuantity(), AmountRemaining);
		if (Entry.ItemInstance->GetQuantity() == AmountToRemove)
		{
			Entry.PreReplicatedRemove(ItemInstanceArray);
			ItemInstanceArray.InstanceEntries.RemoveAt(i);

			Entry.ItemInstance->SetOwner(nullptr);
		}
		else
		{
			Entry.ItemInstance->SetQuantity(Entry.ItemInstance->GetQuantity() - AmountToRemove);
		}
		AmountRemaining -= AmountToRemove;
	}
	
	if (RemovedItems.Num() > 0)
	{
		ItemInstanceArray.MarkArrayDirty();
	}
	
	if (AmountRemaining != 0)
	{
		const int AmountRemoved = Quantity - AmountRemaining;
		const FText Message = FText::FormatNamed(LOCTEXT("RemovedSomeItemsText", "Removed {AmountRemoved} of {ItemName} from inventory"),
			TEXT("AmountRemoved"), AmountRemoved, TEXT("ItemName"), Item->Name);
		FAddRemoveItemResult::SomeItemsRemoved(Quantity, AmountRemoved, MoveTemp(RemovedItems), Message);
	}
	return FAddRemoveItemResult::AllItemsRemoved(Quantity, MoveTemp(RemovedItems));
}

void URPGInventory::AddItemInstance(ARPGItemInstance* ItemInstance)
{
	checkf(GetOwnerRole() == ROLE_Authority, TEXT("Should only be called on server"));
	ItemInstance->AttachToActor(GetOwner(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	ItemInstance->SetItemOwner(this);
	
	const int Index = ItemInstanceArray.InstanceEntries.Add(ItemInstance);
	ItemInstanceArray.MarkItemDirty(ItemInstanceArray.InstanceEntries[Index]);
	ItemInstanceArray.InstanceEntries[Index].PostReplicatedAdd(ItemInstanceArray);
}

void URPGInventory::RemoveItemInstance(ARPGItemInstance* ItemInstance)
{
	checkf(GetOwnerRole() == ROLE_Authority, TEXT("Should only be called on server"));
	
	for (int i = 0; i < ItemInstanceArray.InstanceEntries.Num(); i++)
	{
		FRPGItemInstanceArrayEntry& Entry = ItemInstanceArray.InstanceEntries[i];
		if (Entry.ItemInstance == ItemInstance)
		{
			Entry.PreReplicatedRemove(ItemInstanceArray);
			ItemInstanceArray.InstanceEntries.RemoveAt(i);
			ItemInstanceArray.MarkArrayDirty();
			
			ItemInstance->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
			ItemInstance->SetItemOwner(nullptr);
			break;
		}
	}
}

bool URPGInventory::CombineItemStacks(ARPGItemInstance* ItemBeingCombined, ARPGItemInstance* ReceivingItem)
{
	checkf(ItemBeingCombined->HasAuthority(), TEXT("Should only be called on server"));
	check(IsValid(ItemBeingCombined->GetItem()) &&  IsValid(ReceivingItem->GetItem()));
	bool bCombinedItemRemoved = false;
	
	// Only stackable items of the same type can be combined
	if (ItemBeingCombined->GetItem() != ReceivingItem->GetItem() || !(ItemBeingCombined->GetItem()->bStackable && ReceivingItem->GetItem()->bStackable))
	{
		return bCombinedItemRemoved;
	}

	const int MaxAmountToTransfer = FMath::Max(0, ReceivingItem->GetItem()->MaxStackSize - ReceivingItem->GetQuantity());
	const int ActualAmountTransferred = FMath::Min(ItemBeingCombined->GetQuantity(), MaxAmountToTransfer);
	
	check(IsValid(ItemBeingCombined->GetTypedItemOwner<URPGInventory>()));
	if (ItemBeingCombined->GetQuantity() - ActualAmountTransferred <= 0)
	{
		ItemBeingCombined->GetTypedItemOwner<URPGInventory>()->RemoveItemInstance(ItemBeingCombined);
		bCombinedItemRemoved = true;
	}

	ReceivingItem->SetQuantity(ReceivingItem->GetQuantity() + ActualAmountTransferred);
	ItemBeingCombined->SetQuantity(ItemBeingCombined->GetQuantity() - ActualAmountTransferred);
	return bCombinedItemRemoved;
}

TArray<ARPGItemInstance*> URPGInventory::GetItemInstances() const
{
	TArray<ARPGItemInstance*> Result;
	for (const FRPGItemInstanceArrayEntry& Entry : ItemInstanceArray.InstanceEntries)
	{
		Result.Add(Entry.ItemInstance);
	}
	return Result;
}

void URPGInventory::OnItemInstanceRemoved_Internal(ARPGItemInstance* RemovedItem)
{
	OnInventoryItemRemovedNative.Broadcast(RemovedItem);
	OnInventoryItemRemoved.Broadcast(RemovedItem);
}

void URPGInventory::OnItemInstanceAdded_Internal(ARPGItemInstance* AddedItem)
{
	OnInventoryItemAddedNative.Broadcast(AddedItem);
	OnInventoryItemAdded.Broadcast(AddedItem);
}

void URPGInventory::OnItemInstanceChanged_Internal(ARPGItemInstance* ChangedItem)
{
	OnInventoryItemChangedNative.Broadcast(ChangedItem);
	OnInventoryItemChanged.Broadcast(ChangedItem);
}

#undef LOCTEXT_NAMESPACE
