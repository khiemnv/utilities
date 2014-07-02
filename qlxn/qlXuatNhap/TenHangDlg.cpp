// TenHangDlg.cpp : implementation file
//

#include "stdafx.h"
#include "QLXuatNhap.h"
#include "TenHangDlg.h"


// CTenHangDlg dialog

IMPLEMENT_DYNAMIC(CTenHangDlg, CDialog)

CTenHangDlg::CTenHangDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTenHangDlg::IDD, pParent)
	, m_vStaticTenHangBrif(_T(""))
	, m_vStaticTenHangFull(_T(""))
{

}

CTenHangDlg::~CTenHangDlg()
{
}

void CTenHangDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TENHANGBRIF_STATIC, m_vStaticTenHangBrif);
	DDX_Text(pDX, IDC_TENHANGFULL_STATIC, m_vStaticTenHangFull);
	DDX_Control(pDX, IDC_TENHANG_LIST, m_cTenHangList);
	DDX_Control(pDX, IDC_TEN_HANG_FULL_EDIT, m_cTenHangFull);
	DDX_Control(pDX, IDC_TEN_HANG_BRIF_EDIT, m_cTenHangBrif);
	DDX_Control(pDX, IDC_REMOVE_BUTTON, m_cRemoveBtn);
	DDX_Control(pDX, IDC_EDIT_BUTTON, m_cEditBtn);
	DDX_Control(pDX, IDC_ADD_BUTTON, m_cAddBtn);
}


BEGIN_MESSAGE_MAP(CTenHangDlg, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CTenHangDlg message handlers

void CTenHangDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (::IsWindow(m_cTenHangList.GetSafeHwnd()))
	{
		resize(cx,cy);
	}
}

void CTenHangDlg::resize(int cx, int cy)
{
	int nWidth=55,nHeight=24;

	m_cTenHangList.MoveWindow(9,55,cx-16,cy-55-nHeight-10);

	m_cTenHangFull.MoveWindow(315,28,cx-322,nHeight);

	cx-=7+nWidth;
	cy-=7+nHeight;
	m_cRemoveBtn.MoveWindow(cx,cy,nWidth,nHeight);
	cx-=7+nWidth;
	m_cEditBtn.MoveWindow(cx,cy,nWidth,nHeight);
	cx-=7+nWidth;
	m_cAddBtn.MoveWindow(cx,cy,nWidth,nHeight);
}
