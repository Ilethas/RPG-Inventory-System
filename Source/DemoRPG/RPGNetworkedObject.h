// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "RPGNetworkedObject.generated.h"

UCLASS()
class DEMORPG_API URPGNetworkedObject : public UObject
{
	GENERATED_BODY()
public:
	/** Traverses the outer chain searching for an Actor. */
	UFUNCTION(BlueprintPure, Category = "Networked Object")
	AActor* GetOwningActor() const;

	/** Get the network role of the owning Actor, or ROLE_None if there is no owner. */
	UFUNCTION(BlueprintPure, Category = "Networked Object")
	ENetRole GetOwnerRole() const;

	/** Returns whether owner Actor has network authority. */
	UFUNCTION(BlueprintCallable, Category="Networked Object")
	bool HasAuthority() const;

	virtual UWorld* GetWorld() const override;
	
	virtual bool ReplicateSubobjects(class UActorChannel *Channel, class FOutBunch *Bunch, FReplicationFlags *RepFlags);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool IsSupportedForNetworking() const override;
	virtual int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;
	virtual bool CallRemoteFunction(UFunction* Function, void* Parameters, FOutParmRec* OutParameters, FFrame* Stack) override;
};
