// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Level/CustomLevel_Lobby.h"
#include "Global.h"

void ACustomLevel_Lobby::BeginPlay()
{
	Super::BeginPlay();
	
	FInputModeUIOnly LobbyInputMode;
	auto Controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	Controller->SetInputMode(LobbyInputMode);
	Controller->bShowMouseCursor = true;
}

bool ACustomLevel_Lobby::Conn()
{
	return false;
}
