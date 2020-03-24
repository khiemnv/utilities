#include <Windows.h>
#include <tchar.h>
#include <io.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdint.h>
#include <crtdbg.h>
#include "MD5.h"
#include "sha.h"

#define assert _ASSERT

typedef enum HashMode {
	Md5,
	Sha512
}HashMode;
MD5_CTX ctx;
SHA512Context sha512Ctx;
uint8_t sha512res[129];
int Hash(TCHAR* path, TCHAR* buff, int buffCount, HashMode hashMode);
int Hash(TCHAR* path, TCHAR* buff, int buffCount, HashMode hashMode)
{
	int rc;
	HANDLE hFile;
	HANDLE hMap;
	LARGE_INTEGER offset;
	UINT32 pageSize;
	byte* mapAddr;
	do {
		hFile = CreateFile(
			path,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			0,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_NO_BUFFERING,
			0
		);
		if (hFile == INVALID_HANDLE_VALUE) break;

		LARGE_INTEGER li;
		rc = GetFileSizeEx(hFile, &li);
		if (rc == FALSE) break;

		hMap = CreateFileMapping(
			hFile,
			0,
			PAGE_READWRITE,
			li.HighPart,
			li.LowPart,
			0
		);
		if (hMap == NULL) break;

		switch (hashMode)
		{
		case Md5:
			//md5
			MD5Init(&ctx, 0);
			break;
		case Sha512:
			//sha 512
			rc = SHA512Reset(&sha512Ctx);
			break;
		default:
			break;
		}

		pageSize = GetLargePageMinimum();
		pageSize = max(pageSize, 256 << 20);
		offset.QuadPart = 0;
		for (; offset.QuadPart < li.QuadPart;) {
			int len = min(pageSize, li.QuadPart - offset.QuadPart);
			mapAddr = (byte*)MapViewOfFile(
				hMap,
				FILE_MAP_WRITE,
				offset.HighPart,
				offset.LowPart,
				len);
			if(mapAddr == NULL) break;

			switch (hashMode)
			{
			case Md5:
				//md5
				MD5Update(&ctx, mapAddr, len);
				break;
			case Sha512:
				//sha 512
				rc = SHA512Input(&sha512Ctx, (const uint8_t*)mapAddr, len);
				break;
			default:
				break;
			}

			UnmapViewOfFile(mapAddr);

			offset.QuadPart += len;
		}

		CloseHandle(hMap);
		CloseHandle(hFile);

		int i = 0;
		int len = 0;
		switch (hashMode)
		{
		case Md5:
			//md5
			MD5Final(&ctx);
			for (i = 0; i < 16; i++) {
				len += _stprintf_s(buff + len, buffCount - len, TEXT("%02X"), ctx.digest[i]);
			}
			buff[len] = 0;
			break;
		case Sha512:
			//sha 512
			rc = SHA512Result(&sha512Ctx, sha512res);
			for (i = 0; i < 64; i++) {
				len += _stprintf_s(buff + len, buffCount - len, TEXT("%02X"), sha512res[i]);
			}
			buff[len] = 0;
			break;
		default:
			break;
		}

		//success
		return 0;
	} while (0);

	//error handle
	if (hFile != INVALID_HANDLE_VALUE) {
		CloseHandle(hFile);
	}

	return -2;
}
void testDll() {
	HMODULE g_shaMod;

	int (*pHash)(TCHAR * path, TCHAR * buff, int buffCount, HashMode hashMode);
	HMODULE hmod = LoadLibrary(TEXT("Hash.dll"));

	pHash = (int (*)(TCHAR*, TCHAR*,int,HashMode))GetProcAddress(hmod, "Hash");
	TCHAR buff[MAX_PATH];
	TCHAR* zFile = TEXT("D:\\tmp\\github\\utilities\\cleaner\\cleaner\\cleaner\\MD5.h");
	int rc = pHash(zFile, buff, MAX_PATH, Md5);
	assert(_tcscmp(buff, TEXT("982BCAF72D547E6548C2EB7BC6A6242C")) == 0);
	g_shaMod = hmod;

	FreeLibrary(g_shaMod);
}
int _tmain(int argc, TCHAR** argv) {
	//testDll();
	//return 0;
	//_setmode(_fileno(stdout), _O_U16TEXT);
	int rc = -1;
	do {
		if (argc < 2) break;

		HashMode mode = Md5;
		if (argc == 3) {
			mode = (_tcsicmp(argv[1], TEXT("sha512")) == 0) ? Sha512:
				(_tcsicmp(argv[1], TEXT("md5")) == 0) ? Md5: -1;
			if (mode == -1) break;
		}

		TCHAR buff[MAX_PATH];
		TCHAR* zFile = argv[1];
		rc = Hash(zFile, buff, _countof(buff), mode);
		if (rc) break;

		_tprintf(TEXT("%s\n"), buff);
		return 0;
	} while(0);

	_tprintf(TEXT("error %d\n"),rc);
	return rc;
}