#include "connect.h"

#include <assert.h>
#include <winsock2.h>
#include <Ws2tcpip.h>

#include <stdio.h>
#include <string.h>

#pragma comment(lib, "Ws2_32.lib")

#define SERVER_IP   "127.0.0.1"
#define SERVER_PORT 27015

//start Winsock
int server_init(WSADATA &wsaData);
//bind listen socket
int server_open(char*ipAddr, int port, SOCKET &ListenSocket);
//listen and accept client connect
int server_listen(char*ipAddr, int port, WSADATA &wsaData, SOCKET &ListenSocket, SOCKET &AcceptSocket);
int server_recv(SOCKET &ConnectSocket, char*recvbuf, int recvbuflen);
int server_send(SOCKET &ConnectSocket, char*sendbuf, int sendbuflen);
int server_close(SOCKET &ConnectSocket);
//clear Winsock
int server_final(WSADATA &wsaData);

class socket_server: public cnn_server_if
{
private:
	enum {ipAddrLen = 64,};
	char zIpAddr[ipAddrLen];
	int  port;

	WSADATA wsaData;
	SOCKET  ListenSocket;

	enum {
		default_client = 0,
		max_clients    = 5,

		client_state_free      = 0,
		client_state_connected = 0x01,
	};
	struct client_item_ {
		SOCKET  AcceptSocket;
		int     state;
	} arrClients[max_clients];
	int iSelectedClient;
public:
	socket_server(){}
	~socket_server(){}
	cnn_handle server_init(cnn_server_params* pParams) {
		assert(pParams->cnnType == cnn_type_socket);
		strcpy_s(zIpAddr, ipAddrLen, pParams->zIpAddr);
		port = pParams->port;

		//init client manager
		for (int i = 0; i < max_clients; i++) {
			arrClients[i].state = client_state_free;
		}
		iSelectedClient = default_client;

		//start up Winsock
		::server_init(wsaData);

		return (cnn_handle)this;
	}
	cnn_error server_open() {
		return ::server_open(zIpAddr, port, ListenSocket);
	}
	cnn_error server_listen() {
		return ::server_listen(zIpAddr, port, wsaData, ListenSocket, arrClients[iSelectedClient].AcceptSocket);
	}
	cnn_error server_close() {
		return ::server_close(arrClients[iSelectedClient].AcceptSocket);
	}
	cnn_error server_final() {
		::server_final(wsaData);
		delete this;
		return 0;
	}
	cnn_error server_recv(void* buff, int size,int *count) {
		int rc = ::server_recv(arrClients[iSelectedClient].AcceptSocket, (char*)buff, size);
		if (rc > 0) {
			//success
			*count = rc;
			return 0;
		} else {
			return -1;
		}
	}
    cnn_error server_send(void* buff, int size, int *sent) {
        int rc = ::server_send(arrClients[iSelectedClient].AcceptSocket, (char*)buff, size);
        if (rc > 0) {
            //success
            *sent = rc;
            return 0;
        }
        else {
            return -1;
        }
    }
};

int server_init(WSADATA &wsaData) {
	//----------------------
	// Initialize Winsock.
	//WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		wprintf(L"WSAStartup failed with error: %ld\n", iResult);
		return 1;
	}
	return 0;
}

//create socket
//bind
//if success func return 0
//else if error func return 1
int server_open(char*ipAddr, int port, SOCKET &ListenSocket)
{
	//----------------------
	// Create a SOCKET for listening for
	// incoming connection requests.
	//SOCKET ListenSocket;
	ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ListenSocket == INVALID_SOCKET) {
		wprintf(L"socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	//----------------------
	// The sockaddr_in structure specifies the address family,
	// IP address, and port for the socket that is being bound.
	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr(ipAddr);
	service.sin_port = htons(port);

	if (bind(ListenSocket,
		(SOCKADDR *) & service, sizeof (service)) == SOCKET_ERROR) {
			wprintf(L"bind failed with error: %ld\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return 1;
	}

	return 0;
}
//listen
//accept
//if success func return 0
//else if error func return 1
int server_listen(char*ipAddr, int port, WSADATA &wsaData, SOCKET &ListenSocket, SOCKET &AcceptSocket)
{
	//----------------------
	// Listen for incoming connection requests.
	// on the created socket
	if (listen(ListenSocket, 1) == SOCKET_ERROR) {
		wprintf(L"listen failed with error: %ld\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	//----------------------
	// Create a SOCKET for accepting incoming requests.
	//SOCKET AcceptSocket;
	wprintf(L"Waiting for client to connect...\n");

	//----------------------
	// Accept the connection.
	AcceptSocket = accept(ListenSocket, NULL, NULL);
	if (AcceptSocket == INVALID_SOCKET) {
		wprintf(L"accept failed with error: %ld\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	} else
		wprintf(L"Client connected.\n");

	return 0;
}

int server_recv(SOCKET &ConnectSocket, char*recvbuf, int recvbuflen) {
	return socket_recv(&ConnectSocket, recvbuf, recvbuflen);
}

int server_send(SOCKET &ConnectSocket, char*sendbuf, int sendbuflen) {
    return socket_send(&ConnectSocket, sendbuf, sendbuflen);
}

int server_close(SOCKET &ConnectSocket)
{
	// cleanup
	closesocket(ConnectSocket);
	return 0;
}

int server_final(WSADATA &wsaData) {
	WSACleanup();
	return 0;
}

//APIs implement
cnn_server_if * load_socket_server()
{
	socket_server* p = new socket_server();
	return p;
}

int socket_recv(void* pSocket, char*recvbuf, int recvbuflen)
{
	// Receive until the peer closes the connection
	SOCKET ConnectSocket = *(SOCKET*)pSocket;
	int iResult;

	iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
	if ( iResult > 0 ) {
		//printf("Bytes received: %d\n", iResult);
	} else if ( iResult == 0 ) {
		printf("Connection closed\n");
	} else {
		printf("recv failed: %d\n", WSAGetLastError());
	}

	return iResult;
}
