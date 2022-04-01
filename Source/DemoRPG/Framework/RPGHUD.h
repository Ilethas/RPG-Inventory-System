// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RPGHUDBase.h"
#include "DemoRPG/RPGTypes.h"
#include "RPGHUD.generated.h"

UCLASS()
class DEMORPG_API ARPGHUD : public ARPGHUDBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, Category = HUD)
	TSubclassOf<UUserWidget> SplitItemWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, Category = HUD)
	TSubclassOf<UUserWidget> BagWidgetClass;
	
	UFUNCTION(BlueprintCallable, Category = HUD)
	void ShowCursorOnDrop(class UDragDropOperation* Operation);

	UFUNCTION(BlueprintCallable, Category = HUD)
	void HideCursorOnDrag(class UDragDropOperation* Operation);

	UFUNCTION(BlueprintCallable, Category = HUD)
	void ShowSplitItemWidget(class ARPGItemInstance* ItemInstance, class URPGInventory* TargetInventory, class URPGInventoryMappingContainer* TargetContainer, const FRPGUISlot& TargetSlot);

	UFUNCTION(BlueprintCallable, Category = HUD)
	void HideSplitItemWidget();

	UFUNCTION(BlueprintCallable, Category = HUD)
	void ShowBagWidget(class ARPGItemInstance* BagItem);

	UFUNCTION(BlueprintCallable, Category = HUD)
	void ShowInventoryScreen();

	UFUNCTION(BlueprintCallable, Category = HUD)
	void HideInventoryScreen();

	UFUNCTION(BlueprintCallable, Category = HUD)
	void ToggleInventoryScreen();

	virtual void SetCurrentlyFocusedWidget(class URPGFocusWidget* WidgetToGiveFocus) override;
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = HUD)
	class URPGSplitItemWidget* SplitItemWidget;
};
