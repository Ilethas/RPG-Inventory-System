// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RPGItemFeature.h"
#include "RPGCreateBagFeature.generated.h"

UCLASS(Meta = (DisplayName = "Bag"))
class DEMORPG_API URPGCreateBagFeature : public URPGItemFeature
{
	GENERATED_BODY()
public:
	virtual void OnItemInstanceConstructed(ARPGItemInstance* ItemInstance) override;
};
