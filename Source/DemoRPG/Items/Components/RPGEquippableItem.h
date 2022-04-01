// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RPGEquippableItem.generated.h"

class ARPGCharacterInstance;

DECLARE_MULTICAST_DELEGATE_OneParam(FRPGEquippingCharacterChangedNative, ARPGCharacterInstance*);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRPGEquippingCharacterChanged, ARPGCharacterInstance*, NewEquippingCharacter);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent, DisplayName = "Equippable Item"))
class DEMORPG_API URPGEquippableItem : public UActorComponent
{
	GENERATED_BODY()
public:
	FRPGEquippingCharacterChangedNative OnEquippingCharacterChangedNative;
	FRPGEquippingCharacterChanged OnEquippingCharacterChanged;

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "Equippable Item")
	void SetEquippingCharacter(ARPGCharacterInstance* NewEquippingCharacter);

	ARPGCharacterInstance* GetEquippingCharacter() const { return EquippingCharacter; }
	
	URPGEquippableItem();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(BlueprintSetter = SetEquippingCharacter, ReplicatedUsing = OnRep_EquippingCharacter, Category = "Equippable Item")
	ARPGCharacterInstance* EquippingCharacter;
	
	UFUNCTION()
	void OnRep_EquippingCharacter();
};
