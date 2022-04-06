// Fill out your copyright notice in the Description page of Project Settings.


#include "RotateCharacter.h"

URotateCharacter::URotateCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, WorldContextObject(nullptr)
{
	bIsCreateOnRunning = GIsRunning;
}

URotateCharacter* URotateCharacter::RotateCharacter(const UObject* WorldContextObject, ACharacter* Character, const FRotator& TargetRotation, float RotationSpeed)
{
	URotateCharacter* BlueprintNode = NewObject<URotateCharacter>();
	BlueprintNode->WorldContextObject = WorldContextObject;
	BlueprintNode->Character = Character;
	BlueprintNode->TargetRotation = TargetRotation;
	BlueprintNode->RotationSpeed = RotationSpeed;
	return BlueprintNode;
}

void URotateCharacter::Activate()
{
	
}

void URotateCharacter::Tick(float DeltaTime)
{
	if (!bFinished && !bInterrupted)
	{
		if (Character.IsValid())
		{
			const FRotator CurrentRotation = Character->GetActorRotation();
			const FRotator FinalRotation = FMath::RInterpConstantTo(CurrentRotation, TargetRotation, DeltaTime, RotationSpeed);
			if (!CurrentRotation.Equals(FinalRotation))
			{
				Character->SetActorRotation(FinalRotation);
			}
			else
			{
				OnFinishedNative.Broadcast();
				OnFinished.Broadcast();
				bFinished = true;
			}
		}
		else
		{
			bFinished = true;
		}
	}
}
 
bool URotateCharacter::IsTickable() const
{
	// Notify engine to ignore Tick of the object construction before game running.
	return bIsCreateOnRunning;
}

bool URotateCharacter::IsAllowedToTick() const
{
	return !(bFinished || bInterrupted);
}

TStatId URotateCharacter::GetStatId() const
{
	return GetStatID();
}

void URotateCharacter::Interrupt()
{
	bInterrupted = true;
	OnInterruptedNative.Broadcast();
	OnInterrupted.Broadcast();
}

void URotateCharacter::EndAction()
{
	MarkAsGarbage();
}
