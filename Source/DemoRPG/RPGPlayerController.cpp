// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGPlayerController.h"

#include "HeadMountedDisplayFunctionLibrary.h"
#include "RPGAssetManager.h"
#include "RPGFunctionLibrary.h"
#include "RPGPawnController.h"
#include "SplinePath.h"
#include "AsyncActions/RotateCharacter.h"
#include "Characters/RPGCharacterInstance.h"
#include "Components/CapsuleComponent.h"
#include "Components/RPGInventoryMappingContainer.h"
#include "Components/SplineComponent.h"
#include "Engine/World.h"
#include "Framework/RPGHUD.h"
#include "Items/Components/RPGInventory.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "UI/RPGItemMapping.h"

ARPGPlayerController::ARPGPlayerController()
{
	bShowMouseCursor = true;
}

void ARPGPlayerController::SetSelectedCharacter(ARPGCharacterInstance* CharacterToSelect)
{
	if (SelectedCharacter != CharacterToSelect)
	{
		SelectedCharacter = CharacterToSelect;
		OnCharacterSelectedNative.Broadcast(CharacterToSelect);
		OnCharacterSelected.Broadcast(CharacterToSelect);
	}
}

void ARPGPlayerController::ServerCombineItemStacks_Implementation(ARPGItemInstance* ItemBeingCombined, ARPGItemInstance* ReceivingItem)
{
	CombineItemStacks(ItemBeingCombined, ReceivingItem);
}

void ARPGPlayerController::CombineItemStacks(ARPGItemInstance* ItemBeingCombined, ARPGItemInstance* ReceivingItem)
{
	if (!HasAuthority())
	{
		ServerCombineItemStacks(ItemBeingCombined, ReceivingItem);
		return;
	}
	URPGInventory::CombineItemStacks(ItemBeingCombined, ReceivingItem);
}

void ARPGPlayerController::ServerMoveItem_Implementation(class ARPGItemInstance* ItemInstance, const bool bForceNewPosition, class URPGInventory* TargetInventory, class URPGInventoryMappingContainer* TargetContainer, const FRPGUISlot& TargetSlot)
{
	MoveItem(ItemInstance, bForceNewPosition, TargetInventory, TargetContainer, TargetSlot);
}

void ARPGPlayerController::MoveItem(class ARPGItemInstance* ItemInstance, const bool bForceNewPosition, class URPGInventory* TargetInventory, class URPGInventoryMappingContainer* TargetContainer, const FRPGUISlot& TargetSlot)
{
	if (!HasAuthority())
	{
		ServerMoveItem(ItemInstance, bForceNewPosition, TargetInventory, TargetContainer, TargetSlot);
		return;
	}
	URPGFunctionLibrary::MoveItem(ItemInstance, bForceNewPosition, TargetInventory, TargetContainer, TargetSlot);
}

void ARPGPlayerController::ServerSplitItem_Implementation(ARPGItemInstance* ItemInstance, const int AmountToSplit, class URPGInventory* TargetInventory, class URPGInventoryMappingContainer* TargetContainer, const FRPGUISlot& TargetSlot)
{
	SplitItem(ItemInstance, AmountToSplit, TargetInventory, TargetContainer, TargetSlot);
}

void ARPGPlayerController::SplitItem(ARPGItemInstance* ItemInstance, const int AmountToSplit, class URPGInventory* TargetInventory, class URPGInventoryMappingContainer* TargetContainer, const FRPGUISlot& TargetSlot)
{
	if (!HasAuthority())
	{
		ServerSplitItem(ItemInstance, AmountToSplit, TargetInventory, TargetContainer, TargetSlot);
		return;
	}
	URPGFunctionLibrary::SplitItem(ItemInstance, AmountToSplit, TargetInventory, TargetContainer, TargetSlot);
}

void ARPGPlayerController::ServerEquipItem_Implementation(ARPGCharacterInstance* InCharacterInstance, ARPGItemInstance* InItemInstance, const FName InItemSlot)
{
	EquipItem(InCharacterInstance, InItemInstance, InItemSlot);
}

void ARPGPlayerController::EquipItem(ARPGCharacterInstance* InCharacterInstance, ARPGItemInstance* InItemInstance, const FName InItemSlot)
{
	if (!HasAuthority())
	{
		ServerEquipItem(InCharacterInstance, InItemInstance), InItemSlot;
		return;
	}
	InCharacterInstance->EquipItem(InItemInstance, InItemSlot);
}

void ARPGPlayerController::ServerUnequipItem_Implementation(ARPGCharacterInstance* InCharacterInstance, ARPGItemInstance* InItemInstance, URPGInventory* TargetInventory, URPGInventoryMappingContainer* TargetContainer, const FRPGUISlot& TargetSlot)
{
	UnequipItem(InCharacterInstance, InItemInstance, TargetInventory, TargetContainer, TargetSlot);
}

void ARPGPlayerController::UnequipItem(ARPGCharacterInstance* InCharacterInstance, ARPGItemInstance* InItemInstance, URPGInventory* TargetInventory, URPGInventoryMappingContainer* TargetContainer, const FRPGUISlot& TargetSlot)
{
	if (!HasAuthority())
	{
		ServerUnequipItem(InCharacterInstance, InItemInstance, TargetInventory, TargetContainer, TargetSlot);
		return;
	}
	InCharacterInstance->UnequipItem(InItemInstance);
	URPGFunctionLibrary::MoveItem(InItemInstance, false, TargetInventory, TargetContainer, TargetSlot);
}

void ARPGPlayerController::ServerMoveToLocation_Implementation(AActor* RPGCharacter, const FVector& Destination)
{
	
	ARPGPawnController* PawnController = Cast<ARPGPawnController>(RPGCharacter->GetInstigatorController());
	if (PawnController)
	{
		PawnController->MoveToLocation(Destination);
	}
}

void ARPGPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	FVector2D MousePosition;
	if (!GetMousePosition(MousePosition.X, MousePosition.Y))
	{
		return;
	}
		
	// Trace to see what is under the touch location
	FHitResult HitResult;
	GetHitResultAtScreenPosition(MousePosition, ECC_Pawn, true, HitResult);
	ARPGCharacterInstance* CharacterUnderCursor = Cast<ARPGCharacterInstance>(HitResult.Actor.Get());
	if (CharacterUnderCursor != HighlightedCharacter)
	{
		if (IsValid(HighlightedCharacter))
		{
			HighlightedCharacter->SetHighlighted(false);
		}
		HighlightedCharacter = CharacterUnderCursor;
		if (IsValid(CharacterUnderCursor))
		{
			CharacterUnderCursor->SetHighlighted(true);
		}
	}
}

void ARPGPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("Interact", IE_Released, this, &ARPGPlayerController::OnInteract);
	InputComponent->BindAction("Jump", IE_Released, this, &ARPGPlayerController::OnJump);
	InputComponent->BindAction("Open Inventory", IE_Released, this, &ARPGPlayerController::OnOpenInventory);

	// support touch devices 
	InputComponent->BindAction("ResetVR", IE_Pressed, this, &ARPGPlayerController::OnResetVR);
}

void ARPGPlayerController::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ARPGPlayerController::OnInteract()
{
	// Reset UI RPG focus
	if (ARPGHUD* HUD = GetHUD<ARPGHUD>())
	{
		HUD->ResetRPGFocus();
	}
	
	// Try to select a character if one happens to lie under the player's cursor
	FVector2D MousePosition;
	if (!GetMousePosition(MousePosition.X, MousePosition.Y))
	{
		return;
	}
	
	// Trace to see what is under the touch location
	FHitResult HitResult;
	GetHitResultAtScreenPosition(MousePosition, ECC_Pawn, true, HitResult);
	if (HitResult.bBlockingHit && HitResult.Actor.IsValid())
	{
		if (ARPGCharacterInstance* ClickedCharacter = Cast<ARPGCharacterInstance>(HitResult.Actor.Get()))
		{
			SetSelectedCharacter(ClickedCharacter);

			// We selected the character – therefore no other actions should be performed
			return;
		}
	}

	// No character selected, so we try ordering an interaction to the selected character
	if (IsValid(GetSelectedCharacter()))
	{
		ServerMoveToLocation(GetSelectedCharacter(), HitResult.Location);
	}
}

void ARPGPlayerController::OnJump()
{
	static const int DebugPreview = false; 
	if (IsValid(GetSelectedCharacter()))
	{
		FVector2D MousePosition;
		if (!GetMousePosition(MousePosition.X, MousePosition.Y))
		{
			return;
		}
		
		// Trace to see what is under the touch location
		FHitResult HitResult;
		GetHitResultAtScreenPosition(MousePosition, ECC_Visibility, true, HitResult);
		if (HitResult.bBlockingHit && HitResult.Actor.IsValid())
		{
			if (!SplinePathActorClass)
			{
				UE_LOG(LogTemp, Warning, TEXT("SplinePathActorClass not set in Player Controller"));
				return;
			}
			
			const FVector SelectedActorLocation = GetSelectedCharacter()->GetActorLocation();
			const FVector MeshOffset = GetSelectedCharacter()->GetMesh()->GetRelativeLocation();
	
			// Try calculating a number of trajectories for the jump for adaptive jump height
			const float CollisionCheckTolerance = 25.0f;
			const float ArcMin = 0.3;
			const float ArcMax = 0.6;
			const int ArcSteps = 10;
			for (int i = 0; i < ArcSteps; i++)
			{
				FVector LaunchVelocity;
				const FVector StartPos = SelectedActorLocation + MeshOffset;
				const FVector EndPos = HitResult.Location;
				const float Arc = FMath::Lerp(ArcMax, ArcMin, static_cast<float>(i) / FMath::Max(1, (ArcSteps - 1)));
				const bool bSolutionExists = UGameplayStatics::SuggestProjectileVelocity_CustomArc(this, LaunchVelocity, StartPos, EndPos, 0.f, Arc);
				if (bSolutionExists)
				{
					FPredictProjectilePathParams Params = FPredictProjectilePathParams(0.f, StartPos, LaunchVelocity, 10.f, ECollisionChannel::ECC_Visibility, GetSelectedCharacter());
					if (DebugPreview)
					{
						Params.DrawDebugType = EDrawDebugTrace::ForDuration;
						Params.DrawDebugTime = 1.f;
					}
					Params.ActorsToIgnore = { GetSelectedCharacter() };
					FPredictProjectilePathResult Result;
					const bool bTraceCollisionHappened = UGameplayStatics::PredictProjectilePath(this, Params, Result);
					if (bTraceCollisionHappened)
					{
						const float DistanceToDestination = (Result.HitResult.ImpactPoint - EndPos).Size();
						if (DistanceToDestination > CollisionCheckTolerance)
						{
							continue;
						}
					}
	
					// Check for collision along the predicted path
					bool bSweepCollisionHappened = CheckCapsuleJumpCollision(GetSelectedCharacter(), Result.PathData, CollisionCheckTolerance);
					if (!bSweepCollisionHappened)
					{
						if (DebugPreview)
						{
							PreviewJumpTrajectory(Result.PathData);
						}
						JumpMove(GetSelectedCharacter(), LaunchVelocity, EndPos);
						break;
					}
				}
			}
		}
	}
}

void ARPGPlayerController::OnOpenInventory()
{
	if (ARPGHUD* HUD = GetHUD<ARPGHUD>())
	{
		HUD->ToggleInventoryScreen();
	}
}

void ARPGPlayerController::JumpMove(ACharacter* JumpCharacter, const FVector& JumpVelocity, const FVector& JumpDest)
{
	if (!HasAuthority())
	{
		ServerJumpMove(JumpCharacter, JumpVelocity, JumpDest);
		return;
	}

	const FVector StartPos = JumpCharacter->GetActorLocation();
	const FVector EndPos = { JumpDest.X, JumpDest.Y, StartPos.Z };
	const FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(StartPos, EndPos);

	if (IsValid(RotateCharacterTask) && !RotateCharacterTask->IsFinished())
	{
		RotateCharacterTask->Interrupt();
		RotateCharacterTask->EndAction();
	}
	RotateCharacterTask = URotateCharacter::RotateCharacter(GetWorld(), JumpCharacter, TargetRotation);
	RotateCharacterTask->OnFinishedNative.AddUObject(this, &ARPGPlayerController::PerformLaunch, JumpCharacter, JumpVelocity, JumpDest);
}

void ARPGPlayerController::ServerJumpMove_Implementation(ACharacter* JumpCharacter, const FVector& JumpVelocity, const FVector& JumpDest)
{
	JumpMove(JumpCharacter, JumpVelocity, JumpDest);
	RotateCharacterTask->EndAction();
}

void ARPGPlayerController::PerformLaunch(ACharacter* JumpCharacter, const FVector JumpVelocity, const FVector JumpDest)
{
	if (IsValid(JumpCharacter))
	{
		JumpCharacter->LaunchCharacter(JumpVelocity, true, true);
		if (IsValid(RotateCharacterTask))
		{
			RotateCharacterTask->EndAction();
		}
	}
}

bool ARPGPlayerController::CheckCapsuleJumpCollision(ACharacter* JumpingCharacter, const TArray<FPredictProjectilePathPointData>& JumpTrajectory, const float CollisionCheckTolerance) const
{
	static const int DebugPreview = false; 
	UCapsuleComponent* Capsule = JumpingCharacter->GetCapsuleComponent();
	for (int j = 1; j < JumpTrajectory.Num(); j++)
	{
		FHitResult ChannelTraceHit;
		const FVector CapsuleOffset = FVector::UpVector * 5.f;
		const FVector TraceStart = JumpTrajectory[j - 1].Location + FVector::UpVector * Capsule->GetScaledCapsuleHalfHeight() + CapsuleOffset;
		const FVector TraceEnd = JumpTrajectory[j].Location + FVector::UpVector * Capsule->GetScaledCapsuleHalfHeight() + CapsuleOffset;
		const bool bCollisionHappened = UKismetSystemLibrary::CapsuleTraceSingle(
			this,
			TraceStart,
			TraceEnd,
			Capsule->GetScaledCapsuleRadius(),
			Capsule->GetScaledCapsuleHalfHeight(),
			UEngineTypes::ConvertToTraceType(ECC_Visibility),
			false,
			{ JumpingCharacter },
			DebugPreview ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
			ChannelTraceHit,
			false,
			FLinearColor::Red,
			FLinearColor::Green,
			1.f);
		if (bCollisionHappened)
		{
			const float DistanceToDestination = (ChannelTraceHit.ImpactPoint - JumpTrajectory[JumpTrajectory.Num() - 1].Location).Size();
			if (DistanceToDestination > CollisionCheckTolerance)
			{
				return true;
			}
		}
	}
	return false;
}

void ARPGPlayerController::PreviewJumpTrajectory(const TArray<FPredictProjectilePathPointData>& JumpTrajectory)
{
	const FVector SelectedActorLocation = GetSelectedCharacter()->GetActorLocation();
	
	SplinePathActor = GetWorld()->SpawnActor(SplinePathActorClass, &SelectedActorLocation);
	USplineComponent* Spline = SplinePathActor->FindComponentByClass<USplineComponent>();
	if (!Spline)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spline component not found in SplinePathActor_BP"));
		return;
	}
						
	// Calculate spline points' handle lengths to be roughly the distance between control points
	float CurveLength = 0.f;
	for (int j = 1; j < JumpTrajectory.Num(); j++)
	{
		CurveLength += (JumpTrajectory[j].Location - JumpTrajectory[j - 1].Location).Size();
	}
	const float HandleLength = CurveLength / FMath::Max(1.f, static_cast<float>(JumpTrajectory.Num() - 1));

	// Create spline points
	Spline->ClearSplinePoints();
	for (int j = 0; j < JumpTrajectory.Num(); j++)
	{
		Spline->AddPoint(
		{
			static_cast<float>(j),
			JumpTrajectory[j].Location - SelectedActorLocation,
			JumpTrajectory[j].Velocity.GetClampedToMaxSize(HandleLength),
			JumpTrajectory[j].Velocity.GetClampedToMaxSize(HandleLength)
		});
	}
	
	ASplinePath* SpawnedSpline = Cast<ASplinePath>(SplinePathActor);
	if (IsValid(SpawnedSpline))
	{
		SpawnedSpline->UpdateSplineMesh();
	}
}
