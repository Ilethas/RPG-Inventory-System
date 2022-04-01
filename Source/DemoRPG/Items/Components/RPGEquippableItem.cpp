#include "RPGEquippableItem.h"

#include "Net/UnrealNetwork.h"

void URPGEquippableItem::SetEquippingCharacter(ARPGCharacterInstance* NewEquippingCharacter)
{
	if (EquippingCharacter != NewEquippingCharacter)
	{
		EquippingCharacter = NewEquippingCharacter;
		OnRep_EquippingCharacter();
	}
}

URPGEquippableItem::URPGEquippableItem()
{
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = false;
}

void URPGEquippableItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(URPGEquippableItem, EquippingCharacter);
}

void URPGEquippableItem::OnRep_EquippingCharacter()
{
	OnEquippingCharacterChangedNative.Broadcast(EquippingCharacter);
	OnEquippingCharacterChanged.Broadcast(EquippingCharacter);
}
