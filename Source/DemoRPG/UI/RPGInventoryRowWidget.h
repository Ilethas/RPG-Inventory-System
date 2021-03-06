// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RPGInventoryRowWidget.generated.h"

class UHorizontalBox;
/**
 * 
 */
UCLASS()
class DEMORPG_API URPGInventoryRowWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget), Category = "Inventory Row Widget")
	TObjectPtr<UHorizontalBox> Container;
};
