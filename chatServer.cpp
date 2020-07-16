#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include <process.h>
#include "chatServer.h"

using std::cerr;
using std::cout;
using std::endl;
using std::string;
using namespace std;

#define SERVER_PORT 6666
#define MSG_BUF_SIZE 1024

Server::Server()
{
	cout << "Initializing server" << endl;
	//初始化连接绑定监听一条龙
	winsock_ver = MAKEWORD(2, 2);
	addr_len = sizeof(SOCKADDR_IN);
	addr_svr.sin_family = AF_INET;
	addr_svr.sin_port = ::htons(SERVER_PORT);
	addr_svr.sin_addr.S_un.S_addr = ADDR_ANY;
	memset(buf_ip, 0, IP_BUF_SIZE);
	
	ret_val = ::WSAStartup(winsock_ver, &wsa_data);
	if (ret_val != 0){
		cerr << "WSA failed to start up! Error code: " << ::WSAGetLastError() << endl;
		system("pause");
		exit(1);
	}
	cout << "WSA started up successfully!" << endl;
	
	sock_svr = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock_svr == INVALID_SOCKET){
		cerr << "Failed to create server socket! Error code: " << ::WSAGetLastError() << endl;;
		::WSACleanup();
		system("pause");
		exit(1);
	}
	cout << "Server socket created successfully!"<<endl;
	
	ret_val = ::bind(sock_svr, (SOCKADDR*)&addr_svr, addr_len);
	if (ret_val != 0){
		cerr << "Failed to bind server socket!Error code: " << ::WSAGetLastError() << endl;
		::WSACleanup();
		system("pause");
		exit(1);
	}
	cout << "Server socket bound successfully!"<<endl;
	
	ret_val = ::listen(sock_svr, SOMAXCONN);
	if (ret_val == SOCKET_ERROR){
		cerr << "Server socket failed to listen!Error code: " << ::WSAGetLastError() << "\n";
		::WSACleanup();
		system("pause");
		exit(1);
	}
	cout << "Server socket started to listen successfully" << endl;
}

Server::~Server()
{
	::closesocket(sock_svr);
	::closesocket(sock_clt);
	::WSACleanup();
	cout << "Socket closed" << endl;
}

DWORD WINAPI CreateClientThread(LPVOID lpParameter);

void Server::WaitForClient()
{
	while (true){
		sock_clt = ::accept(sock_svr, (SOCKADDR*)&addr_clt, &addr_len);
		if (sock_clt == INVALID_SOCKET){
			cerr << "Failed to accept client! Error code: " << ::WSAGetLastError() << endl;
			::WSACleanup();
			system("pause");
			exit(1);
		}
		::InetNtop(addr_clt.sin_family, &addr_clt, buf_ip, IP_BUF_SIZE);
		cout << "A new client connected...IP address: " << buf_ip << ", port number: " << ::ntohs(addr_clt.sin_port) << endl;
		h_thread = ::CreateThread(nullptr, 0, CreateClientThread, (LPVOID)sock_clt, 0, nullptr);
		if (h_thread == NULL){
			cerr << "Failed to create a new thread! Error code: " << ::WSAGetLastError() << endl;
			::WSACleanup();
			system("pause");
			exit(1);
		}
		::CloseHandle(h_thread);
	}
}
//登陆的时候绑定账号和SOCKET
int Login(SOCKET connSocket, PUserData pCurrUserNode, bool* isLogin)
{
	int nRet = 0;
	bool isLoginSuccess = false;
	bool isLoginRepeat = false;
	PUserData userData = (PUserData)malloc(sizeof(UserData));
	char* n1 = "usera"; char* n2 = "userb";
	if (userData == NULL){
		printf("Malloc failed!\n");
		return 0;
	}
	nRet = recv(connSocket, (char*)userData, sizeof(UserData), 0);
	if (nRet > 0){
		if (strcmp(userData->userName, n1) == 0 || strcmp(userData->userName, n2) == 0){
			if (strcmp(userData->userPwd, n1) == 0 && strcmp(userData->userName, n1) == 0) {
				isLoginSuccess = true;
				UserInfo[0].userID = connSocket;
				UserInfo[0].if_mes = 1;
				cout << UserInfo[0].userID << " : " << userData->userName << endl;
			}
			else if (strcmp(userData->userPwd, n2) == 0 && strcmp(userData->userName, n2) == 0) {
				isLoginSuccess = true;
				UserInfo[1].userID = connSocket;
				UserInfo[1].if_mes = 1;
				cout << UserInfo[1].userID << " : " << userData->userName << endl;
			}
		}
		// 登陆信息正确
		if (isLoginSuccess) {
			*isLogin = true;
			cout << userData->userName << " successfully login" << endl;
			send(connSocket, "successfully login", sizeof("successfully login"), 0);
		}
		else {
			cout << "Login failed!" << endl;
			send(connSocket, (char*)userData, sizeof(UserData), 0);
			nRet = 0;
		}
	}
	free(userData);
	return nRet;
}

void Receive(void* param)
{
	while (1){
		//客户端接受来自服务器的数据
		SOCKET clientSocket = *(SOCKET*)(param);
		char  recvbuf[2048] = {};		//接收缓冲区
		if (recv(clientSocket, recvbuf, 2048, 0) == SOCKET_ERROR){
			for (int i = 0; i < 2; i++) {
				if(UserInfo[i].userID==clientSocket)
					cout << UserInfo[i].userName << " exited!" << endl;
			}
			system("pause");
		}
		else{
			if (strcmp(recvbuf, "exit") == 0){
				cout << "Client requests to close the connection..." << endl;
				break;
			}
			//getbuf
			if (strstr(recvbuf, ":user") != NULL){
				string chat_mes = recvbuf;
				if (strstr(recvbuf, ":usera ") != NULL) {
					clientSocket = UserInfo[0].userID;
					cout << "userb to usera:" << flush;
				}
				else if (strstr(recvbuf, ":userb ") != NULL) {
					clientSocket = UserInfo[1].userID;
					cout << "usera to userb:" << flush;
				}
				else {
					cout << "Wrong userName" << endl;
					send(clientSocket, "Wrong userName", sizeof("Wrong userName"), 0);
					continue;
				}
				chat_mes.erase(0, 6);
				cout << chat_mes << endl;
				send(clientSocket, recvbuf, MSG_BUF_SIZE, 0);
				continue;
			}
			for (int i = 0; i < 2; i++){
				if (UserInfo[i].userID == clientSocket){
					//cout << sock_clt << endl;
					cout << "Message received from" << UserInfo[i].userName << " : " << recvbuf << endl;
				}
			}
			send(clientSocket, recvbuf, MSG_BUF_SIZE, 0);
			if (clientSocket == SOCKET_ERROR){
				cerr << "Failed to send message to client!Error code: " << ::GetLastError() << "\n";
				::closesocket(clientSocket);
				system("pause");
			}
		}

	}
}

DWORD WINAPI CreateClientThread(LPVOID lpParameter)
{
	SOCKET sock_clt = (SOCKET)lpParameter;
	SOCKET to_sock = NULL;
	//char buf_msg[MSG_BUF_SIZE];
	int ret_val = 0;
	int res_log = 0;
	int snd_result = 0;
	bool isLogin = false;
	bool userChat = false;
	//登陆
	PUserData currUser = NULL;
	currUser = (PUserData)malloc(sizeof(UserData));
	do{
		res_log = Login(sock_clt, currUser, &isLogin);
	} while (!isLogin);
	//子线程
	_beginthread(Receive, 0, &sock_clt);
	while (1) {}//为了避免主线程退出使子线程被迫消亡，使主线程进入循环
	ret_val = ::shutdown(sock_clt, SD_SEND);//关掉
	if (ret_val == SOCKET_ERROR){
		cerr << "Failed to shutdown the client socket!Error code: " << ::GetLastError() << "\n";
		::closesocket(sock_clt);
		system("pause");
		return 1;
	}
	return 0;
}
int main()
{
	Server svr;
	svr.WaitForClient();
	system("pause");
	return 0;
}