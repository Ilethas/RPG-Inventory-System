// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RPGCursor.generated.h"

/**
 * 
 */
UCLASS()
class DEMORPG_API URPGCursor : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget), Category = "Cursor")
	class UImage* CursorImage;
};
