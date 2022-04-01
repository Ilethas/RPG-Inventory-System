// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "RPGCharacterAppearance.h"
#include "RPGCharacterPreview.generated.h"

class ARPGItemInstance;
enum class ERPGCharacterSex : uint8;
UCLASS(BlueprintType)
class ARPGCharacterPreview : public AActor, public IRPGCharacterModelVisitorInterface
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Character)
	FVector SpawnLocation;
	
	ARPGCharacterPreview();
	
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Category = Character)
	class URPGCharacterAppearance* GetCharacterAppearance() const { return CharacterAppearance; }
	
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Category = Character)
	FName GetModelTag() const { return ModelTag; }

	UFUNCTION(BlueprintCallable, Category = Character)
	void LoadBasePreview(class URPGCharacterAppearance* InCharacterAppearance, const FName InModelTag);
	
	void UpdatePreviewFromEquipment(ARPGItemInstance* InItemInstance);

	virtual void ApplyModel(class URPGCharacterModel* Model) override;
	virtual void ApplyModel(class URPGModularCharacterModel* Model) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	class USpringArmComponent* SpringArm;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	class USceneCaptureComponent2D* SceneCaptureComponent;

	UPROPERTY(BlueprintReadOnly, Category = Character)
	class UTextureRenderTarget2D* RenderTarget;
	
	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* CharacterPreviewMaterial;

	UPROPERTY(BlueprintReadOnly, Category = Character)
	class UMaterialInstanceDynamic* CharacterPreviewMaterialInstance;
	
	UPROPERTY(BlueprintGetter = GetCharacterAppearance, BlueprintReadOnly, Category = Character)
	class URPGCharacterAppearance* CharacterAppearance;

	UPROPERTY(BlueprintGetter = GetModelTag, BlueprintReadOnly, Category = "RPG Character")
	FName ModelTag;
	
	virtual void BeginPlay() override;
};
