#include <iostream>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include "chatServer.h"
#include <process.h>
#include <string>
#pragma comment(lib, "ws2_32.lib")
using std::cin;
using std::cerr;
using std::cout;
using std::string;
using std::endl;
using std::flush;
using namespace std;

const char DEFAULT_PORT[] = "6666";
const int SEND_BUF_SIZE = 256;
bool g_isLogin = false;

void Receive(void* param)
{
	while (1){
		//客户端接受来自服务器的数据
		SOCKET clientSocket = *(SOCKET*)(param);
		char  recvbuf[2048] = {};		//接收缓冲区
		if (recv(clientSocket, recvbuf, 2048, 0) == SOCKET_ERROR){
			cout << "Failed to receive data" << endl;
		}
		else{
			string mesto = recvbuf;
			if (strstr(recvbuf, ":usera ") != NULL) {
				mesto.erase(0, 6);
				cout << "\nmessage from userb:" << mesto << endl;
				cout << "enter the message that you want to send:" << flush;
			}
			else if (strstr(recvbuf, ":userb ") != NULL) {
				mesto.erase(0, 6);
				cout << "\nmessage from usera:" << mesto << endl;
				cout << "enter the message that you want to send:" << flush;
			}
			else
				continue;
		}

	}
}

void Send(void* param)
{
	while (1){
		//客户端发送数据给服务器
		SOCKET clientSocket = *(SOCKET*)(param);
		char sendbuf[2048] = {};		//发送缓冲区
		cout << "enter the message that you want to send: " << flush;
		cin.getline(sendbuf, 2048);
		if (send(clientSocket, sendbuf, strlen(sendbuf), 0) == SOCKET_ERROR){
			cout << "Failed to send data!";
		}
		else{
			cout << "Data sent successfully!" << endl;
		}

	}
}

int Login(SOCKET connSocket, char* pUserName, char* pUserPwd, bool* slogin)
{
	int nRet = 0;
	UserData userData;
	strcpy_s(userData.userName, NAME_BUF_SIZE, pUserName);
	strcpy_s(userData.userPwd, NAME_BUF_SIZE, pUserPwd);
	userData.userID = connSocket;
	char send_buf[SEND_BUF_SIZE];
	int rev = 0;
	nRet = send(connSocket, (char*)&userData, sizeof(UserData), 0);
	rev = recv(connSocket, send_buf, SEND_BUF_SIZE, 0);
	if (strcmp(send_buf, "successfully login") == 0){
		*slogin = true;
	}
	else {
		*slogin = false;
		nRet = 0;
	}
	return nRet;
}

int main() {
	WSADATA wsa_data; //WSADATA变量,包含windows socket执行的信息
	int i_result = 0; //接收返回值
	SOCKET sock_client = INVALID_SOCKET;
	addrinfo* result = nullptr, hints;
	//初始化winsock动态库(ws2_32.dll),MAKEWORD(2, 2)用于请求使用winsock2.2版本
	i_result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
	if (i_result != 0) {
		cerr << "WSAStartup() function failed: " << i_result << "\n";
		system("pause");
		return 1;
	}
	SecureZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	//连接
	i_result = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);
	if (i_result != 0) {
		cerr << "getaddrinfo() function failed with error: " << WSAGetLastError() << endl;
		WSACleanup();
		system("pause");
		return 1;
	}
	//创建套接字
	sock_client = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (sock_client == INVALID_SOCKET) {
		cerr << "socket() function failed with error: " << WSAGetLastError() << endl;
		WSACleanup();
		system("pause");
		return 1;
	}
	//连接服务器
	i_result = connect(sock_client, result->ai_addr, result->ai_addrlen);
	if (i_result == SOCKET_ERROR) {
		cerr << "connect() function failed with error: " << WSAGetLastError() << endl;
		WSACleanup();
		system("pause");
		return 1;
	}
	cout << "connect server successfully" << endl;
	freeaddrinfo(result);
	//登陆
	char username[NAME_BUF_SIZE];
	char userpwd[NAME_BUF_SIZE];
	int slog = 0;
	while (!g_isLogin)
	{
		cout << "enter user name：" << flush;
		cin.getline(username, NAME_BUF_SIZE);
		cout << "enter user pwd：" << flush;
		cin.getline(userpwd, NAME_BUF_SIZE);
		slog = Login(sock_client, username, userpwd, &g_isLogin);
	}
	//子线程发送接收
	cout << "If you want to send to another user " << endl;
	cout << "You need to add ':username ' before the message " << endl;
	_beginthread(Receive, 0, &sock_client);
	_beginthread(Send, 0, &sock_client);
	Sleep(INFINITE);//无限期休眠，避免执行完退出
	if (sock_client != INVALID_SOCKET) {
		closesocket(sock_client);
		sock_client = INVALID_SOCKET;
	}
	WSACleanup();
	cout << "socket closed" << endl;
	system("pause");
	return 0;
}