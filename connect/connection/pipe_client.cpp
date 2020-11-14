#include "connect.h"

#include <windows.h>
#include <assert.h>

#if (TRACE_ERROR_LOG == 1)
#include <stdio.h>
#include <tchar.h>
#endif

#define BUFSIZE 512

int client_connect(char* zPipeName, HANDLE &hPipe);
int client_send(HANDLE &hPipe, void* buff, int size, int* sent);
int client_recv(HANDLE &hPipe, void* buff, int size, int* recv);
int client_close(HANDLE &hPipe);

class pipe_client: public cnn_client_if
{
private:
	enum {pipeNameLen = 260,};
	char   zPipeName[pipeNameLen];
	HANDLE hPipe;
public:
	pipe_client(){}
	~pipe_client(){}
	cnn_handle client_init(cnn_client_params *pParams) {
		assert(pParams->cnnType == cnn_type_pipe);
		strcpy_s(zPipeName, pipeNameLen, pParams->zPipe);
		hPipe = 0;
		return (cnn_handle)this;
	}
	cnn_error client_connect() {
		int rc = ::client_connect(zPipeName, hPipe);
		if (rc == 0) {
			//success
		} else if (rc == ERROR_PIPE_BUSY) {
			//need wait and retry
		} else {
			assert(0);
		}
		return 0;
	}
	cnn_error client_shutdown() {
		return ::client_close(hPipe);
	}
	cnn_error client_final() {
		delete this;
		return 0;
	}
	cnn_error client_send(void* buff, int size, int* sent) {
		return ::client_send(hPipe, buff, size, sent);
	}
	cnn_error client_recv(void* buff, int size, int* recv) {
		return ::client_recv(hPipe, buff, size, recv);
	}
};

int client_connect(char* zPipeName, HANDLE &hPipe) {
	hPipe = CreateFile( 
		zPipeName,      // pipe name 
		GENERIC_READ |  // read and write access 
		GENERIC_WRITE, 
		0,              // no sharing 
		NULL,           // default security attributes
		OPEN_EXISTING,  // opens existing pipe 
		0,              // default attributes 
		NULL);          // no template file 
	if (hPipe != INVALID_HANDLE_VALUE) {
		return 0;
	} else {
		return GetLastError();
	}
}

//if success 0
int client_send(HANDLE &hPipe, void* buff, int size, int* sent) {
	return pipe_write(&hPipe, buff, size, sent);
}
//if success return 0
int client_recv(HANDLE &hPipe, void* buff, int size, int* recv) {
	return pipe_read(&hPipe, buff, size, recv);
}

int client_close(HANDLE &hPipe) {
	int rc;
	rc = DisconnectNamedPipe(hPipe);
	//if (rc) {
	//} else {
	//	rc = GetLastError();
	//	printf("client_close DisconnectNamedPipe error %d\n", rc);
	//	assert(0);
	//}
	rc = CloseHandle(hPipe);
	assert(rc);
	return 0;
}
//APIs implement
cnn_client_if* load_pipe_client() {
	pipe_client *p = new pipe_client();
	return p;
}

//if success return 0
int pipe_read(void* pPipe, void* buff, int size, int* recv)
{
	HANDLE hPipe = *(HANDLE*)pPipe;
	int rc;
	DWORD nRead = 0;
	rc = ReadFile(
		hPipe,
		buff,
		size,
		&nRead,
		0);           //not overlapped I/0
	if (rc) {
		//success
		rc = 0;
	} else {
		rc = GetLastError();
	}
	*recv = nRead;
	return rc;
}
int pipe_write(void* pPipe, void* buff, int size, int* sent)
{
	HANDLE hPipe = *(HANDLE*)pPipe;
	int rc;
	DWORD nWrite = 0;
	rc = WriteFile(
		hPipe,
		buff,
		size,
		&nWrite,
		0);           //not overlapped I/0
	if (rc) {
		//success
		rc = 0;
	} else {
		rc = GetLastError();
	}
	*sent = nWrite;
	return rc;
}
