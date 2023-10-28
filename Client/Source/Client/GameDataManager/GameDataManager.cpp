#include <fstream>
#include <sstream>
#include <filesystem>
#include <string>
#include "GameDataManager.h"

bool GameDataManager::CheckDataFile()
{
	std::string sourceFilePath{ TCHAR_TO_ANSI(*FPaths::GameSourceDir()) };
	std::string launchFilePath{ TCHAR_TO_ANSI(*FPaths::LaunchDir()) };
	sourceFilePath += EXTERNAL_SHOPDATAFILE;

	SHOPDATAFILE = launchFilePath + INTERNAL_SHOPDATAFILE;
	ITEMDATAFILE = launchFilePath + INTERNAL_ITEMDATAFILE;

	if (GetFileAttributesA(sourceFilePath.c_str()) != INVALID_FILE_ATTRIBUTES) {
	
		std::ifstream source(sourceFilePath, std::ios::binary);
		std::stringstream ss;
		if (source)	ss << source.rdbuf();
		else UE_LOG(LogTemp, Error, TEXT("%s doesn't Exist"), sourceFilePath.c_str());
		source.close();
		std::filesystem::path dirPath = (launchFilePath + INTERNAL_DATADIR);
		if (!std::filesystem::is_directory(dirPath)) std::filesystem::create_directories(dirPath);
		std::ofstream destination(SHOPDATAFILE,std::ios::binary);
		if (destination) destination << ss.str();
		else UE_LOG(LogTemp, Error, TEXT("%s doesn't Exist"), SHOPDATAFILE.c_str());
		destination.close();
	}


	//-----------------------------------
	sourceFilePath = TCHAR_TO_ANSI(*FPaths::GameSourceDir());
	sourceFilePath += EXTERNAL_ITEMDATAFILE;

	if (GetFileAttributesA(sourceFilePath.c_str()) != INVALID_FILE_ATTRIBUTES) {

		std::ifstream source(sourceFilePath, std::ios::binary);
		std::stringstream ss;
		if (source)	ss << source.rdbuf();
		else UE_LOG(LogTemp, Error, TEXT("%s doesn't Exist"), sourceFilePath.c_str());
		source.close();
		std::filesystem::path dirPath = launchFilePath + INTERNAL_DATADIR;
		if (!std::filesystem::is_directory(dirPath)) std::filesystem::create_directories(dirPath);
		std::ofstream destination(ITEMDATAFILE, std::ios::binary);
		if (destination) destination << ss.str();
		else UE_LOG(LogTemp, Error, TEXT("%s doesn't Exist"), ITEMDATAFILE.c_str());
		destination.close();
	}


	return true;
}

bool GameDataManager::LoadShopData()
{
	try {
		tinyxml2::XMLDocument doc;

		//if (doc.LoadFile(SHOPDATAFILE.c_str()) != tinyxml2::XMLError::XML_SUCCESS) return false;

		std::string filepath{ TCHAR_TO_ANSI(*FPaths::GameSourceDir()) };
		filepath += "../../GameData/ShopData.xml";

		doc.LoadFile(filepath.c_str());

		tinyxml2::XMLElement* pRoot = doc.RootElement();
		tinyxml2::XMLElement* pProductList = pRoot->FirstChildElement("productlist");

		for (tinyxml2::XMLElement* pProduct = pProductList->FirstChildElement("product")
			; pProduct != NULL; pProduct = pProduct->NextSiblingElement()) {

			Item item;
			item.itemCode = stoi(pProduct->FirstChildElement("code")->GetText());
			item.name = pProduct->FirstChildElement("name")->GetText();
			item.price = stoi(pProduct->FirstChildElement("price")->GetText());

			ShopProductsList[item.itemCode] = item;
		}
		return true;
	}
	catch(exception ex){ 
		return false;
	}
}

bool GameDataManager::LoadItemData()
{
	try {
		tinyxml2::XMLDocument doc;

		std::string filepath{ TCHAR_TO_ANSI(*FPaths::GameSourceDir()) };
		filepath += "../../GameData/ItemData.xml";

		doc.LoadFile(filepath.c_str());

		tinyxml2::XMLElement* pRoot = doc.RootElement();
		tinyxml2::XMLElement* pItemList = pRoot->FirstChildElement("itemlist");

		for (tinyxml2::XMLElement* pItem = pItemList->FirstChildElement("item")
			; pItem != NULL; pItem = pItem->NextSiblingElement()) {

			Item item;
			item.itemCode = stoi(pItem->FirstChildElement("code")->GetText());
			item.name = pItem->FirstChildElement("name")->GetText();
			item.price = stoi(pItem->FirstChildElement("price")->GetText());

			ShopProductsList[item.itemCode] = item;
		}
		return true;
	}
	catch (exception ex) {
		return false;
	}
}