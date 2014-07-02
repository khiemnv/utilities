#include "StdAfx.h"
#include "KKString.h"

CKKString::CKKString(void)
{
}

CKKString::~CKKString(void)
{
}
char* CKKString::cstrWtoAstr(CStringW cstrW)
{
	size_t	newsize=(cstrW.GetLength()+1)*2;
	char* Astr=new char[newsize];
	size_t convertedCharsw=0;
	wcstombs_s(&convertedCharsw,Astr,newsize,cstrW,_TRUNCATE);
	return Astr;
}

char* CKKString::cstrAtoAstr(CStringA cstrA)
{
	size_t	newsize=(cstrA.GetLength()+1);
	char* Astr=new char[newsize];
	lstrcpyA(Astr,cstrA);
	return Astr;
}

// bool CKKString::cstrWtoAstr(CStringW &cstrW, char *buff, int buff_size)
// {
// 	size_t convertedCharsw=0;
// 	return wcstombs_s(&convertedCharsw,buff,buff_size,cstrW,_TRUNCATE);
/*}*/