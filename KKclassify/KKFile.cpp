#include "StdAfx.h"
#include "KKFile.h"

typedef char* (__stdcall *MYPROC)(char*); 
typedef char* (__stdcall *SCAN_EXTERNAL)(char*, char*); 

void CKKFile::do_work(char *fName)
{
	strcpy(fileName,fName);
	//Read PE info
	ReadPEinfo();
	// Get file MD5
	GetMD5();
	// Detect Pack
	DetectPack();
	// Check PEInject
	PEInjected = isPEinject();
	// Check Overlay
	Overlay = DetectOverlayedFile();
}

CKKFile::CKKFile(void)
{
}

CKKFile::CKKFile(char *fName)
{
	do_work(fName);
}

CKKFile::~CKKFile(void)
{
}

// Lay thong tin PE info
int CKKFile::ReadPEinfo()
{
	HANDLE hFile, hFileMapping;
	DWORD lpMappedFile;
	IMAGE_SECTION_HEADER tmpsec;
	unsigned long i;
	DWORD tmp;
	
	ZeroMemory(&PEinfo,sizeof(PE_INFO));

	hFile = CreateFileA(fileName,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	if (hFile == INVALID_HANDLE_VALUE) return 0;
	// Get FileSize
	filesize = GetFileSize(hFile,NULL);
	// Map file
	hFileMapping = CreateFileMappingA(hFile,0,PAGE_READONLY,0,0,0);
	if (hFileMapping == NULL) 
	{
		CloseHandle(hFile);
		return 0;
	}

	lpMappedFile = (DWORD) MapViewOfFile(hFileMapping,FILE_MAP_READ,0,0,0);
	if (lpMappedFile == NULL)
	{
		CloseHandle(hFileMapping);
		CloseHandle(hFile);
		return 0;
	}

	// Get DOS_HEADER
	PEinfo.dosHeader = *(IMAGE_DOS_HEADER *) lpMappedFile;
	if (PEinfo.dosHeader.e_magic != IMAGE_DOS_SIGNATURE)
	{
		PEinfo.isPE = 0;
		CloseHandle(hFileMapping);
		CloseHandle(hFile);
		return 1;
	}

	// GetPE Header Point
	if (*(WORD *) (lpMappedFile + PEinfo.dosHeader.e_lfanew) == IMAGE_NT_SIGNATURE)
		PEinfo.peHeaderPoint = PEinfo.dosHeader.e_lfanew;
	else
	{
		for (i=0;i<filesize;i++)
			if (*(WORD *) (lpMappedFile + i) == IMAGE_NT_SIGNATURE)
			{
				PEinfo.peHeaderPoint = i;
				break;
			}
	
		if (i == filesize)
		{
			PEinfo.isPE = 0;
			CloseHandle(hFileMapping);
			CloseHandle(hFile);
			return 1;
		}
	}

	PEinfo.isPE = 1;

	// Get NT_HEADER
	PEinfo.ntHeader = *(IMAGE_NT_HEADERS *) (lpMappedFile + PEinfo.peHeaderPoint);
	PEinfo.numberofSection = PEinfo.ntHeader.FileHeader.NumberOfSections;
	PEinfo.optionalHeader = PEinfo.ntHeader.OptionalHeader;
	
	// Get 3 special Section
		// FirstSection
	i = PEinfo.peHeaderPoint + 0x18 + PEinfo.ntHeader.FileHeader.SizeOfOptionalHeader;
	PEinfo.sectionStartRaw = i;

	PEinfo.firstsec = *(IMAGE_SECTION_HEADER *) (lpMappedFile + i);
		// LastSection
	i += (PEinfo.numberofSection - 1)* 0x28;
	PEinfo.lastsec = * (IMAGE_SECTION_HEADER *) (lpMappedFile + i);
		// ExecSection
	PEinfo.execsec = PEinfo.firstsec;
	i = PEinfo.peHeaderPoint + 0x18 + PEinfo.ntHeader.FileHeader.SizeOfOptionalHeader;
	while (PEinfo.optionalHeader.AddressOfEntryPoint >= PEinfo.execsec.VirtualAddress + PEinfo.execsec.Misc.VirtualSize)
	{
		i += 0x28;
		PEinfo.execsec = *(IMAGE_SECTION_HEADER *) (lpMappedFile + i);
	}

	// Get Sizeof Code
	PEinfo.realCodeSize = 0;
	i = 0;
	while (i<PEinfo.numberofSection)
	{
		tmpsec = * (IMAGE_SECTION_HEADER *) (lpMappedFile + PEinfo.sectionStartRaw + i*0x28);
		//tmp = tmpsec.Characteristics;
		if ((tmpsec.Characteristics / 0x20000000) % 2 == 1)
			PEinfo.realCodeSize += tmpsec.SizeOfRawData;
		i++;
	}

	// GetMAXSTARTCODE
	PEinfo.startCodebyteRead = (PEinfo.execsec.SizeOfRawData + PEinfo.execsec.VirtualAddress - PEinfo.optionalHeader.AddressOfEntryPoint < MAXSTARTCODE) ? 
		PEinfo.execsec.SizeOfRawData + PEinfo.execsec.VirtualAddress - PEinfo.optionalHeader.AddressOfEntryPoint : MAXSTARTCODE;
	memcpy(&PEinfo.StartCode,(void *) (lpMappedFile + PEinfo.execsec.PointerToRawData + PEinfo.optionalHeader.AddressOfEntryPoint - PEinfo.execsec.VirtualAddress),PEinfo.startCodebyteRead);
	//
	CloseHandle(hFileMapping);
	CloseHandle(hFile);
	return 1;
}

void CKKFile::GetMD5()
{
	MD5_CTX fileMD5;
	char tmpbuf[5];
	int i;

	md5file(fileName,0,&fileMD5);

	strcpy(MD5hexview,"");

	for (i=0;i<16;i++)
	{
		sprintf_s(tmpbuf,"%.2X",fileMD5.digest[i]);
		strcat(MD5hexview,tmpbuf);
	}
};

int CKKFile::DetectPack()
{
	CDetectPack pack;

	packed = ScanWithPEiD(fileName,Packinfo);

	if (!packed)
	{
		packed = pack.DectectPack(fileName);
		if (packed) strcpy(Packinfo,"HoanNQ Detecting sign!");
	}

	if (strstr(Packinfo,"Microsoft Visual") != NULL)
		packed = 0;
	if (strstr(Packinfo,"Nullsoft") != NULL)
		packed = 0;
	if (strstr(Packinfo,"Borland") != NULL)
		packed = 0;
	if (strstr(Packinfo,"Delphi") != NULL)
		packed = 0;
	if (strstr(Packinfo,"Pascal") != NULL)
		packed = 0;

	return packed;
};

int CKKFile::isPEinject()
{
	int i;
	int Delta=0;
	BYTE tmp1;
	DWORD tmp2;

	if (packed) return 0;

	strcpy(PEInjectNote,"");
	
	// Check section cuoi
	if ((PEinfo.lastsec.VirtualAddress == PEinfo.execsec.VirtualAddress) && (PEinfo.lastsec.Characteristics >= 0xE0000000))
		strcat(PEInjectNote,"+ Section Cuoi la section chua RVAEntryPoint\r\n");
	if (PEinfo.optionalHeader.AddressOfEntryPoint < PEinfo.firstsec.VirtualAddress)
		strcat(PEInjectNote,"+ EntryPoint truoc section dau tien\r\n");
	
	for (i=0;i<PEinfo.startCodebyteRead - 5;i++)
	{
		tmp1 = *(BYTE *) (PEinfo.StartCode + i);
		tmp2 = *(DWORD *) (PEinfo.StartCode + i + 1);
		if (tmp1 == 0xE8 && tmp2 == 0) 
		{
			Delta = 1;
			break;
		}
	}

	if (Delta)
		strcat(PEInjectNote,"+ Thay ki thuat Delta!\r\n");
	// 
	return 1;
}

int ScanWithPEiD(char *pFileName, char *pOutBuffer)
{
	char *scanresult; 
	// file to scan
	char  scanfile[MAX_PATH];
	
	strcpy(scanfile,pFileName);
	// this will hold the handle to the dll 
	HINSTANCE hinstLib; 
	// this will hold a pointer to the function in the dll 
	MYPROC ProcAdd; 
	SCAN_EXTERNAL ProcScanExternal;

	BOOL fFreeResult, fRunTimeLinkSuccess = FALSE; 

	// LoadLibrary() loads a specific DLL file 
	// if it is successful it will return a handle to the module 
	// if it failes it will return NULL 
	hinstLib = LoadLibrary(TEXT("res\\PEiDll.DLL")); 

	// check to make sure LoadLibrary() didn't return NULL 
	if(hinstLib != NULL){ 

		ProcAdd = (MYPROC)GetProcAddress(hinstLib, "ScanWithPEiD"); 
		ProcScanExternal = (SCAN_EXTERNAL)GetProcAddress(hinstLib, "Scan_External"); 
		// If the function address is valid, call the function. 
		if((ProcAdd != NULL)&&
			(ProcScanExternal != NULL ))
		{ 
			scanresult = (ProcAdd)(scanfile); 
			// we will output the result of the scan 

			if ((strstr(scanresult, "Not a valid PE file") != NULL) ||
				(strstr(scanresult, "Nothing found") != NULL) ||
				(strstr(scanresult, "File does NOT exist") != NULL))
			{
				strcpy(pOutBuffer,scanresult);
				fFreeResult = FreeLibrary(hinstLib); 
				return 0;
			}

			if (strlen(scanresult) >= 512) scanresult[511]='\0';
			strcpy(pOutBuffer,scanresult);
		} 

		fFreeResult = FreeLibrary(hinstLib); 
	} 
	return 1;
}

int GetSection(IMAGE_SECTION_HEADER * section)
{
	return 1;
}

//////////////////////////////////////////////////////////////////////////
// HaoPX
//////////////////////////////////////////////////////////////////////////
#define		ERROR_DETECT				-1
#define		NOT_OVERLAY			 0
#define		OVERLAY_SINGLE_MZ	 1
#define		OVERLAY_DOUBLE_MZ	 2

int CKKFile::DetectOverlayedFile()
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

	if(!PEinfo.isPE) return NOT_OVERLAY;

	hFile = CreateFileA(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) return ERROR_DETECT;

	dwFileSize = GetFileSize(hFile, NULL);
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
	if (dwSign != IMAGE_DOS_SIGNATURE)
	{
		// Not MZ
		CloseHandle(hFile);
		return ERROR_DETECT;
	}

	dwPEOffset = *((DWORD*)(buffer + PEinfo.dosHeader.e_lfanew));
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
	if (dwSign != IMAGE_NT_SIGNATURE)
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

char * CKKFile::MD5_Section(int SectionNumber)
{
	char MD5_hex[33];
	char tmpbuf[5];
	FILE *f;
	MD5_CTX MD5tmp;
	char buf[BLOCK_SIZE];
	long trb = 0;
	IMAGE_SECTION_HEADER tmpsec;

	if (PEinfo.numberofSection == 0) ReadPEinfo();

	if (PEinfo.numberofSection < SectionNumber) return NULL; // Section Number does not Exist
	f = fopen(fileName,"rb");
	if ( f == NULL) return NULL;
	
	// Init
	MD5Init(&MD5tmp,0);

	// Calculate Section
	fseek(f,PEinfo.sectionStartRaw + (SectionNumber - 1) * 0x28,0);
	int rb = fread((BYTE *) &tmpsec,1,0x28,f);
	if (rb != 0x28) 
	{
		fclose(f);
		return NULL; // Read Section Header error!
	}

	trb = tmpsec.SizeOfRawData;
	fseek(f,tmpsec.PointerToRawData,0);

	for (;;)
	{
		rb = fread(buf, 1, BLOCK_SIZE, f);
		//trb += rb; 
		if (trb < BLOCK_SIZE)
			MD5Update(&MD5tmp, (unsigned char *) buf, trb);
		else
			MD5Update(&MD5tmp, (unsigned char *) buf, rb);
		
		trb -= rb;
		if (rb < BLOCK_SIZE || trb <0 ) break;
	}
	fclose (f);
	MD5Final(&MD5tmp);

	// MD5 Hexview
	strcpy(MD5_hex,"");

	for (int i=0;i<16;i++)
	{
		sprintf_s(tmpbuf,"%.2X",MD5tmp.digest[i]);
		strcat(MD5_hex,tmpbuf);
	}

	return MD5_hex;
}

EXTRA_IMAGE_SECTION_HEADER * CKKFile::PeekSection(int SectionNumber)
{
	EXTRA_IMAGE_SECTION_HEADER Sectmp;
	FILE *f;

	if (SectionNumber > PEinfo.numberofSection) return NULL;
	ZeroMemory(&Sectmp,sizeof(EXTRA_IMAGE_SECTION_HEADER));

	f = fopen(fileName,"rb");
	if ( f == NULL) return NULL;

	// Calculate Section
	if (PEinfo.numberofSection == 0) ReadPEinfo();
	if (PEinfo.numberofSection == 0) return NULL;

	fseek(f,PEinfo.sectionStartRaw + (SectionNumber - 1) * 0x28,0);
	int rb = fread((BYTE *) &Sectmp.secheader,1,0x28,f);
	fclose(f);

	if (rb != 0x28) return NULL;
	strcpy(Sectmp.MD5,MD5_Section(SectionNumber));
	Sectmp.SectionName[8] = '\0';
	for (int i=0; i<8;i++)
	{
		Sectmp.SectionName[i] = Sectmp.secheader.Name[i];
	}

	return &Sectmp;
}