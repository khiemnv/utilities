#include "stdafx.h"
#include <windows.h>

#define		MAX_SECTIONS		64
#define		ERROR_DETECT				-1
#define		NOT_OVERLAY			 0
#define		OVERLAY_SINGLE_MZ	 1
#define		OVERLAY_DOUBLE_MZ	 2
int main()
{
	return 0;
}
BOOL IsPEFile(LPCTSTR szFilePath)
{
	HANDLE	hFile;
	DWORD	dwReaded;
	DWORD	dwFileSize;
	DWORD	dwPEOffset;
	DWORD	dwSign;
	const	DWORD dwBufferSize = 100;
	BYTE	buffer[100];
	
	hFile = CreateFile(szFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) return FALSE;
	
	dwFileSize = GetFileSize(hFile, NULL);
	if (dwFileSize == 0)
	{
		CloseHandle(hFile);
		return FALSE;
	}
	
	ReadFile(hFile, buffer, dwBufferSize, &dwReaded, NULL);
	if (dwReaded != dwBufferSize && dwReaded != dwFileSize)
	{	
		// Cannot read file, return
		CloseHandle(hFile);
		return FALSE;
	}
	
	dwSign = *((WORD*)buffer);
	//if (dwSign != 0x5A4D && dwSign != 0x7A6D)
	if (dwSign != 0x5A4D)
	{
		// Not MZ
		CloseHandle(hFile);
		return FALSE;
	}
	
	dwPEOffset = *((DWORD*)(buffer + 0x3C));
	if (dwPEOffset  > (dwFileSize))
	{
		// Offset out of file, return
		CloseHandle(hFile);
		return FALSE;
	}
	
	// Move file pointer to the beginning of the PE header
	if (SetFilePointer(hFile, dwPEOffset, NULL, FILE_BEGIN) == -1)
	{
		CloseHandle(hFile);
		return FALSE;
	}
	
	ReadFile(hFile, buffer, 4, &dwReaded, NULL);
	if (dwReaded != 4)
	{
		CloseHandle(hFile);
		return FALSE;
	}
	
	dwSign = *((DWORD*)buffer);
	//if (dwSign != 0x4550 && dwSign != 0x6570)
	if (dwSign != 0x4550)
	{
		// Not PE
		CloseHandle(hFile);
		return FALSE;
	}
	
	CloseHandle(hFile);
	return TRUE;
}

int DetectOverlayedFile(LPCTSTR szFilePath)
{
	HANDLE	hFile;
	DWORD	dwReaded;
	DWORD	dwFileSize;
	DWORD	dwPEOffset;
	DWORD	dwSign;
	const	DWORD dwBufferSize = 4096;
	BYTE	buffer[4096];
	BYTE*	pSectionsTable;
	WORD	wNumberOfSections;
	DWORD	dwMaxRawSize, dwRawSize;
	IMAGE_SECTION_HEADER*	pSectionHeader;
	int		i;
	
	if(!IsPEFile(szFilePath)) return NOT_OVERLAY;

	hFile = CreateFile(szFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) return ERROR_DETECT;
	
	dwFileSize = ::GetFileSize(hFile, NULL);
	if (dwFileSize == 0)
	{
		CloseHandle(hFile);
		return ERROR_DETECT;
	}
	
	ReadFile(hFile, buffer, dwBufferSize, &dwReaded, NULL);
	if (dwReaded != dwBufferSize && dwReaded != dwFileSize)
	{	
		// Cannot read file, return
		CloseHandle(hFile);
		return ERROR_DETECT;
	}
	
	dwSign = *((WORD*)buffer);
	if (dwSign != 0x5A4D)
	{
		// Not MZ
		CloseHandle(hFile);
		return ERROR_DETECT;
	}
	
	dwPEOffset = *((DWORD*)(buffer + 0x3C));
	if (dwPEOffset  > (dwFileSize))
	{
		// Offset out of file, return
		CloseHandle(hFile);
		return ERROR_DETECT;
	}
	
	// Move file pointer to the beginning of the PE header
	if (SetFilePointer(hFile, dwPEOffset, NULL, FILE_BEGIN) == -1)
	{
		CloseHandle(hFile);
		return ERROR_DETECT;
	}
	
	// Read PE header to buffer
	ReadFile(hFile, buffer, dwBufferSize, &dwReaded, NULL);
	if (dwReaded != dwBufferSize)
	{
		CloseHandle(hFile);
		return ERROR_DETECT;
	}
	
	dwSign = *((DWORD*)buffer);
	if (dwSign != 0x4550)
	{
		// Not PE
		CloseHandle(hFile);
		return ERROR_DETECT;
	}	

	pSectionsTable = buffer + (*(WORD*)(buffer + 0x14)) + 0x18;
	wNumberOfSections = *(WORD*)(buffer + 6);
	if (wNumberOfSections == 0 || wNumberOfSections >= MAX_SECTIONS) return FALSE;
	
	dwMaxRawSize = 0;
	for (i = 0; i < wNumberOfSections; i++)
	{
		pSectionHeader = (IMAGE_SECTION_HEADER*)(pSectionsTable + 0x28 * i);
		dwRawSize = pSectionHeader->PointerToRawData + pSectionHeader->SizeOfRawData;
		if (dwMaxRawSize < dwRawSize) dwMaxRawSize = dwRawSize;
	}
	if(dwMaxRawSize == dwFileSize)
	{ 
		CloseHandle(hFile);
		return NOT_OVERLAY;		
	}
	if(SetFilePointer(hFile,dwMaxRawSize,NULL,FILE_BEGIN) == -1)
	{
		CloseHandle(hFile);
		return ERROR_DETECT;
	}
	// Doc va kiem tra MZ, PE
	ReadFile(hFile,buffer,dwBufferSize,&dwReaded,NULL);
	if(dwReaded <dwBufferSize) return NOT_OVERLAY;
	for(i=0; i<dwBufferSize-sizeof(IMAGE_DOS_HEADER);i++)
	{
		if(buffer[i]=='M' && buffer[i+1]=='Z' && buffer[i+sizeof(IMAGE_DOS_HEADER)]=='P' && buffer[i+1+sizeof(IMAGE_DOS_HEADER)])
		{
			CloseHandle(hFile);
			return OVERLAY_DOUBLE_MZ;
		}
	}
	CloseHandle(hFile);
	return OVERLAY_SINGLE_MZ;
	//	return (dwMaxRawSize < dwFileSize);
}