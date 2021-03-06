// NhapDlg.cpp : implementation file
//

#include "stdafx.h"
#include "QLXuatNhap.h"
#include "NhapDlg.h"


// CNhapDlg dialog

IMPLEMENT_DYNAMIC(CNhapDlg, CDialog)

CNhapDlg::CNhapDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNhapDlg::IDD, pParent)
	, m_vStaticTenHang(_T(""))
	, m_vStaticSoLuongKg(_T(""))
	, m_vStaticSoLuongCay(_T(""))
	, m_vStaticDonGia(_T(""))
	, m_vStaticThanhTienCT(_T(""))
	, m_vStaticThueVAT(_T(""))
	, m_vStaticDotNhap(_T(""))
	, m_vDotNhap(_T(""))
	, m_vTenHang(_T(""))
	, m_vSoLuongKg(_T(""))
	, m_vSoLuongCay(_T(""))
	, m_vDonGia(_T(""))
	, m_vThue(0)
	, m_vComment(_T(""))
{

}

CNhapDlg::~CNhapDlg()
{
}

void CNhapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TENHANG_STATIC, m_vStaticTenHang);
	DDX_Text(pDX, IDC_SOLUONGKG_STATIC, m_vStaticSoLuongKg);
	DDX_Text(pDX, IDC_SOLUONGCAY_STATIC, m_vStaticSoLuongCay);
	DDX_Text(pDX, IDC_DONGIA_STATIC, m_vStaticDonGia);
	DDX_Text(pDX, IDC_THANHTIENCHUATHUE_STATIC, m_vStaticThanhTienCT);
	DDX_Text(pDX, IDC_THUEVAT_STATIC, m_vStaticThueVAT);
	DDX_Text(pDX, IDC_DOTNHAP_STATIC, m_vStaticDotNhap);
	DDX_Control(pDX, IDC_NHAP_LIST, m_cNhapList);
	DDX_Control(pDX, IDC_REMOVE_BUTTON, m_cRemoveBtn);
	DDX_Control(pDX, IDC_EDIT_BUTTON, m_cEditBtn);
	DDX_Control(pDX, IDC_ADD_BUTTON, m_cAddBtn);
	DDX_Text(pDX, IDC_DOT_NHAP_EDIT, m_vDotNhap);
	DDX_Text(pDX, IDC_TENHANG_EDIT, m_vTenHang);
	DDX_Text(pDX, IDC_SO_LUONG_KG_EDIT, m_vSoLuongKg);
	DDX_Text(pDX, IDC_SO_LUONG_CAY_EDIT, m_vSoLuongCay);
	DDX_Text(pDX, IDC_DON_GIAI_EDIT, m_vDonGia);
	DDX_Text(pDX, IDC_COMENT_EDIT, m_vComment);
}


BEGIN_MESSAGE_MAP(CNhapDlg, CDialog)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_ADD_BUTTON, &CNhapDlg::OnBnClickedAddButton)
	ON_EN_CHANGE(IDC_DOT_NHAP_EDIT, &CNhapDlg::OnEnChangeDotNhapEdit)
	ON_EN_CHANGE(IDC_TENHANG_EDIT, &CNhapDlg::OnEnChangeTenhangEdit)
	ON_STN_CLICKED(IDC_SOLUONGKG_STATIC, &CNhapDlg::OnStnClickedSoluongkgStatic)
	ON_EN_CHANGE(IDC_SO_LUONG_CAY_EDIT, &CNhapDlg::OnEnChangeSoLuongCayEdit)
	ON_EN_CHANGE(IDC_DON_GIAI_EDIT, &CNhapDlg::OnEnChangeDonGiaiEdit)
	ON_BN_CLICKED(IDC_RADIO1, &CNhapDlg::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CNhapDlg::OnBnClickedRadio2)
END_MESSAGE_MAP()


// CNhapDlg message handlers

void CNhapDlg::resize(int cx, int cy)
{
	int nWidth=55,nHeight=24;

	m_cNhapList.MoveWindow(9,55,cx-16,cy-55-nHeight-10);

	//m_cCommentEdit.MoveWindow(390,28,cx-397,nHeight);

	cx-=7+nWidth;
	cy-=7+nHeight;
	m_cRemoveBtn.MoveWindow(cx,cy,nWidth,nHeight);
	cx-=7+nWidth;
	m_cEditBtn.MoveWindow(cx,cy,nWidth,nHeight);
	cx-=7+nWidth;
	m_cAddBtn.MoveWindow(cx,cy,nWidth,nHeight);
}

void CNhapDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (::IsWindow(m_cNhapList.GetSafeHwnd()))
	{
		resize(cx,cy);
	}
}

void CNhapDlg::OnBnClickedAddButton()
{
	// TODO: Add your control notification handler code here
	/*qlxn->m_cMySql.select_table("nhap");*/
// 	CString* values;
// 	int		number_fields=7;
// 	values = new CString[number_fields];
// 	values[0]=m_vTenHang;
// 	values[1]=m_vSoLuongKg;
// 	values[2]=m_vDotNhap;
// 	values[3]=m_vSoLuongCay;
// 	values[4]=m_vDonGia;
// 	char	temp[10];
// 	sprintf_s(temp,10,"%f",m_vThue);
// 	values[5]=CString(temp);
// 	values[6]=m_vComment;
// 	m_cKmysql->insert_row(values,number_fields);
}

void CNhapDlg::OnEnChangeDotNhapEdit()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
}

void CNhapDlg::OnEnChangeTenhangEdit()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
}

void CNhapDlg::OnStnClickedSoluongkgStatic()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
}

void CNhapDlg::OnEnChangeSoLuongCayEdit()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
}

void CNhapDlg::OnEnChangeDonGiaiEdit()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
}

void CNhapDlg::OnBnClickedRadio1()
{
	// TODO: Add your control notification handler code here
	/*m_vThue=0.05;*/
}

void CNhapDlg::OnBnClickedRadio2()
{
	// TODO: Add your control notification handler code here
	/*m_vThue=0.1;*/
}
