#pragma once
#include "session.h"

constexpr int MAX_THREAD = 6;
constexpr int INVALID_KEY = -1;

class Server {
public:
	void disconnect(int c_id);
	int get_new_client_id();
	void process_packet(int c_id, char* packet);
	void do_worker();
	void init();
	void process_event(char* message);

	HANDLE get_handle();
public:
	std::array <SESSION, 1000> clients;

private:
	SOCKET                         m_listenSocket;
	HANDLE                         m_hcp;

	std::vector <std::thread>      m_worker_threads;
};