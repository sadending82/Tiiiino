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

    int GetRank() const { return mRank; }
    void SetRank(int rank) { mRank = rank; }

    bool IsPlayerArrived();
    bool CanPlayerArrive();
protected:
    //반드시 락을 불러주는 함수 하위에서 쓸 것
    //해당 함수가 실행되려면 mPlayerStateLock.lock()이 걸린 상태에서 써야함. 예시는 CanPlayerArrive()를 참고
    void ChangePlayerState(const ePlayerState playerState);
protected:
    std::wstring mNickName;
    float mDepartment;
    eEquipmentFlags mEquipment;
    std::mutex      mPlayerStateLock;
    ePlayerState    mPlayerState;
    int mRoomID;
    int mRank;    //몇등?
};

