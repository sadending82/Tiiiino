// Copyright Epic Games, Inc. All Rights Reserved.

#include "ClientGameMode.h"
#include "ClientCharacter.h"
#include "Data/ItemData.h"
#include "Network/Network.h"
#include "GameDataManager/GameDataManager.h"
#include "Engine/DataTable.h"
#include "Actor/Accessory/AccessoryItem.h"
#include "Global.h"


AClientGameMode::AClientGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	//ASoundManager::GetSoundManager()->PlayBGM();
}

FItemData* AClientGameMode::GetItemData(const int64& ItemCode) const
{
	return ItemData->FindRow<FItemData>(*FString::FromInt(ItemCode), TEXT(""));
}

void AClientGameMode::SetItemDataTable()
{
	auto network = Network::GetNetwork();
	GameDataManager* gdManager = network->mGameDataManager;

	for (auto& data : gdManager->GetItemList())
	{
		EEquipType equipType;

		if (data.second.itemCode < 0) continue;
		else if (data.second.itemCode >= STARTCODE_BACKEQUIP && data.second.itemCode < STARTCODE_HANDEQUIP) equipType = EEquipType::EEquipType_Back;
		else if (data.second.itemCode >= STARTCODE_HANDEQUIP && data.second.itemCode < STARTCODE_FACEEQUIP) equipType = EEquipType::EEquipType_Hand;
		else if (data.second.itemCode >= STARTCODE_FACEEQUIP && data.second.itemCode < STARTCODE_HEADEQUIP) equipType = EEquipType::EEquipType_Face;
		else if (data.second.itemCode < STARTCODE_HEADEQUIP) equipType = EEquipType::EEquipType_Head;
		else continue;


		FItemData newItemData(data.second.itemCode, equipType, FText::FromString(data.second.name), FText::FromString(data.second.text), data.second.cutline, data.second.price);

		FString BpPath = TEXT("Blueprint'/Game/Accessory/BP/BP_"); //_C를 꼭 붙여야 된다고 함.
		BpPath += data.second.assetName;
		BpPath += ".BP_";
		BpPath += data.second.assetName;
		BpPath += "_C'";

		AAccessoryItem* GeneratedAccessoryBP = Cast<AAccessoryItem>(StaticLoadObject(UClass::StaticClass(), NULL, *BpPath));
		
		//FString IconPath = TEXT("Texture2D'/Game/Accessory/Icon/Icon_");
		//IconPath += data.second.assetName;
		//IconPath += ".Icon_";
		//IconPath += data.second.assetName;
		//IconPath += "'";

		//UTexture2D* IconTexture = LoadObject<UTexture2D>(NULL, *IconPath, NULL, LOAD_None, NULL);

		UTexture2D* IconTexture = LoadObject<UTexture2D>(NULL, TEXT("Texture2D'/Game/Characters/Tino/Cloth/Texture/DepartmentTestTexture/Tino_A.Tino_A'"), NULL, LOAD_None, NULL);

		newItemData.AssetData.BPClass = GeneratedAccessoryBP->StaticClass();
		newItemData.AssetData.Icon = IconTexture;

		ItemData->AddRow(FName(*data.second.assetName), newItemData);
	}
}

void AClientGameMode::BeginPlay()
{
	Super::BeginPlay();

	auto Game = Network::GetNetwork();
	Game->init();

	ItemData = NewObject<UDataTable>();
	
	SetItemDataTable();

	GetWorld()->SpawnActor<ASoundManager>(SoundMangerClass);
	EBGMType LevelType;

	if (UGameplayStatics::GetCurrentLevelName(GetWorld()) == "Lobby")
		LevelType = EBGMType::EBGMType_Lobby;
	else
		LevelType = EBGMType::EBGMType_Race;

	ASoundManager::GetSoundManager()->SetBGM(LevelType);
	ASoundManager::GetSoundManager()->PlayBGM();
	
	
}
