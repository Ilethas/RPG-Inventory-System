// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "RPGTypes.h"
#include "GameFramework/PlayerController.h"
#include "Items/RPGItemInstance.h"
#include "RPGPlayerController.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FRPGCharacterSelectedNative, class ARPGCharacterInstance*);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRPGCharacterSelected, class ARPGCharacterInstance*, SelectedCharacter);

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
	void SetSelectedCharacter(class ARPGCharacterInstance* CharacterToSelect);

	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Category = Gameplay)
	class ARPGCharacterInstance* GetSelectedCharacter() const { return SelectedCharacter; }

	UFUNCTION(Server, Reliable)
	void ServerMoveToLocation(AActor* RPGCharacter, const FVector& Destination);

	void CombineItemStacks(class ARPGItemInstance* ItemBeingCombined, class ARPGItemInstance* ReceivingItem);

	UFUNCTION(Server, Reliable)
	void ServerCombineItemStacks(class ARPGItemInstance* ItemBeingCombined, class ARPGItemInstance* ReceivingItem);
	
	void MoveItem(class ARPGItemInstance* ItemInstance, const bool bForceNewPosition, class URPGInventory* TargetInventory, class URPGInventoryMappingContainer* TargetContainer, const FRPGUISlot& TargetSlot);

	UFUNCTION(Server, Reliable)
	void ServerMoveItem(class ARPGItemInstance* ItemInstance, const bool bForceNewPosition, class URPGInventory* TargetInventory, class URPGInventoryMappingContainer* TargetContainer, const FRPGUISlot& TargetSlot);
	
	void SplitItem(ARPGItemInstance* ItemInstance, const int AmountToSplit, class URPGInventory* TargetInventory, class URPGInventoryMappingContainer* TargetContainer, const FRPGUISlot& TargetSlot);

	UFUNCTION(Server, Reliable)
	void ServerSplitItem(ARPGItemInstance* ItemInstance, const int AmountToSplit, class URPGInventory* TargetInventory, class URPGInventoryMappingContainer* TargetContainer, const FRPGUISlot& TargetSlot);

	UFUNCTION(Server, Reliable)
	void ServerEquipItem(ARPGCharacterInstance* InCharacterInstance, ARPGItemInstance* InItemInstance, const FName InItemSlot = NAME_None);

	void EquipItem(ARPGCharacterInstance* InCharacterInstance, ARPGItemInstance* InItemInstance, const FName InItemSlot = NAME_None);

	UFUNCTION(Server, Reliable)
	void ServerUnequipItem(ARPGCharacterInstance* InCharacterInstance, ARPGItemInstance* InItemInstance, URPGInventory* TargetInventory, URPGInventoryMappingContainer* TargetContainer, const FRPGUISlot& TargetSlot);

	void UnequipItem(ARPGCharacterInstance* InCharacterInstance, ARPGItemInstance* InItemInstance, URPGInventory* TargetInventory, URPGInventoryMappingContainer* TargetContainer, const FRPGUISlot& TargetSlot);

protected:
	UPROPERTY(BlueprintReadOnly, Category = Gameplay)
	class ARPGCharacterInstance* SelectedCharacter;
	
	UPROPERTY(BlueprintReadOnly, Category = Gameplay)
	class ARPGCharacterInstance* HighlightedCharacter;
	
	UPROPERTY(EditAnywhere, Category = Movement, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	class UClass* SplinePathActorClass;
	
	UPROPERTY(VisibleAnywhere, Category = Movement, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	class AActor* SplinePathActor;
	
	UPROPERTY()
	class URotateCharacter* RotateCharacterTask;

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

	bool CheckCapsuleJumpCollision(ACharacter* JumpingCharacter, const TArray<struct FPredictProjectilePathPointData>& JumpTrajectory, float CollisionCheckTolerance) const;
	void PreviewJumpTrajectory(const TArray<struct FPredictProjectilePathPointData>& JumpTrajectory);
};


