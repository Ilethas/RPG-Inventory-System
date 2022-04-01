// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RPGFocusWidget.h"
#include "DemoRPG/RPGTypes.h"
#include "RPGButtonWidget.generated.h"


UCLASS()
class DEMORPG_API URPGButtonWidget : public URPGFocusWidget
{
	GENERATED_BODY()
public:
	FClickedNative OnClickedNative;
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "RPG Button")
	FClicked OnClicked;
	
	FClickedNative OnPressedNative;
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "RPG Button")
	FClicked OnPressed;
	
	FClickedNative OnReleasedNative;
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "RPG Button")
	FClicked OnReleased;
	
	FClickedNative OnHoveredNative;
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "RPG Button")
	FClicked OnHovered;
	
	FClickedNative OnUnhoveredNative;
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "RPG Button")
	FClicked OnUnhovered;

	UFUNCTION(BlueprintCallable, Category = "RPG Button")
	void BroadcastClick() const;
	
	UFUNCTION(BlueprintCallable, Category = "RPG Button")
	void BroadcastPress() const;
	
	UFUNCTION(BlueprintCallable, Category = "RPG Button")
	void BroadcastRelease() const;
	
	UFUNCTION(BlueprintCallable, Category = "RPG Button")
	void BroadcastHover() const;
	
	UFUNCTION(BlueprintCallable, Category = "RPG Button")
	void BroadcastUnhover() const;
};
