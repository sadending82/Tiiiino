#pragma once
#include <string>
#include "../../Network/Network.h"
#include "../Object.h"


class Player : public Object, public Network
{
public:
    Player();
    ~Player();

    bool CanMakeID();

    int GetRoomID() const { return mRoomID; }
    void SetRoomID(int roomID) { mRoomID = roomID; }
protected:
    std::wstring mNickName;
    float mDepartment;
    eItemFlags mItem;
    int mRoomID;
};

