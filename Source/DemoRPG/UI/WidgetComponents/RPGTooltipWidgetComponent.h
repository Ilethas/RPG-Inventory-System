// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RPGWidgetComponent.h"
#include "RPGTooltipWidgetComponent.generated.h"

UCLASS(Meta = (DisplayName = "Tooltip Widget Component"))
class DEMORPG_API URPGTooltipWidgetComponent : public URPGWidgetComponent
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Category = Tooltip)
	class URPGTooltip* GetTooltipWidget() const { return TooltipWidget; }

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = Tooltip)
	void SetTooltipDelay(const float NewDelay);

	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Category = Tooltip)
	float GetTooltipDelay() const { return TooltipDelay; }

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = Tooltip)
	void SetTooltipOffset(const FVector2D& NewTooltipOffset);

	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Category = Tooltip)
	const FVector2D& GetTooltipOffset() const { return TooltipOffset; }
	
	virtual void BeginDestroy() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Tooltip)
	TSubclassOf<class URPGTooltip> TooltipWidgetClass;
	
	UPROPERTY(BlueprintReadOnly, BlueprintGetter = GetTooltipWidget, Category = Tooltip)
	class URPGTooltip* TooltipWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintSetter = SetTooltipDelay, BlueprintGetter = GetTooltipDelay, Category = Tooltip)
	float TooltipDelay = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintSetter = SetTooltipOffset, BlueprintGetter = GetTooltipOffset, Category = Tooltip)
	FVector2D TooltipOffset;
	
	void ShowTooltip() const;
	void HideTooltip() const;
	
	virtual void OnConstructedNative() override;
	virtual void OnRemovedNative() override;
	
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
};