#include "connect.h"

#include <windows.h> 
#include <stdio.h> 
#include <tchar.h>
#include <assert.h>

#define BUFSIZE 512

int server_open(char* zPipeName, HANDLE &hPipe);
int server_connect(HANDLE &hPipe);
int server_close(HANDLE &hPipe);
int server_send(HANDLE &hPipe, void* buff, int size, int* sent);
int server_recv(HANDLE &hPipe, void* buff, int size, int* recv);

class pipe_server: public cnn_server_if
{
private:
	enum {pipeNameLen = 260,};
	char   zPipeName[pipeNameLen];
	HANDLE hPipe;
public:
	pipe_server(){}
	~pipe_server(){}
	cnn_handle server_init(cnn_server_params* pParams) {
		assert(pParams->cnnType == cnn_type_pipe);
		strcpy_s(zPipeName, pipeNameLen, pParams->zPipe);
		hPipe = 0;
		return (cnn_handle)this;
	}
	cnn_error server_open() {
		return ::server_open(zPipeName, hPipe);
	}
	cnn_error server_listen() {
		return ::server_connect(hPipe);
	}
	cnn_error server_close() {
		return ::server_close(hPipe);
	}
	cnn_error server_final() {
		delete this;
		return 0;
	}
	cnn_error server_recv(void* buff, int size,int *count) {
		return ::server_recv(hPipe, (char*)buff, size, count);
	}
	cnn_error server_send(void* buff, int size, int* sent) {
		return ::server_send(hPipe, buff, size, sent);
	}
};

//if success func return 0
//else if error func return 1
int server_open(char* zPipeName, HANDLE &hPipe) {
	hPipe = CreateNamedPipe( 
		zPipeName,                // pipe name 
		PIPE_ACCESS_DUPLEX,       // read/write access 
		PIPE_TYPE_MESSAGE |       // message type pipe 
		PIPE_READMODE_MESSAGE |   // message-read mode 
		PIPE_WAIT,                // blocking mode 
		PIPE_UNLIMITED_INSTANCES, // max. instances  
		BUFSIZE,                  // output buffer size 
		BUFSIZE,                  // input buffer size 
		0,                        // client time-out 
		NULL);                    // default security attribute 

	if (hPipe == INVALID_HANDLE_VALUE) 
	{
		_tprintf(TEXT("CreateNamedPipe failed, GLE=%d.\n"), GetLastError()); 
		return 1;
	}
	return 0;
}

//wait until for client connect
int server_connect(HANDLE &hPipe) {
	int rc;
	_tprintf(TEXT("wait client ...\n"));
	rc = ConnectNamedPipe(hPipe, NULL);
	if (rc) {
		rc = 0;
		_tprintf(TEXT("wait client connected\n"));
	} else {
		assert(0);
		rc = GetLastError();
		if (rc == ERROR_IO_PENDING) {
		} else {
			assert(rc == ERROR_PIPE_CONNECTED);
			assert(0);
		}
		_tprintf(TEXT("wait client error %d\n"), rc);
	}
	return rc;
}

int server_close(HANDLE &hPipe) {
	int rc = FlushFileBuffers(hPipe);
	assert(rc);
	rc = CloseHandle(hPipe);
	assert(rc);
	return 0;
}

//if success 0
int server_send(HANDLE &hPipe, void* buff, int size, int* sent) {
	return pipe_write(&hPipe, buff, size, sent);
}
//if success return 0
int server_recv(HANDLE &hPipe, void* buff, int size, int* recv) {
	return pipe_read(&hPipe, buff, size, recv);
}

//APIs implement
cnn_server_if * load_pipe_server() {
	pipe_server *p = new pipe_server();
	return p;
}
