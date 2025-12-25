#pragma once
#include<vector>
#include <string>
enum {
	NET_STATE_INACTIVE,//: The NetworkSystem has not been Started up.
	NET_STATE_IDLE, // : WSAStartup has been called, but we are neither listening nor connecting.
	NET_STATE_SERVER_LISTENING, // : A non - blocking listen socket has been created using socket() and configured to be non - blocking with ioctlsocket(), bound to a port using bind(), and listen() called on it; incoming connections to that port have accept() called on them, creating a new socket for communication with the newly - connected client.
	NET_STATE_CLIENT_CONNECTING, //: A non - blocking socket has been created using socket() and configured to be non - blocking with ioctlsocket(), then connect() is called initiating attempts to connect to a remote(or local) server IP address and Port number.
	NET_STATE_CLIENT_CONNECTED, // : The above socket has been tested for writeability using select() and is confirmed connected to the server.

};

class NetworkSystem {
public:
	void Startup();
	void StartServer(std::string ip = "127.0.0.1", std::string port = "3100");
	void StartClient(std::string ip = "127.0.0.1", std::string port = "3100");

	void BeginFrame();
	void ServerBeginFrame();
	void ClientBeginFrame();

	void Disconnect();

	void Shutdown();

	void ErrorHandling();
	int GetBufferSize(const char* buffer);

public:
    std::string m_name = "Daniel He";


	unsigned char m_networkStatus = NET_STATE_INACTIVE;
	char* m_sendBuffer;
    char* m_recvBuffer;
	int m_revcvBufferSize = 0;
	std::string m_ip;
	uint16_t m_port = 0;

	std::string clientIP;
	std::string clientPort;

};