// Copyright Epic Games, Inc. All Rights Reserved.

#include "ClientGameMode.h"
#include "Data/ItemData.h"
#include "Network/Network.h"
#include "GameDataManager/GameDataManager.h"
#include "Engine/DataTable.h"
#include "Actor/Accessory/AccessoryItem.h"
#include "Global.h"


AClientGameMode::AClientGameMode()
{
}

FItemData* AClientGameMode::GetItemData(const int64& ItemCode) const
{
	return ItemData->FindRow<FItemData>(*FString::FromInt(ItemCode), TEXT(""));
}

FItemData* AClientGameMode::GetShopProductData(const int64& ItemCode) const
{
	return ShopData->FindRow<FItemData>(*FString::FromInt(ItemCode), TEXT(""));
}

void AClientGameMode::SetItemDataTable()
{
	auto network = Network::GetNetwork();
	GameDataManager* gdManager = network->mGameDataManager;

	for (auto& data : gdManager->GetItemList())
	{
		FItemData newItemData;
		GenerateItemDataFromServer(newItemData, data.second);

		ItemData->AddRow(FName(*FString::FromInt(newItemData.ItemCode)), newItemData);
	}
}

void AClientGameMode::SetShopDataTable()
{
	auto network = Network::GetNetwork();
	GameDataManager* gdManager = network->mGameDataManager;

	for (auto& data : gdManager->GetShopProductsList())
	{
		FItemData newItemData;
		GenerateItemDataFromServer(newItemData, data.second);

		ShopData->AddRow(FName(*FString::FromInt(newItemData.ItemCode)), newItemData);
	}
}

void AClientGameMode::GetShopProductTable(TArray<int>& f_out)
{
	TArray<FItemData*> TempData;
	ShopData->GetAllRows(TEXT(""), TempData);

	for (auto data : TempData) {
		f_out.Add(data->ItemCode);
	}
}

void AClientGameMode::BeginPlay()
{
	Super::BeginPlay();

	auto Game = Network::GetNetwork();
	Game->init();

	//ItemData = NewObject<UDataTable>();

	SetItemDataTable();
	SetShopDataTable();

	GetWorld()->SpawnActor<ASoundManager>(SoundMangerClass);
	EBGMType LevelType;

	if (UGameplayStatics::GetCurrentLevelName(GetWorld()) == "Lobby")
		LevelType = EBGMType::EBGMType_Lobby;
	else
		LevelType = EBGMType::EBGMType_Race;

	ASoundManager::GetSoundManager()->SetBGM(LevelType);
	ASoundManager::GetSoundManager()->PlayBGM();
	
}

void AClientGameMode::GenerateItemDataFromServer(FItemData& OutData,const Item& data)
{
	EEquipType equipType;
	int32 ItemCode = StaticCast<int32>(data.itemCode);


	if (ItemCode < 0) return;
	else if (ItemCode >= STARTCODE_BACKEQUIP && ItemCode < STARTCODE_HANDEQUIP) equipType = EEquipType::EEquipType_Back;
	else if (ItemCode >= STARTCODE_HANDEQUIP && ItemCode < STARTCODE_FACEEQUIP) equipType = EEquipType::EEquipType_Hand;
	else if (ItemCode >= STARTCODE_FACEEQUIP && ItemCode < STARTCODE_HEADEQUIP) equipType = EEquipType::EEquipType_Face;
	else if (ItemCode >= STARTCODE_HEADEQUIP) equipType = EEquipType::EEquipType_Head;
	else return;

	//_C를 꼭 붙여야 된다고 함.
	FString BpPath = "Blueprint'/Game/Accessory/BP/BP_"
		+ data.assetName
		+ ".BP_"
		+ data.assetName
		+ "_C'";

	TSubclassOf<AActor> AccessoryBP;
	UHelpers::GetClassDynamic<AActor>(&AccessoryBP, BpPath);

	//auto GeneratedAccessoryBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *BpPath));
	//auto objac = Cast<AActor>(obj);
	//AAccessoryItem* GeneratedAccessoryBP = Cast<AAccessoryItem>(obj);

	//FString IconPath = TEXT("Texture2D'/Game/Accessory/Icon/Icon_");
	//IconPath += data.second.assetName;
	//IconPath += ".Icon_";
	//IconPath += data.second.assetName;
	//IconPath += "'";

	//UTexture2D* IconTexture = LoadObject<UTexture2D>(NULL, *IconPath, NULL, LOAD_None, NULL);

	UTexture2D* IconTexture = LoadObject<UTexture2D>(NULL, TEXT("Texture2D'/Game/Characters/Tino/Cloth/Texture/DepartmentTestTexture/Tino_A.Tino_A'"), NULL, LOAD_None, NULL);

	OutData.ItemCode = ItemCode;
	OutData.EquipType = equipType;
	OutData.TextData.Name = FText::FromString(data.name);
	OutData.TextData.Description = FText::FromString(data.text);
	OutData.NumericData.PurchaseCondition = data.cutline;
	OutData.SellValue = data.price;
	
	OutData.AssetData.BPClass = AccessoryBP;
	OutData.AssetData.Icon = IconTexture;
}
