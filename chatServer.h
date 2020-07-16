#ifndef TESTSERVER_H
#define TESTSERVER_H

#include <Winsock2.h>
#include <windows.h>

#pragma comment (lib, "ws2_32.lib")

#define IP_BUF_SIZE 129
#define NAME_BUF_SIZE 50

class Server{
public:
	Server();
	~Server();
	Server(const Server&) = delete;
	Server& operator=(const Server&) = delete;
	void WaitForClient();
private:
	WORD winsock_ver;
	WSADATA wsa_data;
	SOCKET sock_svr;
	SOCKET sock_clt;
	HANDLE h_thread;
	SOCKADDR_IN addr_svr;
	SOCKADDR_IN addr_clt;
	int ret_val;
	int addr_len;
	char buf_ip[IP_BUF_SIZE];
};

typedef struct UserData{
	char userName[NAME_BUF_SIZE];
	char userPwd[NAME_BUF_SIZE];
	SOCKET userID;
	int if_mes;
}UserData, * PUserData;

extern struct UserData UserInfo[2] = {
	{"usera","usera",NULL,0},
	{"userb","userb",NULL,0},
};
#endif