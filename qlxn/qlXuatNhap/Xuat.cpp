#include "StdAfx.h"
#include "Xuat.h"
/*#include "Kmysql.h"*/
CXuat::CXuat(void)
{
}

CXuat::~CXuat(void)
{
}
BEGIN_MESSAGE_MAP(CXuat, CAllDlg)
	ON_WM_INITMENU()
END_MESSAGE_MAP()

void CXuat::OnInitMenu(CMenu* pMenu)
{
	CAllDlg::OnInitMenu(pMenu);

	// TODO: Add your message handler code here
// 	KMySQL	xuat_query("pet","localhost","root","rabbit",0,0);
// 	xuat_query.connect();
}
