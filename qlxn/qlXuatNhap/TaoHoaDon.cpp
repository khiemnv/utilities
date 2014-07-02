// TaoHoaDon.cpp : implementation file
//

#include "stdafx.h"
#include "qlXuatNhap.h"
#include "TaoHoaDon.h"


// CTaoHoaDon dialog

IMPLEMENT_DYNAMIC(CTaoHoaDon, CDialog)

CTaoHoaDon::CTaoHoaDon(CWnd* pParent /*=NULL*/)
	: CDialog(CTaoHoaDon::IDD, pParent)
	, m_pKmySql(NULL)
{

}

CTaoHoaDon::~CTaoHoaDon()
{
}

void CTaoHoaDon::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTaoHoaDon, CDialog)
END_MESSAGE_MAP()


// CTaoHoaDon message handlers
