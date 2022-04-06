// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "RPGCharacterAppearance.h"
#include "GameFramework/Character.h"
#include "RPGCharacterInstance.generated.h"

class ARPGCharacterInstance;
class URPGInventoryMappingContainer;
class URPGInventory;
struct FRPGEquippedItemsArray;
class URPGItem;
class ARPGItemInstance;

USTRUCT()
struct FRPGEquippedItemsArrayEntry : public FFastArraySerializerItem
{
	GENERATED_USTRUCT_BODY()

	FRPGEquippedItemsArrayEntry() = default;
	FRPGEquippedItemsArrayEntry(const FName InItemSlot, ARPGItemInstance* InItemInstance) : ItemSlot(InItemSlot), ItemInstance(InItemInstance) {}

	UPROPERTY()
	FName ItemSlot = NAME_None;
	
	UPROPERTY()
	TObjectPtr<ARPGItemInstance> ItemInstance = nullptr;

	bool bIsAboutToBeRemoved = false;

	void PreReplicatedRemove(const FRPGEquippedItemsArray& InArraySerializer);
	void PostReplicatedAdd(const FRPGEquippedItemsArray& InArraySerializer);
	void PostReplicatedChange(const FRPGEquippedItemsArray& InArraySerializer);

private:
	bool bWasPostReplicatedAddCalled = false;
};

USTRUCT()
struct FRPGEquippedItemsArray : public FFastArraySerializer
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	TArray<FRPGEquippedItemsArrayEntry> ItemEntries;
	
	UPROPERTY(NotReplicated)
	TObjectPtr<ARPGCharacterInstance> EquippingCharacter = nullptr;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams);
};

template<>
struct TStructOpsTypeTraits<FRPGEquippedItemsArray> : public TStructOpsTypeTraitsBase2<FRPGEquippedItemsArray>
{
	enum 
	{
		WithNetDeltaSerializer = true,
	};
};

UCLASS(Blueprintable)
class ARPGCharacterInstance : public ACharacter, public IAbilitySystemInterface, public IRPGCharacterModelVisitorInterface
{
	GENERATED_BODY()
	friend struct FRPGEquippedItemsArrayEntry;
public:
	ARPGCharacterInstance();

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "RPG Character")
	void SetHighlighted(const bool bHighlighted);

	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Category = "RPG Character")
	bool IsHighlighted() const { return bIsHighlighted; }

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }
	virtual void PostInitializeComponents() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void PostInitProperties() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG Character")
	TObjectPtr<URPGInventory> InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG Character")
	TObjectPtr<URPGInventoryMappingContainer> InventoryMappingContainer;

	virtual void ApplyModel(URPGCharacterModel* Model) override;
	virtual void ApplyModel(URPGModularCharacterModel* Model) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void EquipItem(ARPGItemInstance* ItemInstance, const FName ItemSlot = NAME_None);
	void UnequipItem(ARPGItemInstance* ItemInstance);
	bool IsEquipped(ARPGItemInstance* ItemInstance) const;
	
	FName GetEquippedItemSlot(ARPGItemInstance* ItemInstance) const;
	FName FindSlotForItem(ARPGItemInstance* ItemInstance) const;

protected:
	UPROPERTY(Transient, Replicated)
	FRPGEquippedItemsArray EquippedItems;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG Character")
	class URPGCharacter* Character;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (Categories = "Character Sex"), Category = "RPG Character")
	FGameplayTag CharacterSex;

	UPROPERTY(BlueprintSetter = SetHighlighted, BlueprintGetter = IsHighlighted, Category = "RPG Character")
	bool bIsHighlighted;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RPG Character")
	TSubclassOf<class ARPGCharacterPreview> CharacterPreviewClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG Character")
	TObjectPtr<ARPGCharacterPreview> CharacterPreview;

	void OnEquippedItemRemoved_Internal(const FName InItemSlot, ARPGItemInstance* InItemInstance);
	void OnEquippedItemAdded_Internal(const FName InItemSlot, ARPGItemInstance* InItemInstance);
	void OnEquippedItemChanged_Internal(const FName InItemSlot, ARPGItemInstance* InItemInstance);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG Character", Meta = (AllowPrivateAccess = "true"))
	TMap<TObjectPtr<URPGItem>, int> DefaultInventory;

	void OnItemRemoved(ARPGItemInstance* RemovedItem);

	void UpdateAppearanceFromEquipment(ARPGItemInstance* InItemInstance);
	void LoadBaseAppearance(URPGCharacter* InCharacter);
	void RefreshAppearance();
};

