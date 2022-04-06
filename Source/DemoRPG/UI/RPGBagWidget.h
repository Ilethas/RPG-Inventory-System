// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WidgetComponents/RPGWidgetWithComponents.h"
#include "RPGBagWidget.generated.h"

class ARPGItemInstance;
class URPGInventoryWidget;
class URPGViewportDragWidgetComponent;
UCLASS()
class DEMORPG_API URPGBagWidget : public URPGWidgetWithComponents
{
	GENERATED_BODY()
public:
	URPGBagWidget(const FObjectInitializer& ObjectInitializer);
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Bag Widget")
	void RefreshWidget();
	
	UFUNCTION(BlueprintCallable, Category = "Bag Widget")
	void SetBagItemInstance(ARPGItemInstance* NewBagItemInstance);

	UFUNCTION(BlueprintPure, Category = "Bag Widget")
	ARPGItemInstance* GetBagItemInstance() const { return BagItemInstance; }

	virtual void PostInitProperties() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bag Widget")
	TObjectPtr<URPGViewportDragWidgetComponent> DragComponent;
	
	UPROPERTY(BlueprintSetter = SetBagItemInstance, BlueprintGetter = GetBagItemInstance, Category = "Bag Widget")
	TObjectPtr<ARPGItemInstance> BagItemInstance;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget), Category = "Bag Widget")
	TObjectPtr<URPGInventoryWidget> InventoryWidget;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
};
