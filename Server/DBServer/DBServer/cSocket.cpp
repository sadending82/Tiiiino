#pragma once
#include "cSocket.h"

int Socket::SetKey()
{
    int cnt = STARTKEY;
    while (true) {
        if (cnt == MAXLOBBY)
            return INVALIDKEY;
        if (eSessionState::ST_FREE == mSessions[cnt].GetState()) {
            return cnt;
        }
        else
            ++cnt;
    }
}

void Socket::Disconnect(int key)
{
    if (mSessions[key].GetState() == eSessionState::ST_FREE) {
        return;
    }
    closesocket(mSessions[key].GetSocket());
    mSessions[key].SetState(eSessionState::ST_FREE);

    DEBUGMSGONEPARAM("Lobby Disconnect: %d\n", key);
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
                mSessions[newKey].SetSocket(cSocket);
                mSessions[newKey].GetExOver().SetmOpType(eOpType::OP_RECV);
                mSessions[newKey].SetPrevData(0);
                mSessions[newKey].SetState(eSessionState::ST_ACCEPTED);
                mSessions[newKey].SetID(newKey);
                CreateIoCompletionPort((HANDLE)mSessions[newKey].GetSocket(), mHcp, newKey, 0);

                DEBUGMSGONEPARAM("Lobby Accept: %d\n", newKey);
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
                    DEBUGMSGONEPARAM("Accept Error: %d\n", err_num);
                }
            }
            break;
        }
        case eOpType::OP_RECV: {
            unsigned char* packet_ptr = exOver->mMessageBuf;
            int required_data = Transferred + mSessions[key].GetPrevData();
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
            mSessions[key].SetPrevData(0);
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

void Socket::ServerReady()
{
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
            DEBUGMSGONEPARAM("Accept Error: %d\n", err_num);
            return;
        }
    }
    cout <<"Connect To Lobby Server Ready\n";

    WorkerFunc();
}

void Socket::processPacket(int key, unsigned char* buf)
{
    switch (buf[1])
    {
    case LD_LOGIN: {
        ProcessPacket_Login(key, buf);
        break;
    }
    case LD_LOGOUT: {
        ProcessPacket_Logout(buf);
        break;
    }
    case LD_SIGNUP: {
        ProcessPacket_SignUp(key, buf);
        break;
    }
    case LD_UPDATE_GRADE: {
        ProcessPacket_UpdateGrade(key, buf);
        break;
    }
    case LD_CHANGE_DEPARTMENT: {
        ProcessPacket_ChangeDepartment(key, buf);
        break;
    }
    default:
    {
        break;
    }
    }
}

bool Socket::CheckVersion(const char* version)
{
    if (strcmp(GAMEVERSION, version) == 0) return true;

    return false;
}

// DB
bool Socket::CheckLogin(int key, const char* id, const char* password, int userKey, const char* version)
{
#ifdef CHECK_VERSION
    if (CheckVersion(version) == false) {
        return false;
    }
#endif

#ifdef RUN_DB
    int res = Getm_pDB()->CheckVerifyUser(id, password);
    if (res == false) {
        DEBUGMSGONEPARAM("Login Information Invalid: %s\n", id);
        return false;
    }

    auto userData = Getm_pDB()->SelectUserDataForLogin(id);

    if (get<0>(userData) == 0) {
        return false;
    }

    int uid = get<0>(userData);
    double grade = get<1>(userData);
    int point = get<2>(userData);
    int state = get<3>(userData);
    char department = get<4>(userData);

    SendLoginOK(key, uid, id, grade, point, state, department, userKey);

    if (state == FALSE)
        Getm_pDB()->UpdateUserConnectionState(uid, true);

    DEBUGMSGONEPARAM("Login Success / ID: %s\n", id);

#endif
    return true;
}

bool Socket::CheckValidString(const char* input)
{
    string str = input;

    for (wchar_t c : str) {
        if (iswpunct(c)) return false;
        if (0xAC00 <= c && c <= 0xD7A3) return false;
    }

    return true;
}

// SendPacket
void Socket::SendLoginOK(int key, int uid, const char* id
    , double grade, int point, int state, char department, int userKey)
{
    DL_LOGIN_OK_PACKET p;
    p.size = sizeof(DL_LOGIN_OK_PACKET);
    p.type = SPacketType::DL_LOGIN_OK;
    p.uid = uid;
    memcpy(p.id, id, sizeof(p.id));
    p.grade = grade;
    p.point = point;
    p.connState = state;
    p.department = department;
    p.userKey = userKey;

    mSessions[key].DoSend((void*)(&p));
}

void Socket::SendLoginFail(int key, int userKey) {
    DL_LOGIN_FAIL_PACKET p;
    p.size = sizeof(DL_LOGIN_FAIL_PACKET);
    p.type = SPacketType::DL_LOGIN_FAIL;
    p.userKey = userKey;

    mSessions[key].DoSend((void*)(&p));
}

void Socket::SendSignUpOK(int key, int userKey)
{
    DL_SIGNUP_OK_PACKET p;
    p.size = sizeof(DL_SIGNUP_OK_PACKET);
    p.type = SPacketType::DL_SIGNUP_OK;
    p.userKey = userKey;

    mSessions[key].DoSend((void*)(&p));
}

void Socket::SendSignUpFail(int key, int userKey)
{
    DL_SIGNUP_FAIL_PACKET p;
    p.size = sizeof(DL_SIGNUP_FAIL_PACKET);
    p.type = SPacketType::DL_SIGNUP_FAIL;
    p.userKey = userKey;

    mSessions[key].DoSend((void*)(&p));
}

// ProcessPacket
void Socket::ProcessPacket_Login(int key, unsigned char* buf)
{
    LD_LOGIN_PACKET* p = reinterpret_cast<LD_LOGIN_PACKET*>(buf);

#ifdef RUN_DB
    bool bResult = CheckLogin(key, p->id, p->password, p->userKey, p->gameVersion);
    if (bResult == false) {
        SendLoginFail(key, p->userKey);
    }
    return;
#endif

    int uid = SetUIDForTest();

    SendLoginOK(key, uid, p->id
        , 3.0, 100000, 1, 0, p->userKey);
}

void Socket::ProcessPacket_Logout(unsigned char* buf)
{
#ifdef RUN_DB
    LD_LOGOUT_PACKET* p = reinterpret_cast<LD_LOGOUT_PACKET*>(buf);
    Getm_pDB()->UpdateUserConnectionState(p->uid, 0);
#endif
}

void Socket::ProcessPacket_SignUp(int key, unsigned char* buf)
{
    LD_SIGNUP_PACKET* p = reinterpret_cast<LD_SIGNUP_PACKET*>(buf);

    if (CheckValidString(p->id) != true) {
        DEBUGMSGONEPARAM("User Input Invaild Charactor In ID / ID: %s\n", p->id);
        SendSignUpFail(key, p->userKey);
        return;
    }
    if (CheckValidString(p->password) != true) {
        DEBUGMSGONEPARAM("User Input Invaild Charactor In Password / PW: %s\n", p->password);
        SendSignUpFail(key, p->userKey);
        return;
    }

#ifdef RUN_DB
    bool bResult = Getm_pDB()->SignUpNewPlayer(p->id, p->password, p->department);
    if (bResult != true) {
        DEBUGMSGONEPARAM("Sign Up new user failed / ID: %s\n", p->id);
        SendSignUpFail(key, p->userKey);
        return;
    }
#endif
    DEBUGMSGONEPARAM("Sign Up new user success / ID: %s\n", p->id);
    SendSignUpOK(key, p->userKey);
}

void Socket::ProcessPacket_UpdateGrade(int key, unsigned char* buf)
{
    LD_UPDATE_GRADE_PACKET* p = reinterpret_cast<LD_UPDATE_GRADE_PACKET*>(buf);

    if (ADMIN_START_UID <= p->uid 
        && p->uid <= ADMIN_LAST_UID) {
        return;
    }

#ifdef RUN_DB

    auto userData = Getm_pDB()->SelectUserGradeAndDepartment(p->uid);
    int gradeBeforeUpdate = get<0>(userData);
    char department = get<1>(userData);

    bool bResult = Getm_pDB()->UpdateUserGrade(p->uid, p->grade);
    if (bResult != true) {
        DEBUGMSGONEPARAM("Update User Grade failed / UID: %d\n", p->uid);
    }

    // 랭킹에 반영할 점수 나중에 수정
    /*int score = 100;

    bResult = Getm_pDB()->UpdateRanking(department, score);
    if (bResult != true) {
        cout << "Update Ranking failed\n";
    }*/

#endif
}

void Socket::ProcessPacket_ChangeDepartment(int key, unsigned char* buf)
{
    LD_CHANGE_DEPARTMENT_PACKET* p = reinterpret_cast<LD_CHANGE_DEPARTMENT_PACKET*>(buf);
#ifdef RUN_DB
    bool bResult = Getm_pDB()->UpdateUserDepartment(p->uid, p->department);
    if (bResult != true) {

    }
#endif
}


int Socket::SetUIDForTest()
{
    return testUID++;
}
