#include "connect.h"
#include "syncwrite.h"

#include <Windows.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv)
{
    int rc;

    int isMainProcess = 0;
    if (argc == 1) {
        isMainProcess = 1;
    }
    if (isMainProcess) {
        HANDLE hf;
        int count;

        char* zFile = "e:\\test.txt";
        hf = syncfile_create(
            zFile,
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL
            );
        assert(hf != INVALID_HANDLE_VALUE);

        rc = syncfile_seek(hf, 0, 0, FILE_BEGIN);
        assert(rc == 0);

        rc = syncfile_write(hf, "abc", 4, &count, 0);
        assert(count == 4);

        rc = syncfile_seek(hf, 0, 0, FILE_BEGIN);
        assert(rc == 0);

        char buff[16];
        rc = syncfile_read(hf, buff, 4, &count, 0);
        assert(count == 4);
        assert(strcmp(buff, "abc") == 0);

        rc = syncfile_close(hf);
    }
    else {
        rc = syncfile::server_main_prog(argc, argv);
    }

    return rc;
}
int sent_file(int argc, char** argv)
{
	enum {zParamsLen = 64, port = 27015, zFileLen = 256,};

	cnn_params params;
	char zFile[zFileLen];
	char zParams[zParamsLen];
/*
	enum {buffLen = 1024};
	char buff[buffLen];
	int count;
	char outbuff[16];

	void* rmHandle = rm_init(buff, buffLen);
	void* rmHandle2 = rm_assign(buff, buffLen);
	rm_write(rmHandle, "123", 4, &count);
	rm_read(rmHandle,outbuff, 16, &count);
*/
	enum {
		iCnnType = 1,
		iMode    = 2,
		iParam   = 3,
		iFile    = 4,
		iBlockSize = 5,
		iPageCount = 6,

		nArg     = 5,
	};

	if (argc >= nArg) {
		//setup params
		if (_stricmp(argv[iCnnType], "socket") == 0) {
			params.cnnType = cnn_type_socket;
			strcpy_s(zParams, zParamsLen, argv[iParam]);
			params.zIpAddr = zParams;
			params.port  = port;
		} else if (_stricmp(argv[iCnnType], "pipe") == 0) {
			params.cnnType = cnn_type_pipe;
			strcpy_s(zParams, zParamsLen, argv[iParam]);
			params.zPipe = zParams;
		} else if (_stricmp(argv[iCnnType], "sharefile") == 0) {
			params.cnnType = cnn_type_sharefile;
			strcpy_s(zParams, zParamsLen, argv[iParam]);
			params.zShareFile = zParams;
		} else {
			assert(0);
		}

		strcpy_s(zFile, zFileLen, argv[iFile]);
		params.zFile = zFile;

		//block size
		enum {minBlockSize = 1024, maxBlockSize = 1024*1024,};
		int blockSize = minBlockSize;
		if (argc >= (nArg + 1)) {
			blockSize = atoi(argv[iBlockSize]);
			if (blockSize < minBlockSize) {
				blockSize = minBlockSize;
			} else if (blockSize > maxBlockSize) {
				blockSize = maxBlockSize;
			}
		}
		params.blockSize = blockSize;

		enum {minPageCount = 1024, maxPageCount = 128*1024};
		int pageCount = minPageCount;
		if (argc >= (nArg + 2)) {
			pageCount = atoi(argv[iPageCount]);
			if (pageCount < minPageCount) {
				pageCount = minPageCount;
			} else if (pageCount > maxPageCount) {
				pageCount = maxPageCount;
			}
		}
		params.nPage = pageCount;

		//start
		if (strcmp(argv[iMode], "server") == 0) {
			start_server(&params);
		} else if (strcmp(argv[iMode], "client") == 0) {
			start_client(&params);
		}
	} else {
		printf("<socket/pipe/sharefile> <server/client> <ipAddr/namedPipe/sharefilename> <zFout/zFin> [blockSize] [pageCount]\n");
	}

	return 0;
}

