// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuUI/DialogUIWidget.h"
#include "Actor/Controller/TinoController.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Network/Network.h"

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
	FailedButton->SetRenderOpacity(0.0);
	FailedButtonText->SetRenderOpacity(0.0);
	FailedImage->SetRenderOpacity(0.0);
	FailedText->SetRenderOpacity(0.0);

	InGameMenuButton->SetVisibility(ESlateVisibility::HitTestInvisible);
	FailedButton->SetVisibility(ESlateVisibility::HitTestInvisible);
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
	Network::GetNetwork()->bIsConnected = false;
	Network::GetNetwork()->bLevelOpenTriggerEnabled = true;
	UGameplayStatics::OpenLevel(GetWorld(), FName("Lobby"));
	// 임시로 게임종료하도록 설정
	//UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, false);

}

void UDialogUIWidget::RenderUIAlertMessage()
{
	FailedButton->SetRenderOpacity(1.0);
	FailedButtonText->SetRenderOpacity(1.0);
	FailedImage->SetRenderOpacity(1.0);
	FailedText->SetRenderOpacity(1.0);
	FailedButton->SetVisibility(ESlateVisibility::Visible);
}

void UDialogUIWidget::SetLoginFailedUI()
{
	FString text = L"아이디 혹은 비밀번호가 존재하지 않는 계정입니다.";
	FText CurrentText = FText::FromString(text);
	FailedText->SetText(CurrentText);
}

void UDialogUIWidget::SetCreateAccountSuccessUI()
{
	FString text = L"회원가입에 성공했습니다.";
	FText CurrentText = FText::FromString(text);
	FailedText->SetText(CurrentText);
}

void UDialogUIWidget::SetCreateAccountFailedUI()
{
	FString text = L"회원가입에 실패했습니다.";
	FText CurrentText = FText::FromString(text);
	FailedText->SetText(CurrentText);
}

void UDialogUIWidget::FixedAlertMessage()
{
	FString text = L"";
	FText CurrentText = FText::FromString(text);
	FailedText->SetText(CurrentText);
}
