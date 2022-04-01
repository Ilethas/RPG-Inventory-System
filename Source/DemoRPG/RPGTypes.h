// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RPGTypes.generated.h"

#define STENCIL_CHARACTER_OUTLINE 252

#define COLLISION_CAMERA_PIVOT ECC_GameTraceChannel1

class ARPGItemInstance;
DECLARE_MULTICAST_DELEGATE(FClickedNative);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FClicked);

DECLARE_MULTICAST_DELEGATE(FPressedNative);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPressed);

DECLARE_MULTICAST_DELEGATE(FReleasedNative);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FReleased);

DECLARE_MULTICAST_DELEGATE(FHoveredNative);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHovered);

DECLARE_MULTICAST_DELEGATE(FUnhoveredNative);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUnhovered);

USTRUCT(BlueprintType)
struct FRPGUISlot
{
	GENERATED_BODY()
	FRPGUISlot(const int InX = 0, const int InY = 0) : X(InX), Y(InY) {}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Slot")
	int X = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Slot")
	int Y = 0;

	bool operator==(const FRPGUISlot& Other) const;

	bool operator!=(const FRPGUISlot& Other) const;
};

USTRUCT(BlueprintType)
struct FRPGEquippedItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipped Item")
	FName ItemSlot = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipped Item")
	ARPGItemInstance* ItemInstance = nullptr;
};