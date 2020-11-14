#include "connect.h"

#include <Windows.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#define TRACE_INSIDE_LOOP (0x01)
#define TRACE_MEASURE     (0x02)
#define TRACE_ERROR       (0x04)
#define TRACE_PROGRESS    (0x08)
#define TRACE_FLAGS       (TRACE_MEASURE | TRACE_PROGRESS)

typedef struct {
	HANDLE  hFile;
	INT64   byteRecv;
	INT64   byteSent;
	CRITICAL_SECTION lock;
	cnn_handle       cnnHandle;
	int     blockSize;
} cnn_thread_params;

//functions declare

//thread
HANDLE my_CreateThread(void* pcallback, void* param, DWORD *pId);
//lock
int initLock(CRITICAL_SECTION* pLock);
int finalLock(CRITICAL_SECTION* pLock);
void enterLock(CRITICAL_SECTION* pLock);
void leaveLock(CRITICAL_SECTION* pLock);
//
void printRecvMsg(INT64 recvBytes);
void printSentMsg(INT64 sentBytes, INT64 totalBytes);

//buffer size
//#define SEND_RECV_BUFF_SIZE 16*1024
DWORD WINAPI cnn_client_thread_callback(LPVOID lpParam);
DWORD WINAPI cnn_server_thread_callback(LPVOID lpParam);

//implement

int start_client(cnn_params *pParams)
{
	int iResult;
	char*  zFin = pParams->zFile;
	//pClient = load_cnn_client(zDll, &ccp);

	//create file
	HANDLE fileHandle = CreateFile(
		zFin,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);

	if (fileHandle != INVALID_HANDLE_VALUE) {
		LARGE_INTEGER fileSize;
		INT64         sentBytes;
		GetFileSizeEx(fileHandle, &fileSize);
		printf("start sent file %s %I64d bytes\n", zFin, fileSize.QuadPart);

		//create socket
		cnn_client_params ccp;
		cnn_handle pClient = 0;
		if (pParams->cnnType == cnn_type_socket) {
			ccp.cnnType = cnn_type_socket;
			ccp.zIpAddr = pParams->zIpAddr;
			ccp.port    = pParams->port;
			pClient = client_init(&ccp);
		} else if (pParams->cnnType == cnn_type_pipe) {
			ccp.cnnType = cnn_type_pipe;
			ccp.zPipe   = pParams->zPipe;
			pClient = client_init(&ccp);
		} else if (pParams->cnnType == cnn_type_sharefile) {
			ccp.cnnType    = cnn_type_sharefile;
			ccp.zShareFile = pParams->zShareFile;
			ccp.nPage = pParams->nPage;
			pClient        = client_init(&ccp);
		}
		assert(pClient);

		//init thread params
		cnn_thread_params ctp;
		ctp.hFile    = fileHandle;
		ctp.byteSent = 0;
		ctp.cnnHandle= pClient;
		ctp.blockSize= pParams->blockSize;

		//init lock
		initLock(&ctp.lock);

		//iResult = client_connect(ipAddr, ctp.wsaData, ctp.soc);
		iResult = client_connect(pClient);
		if (iResult == 0) {
			//start thread
			void*  param = &ctp;
			DWORD  client_thread_id;
			HANDLE client_thread_handle = my_CreateThread(cnn_client_thread_callback, param, &client_thread_id);

			if (client_thread_handle != INVALID_HANDLE_VALUE) {
				for (;;) {
					iResult = WaitForSingleObject(client_thread_handle, 0);

					//show status
					enterLock(&ctp.lock);
					sentBytes = ctp.byteSent;
					leaveLock(&ctp.lock);
#if (TRACE_FLAGS & TRACE_PROGRESS)
					printSentMsg(sentBytes, fileSize.QuadPart);
#endif
					//wait in 1 second
					Sleep(1000);

					//if sent complete ->break
					if (iResult != WAIT_TIMEOUT) {
						break;
					}
				}
				CloseHandle(client_thread_handle);
			}

			//final lock
			finalLock(&ctp.lock);

			iResult = client_shutdown(pClient);

#if (TRACE_FLAGS & TRACE_MEASURE)
			printf("client sent %I64d bytes\n", sentBytes);
#endif
		}

		client_final(pClient);

		CloseHandle(fileHandle);
	}

	return 0;
}

HANDLE my_CreateThread(void* pcallback, void* param, DWORD *pId)
{
	DWORD tempId;
	if (pId == 0) {
		pId = &tempId;
	}
	HANDLE hThread = CreateThread( 
		NULL,                   // default security attributes
		0,                      // use default stack size  
		(LPTHREAD_START_ROUTINE)pcallback,              // thread function name
		param,               // argument to thread function 
		0,                      // use default creation flags 
		pId);           // returns the thread identifier 
	assert(sizeof(int) == sizeof(HANDLE));
	return hThread;
}

int initLock(CRITICAL_SECTION* pLock) {
	InitializeCriticalSection(pLock);
	return 0;
}
int finalLock(CRITICAL_SECTION* pLock){
	DeleteCriticalSection(pLock);
	return 0;
}
void enterLock(CRITICAL_SECTION* pLock)
{
	EnterCriticalSection(pLock);
}
void leaveLock(CRITICAL_SECTION* pLock)
{
	LeaveCriticalSection(pLock);
}

void printRecvMsg(INT64 recvBytes)
{
	//int sizeMB = (int)((recvBytes >> 20) & 0x3FF);
	//int sizeGB = (int)((recvBytes >> 30));
	//printf("recv %04d.%04d (GB)\n", sizeGB, sizeMB);
	double sizeGB = (double)recvBytes / (1024*1024*1024);
	//printf("sent %0.2f%% of %04d.%04d (GB)\n", per, sizeGB, sizeMB);
	printf("recv %0.2f (GB)\n", sizeGB);
	return;
}
void printSentMsg(INT64 sentBytes, INT64 totalBytes)
{
	//int sizeMB = (int)((totalBytes >> 20) & 0x3FF);
	//int sizeGB = (int)((totalBytes >> 30));
	double per = (double)sentBytes / totalBytes * 100;
	//printf("sent %0.2f%% of %04d.%04d (GB)\n", per, sizeGB, sizeMB);
	printf("sent %0.2f%%\n", per);
	return;
}

//
DWORD WINAPI cnn_client_thread_callback(LPVOID lpParam)
{
	int  rc;
	char *buff;
	cnn_thread_params *p = (cnn_thread_params*) lpParam;
	cnn_handle pClient = p->cnnHandle;

	DWORD  nByte;
	int nSent;

	int blockSize = p->blockSize;
	buff = new(char[blockSize]);
	assert(buff);

	if (buff) {
#if (TRACE_FLAGS & TRACE_MEASURE)
	printf("start sent %d milliseconds\n", GetTickCount());
#endif

	do {
		//read from file
		rc = ReadFile(p->hFile, buff, blockSize, &nByte, 0);
#if (TRACE_FLAGS & TRACE_INSIDE_LOOP)
		printf("readfile rc %d size %d\n", rc, nByte);
#endif
		if (rc) {
			if (nByte > 0) {
				//send data
				rc = client_send(pClient, buff, nByte, &nSent);
				if (rc == 0) {
					assert(nByte == nSent);
					enterLock(&p->lock);
					p->byteSent += nSent;
					leaveLock(&p->lock);
				} else {
					//error
				}
			} else {
				//end of file ->break
				rc = 1;
			}
		} else {
			//error
			rc = GetLastError();
			if (ERROR_IO_PENDING == rc) {
				//ERROR_IO_PENDING not error
				continue;
			} else {
				//break loop
			}
		}
	} while( rc == 0 );

#if (TRACE_FLAGS & TRACE_MEASURE)
	printf("end sent %d milliseconds\n", GetTickCount());
#endif

	delete buff;
	} else {
		printf("alloc buff fail!\n");
	}
	return 0;
}

//receive thread
DWORD WINAPI cnn_server_thread_callback(LPVOID lpParam)
{
	int  rc;
	char *buff;
	cnn_thread_params *p = (cnn_thread_params*) lpParam;
	cnn_handle pServer = p->cnnHandle;

	DWORD  nWrite;
	int    flags;
	enum { recv_flag = 0x01, write_flag = 0x02,};
	flags = recv_flag;
	int count;

	int blockSize = p->blockSize;
	buff = new(char[blockSize]);
	assert(buff);

	if (buff) {
#if (TRACE_FLAGS & TRACE_MEASURE)
		printf("start recv %d milliseconds\n", GetTickCount());
#endif

		do {
			//recv data
			if (flags & recv_flag) {
				rc = server_recv(pServer, buff, blockSize, &count);
#if (TRACE_FLAGS & TRACE_INSIDE_LOOP)
				printf("recv %d bytes\n", count);
#endif
				if (rc == 0) {
					flags = write_flag;
					enterLock(&p->lock);
					p->byteRecv += count;
					leaveLock(&p->lock);
				} else {
					//error count == 0
					//break loop
				}
			}

			if (flags & write_flag) {
				//write to file
				rc = WriteFile(p->hFile, buff, count, &nWrite, 0);
				if (rc) {
					//success
					flags = recv_flag;
					rc = 0;
				} else {
					//error
					rc = GetLastError();
					if (rc != ERROR_IO_PENDING) {
						//ERROR_IO_PENDING not error
						rc = 0;  //continue
					} else {
						//break loop
					}
				}
			}
		} while( rc == 0 );

#if (TRACE_FLAGS & TRACE_MEASURE)
		printf("end recv %d milliseconds\n", GetTickCount());
#endif

		delete buff;
	} else {
		printf("alloc buff fail!\n");
	}

	return 0;
}

int start_server(cnn_params *pParams)
{
	int iResult;
	char*  zFout = pParams->zFile;

	//create file
	HANDLE fileHandle = CreateFile(
		zFout,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);

	if (fileHandle != INVALID_HANDLE_VALUE) {
		INT64 recvBytes;

		//start server
		cnn_server_params csp;
		cnn_handle pServer = 0;

		if (pParams->cnnType == cnn_type_socket) {
			csp.cnnType = cnn_type_socket;
			csp.zIpAddr = pParams->zIpAddr;
			csp.port    = pParams->port;
			pServer     = server_init(&csp);
		} else if (pParams->cnnType == cnn_type_pipe) {
			csp.cnnType = cnn_type_pipe;
			csp.zPipe   = pParams->zPipe;
			pServer     = server_init(&csp);
		} else if (pParams->cnnType == cnn_type_sharefile) {
			csp.cnnType = cnn_type_sharefile;
			csp.zShareFile = pParams->zShareFile;
			csp.nPage = pParams->nPage;
			pServer = server_init(&csp);
		} else {
			assert(0);
		}
		assert(pServer);

		//init thread params
		cnn_thread_params stp;
		stp.hFile    = fileHandle;
		stp.byteRecv = 0;
		stp.cnnHandle= pServer;
		stp.blockSize= pParams->blockSize;

		//init lock
		initLock(&stp.lock);

		iResult = server_open(pServer);
		assert(iResult == 0);

		iResult = server_listen(pServer);
		assert(iResult == 0);

		if (iResult == 0) {
			void*  param = &stp;
			DWORD  server_thread_id;
			HANDLE server_thread_handle = my_CreateThread(cnn_server_thread_callback, param, &server_thread_id);

			if (server_thread_handle != INVALID_HANDLE_VALUE) {
				for (;;) {
					iResult = WaitForSingleObject(server_thread_handle, 0);

					//wait
					Sleep(1000);

					//show status
					enterLock(&stp.lock);
					recvBytes = stp.byteRecv;
					leaveLock(&stp.lock);

#if (TRACE_FLAGS & TRACE_PROGRESS)
					printRecvMsg(recvBytes);
#endif

					if (iResult != WAIT_TIMEOUT) {
						//complete
						break;
					}
				}
				CloseHandle(server_thread_handle);

				iResult = server_close(pServer);

#if (TRACE_FLAGS & TRACE_MEASURE)
				printf("server recv %I64d bytes\n", recvBytes);
#endif

			}

			//final lock
			finalLock(&stp.lock);

			iResult = server_final(pServer);
		}

		CloseHandle(fileHandle);
	}

	return 0;
}


//APIs - implement
cnn_server_if* load_cnn_server(char *zDll, cnn_server_params *pParams)
{
	return 0;
}
cnn_client_if* load_cnn_client(char *zDll, cnn_client_params *pParams)
{
	return 0;
}

cnn_handle server_init(cnn_server_params* pParams) {
	cnn_server_if *pServer = 0;
	if (pParams->cnnType == cnn_type_socket) {
		pServer = load_socket_server();
	} else if (pParams->cnnType == cnn_type_pipe) {
		pServer = load_pipe_server();
	} else if (pParams->cnnType == cnn_type_sharefile) {
		pServer = load_sharefile_server();
	} else {
		assert(0);
	}
	if (pServer) {
		return pServer->server_init(pParams);
	}
	return 0;
}
cnn_error server_open(cnn_handle handle) {
	return ((cnn_server_if*)handle)->server_open();
}
cnn_error server_listen(cnn_handle handle) {
	return ((cnn_server_if*)handle)->server_listen();
}
cnn_error server_close(cnn_handle handle) {
	return ((cnn_server_if*)handle)->server_close();
}
cnn_error server_final(cnn_handle handle) {
	return ((cnn_server_if*)handle)->server_final();
}
cnn_error server_recv(cnn_handle handle, void* buff, int size,int *count) {
	return ((cnn_server_if*)handle)->server_recv(buff, size, count);
}
cnn_error server_send(cnn_handle handle, void* buff, int size, int *sent) {
    return ((cnn_server_if*)handle)->server_send(buff, size, sent);
}

cnn_handle client_init(cnn_client_params *pParams) {
	cnn_client_if *pClient = 0;
	if (pParams->cnnType == cnn_type_socket) {
		pClient = load_socket_client();
	} else if (pParams->cnnType == cnn_type_pipe) {
		pClient = load_pipe_client();
	} else if (pParams->cnnType == cnn_type_sharefile) {
		pClient = load_sharefile_client();
	} else {
		assert(0);
	}
	if (pClient) {
		return pClient->client_init(pParams);
	}
	return 0;
}
cnn_error client_connect(cnn_handle handle) {
	return ((cnn_client_if*)handle)->client_connect();
}
cnn_error client_shutdown(cnn_handle handle) {
	return ((cnn_client_if*)handle)->client_shutdown();
}
cnn_error client_final(cnn_handle handle) {
	return ((cnn_client_if*)handle)->client_final();
}
cnn_error client_send(cnn_handle handle, void* buff, int size, int* sent) {
	return ((cnn_client_if*)handle)->client_send(buff, size, sent);
}
cnn_error client_recv(cnn_handle handle, void* buff, int size, int* count) {
    return ((cnn_client_if*)handle)->client_recv(buff, size, count);
}

cnn_error cnn_recv(cnn_handle handle, void* buff, int size, int *count)
{
    return ((cnn_if*)handle)->cnn_recv(buff, size, count);
}
cnn_error cnn_send(cnn_handle handle, void* buff, int size, int *sent)
{
    return ((cnn_if*)handle)->cnn_send(buff, size, sent);
}

