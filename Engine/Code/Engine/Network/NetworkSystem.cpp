#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>
#include <WS2TCPIP.h>
#pragma comment(lib, "Ws2_32.lib")
#include <cstdint>
#include <cstdlib> 
#include "Engine/Network/NetworkSystem.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EngineCommon.hpp"
SOCKET m_listenSocket = INVALID_SOCKET;
SOCKET m_connectionToServer = INVALID_SOCKET;

std::vector<SOCKET> m_connectedSocket;

void NetworkSystem::Startup()
{
    m_sendBuffer=new char[256];
	m_sendBuffer[0] = '\0';
	m_recvBuffer = new char[256];
	m_recvBuffer[0] = '\0';
    WSADATA wsaData;
    int errorCode = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (errorCode != 0) {
		printf("WSAStartup failed with error: %d\n", errorCode);
		return;
    }
    m_networkStatus = NET_STATE_IDLE;
}

void NetworkSystem::StartServer(std::string ip, std::string port)
{
   m_listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   unsigned long blockingMode = 1;									
   ioctlsocket(m_listenSocket, FIONBIO, &blockingMode);
   IN_ADDR inAddr;
   inet_pton(AF_INET, ip.c_str(), &inAddr);
   uint32_t myIPAddressU32 = ntohl(inAddr.S_un.S_addr);
   m_ip = ip;
   uint16_t myListenPortU16 = static_cast<unsigned short>(atoi(port.c_str()));
   m_port = myListenPortU16;
   sockaddr_in addr;											// SAME
   addr.sin_family = AF_INET;										// SAME
   addr.sin_addr.S_un.S_addr = htonl(myIPAddressU32);					// SIMILAR
   addr.sin_port = htons(myListenPortU16);							// SIMILAR
   int result = bind(m_listenSocket, (sockaddr*)&addr, (int)sizeof(addr)); // SERVER-SPECIFIC
   UNUSED(result);
   listen(m_listenSocket, SOMAXCONN);

   m_networkStatus = NET_STATE_SERVER_LISTENING;
}

void NetworkSystem::StartClient(std::string ip, std::string port)
{
    clientIP = ip;
    clientPort = port;
	m_connectionToServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);		// SIMILAR

	unsigned long blockingMode = 1;
	ioctlsocket(m_connectionToServer, FIONBIO, &blockingMode);

	IN_ADDR inAddr;												// CLIENT-SPECIFIC
	int result = inet_pton(AF_INET, ip.c_str(), &inAddr); // server’s IP!	// CLIENT-SPECIFIC
	UNUSED(result);
	uint32_t serverIPAddressU32 = ntohl(inAddr.S_un.S_addr);				// SAME
	m_ip = ip;
	uint16_t serverPortU16 = static_cast<unsigned short>(atoi(port.c_str()));		// SAME
	m_port = serverPortU16;
	sockaddr_in addr;											// SAME
	addr.sin_family = AF_INET;										// SAME
	addr.sin_addr.S_un.S_addr = htonl(serverIPAddressU32);				// SAME
	addr.sin_port = htons(serverPortU16);							// SAME
	connect(m_connectionToServer, (sockaddr*)(&addr), (int)sizeof(addr));	// CLIENT-SPECIFIC
	m_networkStatus =NET_STATE_CLIENT_CONNECTING;
}

void NetworkSystem::BeginFrame()
{
    if (m_networkStatus == NET_STATE_SERVER_LISTENING) {
        ServerBeginFrame();
    }
    else if(m_networkStatus==NET_STATE_CLIENT_CONNECTING||m_networkStatus==NET_STATE_CLIENT_CONNECTED) {
        ClientBeginFrame();
    }
}

void NetworkSystem::ServerBeginFrame()
{
	if (m_connectedSocket.size() == 0) {
		SOCKET newClientSocket = accept(m_listenSocket, NULL, NULL);

		if (newClientSocket != INVALID_SOCKET) {
			unsigned long blockingMode = 1;
			ioctlsocket(newClientSocket, FIONBIO, &blockingMode);
			m_connectedSocket.push_back(newClientSocket);
		}
    }
    

    if (m_networkStatus == NET_STATE_SERVER_LISTENING) {
		if (m_connectedSocket.size() > 0) {
			if (m_sendBuffer[0] != '\0') {
				send(m_connectedSocket[0], m_sendBuffer, GetBufferSize(m_sendBuffer) + 1, 0);
				m_sendBuffer[0] = '\0';
			}
			
			recv(m_connectedSocket[0], m_recvBuffer, 256, 0);
			if (m_recvBuffer) {
				m_revcvBufferSize = GetBufferSize(m_recvBuffer);
			}
			

	    }
    }
}

void NetworkSystem::ClientBeginFrame()
{

	

	fd_set writeSockets;	// a list of sockets that can be written-to			// CLIENT-SPECIFIC
	fd_set exceptSockets;	// a list of sockets with errors					// CLIENT-SPECIFIC
	FD_ZERO(&writeSockets);	// like std::vector.clear()					// CLIENT-SPECIFIC
	FD_ZERO(&exceptSockets);	// like std::vector.clear()					// CLIENT-SPECIFIC
	FD_SET(m_connectionToServer, &writeSockets);	// like .push_back()		// CLIENT-SPECIFIC
	FD_SET(m_connectionToServer, &exceptSockets);	// like .push_back()		// CLIENT-SPECIFIC
	timeval waitTime = { };										// CLIENT-SPECIFIC
	select(0, NULL, &writeSockets, &exceptSockets, &waitTime);			// CLIENT-SPECIFIC
	
	
	if(FD_ISSET(m_connectionToServer, &exceptSockets) !=0){
		if (m_networkStatus == NET_STATE_CLIENT_CONNECTING) {
			IN_ADDR inAddr;												// CLIENT-SPECIFIC
			int result = inet_pton(AF_INET, clientIP.c_str(), &inAddr); // server’s IP!	// CLIENT-SPECIFIC
			UNUSED(result);
			uint32_t serverIPAddressU32 = ntohl(inAddr.S_un.S_addr);				// SAME
			uint16_t serverPortU16 = static_cast<unsigned short>(atoi(clientPort.c_str()));		// SAME
			sockaddr_in addr;											// SAME
			addr.sin_family = AF_INET;										// SAME
			addr.sin_addr.S_un.S_addr = htonl(serverIPAddressU32);				// SAME
			addr.sin_port = htons(serverPortU16);							// SAME
			connect(m_connectionToServer, (sockaddr*)(&addr), (int)sizeof(addr));	// CLIENT-SPECIFIC
		}
	}
	if (FD_ISSET(m_connectionToServer, &writeSockets) != 0) {
	    m_networkStatus = NET_STATE_CLIENT_CONNECTED;

	}
	

	if (m_networkStatus == NET_STATE_CLIENT_CONNECTED) {
		
		if (m_sendBuffer[0] != '\0') {
			send(m_connectionToServer, m_sendBuffer, GetBufferSize(m_sendBuffer)+1, 0);	
			m_sendBuffer[0] = '\0';
		}

		recv(m_connectionToServer, m_recvBuffer, 256, 0);
		if (m_recvBuffer) {
		    m_revcvBufferSize = GetBufferSize(m_recvBuffer);
		}

	}
	

}

void NetworkSystem::Disconnect()
{
    if (m_networkStatus == NET_STATE_SERVER_LISTENING&&m_connectedSocket.size()>0) {
        closesocket(m_connectedSocket[0]);
    }
    if (m_networkStatus == NET_STATE_CLIENT_CONNECTED) {
        closesocket(m_connectionToServer);
    }
	m_networkStatus = NET_STATE_IDLE;
    
}

void NetworkSystem::ErrorHandling()
{
	int errorCode = WSAGetLastError();
	UNUSED(errorCode);
}

int NetworkSystem::GetBufferSize(const char* buffer)
{
	int i = 0;
	while( buffer[i] != '\0') {
		i++;
	}
	return i;
}

void NetworkSystem::Shutdown() {
    Disconnect();
}