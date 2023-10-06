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

void UInGameUIWidget::LevelSuccess()
{
	// 완주 성공

}

void UInGameUIWidget::LevelFailed()
{
	// 완주 실패

}

void UInGameUIWidget::LevelClearCheck()
{
	// 완주 성공 실패 체크
	// 성공시 bLevelClearCheck = true
	// 실패시 bLevelClearCheck = false
	
}
