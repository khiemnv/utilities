// XuatDlg.cpp : implementation file
//

#include "stdafx.h"
#include "QLXuatNhap.h"
#include "XuatDlg.h"


// CXuatDlg dialog

IMPLEMENT_DYNAMIC(CXuatDlg, CDialog)

CXuatDlg::CXuatDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CXuatDlg::IDD, pParent)
	, m_vStaticTenHang(_T(""))
	, m_vStaticNgay(_T(""))
	, m_vStaticSoLuongKg(_T(""))
	, m_vStaticSoLuongCay(_T(""))
	, m_vStaticDonGia(_T(""))
	, m_vStaticThanhTienCT(_T(""))
	, m_vStaticThueVAT(_T(""))
	, m_vStaticDotNhap(_T(""))
	/*, ptrKmysql(NULL)*/
/*	, qlxn(NULL)*/
{

}

CXuatDlg::~CXuatDlg()
{
}

void CXuatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TENHANG_STATIC, m_vStaticTenHang);
	DDX_Text(pDX, IDC_NGAY_STATIC, m_vStaticNgay);
	DDX_Text(pDX, IDC_SOLUONOGKG_STATIC, m_vStaticSoLuongKg);
	DDX_Text(pDX, IDC_SOLUONGCAY_STATIC, m_vStaticSoLuongCay);
	DDX_Text(pDX, IDC_DONGIA_STATIC, m_vStaticDonGia);
	DDX_Text(pDX, IDC_THANHTIENCHUATHUE_STATIC, m_vStaticThanhTienCT);
	DDX_Text(pDX, IDC_THUEVAT_STATIC, m_vStaticThueVAT);
	DDX_Text(pDX, IDC_DOTNHAP_STATIC, m_vStaticDotNhap);
	DDX_Control(pDX, IDC_XUAT_LIST, m_cXuatList);
	DDX_Control(pDX, IDC_ADD_BUTTON, m_cAddBtn);
	DDX_Control(pDX, IDC_EDIT_BUTTON, m_cEditBtn);
	DDX_Control(pDX, IDC_REMOVE_BUTTON, m_cRemoveBtn);
}


BEGIN_MESSAGE_MAP(CXuatDlg, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CXuatDlg message handlers

void CXuatDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (::IsWindow(m_cXuatList.GetSafeHwnd()))
	{
		resize(cx,cy);
	}
}

void CXuatDlg::resize(int cx, int cy)
{
	int nWidth=55,nHeight=24;

	m_cXuatList.MoveWindow(9,55,cx-16,cy-55-nHeight-10);

	//m_cCommentEdit.MoveWindow(390,28,cx-397,nHeight);

	cx-=7+nWidth;
	cy-=7+nHeight;
	m_cRemoveBtn.MoveWindow(cx,cy,nWidth,nHeight);
	cx-=7+nWidth;
	m_cEditBtn.MoveWindow(cx,cy,nWidth,nHeight);
	cx-=7+nWidth;
	m_cAddBtn.MoveWindow(cx,cy,nWidth,nHeight);
}
