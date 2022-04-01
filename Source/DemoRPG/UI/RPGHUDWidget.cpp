#include "RPGHUDWidget.h"

#include "RPGInventoryScreenWidget.h"
#include "RPGInventoryWidget.h"
#include "DemoRPG/RPGPlayerController.h"
#include "DemoRPG/Characters/RPGCharacterInstance.h"
#include "DemoRPG/Framework/RPGGameState.h"

void URPGHUDWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (ARPGPlayerController* PlayerController = GetOwningPlayer<ARPGPlayerController>())
	{
		PlayerController->OnCharacterSelectedNative.AddUObject(this, &URPGHUDWidget::OnSelectedCharacterChanged);
	}
	if (ARPGGameState* GameState = GetWorld()->GetGameState<ARPGGameState>())
	{
		InventoryScreen->PartyStash->SetInventory(GameState->PartyStash);
		InventoryScreen->PartyStash->SetInventoryMappingContainer(GameState->PartyStashMappingContainer);
	}
}

void URPGHUDWidget::OnSelectedCharacterChanged(ARPGCharacterInstance* NewSelectedCharacter)
{
	InventoryScreen->SetCurrentCharacter(NewSelectedCharacter);
	if (IsValid(NewSelectedCharacter))
	{
		InventoryScreen->CharacterInventory->SetInventory(NewSelectedCharacter->InventoryComponent);
		InventoryScreen->CharacterInventory->SetInventoryMappingContainer(NewSelectedCharacter->InventoryMappingContainer);
	}
	else
	{
		InventoryScreen->CharacterInventory->SetInventory(nullptr);
		InventoryScreen->CharacterInventory->SetInventoryMappingContainer(nullptr);
	}
}
