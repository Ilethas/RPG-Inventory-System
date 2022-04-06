// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RPGFocusWidget.h"
#include "RPGHUDWidget.generated.h"

class ARPGCharacterInstance;
class UCanvasPanel;
class URPGInventoryScreenWidget;
UCLASS()
class DEMORPG_API URPGHUDWidget : public URPGFocusWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget), Category = HUD)
	TObjectPtr<URPGInventoryScreenWidget> InventoryScreen;
	
	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget), Category = HUD)
	TObjectPtr<UCanvasPanel> BagLayer;
	
	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget), Category = HUD)
	TObjectPtr<UCanvasPanel> TooltipLayer;

protected:
	virtual void NativeOnInitialized() override;
	void OnSelectedCharacterChanged(ARPGCharacterInstance* NewSelectedCharacter);
};
