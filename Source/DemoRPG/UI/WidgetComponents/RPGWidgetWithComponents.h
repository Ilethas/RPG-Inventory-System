// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DemoRPG/UI/RPGFocusWidget.h"
#include "RPGWidgetWithComponents.generated.h"

class URPGWidgetComponent;
UCLASS()
class DEMORPG_API URPGWidgetWithComponents : public URPGFocusWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Widget Components")
	void AddWidgetComponent(URPGWidgetComponent* Component);
	
	UFUNCTION(BlueprintCallable, Category = "Widget Components")
	void RemoveWidgetComponent(URPGWidgetComponent* Component);

	UFUNCTION(BlueprintCallable, Meta = (DeterminesOutputType="ComponentClass", DisplayName = "Get Component"), Category = "Widget Components")
	URPGWidgetComponent* GetComponent_BP(const TSubclassOf<URPGWidgetComponent> ComponentClass);

	template <typename ComponentType>
	ComponentType* GetComponent();

	virtual void PostInitProperties() override;

protected:
	UPROPERTY(Transient)
	TSet<TObjectPtr<URPGWidgetComponent>> WidgetComponents;
	
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};

template <typename ComponentType>
ComponentType* URPGWidgetWithComponents::GetComponent()
{
	return GetComponent_BP(ComponentType::StaticClass());
}
