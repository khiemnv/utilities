// Writer: Ngo Quoc Hoan
#include "stdafx.h"
#include "stdio.h"
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>
#include <tchar.h>
#include "CDetectPack.h"

// CDectectPack Class
/////////////////////////////////////////


// Detect Pack File ; 
// Input:  Full path File Name!
// File da duoc kiem tra truoc...
// OutPut Return 0 , if Not Pack ; Return 1 if Pack

int CDetectPack::DectectPack(char * pszFileName) 
{
	
	Fi.Handle= open(pszFileName, _O_BINARY|_O_RDONLY); 
	
	BYTE	bBuf[MAXSTARTHEADER];
	
	if (Fi.Handle==-1)	{ return 0;}; 
	
	Fi.dwSize=_lseek(Fi.Handle,0, SEEK_END); // tinh kich thuc file

	if (Fi.dwSize<=MAXSTARTFILE) 
	{	_close(Fi.Handle); 
			return 0;
	} ;

		
	int  ret = Reads(Fi.Handle,bBuf,0,MAXSTARTFILE); // doc MAXSTARTFILE Byte o dau file vao pBuf

	if (ret<MAXSTARTFILE) 
	 {	 
		 _close(Fi.Handle); 
		 return 0;
	 }

	Fi.dwPEHdPoint=*(DWORD*)(&bBuf[0x3C]); //offset of Start File Header
	
	ret=Reads(Fi.Handle,Fi.bPEHeader,Fi.dwPEHdPoint,MAXSTARTHEADER);

	if (ret<MAXSTARTFILE)	
	{
		 _close(Fi.Handle);
		 return 0;
	}

	if ((Fi.bPEHeader[0]!='P')||(Fi.bPEHeader[1]!='E')) // kiem tra PE
	{
		_close(Fi.Handle); 
		return 0;
	}; 

	Fi.wNumberOFSections = *(WORD*)(&Fi.bPEHeader[0x06]);	// numbers of section number

	if ((Fi.wNumberOFSections >= MAX_SECTIONS) || (Fi.wNumberOFSections < 1))  // Bad header
	{
		_close(Fi.Handle); 
		return 0;
	}

	//	Offset that cua bang section o NtHdrSize
	Fi.wNtHdrSize = *(WORD*)(&Fi.bPEHeader[0x14]) + 0x18;	//section_size +  optional_section_size
	Fi.dwPEHdrSize = *(DWORD*)(&Fi.bPEHeader[0x54]); //
	if (Fi.wNtHdrSize+Fi.wNumberOFSections*0x28>MAXSTARTHEADER) 
	{
		_close(Fi.Handle); 
		return 0;
	}

		
	Fi.dwImageBase=*(DWORD*)(&Fi.bPEHeader[0x34]);
	Fi.dwEntryPointRVA=*(DWORD*)(&Fi.bPEHeader[0x28])+Fi.dwImageBase;

	//Lay thong tin Sections
	int i;
	for(i=0;i<(int)Fi.wNumberOFSections;i++)
	{
		memcpy(Fi.Section[i].bName,Fi.bPEHeader+Fi.wNtHdrSize+i*0x28,8);
		Fi.Section[i].lStartInFile=*(DWORD*)(Fi.bPEHeader+Fi.wNtHdrSize+i*0x28+0x14);
		Fi.Section[i].lStartInMem=*(DWORD*)(Fi.bPEHeader+Fi.wNtHdrSize+i*0x28+0x0C);
		Fi.Section[i].lVirtualSize=*(DWORD*)(Fi.bPEHeader+Fi.wNtHdrSize+i*0x28+0x08);
		Fi.Section[i].lPhysicalSize=*(DWORD*)(Fi.bPEHeader+Fi.wNtHdrSize+i*0x28+0x10);

		if ((Fi.dwEntryPointRVA>=(Fi.Section[i].lStartInMem+Fi.dwImageBase))&&(Fi.dwEntryPointRVA<=(Fi.Section[i].lStartInMem+Fi.dwImageBase+Fi.Section[i].lVirtualSize)))
		{
			Fi.iETPSection=i; // Section chua Entry point
		}
	
	}

	
	//	Lay EntryPoint
		
	Fi.dwEntryPointFileOffset=(unsigned int)RVAToOffset_(Fi.dwEntryPointRVA,Fi);
	if (Fi.dwEntryPointFileOffset==-1) 
	{
		_close(Fi.Handle); 
		return 0;
	}
	//////////////////


	// Duyet cac cac section kha nghi chua du lieu nen

	int NSectionSkip = 5; //bo qua section .rsrc , .edata, . idata, .debug, .reloc
	BOOL Skip;
	TCHAR *SectionSkip[7];
	SectionSkip[0]=_T(".rsrc");
	SectionSkip[1]=_T(".edata");
	SectionSkip[2]=_T(".idata");
	SectionSkip[3]=_T(".debug");
	SectionSkip[4]=_T(".Reloc");
		

	// Cac bien luu ket qua tinh ti le 	 
	iNumberSampleCompress=0;// So luong mau lay thuc te
	iNumberSampleCompressRelative=0; // So luong mau lay tuong doi
	iNumberSampleCompressOk=0 ;	 // So luong mau > Percent_Standard
	iNumberSampleRelativeOk=0; // so luong mau dat tuong doi
	fPercent_General = 0; // Ti le nen chung , = Trung binh ti le nen cua cac BLOCK nen	
	fPercentSampleOK= 0; // ti le mau dat chuan = iNumberSampleCompressOk/iNumberSampleCompress
	fPercent_Standard=float(RationCompressStandard); //91,7% ket qua nay co duoc , qua thong ke
	fPercentSampleOK_Standard=(float)(RationCompressSampleOK);
//	fPercentGeneralRelative = 0;

	///////////////////////////////////////////////////////////////////////
	DWORD dwSizeBlock=0; // tinh toan size cua Block
	
	for (i=0;i<Fi.wNumberOFSections;i++)
	{
		// kiem tra Section co hop le hay ko?
		Skip=FALSE;
		for (int j=0;j<NSectionSkip;j++)
		{
			if (_StrCmp(Fi.Section[i].bName,SectionSkip[j])==0)			
			{
				Skip=TRUE;
				break;
			}
		}

		if ((!Skip)&&(Fi.Section[i].lPhysicalSize>=SizeSample))
		{
		 if (((Fi.Section[i].lStartInFile+Fi.Section[i].lPhysicalSize)<Fi.dwSize))
			{
				
			 if (Fi.iETPSection==i) // la section chua Entry Point
			 {
				
				// Tranh doan code UnPack
				
				 dwSizeBlock=Fi.dwEntryPointFileOffset-Fi.Section[i].lStartInFile;
				 if (dwSizeBlock>SizeSample*4)
				 {
				   dwSizeBlock=dwSizeBlock-0x100;
				   ProccessBlock(Fi.Section[i].lStartInFile,dwSizeBlock);
					
				 }
				 
	
				 dwSizeBlock=Fi.Section[i].lPhysicalSize+Fi.Section[i].lStartInFile-Fi.dwEntryPointFileOffset;
				 if (dwSizeBlock>SizeSample*4)
				 {
					dwSizeBlock=dwSizeBlock-0x100;
					ProccessBlock(Fi.Section[i].lStartInFile,dwSizeBlock); 		 
				 };
	
			 } 
			 else
			 {
				 ProccessBlock(Fi.Section[i].lStartInFile,Fi.Section[i].lPhysicalSize);
			  
			 }

			}
		}

	}
	
	if (iNumberSampleCompress!=0)
	{
		fPercent_General=(float)(fPercent_General/iNumberSampleCompress);
		fPercentSampleOK=(float)(iNumberSampleCompressOk*100/iNumberSampleCompress);
		fPercentSampleRelativeOK=(float)(iNumberSampleRelativeOk*100/iNumberSampleCompressRelative);
	//	fPercentGeneralRelative=(float)(fPercentGeneralRelative/iNumberSampleCompressRelative)*100;
	}
		
	if ((fPercentSampleOK>=fPercentSampleOK_Standard)||(fPercentSampleRelativeOK>=(float)RationSampleRelative)) 
	{
		_close(Fi.Handle);
		return 1;
	}
	
	_close(Fi.Handle);
	return 0;
}



// Viet lai ham so sanh xau 
//so sanh 2 xau s1, s2 , neu s2 =s1  return 0; else 1;
int CDetectPack::_StrCmp(TCHAR S1[10], TCHAR S2[10])
{
	int i=0;
	while  ((i<8)&&(S1[i]!='\0')&&(S1[i]!='\0')&&(S1[i]==S2[i])) i++;
	if (S1[i]==S2[i]) return 0;
	return 1;

}
//////////////////


// Viet Lai Ham doc file
int CDetectPack::Reads(int Handle,BYTE *Buff,DWORD StartOff,DWORD Size)
{
	if (_lseek(Handle, StartOff, SEEK_SET)==-1) return 0;
	int ret = _read(Handle, Buff,Size);
	return ret;
}


// OffsetToFile 

DWORD CDetectPack::RVAToOffset_( unsigned long Off,FILE_INFO Fi)
{
	unsigned long		first, last, t, rt;

	//PListSectionsData pTempSection = Fi.StartSession;
	for (int i=0;i<Fi.wNumberOFSections;i++)
	{
		first = Fi.Section[i].lStartInMem + Fi.dwImageBase;
		
		if (Fi.Section[i].lPhysicalSize)
			last = first + Fi.Section[i].lPhysicalSize;
		else         
			last = first +Fi.Section[i].lVirtualSize;
		
		if ((Off >= first) && (Off < last))
		{
			t = Off - first;
			rt = Fi.Section[i].lStartInFile + t;
			
			if (rt <= Fi.dwSize)
				return(rt);
			else
				return -1;
		}
		
	}
	
	return -1;
							
}



//  Process Block , chia Block thanh cac Sample
//  tinh toan ti le nen cua tung Sample ...
//  dau vao phai dam bao du lieu vao la hop le -> trong ham ko kiem tra nua

BOOL CDetectPack::ProccessBlock(DWORD dwOffsetStart,DWORD dwSize)
{
	int n=DWORD(dwSize/RationSample);// so luong mau lay trong Block
	DWORD Odd=dwSize-n*RationSample; // so du
	DWORD OffsetStartSample;
	float t=0; // ti le tam thoi trong qua trinh tinh
	
	if (n==0)
	{
		Odd=DWORD(dwSize/2);
		OffsetStartSample =dwOffsetStart+Odd-0x100;
		
		int ret=Reads(Fi.Handle,CompressBlock.BLOCK,OffsetStartSample,SizeSample);
		if (ret==SizeSample)
		{
			iNumberSampleCompress++;
			iNumberSampleCompressRelative+=DWORD(dwSize/SizeSample);
			
			// tinh toan ti le nen
			CompressBlock.Compress();
			
			t=(float)(CompressBlock.TotalBit*100/MAX_BIT);
			if (t>=fPercent_Standard)  
			{
				iNumberSampleCompressOk++;
				iNumberSampleRelativeOk+=DWORD(dwSize/SizeSample);
			}
			
			fPercent_General+=t;
//			fPercentGeneralRelative+=(t/100)*DWORD(dwSize/SizeSample);
			
		}
		

	} 
	else //n>0
	{
       dwOffsetStart=dwOffsetStart+DWORD(Odd/2);
	
	   for (int i=0; i<n; i++)
		{
			OffsetStartSample =dwOffsetStart+i*RationSample+DWORD(RationSample/2)-0x100;
			
			int ret=Reads(Fi.Handle,CompressBlock.BLOCK,OffsetStartSample,SizeSample);
			if (ret==SizeSample)
			{
				
				iNumberSampleCompress++;
				iNumberSampleCompressRelative+=DWORD(dwSize/SizeSample);

				// tinh toan ti le nen
				CompressBlock.Compress();
				
				t=(float)(CompressBlock.TotalBit*100/MAX_BIT);
				if (t>=fPercent_Standard)  
				{
					iNumberSampleCompressOk++;
					iNumberSampleRelativeOk+=DWORD(dwSize/SizeSample);
				}
				
				fPercent_General+=t;
			//	fPercentGeneralRelative+=(float)(t/100)*DWORD(dwSize/SizeSample);
				
			}
			
			
		}


	}
	
	return TRUE;
}


CDetectPack::CDetectPack()
{

}


CDetectPack::~CDetectPack()
{
	
}



///End CDetectPack Class

//////////////////////////////////////

// Class Compress

CCompress::CCompress()
{
}


CCompress::~CCompress()
{
	
}

void CCompress::Compress()
{
	// khoi tao
	OrigBytes = 0;
	ActiveSymbs = 0;
	TotalBit = 0;
	int i=0;

	BitCounter = 0;
	NumOfRootNode = 0;
	for (i = 0; i < 511; i++) 
	{
		OurTree[i].freq = 0;
		OurTree[i].parent=-1;
		OurTree[i].left=-1;
		OurTree[i].right=-1;
	};
	for (i = 0; i < 256; i++) EncodedCode[i] = 0;


	////////

	for ( i=0;i<SizeSample;i++)
	{
	  if (OurTree[BLOCK[i]].freq==0)
	  {
		ActiveSymbs++;
	  }
	  OurTree[BLOCK[i]].freq++;
	  OrigBytes++;
	}
	
	
	BuildHufTree();
	
	bit_count = 0;

	for (i=0;i<256;i++)
	{
		if (OurTree[i].freq!=0)
		{
			Compress1Byte(i);
			EncodedCode[i] = bit_count;
			TotalBit=TotalBit+OurTree[i].freq*bit_count;
			bit_count = 0;
			
		}
		
	}
	
	
}

void CCompress::BuildHufTree()
{
	int NodeCounter = 256;
	int i;
	
	for (i = 0; i < NodeCounter; i++)
	{
		OurTree[i].parent = -1;
		OurTree[i].right = -1;
		OurTree[i].left = -1;
	}
	
	while (1)
	{
		int MinFreq0 = -1;
		int MinFreq1 = -1;
		
		// tim 2 node co gia tri nho nhat
		for (i = 0; i < NodeCounter; i++)
		{
			if (i != MinFreq0)
			{
				if (OurTree[i].freq > 0 && OurTree[i].parent == -1)
				{
					if (MinFreq0 == -1 || OurTree[i].freq < OurTree[MinFreq0].freq)
					{
						if (MinFreq1 == -1 || OurTree[i].freq < OurTree[MinFreq1].freq)
							MinFreq1 = MinFreq0;
						MinFreq0 = i;
					}
					else if (MinFreq1 == -1 || OurTree[i].freq < OurTree[MinFreq1].freq)
						MinFreq1 = i;
				}
			}
		}
		if (MinFreq1 == -1)
		{
			NumOfRootNode = MinFreq0;
			break;
		}
		
		//Combine two nodes to form a parent node
		OurTree[MinFreq0].parent = NodeCounter;
		OurTree[MinFreq1].parent = NodeCounter;
		OurTree[NodeCounter].freq = OurTree[MinFreq0].freq + OurTree[MinFreq1].freq;
		OurTree[NodeCounter].right = MinFreq0;
		OurTree[NodeCounter].left = MinFreq1;
		OurTree[NodeCounter].parent = -1;
		NodeCounter++;
	}
}


void CCompress::Compress1Byte(int node)
{
	if (OurTree[node].parent != -1)
	{
		bit_count++;
		Compress1Byte(OurTree[node].parent);
		// dieu kien kiem soat tran bo nho
		 if (bit_count>20 ) return;
	}

}

/// End Class Compress
