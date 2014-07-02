#pragma once
#ifdef UNICODE
#define cstrToAstr cstrWtoAstr
#else
#define cstrToAstr cstrAtoAstr
#endif

class CKKString
{
public:
	CKKString(void);
	~CKKString(void);
	static char* cstrWtoAstr(CStringW cstrW);
	static char* cstrAtoAstr(CStringA cstrA);
};