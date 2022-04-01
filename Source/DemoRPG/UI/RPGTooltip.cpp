#include "RPGTooltip.h"
#include "TimerManager.h"

void URPGTooltip::ShowTooltip()
{
	const APlayerController* Controller = GetOwningPlayer();
	if (IsValid(Controller))
	{
		FTimerManager& Manager = GetOwningPlayer()->GetWorldTimerManager();
		Manager.ClearTimer(ShowTooltipHandle);
		if (TooltipDelay <= 0.0f)
		{
			OnShowTooltip();
		}
		else
		{
			Manager.SetTimer(ShowTooltipHandle, this, &URPGTooltip::OnShowTooltip, TooltipDelay);
		}
	}
}

void URPGTooltip::HideTooltip()
{
	const APlayerController* Controller = GetOwningPlayer();
	if (IsValid(Controller))
	{
		FTimerManager& Manager = GetOwningPlayer()->GetWorldTimerManager();
		Manager.ClearTimer(ShowTooltipHandle);
		OnHideTooltip();
	}
}
