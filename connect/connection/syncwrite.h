#ifndef SYNC_WRITE_H
#define SYNC_WRITE_H

#ifdef SYNCWRITE_DLL
#ifdef SYNCWRITE_LIBRARY_EXPORT
#   define SYNCWRITE_API   __declspec(dllexport)
#else
#   define SYNCWRITE_API   __declspec(dllimport)
#endif /*SYNCWRITE_LIBRARY_EXPORT*/
#else
#   define SYNCWRITE_API
#endif /*SYNCWRITE_DLL*/

#define START_SERVER_MANUAL (1)

typedef struct exchg_msg_ {
    int msgSize;   //msg size in byte
    int type;
    int dataSize;  //data size in byte
    int dataOffset;
} ExchgMsg;

//exche msg data depend on msg type
enum exchg_msg_type_ {
    exchg_msg_type_req  = 0x0100,
    exchg_msg_type_ack  = 0x0200,
    exchg_msg_type_msk1 = 0x0F00,

    exchg_msg_type_general = 0x0000,
    exchg_msg_type_special = 0x1000,

    exchg_msg_type_create   =  1, //ceate file
    exchg_msg_type_seek     =  2, //seek
    exchg_msg_type_write    =  3, //write
    exchg_msg_type_read     =  4, //read
    exchg_msg_type_close    =  5, //close
    exchg_msg_type_msk2     = 0x00FF,
};

//general msg data
typedef struct msg_data_ack_ {
    int errorCode;
}AckMsgData;
typedef struct msg_data_req_ {
    int param;
}ReqMsgData;
//special msg data
typedef struct msg_data_create_req_ {
    char zFile[1];
}CrtMsgData;
typedef struct msg_data_seek_req_ {
    struct {
        unsigned long low;
        long hi;
    } pos;
}SeekMsgData;
typedef struct msg_data_write_req_ {
    char data[1];
}WrtMsgData;
typedef struct msg_data_write_ack_ {
    int errorCode;
    int nWrite;
}WrtAckMsgData;
typedef struct msg_data_read_req_ {
    int size;
}ReadMsgData;
typedef struct msg_data_read_ack_ {
    int errorCode;
    int dataSize;
    char data[1];
}ReadAckMsgData;

class exchgmsg
{
public:
    enum { exchgmsg_max_size = 4 * 1024 * 1024 };
    //if success func return pointer to new exchgmsg
    //else if error func return 0
    //NOTE: should final new exchgmsg
    static ExchgMsg* exchgmsg_init(int msgType, void* data, int size);
    static ExchgMsg* exchgmsg_recv(int cnnHandle, void*buff, int size);
    //if success func return 0
    static int exchgmsg_send(int cnnHandle, int msgType, void* data, int size);
    //if success func return msgSize (!=0)
    static int exchgmsg_format(void* buff, int buffSize, int msgType, void* data, int dataSize);
    //if success func return 0
    static int exchgmsg_final(ExchgMsg* hMsg);
};

class syncfile :public exchgmsg
{
public:
    static int server_main_prog(int argc, char* argv[]);
public:
    syncfile();
    ~syncfile();
    int init();
    int create(
        char* zFile,
        int   dwDesiredAccess,
        int   dwShareMode,
        void* lpSecurityAttributes,
        int   dwCreationDisposition,
        int   dwFlagsAndAttributes,
        void* hTemplateFile
        );
    int read(
        void* lpBuffer,
        int   nNumberOfBytesToRead,
        int*  lpNumberOfBytesRead,
        void* lpOverlapped
        );
    int write(
        void* lpBuffer,
        int   nNumberOfBytesToWrite,
        int*  lpNumberOfBytesWritten,
        void* lpOverlapped
        );
    int seek(
        long  lDistanceToMove,
        long* lpDistanceToMoveHigh,
        int   dwMoveMethod
        );
    int close();
    int final();
private:
    int mhClient;
    struct server_info {
        void* hProcess;
        void* hThread;
    } mhServer;

    int crt_server_process(server_info*);
    int del_server_process(server_info*);
    int get_ack_msg_code();

    struct {
        char* p;
        int size;
    } mTempBuff;
};

//-----write sync APIs-------
//if success func return handle (!=-1)
//else if error func return (-1)
SYNCWRITE_API void* syncfile_create(
    char* zFile,
    int   dwDesiredAccess,
    int   dwShareMode,
    void* lpSecurityAttributes,
    int   dwCreationDisposition,
    int   dwFlagsAndAttributes,
    void* hTemplateFile
    );
//if success func return 0
//else if error func return error code (!=0)
SYNCWRITE_API int syncfile_seek(
    void* hFile,
    long  lDistanceToMove,
    long* lpDistanceToMoveHigh,
    int   dwMoveMethod
    );
SYNCWRITE_API int syncfile_read(
    void* hFile,
    void* lpBuffer,
    int   nNumberOfBytesToRead,
    int*  lpNumberOfBytesRead,
    void* lpOverlapped
    );
SYNCWRITE_API int syncfile_write(
    void* hFile,
    void* lpBuffer,
    int   nNumberOfBytesToWrite,
    int*  lpNumberOfBytesWritten,
    void* lpOverlapped
    );
SYNCWRITE_API int syncfile_close(void* hFile);
#endif /*SYNC_WRITE_H*/
