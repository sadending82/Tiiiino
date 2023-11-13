// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuUI/LobbyUIWidget.h"
#include "MenuUI/LoginUIWidget.h"
#include "MenuUI/StoreUIWidget.h"
#include "MenuUI/InventoryUIWidget.h"
#include "CouponUIWidget.h"
#include "Components/TextBlock.h"
#include "Actor/Controller/TinoController.h"
#include "Actor/Character/TinoCharacter.h"
#include "Network/Network.h"
#include "Global.h"

void ULobbyUIWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	//Grade = 0.f;
}

void ULobbyUIWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
}

void ULobbyUIWidget::NativeDestruct()
{
	Super::NativeDestruct();
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

void ULobbyUIWidget::UpdatePointAndGrade()
{
	auto TinoController = Cast<ATinoController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (!!TinoController)
	{
		auto TinoCharacter = TinoController->GetPawn<ATinoCharacter>();
		if (!!TinoCharacter)
		{
			Point = TinoCharacter->GetPoint();
			Grade = TinoCharacter->GetGrade();
		}
	}
}

void ULobbyUIWidget::TryBack()
{
	auto TinoController = Cast<ATinoController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	send_matchout_packet(Network::GetNetwork()->l_socket);
	send_logout_packet(Network::GetNetwork()->l_socket);
	TinoController->ChangeMenuWidget(TinoController->LoginUIInstance);
}

void ULobbyUIWidget::TryEnterStore()
{
	auto TinoController = Cast<ATinoController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	//TinoController->StoreUIInstance->UpdatePointAndPoint();
	TinoController->ChangeMenuWidget(TinoController->StoreUIInstance);
}

void ULobbyUIWidget::TryEnterInventory()
{
	auto TinoController = Cast<ATinoController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	//TinoController->InventoryUIInstance->UpdatePointAndPoint();
	TinoController->ChangeMenuWidget(TinoController->InventoryUIInstance);
}

void ULobbyUIWidget::TryOpenCouponUI()
{
	auto TinoController = Cast<ATinoController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	TinoController->CouponUIInstance->AddToViewport();
}

void ULobbyUIWidget::GetRankData(rankInfo rank[])
{
	// 서버에서 보내준 랭크데이터 받는 함수
	for (int i = 0; i < RANK_COUNT; ++i)
	{
		if (rank != nullptr)
		{
			DepartmentNameArray[i] = int(rank[i].department);
			DepartmentPointArray[i] = rank[i].point;
		}
	}
}

