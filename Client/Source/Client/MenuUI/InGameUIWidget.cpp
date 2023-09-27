// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuUI/InGameUIWidget.h"


void UInGameUIWidget::NativePreConstruct()
{
	RestGameTime = 20;
}

void UInGameUIWidget::NativeDestruct()
{
}

void UInGameUIWidget::RankImageSwitcher()
{

}

void UInGameUIWidget::SetGameTime(int32 GameTime)
{
	RestGameTime = GameTime;
}
