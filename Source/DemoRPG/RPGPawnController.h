// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "RPGPawnController.generated.h"

/**
 * 
 */
UCLASS()
class DEMORPG_API ARPGPawnController : public AAIController
{
	GENERATED_BODY()
public:
	ARPGPawnController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
