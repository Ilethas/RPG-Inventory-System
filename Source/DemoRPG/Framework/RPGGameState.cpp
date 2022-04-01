// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGGameState.h"

#include "DemoRPG/Components/RPGInventoryMappingContainer.h"
#include "DemoRPG/Items/Components/RPGInventory.h"


ARPGGameState::ARPGGameState()
{
	PartyStash = CreateDefaultSubobject<URPGInventory>(TEXT("PartyStash"));
	PartyStashMappingContainer = CreateDefaultSubobject<URPGInventoryMappingContainer>(TEXT("PartyStashMappingContainer"));
}

void ARPGGameState::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (HasAuthority())
	{
		PartyStash->OnInventoryItemAddedNative.AddUObject(PartyStashMappingContainer, &URPGInventoryMappingContainer::OnItemAdded);
		PartyStash->OnInventoryItemRemovedNative.AddUObject(PartyStashMappingContainer, &URPGInventoryMappingContainer::OnItemRemoved);
	}
}
