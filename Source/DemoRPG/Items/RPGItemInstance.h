// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RPGItemInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FItemInstanceChangedNative);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FItemInstanceChanged);

UCLASS(BlueprintType)
class DEMORPG_API ARPGItemInstance : public AActor
{
	GENERATED_BODY()
public:
	ARPGItemInstance();

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "Item Instance")
	void SetItem(class URPGItem* NewItem);
	
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Category = "Item Instance")
	URPGItem* GetItem() const { return Item; }

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "Item Instance")
	void SetQuantity(const int NewQuantity);

	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Category = "Item Instance")
	int GetQuantity() const { return Quantity; }

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "Item Instance")
	void SetItemOwner(UObject* NewItemOwner);

	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Category = "Item Instance")
	UObject* GetItemOwner() const { return ItemOwner; }

	template <typename T>
	T* GetTypedItemOwner() const;

	UFUNCTION()
	void OnRep_ItemInstance();
	
	FItemInstanceChangedNative OnItemInstanceChangedNative;
	UPROPERTY(BlueprintAssignable, Category = "Item Instance")
	FItemInstanceChanged OnItemInstanceChanged;

	UFUNCTION(BlueprintCallable, Category = "Item Instance")
	static ARPGItemInstance* Construct(UObject* NewItemOwner, URPGItem* ItemType, const int ItemQuantity = 1);

	virtual void OnSubobjectCreatedFromReplication(UObject* NewSubobject) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintSetter = SetItemOwner, BlueprintGetter = GetItemOwner, Category = "Item Instance", Replicated)
	TObjectPtr<UObject> ItemOwner;
	
	UPROPERTY(VisibleAnywhere, BlueprintSetter = SetItem, BlueprintGetter = GetItem, Category = "Item Instance", ReplicatedUsing = OnRep_Item)
	TObjectPtr<URPGItem> Item;

	UFUNCTION()
	void OnRep_Item();
	
	UPROPERTY(VisibleAnywhere, BlueprintSetter = SetQuantity, BlueprintGetter = GetQuantity, Category = "Item Instance", ReplicatedUsing = OnRep_ItemInstance,
		Meta = (UIMin = 1, EditCondition = "IsValid(Item) && Item->bStackable"))
	int Quantity = 1;
	
	virtual void BeginPlay() override;

#if WITH_EDITOR
	static int ARPGItemInstance_Index;
#endif
};

template <typename T>
T* ARPGItemInstance::GetTypedItemOwner() const
{
	return Cast<T>(ItemOwner);
}
