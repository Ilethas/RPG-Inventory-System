// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RPGPlayerPawn.generated.h"

class UCameraComponent;
class USpringArmComponent;
UCLASS()
class DEMORPG_API ARPGPlayerPawn : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ARPGPlayerPawn();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	/** Returns CameraBoom subobject **/
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Determines how fast the player can change the destination camera distance. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Camera)
	float ZoomRate;

	/** How fast the actual camera distance changes towards the desires distance */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Camera, Meta = (AllowPrivateAccess = "true"))
	float ZoomAdjustmentSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Camera, Meta = (AllowPrivateAccess = "true"))
	float HeightAdjustmentSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Camera, Meta = (AllowPrivateAccess = "true"))
	float MaxHeightOffset;

private:
	/**  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Camera, Meta = (AllowPrivateAccess = "true", ClampMin = "0.0", UIMin = "0.0"))
	float MinCameraDistance;

	/**  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Camera, Meta = (AllowPrivateAccess = "true", ClampMin = "0.0", UIMin = "0.0"))
	float MaxCameraDistance;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** Called when entering rotation mode. Only relevant when using a mouse */
	void EnterRotationMode();

	/** Called when leaving rotation mode. Only relevant when using a mouse */
	void LeaveRotationMode();

	/** Called when rotating horizontally using a mouse */
	void OrbitHorizontal(float Value);

	/** Called when rotating vertically using a mouse */
	void OrbitVertical(float Value);

	/** Called when zooming in/out */
	void Zoom(float Value);

	/** 
	* Called via input to turn at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);

private:
	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;
	
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> TopDownCameraComponent;

	FVector GetDirection(EAxis::Type Axis) const;

	bool bIsInRotationMode = false;
	FVector2D RotationMousePosition;
	float TargetCameraDistance;

	float CameraDistance;
	float CameraYaw;
};
