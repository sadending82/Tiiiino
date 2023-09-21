#pragma once
#include <string>
#include "../../Network/Network.h"
#include "../Object.h"

enum class eItemFlags : uint64_t
{

};

class Player : public Object, public Network
{
public:
    Player();
    ~Player();

    bool CanMakeID();
    int GetSocketID() const { return mSocketID; }
    void SetSocketID(int id) { mSocketID = id; }
protected:
    std::wstring mNickName;
    float mDepartment;
    eItemFlags mItem;
    int mSocketID;
};

