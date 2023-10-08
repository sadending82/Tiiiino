// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyUIWidget.h"
#include "Actor/Controller/TinoController.h"
#include "Network/Network.h"
#include "Global.h"

void ULobbyUIWidget::NativePreConstruct()
{
}

void ULobbyUIWidget::NativeDestruct()
{
}

void ULobbyUIWidget::TryGameStart()
{
	// If Ready Check
	// Game Start
	send_match_packet(Network::GetNetwork()->l_socket);

}

void ULobbyUIWidget::TryGameReady()
{
	// If not ready
	// GameStart Button Onclicked
}

void ULobbyUIWidget::TryGameReadyCancel()
{
	// If ready
	send_matchout_packet(Network::GetNetwork()->l_socket);
	// GameStart Button Onclicked

}

void ULobbyUIWidget::TryBack()
{
	auto TinoController = Cast<ATinoController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	send_matchout_packet(Network::GetNetwork()->l_socket);
	TinoController->ChangeMenuWidget(TinoController->GetStartingWidgetClass());
}

void ULobbyUIWidget::DisconnectNetwork()
{
	auto TinoController = Cast<ATinoController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	TinoController->DisconnectNetwork();
}
