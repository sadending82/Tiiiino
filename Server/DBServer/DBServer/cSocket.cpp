#pragma once
#include "cSocket.h"

#define Test

int Socket::SetKey()
{
    int cnt = STARTKEY;
    while (true) {
        if (cnt == MAXLOBBY)
            return INVALIDKEY;
        if (eSessionState::ST_FREE == mSessions[cnt].mState) {
            return cnt;
        }
        else
            ++cnt;
    }
}

void Socket::Disconnect(int key)
{
    if (mSessions[key].mState == eSessionState::ST_FREE) {
        return;
    }
    closesocket(mSessions[key].mSocket);
    mSessions[key].mState = eSessionState::ST_FREE;

#ifdef Test
    cout << "Lobby Disconnect: " << key << endl;
#endif

}

void Socket::WorkerFunc()
{
    while (1) {
        DWORD Transferred;
        ULONG key;
        WSAOVERLAPPED* over;

        BOOL retval = GetQueuedCompletionStatus(mHcp, &Transferred,
            (PULONG_PTR)&key, &over, INFINITE);

        ExOver* exOver = reinterpret_cast<ExOver*>(over);

        if (false == retval && Transferred == 0) {
            Disconnect(key);
        }

        switch (exOver->mOpType) {
        case eOpType::OP_ACCEPT: {
            int newKey = SetKey();
            if (newKey != INVALIDKEY) {
                SOCKET cSocket = reinterpret_cast<SOCKET>(exOver->mWsaBuf.buf);
                mSessions[newKey].mSocket = cSocket;
                mSessions[newKey].mExOver.mOpType = eOpType::OP_RECV;
                mSessions[newKey].mPrevData = 0;
                mSessions[newKey].mState = eSessionState::ST_ACCEPTED;
                mSessions[newKey].mID = newKey;
                CreateIoCompletionPort((HANDLE)mSessions[newKey].mSocket, mHcp, newKey, 0);
#ifdef Test
                cout << "Lobby Accept: " << newKey << endl;
#endif
                mSessions[newKey].DoRecv();
            }
            else {
                Disconnect(newKey);
                break;
            }

            ZeroMemory(&exOver->mOver, sizeof(exOver->mOver));
            SOCKET cSocket = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
            exOver->mWsaBuf.buf = reinterpret_cast<CHAR*>(cSocket);
            BOOL ret = AcceptEx(mListenSocket, cSocket
                , exOver->mMessageBuf
                , 0, 32, 32, NULL, &exOver->mOver);
            if (false == ret) {
                int err_num = WSAGetLastError();
                if (err_num != WSA_IO_PENDING)
                {
#ifdef Test
                    std::cout << "Accept Error: " << err_num << std::endl;
#endif
                }
            }
            break;
        }
        case eOpType::OP_RECV: {
            unsigned char* packet_ptr = exOver->mMessageBuf;
            int required_data = Transferred + mSessions[key].mPrevData;
            int packet_size = packet_ptr[0];
            while (required_data >= packet_size) {
                if (required_data >= BUFSIZE) break;
                if (packet_size <= 0) break;
                processPacket(key, packet_ptr);
                required_data -= packet_size;
                packet_ptr += packet_size;
                packet_size = packet_ptr[0];
            }
            packet_size = 0;
            mSessions[key].mPrevData = 0;
            if (0 != required_data)
                memcpy(exOver->mMessageBuf, packet_ptr, required_data);
            mSessions[key].DoRecv();
            break;
        }
        case eOpType::OP_SEND: {
            delete exOver;
            break;
        }
        }

    }
}

void Socket::ServerReady(DB* pDB)
{
    Setm_pDB(pDB);

    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    mHcp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
    if (mHcp == NULL) return;

    mListenSocket = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    CreateIoCompletionPort(reinterpret_cast<HANDLE>(mListenSocket), mHcp, 0, 0);
    SOCKADDR_IN serverAddr;
    memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(DBSERVERPORT);
    serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    ::bind(mListenSocket, (struct sockaddr*)&serverAddr, sizeof(SOCKADDR_IN));
    listen(mListenSocket, SOMAXCONN);

    ExOver accept_over;
    accept_over.mOpType = eOpType::OP_ACCEPT;
    memset(&accept_over.mOver, 0, sizeof(accept_over.mOver));
    SOCKET c_socket = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    accept_over.mWsaBuf.buf = reinterpret_cast<char*>(c_socket);
    BOOL ret = AcceptEx(mListenSocket, c_socket, accept_over.mMessageBuf
        , 0, 32, 32, NULL, &accept_over.mOver);

    if (false == ret) {
        int err_num = WSAGetLastError();
        if (err_num != WSA_IO_PENDING)
        {
#ifdef Test
            std::cout << "Accept Error: " << err_num << std::endl;
#endif
            return;
        }
    }
#ifdef Test
    std::cout << "Connect To Lobby Server Ready" << std::endl;
#endif

    WorkerFunc();
}

void Socket::processPacket(int key, unsigned char* buf)
{
    switch (buf[1])
    {
        // 로그인 요청
    case LD_LOGIN: {
        ProcessPacket_Login(key, buf);
        break;
    }
    case LD_SIGNUP: {
        ProcessPacket_SignUp(buf);
        break;
    }
    default:
    {
        break;
    }
    }
}

// DB
bool Socket::CheckLogin(int key, const char* id, const char* password, int userKey)
{
    int res = Getm_pDB()->CheckVerifyUser(id, password);
    if (res == false) {
        cout << "Login Information Invalid\n";
        return false;
    }

    auto userData = Getm_pDB()->SelectUserDataForLogin(id);

    if (get<0>(userData) == 0) {
        return false;
    }

    int uid = get<0>(userData);
    string nickname = get<1>(userData);
    double credit = get<2>(userData);
    int point = get<3>(userData);
    int state = get<4>(userData);

    SendUserDataAfterLogin(key, uid, nickname, id, credit, point, state, userKey);

    if (state == FALSE)
        Getm_pDB()->UpdateUserConnectionState(uid, true);

    cout << "Login Success / ID: " << id << endl;

    return true;
}

// SendPacket
void Socket::SendUserDataAfterLogin(int key, int uid, string& nickname, const char* id, double credit, int point, int state, int userKey)
{
    DL_LOGIN_OK_PACKET p;
    p.size = sizeof(DL_LOGIN_OK_PACKET);
    p.type = SPacketType::DL_LOGIN_OK;
    p.uid = uid;
    memcpy(p.id, id, sizeof(id));
    size_t lengthToCopy = min(nickname.size(), sizeof(p.nickname) - 1);
    memcpy(p.nickname, nickname.c_str(), lengthToCopy);
    p.nickname[lengthToCopy] = '\0';
    p.credit = credit;
    p.point = point;
    p.connState = state;
    p.userKey = userKey;

    mSessions[key].DoSend(reinterpret_cast<DL_LOGIN_OK_PACKET*>(&p));
}

void Socket::SendLoginFail(int key, const char* id, int userKey) {
    DL_LOGIN_FAIL_PACKET p;
    p.size = sizeof(DL_LOGIN_FAIL_PACKET);
    p.type = SPacketType::DL_LOGIN_FAIL;
    p.userKey = userKey;

    mSessions[key].DoSend(reinterpret_cast<DL_LOGIN_FAIL_PACKET*>(&p));
}

// ProcessPacket
void Socket::ProcessPacket_Login(int key, unsigned char* buf)
{
    LD_LOGIN_PACKET* p = reinterpret_cast<LD_LOGIN_PACKET*>(buf);

    bool bCheckLogin = CheckLogin(key, p->id, p->password, p->userKey);
    if (bCheckLogin == false) {
        SendLoginFail(key, p->id, p->userKey);
    }
}

void Socket::ProcessPacket_SignUp(unsigned char* buf)
{
    LD_SIGNUP_PACKET* p = reinterpret_cast<LD_SIGNUP_PACKET*>(buf);
    bool bJoin = Getm_pDB()->SignUpNewPlayer(p->id, p->password);
    if (bJoin == false) {
        cout << "Sign Up new user failed\n";
    }
}