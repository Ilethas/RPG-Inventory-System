// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DemoRPG/RPGNetworkedObject.h"
#include "UObject/NoExportTypes.h"
#include "RPGUIMappingContainer.generated.h"

DECLARE_MULTICAST_DELEGATE(FUIMappingChangedNative);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUIMappingChanged);

UCLASS(BlueprintType)
class URPGUIMapping : public URPGNetworkedObject
{
	GENERATED_BODY()
public:
	FUIMappingChangedNative OnUIMappingChangedNative;
	UPROPERTY(BlueprintAssignable, Category = "UI Mapping")
	FUIMappingChanged OnUIMappingChanged;
	
	void SetOwnerContainer(class URPGUIMappingContainer* NewOwnerContainer);

	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Category = "UI Mapping")
	class URPGUIMappingContainer* GetOwnerContainer() const { return OwnerContainer; }
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(BlueprintReadOnly, BlueprintGetter = GetOwnerContainer, Category = "UI Mapping", ReplicatedUsing = OnRep_UIMapping)
	class URPGUIMappingContainer* OwnerContainer;

	UFUNCTION()
	virtual void OnRep_UIMapping();
};

USTRUCT(BlueprintType)
struct FRPGUIMappingEntry : public FFastArraySerializerItem
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly)
	class URPGUIMappingContainer* OwnerContainer = nullptr;

	UPROPERTY(BlueprintReadOnly)
	URPGUIMapping* UIMapping = nullptr;

	void PreReplicatedRemove(const struct FRPGItemUIMappingArray& InArraySerializer);
	void PostReplicatedAdd(const struct FRPGItemUIMappingArray& InArraySerializer);
	void PostReplicatedChange(const struct FRPGItemUIMappingArray& InArraySerializer);

private:
	bool bWasPostReplicatedAddCalled = false;
};

USTRUCT(BlueprintType)
struct FRPGItemUIMappingArray: public FFastArraySerializer
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly)
	TArray<FRPGUIMappingEntry> UIMappingEntries;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams);
};

template<>
struct TStructOpsTypeTraits<FRPGItemUIMappingArray> : public TStructOpsTypeTraitsBase2<FRPGItemUIMappingArray>
{
	enum 
	{
		WithNetDeltaSerializer = true,
	};
};

DECLARE_MULTICAST_DELEGATE_OneParam(FUIMappingRemovedNative, class URPGUIMapping*);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUIMappingRemoved, class URPGUIMapping*, RemovedUIMapping);

DECLARE_MULTICAST_DELEGATE_OneParam(FUIMappingAddedNative, class URPGUIMapping*);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUIMappingAdded, class URPGUIMapping*, AddedUIMapping);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DEMORPG_API URPGUIMappingContainer : public UActorComponent
{
	GENERATED_BODY()
	friend struct FRPGUIMappingEntry;
public:
	FUIMappingRemovedNative OnUIMappingRemovedNative;
	UPROPERTY(BlueprintAssignable, Category = "UI Mapping Container")
	FUIMappingRemoved OnUIMappingRemoved;

	FUIMappingAddedNative OnUIMappingAddedNative;
	UPROPERTY(BlueprintAssignable, Category = "UI Mapping Container")
	FUIMappingAdded OnUIMappingAdded;

	FUIMappingChangedNative OnUIMappingChangedNative;
	UPROPERTY(BlueprintAssignable, Category = "UI Mapping Container")
	FUIMappingChanged OnUIMappingChanged;
	
	URPGUIMappingContainer();

	UFUNCTION(BlueprintCallable, Category = "UI Mapping Container")
	FRPGUIMappingEntry& AddUIMapping(URPGUIMapping* UIMapping);

	UFUNCTION(BlueprintCallable, Category = "UI Mapping Container")
	void RemoveUIMapping(URPGUIMapping* UIMapping);

	template <typename Predicate>
	void RemoveUIMappingsByPredicate(Predicate Pred);

	UFUNCTION(BlueprintPure, Category = "UI Mapping Container")
	TArray<URPGUIMapping*> GetUIMappings(const TSubclassOf<URPGUIMapping> UIMappingClass) const;

	template <typename UIMappingType>
	TArray<UIMappingType*> GetUIMappingsTyped() const;

	template <typename Functor>
	void ForEachUIMapping(Functor Func);

	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(BlueprintReadOnly, Transient, Replicated, Category = "UI Mapping Container")
	FRPGItemUIMappingArray UIMappingArray;
	
	void OnUIMappingRemoved_Internal(class URPGUIMapping* RemovedUIMapping);
	void OnUIMappingAdded_Internal(class URPGUIMapping* AddedUIMapping);
	void OnUIMappingChanged_Internal(class URPGUIMapping* ChangedUIMapping);
};

template <typename Predicate>
void URPGUIMappingContainer::RemoveUIMappingsByPredicate(Predicate Pred)
{
	checkf(GetOwnerRole() == ROLE_Authority, TEXT("Should only be called on server"));
	
	bool bRemovedSomething = false;
	for (int i = UIMappingArray.UIMappingEntries.Num() - 1; i >= 0; i--)
	{
		FRPGUIMappingEntry& Entry = UIMappingArray.UIMappingEntries[i];
		if (Pred(Entry.UIMapping))
		{
			URPGUIMapping* UIMapping = Entry.UIMapping;
			
			Entry.PreReplicatedRemove(UIMappingArray);
			UIMappingArray.UIMappingEntries.RemoveAt(i);
			bRemovedSomething = true;
			
			UIMapping->SetOwnerContainer(nullptr);
		}
	}
	
	if (bRemovedSomething)
	{
		UIMappingArray.MarkArrayDirty();
	}
}

template <typename UIMappingType>
TArray<UIMappingType*> URPGUIMappingContainer::GetUIMappingsTyped() const
{
	TArray<UIMappingType*> Result;
	for (const FRPGUIMappingEntry& Entry : UIMappingArray.UIMappingEntries)
	{
		if (UIMappingType* UIMapping = Cast<UIMappingType>(Entry.UIMapping))
		{
			Result.Add(UIMapping);
		}
	}
	return Result;
}

template <typename Functor>
void URPGUIMappingContainer::ForEachUIMapping(Functor Func)
{
	for (const FRPGUIMappingEntry& Entry : UIMappingArray.UIMappingEntries)
	{
		if (Func(Entry.UIMapping))
		{
			break;
		}
	}
}
