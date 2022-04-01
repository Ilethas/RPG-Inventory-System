#include "RPGItemFeature.h"

void URPGItemFeature::Initialize()
{
	if (!bInitialized)
	{
		OnInitialized();
		bInitialized = true;
	}
}

void URPGItemFeature::OnItemInstanceConstructed(ARPGItemInstance* ItemInstance)
{
	OnItemInstanceConstructed_BP(ItemInstance);
}

void URPGItemFeature::OnInitialized()
{
	OnInitialized_BP();
}

void URPGItemFeature::OnRemoved()
{
	OnRemoved_BP();
}
