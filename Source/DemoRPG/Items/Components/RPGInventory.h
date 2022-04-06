// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RPGInventory.generated.h"

class URPGItem;
class ARPGItemInstance;
struct FRPGItemInstanceArray;
USTRUCT(BlueprintType, Meta = (DisplayName = "Item Instance Entry"))
struct FRPGItemInstanceArrayEntry : public FFastArraySerializerItem
{
	GENERATED_USTRUCT_BODY()

	FRPGItemInstanceArrayEntry() = default;
	FRPGItemInstanceArrayEntry(ARPGItemInstance* ItemInstance) : ItemInstance(ItemInstance) {}
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<ARPGItemInstance> ItemInstance = nullptr;

	void PreReplicatedRemove(const FRPGItemInstanceArray& InArraySerializer);
	void PostReplicatedAdd(const FRPGItemInstanceArray& InArraySerializer);
	void PostReplicatedChange(const FRPGItemInstanceArray& InArraySerializer);

private:
	bool bWasPostReplicatedAddCalled = false;
};

USTRUCT(BlueprintType, Meta = (DisplayName = "Item Instance Array"))
struct FRPGItemInstanceArray: public FFastArraySerializer
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FRPGItemInstanceArrayEntry> InstanceEntries;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams);
};

template<>
struct TStructOpsTypeTraits<FRPGItemInstanceArray> : public TStructOpsTypeTraitsBase2<FRPGItemInstanceArray>
{
	enum 
	{
		WithNetDeltaSerializer = true,
	};
};

UENUM(BlueprintType)
enum class EAddRemoveItemResultType : uint8
{
	NoItemsAddedOrRemoved,
	
	SomeItemsAdded,
	AllItemsAdded,

	SomeItemsRemoved,
	AllItemsRemoved
};

USTRUCT(BlueprintType)
struct FAddRemoveItemResult
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AddRemoveItemResult", Meta = (ExposeOnSpawn = "true"))
	EAddRemoveItemResultType ResultType = EAddRemoveItemResultType::NoItemsAddedOrRemoved;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AddRemoveItemResult", Meta = (ExposeOnSpawn = "true"))
	int AmountToChange = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AddRemoveItemResult", Meta = (ExposeOnSpawn = "true"))
	int AmountChanged = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AddRemoveItemResult", Meta = (ExposeOnSpawn = "true"))
	FText ErrorMessage = FText::GetEmpty();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AddRemoveItemResult", Meta = (ExposeOnSpawn = "true"))
	TArray<TObjectPtr<ARPGItemInstance>> ItemInstances;

	static FAddRemoveItemResult AllItemsAdded(const int Amount, TArray<ARPGItemInstance*> ItemInstances);
	static FAddRemoveItemResult SomeItemsRemoved(const int AmountToRemove, const int AmountRemoved, TArray<ARPGItemInstance*> ItemInstances, const FText& ErrorMessage);
	static FAddRemoveItemResult AllItemsRemoved(const int Amount, TArray<ARPGItemInstance*> ItemInstances);
};

DECLARE_MULTICAST_DELEGATE_OneParam(FInventoryItemRemovedNative, ARPGItemInstance*);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryItemRemoved, class ARPGItemInstance*, RemovedItem);

DECLARE_MULTICAST_DELEGATE_OneParam(FInventoryItemAddedNative, ARPGItemInstance*);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryItemAdded, class ARPGItemInstance*, AddedItem);

DECLARE_MULTICAST_DELEGATE_OneParam(FInventoryItemChangedNative, ARPGItemInstance*);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryItemChanged, class ARPGItemInstance*, ChangedItem);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent, DisplayName = "Inventory"))
class DEMORPG_API URPGInventory : public UActorComponent
{
	GENERATED_BODY()
	friend struct FRPGItemInstanceArrayEntry;
public:
	FInventoryItemRemovedNative OnInventoryItemRemovedNative;
	UPROPERTY(BlueprintAssignable, Category = Inventory)
	FInventoryItemRemoved OnInventoryItemRemoved;

	FInventoryItemAddedNative OnInventoryItemAddedNative;
	UPROPERTY(BlueprintAssignable, Category = Inventory)
	FInventoryItemAdded OnInventoryItemAdded;

	FInventoryItemChangedNative OnInventoryItemChangedNative;
	UPROPERTY(BlueprintAssignable, Category = Inventory)
	FInventoryItemChanged OnInventoryItemChanged;
	
	URPGInventory();

	UFUNCTION(BlueprintCallable, Category = Inventory)
	FAddRemoveItemResult AddItem(URPGItem* Item, int Quantity = 1);
	
	UFUNCTION(BlueprintCallable, Category = Inventory)
	FAddRemoveItemResult RemoveItem(URPGItem* Item, int Quantity = -1);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	void AddItemInstance(ARPGItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	void RemoveItemInstance(ARPGItemInstance* ItemInstance);
	
	UFUNCTION(BlueprintCallable, Category = Inventory)
	static bool CombineItemStacks(ARPGItemInstance* ItemBeingCombined, ARPGItemInstance* ReceivingItem);

	UFUNCTION(BlueprintPure, Category = Inventory)
	TArray<ARPGItemInstance*> GetItemInstances() const;

	template <typename Functor>
	void ForEachItemInstance(Functor Func);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool ContainsItem(ARPGItemInstance* ItemInstance, const bool bRecursive = false);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Replicated, Category = Inventory)
	FRPGItemInstanceArray ItemInstanceArray;
	
	void OnItemInstanceRemoved_Internal(ARPGItemInstance* RemovedItem);
	void OnItemInstanceAdded_Internal(ARPGItemInstance* AddedItem);
	void OnItemInstanceChanged_Internal(ARPGItemInstance* ChangedItem);
};

template <typename Functor>
void URPGInventory::ForEachItemInstance(Functor Func)
{
	for (const FRPGItemInstanceArrayEntry& Entry : ItemInstanceArray.InstanceEntries)
	{
		Func(Entry.ItemInstance);
	}
}
