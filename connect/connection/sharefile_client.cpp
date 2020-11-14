#include "connect.h"
#include "rmem.h"

#include <Windows.h>
#include <assert.h>

#define ALIGN_BY(size, block) ((size + block - 1) & (~(block -1)))

int sharefile_client_init(char* zEvent, HANDLE &hEvent, char* zMutex, HANDLE &hMutex);
int sharefile_client_connect(char* zFile, __int64 nMapsize, HANDLE hFile, HANDLE &hMapfile, void* &hMapaddr, void* &hRingmem);
int sharefile_client_shutdown(HANDLE hMapfile, HANDLE hMapaddr);
int sharefile_client_final(HANDLE hEvent, HANDLE hMutex);
int sharefile_client_sent(HANDLE hMutex, void* hRingmem, void* buff, int size, int* sent);

static int gPageSize = rm_pagesize();

class sharefile_client: public cnn_client_if
{
private:
	enum   {shareFileNameLen = 260,};
	char   zShareFile[shareFileNameLen];
	HANDLE hFile;
	HANDLE hMapfile;
	HANDLE hMapaddr;
	HANDLE hEvent;
	HANDLE hMutex;
	void*  hRingmem;
	int    nPage;   //map size = nPage * system page size
public:
	sharefile_client(){}
	~sharefile_client(){}
	cnn_handle client_init(cnn_client_params *pParams) {
		assert(pParams->cnnType == cnn_type_sharefile);
		strcpy_s(zShareFile, shareFileNameLen, pParams->zShareFile);
		hRingmem = 0;
		hFile    = 0;
		nPage    = pParams->nPage;
		int rc = sharefile_client_init(Z_FILESHARE_EVENT, hEvent, Z_FILESHARE_MUTEX, hMutex);
		assert(rc == 0);
		return (cnn_handle)this;
	}
	cnn_error client_connect() {
		__int64 mapSize = nPage;
		mapSize *= gPageSize;
		int rc = sharefile_client_connect(zShareFile, mapSize, hFile, hMapfile, hMapaddr, hRingmem);
#if (1)
		if (rc == 0) {
			rm_acq_lock(hMutex);
			//ref hRingmem to share RingMem object
			hRingmem = rm_assign(hMapaddr, (int)mapSize);
			//change to connected state
			rm_update_client_status(hRingmem, rmstatus_conected);
			rm_rls_lock(hMutex);
			//notify to server
			SetEvent(hEvent);
		}
#endif
		return rc;
	}
	cnn_error client_shutdown() {
		int rc;
		rm_acq_lock(hMutex);
		rm_update_client_status(hRingmem, rmstatus_disconected);
		rm_release(hRingmem);
		rm_rls_lock(hMutex);
		rc = sharefile_client_shutdown(hMapfile, hMapaddr);
		return rc;
	}
	cnn_error client_final() {
		int rc = sharefile_client_final(hEvent, hMutex);
		delete this;
		return rc;
	}
	cnn_error client_send(void* buff, int size, int* sent) {
		int rc = -1;
		int remain;
		int i; //retry counter
		enum {retryTime = 0x10};
		for (i = 1; i < retryTime; i++) {
			rm_acq_lock(hMutex);
			remain = rm_remain(hRingmem);
			if (remain >= size) {
				rc = sharefile_client_sent(hMutex, hRingmem, buff, size, sent);
				i += retryTime; //break the loop
			}
			rm_rls_lock(hMutex);
		}
		if (i == retryTime) {
			//retry time elapsed
			assert(0);
		}
		
		return rc;
	}
    cnn_error client_recv(void* buff, int size, int* count) {
        return 0;
    }
};

int sharefile_client_init(char* zEvent, HANDLE &hEvent, char* zMutex, HANDLE &hMutex) {
	int rc = 0;
	int error = 0;
	enum {
		OpenEventError = 0x01,
		OpenMutexError = 0x02,
	};

	hEvent = OpenEvent(EVENT_ALL_ACCESS , TRUE, zEvent);
	if (hEvent) {
		//success
	} else {
		rc = GetLastError();
		error |= OpenEventError;
		assert(0);
	}

	hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, zMutex);
	if (hMutex) {
	} else {
		rc = GetLastError();
		error |= OpenMutexError;
		assert(0);
	}

	return error;
}

int sharefile_client_connect(char* zFile, __int64 nMapsize, HANDLE hFile, HANDLE &hMapfile, void* &hMapaddr, void* &hRingmem) {
	int rc;

	LARGE_INTEGER mapsize, mapbegin;
	mapsize.QuadPart  = nMapsize;
	mapbegin.QuadPart = 0;

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
		//open file mapping
		hMapfile = OpenFileMapping(
			FILE_MAP_ALL_ACCESS,
			FALSE,
			Z_FILESHARE_NAME
			);
#endif
		if (hMapfile) {
			hMapaddr = MapViewOfFile (
				hMapfile,
				FILE_MAP_WRITE,
				mapbegin.HighPart, mapbegin.LowPart,
				mapsize.LowPart
				);
			if (hMapaddr) {
				//success
				rc = 0;
			} else {
				rc = GetLastError();
				assert(0);
			}
		} else {
			rc = GetLastError();
			assert(0);
		}
	} else {
		rc = GetLastError();
		assert(0);
	}
	return rc;
}

int sharefile_client_shutdown(HANDLE hMapfile, HANDLE hMapaddr) {
	int rc;
	rc = UnmapViewOfFile(hMapaddr);
	assert(rc);
	rc = CloseHandle(hMapfile);
	assert(rc);
	return 0;
}
int sharefile_client_final(HANDLE hEvent, HANDLE hMutex) {
	CloseHandle(hEvent);
	CloseHandle(hMutex);
	return 0;
}
int sharefile_client_sent(HANDLE hMutex, void* hRingmem, void* buff, int size, int* sent) {
	int rc;
	rc = rm_get_server_status(hRingmem);
	if (rc & rmstatus_conected) {
		rc = rm_write(hRingmem, (char*)buff, size, sent);
		assert(rc == 0);
	} else {
		rc = rm_error_notconnected;
		assert(0);
	}
	return rc;
}
//C APIs implement
void* rm_init(void* buff, int size)
{
	//setup init flags
	sharefile_header* pHdr = (sharefile_header*)buff;
	pHdr->server_status = rmstatus_initialized | rmtype_server;

	//init rmem
	RingMem *p = (RingMem*)rm_get_ringmem_from_hdr(pHdr);
	int hdrSize = (char*)p - (char*)buff;

	assert(hdrSize == ALIGN_BY(sharefile_header_size, gPageSize));
	int remainSize = size - hdrSize;

	p->Reset((char*)p, remainSize, 1);

	return p;
}
void* rm_assign(void* buff, int size)
{
	//setup init flags
	sharefile_header* pHdr = (sharefile_header*)buff;
	assert(pHdr->server_status & rmstatus_initialized);
	assert(pHdr->server_status & rmtype_server);
	pHdr->client_status[0] = rmstatus_initialized | rmtype_client;

	//init rmem
	RingMem* p = (RingMem*)rm_get_ringmem_from_hdr(pHdr);
	int remain = p->Remain();
	int offset = (char*)p - (char*)pHdr;
	offset += ALIGN_BY(sizeof(RingMem), gPageSize);

	return p;
}

int rm_release(void* rmHandle)
{
	int rc = 0;
	//setup init flags
	sharefile_header* pHdr = (sharefile_header*)rm_get_hdr_from_ringmem(rmHandle);
	assert(pHdr->client_status[0] & rmstatus_disconected);
	assert(pHdr->client_status[0] & rmtype_client);
	pHdr->client_status[0] = 0;
	return rc;
}
int rm_final(void* rmHandle)
{
	int rc = 0;
	sharefile_header* pHdr = (sharefile_header*)rm_get_hdr_from_ringmem(rmHandle);
	assert(pHdr->server_status & rmstatus_disconected);
	assert(pHdr->server_status & rmtype_server);
	pHdr->server_status = 0;
	return rc;
}
void* rm_get_hdr_from_ringmem(void* hRingmem) {
	int offset = ALIGN_BY(sharefile_header_size, gPageSize);
	return ((char*)hRingmem - offset);
}
void* rm_get_ringmem_from_hdr(void* hdr) {
	int offset = ALIGN_BY(sharefile_header_size, gPageSize);
	return ((char*)hdr + offset);
}
int rm_acq_lock(void* hMutex) {
	return WaitForSingleObject(hMutex, INFINITE);
}
int rm_rls_lock(void* hMutex) {
	return ReleaseMutex(hMutex);
}
int rm_get_server_status(void* rmHandle) {
	sharefile_header *p = (sharefile_header*)rm_get_hdr_from_ringmem(rmHandle);
	return p->server_status;
};
int rm_get_client_status(void* rmHandle) {
	sharefile_header *p = (sharefile_header*)rm_get_hdr_from_ringmem(rmHandle);
	return p->client_status[0];
};
int rm_set_server_status(void* rmHandle, int newStatus) {
	sharefile_header *p = (sharefile_header*)rm_get_hdr_from_ringmem(rmHandle);
	p->server_status = newStatus;
	return 0;
}
int rm_set_client_status(void* rmHandle, int newStatus) {
	sharefile_header *p = (sharefile_header*)rm_get_hdr_from_ringmem(rmHandle);
	p->client_status[0] = newStatus;
	return 0;
}
static int rm_update_status(int rmStatus, int newStatus) {
	switch (rmstatus_mask & newStatus) {
		case rmstatus_initialized:
		case rmstatus_disconected:
		case rmstatus_conected:
			rmStatus &= ~rmstatus_mask;
			rmStatus |= newStatus;
			break;
		default:
			assert(0);
			break;
	}
	return rmStatus;
}
int rm_update_server_status(void* rmHandle, int newStatus) {
	sharefile_header *p = (sharefile_header*)rm_get_hdr_from_ringmem(rmHandle);
	p->server_status = rm_update_status(p->server_status, newStatus);
	return 0;
}
int rm_update_client_status(void* rmHandle, int newStatus) {
	sharefile_header *p = (sharefile_header*)rm_get_hdr_from_ringmem(rmHandle);
	p->client_status[0] = rm_update_status(p->client_status[0], newStatus);
	return 0;
}
int rm_read(void* handle, char *outBuff, int nByte, int *pRead)
{
	return ((RingMem*)handle)->Read(outBuff, nByte, pRead);
}
int rm_write(void* handle, char *data, int nByte, int *pWrited)
{
	return ((RingMem*)handle)->Write(data, nByte, pWrited);
}
int rm_remain(void* rmHandle)
{
	return ((RingMem*)rmHandle)->Remain();
}
int rm_pagesize() {
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	return si.dwPageSize;
}
//
//void rm_reset(void* handle, char *buffer, int size)
//{
//	return ((RingMem*)handle)->Reset(buffer, size);
//}

cnn_client_if* load_sharefile_client() {
	sharefile_client* p = new sharefile_client();
	return p;
}
