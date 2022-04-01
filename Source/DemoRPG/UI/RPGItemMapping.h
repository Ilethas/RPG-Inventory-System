// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DemoRPG/RPGTypes.h"
#include "DemoRPG/Components/RPGUIMappingContainer.h"
#include "RPGItemMapping.generated.h"

UCLASS(BlueprintType)
class DEMORPG_API URPGItemMapping : public URPGUIMapping
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Category = "Item Mapping")
	class ARPGItemInstance* GetItemInstance() const { return ItemInstance; }
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	template <typename UIMappingType>
	static UIMappingType* FindMappingForItemTyped(class URPGUIMappingContainer* Container, class ARPGItemInstance* ItemInstance);

protected:
	UPROPERTY(BlueprintReadOnly, BlueprintGetter = GetItemInstance, Category = "Item Mapping", ReplicatedUsing = OnRep_ItemInstance)
	class ARPGItemInstance* ItemInstance;

	UFUNCTION()
	void OnRep_ItemInstance();
};

DECLARE_MULTICAST_DELEGATE_TwoParams(FInventoryMappingUISlotChangedNative, FRPGUISlot, class URPGInventoryMapping*);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInventoryMappingUISlotChanged, FRPGUISlot, OldUISlot, class URPGInventoryMapping*, InventoryMapping);

UCLASS(BlueprintType)
class DEMORPG_API URPGInventoryMapping : public URPGItemMapping
{
	GENERATED_BODY()
public:
	FInventoryMappingUISlotChangedNative OnInventoryMappingUISlotChangedNative;
	UPROPERTY(BlueprintAssignable, Category = "Inventory Mapping")
	FInventoryMappingUISlotChanged OnInventoryMappingUISlotChanged;

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "Inventory Mapping")
	void SetUISlot(const FRPGUISlot& NewUISlot);

	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Category = "Inventory Mapping")
	const FRPGUISlot& GetUISlot() const { return UISlot; }

	UFUNCTION(BlueprintCallable, Category = "Inventory Mapping")
	static URPGInventoryMapping* Construct(class ARPGItemInstance* Instance, class URPGUIMappingContainer* Container, const FRPGUISlot& InItemSlot);

	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "GetFirstUnoccupiedSlot", AutoCreateRefTerm = "StartingSlot"), Category = "Inventory Mapping")
	static FRPGUISlot GetFirstUnoccupiedSlot_BP(const URPGUIMappingContainer* Container, const int SlotsPerRow, const FRPGUISlot& StartingSlot);

	static FRPGUISlot GetFirstUnoccupiedSlot(const URPGUIMappingContainer* Container, const int SlotsPerRow, const FRPGUISlot& StartingSlot = FRPGUISlot());

	static FRPGUISlot FindSlotForItem(const ARPGItemInstance* InItemInstance, const URPGUIMappingContainer* Container, const int SlotsPerRow, const FRPGUISlot& StartingSlot = FRPGUISlot());
	
	UFUNCTION(BlueprintCallable, Category = "Inventory Mapping")
	static URPGInventoryMapping* FindMappingForSlot(class URPGUIMappingContainer* Container, const FRPGUISlot& TargetUISlot);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(BlueprintSetter = SetUISlot, BlueprintGetter = GetUISlot, Category = "Inventory Mapping", ReplicatedUsing=OnRep_UISlot)
	FRPGUISlot UISlot;
	UFUNCTION()
	void OnRep_UISlot(const FRPGUISlot OldUISlot);
};

DECLARE_MULTICAST_DELEGATE_TwoParams(FEquipmentMappingItemSlotChangedNative, FName, class URPGEquipmentMapping*);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEquipmentMappingItemSlotChanged, FName, OldItemSlot, class URPGEquipmentMapping*, EquipmentMapping);

UCLASS(BlueprintType)
class DEMORPG_API URPGEquipmentMapping : public URPGItemMapping
{
	GENERATED_BODY()
public:
	FEquipmentMappingItemSlotChangedNative OnEquipmentMappingItemSlotChangedNative;
	UPROPERTY(BlueprintAssignable, Category = "Inventory Mapping")
	FEquipmentMappingItemSlotChanged OnEquipmentMappingItemSlotChanged;
	
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "Equipment Mapping")
	void SetItemSlot(const FName NewItemSlot);
	
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Category = "Equipment Mapping")
	FName GetItemSlot() const { return ItemSlot; }

	UFUNCTION(BlueprintCallable, Category = "Equipment Mapping")
	static URPGEquipmentMapping* Construct(ARPGItemInstance* Instance, URPGUIMappingContainer* Container, const FName InItemSlot);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(BlueprintSetter = SetItemSlot, BlueprintGetter = GetItemSlot, Category = "Equipment Mapping", ReplicatedUsing=OnRep_ItemSlot)
	FName ItemSlot;
	UFUNCTION()
	void OnRep_ItemSlot(const FName OldItemSlot);
};

template <typename UIMappingType>
UIMappingType* URPGItemMapping::FindMappingForItemTyped(URPGUIMappingContainer* Container, ARPGItemInstance* ItemInstance)
{
	UIMappingType* Result = nullptr;
	Container->ForEachUIMapping([&](URPGUIMapping* UIMapping)
	{
		UIMappingType* ItemMapping = Cast<UIMappingType>(UIMapping);
		if (IsValid(ItemMapping) && ItemMapping->ItemInstance == ItemInstance)
		{
			Result = ItemMapping;
			return true;
		}
		return false;
	});
	return Result;
}
