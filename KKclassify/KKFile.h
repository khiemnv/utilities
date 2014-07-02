#include "CDetectPack.h"
#include "MD5.H"

#ifndef ___KKFILE_H___
#define ___KKFILE_H___

#pragma once

int ScanWithPEiD(char *pFile, char *pOutBuffer);
int GetSection(IMAGE_SECTION_HEADER *);
//////////////////////////////////////////////////////////////////////////
// Extra Section Info
//////////////////////////////////////////////////////////////////////////
typedef struct  
{
	IMAGE_SECTION_HEADER secheader;
	char SectionName[9];
	char MD5[33];
} EXTRA_IMAGE_SECTION_HEADER;

typedef struct
{
	char file_version[100];
	char iternal_name[200];
	char languages[50];
	char original_file_name[100];
	char	product_name[200];
	char	product_version[50];
}KKVERSION;

//////////////////////////////////////////////////////////////////////////
// File info
//////////////////////////////////////////////////////////////////////////
typedef struct 
{
	IMAGE_DOS_HEADER dosHeader;
	IMAGE_NT_HEADERS ntHeader;
	IMAGE_OPTIONAL_HEADER optionalHeader;
	IMAGE_SECTION_HEADER lastsec,firstsec,execsec;
	DWORD peHeaderPoint;
	BYTE StartCode[MAXSTARTCODE];

	int	isPE;	// if file is PE file = 1
				// if file is not PE file = 0
	int numberofSection;
	DWORD sectionStartRaw;
	unsigned long realCodeSize;
	int startCodebyteRead;
} PE_INFO;

class CKKFile
{
private:
	int ReadPEinfo();
	void GetMD5();
	int DetectPack();
	int isPEinject();
	int DetectOverlayedFile();

public:
	CKKFile(void);
	CKKFile(char *fName);
	~CKKFile(void);

	// File path & File Name info
	PE_INFO PEinfo;
	char fileName[MAX_PATH];
	char filePath[MAX_PATH];
	char fileExt[10];

	// lay thong tin cua file vd: version ... 
	void do_work(char *fName);
	int PEInjected;
	char PEInjectNote[1024];
	char	MD5hexview[40];
	int	packed;
	char	Packinfo[512];
	
	unsigned long filesize;
	int Overlay;
	char *MD5_Section(int SectionNumber);
	EXTRA_IMAGE_SECTION_HEADER * PeekSection(int SectionNumber);
};
#endif