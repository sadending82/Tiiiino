// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuUI/InGameTimerWidget.h"
#include "Actor/Controller/TinoController.h"
#include "Actor/Character/TinoCharacter.h"
#include "MenuUI/InGameUIWidget.h"
#include "Network/Network.h"

#include "Global.h"

void UInGameTimerWidget::NativePreConstruct()
{
}

void UInGameTimerWidget::NativeDestruct()
{
}

void UInGameTimerWidget::TimerStart(ETimerType Type)
{
	auto TinoController = Cast<ATinoController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (!!TinoController)
	{
		auto TinoCharacter = TinoController->GetPawn<ATinoCharacter>();
		if (!!TinoCharacter)
		{
			InGameUI = Network::GetNetwork()->mMyCharacter->InGameWidgetInstance;
			if (Type == ETimerType::ETT_LevelClear)
			{
				RestGameTime = 19;
			}
			else if (Type == ETimerType::ETT_LevelStart)
			{
				RestGameTime = 3;
			}
			TinoCharacter->TimerStart(Type);
			
		}
	}
}
	

void UInGameTimerWidget::TimerRun(ETimerType Type)
{
	if (Type == ETimerType::ETT_LevelClear)
	{
		if (!!InGameUI)
			InGameUI->TimerTextChange(RestGameTime, Type);
	}
	else if (Type == ETimerType::ETT_LevelStart)
	{
		if (!!InGameUI)
			InGameUI->TimerTextChange(RestGameTime, Type);
	}
	RestGameTime--;
	// if Timer End
	//TimerEnd(Type);
}

void UInGameTimerWidget::TimerEnd(ETimerType Type)
{
	// reset Timer
	RestGameTime = 0;
}
