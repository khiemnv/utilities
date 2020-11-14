#include "syncwrite.h"
#include "connect.h"

#include <Windows.h>
#include <assert.h>
#include <string.h>

#define SYNCFILE_SOCKET_PORT    27015
#define ALIGN_BY_16(size)       (((size) + 0x0F) & ~0x0F)

syncfile::syncfile()
{
    mTempBuff.p = new char[exchgmsg_max_size];
    if (mTempBuff.p) {
        mTempBuff.size = exchgmsg_max_size;
    }
    else {
        mTempBuff.size = 0;
    }
}

syncfile::~syncfile()
{
    if (mTempBuff.p) {
        delete mTempBuff.p; 
    }
}

//-----msg processing--------
//create new msg
//NOTE: should final if unuse
ExchgMsg* exchgmsg::exchgmsg_init(int msgType, void* data, int size)
{
    union {
        char* buff;
        ExchgMsg *pMsg;
    };
    buff = 0;
    int msgSize = 0;
    int dataOffset = 0;

    if (msgType & exchg_msg_type_req) {
        if (msgType & exchg_msg_type_create) {
            //data is char* zFile
        }
        else if (msgType & exchg_msg_type_seek) {
            //data is int* pos
        }
        else if (msgType & exchg_msg_type_write) {
            //raw data
        }
        else if (msgType & exchg_msg_type_read) {
            //data is int* size
        }
        else {
            assert(0);
        }
    }
    else if (msgType & exchg_msg_type_ack) {
        //ack msg
        if (msgType & exchg_msg_type_read) {
            //raw data
        }
        else if (msgType & exchg_msg_type_create) {
            //file handle
        }
        else {
            //default ack msg - error code
        }
    }
    else {
        assert(0);
    }

    msgSize = size + sizeof(ExchgMsg);
    dataOffset = sizeof(ExchgMsg);
    msgSize = ALIGN_BY_16(msgSize);
    assert(msgSize <= exchgmsg_max_size);

    buff = new char[msgSize];
    if (buff) {
        pMsg->msgSize = msgSize;
        pMsg->type = msgType;
        pMsg->dataOffset = dataOffset;
        pMsg->dataSize = size;
        memcpy((char*)pMsg + dataOffset, data, size);
    }
    else {
        assert(0);
    }

    return pMsg;
}
//format a msg
int exchgmsg::exchgmsg_format(void* buff, int buffSize, int msgType, void* data, int dataSize)
{
    int rc = 0;

    int msgSize = dataSize + sizeof(ExchgMsg);
    msgSize = ALIGN_BY_16(msgSize);
    int dataOffset = sizeof(ExchgMsg);
    ExchgMsg* pMsg = (ExchgMsg*)buff;

    if (msgSize <= buffSize) {
        pMsg->msgSize = msgSize;
        pMsg->type = msgType;
        pMsg->dataOffset = dataOffset;
        pMsg->dataSize = dataSize;
        if (data) { memcpy((char*)pMsg + dataOffset, data, dataSize); }

        rc = msgSize;
    }
    else {
        assert(0);
    }

    return rc;
}
//release a msg
int exchgmsg::exchgmsg_final(ExchgMsg* hMsg)
{
    assert(hMsg);
    if (hMsg) {
        delete hMsg;
    }
    return 0;
}
//recv msg and save to buff
//NOTE: if buffer size not enough new msg is create,
//  so should final new msg in this case.
ExchgMsg* exchgmsg::exchgmsg_recv(int cnnHandle, void* buff, int size)
{
    int rc = -1;
    int count;
    int remain, iCur;
    ExchgMsg msgHdr;
    enum { hdrSize = sizeof(ExchgMsg), };
    //+ get hdr
    rc = cnn_recv((cnn_handle)cnnHandle, (char*)&msgHdr, hdrSize, &count);
    assert(rc == 0);
    if (rc == 0) {
        assert(count == hdrSize);
        //+ get full msg
        assert(msgHdr.msgSize <= exchgmsg_max_size);
        remain = msgHdr.msgSize - hdrSize;
        iCur = 0;
        assert(size >= msgHdr.msgSize);

        if (size < msgHdr.msgSize) {
            //if buffer not enough space, alloc a new one.
            //the caller should free it.
            buff = new char[msgHdr.msgSize];
        }

        if (buff) {
            memcpy((char*)buff + iCur, &msgHdr, count);
            while (remain) {
                iCur += count;
                rc = cnn_recv((cnn_handle)cnnHandle, (char*)buff + iCur, remain, &count);
                assert(rc == 0);
                if (rc != 0) break;
                remain -= count;
            }
        }
        else {
            assert(0);
            rc = -1;
        }
    }
    else {
        assert(0);
    }

    if (rc == 0) {
        return (ExchgMsg*)buff;
    }
    else {
        return 0;
    }
}
//create new msg, send msg then final it
int exchgmsg::exchgmsg_send(int cnnHandle, int msgType, void* data, int size)
{
    int rc = -1;
    ExchgMsg *pReqMsg = exchgmsg_init(msgType, data, size);
    if (pReqMsg) {
        //sent req msg
        int sent;
        rc = cnn_send((cnn_handle)cnnHandle, pReqMsg, pReqMsg->msgSize, &sent);
        assert(rc == 0);
        assert(sent == pReqMsg->msgSize);

        exchgmsg_final(pReqMsg);
    }
    return rc;
}

//-----client side-----------
int syncfile::init()
{
    int rc = 0;

    //setup params for socket cnn
    cnn_params params;
    memset(&params, 0, sizeof(cnn_params));
    params.cnnType = cnn_type_socket;
    params.port = SYNCFILE_SOCKET_PORT;
    params.zIpAddr = "127.0.0.1";

    cnn_params *pParams = &params;

    //create server in new process
#if (START_SERVER_MANUAL == 0)
    rc = crt_server_process(&mhServer);
#endif

    //create client
    cnn_client_params ccp;
    cnn_handle pClient = 0;
    if (pParams->cnnType == cnn_type_socket) {
        ccp.cnnType = cnn_type_socket;
        ccp.zIpAddr = pParams->zIpAddr;
        ccp.port = pParams->port;
        pClient = client_init(&ccp);
    }
    else if (pParams->cnnType == cnn_type_pipe) {
        ccp.cnnType = cnn_type_pipe;
        ccp.zPipe = pParams->zPipe;
        pClient = client_init(&ccp);
    }
    else if (pParams->cnnType == cnn_type_sharefile) {
        ccp.cnnType = cnn_type_sharefile;
        ccp.zShareFile = pParams->zShareFile;
        ccp.nPage = pParams->nPage;
        pClient = client_init(&ccp);
    }
    assert(pClient);
    mhClient = pClient;

    //connect to server
    rc = client_connect(pClient);

    return rc;
}

int syncfile::create(
    char* zFile,
    int   dwDesiredAccess,
    int   dwShareMode,
    void* lpSecurityAttributes,
    int   dwCreationDisposition,
    int   dwFlagsAndAttributes,
    void* hTemplateFile
    )
{
    int rc;
    //sent msg req crt
    int dataSize = strlen(zFile) + 1;
    ExchgMsg *pReqMsg = exchgmsg_init(
        exchg_msg_type_req | exchg_msg_type_create,
        zFile, dataSize);
    if (pReqMsg) {
        //sent req msg
        int sent;
        rc = client_send(mhClient, pReqMsg, pReqMsg->msgSize, &sent);
        assert(rc == 0);
        assert(sent == pReqMsg->msgSize);

        if (rc == 0) {
            //wait ack msg
            //get ack msg code
            rc = get_ack_msg_code();
        }

        exchgmsg_final(pReqMsg);
    }
    else {
        rc = -1;
        assert(0);
    }
    return rc;
}
//NOTE: not support overlapped
int syncfile::read(
    void* lpBuffer,
    int   nNumberOfBytesToRead,
    int*  lpNumberOfBytesRead,
    void* lpOverlapped
    )
{
    assert(lpOverlapped == 0);
    int rc;
    //sent msg req read
    ReadMsgData msgData = { nNumberOfBytesToRead };
    int msgSize = exchgmsg_format(mTempBuff.p, mTempBuff.size,
        exchg_msg_type_special | exchg_msg_type_req | exchg_msg_type_read,
        &(msgData), sizeof(msgData));
    assert(msgSize);
    ExchgMsg *pReqMsg = (ExchgMsg*)mTempBuff.p;
    assert(msgSize == pReqMsg->msgSize);
    int count;
    rc = client_send(mhClient, mTempBuff.p, msgSize, &count);
    assert(rc == 0);
    assert(count == msgSize);

    //recv ack msg
    if (rc == 0) {
        ExchgMsg *pAckMsg = exchgmsg_recv(mhClient, mTempBuff.p, mTempBuff.size);
        assert(pAckMsg);
        if (pAckMsg) {
            union {
                void* pData;
                ReadAckMsgData* pReadAck;
            };
            pData = (char*)pAckMsg + pAckMsg->dataOffset;
            rc = pReadAck->errorCode;
            if (rc == 0) {
                assert(pReadAck->dataSize <= nNumberOfBytesToRead);
                *lpNumberOfBytesRead = pReadAck->dataSize;
                memcpy(lpBuffer, &(pReadAck->data[0]), pReadAck->dataSize);
            }
        }
        else {
            assert(0);
            rc = -1;
        }
    }

    return rc;
}
int syncfile::write(
    void* lpBuffer,
    int   nNumberOfBytesToWrite,
    int*  lpNumberOfBytesWritten,
    void* lpOverlapped
    )
{
    int rc;
    //sent msg req write
    WrtMsgData *pMsgData = (WrtMsgData*)lpBuffer;
    assert(lpBuffer == &(pMsgData->data[0]));
    ExchgMsg *pReqMsg = exchgmsg_init(
        exchg_msg_type_special | exchg_msg_type_req | exchg_msg_type_write,
        &(pMsgData->data[0]), nNumberOfBytesToWrite);
    if (pReqMsg) {
        //sent req msg
        int sent;
        rc = client_send(mhClient, pReqMsg, pReqMsg->msgSize, &sent);
        assert(rc == 0);
        assert(sent == pReqMsg->msgSize);

        if (rc == 0) {
            //wait ack msg
            //get return code
            ExchgMsg *pAckMsg = exchgmsg_recv(mhClient, mTempBuff.p, mTempBuff.size);
            assert(pAckMsg);
            if (pAckMsg) {
                void *pData = (char*)pAckMsg + pAckMsg->dataOffset;
                rc = ((WrtAckMsgData*)pData)->errorCode;
                *lpNumberOfBytesWritten = ((WrtAckMsgData*)pData)->nWrite;
            }
        }

        exchgmsg_final(pReqMsg);
    }
    else {
        rc = -1;
        assert(0);
    }
    return rc;
}
//if success func return 0
//else if error func return -1
//NOTE: not support huge seek
int syncfile::seek(
    long lDistanceToMove,
    long* lpDistanceToMoveHigh,
    int dwMoveMethod
    )
{
    assert(lpDistanceToMoveHigh == 0);
    int rc = -1;
    //sent msg req crt
    if (mTempBuff.p) {
        SeekMsgData msgData = { lDistanceToMove, 0 };
        int msgSize = exchgmsg_format(mTempBuff.p,
            mTempBuff.size,
            exchg_msg_type_special | exchg_msg_type_req | exchg_msg_type_seek,
            &msgData,
            sizeof(msgData));

        if (msgSize != 0) {
            //sent req msg
            int sent;
            rc = client_send(mhClient, mTempBuff.p, msgSize, &sent);
            assert(rc == 0);
            assert(sent == msgSize);
            if (rc == 0) {
                //wait ack msg
                rc = get_ack_msg_code();
            }
        }
        else {
            assert(0);
        }
    }
    else {
        assert(0);
    }
    return rc;
}
//if success func return 0
int syncfile::close()
{
    int rc = -1;
    //sent msg req crt
    if (mTempBuff.p) {
        int param = 0;
        int msgSize = exchgmsg_format(mTempBuff.p,
            mTempBuff.size,
            exchg_msg_type_req | exchg_msg_type_close,
            &param,
            sizeof(param));

        if (msgSize != 0) {
            //sent req msg
            int sent;
            rc = client_send(mhClient, mTempBuff.p, msgSize, &sent);
            assert(rc == 0);
            assert(sent == msgSize);

            if (rc == 0) {
                //wait ack msg
                rc = get_ack_msg_code();
                assert(rc == 0);
            }
        }
        else {
            assert(0);
        }
    }
    else {
        assert(0);
    }
    return rc;
}
int syncfile::final()
{
    client_final(mhClient);
#if (START_SERVER_MANUAL == 0)
    del_server_process(&mhServer);
#endif
    delete this;
    return 0;
}

int syncfile::get_ack_msg_code()
{
    int rc = -1;
    //get ack msg
    enum {buffSize = 64,};
    char buff[buffSize];
    ExchgMsg *pAckMsg = exchgmsg_recv(mhClient, buff, buffSize);
    assert(pAckMsg);
    if (pAckMsg) {
        //assert(is_general(ack msg))
        if (pAckMsg->type & exchg_msg_type_special) {
            assert(0);
        }

        //get error code
        void* pData = (char*)pAckMsg + pAckMsg->dataOffset;
        rc = ((AckMsgData*)pData)->errorCode;

        SetLastError(NO_ERROR);
    }
    else {
        SetLastError(-1);
    }
    return rc;
}
//-----APIs implement--------
void* syncfile_create(
    char* zFile,
    int   dwDesiredAccess,
    int   dwShareMode,
    void* lpSecurityAttributes,
    int   dwCreationDisposition,
    int   dwFlagsAndAttributes,
    void* hTemplateFile
    )
{
    syncfile *p;
    int rc = -1;

    p = new syncfile();
    if (p) {
        rc = p->init();
        if (rc == 0) {
            rc = p->create(zFile,
                dwDesiredAccess,
                dwShareMode,
                lpSecurityAttributes,
                dwCreationDisposition,
                dwFlagsAndAttributes, hTemplateFile);
            if (rc != -1) {
                //success
                rc = 0;
            } else {
                //error
                p->final();
            }
        }
        else {
            p->final();
        }
    }
    else {
        rc = -1;
    }

    if (rc == 0) return p;
    else return INVALID_HANDLE_VALUE;
}
int syncfile_read(
    void* hFile,
    void* lpBuffer,
    int   nNumberOfBytesToRead,
    int*  lpNumberOfBytesRead,
    void* lpOverlapped
    )
{
    return ((syncfile*)hFile)->read(lpBuffer,
        nNumberOfBytesToRead,
        lpNumberOfBytesRead,
        lpOverlapped);
}
int syncfile_write(
    void* hFile,
    void* lpBuffer,
    int   nNumberOfBytesToWrite,
    int*  lpNumberOfBytesWritten,
    void* lpOverlapped
    )
{
    return ((syncfile*)hFile)->write(lpBuffer,
        nNumberOfBytesToWrite,
        lpNumberOfBytesWritten,
        lpOverlapped);
}
int syncfile_seek(
    void* hFile,
    long  lDistanceToMove,
    long* lpDistanceToMoveHigh,
    int   dwMoveMethod
    )
{
    return ((syncfile*)hFile)->seek(lDistanceToMove,
        lpDistanceToMoveHigh,
        dwMoveMethod);
}
int syncfile_close(void* hFile)
{
    int rc = ((syncfile*)hFile)->close();
    assert(rc == 0);
    ((syncfile*)hFile)->final();
    return rc;
}
