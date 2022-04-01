// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "RotateCharacter.generated.h"

DECLARE_MULTICAST_DELEGATE(FRotateCharacterOutputPinNative);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRotateCharacterOutputPin);


/**
 * 
 */
UCLASS(BlueprintType, meta=(ExposedAsyncProxy = AsyncTask))
class URotateCharacter : public UBlueprintAsyncActionBase, public FTickableGameObject
{
	GENERATED_BODY()
public:
	FRotateCharacterOutputPinNative OnFinishedNative;
	
	UPROPERTY(BlueprintAssignable)
	FRotateCharacterOutputPin OnFinished;
	
	FRotateCharacterOutputPinNative OnInterruptedNative;
	
	UPROPERTY(BlueprintAssignable)
	FRotateCharacterOutputPin OnInterrupted;
	
	UFUNCTION(BlueprintCallable, Meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"))
	static URotateCharacter* RotateCharacter(const UObject* WorldContextObject, ACharacter* Character, const FRotator& TargetRotation, float RotationSpeed = 300.f);

	URotateCharacter(const FObjectInitializer& ObjectInitializer);

	// Begin UBlueprintAsyncActionBase interface
	virtual void Activate() override;
	// End UBlueprintAsyncActionBase interface

	// Begin FTickableGameObject Interface
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override;
	virtual bool IsAllowedToTick() const override;
	virtual TStatId GetStatId() const override;
	// End FTickableGameObject Interface

	UFUNCTION(BlueprintCallable)
	void Interrupt();

	UFUNCTION(BlueprintCallable)
	void EndAction();

	UFUNCTION(BlueprintCallable)
	bool IsFinished() const { return bFinished || bInterrupted; }
	
private:
	const UObject* WorldContextObject;
	TWeakObjectPtr<ACharacter> Character;
	FRotator TargetRotation;
	float RotationSpeed;
	bool bFinished = false;
	bool bInterrupted = false;
	
	// Because engine would construct inner object when game load package (before game start), so we need to add a flag to identify which one need to be constructed on game running.
	bool bIsCreateOnRunning = false;
};
