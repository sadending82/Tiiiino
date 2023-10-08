// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuUI/DialogUIWidget.h"
#include "Actor/Controller/TinoController.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

#include "Global.h"

void UDialogUIWidget::NativePreConstruct()
{
}

void UDialogUIWidget::NativeDestruct()
{
}

void UDialogUIWidget::ResetWindow()
{
	DisconnectNetworkButton->SetRenderOpacity(0.0);
	DisconnectNetworkText->SetRenderOpacity(0.0);
	DisconnectNetworkButtonText->SetRenderOpacity(0.0);
	DisconnectNetworkImage->SetRenderOpacity(0.0);
	InGameMenuButton->SetRenderOpacity(0.0);
	InGameMenuButtonText->SetRenderOpacity(0.0);
	InGameMenuImage->SetRenderOpacity(0.0);
	LoginFailedButton->SetRenderOpacity(0.0);
	LoginFailedButtonText->SetRenderOpacity(0.0);
	LoginFailedImage->SetRenderOpacity(0.0);
	LoginFailedText->SetRenderOpacity(0.0);

	InGameMenuButton->SetVisibility(ESlateVisibility::HitTestInvisible);
	LoginFailedButton->SetVisibility(ESlateVisibility::HitTestInvisible);
	DisconnectNetworkButton->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UDialogUIWidget::DisconnectNetwork()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, false);
}

void UDialogUIWidget::RenderDisconnectNetworkWindow()
{
	DisconnectNetworkButton->SetRenderOpacity(1.0);
	DisconnectNetworkText->SetRenderOpacity(1.0);
	DisconnectNetworkButtonText->SetRenderOpacity(1.0);
	DisconnectNetworkImage->SetRenderOpacity(1.0);
	DisconnectNetworkButton->SetVisibility(ESlateVisibility::Visible);
}

void UDialogUIWidget::RenderInGameMenuUI()
{
	InGameMenuButton->SetRenderOpacity(1.0);
	InGameMenuButtonText->SetRenderOpacity(1.0);
	InGameMenuImage->SetRenderOpacity(1.0);
	InGameMenuButton->SetVisibility(ESlateVisibility::Visible);

}

void UDialogUIWidget::DisconnectGame()
{
	// 로비로 돌아가는 기능

	// 임시로 게임종료하도록 설정
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, false);

}

void UDialogUIWidget::RenderLoginFailedUI()
{
	LoginFailedButton->SetRenderOpacity(1.0);
	LoginFailedButtonText->SetRenderOpacity(1.0);
	LoginFailedImage->SetRenderOpacity(1.0);
	LoginFailedText->SetRenderOpacity(1.0);
	LoginFailedButton->SetVisibility(ESlateVisibility::Visible);
}
