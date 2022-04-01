// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RPGFocusWidget.h"
#include "RPGHUDWidget.generated.h"

UCLASS()
class DEMORPG_API URPGHUDWidget : public URPGFocusWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget), Category = HUD)
	class URPGInventoryScreenWidget* InventoryScreen;
	
	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget), Category = HUD)
	class UCanvasPanel* BagLayer;
	
	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget), Category = HUD)
	class UCanvasPanel* TooltipLayer;

protected:
	virtual void NativeOnInitialized() override;
	void OnSelectedCharacterChanged(class ARPGCharacterInstance* NewSelectedCharacter);
};
