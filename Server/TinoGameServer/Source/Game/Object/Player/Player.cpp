#include "Player.h"
#include "../../Server/MainServer/MainServer.h"
#include "../../Room/Room.h"

Player::Player()
	:mNickName("")
	, mGrade(0.0f)
	, mDepartment(eDepartment::None)
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

void Player::DisConnectAndReset()
{
	mStateLock.lock();
	if (mSocketState == eSocketState::ST_FREE)
	{
		mStateLock.unlock();
		return;
	}
	else {
		mSocketState = eSocketState::ST_FREE;
		mStateLock.unlock();
	}
	DisConnect();
	if(mRank < 0)
		gMainServer->send_player_result_packet(mUID, mRank, mRoomID, true);
	//gMainServer->GetRooms()[mRoomID]->DisablePlayer(this);
	auto sPacket = gMainServer->make_player_remove_packet(mRoomSyncID);
	gMainServer->SendRoomBroadCast(mRoomID, (void*)&sPacket, sizeof(sPacket));
	Reset();
}

void Player::DisConnect()
{
	__super::DisConnect();
}

void Player::Reset()
{
	__super::Reset();

	mNickName.clear();
	mDepartment = eDepartment::None;
	mEquipment = eEquipmentFlags::None;
	mGrade = 0.0f;
	mRoomID = INVALID_ROOM_ID;
	mRoomSyncID = -1;
	mRank = -1;
	mPlayerState = ePlayerState::ST_RUNNING;
	mUID = -1;
	mPing = 0;
}

bool Player::RecvPacket()
{
	if (false == __super::RecvPacket())
	{
		DisConnectAndReset();
		return false;
	}
	return true;
}

bool Player::SendPacket(void* packet, int bytes)
{
	if (false == __super::SendPacket(packet, bytes))
	{
		DisConnectAndReset();
		return false;
	}
	return true;
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
