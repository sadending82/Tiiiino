#pragma once
#include <string>
#include "../../Network/Network.h"
#include "../Object.h"

class Room;
class Player : public Object, public Network
{
public:
    Player();
    ~Player();

    virtual void DisConnect() override;
    virtual void Reset() override;

    bool CanMakeID();

    int GetRoomID() const { return mRoomID; }
    void SetRoomID(int roomID) { mRoomID = roomID; }
    //client using this for own identify key
    int GetRoomSyncID() const { return mRoomSyncID; }
    void SetRoomSyncID(int roomSyncID) { mRoomSyncID = roomSyncID; }

    int GetRank() const { return mRank; }
    void SetRank(int rank) { mRank = rank; }

    bool IsPlayerArrived();
    bool CanPlayerArrive();

    int GetUID() const { return mUID; }
    void SetUID(const int uID) { mUID = uID; }

    std::string GetNickName() const { return mNickName; }
    void SetNickName(const std::string name) { mNickName = name; }
    std::string GetID() const { return mID; }
    void SetID(const std::string id) { mID = id; }

    long long GetPing() const { return mPing; }
    void SetPing(const long long ping) { mPing = ping; }
protected:
    //Be sure to use it below the function that calls the lock. ex)CanPlayerArrive()
    //반드시 락을 불러주는 함수 하위에서 쓸 것
    //해당 함수가 실행되려면 mPlayerStateLock.lock()이 걸린 상태에서 써야함. 예시는 CanPlayerArrive()를 참고
    void ChangePlayerState(const ePlayerState playerState);
protected:
    std::string mID;
    std::string mNickName;
    float mDepartment;
    eEquipmentFlags mEquipment;
    std::mutex      mPlayerStateLock;
    ePlayerState    mPlayerState;
    int mRoomID;    // room number, check own mRooms[mRoomID]
    //client using this for own identify key
    // in room number, client use "Othercharacter[mRoomSyncID]"
    int mRoomSyncID;
    int mRank;    //몇등?
    int mUID;       //DB에서 부여해주는 ID use for game result 
    long long mPing;    //milliseconds
};

