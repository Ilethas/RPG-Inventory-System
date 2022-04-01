// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RPGTooltip.generated.h"

UCLASS()
class DEMORPG_API URPGTooltip : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Category = Tooltip)
	float TooltipDelay;

	UFUNCTION(BlueprintCallable, Category = Tooltip)
	void ShowTooltip();
	
	UFUNCTION(BlueprintCallable, Category = Tooltip)
	void HideTooltip();
	
protected:
	FTimerHandle ShowTooltipHandle;
	
	UFUNCTION(BlueprintImplementableEvent, Category = Tooltip)
	void OnShowTooltip();
	
	UFUNCTION(BlueprintImplementableEvent, Category = Tooltip)
	void OnHideTooltip();
};
