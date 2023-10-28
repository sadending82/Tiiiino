#include "GameDataManager.h"

bool GameDataManager::CheckDataFile()
{
	SHOPDATAFILE = *FPaths::GameSourceDir() + EXTERNAL_SHOPDATAFILE;
	ITEMDATAFILE = EXTERNAL_ITEMDATAFILE;
	/*if (GetFileAttributesA(EXTERNAL_SHOPDATAFILE) == INVALID_FILE_ATTRIBUTES) {
		SHOPDATAFILE = INTERNAL_SHOPDATAFILE;
	}
	else {
		std::FILE* source = std::fopen(EXTERNAL_SHOPDATAFILE, "rb");
		std::FILE* destination = std::fopen(INTERNAL_SHOPDATAFILE, "wb");
		char buffer[1024];
		std::size_t bytesRead;
		while ((bytesRead = std::fread(buffer, 1, sizeof(buffer), source)) > 0) {
			std::fwrite(buffer, 1, bytesRead, destination);
		}
		std::fclose(destination);
		std::fclose(source);

		SHOPDATAFILE = EXTERNAL_SHOPDATAFILE;
	}

	if (GetFileAttributesA(EXTERNAL_ITEMDATAFILE) == INVALID_FILE_ATTRIBUTES) {
		ITEMDATAFILE = INTERNAL_ITEMDATAFILE;
	}
	else {
		std::FILE* source = std::fopen(EXTERNAL_ITEMDATAFILE, "rb");
		std::FILE* destination = std::fopen(INTERNAL_ITEMDATAFILE, "wb");
		char buffer[1024];
		std::size_t bytesRead;
		while ((bytesRead = std::fread(buffer, 1, sizeof(buffer), source)) > 0) {
			std::fwrite(buffer, 1, bytesRead, destination);
		}
		std::fclose(destination);
		std::fclose(source);

		ITEMDATAFILE = EXTERNAL_ITEMDATAFILE;
	}*/

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