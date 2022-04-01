// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "RPGHUDBase.generated.h"

UCLASS()
class DEMORPG_API ARPGHUDBase : public AHUD
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = HUD)
	virtual void SetCurrentlyFocusedWidget(class URPGFocusWidget* WidgetToGiveFocus);
	
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Category = HUD)
	class URPGFocusWidget* GetCurrentlyFocusedWidget() const { return CurrentlyFocusedWidget; }

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = HUD)
	void ResetRPGFocus();

	UFUNCTION(BlueprintCallable, Category = HUD)
	void ShowHUDWidget();

	UFUNCTION(BlueprintCallable, Category = HUD)
	void HideHUDWidget();

	UFUNCTION(BlueprintCallable, Category = HUD)
	void AddTooltip(class URPGTooltip* Tooltip);

protected:
	UPROPERTY(EditDefaultsOnly, Category = HUD)
	TSubclassOf<class URPGHUDWidget> HUDWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, Category = HUD)
	TSubclassOf<UUserWidget> CursorWidgetClass;
	
	UPROPERTY(BlueprintReadOnly, Category = HUD)
	class URPGHUDWidget* HUDWidget;
	
	UPROPERTY(BlueprintReadOnly, Category = HUD)
	class URPGCursor* Cursor;
	
	virtual void BeginPlay() override;

private:
	UPROPERTY(BlueprintSetter = SetCurrentlyFocusedWidget, BlueprintGetter = GetCurrentlyFocusedWidget, Category = HUD)
	class URPGFocusWidget* CurrentlyFocusedWidget;
};
