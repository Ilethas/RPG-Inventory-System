// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGItemTooltip.h"

#include "Components/TextBlock.h"
#include "DemoRPG/Items/RPGItemInstance.h"

void URPGItemTooltip::SetItemInstance(ARPGItemInstance* NewItemInstance)
{
	if (IsValid(ItemInstance))
	{
		ItemInstance->OnItemInstanceChangedNative.RemoveAll(this);
		ItemInstance->OnItemInstanceChanged.RemoveAll(this);
	}
	ItemInstance = NewItemInstance;
	if (IsValid(NewItemInstance))
	{
		ItemInstance->OnItemInstanceChangedNative.AddUObject(this, &URPGItemTooltip::RefreshWidget);
		
	}
	RefreshWidget();
}
