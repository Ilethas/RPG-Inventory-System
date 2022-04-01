// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RPGWidgetComponent.h"
#include "RPGViewportDragWidgetComponent.generated.h"

UCLASS(Meta = (DisplayName = "Viewport Drag Widget Component"))
class DEMORPG_API URPGViewportDragWidgetComponent : public URPGWidgetComponent
{
	GENERATED_BODY()
protected:
	FVector2D DragOffset = FVector2D::ZeroVector;
	bool bIsDragged = false;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void TickNative(const FGeometry& MyGeometry, float InDeltaTime) override;
};