// TonDlg.cpp : implementation file
//

#include "stdafx.h"
#include "QLXuatNhap.h"
#include "TonDlg.h"


// CTonDlg dialog

IMPLEMENT_DYNAMIC(CTonDlg, CDialog)

CTonDlg::CTonDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTonDlg::IDD, pParent)
	, m_vStaticDotNhap(_T(""))
	, m_vStaticTenHang(_T(""))
	, m_vStaticSoLuongKg(_T(""))
	, m_vStaticSoLuongCay(_T(""))
	, m_vStaticDonGia(_T(""))
	, m_vStaticThanhTienCT(_T(""))
{

}

CTonDlg::~CTonDlg()
{
}

void CTonDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_DOTNHAP_STATIC, m_vStaticDotNhap);
	DDX_Text(pDX, IDC_TENHANG_STATIC, m_vStaticTenHang);
	DDX_Text(pDX, IDC_SOLUONGKG_STATIC, m_vStaticSoLuongKg);
	DDX_Text(pDX, IDC_SOLUONGCAY_STATIC, m_vStaticSoLuongCay);
	DDX_Text(pDX, IDC_DONGIA_STATIC, m_vStaticDonGia);
	DDX_Text(pDX, IDC_THANHTIENCHUATHUE_STATIC, m_vStaticThanhTienCT);
	DDX_Control(pDX, IDC_TON_LIST, m_cTonList);
	DDX_Control(pDX, IDC_ADD_BUTTON, m_cAddBtn);
	DDX_Control(pDX, IDC_EDIT_BUTTON, m_cEditBtn);
	DDX_Control(pDX, IDC_REMOVE_BUTTON, m_cRemoveBtn);
}


BEGIN_MESSAGE_MAP(CTonDlg, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CTonDlg message handlers

void CTonDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (::IsWindow(m_cTonList.GetSafeHwnd()))
	{
		resize(cx,cy);
	}
}

void CTonDlg::resize(int cx, int cy)
{
	int nWidth=55,nHeight=24;

	m_cTonList.MoveWindow(9,55,cx-16,cy-55-nHeight-10);

	//m_cCommentEdit.MoveWindow(390,28,cx-397,nHeight);

	cx-=7+nWidth;
	cy-=7+nHeight;
	m_cRemoveBtn.MoveWindow(cx,cy,nWidth,nHeight);
	cx-=7+nWidth;
	m_cEditBtn.MoveWindow(cx,cy,nWidth,nHeight);
	cx-=7+nWidth;
	m_cAddBtn.MoveWindow(cx,cy,nWidth,nHeight);
}
