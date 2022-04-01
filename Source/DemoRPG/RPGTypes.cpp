// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGTypes.h"

bool FRPGUISlot::operator==(const FRPGUISlot& Other) const
{
	return X == Other.X && Y == Other.Y;
}

bool FRPGUISlot::operator!=(const FRPGUISlot& Other) const
{
	return !(*this == Other);
}
