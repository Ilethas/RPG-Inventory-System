// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RPGItemDragWidget.generated.h"

class URPGItemMapping;
UCLASS()
class DEMORPG_API URPGItemDragWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent)
	void RefreshWidget();
	
	UFUNCTION(BlueprintCallable, Category = "Item Drag Widget")
	void SetSize(const FVector2D& NewSize);

	UFUNCTION(BlueprintCallable, Category = "Item Drag Widget")
	void SetItemMapping(URPGItemMapping* InItemMapping);
	
protected:
	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget), Category = "Item Drag Widget")
	class USizeBox* SizeBox;

	UPROPERTY(BlueprintReadOnly, Category = "Item Drag Widget")
	URPGItemMapping* ItemMapping;
};
