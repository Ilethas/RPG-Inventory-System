// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGEquippableFeature.h"

#include "DemoRPG/Items/RPGItemInstance.h"
#include "DemoRPG/Items/Components/RPGEquippableItem.h"

void URPGEquippableFeature::OnItemInstanceConstructed(ARPGItemInstance* ItemInstance)
{
	Super::OnItemInstanceConstructed(ItemInstance);
	if (ItemInstance->HasAuthority())
	{
		URPGEquippableItem* EquippableItem = NewObject<URPGEquippableItem>(ItemInstance);
		EquippableItem->RegisterComponent();
		ItemInstance->AddInstanceComponent(EquippableItem);
	}
}

FRPGModularModelPartReplacement* URPGEquippableFeature::GetModel(const FName InAppearanceName, const FName InModelTag, const FName InModelPartTag)
{
	for (FRPGModularModelPartReplacement& ModelPart : ItemModels)
	{
		if (ModelPart.AppearanceName == InAppearanceName && ModelPart.ModelTag == InModelTag && ModelPart.ModelPartTag == InModelPartTag)
		{
			return &ModelPart;
		}
	}
	return nullptr;
}
