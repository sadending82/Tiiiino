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
    case LD_EQUIP_ITEM: {
        ProcessPacket_EquipItem(key, buf);
        break;
    }
    case LD_UNEQUIP_ITEM: {
        ProcessPacket_UnequipItem(key, buf);
        break;
    }
    case LD_BUY_ITEM: {
        ProcessPacket_BuyItem(key, buf);
        break;
    }
    case LD_USE_COUPON: {
        ProcessPacket_UseCoupon(key, buf);
        break;
    }
    case LD_REFRESH_INVENTORY: {
        ProcessPacket_RefreshInventory(key, buf);
        break;
    }
    case LD_REFRESH_DEP_RANK: {
        ProcessPacket_RefreshDepRank(key, buf);
        break;
    }
    case LD_GET_POINT: {
        ProcessPacket_GetPoint(key, buf);
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
    long long equippedItems = get<5>(userData);
    long long inventoryItems = Getm_pDB()->SelectInventory(uid);

    vector<rankInfo> ranking = Getm_pDB()->SelectRanking();

    SendLoginOK(key, uid, id, grade, point, state, department, equippedItems
        , inventoryItems, ranking, userKey);

    if (state == FALSE)
        Getm_pDB()->UpdateUserConnectionState(uid, true);

    DEBUGMSGONEPARAM("Login Success / ID: %s\n", id);

#endif
    return true;
}

bool Socket::CheckValidString(const char* input)
{
    string str = input;

    if (str.length() == 0) return false;

    for (wchar_t c : str) {
        if (c == ' ') return false;
        if (iswpunct(c)) return false;
        if (0xAC00 <= c && c <= 0xD7A3) return false;
    }

    return true;
}

long long Socket::EquipItem(int itemCode, long long equipmentFlag)
{
    long long initBit;

    if (itemCode < STARTCODE_HANDEQUIP)
        initBit = BACKEQUIP;
    else if (STARTCODE_HANDEQUIP <= itemCode && itemCode < STARTCODE_FACEEQUIP)
        initBit = HANDEQUIP;
    else if (STARTCODE_FACEEQUIP <= itemCode && itemCode < STARTCODE_HEADEQUIP)
        initBit = FACEEQUIP;
    else
        initBit = HEADEQUIP;

    return (equipmentFlag & initBit) | (1ULL << itemCode);
}

long long Socket::UnequipItem(int itemCode, long long equipmentFlag)
{
    return equipmentFlag & ~(1ULL << itemCode);
}

// SendPacket
void Socket::SendLoginOK(int key, int uid, const char* id
    , double grade, int point, int state, char department
    , long long equippedItemFlag, long long inventoryFlag,
    vector<rankInfo>& ranking, int userKey)
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
    p.equipmentflag = equippedItemFlag;
    p.inventoryflag = inventoryFlag;

    for (int i = 0; i < ranking.size(); ++i) {
        p.ranking[i] = ranking[i];
    }

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

void Socket::SendInventory(int key, long long inventoryFlag, int userKey)
{
    DL_REFRESH_INVENTORY_PACKET p;
    p.size = sizeof(DL_REFRESH_INVENTORY_PACKET);
    p.type = SPacketType::DL_REFRESH_INVENTORY;
    p.inventoryFlag = inventoryFlag;
    p.userKey = userKey;

    mSessions[key].DoSend((void*)(&p));
}

void Socket::SendBuyItemOK(int key, int itemCode, int userKey, int pointAfterPurchase)
{
    DL_BUYITEM_OK_PACKET p;
    p.size = sizeof(DL_BUYITEM_OK_PACKET);
    p.type = SPacketType::DL_BUYITEM_OK;
    p.itemCode = itemCode;
    p.pointAfterPurchase = pointAfterPurchase;
    p.userKey = userKey;

    mSessions[key].DoSend((void*)(&p));
}

void Socket::SendBuyItemFail(int key, int userKey)
{
    DL_BUYITEM_FAIL_PACKET p;
    p.size = sizeof(DL_BUYITEM_FAIL_PACKET);
    p.type = SPacketType::DL_BUYITEM_FAIL;
    p.userKey = userKey;

    mSessions[key].DoSend((void*)(&p));
}

void Socket::SendUseCouponOK(int key, int itemCode, int userKey, long long inventoryFlag)
{
    DL_USE_COUPON_OK_PACKET p;
    p.size = sizeof(DL_USE_COUPON_OK_PACKET);
    p.type = SPacketType::DL_USE_COUPON_OK;
    p.inventoryFlag = inventoryFlag;
    p.itemcode = itemCode;
    p.userKey = userKey;

    mSessions[key].DoSend((void*)(&p));
}

void Socket::SendUseCouponFail(int key, int userKey)
{
    DL_USE_COUPON_FAIL_PACKET p;
    p.size = sizeof(DL_USE_COUPON_FAIL_PACKET);
    p.type = SPacketType::DL_USE_COUPON_FAIL;
    p.userKey = userKey;

    mSessions[key].DoSend((void*)(&p));
}

void Socket::SendRanking(int key, vector<rankInfo>& ranking, int userKey)
{
    DL_REFRESH_DEP_RANK_PACKET p;
    p.size = sizeof(DL_REFRESH_DEP_RANK_PACKET);
    p.type = SPacketType::DL_REFRESH_DEP_RANK;
    p.userKey = userKey;

    for (int i = 0; i < ranking.size(); ++i) {
        p.ranking[i] = ranking[i];
    }
    mSessions[key].DoSend((void*)(&p));
}

void Socket::SendEquipOK(int key, long long equipmentFlag, int userKey)
{
    DL_EQUIP_OK_PACKET p;
    p.size = sizeof(p);
    p.type = SPacketType::DL_EQUIP_OK;
    p.equipmentFlag = equipmentFlag;
    p.userKey = userKey;

    mSessions[key].DoSend((void*)(&p));
}

void Socket::SendUnequipOK(int key, long long equipmentFlag, int userKey)
{
    DL_UNEQUIP_OK_PACKET p;
    p.size = sizeof(p);
    p.type = SPacketType::DL_UNEQUIP_OK;
    p.equipmentFlag = equipmentFlag;
    p.userKey = userKey;

    mSessions[key].DoSend((void*)(&p));
}

void Socket::SendPoint(int key, int point, int userKey)
{
    DL_GET_POINT_PACKET p;
    p.size = sizeof(p);
    p.type = SPacketType::DL_GET_POINT;
    p.point = point;
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
    vector<rankInfo> emptyRanking{};
    SendLoginOK(key, uid, p->id
        , 3.0, 100000, 1, 0, 0, 0, emptyRanking, p->userKey);
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

#ifdef RUN_DB

    auto userData = Getm_pDB()->SelectUserGradeAndDepartment(p->uid);
    int gradeBeforeUpdate = get<0>(userData);
    char department = get<1>(userData);

    bool bResult = Getm_pDB()->UpdateUserGrade(p->uid, p->grade);
    if (bResult != true) {
        DEBUGMSGONEPARAM("Update User Grade failed / UID: %d\n", p->uid);
    }

    bResult = Getm_pDB()->UpdateUserPoint(p->uid, p->point);
    if (bResult != true) {
        Getm_pDB()->UpdateUserGrade(p->uid, gradeBeforeUpdate);
        DEBUGMSGONEPARAM("Update User Grade failed / UID: %d\n", p->uid);
        return;
    }

    bResult = Getm_pDB()->UpdateRanking(department, p->point);
    if (bResult != true) {
        Getm_pDB()->UpdateUserGrade(p->uid, gradeBeforeUpdate);
        Getm_pDB()->UpdateUserPoint(p->uid, -p->point);
        DEBUGMSGONEPARAM("Update Ranking failed / UID: %d\n", p->uid);
    }

#endif
}

void Socket::ProcessPacket_RefreshInventory(int key, unsigned char* buf)
{
    LD_REFRESH_INVENTORY_PACKET* p = reinterpret_cast<LD_REFRESH_INVENTORY_PACKET*>(buf);
    long long bitFlag = 0;
#ifdef RUN_DB
    bitFlag = Getm_pDB()->SelectInventory(p->uid);
#endif
    SendInventory(key, bitFlag, p->userKey);
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

void Socket::ProcessPacket_EquipItem(int key, unsigned char* buf)
{
    LD_EQUIP_ITEM_PACKET* p = reinterpret_cast<LD_EQUIP_ITEM_PACKET*>(buf);

    long long equipmentItem = 0;
#ifdef RUN_DB
    equipmentItem = Getm_pDB()->SelectEquipmentItems(p->uid);
    equipmentItem = EquipItem(p->itemCode, equipmentItem);

    bool bResult = Getm_pDB()->UpdateEquipItemFlag(p->uid, equipmentItem);
#endif
    SendEquipOK(key, equipmentItem, p->userKey);
}

void Socket::ProcessPacket_UnequipItem(int key, unsigned char* buf)
{
    LD_UNEQUIP_ITEM_PACKET* p = reinterpret_cast<LD_UNEQUIP_ITEM_PACKET*>(buf);

    long long equipmentItem = 0;
#ifdef RUN_DB
    equipmentItem = Getm_pDB()->SelectEquipmentItems(p->uid);
    equipmentItem = UnequipItem(p->itemCode, equipmentItem);

    bool bResult = Getm_pDB()->UpdateEquipItemFlag(p->uid, equipmentItem);
#endif
    SendUnequipOK(key, equipmentItem, p->userKey);
}

void Socket::ProcessPacket_BuyItem(int key, unsigned char* buf)
{
    LD_BUY_ITEM_PACKET* p = reinterpret_cast<LD_BUY_ITEM_PACKET*>(buf);
    int pointAfterPurchase = 0;
#ifdef RUN_DB

    bool bResult = Getm_pDB()->UpdateUserPointAfterBuyItem(p->uid, p->price);
    if (bResult != true) {
        SendBuyItemFail(key, p->userKey);
        return;
    }

    bResult = Getm_pDB()->UpdateInventory(p->uid, p->itemCode);
    if (bResult != true) {
        Getm_pDB()->UpdateUserPointAfterBuyItem(p->uid, -p->price);
        SendBuyItemFail(key, p->userKey);
        return;
    }
    
    pointAfterPurchase = Getm_pDB()->SelectPoint(p->uid);

#endif

    SendBuyItemOK(key, p->itemCode, p->userKey, pointAfterPurchase);
}

void Socket::ProcessPacket_UseCoupon(int key, unsigned char* buf)
{
    LD_USE_COUPON_PACKET* p = reinterpret_cast<LD_USE_COUPON_PACKET*>(buf);
    int itemCode = 0;

#ifdef RUN_DB
    auto info = m_pDB->SelectCouponInfo(p->couponCode);

    itemCode = get<0>(info);
    bool canDuplicated = get<1>(info);
    bool isUsed = get<2>(info);

    if (canDuplicated == false && isUsed == true) {
        SendUseCouponFail(key, p->userKey);
        return;
    }
    bool bResult = false;
    if (canDuplicated == false) {
        bResult = m_pDB->UpdateCouponUsed(p->couponCode, true);
        if (bResult == false) {
            SendUseCouponFail(key, p->userKey);
            return;
        }
    }

    bResult = m_pDB->UpdateInventory(p->uid, itemCode);
    if (bResult == false) {
        if (canDuplicated == false) {
            m_pDB->UpdateCouponUsed(p->couponCode, false);
        }
        SendUseCouponFail(key, p->userKey);
        return;
    }

    long long newInventory = m_pDB->SelectInventory(p->uid);
    if (newInventory == -1) {
        if (canDuplicated == false) {
            m_pDB->UpdateCouponUsed(p->couponCode, false);
        }
        m_pDB->UpdateInventoryDeleteItem(p->uid, itemCode);
        SendUseCouponFail(key, p->userKey);
        return;
    }
#endif

    SendUseCouponOK(key, itemCode, p->userKey, newInventory);
}

void Socket::ProcessPacket_RefreshDepRank(int key, unsigned char* buf)
{
    LD_REFRESH_DEP_RANK_PACKET* p = reinterpret_cast<LD_REFRESH_DEP_RANK_PACKET*>(buf);

    vector<rankInfo> ranking;
#ifdef RUN_DB
    ranking = m_pDB->SelectRanking();
#endif

    SendRanking(key, ranking, p->userKey);
}

void Socket::ProcessPacket_GetPoint(int key, unsigned char* buf)
{
    LD_GET_POINT_PACKET* p = reinterpret_cast<LD_GET_POINT_PACKET*>(buf);

    int point = 0;
#ifdef RUN_DB
    point = m_pDB->SelectPoint(p->uid);
#endif
    SendPoint(key, point, p->userKey);
}

int Socket::SetUIDForTest()
{
    return testUID++;
}
