// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RPGNetworkedAsset.generated.h"

UCLASS()
class DEMORPG_API URPGNetworkedAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	/** Traverses the outer chain searching for an Actor. */
	UFUNCTION(BlueprintPure, Category = "Networked Asset")
	AActor* GetOwningActor() const;

	/** Get the network role of the owning Actor, or ROLE_None if there is no owner. */
	UFUNCTION(BlueprintPure, Category = "Networked Asset")
	ENetRole GetOwnerRole() const;

	/** Returns whether owner Actor has network authority. */
	UFUNCTION(BlueprintCallable, Category="Networked Asset")
	bool HasAuthority() const;

	virtual UWorld* GetWorld() const override;
	
	virtual bool ReplicateSubobjects(class UActorChannel *Channel, class FOutBunch *Bunch, FReplicationFlags *RepFlags);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool IsSupportedForNetworking() const override;
	virtual int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;
	virtual bool CallRemoteFunction(UFunction* Function, void* Parameters, FOutParmRec* OutParameters, FFrame* Stack) override;
};
