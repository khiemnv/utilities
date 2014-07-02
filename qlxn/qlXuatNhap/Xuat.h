#pragma once
#include "alldlg.h"
#include "kmysql.h"

class CXuat :
	public CAllDlg
{
public:
	CXuat(void);
	~CXuat(void);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnInitMenu(CMenu* pMenu);
	KMySQL* ptrMysql;
};
