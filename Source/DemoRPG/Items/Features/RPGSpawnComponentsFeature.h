// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RPGItemFeature.h"
#include "RPGSpawnComponentsFeature.generated.h"

UCLASS(Meta = (DisplayName = "Spawn Component"))
class DEMORPG_API URPGSpawnComponentsFeature : public URPGItemFeature
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Feature")
	TArray<TSubclassOf<UActorComponent>> ComponentsToSpawn;
	
	virtual void OnItemInstanceConstructed(ARPGItemInstance* ItemInstance) override;
};
