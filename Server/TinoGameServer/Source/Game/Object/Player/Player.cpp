#include "Player.h"
#include "../../Server/MainServer/MainServer.h"

Player::Player()
	:mNickName("")
	, mDepartment(0.0f)
	, mEquipment(eEquipmentFlags::None)
	, mRoomID(INVALID_ROOM_ID)
	, mRoomSyncID(-1)
	, mRank(-1)
	, mPlayerState(ePlayerState::ST_RUNNING)
	, mUID(-1)
	, mPing(0)
{
}

Player::~Player()
{
}

void Player::DisConnect()
{
	__super::DisConnect();
	auto sPacket = gMainServer->make_player_remove_packet(mRoomSyncID);
	gMainServer->SendRoomBroadCast(mRoomID, (void*)&sPacket, sizeof(sPacket));
	Reset();
}

void Player::Reset()
{
	__super::Reset();

	mNickName.clear();
	mDepartment = 0.0f;
	mEquipment = eEquipmentFlags::None;
	mRoomID = INVALID_ROOM_ID;
	mRoomSyncID = -1;
	mRank = -1;
	mPlayerState = ePlayerState::ST_RUNNING;
	mUID = -1;
	mPing = 0;
}

bool Player::CanMakeID()
{
	mStateLock.lock();
	if (eSocketState::ST_FREE == mSocketState)
	{
		mSocketState = eSocketState::ST_ACCEPT;
		mStateLock.unlock();
		//cout << i <<"번째 캐릭터 입장"<< endl;
		return true;;
	}
	mStateLock.unlock();
	return false;
}

bool Player::IsPlayerArrived()
{
	mPlayerStateLock.lock();
	if (mPlayerState == ePlayerState::ST_ARRIVED)
	{
		mPlayerStateLock.unlock();
		return true;
	}
	mPlayerStateLock.unlock();
	return false;
}

bool Player::CanPlayerArrive()
{
	mPlayerStateLock.lock();
	if (mPlayerState == ePlayerState::ST_RUNNING)
	{
		ChangePlayerState(ePlayerState::ST_ARRIVED);
		mPlayerStateLock.unlock();
		return true;
	}
	mPlayerStateLock.unlock();
	return false;
}

void Player::ChangePlayerState(const ePlayerState playerState)
{
	mPlayerState = playerState;
}

//void Player::PlayerArrive(int rank)
//{
//	mPlayerStateLock.lock();
//	if (mPlayerState == ePlayerState::ST_RUNNING)
//	{
//		mPlayerState = ePlayerState::ST_ARRIVED;
//	}
//	mPlayerStateLock.unlock();
//}
