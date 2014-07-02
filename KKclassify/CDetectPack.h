// Writer: Ngo Quoc Hoan
#include <Windows.h>

#ifndef ___CDETECTPACK_H___
#define ___CDETECTPACK_H___


#define     RationSample        0xC000  // mat do lay mau
#define		SizeSample			0x200 // kich thuc cua mau ; 0x200 = 512 byte
#define     RationCompressStandard 91.7 // ti le nen chuan
#define     RationCompressSampleOK 71.7 // ti le mau dat chuan 
#define     RationSampleRelative   96.8 // ti le  mau dat chuan tuong doi
//#define     RationGeneralRelative  86.8 // ti le chung tuong doi

#define		MAX_SECTIONS		97 
#define		MAXSTARTHEADER		1024 
#define		MAXSTARTCODE		1024
#define		MAXSTARTFILE		96
#define		MAX_BUF				1024

#define		MAXNODE				512 // so nut toi da trong cay Huffman
#define		MAX_BIT				4096 // so bit doi da = 8*512


///////////////////// CCommpress Class

typedef unsigned int FREQCOUNTER;

struct HufNodeStruc
{
	FREQCOUNTER freq;
	int parent;
	int right;
	int left;
};

const int MaxFileNameLength = 256;

class CCompress
{
public:
	
	CCompress();
	virtual ~CCompress();
	
	void Compress();

	BYTE BLOCK[SizeSample]; // khoi du lieu de nen
	int TotalBit;	

private:
	void BuildHufTree();
	void Compress1Byte(int node);
	
private:
	
	HufNodeStruc OurTree[MAXNODE]; //MAXNODE 2^8+2^7+2^6+2^5+2^4+2^3+2^2+2^1
	int NumOfRootNode;
	unsigned char BitCounter;
	unsigned char EncodedCode[256];
	int bit_count;
	
	FREQCOUNTER OrigBytes;
	int ActiveSymbs;
};

///////////////////////////////////////////////////End Compress Class
//
//// CDetectPack Class

struct	_SECTIONDATA
{
	TCHAR	bName[8];
	DWORD	lStartInFile;
	DWORD	lStartInMem;	//	Chua cong Imagebase
	DWORD	lVirtualSize;
	DWORD	lPhysicalSize;
} ;


struct  FILE_INFO
{
	int iETPSection; // Section chua entry Point
	DWORD dwSize,dwPEHdPoint,dwEntryPointRVA, dwImageBase, dwEntryPointFileOffset;
	BYTE bPEHeader[MAXSTARTHEADER];
	BYTE bStartCode[MAXSTARTCODE];
	BYTE bStartFile[MAXSTARTFILE];
	WORD wNumberOFSections;
	WORD wNtHdrSize;
	DWORD dwPEHdrSize;
	int Handle;
	
	_SECTIONDATA	Section[MAX_SECTIONS];	
};


class CDetectPack
{
public:
	
	CDetectPack();

	virtual ~CDetectPack();

	int DectectPack(char * pszFileName/*	must be full path name */);

private :

	DWORD RVAToOffset_( unsigned long Off,FILE_INFO Fi);
	int Reads(int Handle,BYTE *Buff,DWORD StartOff,DWORD Size);
	int _StrCmp(TCHAR S1[10], TCHAR S2[10]);
	BOOL ProccessBlock(DWORD dwOffsetStart,DWORD dwSize);

private:
	
	FILE_INFO Fi; 
	CCompress  CompressBlock;
	 // Cac bien luu ket qua tinh ti le
	int iNumberSampleCompress ;		// So luong mau lay thuc te
	int iNumberSampleCompressRelative; // So luong mau lay tuong doi
	int iNumberSampleCompressOk ;	// So luong mau > Percent_Standard
	int iNumberSampleRelativeOk ;   // so luong mau dat chuan tuong doi
	float fPercent_General;			// Ti le nen chung , = Trung binh ti le nen cua cac BLOCK nen	
	float fPercentSampleOK;			// ti le mau dat chuan = iNumberSampleCompressOk/iNumberSampleCompress
	float fPercent_Standard;		//91,7% ket qua nay co duoc , qua thong ke
	float fPercentSampleOK_Standard; // 
	float fPercentSampleRelativeOK;  // 
	//float fPercentGeneralRelative;   //  
	///////////////////////////////////////////////////////////////////////
};
#endif
