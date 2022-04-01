// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "RPGAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class DEMORPG_API URPGAssetManager : public UAssetManager
{
	GENERATED_BODY()
public:
	virtual void StartInitialLoading() override;
	
	static const FPrimaryAssetType ItemType;
	static const FPrimaryAssetType CharacterAppearanceType;
	static const FPrimaryAssetType CharacterType;

	/** Returns the current AssetManager object */
	static URPGAssetManager& Get();

	/**
	 * Synchronously loads an RPGItem subclass, this can hitch but is useful when you cannot wait for an async load
	 * This does not maintain a reference to the item so it will garbage collect if not loaded some other way
	 *
	 * @param PrimaryAssetId The asset identifier to load
	 * @param bDisplayWarning If true, this will log a warning if the item failed to load
	 */
	template <typename AssetType>
	AssetType* ForceLoadAsset(const FPrimaryAssetId& PrimaryAssetId, bool bLogWarning = true);
};

template <typename AssetType>
AssetType* URPGAssetManager::ForceLoadAsset(const FPrimaryAssetId& PrimaryAssetId, bool bLogWarning)
{
	const FSoftObjectPath ItemPath = GetPrimaryAssetPath(PrimaryAssetId);

	// This does a synchronous load and may hitch
	AssetType* LoadedItem = Cast<AssetType>(ItemPath.TryLoad());

	if (bLogWarning && LoadedItem == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load asset for identifier %s!"), *PrimaryAssetId.ToString());
	}

	return LoadedItem;
}