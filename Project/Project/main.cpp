#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#pragma comment(lib,"ws2_32")
#include <exception>
#include "TriviaServer.h"

int main()
{
	try
	{
		WSADATA wsa_data = {};
		if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
			throw std::exception("WSAStartup Failed");
		TriviaServer s;
		s.server();
	}
	catch (exception e)
	{
		cout << e.what() << endl;
	}
	
	
	return(0);
}