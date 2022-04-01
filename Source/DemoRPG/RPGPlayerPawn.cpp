// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGPlayerPawn.h"

#include "RPGTypes.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
ARPGPlayerPawn::ARPGPlayerPawn()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Setup input defaults
	BaseTurnRate = 180.0f;
	ZoomRate = 250.0f;
	ZoomAdjustmentSpeed = 10.0f;
	HeightAdjustmentSpeed = 10.0f;
	MaxHeightOffset = 250.0f;
	MinCameraDistance = 100.0f;
	MaxCameraDistance = 2000.0f;
	TargetCameraDistance = MaxCameraDistance;

	GetCharacterMovement()->DefaultLandMovementMode = EMovementMode::MOVE_Flying;
	SetActorEnableCollision(false);
	GetCharacterMovement()->MaxAcceleration = 30000.0f;
	GetCharacterMovement()->BrakingDecelerationFlying = 10000.0f;
	GetCharacterMovement()->MaxFlySpeed = 2000.0f;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetCapsuleComponent());
	CameraBoom->TargetArmLength = MaxCameraDistance;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
}

// Called when the game starts or when spawned
void ARPGPlayerPawn::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ARPGPlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Adjust camera distance
	const float ZoomFactor = UKismetMathLibrary::FClamp(ZoomAdjustmentSpeed * GetWorld()->DeltaTimeSeconds, 0.f, 1.f);
	CameraBoom->TargetArmLength = UKismetMathLibrary::Lerp(CameraBoom->TargetArmLength, TargetCameraDistance, ZoomFactor);

	// Update Pawn's pivot point height above the landscape
	const FVector PivotPoint = GetActorLocation();
	const FVector TraceOffset = FVector::UpVector * 10000.0f;
	
	TArray<FHitResult> HitResults;
	const FVector Start = PivotPoint + TraceOffset;
	const FVector End = PivotPoint - TraceOffset;
	GetWorld()->LineTraceMultiByChannel(HitResults, Start, End, COLLISION_CAMERA_PIVOT);

	if (HitResults.Num() > 0)
	{
		HitResults.Sort([&](const FHitResult& A, const FHitResult& B)
		{
			return A.ImpactPoint.Z - PivotPoint.Z < B.ImpactPoint.Z - PivotPoint.Z;
		});

		FVector NewPivotPoint = PivotPoint;
		for (const FHitResult& Result : HitResults)
		{
			if (Result.ImpactPoint.Z - PivotPoint.Z <= MaxHeightOffset)
			{
				NewPivotPoint = Result.ImpactPoint;
			}
			else
			{
				break;
			}
		}

		const float HeightFactor = UKismetMathLibrary::FClamp(ZoomAdjustmentSpeed * GetWorld()->DeltaTimeSeconds, 0.f, 1.f);
		const FVector NewLocation = UKismetMathLibrary::VLerp(PivotPoint, NewPivotPoint, HeightFactor);
		SetActorLocation(NewLocation);
	}
}

// Called to bind functionality to input
void ARPGPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ARPGPlayerPawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ARPGPlayerPawn::MoveRight);

	PlayerInputComponent->BindAction("RotationMode", EInputEvent::IE_Pressed, this, &ARPGPlayerPawn::EnterRotationMode);
	PlayerInputComponent->BindAction("RotationMode", EInputEvent::IE_Released, this, &ARPGPlayerPawn::LeaveRotationMode);
	PlayerInputComponent->BindAxis("OrbitHorizontal", this, &ARPGPlayerPawn::OrbitHorizontal);
	PlayerInputComponent->BindAxis("OrbitVertical", this, &ARPGPlayerPawn::OrbitVertical);
	PlayerInputComponent->BindAxis("Zoom", this, &ARPGPlayerPawn::Zoom);
}

void ARPGPlayerPawn::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		AddMovementInput(GetDirection(EAxis::X), Value);
	}
}

void ARPGPlayerPawn::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		AddMovementInput(GetDirection(EAxis::Y), Value);
	}
}

void ARPGPlayerPawn::EnterRotationMode()
{
	bIsInRotationMode = true;
	APlayerController* PlayerController = GetController<APlayerController>(); 
	if (PlayerController)
	{
		PlayerController->GetMousePosition(RotationMousePosition.X, RotationMousePosition.Y);
		PlayerController->bShowMouseCursor = false;
	}
}

void ARPGPlayerPawn::LeaveRotationMode()
{
	bIsInRotationMode = false;
	APlayerController* PlayerController = GetController<APlayerController>(); 
	if (PlayerController)
	{
		PlayerController->bShowMouseCursor = true;
	}
}

void ARPGPlayerPawn::OrbitHorizontal(float Value)
{
	if (bIsInRotationMode)
	{
	APlayerController* PlayerController = GetController<APlayerController>(); 
		if (PlayerController)
		{
			const FRotator CurrentRotation = GetActorRotation();
			PlayerController->SetControlRotation({
				CurrentRotation.Pitch,
				CurrentRotation.Yaw + Value * BaseTurnRate * GetWorld()->GetDeltaSeconds(),
				CurrentRotation.Roll});
			PlayerController->SetMouseLocation(RotationMousePosition.X, RotationMousePosition.Y);
		}
	}
}

void ARPGPlayerPawn::OrbitVertical(float Value)
{
	if (bIsInRotationMode)
	{
		const float NewAngle = UKismetMathLibrary::FClamp(CameraBoom->GetRelativeRotation().Pitch + Value * BaseTurnRate * GetWorld()->GetDeltaSeconds(), -90.f, 0.f);
		CameraBoom->SetRelativeRotation({NewAngle, 0.f, 0.f});
		APlayerController* PlayerController = GetController<APlayerController>(); 
		if (PlayerController)
		{
			PlayerController->SetMouseLocation(RotationMousePosition.X, RotationMousePosition.Y);
		}
	}
}

void ARPGPlayerPawn::Zoom(float Value)
{
	TargetCameraDistance = UKismetMathLibrary::FClamp(TargetCameraDistance - Value * ZoomRate, MinCameraDistance, MaxCameraDistance);
}

void ARPGPlayerPawn::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

FVector ARPGPlayerPawn::GetDirection(EAxis::Type Axis) const
{
	// find out which way is forward
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// get movement vector
	return FRotationMatrix(YawRotation).GetUnitAxis(Axis);
}
