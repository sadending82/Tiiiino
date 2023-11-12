#include "Network.h"
#include "framework.h"

Network* gNetwork;

Network::Network()
	: s_socket(INVALID_SOCKET)
{
}

Network::~Network()
{
	WSACleanup();
}

void Network::release()
{
	closesocket(s_socket);
	prev_size = 0;
	WSACleanup();
}

bool Network::init()
{
	WSAStartup(MAKEWORD(2, 2), &WSAData);
	ConnectServer();
	return true;
}

void Network::process_packet(unsigned char* p)
{
	unsigned char Type = p[1];

	switch (Type)
	{
	case SC_LOGIN_OK:
	{
		break;
	}
	default:
		break;
	}

}

void Network::error_display(int err_no)
{
	WCHAR* lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, 0);
	std::wcout << lpMsgBuf << std::endl;
	LocalFree(lpMsgBuf);
}

bool Network::ConnectServer()
{
	s_socket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(LOBBYSERVERPORT);

	inet_pton(AF_INET, LOBBYSERVER_ADDR, &server_addr.sin_addr);
	int rt = connect(s_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	if (SOCKET_ERROR == rt)
	{
		std::cout << "connet Error :";
		int err_num = WSAGetLastError();
		error_display(err_num);
		closesocket(s_socket);
		return false;
	}
	else {
		cout << "로비서버랑 연결 성공\n";
	}
	return RecvPacket();
}

bool Network::RecvPacket()
{
	DWORD recv_flag = 0;
	ZeroMemory(&recv_expover.GetWsaOver(), sizeof(recv_expover.GetWsaOver()));

	recv_expover.GetWsaBuf().buf = reinterpret_cast<char*>(recv_expover.GetBuf() + prev_size);
	recv_expover.GetWsaBuf().len = BUF_SIZE - prev_size;

	int ret = WSARecv(s_socket, &recv_expover.GetWsaBuf(), 1, NULL, &recv_flag, &recv_expover.GetWsaOver(), recv_callback);
	if (SOCKET_ERROR == ret)
	{
		int err = WSAGetLastError();
		if (err != WSA_IO_PENDING)
		{
			return false;
		}
		else {
			//UE_LOG(LogTemp, Error, TEXT("return true"));
		}
	}
	return true;
}

void send_callback(DWORD err, DWORD num_byte, LPWSAOVERLAPPED send_over, DWORD flag)
{	
	//cout << "send_callback is called" << endl;
	WSA_OVER_EX* once_exp = reinterpret_cast<WSA_OVER_EX*>(send_over);
	delete once_exp;
}

void recv_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED recv_over, DWORD flag)
{

	WSA_OVER_EX* over = reinterpret_cast<WSA_OVER_EX*>(recv_over);
	auto Game = gNetwork;
	if (err != 0)
	{
		Game->release();
		return;
	}
	if (num_bytes == 0)return;

	int to_process_data = num_bytes + Game->prev_size;
	unsigned char* packet = over->GetBuf();
	int packet_size = packet[0];
	while (packet_size <= to_process_data) {
		Game->process_packet(packet);
		to_process_data -= packet_size;
		packet += packet_size;
		if (to_process_data > 0) packet_size = packet[0];
		else break;
	}
	Game->prev_size = to_process_data;
	if (to_process_data > 0)
	{
		memcpy(over->GetBuf(), packet, to_process_data);
	}
	Game->RecvPacket();
}

void send_alert_packet(SOCKET& sock, wstring& alert)
{
	ML_ALERT_PACKET packet;
	packet.size = sizeof(packet);
	packet.type = ML_ALERT;
	memcpy(packet.alert, alert.c_str(), sizeof(packet.alert));
	WSA_OVER_EX* once_exp = new WSA_OVER_EX(sizeof(packet), &packet);
	int ret = WSASend(sock, &once_exp->GetWsaBuf(), 1, 0, 0, &once_exp->GetWsaOver(), send_callback);

}
