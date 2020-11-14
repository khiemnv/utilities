#include "connect.h"
#include "rmem.h"

#include <Windows.h>
#include <assert.h>

int sharefile_server_init(char* zEvent, HANDLE &hEvent, char* zMutex, HANDLE &hMutex);
int sharefile_server_open(char* zFile, __int64 nMapsize, HANDLE &hFile, HANDLE &hMapfile, void* &hMapaddr, void* &hRingmem);
int sharefile_server_listen(HANDLE hEvent, DWORD timeOut);
int sharefile_server_close(HANDLE hFile, HANDLE hMapfile, void* hMapaddr);
int sharefile_server_final(HANDLE hEvent, HANDLE hMutex);
int sharefile_server_recv(void* hRingmem, HANDLE hMutex, void* buff, int size,int *count);

static int gPageSize = rm_pagesize();

class sharefile_server: public cnn_server_if
{
private:
	enum {shareFileNameLen = 260,};
	char   zShareFile[shareFileNameLen];
	HANDLE hFile;
	HANDLE hMapfile;
	HANDLE hMapaddr;
	HANDLE hEvent;
	HANDLE hMutex;
	void*  hRingmem;
	int    nPage;
public:
	sharefile_server(){}
	~sharefile_server(){}
	cnn_handle server_init(cnn_server_params* pParams) {
		assert(pParams->cnnType == cnn_type_sharefile);
		strcpy_s(zShareFile, shareFileNameLen, pParams->zShareFile);
		hFile = 0;
		nPage = pParams->nPage;
		int rc = sharefile_server_init(Z_FILESHARE_EVENT, hEvent, Z_FILESHARE_MUTEX, hMutex);
		assert(rc == 0);
		return (cnn_handle)this;
	}
	cnn_error server_open() {
		__int64 mapSize = nPage;
		mapSize *= gPageSize;
		int rc = sharefile_server_open(zShareFile, mapSize, hFile, hMapfile, hMapaddr, hRingmem);
		if (rc == 0) {
			//success
			rm_acq_lock(hMutex);
			hRingmem = rm_init(hMapaddr, (int)mapSize);
			rm_rls_lock(hMutex);
		}
		return rc;
	}
	cnn_error server_listen() {
		int rc = sharefile_server_listen(hEvent, INFINITE);
		if (rc == 0) {
			rm_acq_lock(hMutex);
			rm_update_server_status(hRingmem, rmstatus_conected);
			rm_rls_lock(hMutex);
		}
		return rc;
	}
	cnn_error server_close() {
		rm_acq_lock(hMutex);
		rm_update_server_status(hRingmem, rmstatus_disconected);
		rm_final(hRingmem);
		rm_rls_lock(hMutex);
		return sharefile_server_close(hFile, hMapfile, hMapaddr);
	}
	cnn_error server_final() {
		int rc = sharefile_server_final(hEvent, hMutex);
		delete this;
		return 0;
	}
	cnn_error server_recv(void* buff, int size,int *pBytes) {
		int rc;
		rm_acq_lock(hMutex);
		rc = sharefile_server_recv(hRingmem, hMutex, buff, size, pBytes);
		rm_rls_lock(hMutex);
		if (rc == rm_error_notconnected) {
			//client complete transfer
			rc = rm_read(hRingmem, (char*)buff, size, pBytes);
			assert(rc == 0);
			if (*pBytes == 0) {
				//no more data
				rc = -1;
			}
		} else {
			assert(rc == 0);
		}
		return rc;
	}
	cnn_error server_send(void* buff, int size, int* sent) {
		return 0;
	}
};

int sharefile_server_init(char* zEvent, HANDLE &hEvent, char* zMutex, HANDLE &hMutex)
{
	int rc = 0;
	int error = 0;
	enum {
		CreateEventError = 0x01,
		CreateMutexError = 0x02,
	};

	SECURITY_ATTRIBUTES event_sa;
	ZeroMemory( &event_sa, sizeof(event_sa) );
	event_sa.nLength = sizeof(event_sa);
	event_sa.bInheritHandle = TRUE;

	hEvent = CreateEvent(&event_sa, TRUE, TRUE, zEvent);
	if (hEvent) {
		//success
	} else {
		rc = GetLastError();
		error |= CreateEventError;
	}

	hMutex = CreateMutex(NULL, FALSE, zMutex);
	if (hMutex) {
	} else {
		rc = GetLastError();
		error |= CreateMutexError;
	}

	return error;
}
int sharefile_server_final(HANDLE hEvent, HANDLE hMutex)
{
	int rc = 0;
	rc = CloseHandle(hEvent);
	if (rc) {
	} else {
		rc = GetLastError();
		assert(0);
	}
	rc = CloseHandle(hMutex);
	if (rc) {
	} else {
		rc = GetLastError();
		assert(0);
	}
	return 0;
}
//return 0 if success
int sharefile_server_open(char* zFile, __int64 nMapsize, HANDLE &hFile, HANDLE &hMapfile, void* &hMapaddr, void* &hRingmem) {
	int rc;
	int error = 0;

	LARGE_INTEGER mapsize, mapbegin;
	mapsize.QuadPart  = nMapsize;
	mapbegin.QuadPart = 0;

	//open file mapping
	hFile = CreateFile(
		zFile,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,//&sa1,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);
	if (hFile != INVALID_HANDLE_VALUE) {
#if (MAP_IO_FILE)
		//SECURITY_ATTRIBUTES sa;
		//ZeroMemory( &sa, sizeof(sa) );
		//sa.nLength = sizeof(sa);
		//sa.bInheritHandle = TRUE;
		hMapfile = CreateFileMapping(
			hFile,
			//INVALID_HANDLE_VALUE,
			//&sa,
			NULL,
			PAGE_READWRITE,
			mapsize.HighPart, mapsize.LowPart,
			//Z_FILESHARE_NAME//fileName
			NULL
			);
#else
		hMapfile = CreateFileMapping(//hFile,
			INVALID_HANDLE_VALUE,
			//&sa,
			NULL,
			PAGE_READWRITE,
			mapsize.HighPart, mapsize.LowPart,
			Z_FILESHARE_NAME//fileName
			);
#endif
		if (hMapfile) {
			hMapaddr = MapViewOfFile (
				hMapfile,
				FILE_MAP_WRITE,
				mapbegin.HighPart,
				mapbegin.LowPart,
				mapsize.LowPart
				);
			if (hMapaddr) {
				rc = 0;
			} else {
				rc = GetLastError();
				error |= 0x04;
				assert(0);
			}
		} else {
			rc = GetLastError();
			error |= 0x02;
			assert(0);
		}
	} else {
		rc = GetLastError();
		error |= 0x01;
		assert(0);
	}
	return error;
}

int sharefile_server_listen(HANDLE hEvent, DWORD timeOut)
{
	int rc = 0;
	rc = ResetEvent(hEvent);
	if (rc) {
		rc = WaitForSingleObject(hEvent, timeOut);
		if (rc == WAIT_OBJECT_0) {
			//client connected
		} else {
			//error
			assert(0);
		}
	} else {
		rc = GetLastError();
		assert(0);
	}
	return rc;
}

int sharefile_server_close(HANDLE hFile, HANDLE hMapfile, void* hMapaddr)
{
	int rc;
	int error = 0;
	rc = UnmapViewOfFile(hMapaddr);
	if (rc) {
	} else {
		rc = GetLastError();
		error |= 0x01;
		assert(0);
	}
	rc = CloseHandle(hMapfile);
	if (rc) {
	} else {
		rc = GetLastError();
		error |= 0x02;
		assert(0);
	}
	rc = CloseHandle(hFile);
	if (rc) {
	} else {
		rc = GetLastError();
		error |= 0x04;
		assert(0);
	}
	return error;
}

int sharefile_server_recv(void* hRingmem, HANDLE hMutex, void* buff, int size,int *count)
{
	int rc;
	rc = rm_get_client_status(hRingmem);
	if (rc & rmstatus_conected) {
		//success
		rc = rm_read(hRingmem, (char*)buff, size, count);
		assert(rc == 0);
	} else {
		rc = rm_error_notconnected;
	}
	return rc;
}

cnn_server_if * load_sharefile_server() {
	sharefile_server *p = new sharefile_server();
	return p;
}
