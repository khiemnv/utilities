#include "syncwrite.h"
#include "connect.h"

#include <Windows.h>
#include <assert.h>
#include <stdio.h>
#include <vector>

using namespace std;

#define SYNCFILE_SOCKET_PORT    27015
#define ALIGN_BY_16(size)       (((size) + 0x0F) & ~0x0F)

//-----server side-----------
#define TRACE_INSIDE_LOOP (0x01)
#define TRACE_ERROR       (0x02)
#define TRACE_INFO        (0x04)
#define TRACE_DEBUG       (0x08)
#define TRACE_FLAGS       (TRACE_DEBUG | TRACE_INFO | TRACE_ERROR)

#define TRACE_LOG(flag, format, ...) if (TRACE_FLAGS & TRACE_##flag) {printf(format, __VA_ARGS__);}

typedef struct {
    //share and lock
    int state;
    CRITICAL_SECTION lock;
    //connection
    cnn_handle       cnnHandle;
    int     blockSize;
} server_thread_params;
//-----functions declare-----
int initLock(CRITICAL_SECTION* pLock);
int finalLock(CRITICAL_SECTION* pLock);
void enterLock(CRITICAL_SECTION* pLock);
void leaveLock(CRITICAL_SECTION* pLock);
HANDLE my_CreateThread(void* pcallback, void* param, DWORD *pId);

int GetThreadState(server_thread_params *pParams);
int UpdateThreadState(server_thread_params *pParams, int newState);
DWORD WINAPI server_thread_callback(LPVOID lpParam);
//-----lock & thread---------
//int initLock(CRITICAL_SECTION* pLock) {
//    InitializeCriticalSection(pLock);
//    return 0;
//}
//int finalLock(CRITICAL_SECTION* pLock) {
//    DeleteCriticalSection(pLock);
//    return 0;
//}
//void enterLock(CRITICAL_SECTION* pLock)
//{
//    EnterCriticalSection(pLock);
//}
//void leaveLock(CRITICAL_SECTION* pLock)
//{
//    LeaveCriticalSection(pLock);
//}
//HANDLE my_CreateThread(void* pcallback, void* param, DWORD *pId)
//{
//    DWORD tempId;
//    if (pId == 0) {
//        pId = &tempId;
//    }
//    HANDLE hThread = CreateThread(
//        NULL,                   // default security attributes
//        0,                      // use default stack size  
//        (LPTHREAD_START_ROUTINE)pcallback,              // thread function name
//        param,               // argument to thread function 
//        0,                      // use default creation flags 
//        pId);           // returns the thread identifier 
//    assert(sizeof(int) == sizeof(HANDLE));
//    return hThread;
//}
int GetThreadState(server_thread_params *pParams)
{
    int state;
    enterLock(&pParams->lock);
    state = pParams->state;
    leaveLock(&pParams->lock);
    return state;
}
int UpdateThreadState(server_thread_params *pParams, int newState)
{
    int state;
    enterLock(&pParams->lock);
    state = pParams->state;
    pParams->state = newState;
    leaveLock(&pParams->lock);
    return state;
}
//-----init and destroy------
int syncfile::crt_server_process(server_info* hServer)
{
    int rc;

    char *zServerExeFile = "socket.exe";
    char *zServerExeCmd = "socket.exe socket server 127.0.0.1";
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    rc = CreateProcessA(
        zServerExeFile,
        zServerExeCmd,
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &si,
        &pi
        );

    if (rc) {
        //success
        hServer->hProcess = (void*)pi.hProcess;
        hServer->hThread = (void*)pi.hThread;
        rc = 0;
    }
    else {
        rc = GetLastError();
        assert(0);
    }

    return rc;
}

int syncfile::del_server_process(server_info *hServer)
{
    int rc = 0;
    // Wait until child process exits.
    WaitForSingleObject((HANDLE)hServer->hProcess, INFINITE);

    // Close process and thread handles
    CloseHandle((HANDLE)hServer->hProcess);
    CloseHandle((HANDLE)hServer->hThread);
    return rc;
}

//-----server main prog------
int start_server_thread(cnn_params* pParams);
int syncfile::server_main_prog(int argc, char* argv[])
{
    int rc;

    enum { zParamsLen = 256, port = 27015,};

    cnn_params params;
    memset(&params, 0, sizeof(cnn_params));

    char zParams[zParamsLen];

    enum {
        iCnnType = 1,
        iMode = 2,
        iParam = 3,

        iBlockSize = 4,
        iPageCount = 5,

        nArg = 4,
    };

    if (argc >= nArg) {
        //setup params
        if (_stricmp(argv[iCnnType], "socket") == 0) {
            params.cnnType = cnn_type_socket;
            strcpy_s(zParams, zParamsLen, argv[iParam]);
            params.zIpAddr = zParams;
            params.port = port;
        }
        else if (_stricmp(argv[iCnnType], "pipe") == 0) {
            params.cnnType = cnn_type_pipe;
            strcpy_s(zParams, zParamsLen, argv[iParam]);
            params.zPipe = zParams;
        }
        else if (_stricmp(argv[iCnnType], "sharefile") == 0) {
            params.cnnType = cnn_type_sharefile;
            strcpy_s(zParams, zParamsLen, argv[iParam]);
            params.zShareFile = zParams;
        }
        else {
            assert(0);
        }

        //block size
        enum { minBlockSize = 1024, maxBlockSize = 1024 * 1024, };
        int blockSize = minBlockSize;
        if (argc >= (nArg + 1)) {
            blockSize = atoi(argv[iBlockSize]);
            if (blockSize < minBlockSize) {
                blockSize = minBlockSize;
            }
            else if (blockSize > maxBlockSize) {
                blockSize = maxBlockSize;
            }
        }
        params.blockSize = blockSize;

        enum { minPageCount = 1024, maxPageCount = 128 * 1024 };
        int pageCount = minPageCount;
        if (argc >= (nArg + 2)) {
            pageCount = atoi(argv[iPageCount]);
            if (pageCount < minPageCount) {
                pageCount = minPageCount;
            }
            else if (pageCount > maxPageCount) {
                pageCount = maxPageCount;
            }
        }
        params.nPage = pageCount;

        //start
        if (strcmp(argv[iMode], "server") == 0) {
            start_server_thread(&params);
            rc = 0;
        }
        else {
            rc = -1;
        }
    }
    else {
        printf("<socket/pipe/sharefile> <server/client> <ipAddr/namedPipe/sharefilename> [blockSize] [pageCount]\n");
        rc = -1;
    }
    return rc;
}
//-----msg processing--------
HANDLE on_msg_create(CrtMsgData *pData) {
    int rc = 0;
    char* zFileName = &pData->zFile[0];
#if (TRACE_FLAGS & TRACE_INFO)
    printf("create file %s\n", zFileName);
#endif
    HANDLE hf = CreateFileA(
        zFileName,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
        );
    if (hf == INVALID_HANDLE_VALUE) {
        rc = GetLastError();
        assert(0);
#if (TRACE_FLAGS & TRACE_ERROR)
        printf("create file %s rc %d\n", zFileName, rc);
#endif
    }
    else {
        rc = 0;
    }
    return hf;
}
int on_msg_seek(SeekMsgData *pData, HANDLE hFile)
{
    int rc;
    assert(hFile != INVALID_HANDLE_VALUE);
    rc = SetFilePointer(hFile, pData->pos.low, &(pData->pos.hi), FILE_BEGIN);
    if (rc != INVALID_SET_FILE_POINTER) {
        //success
        rc = 0;
    }
    else {
        rc = GetLastError();
        if (rc == NO_ERROR) {
            //success
        }
        else {
            assert(0);
        }
    }
    return rc;
}
int on_msg_read(ReadAckMsgData* pData, int size, HANDLE hFile, int* nRead)
{
    assert(size <= pData->dataSize);
    unsigned long count = 0;
    int rc = ReadFile(hFile, &pData->data[0], size, &count, 0);
    if (rc) {
        //success
        pData->dataSize = count;
        pData->errorCode = 0;
    }
    else {
        //fail
        rc = GetLastError();
        pData->dataSize = 0;
        pData->errorCode = rc;
        assert(0);
    }
    *nRead = count;
    return rc;
}
int on_msg_write(WrtMsgData* pData, int size, HANDLE hFile, int *nWrite)
{
    int rc;
    DWORD count;
    rc = WriteFile(hFile, &pData->data[0], size, &count, 0);
    assert(count == size);
    *nWrite = count;
    return rc;
}
int on_msg_close(HANDLE hFile)
{
    int rc = CloseHandle(hFile);
    if (rc) {
        //success
        rc = 0;
    }
    else {
        rc = GetLastError();
    }
    return rc;
}
//-----server thread---------
int start_server_thread(cnn_params* pParams)
{
    int rc;

    //start server
    cnn_server_params csp;
    cnn_handle pServer = 0;

    if (pParams->cnnType == cnn_type_socket) {
        csp.cnnType = cnn_type_socket;
        csp.zIpAddr = pParams->zIpAddr;
        csp.port = pParams->port;
        pServer = server_init(&csp);
    }
    else if (pParams->cnnType == cnn_type_pipe) {
        csp.cnnType = cnn_type_pipe;
        csp.zPipe = pParams->zPipe;
        pServer = server_init(&csp);
    }
    else if (pParams->cnnType == cnn_type_sharefile) {
        csp.cnnType = cnn_type_sharefile;
        csp.zShareFile = pParams->zShareFile;
        csp.nPage = pParams->nPage;
        pServer = server_init(&csp);
    }
    else {
        assert(0);
    }
    assert(pServer);

    //init thread params
    server_thread_params stp;
    stp.state = 0;
    stp.cnnHandle = pServer;
    stp.blockSize = pParams->blockSize;

    //init lock
    ::initLock(&stp.lock);

    rc = server_open(pServer);
    assert(rc == 0);

    rc = server_listen(pServer);
    assert(rc == 0);

    if (rc == 0) {
        void*  param = &stp;
        DWORD  server_thread_id;
        HANDLE server_thread_handle = my_CreateThread(server_thread_callback, param, &server_thread_id);

        if (server_thread_handle != INVALID_HANDLE_VALUE) {
            for (;;) {
                rc = WaitForSingleObject(server_thread_handle, 0);

                //wait
                Sleep(1000);

                //show status
                enterLock(&stp.lock);
                int curState = stp.state;
                leaveLock(&stp.lock);

                TRACE_LOG(DEBUG, "server thread state %d\n", curState);

                if (rc != WAIT_TIMEOUT) {
                    //complete
                    break;
                }
            }
            CloseHandle(server_thread_handle);

            rc = server_close(pServer);

            TRACE_LOG(INFO, "server closed %d\n", rc);
        }

        //final lock
        ::finalLock(&stp.lock);

        rc = server_final(pServer);
    }
    return rc;
}

//-----thread callback-------
DWORD WINAPI server_thread_callback(LPVOID lpParam)
{
    int  rc;

    server_thread_params *p = (server_thread_params*)lpParam;
    cnn_handle pServer = p->cnnHandle;

    /*ExchgMsg msgHdr;
    vector <char> fullMsg;
    int count;
    enum { hdrSize = sizeof(msgHdr) };
    int remain;
    int iCur;*/
    HANDLE hFile = 0;

    vector <char> buff;
    buff.resize(syncfile::exchgmsg_max_size);
    struct {
        char* p;
        int size;
    } msgBuff = {&buff[0], buff.size()};

    for (;;){
        //get msg
        /*
        //+ get hdr
        rc = server_recv(pServer, (char*)&msgHdr, hdrSize, &count);
        assert(rc == 0);
        if (rc == -1) {
            //client disconnect
            break;
        }
        assert(count == hdrSize);
        //+ get full msg
        remain = msgHdr.msgSize - hdrSize;
        iCur = 0;
        fullMsg.resize(msgHdr.msgSize);

        memcpy(&fullMsg[iCur], &msgHdr, msgHdr.msgSize);
        while (remain) {
            iCur += count;
            rc = server_recv(pServer, (char*)&fullMsg[iCur], remain, &count);
            assert(rc == 0);
            remain -= count;
        }

        //process msg
        int errorCode;
        void* pData = &fullMsg[msgHdr.dataOffset];
        assert(msgHdr.type & exchg_msg_type_req);
        switch (msgHdr.type & exchg_msg_type_msk2)
        */
        //process msg
        ExchgMsg* pMsg = exchgmsg::exchgmsg_recv(pServer, msgBuff.p, msgBuff.size);
        assert(pMsg);
        assert(pMsg->type & exchg_msg_type_req);

        if (pMsg == 0) break;

        int errorCode;
        int ackMsgSize = 0;

        void* pMsgData = (char*)pMsg + pMsg->dataOffset;
        int sizeMsgData = pMsg->dataSize;

        int msgType2 = pMsg->type & exchg_msg_type_msk2;
        switch (msgType2)
        {
        case exchg_msg_type_create:
            assert(hFile == 0);
            hFile = on_msg_create((CrtMsgData*)pMsgData);
            errorCode = (int)hFile;
            break;
        case exchg_msg_type_seek:
            errorCode = on_msg_seek((SeekMsgData*)pMsgData, hFile);
            break;
        case exchg_msg_type_read:
        {
                                    int size = ((ReadMsgData*)pMsgData)->size;
                                    int nRead = 0;
                                    int ReadAckMsgSize = sizeof(ReadAckMsgData) + size;
                                    ackMsgSize = syncfile::exchgmsg_format(msgBuff.p, msgBuff.size,
                                        exchg_msg_type_special | exchg_msg_type_ack | exchg_msg_type_read,
                                        0, ReadAckMsgSize);
                                    ExchgMsg* pAckMsg = (ExchgMsg*)msgBuff.p;
                                    void* pReadAckMsgData = (char*)pAckMsg + pAckMsg->dataOffset;
                                    ((ReadAckMsgData*)pReadAckMsgData)->dataSize = size;
                                    errorCode = on_msg_read((ReadAckMsgData*)pReadAckMsgData, size, hFile, &nRead);
                                    break;
        }
        case exchg_msg_type_write:
        {
                                     int nWrite = 0;
                                     errorCode = on_msg_write((WrtMsgData*)pMsgData, sizeMsgData, hFile, &nWrite);
                                     WrtAckMsgData msg = { errorCode, nWrite };
                                     ackMsgSize = syncfile::exchgmsg_format(msgBuff.p,
                                         msgBuff.size,
                                         exchg_msg_type_special | exchg_msg_type_ack | exchg_msg_type_write,
                                         &msg,
                                         sizeof(WrtAckMsgData));
                                     break;
        }
        case exchg_msg_type_close:
            errorCode = on_msg_close(hFile);
            hFile = 0;
            break;
        default:
            assert(0);
            break;
        }

        //send ack msg
        if (ackMsgSize == 0) {
            //create default ack msg
            ackMsgSize = syncfile::exchgmsg_format(msgBuff.p,
                msgBuff.size,
                exchg_msg_type_ack | msgType2,
                &errorCode,
                sizeof(errorCode));
        }

        int count = 0;
        rc = server_send(pServer, msgBuff.p, ackMsgSize, &count);
        assert(rc == 0);
        assert(count == ackMsgSize);
    }

    return rc;
}
