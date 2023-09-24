// Fill out your copyright notice in the Description page of Project Settings.


#include "LoginUIWidget.h"
#include "Components/Button.h"

void ULoginUIWidget::NativePreConstruct()
{
	//LoginButton->OnClicked.AddDynamic(this, &ULoginUIWidget::TryLogin);
}

void ULoginUIWidget::NativeDestruct()
{
}

void ULoginUIWidget::TryLogin()
{

}
