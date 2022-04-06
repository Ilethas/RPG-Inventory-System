// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "RPGHUDBase.generated.h"

class URPGCursor;
class URPGFocusWidget;
UCLASS()
class DEMORPG_API ARPGHUDBase : public AHUD
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = HUD)
	virtual void SetCurrentlyFocusedWidget(URPGFocusWidget* WidgetToGiveFocus);
	
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Category = HUD)
	URPGFocusWidget* GetCurrentlyFocusedWidget() const { return CurrentlyFocusedWidget; }

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
	TObjectPtr<URPGHUDWidget> HUDWidget;
	
	UPROPERTY(BlueprintReadOnly, Category = HUD)
	TObjectPtr<URPGCursor> Cursor;
	
	virtual void BeginPlay() override;

private:
	UPROPERTY(BlueprintSetter = SetCurrentlyFocusedWidget, BlueprintGetter = GetCurrentlyFocusedWidget, Category = HUD)
	TObjectPtr<URPGFocusWidget> CurrentlyFocusedWidget;
};
