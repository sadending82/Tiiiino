// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuUI/DialogUIWidget.h"
#include "Actor/Controller/TinoController.h"

#include "Global.h"

void UDialogUIWidget::NativePreConstruct()
{
}

void UDialogUIWidget::NativeDestruct()
{
}

void UDialogUIWidget::DisconnectNetwork()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, false);
}
