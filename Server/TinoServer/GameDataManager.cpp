#include "GameDataManager.h"

bool GameDataManager::LoadShopData()
{
	try {
		tinyxml2::XMLDocument doc;

		doc.LoadFile(SHOPDATAFILE);
		doc.SetBOM(true);

		tinyxml2::XMLElement* pRoot = doc.RootElement();
		tinyxml2::XMLElement* pProductList = pRoot->FirstChildElement("productlist");

		for (tinyxml2::XMLElement* pProduct = pProductList->FirstChildElement("product")
			; pProduct != NULL; pProduct = pProduct->NextSiblingElement()) {

			Item item;
			item.itemCode = stoi(pProduct->FirstChildElement("code")->GetText());
			item.price = stoi(pProduct->FirstChildElement("price")->GetText());
			item.cutline = stoi(pProduct->FirstChildElement("cutline")->GetText());

			const char* uft8Name = pProduct->FirstChildElement("name")->GetText();
			wstring unicodeName;
			int nLen = MultiByteToWideChar(CP_UTF8, 0, uft8Name, strlen(uft8Name), NULL, NULL);
			MultiByteToWideChar(CP_UTF8, 0, uft8Name, strlen(uft8Name), &unicodeName[0], nLen);
			wcout << unicodeName << endl;
			//item.name = wstring(unicodeName);

			ShopProductsList[item.itemCode] = item;
		}
		return true;
	}
	catch(exception ex){ 
		DEBUGMSGNOPARAM("Load Shop Data Failed\n");
		return false;
	}
}

bool GameDataManager::LoadItemData()
{
	try {
		tinyxml2::XMLDocument doc;

		doc.LoadFile(ITEMDATAFILE);
		doc.SetBOM(true);

		tinyxml2::XMLElement* pRoot = doc.RootElement();
		tinyxml2::XMLElement* pItemList = pRoot->FirstChildElement("itemlist");

		for (tinyxml2::XMLElement* pItem = pItemList->FirstChildElement("item")
			; pItem != NULL; pItem = pItem->NextSiblingElement()) {

			Item item;
			item.itemCode = stoi(pItem->FirstChildElement("code")->GetText());
			item.price = stoi(pItem->FirstChildElement("price")->GetText());

			const char* uft8Name = pItem->FirstChildElement("name")->GetText();
			wchar_t unicodeName[256] = { 0, };
			int nLen = MultiByteToWideChar(CP_UTF8, 0, uft8Name, strlen(uft8Name), NULL, NULL);
			MultiByteToWideChar(CP_UTF8, 0, uft8Name, strlen(uft8Name), unicodeName, nLen);
			item.name = wstring(unicodeName);

			ItemList[item.itemCode] = item;
		}
		return true;
	}
	catch (exception ex) {
		DEBUGMSGNOPARAM("Load Item Data Failed\n");
		return false;
	}
}