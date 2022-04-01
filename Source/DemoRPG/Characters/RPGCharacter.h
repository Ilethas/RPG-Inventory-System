// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RPGCharacter.generated.h"

UCLASS(BlueprintType, Meta = (DisplayName = "Character"))
class DEMORPG_API URPGCharacter : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RPG Character")
	class URPGCharacterAppearance* CharacterAppearance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RPG Character")
	int MaxHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RPG Character")
	int MaxMana;
	
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};
