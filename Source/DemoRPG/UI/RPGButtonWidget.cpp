// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGButtonWidget.h"

void URPGButtonWidget::BroadcastClick() const
{
	OnClickedNative.Broadcast();
	OnClicked.Broadcast();
}

void URPGButtonWidget::BroadcastPress() const
{
	OnPressedNative.Broadcast();
	OnPressed.Broadcast();
}

void URPGButtonWidget::BroadcastRelease() const
{
	OnReleasedNative.Broadcast();
	OnReleased.Broadcast();
}

void URPGButtonWidget::BroadcastHover() const
{
	OnHoveredNative.Broadcast();
	OnHovered.Broadcast();
}

void URPGButtonWidget::BroadcastUnhover() const
{
	OnUnhoveredNative.Broadcast();
	OnUnhovered.Broadcast();
}
