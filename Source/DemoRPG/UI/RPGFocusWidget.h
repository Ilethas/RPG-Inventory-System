// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RPGFocusWidget.generated.h"

UCLASS(Meta = (DisplayName = "RPG Focus Widget"))
class DEMORPG_API URPGFocusWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Focus Widget")
	void SetRPGFocus(URPGFocusWidget* PreviouslyFocusedWidget);
	
	UFUNCTION(BlueprintCallable, Category = "Focus Widget")
	void ClearRPGFocus(URPGFocusWidget* WidgetThatReceivedFocus);

	UFUNCTION(BlueprintPure, Category = "Focus Widget")
	bool HasRPGFocus() const;

	UFUNCTION(BlueprintCallable, Category = "Focus Widget")
	void SetRPGFocusable(const bool bNewFocusableState);

	UFUNCTION(BlueprintPure, Category = "Focus Widget")
	bool IsRPGFocusable() const { return bIsRPGFocusable; }

	UFUNCTION(BlueprintPure, Category = "Focus Widget")
	URPGFocusWidget* GetFocusableParent() const;

protected:
	UFUNCTION(BlueprintImplementableEvent, Meta = (DisplayName = "Received RPG Focus"), Category = "Focus Widget")
	void ReceivedRPGFocus_BP(URPGFocusWidget* PreviouslyFocusedWidget);
	
	UFUNCTION(BlueprintImplementableEvent, Meta = (DisplayName = "Lost RPG Focus"), Category = "Focus Widget")
	void LostRPGFocus_BP(URPGFocusWidget* WidgetThatReceivedFocus);
	
	UFUNCTION(BlueprintImplementableEvent, Meta = (DisplayName = "Descendant Lost RPG Focus"), Category = "Focus Widget")
	void DescendantLostRPGFocus_BP(URPGFocusWidget* Descendant, const bool bIsDirectRPGFocusableChild, URPGFocusWidget* WidgetThatReceivedFocus);
	
	virtual void ReceivedRPGFocus(URPGFocusWidget* PreviouslyFocusedWidget);
	virtual void LostRPGFocus(URPGFocusWidget* WidgetThatReceivedFocus);
	virtual void DescendantLostRPGFocus(URPGFocusWidget* Descendant, const bool bIsDirectRPGFocusableChild, URPGFocusWidget* WidgetThatReceivedFocus);

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Focus Widget")
	bool bIsRPGFocusable = true;
	bool bHasRPGFocus = false;
};
