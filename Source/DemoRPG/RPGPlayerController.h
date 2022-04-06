// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "RPGTypes.h"
#include "GameFramework/PlayerController.h"
#include "Items/RPGItemInstance.h"
#include "RPGPlayerController.generated.h"

struct FPredictProjectilePathPointData;
class URotateCharacter;
class URPGInventoryMappingContainer;
class URPGInventory;
class ARPGCharacterInstance;
DECLARE_MULTICAST_DELEGATE_OneParam(FRPGCharacterSelectedNative, ARPGCharacterInstance*);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRPGCharacterSelected, ARPGCharacterInstance*, SelectedCharacter);

UCLASS()
class ARPGPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	FRPGCharacterSelectedNative OnCharacterSelectedNative;
	UPROPERTY(BlueprintAssignable, Category = Gameplay)
	FRPGCharacterSelected OnCharacterSelected;
	
	ARPGPlayerController();

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = Gameplay)
	void SetSelectedCharacter(ARPGCharacterInstance* CharacterToSelect);

	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Category = Gameplay)
	ARPGCharacterInstance* GetSelectedCharacter() const { return SelectedCharacter; }

	UFUNCTION(Server, Reliable)
	void ServerMoveToLocation(AActor* RPGCharacter, const FVector& Destination);

	void CombineItemStacks(ARPGItemInstance* ItemBeingCombined, ARPGItemInstance* ReceivingItem);

	UFUNCTION(Server, Reliable)
	void ServerCombineItemStacks(ARPGItemInstance* ItemBeingCombined, ARPGItemInstance* ReceivingItem);
	
	void MoveItem(ARPGItemInstance* ItemInstance, const bool bForceNewPosition, URPGInventory* TargetInventory, URPGInventoryMappingContainer* TargetContainer, const FRPGUISlot& TargetSlot);

	UFUNCTION(Server, Reliable)
	void ServerMoveItem(ARPGItemInstance* ItemInstance, const bool bForceNewPosition, URPGInventory* TargetInventory, URPGInventoryMappingContainer* TargetContainer, const FRPGUISlot& TargetSlot);
	
	void SplitItem(ARPGItemInstance* ItemInstance, const int AmountToSplit, URPGInventory* TargetInventory, URPGInventoryMappingContainer* TargetContainer, const FRPGUISlot& TargetSlot);

	UFUNCTION(Server, Reliable)
	void ServerSplitItem(ARPGItemInstance* ItemInstance, const int AmountToSplit, URPGInventory* TargetInventory, URPGInventoryMappingContainer* TargetContainer, const FRPGUISlot& TargetSlot);

	UFUNCTION(Server, Reliable)
	void ServerEquipItem(ARPGCharacterInstance* InCharacterInstance, ARPGItemInstance* InItemInstance, const FName InItemSlot = NAME_None);

	void EquipItem(ARPGCharacterInstance* InCharacterInstance, ARPGItemInstance* InItemInstance, const FName InItemSlot = NAME_None);

	UFUNCTION(Server, Reliable)
	void ServerUnequipItem(ARPGCharacterInstance* InCharacterInstance, ARPGItemInstance* InItemInstance, URPGInventory* TargetInventory, URPGInventoryMappingContainer* TargetContainer, const FRPGUISlot& TargetSlot);

	void UnequipItem(ARPGCharacterInstance* InCharacterInstance, ARPGItemInstance* InItemInstance, URPGInventory* TargetInventory, URPGInventoryMappingContainer* TargetContainer, const FRPGUISlot& TargetSlot);

protected:
	UPROPERTY(BlueprintReadOnly, Category = Gameplay)
	TObjectPtr<ARPGCharacterInstance> SelectedCharacter;
	
	UPROPERTY(BlueprintReadOnly, Category = Gameplay)
	TObjectPtr<ARPGCharacterInstance> HighlightedCharacter;
	
	UPROPERTY(EditAnywhere, Category = Movement, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UClass> SplinePathActorClass;
	
	UPROPERTY(VisibleAnywhere, Category = Movement, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AActor> SplinePathActor;
	
	UPROPERTY()
	TObjectPtr<URotateCharacter> RotateCharacterTask;

	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	// End PlayerController interface

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	void OnInteract();
	void OnJump();

	void OnOpenInventory();

	void JumpMove(ACharacter* JumpCharacter, const FVector& JumpVelocity, const FVector& JumpDest);
	UFUNCTION(Server, Reliable)
	void ServerJumpMove(ACharacter* JumpCharacter, const FVector& JumpVelocity, const FVector& JumpDest);
	void PerformLaunch(ACharacter* JumpCharacter, const FVector JumpVelocity, const FVector JumpDest);

	bool CheckCapsuleJumpCollision(ACharacter* JumpingCharacter, const TArray<FPredictProjectilePathPointData>& JumpTrajectory, float CollisionCheckTolerance) const;
	void PreviewJumpTrajectory(const TArray<FPredictProjectilePathPointData>& JumpTrajectory);
};


