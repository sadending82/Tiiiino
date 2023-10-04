// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuUI/InGameUIWidget.h"
#include "Components/EditableTextBox.h"

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

void UInGameUIWidget::EndGameTime()
{

}
