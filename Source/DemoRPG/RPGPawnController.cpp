// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGPawnController.h"

#include "Navigation/CrowdFollowingComponent.h"

ARPGPawnController::ARPGPawnController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
}
