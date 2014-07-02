// DotDlg.cpp : implementation file
//

#include "stdafx.h"
#include "QLXuatNhap.h"
#include "DotDlg.h"


// CDotDlg dialog

IMPLEMENT_DYNAMIC(CDotDlg, CDialog)

CDotDlg::CDotDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDotDlg::IDD, pParent)
	, m_vStaticDotNhap(_T(""))
	, m_vStaticTenHang(_T(""))
	, m_vStaticNgayNhap(_T(""))
	, m_vStaticComment(_T(""))
	, m_vStaticChungTu(_T(""))
{

}

CDotDlg::~CDotDlg()
{
}

void CDotDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_DOTNHAP_STATIC, m_vStaticDotNhap);
	/*DDX_Text(pDX, IDC_TENHANG_STATIC, m_vStaticTenHang);*/
	DDX_Text(pDX, IDC_NGAYNHAP_STATIC, m_vStaticNgayNhap);
	DDX_Text(pDX, IDC_COMMENT_STATIC, m_vStaticComment);
	DDX_Control(pDX, IDC_DOT_LIST, m_cDotList);
	DDX_Control(pDX, IDC_REMOVE_BUTTON, m_cRemoveBtn);
	DDX_Control(pDX, IDC_EDIT_BUTTON, m_cEditBtn);
	DDX_Control(pDX, IDC_ADD_BUTTON, m_cAddBtn);
	DDX_Control(pDX, IDC_COMENT_EDIT, m_cCommentEdit);
	DDX_Text(pDX, IDC_CHUNGTU_STATIC, m_vStaticChungTu);
}


BEGIN_MESSAGE_MAP(CDotDlg, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CDotDlg message handlers

void CDotDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (::IsWindow(m_cDotList.GetSafeHwnd()))
	{
		resize(cx,cy);
	}
}
void CDotDlg::resize(int cx, int cy)
{
	int nWidth=55,nHeight=24;

	m_cDotList.MoveWindow(9,55,cx-16,cy-55-nHeight-10);
	
	m_cCommentEdit.MoveWindow(390,28,cx-397,nHeight);

	cx-=7+nWidth;
	cy-=7+nHeight;
	m_cRemoveBtn.MoveWindow(cx,cy,nWidth,nHeight);
	cx-=7+nWidth;
	m_cEditBtn.MoveWindow(cx,cy,nWidth,nHeight);
	cx-=7+nWidth;
	m_cAddBtn.MoveWindow(cx,cy,nWidth,nHeight);
}
