// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RPGItemFeature.h"
#include "RPGEquippableFeature.generated.h"

USTRUCT(BlueprintType, Meta = (DisplayName = "Modular Model Part Replacement"))
struct FRPGModularModelPartReplacement
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Appearance)
	FName AppearanceName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Appearance)
	FName ModelTag = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Appearance)
	FName ModelPartTag = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Appearance)
	USkeletalMesh* SkeletalMesh = nullptr;
};

UCLASS(BlueprintType, Meta = (DisplayName = "Equippable"))
class DEMORPG_API URPGEquippableFeature : public URPGItemFeature
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item)
	TArray<FName> ItemSlots;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item)
	TArray<FRPGModularModelPartReplacement> ItemModels;

	virtual void OnItemInstanceConstructed(ARPGItemInstance* ItemInstance) override;
	FRPGModularModelPartReplacement* GetModel(const FName InAppearanceName, const FName InModelTag, const FName InModelPartTag);
};
