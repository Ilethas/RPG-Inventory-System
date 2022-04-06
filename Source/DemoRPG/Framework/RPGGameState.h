// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "RPGGameState.generated.h"

class URPGInventoryMappingContainer;
class URPGInventory;
/**
 * 
 */
UCLASS()
class DEMORPG_API ARPGGameState : public AGameState
{
	GENERATED_BODY()
public:
	ARPGGameState();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG Game State")
	TObjectPtr<URPGInventory> PartyStash;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG Game State")
	TObjectPtr<URPGInventoryMappingContainer> PartyStashMappingContainer;

	virtual void PostInitializeComponents() override;
};
