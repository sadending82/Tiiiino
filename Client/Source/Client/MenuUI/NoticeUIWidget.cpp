// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuUI/NoticeUIWidget.h"

void UNoticeUIWidget::NativeConstruct()
{

}

FString UNoticeUIWidget::GetNotice()
{
	return Notice;
}

void UNoticeUIWidget::SetNotice(FString notice)
{
	Notice = notice;
}
