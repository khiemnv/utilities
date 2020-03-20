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
int Hash(TCHAR* path, TCHAR* buff, int buffCount, HashMode hashMode)
{
	int err;
	HANDLE hFile;
	HANDLE hMap;
	LARGE_INTEGER offset;
	UINT32 pageSize;
	byte* mapAddr;

	hFile = CreateFile(
		path,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		0,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_NO_BUFFERING,
		0
	);
	assert(hFile != INVALID_HANDLE_VALUE);
	LARGE_INTEGER li;
	GetFileSizeEx(hFile, &li);
	hMap = CreateFileMapping(
		hFile,
		0,
		PAGE_READWRITE,
		li.HighPart,
		li.LowPart,
		0
	);

	switch (hashMode)
	{
	case Md5:
		//md5
		MD5Init(&ctx,0);
		break;
	case Sha512:
		//sha 512
		err = SHA512Reset(&sha512Ctx);
		break;
	default:
		break;
	}

	pageSize = GetLargePageMinimum();
	pageSize = max(pageSize, 256 << 20);
	offset.QuadPart = 0;
	for (; offset.QuadPart < li.QuadPart;) {
		int len = min(pageSize, li.QuadPart - offset.QuadPart);
		mapAddr = (byte*)MapViewOfFile(hMap,
			FILE_MAP_WRITE,
			offset.HighPart,
			offset.LowPart,
			len);
		assert(mapAddr != NULL);
		switch (hashMode)
		{
		case Md5:
			//md5
			MD5Update(&ctx, mapAddr, len);
			break;
		case Sha512:
			//sha 512
			err = SHA512Input(&sha512Ctx, (const uint8_t*)mapAddr, len);
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
			len+=_stprintf_s(buff + len, buffCount -len, TEXT("%02X"), ctx.digest[i]);
		}
		buff[len] = 0;
		break;
	case Sha512:
		//sha 512
		err = SHA512Result(&sha512Ctx, sha512res);
		for (i = 0; i < 64; i++) {
			len+=_stprintf_s(buff + len, buffCount - len, TEXT("%02X"), sha512res[i]);
		}
		buff[len] = 0;
		break;
	default:
		break;
	}

	return i;
}
int main() {
	TCHAR buff[MAX_PATH];
	const TCHAR* zFile = TEXT("D:\\tmp\\github\\utilities\\cleaner\\cleaner\\sha\\sha.h");
	Hash((TCHAR*)zFile, buff, _countof(buff), Sha512);
	printf("%s\n", buff);
	return 0;
}