#ifndef _CONNCECT_H__
#define _CONNCECT_H__

#ifdef CONNECT_DLL
#ifdef CONNECT_LIBRARY_EXPORT
#   define CONNECT_API   __declspec(dllexport)
#else
#   define CONNECT_API   __declspec(dllimport)
#endif /*CONNECT_LIBRARY_EXPORT*/
#else
#   define CONNECT_API
#endif /*CONNECT_DLL*/

enum cnn_type {
	cnn_type_socket   = 0,
	cnn_type_pipe     = 1,
	cnn_type_sharefile= 2,
};

typedef int cnn_handle;
typedef int cnn_error;
class cnn_server_if;
class cnn_client_if;

typedef struct {
	int   cnnType;
	int   port;
	union {
		//cnn type socket
		char* zIpAddr;
		//cnn type pipe
		char* zPipe;
		//share file
		char* zShareFile;
	};
	//in out file
	char* zFile;
	//sharefile
	int   blockSize;
	int   nPage;
} cnn_params;

typedef struct {
	int  cnnType;
	int  port;
	union {
		//if type pipe
		char *zPipe;
		//if type socket
		char *zIpAddr;
		//if share file
		char *zShareFile;
	};
	//share file
	int  nPage;
} cnn_server_params, cnn_client_params;

class cnn_if
{
public:
    virtual cnn_error cnn_recv(void* buff, int size, int *count) = 0;
    virtual cnn_error cnn_send(void* buff, int size, int *sent) = 0;
};

class cnn_server_if :public cnn_if
{
public:
	virtual cnn_handle server_init(cnn_server_params* pParams) = 0;
	//if success return 0
	virtual cnn_error server_open() = 0;
	virtual cnn_error server_listen() = 0;
	virtual cnn_error server_close() = 0;
	virtual cnn_error server_final() = 0;
    virtual cnn_error server_recv(void* buff, int size, int *count) = 0;
    virtual cnn_error server_send(void* buff, int size, int *sent) = 0;

    cnn_error cnn_recv(void* buff, int size, int *count) {
        return server_recv(buff, size, count);
    };
    cnn_error cnn_send(void* buff, int size, int *sent) {
        return server_send(buff, size, sent);
    }
};

class cnn_client_if :public cnn_if
{
public:
	virtual cnn_handle client_init(cnn_client_params *pParams) = 0;
	//if success return 0
	virtual cnn_error client_connect() = 0;
	virtual cnn_error client_shutdown() = 0;
	virtual cnn_error client_final() = 0;
    virtual cnn_error client_send(void* buff, int size, int* sent) = 0;
    virtual cnn_error client_recv(void* buff, int size, int* count) = 0;

    cnn_error cnn_recv(void* buff, int size, int *count) {
        return client_recv(buff, size, count);
    };
    cnn_error cnn_send(void* buff, int size, int *sent) {
        return client_send(buff, size, sent);
    }
};

//APIs
CONNECT_API cnn_error cnn_recv(cnn_handle handle, void* buff, int size, int *count);
CONNECT_API cnn_error cnn_send(cnn_handle handle, void* buff, int size, int *sent);

CONNECT_API cnn_handle server_init(cnn_server_params* pParams);
//if success return 0
CONNECT_API cnn_error server_open(cnn_handle handle);
CONNECT_API cnn_error server_listen(cnn_handle handle);
CONNECT_API cnn_error server_close(cnn_handle handle);
CONNECT_API cnn_error server_final(cnn_handle handle);
CONNECT_API cnn_error server_recv(cnn_handle handle, void* buff, int size, int *count);
CONNECT_API cnn_error server_send(cnn_handle handle, void* buff, int size, int *sent);

CONNECT_API cnn_handle client_init(cnn_client_params *pParams);
//if success return 0
CONNECT_API cnn_error client_connect(cnn_handle handle);
CONNECT_API cnn_error client_shutdown(cnn_handle handle);
CONNECT_API cnn_error client_final(cnn_handle handle);
CONNECT_API cnn_error client_send(cnn_handle handle, void* buff, int size, int* sent);
CONNECT_API cnn_error client_recv(cnn_handle handle, void* buff, int size, int* count);

//basic sent recv
//+ socket
int socket_send(void* pSocket, char*recvbuf, int recvbuflen);
int socket_recv(void* pSocket, char*sendbuf, int sendbuflen);
//+ pipe
int pipe_read(void* pPipe, void* buff, int size, int* nRead);
int pipe_write(void* pPipe, void* buff, int size, int* nWrite);
//+ sharefile
//rm_init()
//PARAMS
//  __in_buff: buff that will hold ring mem object
void* rm_init(void* buff, int size);
//rm_assign()
//PARAMS
//  __in_buff: buff that already store ring mem object
void* rm_assign(void* buff, int size);
int rm_read(void* handle, char *outBuff, int nByte, int *pRead);
int rm_write(void* handle, char *data, int nByte, int *pWrited);
int rm_remain(void* rmHandle);
int rm_pagesize();

//void rm_reset(void* handle, char *buffer, int size);
//get status flags
int rm_get_server_status(void* rmHandle);
int rm_get_client_status(void* rmHandle);
int rm_set_server_status(void* rmHandle, int newStatus);
int rm_set_client_status(void* rmHandle, int newStatus);
int rm_update_server_status(void* rmHandle, int newStatus);
int rm_update_client_status(void* rmHandle, int newStatus);
void* rm_get_hdr_from_ringmem(void* rmHandle);
void* rm_get_ringmem_from_hdr(void* hdr);
int rm_acq_lock(void* hMutex);
int rm_rls_lock(void* hMutex);
int rm_release(void* rmHandle);
int rm_final(void* rmHandle);
typedef struct {
	int server_status;
	int client_status[1];
} sharefile_header;
enum rm_status {
	sharefile_header_size= 32,

	rmstatus_initialized = 0x0001,
	rmstatus_disconected = 0x0002,
	rmstatus_conected    = 0x0004,
	rmstatus_mask        = 0x00FF,

	rmtype_client        = 0x0100,
	rmtype_server        = 0x0200,
	rmtype_mask          = 0x0F00,

	rm_error_notconnected= 0x01,
};
#define MAP_IO_FILE         (1)
//#define MAP_SIZE            (1024)             //by system page size
#define Z_FILESHARE_MUTEX   "fileshareMutex"
#define Z_FILESHARE_EVENT   "fileshareEvent"
#define Z_FILESHARE_NAME    "Global\\MyFileMappingObject"

cnn_server_if * load_cnn_server(char *zDll, cnn_server_params *pParams);
cnn_client_if * load_cnn_client(char *zDll, cnn_client_params *pParams);

cnn_server_if * load_socket_server();
cnn_client_if * load_socket_client();

cnn_server_if * load_pipe_server();
cnn_client_if * load_pipe_client();

cnn_server_if * load_sharefile_server();
cnn_client_if * load_sharefile_client();

int start_server(cnn_params *pParams);
int start_client(cnn_params *pParams);

#endif /*_CONNCECT_H__*/
