#pragma once
#ifndef ___KKSTRING_H___
#define ___KKSTRING_H___
//implement code here
#ifdef UNICODE
#define cstrToAstr CKKString::cstrWtoAstr
#else
#define cstrToAstr CKKString::cstrAtoAstr
#endif

class CKKString
{
public:
	CKKString(void);
	~CKKString(void);
	static char* cstrWtoAstr(CStringW cstrW);
	static char* cstrAtoAstr(CStringA cstrA);
	static bool cstrWtoAstr(CStringW& cstrW, WCHAR* buff, int buff_size);
	static bool cstrWtoAstr(CStringA& cstrA, char* buff, int buff_size);
	static char* cstrA;
};

#endif