#pragma once

#include "timer.h"

void InitializeNetwork();
void GetPointCloud(int* size, float** points);

extern int global_delay;
extern std::atomic_int active_clients;

void SendMatchingPacket(int key);

void ConnectGameServer(int key);

void MoveWorker();

void SendMoveRand(int key);

void SendGameLoadAck(int key);

void SendGameServerLoginPacket(int key);

void SendPingPacket(int key, const long long ping);

string MakeDummyID(int num);