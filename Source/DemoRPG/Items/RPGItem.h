// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "DemoRPG/RPGNetworkedAsset.h"
#include "RPGItem.generated.h"

DECLARE_MULTICAST_DELEGATE(FItemChangedNative);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FItemChanged);

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class DEMORPG_API URPGItem : public URPGNetworkedAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item, ReplicatedUsing = OnRep_ItemChanged)
	TObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item, ReplicatedUsing = OnRep_ItemChanged)
	FText Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item, ReplicatedUsing = OnRep_ItemChanged, Meta = (MultiLine = true))
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item, ReplicatedUsing = OnRep_ItemChanged)
	bool bStackable = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item, ReplicatedUsing = OnRep_ItemChanged, Meta = (ClampMin = 2, EditCondition = bStackable))
	int MaxStackSize = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item, ReplicatedUsing = OnRep_ItemChanged)
	FGameplayTagContainer ItemTags;
	
	FItemChangedNative OnItemChangedNative;
	UPROPERTY(BlueprintAssignable, Category = Item)
	FItemChanged OnItemChanged;
	
	UPROPERTY(BlueprintReadOnly)
	bool bIsDynamicAsset = false;
	
	UFUNCTION(BlueprintCallable, Meta = (DeterminesOutputType="FeatureClass"), Category = Item)
	class URPGItemFeature* GetFeature(const TSubclassOf<URPGItemFeature> FeatureClass);

	template <typename T>
	T* GetFeature();

	UFUNCTION(BlueprintCallable, Category = Item)
	void AddFeature(URPGItemFeature* Feature);

	UFUNCTION(BlueprintCallable, Category = Item)
	void RemoveFeature(URPGItemFeature* Feature);

	template <typename Functor>
	void ForEachItemFeature(Functor Func);

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
	virtual void PostInitProperties() override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
	
protected:
	UPROPERTY(EditDefaultsOnly, Instanced, Category = Item, ReplicatedUsing = OnRep_ItemComponents)
	TArray<TObjectPtr<URPGItemFeature>> ItemFeatures;
	
	UFUNCTION()
	void OnRep_ItemChanged() const;
	
	UFUNCTION()
	void OnRep_ItemComponents() const;
};

template <typename T>
T* URPGItem::GetFeature()
{
	return Cast<T>(GetFeature(T::StaticClass()));
}

template <typename Functor>
void URPGItem::ForEachItemFeature(Functor Func)
{
	for (URPGItemFeature* Feature : ItemFeatures)
	{
		if (Func(Feature))
		{
			break;
		}
	}
}
