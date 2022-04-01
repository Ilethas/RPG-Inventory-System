// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Ability.generated.h"

/**
 * 
 */
UCLASS()
class DEMORPG_API UAbility : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GUI")
	FName Title;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GUI")
	FName Description;
};


UCLASS()
class DEMORPG_API UAbilityInstance : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAbility* Ability;
};
