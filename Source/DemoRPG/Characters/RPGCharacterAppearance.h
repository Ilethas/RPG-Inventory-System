// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RPGCharacterAppearance.generated.h"

UCLASS(BlueprintType, Meta = (DisplayName = "Character Appearance Tags"))
class DEMORPG_API URPGCharacterAppearanceTags : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	static const FName Appearance;
	static const FName Body;
};

USTRUCT(BlueprintType, Meta = (DisplayName = "Modular Model Part"))
struct FRPGModularModelPart
{
	GENERATED_BODY()
	FRPGModularModelPart(const FName InTag = NAME_None) : ModelPartTag(InTag) {}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Model)
	FName ModelPartTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Model)
	USkeletalMesh* SkeletalMesh = nullptr;
};

UINTERFACE(MinimalAPI, Blueprintable)
class URPGCharacterModelVisitorInterface : public UInterface
{
	GENERATED_BODY()
};

class IRPGCharacterModelVisitorInterface
{    
	GENERATED_BODY()

public:
	virtual void ApplyModel(class URPGCharacterModel* Model) {}
	virtual void ApplyModel(class URPGModularCharacterModel* Model) {}
};

UCLASS(Abstract, EditInlineNew, DefaultToInstanced)
class DEMORPG_API URPGCharacterModelBase : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Model)
	FName ModelTag;

	virtual void ApplyModel(IRPGCharacterModelVisitorInterface* Visitor) {}
};

UCLASS(BlueprintType, Meta = (DisplayName = "Character Model"))
class DEMORPG_API URPGCharacterModel : public URPGCharacterModelBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Model)
	USkeletalMesh* SkeletalMesh = nullptr;

	virtual void ApplyModel(IRPGCharacterModelVisitorInterface* Visitor) override;
};

UCLASS(BlueprintType, Meta = (DisplayName = "Modular Character Model"))
class DEMORPG_API URPGModularCharacterModel : public URPGCharacterModelBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Model)
	TArray<FRPGModularModelPart> ModelParts;

	virtual void ApplyModel(IRPGCharacterModelVisitorInterface* Visitor) override;
};

UCLASS(BlueprintType, Meta = (DisplayName = "Character Appearance"))
class DEMORPG_API URPGCharacterAppearance : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Appearance)
	FName AppearanceName;
	
	UPROPERTY(EditDefaultsOnly, Instanced, BlueprintReadOnly, Category = Appearance)
	TArray<URPGCharacterModelBase*> Models;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Appearance)
	UAnimBlueprint* AnimationBlueprint;

	UFUNCTION(BlueprintCallable, Category = Appearance)
	URPGCharacterModelBase* GetModel(const FName InModelTag);
	
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};
