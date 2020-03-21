// cleaner.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <Windows.h>
#include <tchar.h>
#include <io.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdint.h>
#include <crtdbg.h>


#include "MD5.h"
//#include "sha.h"

#define assert _ASSERT
#define use_hash 1
#define use_sha 0

/*
 *  These constants hold size information for each of the SHA
 *  hashing operations
 */
enum
{
	SHA1_Message_Block_Size = 64, SHA224_Message_Block_Size = 64,
	SHA256_Message_Block_Size = 64, SHA384_Message_Block_Size = 128,
	SHA512_Message_Block_Size = 128,
	USHA_Max_Message_Block_Size = SHA512_Message_Block_Size,

	SHA1HashSize = 20, SHA224HashSize = 28, SHA256HashSize = 32,
	SHA384HashSize = 48, SHA512HashSize = 64,
	USHAMaxHashSize = SHA512HashSize,

	SHA1HashSizeBits = 160, SHA224HashSizeBits = 224,
	SHA256HashSizeBits = 256, SHA384HashSizeBits = 384,
	SHA512HashSizeBits = 512, USHAMaxHashSizeBits = SHA512HashSizeBits
};
/*
 *  This structure will hold context information for the SHA-512
 *  hashing operation.
 */
typedef struct SHA512Context
{
#ifdef USE_32BIT_ONLY
	uint32_t Intermediate_Hash[SHA512HashSize / 4];	/* Message Digest  */
	uint32_t Length[4];		/* Message length in bits */
#else				/* !USE_32BIT_ONLY */
	uint64_t Intermediate_Hash[SHA512HashSize / 8];	/* Message Digest */
	uint64_t Length_Low, Length_High;	/* Message length in bits */
#endif				/* USE_32BIT_ONLY */
	int_least16_t Message_Block_Index;	/* Message_Block array index */
	/* 1024-bit message blocks */
	uint8_t Message_Block[SHA512_Message_Block_Size];

	int Computed;			/* Is the digest computed? */
	int Corrupted;		/* Is the digest corrupted? */
} SHA512Context;

//#define MY_EXTERN __declspec(dllimport)
//#define MY_EXTERN __declspec(dllexport)
#define MY_EXTERN  
/* SHA-512 */
MY_EXTERN int (*SHA512Reset)(SHA512Context*);
MY_EXTERN int (*SHA512Input)(SHA512Context*, const uint8_t* bytes,
	unsigned int bytecount);
MY_EXTERN int (*SHA512FinalBits)(SHA512Context*, const uint8_t bits,
	unsigned int bitcount);
MY_EXTERN int (*SHA512Result)(SHA512Context*,
	uint8_t Message_Digest[SHA512HashSize]);

SYSTEMTIME g_st;
TCHAR g_hashRes[129];
TCHAR g_temp[MAX_PATH];

class  MyOutBuff
{
public:
	MyOutBuff(TCHAR* path) {
		int err;
		hFile = CreateFile(
			path,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			0,
			CREATE_NEW,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_NO_BUFFERING,
			0
		);
		LARGE_INTEGER li;
		li.QuadPart = (INT64)1024 * 1024 * 1024;
		hMap = CreateFileMapping(
			hFile,
			0,
			PAGE_READWRITE,
			li.HighPart,
			li.LowPart,
			0
		);
		pageSize = GetLargePageMinimum();
		offset = 0;
		cursor = 0;
		mapAddr = (byte*) MapViewOfFile(hMap,
			FILE_MAP_WRITE,
			0,
			0,
			pageSize);
	}
	~MyOutBuff() {
		UnmapViewOfFile(mapAddr);
		CloseHandle(hMap);
		LARGE_INTEGER li;
		li.QuadPart = offset + cursor;
		SetFilePointerEx(hFile, li, NULL, FILE_BEGIN);
		int err = SetEndOfFile(hFile);
		CloseHandle(hFile);
	}
	void add(byte* buff, UINT32 len) {
		UINT32 copied = 0;
		for (; copied<len;) {
			UINT32 remain = pageSize - cursor;
			if (remain < (len - copied)) {
				memcpy_s(mapAddr + cursor, remain, buff+copied, remain);

				LARGE_INTEGER li;
				li.QuadPart = offset + pageSize;
				UnmapViewOfFile(mapAddr);
				mapAddr = (byte*)MapViewOfFile(hMap,
					FILE_MAP_WRITE,
					li.HighPart,
					li.LowPart,
					pageSize);
				offset = li.QuadPart;
				cursor = 0;

				copied += remain;
			}
			else {
				memcpy_s(mapAddr + cursor, remain, buff+copied, len-copied);
				cursor += len - copied;
				break;
			}
		}
	}
private:
	HANDLE hFile;
	HANDLE hMap;
	UINT64 offset;
	UINT32 pageSize;
	UINT32 cursor;
	byte* mapAddr;
};

FILE* fout;
MyOutBuff* pOutBuff;

#define mode 1


void printOut(TCHAR* temp, int len) {
	//fwrite(temp, sizeof(TCHAR) * len, 1, fout);
	//pOutBuff->add((byte*)temp, sizeof(TCHAR) * len);
#if mode == 1
	_ftprintf(fout,TEXT("%s"), temp);
#elif mode == 3
	fwrite(temp, sizeof(TCHAR) * len,1, fout);
#elif mode == 4
	_tprintf(TEXT("%s"), temp);
#endif
}

void printOut(TCHAR type, UINT64 size, DWORD y, DWORD m, DWORD d, TCHAR* md5, TCHAR* dir, TCHAR* name) {
	_ftprintf(fout, TEXT("%c %I64d %04d-%02d-%02d "),type, size, y, m, d);
	_ftprintf(fout, TEXT("%s "), md5);
	_ftprintf(fout, TEXT("%s\\%s\n"), dir, name);
}
void printOut(TCHAR type, UINT64 size, DWORD y, DWORD m, DWORD d, TCHAR* dir, TCHAR* name) {
	_ftprintf(fout, TEXT("%c %I64d %04d-%02d-%02d "), type, size, y, m, d);
	_ftprintf(fout, TEXT("%016d "), 0);
	_ftprintf(fout, TEXT("%s\\%s\n"), dir, name);
}
void printOut(TCHAR type, UINT64 size, DWORD y, DWORD m, DWORD d, TCHAR* path) {
	_ftprintf(fout, TEXT("%c %I64d %04d-%02d-%02d "), type, size, y, m, d);
	_ftprintf(fout, TEXT("%016d "), 0);
	_ftprintf(fout, TEXT("%s\n"), path);
}
void printOut(TCHAR type, UINT64 size, TCHAR* path) {
	_ftprintf(fout, TEXT("%c %I64d "), type, size);
	_ftprintf(fout, TEXT("%s\n"), path);
}
void printOut(TCHAR type, UINT64 size, TCHAR* dir, TCHAR* name) {
	_ftprintf(fout, TEXT("%c %I64d "), type, size);
	_ftprintf(fout, TEXT("%s\\%s\n"), dir, name);
}

class MyHash
{
public:
	MyHash() {}
	~MyHash() {}
	enum HashMode {
		Md5,
		Sha512
	};
	int Hash(TCHAR* path, TCHAR* buff, int buffCount, HashMode hashMode)
	{
		int err =0;
		HANDLE hFile;
		HANDLE hMap;
		LARGE_INTEGER offset;
		UINT32 pageSize;
		byte* mapAddr = 0;

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
		case HashMode::Md5:
			//md5
			MD5Init(&ctx);
			break;
		case HashMode::Sha512:
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
			case HashMode::Md5:
				//md5
				MD5Update(&ctx, mapAddr, len);
				break;
			case HashMode::Sha512:
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
		case HashMode::Md5:
			//md5
			MD5Final(&ctx);
			for (i=0; i < 16; i++) {
				len += _stprintf_s(buff + len, buffCount - len, TEXT("%02X"), ctx.digest[i]);
			}
			break;
		case HashMode::Sha512:
			//sha 512
			err = SHA512Result(&sha512Ctx, sha512res);
			for (i = 0; i < 64; i++) {
				len +=_stprintf_s(buff + len, buffCount -len, TEXT("%02X"), sha512res[i]);
			}
			break;
		default:
			break;
		}

		return err;
	}
private:
	uint8_t sha512res[64];
	SHA512Context sha512Ctx;
	MD5_CTX ctx;
};

MyHash g_hash;
int GetMd5(TCHAR* path, TCHAR* buff, int buffCount) 
{
	return g_hash.Hash(path, buff, buffCount, MyHash::HashMode::Md5);
}
int GetSha512(TCHAR* path, TCHAR* buff, int buffCount) {
	return g_hash.Hash(path, buff, buffCount, MyHash::HashMode::Sha512);
}

UINT64 ListFileR(TCHAR* path, int lvl) {

	TCHAR buff[MAX_PATH];
	WIN32_FIND_DATA fd;
	HANDLE hFind;
	UINT64 totalSize;
	int rc;

	//list all file
	totalSize = 0;
	_stprintf_s(buff, TEXT("%s\\*.*"), path);
	hFind = FindFirstFile(buff, &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		for (;;)
		{
			if (_tcscmp(fd.cFileName, TEXT(".")) == 0
				|| _tcscmp(fd.cFileName, TEXT("..")) == 0
				|| fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)
			{
				//do nothing
			}
			else if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				//is directory, recursive find
				_stprintf_s(buff, TEXT("%s\\%s"), path, fd.cFileName);
				UINT64 subSize = ListFileR(buff, lvl + 1);

				FileTimeToSystemTime(&fd.ftLastWriteTime, &g_st);
				printOut(TEXT('D'), subSize, g_st.wYear, g_st.wMonth, g_st.wDay,
					path, fd.cFileName);

				totalSize += subSize;
			}
			else
			{
				//is file,
				LARGE_INTEGER li;
				li.HighPart = fd.nFileSizeHigh;
				li.LowPart = fd.nFileSizeLow;
				FileTimeToSystemTime(&fd.ftLastWriteTime, &g_st);
				_stprintf_s(g_temp, TEXT("%s\\%s"), path, fd.cFileName);
#if use_hash
				GetMd5(g_temp, g_hashRes, _countof(g_hashRes));
#else
				g_hashRes[0] = '0';
				g_hashRes[1] = 0;
#endif

				printOut(TEXT('F'), li.QuadPart, g_st.wYear, g_st.wMonth, g_st.wDay, 
					g_hashRes, path, fd.cFileName);

				totalSize += li.QuadPart;
			}

			//find next
			rc = FindNextFile(hFind, &fd);
			if (rc) {
				//continue;
			}
			else
			{
				FindClose(hFind);
				break;
			}
		}
	}
	return totalSize;
}

HMODULE g_shaMod;
void LoadShaLib() {
	HMODULE hmod = LoadLibrary(TEXT("sha.dll"));
	SHA512Reset = (int (*)(SHA512Context*))GetProcAddress(hmod, "SHA512Reset");
	SHA512Input = (int (*)(SHA512Context*, const uint8_t * , unsigned int ))GetProcAddress(hmod, "SHA512Input");
	SHA512FinalBits = (int (*)(SHA512Context*, const uint8_t , unsigned int ))GetProcAddress(hmod, "SHA512FinalBits");
	SHA512Result = (int (*)(SHA512Context*, uint8_t Message_Digest[SHA512HashSize]))GetProcAddress(hmod, "SHA512Result");
	g_shaMod = hmod;
}

void UnloadShaLib() {
	FreeLibrary(g_shaMod);
}

void test_MD5hash() {
	const TCHAR* zFile = TEXT("MD5.h");
	TCHAR buff[MAX_PATH];
	GetMd5((TCHAR*)zFile, buff, _countof(buff));
	assert(_tcscmp(buff, TEXT("982BCAF72D547E6548C2EB7BC6A6242C"))==0);
	GetSha512((TCHAR*)zFile, buff, _countof(buff));
	assert(_tcscmp(buff, TEXT("5ED0B965F43A9B72C7390D1C9E85D6DC780599612041C2EBF0E3C9600DDAD88F0525079194D9AFBAC43CC7662F36DCDFED9079311D347105E49121147517F1C2")) == 0);
}

int _tmain(int argc, TCHAR** argv)
{

#if use_sha
	LoadShaLib();
#endif

	//test_MD5hash();
	//return 0;

#if mode == 4
	if (argc < 2) {
		return -1;
	}
#else
	if (argc < 3) {
		return -1;
	}
#endif
	
	DWORD start = GetTickCount();

#if mode == 2
	pOutBuff = new MyOutBuff(argv[2]);
#elif mode ==1 
	int err = _tfopen_s(&fout, argv[2], TEXT("w+,ccs=UTF-8"));
	if (err) {
		_tprintf(TEXT("open error %d %s\n"), err, argv[2]);
		return -2;
	}
#elif mode == 3
	int err = _tfopen_s(&fout, argv[2], TEXT("wt+, ccs=UNICODE"));
	if (err) {
		_tprintf(TEXT("open error %d %s\n"), err, argv[2]);
		return -2;
	}
#endif

	UINT64 totalSize = ListFileR(argv[1], 0);
	TCHAR temp[MAX_PATH];
	int len = _stprintf_s(temp,TEXT("D %I64d %s\n"), totalSize, argv[1]);
	printOut(TEXT('D'),totalSize, 0,0,0, argv[1]);

	DWORD end = GetTickCount();

	_tprintf(TEXT("%d\n"), end - start);

#if mode == 2
	delete pOutBuff;
#elif mode == 1 || mode == 3
	fclose(fout);
#endif

#if use_sha
	UnloadShaLib();
#endif

	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
