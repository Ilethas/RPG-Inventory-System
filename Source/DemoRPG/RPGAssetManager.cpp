// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGAssetManager.h"

#include "AbilitySystemGlobals.h"

const FPrimaryAssetType	URPGAssetManager::ItemType = TEXT("Item");
const FPrimaryAssetType	URPGAssetManager::CharacterAppearanceType = TEXT("Character Appearance");
const FPrimaryAssetType	URPGAssetManager::CharacterType = TEXT("Character");

void URPGAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	UAbilitySystemGlobals::Get().InitGlobalData();
}

URPGAssetManager& URPGAssetManager::Get()
{
	URPGAssetManager* This = Cast<URPGAssetManager>(GEngine->AssetManager);

	if (This)
	{
		return *This;
	}
	else
	{
		UE_LOG(LogTemp, Fatal, TEXT("Invalid AssetManager in DefaultEngine.ini, must be RPGAssetManager!"));
		return *NewObject<URPGAssetManager>(); // Never calls this
	}
}
