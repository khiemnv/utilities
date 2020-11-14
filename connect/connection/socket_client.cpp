#include "connect.h"

#include <assert.h>
#include <winsock2.h>
#include <Ws2tcpip.h>

#include <stdio.h>
#include <string.h>

#pragma comment(lib, "Ws2_32.lib")

//create socket
//connect
#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT   27015
#define SERVER_IP      "127.0.0.1"

int client_connect(char* ipAddr, int port, WSADATA &wsaData, SOCKET &ConnectSocket);
int client_send(SOCKET &ConnectSocket, char* sendbuf, int size);
int client_recv(SOCKET &ConnectSocket, char* recvbuf, int size);
int client_shutdown(SOCKET &ConnectSocket);

class socket_client: public cnn_client_if
{
private:
	enum {ipAddrLen = 64,};
	char    zIpAddr[ipAddrLen];
	int     port;

	WSADATA wsaData;
	SOCKET  ConnectSocket;
public:
	socket_client(){}
	~socket_client(){}
	cnn_handle client_init(cnn_client_params *pParams) {
		assert(pParams->cnnType == cnn_type_socket);
		strcpy_s(zIpAddr, ipAddrLen, pParams->zIpAddr);
		port = pParams->port;
		return (cnn_handle)this;
	}
	cnn_error client_connect() {
		return ::client_connect(zIpAddr, port, wsaData, ConnectSocket);
	}
	cnn_error client_shutdown() {
		return ::client_shutdown(ConnectSocket);
	}
	cnn_error client_final() {
		delete this;
		return 0;
	}
	cnn_error client_send(void* buff, int size, int* sent) {
		int rc = ::client_send(ConnectSocket, (char*) buff, size);
		if (rc > 0) {
			//success
			*sent = rc;
			return 0;
		} else {
			return -1;
		}
	}
    cnn_error client_recv(void* buff, int size, int* count) {
        int rc = ::client_recv(ConnectSocket, (char*)buff, size);
        if (rc > 0) {
            //success
            *count = rc;
            return 0;
        }
        else {
            return -1;
        }
    }
};

int client_connect(char* ipAddr, int port, WSADATA &wsaData, SOCKET &ConnectSocket)
{
	//----------------------
	// Declare and initialize variables.
	int iResult;
	//WSADATA wsaData;

	//SOCKET ConnectSocket = INVALID_SOCKET;
	ConnectSocket = INVALID_SOCKET;
	struct sockaddr_in clientService; 

	int recvbuflen = DEFAULT_BUFLEN;
	char *sendbuf = "Client: sending data test";
	char recvbuf[DEFAULT_BUFLEN] = "";

	//----------------------
	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != NO_ERROR) {
		wprintf(L"WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	//----------------------
	// Create a SOCKET for connecting to server
	ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ConnectSocket == INVALID_SOCKET) {
		wprintf(L"socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	//----------------------
	// The sockaddr_in structure specifies the address family,
	// IP address, and port of the server to be connected to.
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr(ipAddr);
	clientService.sin_port = htons( port );

	//----------------------
	// Connect to server.
	iResult = connect( ConnectSocket, (SOCKADDR*) &clientService, sizeof(clientService) );
	if (iResult == SOCKET_ERROR) {
		wprintf(L"connect failed with error: %d\n", WSAGetLastError() );
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	return 0;
}
//if success func return not zero
//else func return zero
int client_send(SOCKET &ConnectSocket, char* sendbuf, int size) {
	return socket_send(&ConnectSocket, sendbuf, size);
}

int client_recv(SOCKET &ConnectSocket, char* recvbuf, int size) {
    return socket_recv(&ConnectSocket, recvbuf, size);
}

// shutdown the connection since no more data will be sent
//close socket
int client_shutdown(SOCKET &ConnectSocket)
{
	int iResult;
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		wprintf(L"shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	// close the socket
	iResult = closesocket(ConnectSocket);
	if (iResult == SOCKET_ERROR) {
		wprintf(L"close failed with error: %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	WSACleanup();

	return 0;
}

//APIs implement
cnn_client_if * load_socket_client()
{
	socket_client *p = new socket_client();
	return p;
}

int socket_send(void* pSocket, char*sendbuf, int sendbuflen)
{
	SOCKET ConnectSocket = *(SOCKET*)pSocket;
	int iResult;
	iResult = send( ConnectSocket, sendbuf, sendbuflen, 0 );
	if (iResult == SOCKET_ERROR) {
		wprintf(L"send failed with error: %d\n", WSAGetLastError());
		return 0;
	}

	//printf("Bytes Sent: %d\n", iResult);
	return iResult;
}
