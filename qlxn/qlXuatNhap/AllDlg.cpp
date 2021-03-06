// AllDlg.cpp : implementation file
//

#include "stdafx.h"
#include "QLXuatNhap.h"
#include "AllDlg.h"

// CAllDlg dialog

IMPLEMENT_DYNAMIC(CAllDlg, CDialog)

CAllDlg::CAllDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAllDlg::IDD, pParent)
	, m_vStaticDotNhap(_T(""))
	, m_vStaticChungTu(_T(""))
	, m_vStaticSoLuongKgN(_T(""))
	, m_vStaticSoLuongCayN(_T(""))
	, m_vStaticDonGia(_T(""))
	, m_vStaticThanhTienCT(_T(""))
	, m_vStaticThue(_T(""))
	, m_vStaticNgayXuat(_T(""))
	, m_vStaticSoLuongKgX(_T(""))
	, m_vStaticSoLuongCayX(_T(""))
	, m_vStaticCommnet(_T(""))
	, m_vStaticXuat(_T(""))
	, m_vStaticNgayNhap(_T(""))
	, tab_id(0)
	, m_vDotNhap(_T(""))
	, m_vChungTu(_T(""))
	, m_vNgayNhap(_T(""))
	, m_vTenHangBrif(_T(""))
	, m_vTenHangFull(_T(""))
	, m_vSoLuongKgN(_T(""))
	, m_vSoLuongCayN(_T(""))
	, m_vDonGia(_T(""))
	, m_vThueNhap(_T(""))
	, m_vNgayXuat(_T(""))
	, m_vSoLuongKgX(_T(""))
	, m_vSoLuongCayX(_T(""))
	, m_vComment(_T(""))
	, total_row(0)
{

}

CAllDlg::~CAllDlg()
{
}

void CAllDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_DOTNHAP_STATIC, m_vStaticDotNhap);
	DDX_Text(pDX, IDC_CHUNGTU_STATIC, m_vStaticChungTu);
	DDX_Text(pDX, IDC_TENHANG_STATIC, m_vStaticTenHangBrif);
	DDX_Text(pDX, IDC_TENHANGFULL_STATIC, m_vStaticTenHangFull);
	DDX_Text(pDX, IDC_NHAP_STATIC, m_vStaticNhap);
	DDX_Text(pDX, IDC_NSOLUONGKG_STATIC, m_vStaticSoLuongKgN);
	DDX_Text(pDX, IDC_SOLUONGCAY_STATIC, m_vStaticSoLuongCayN);
	DDX_Text(pDX, IDC_DONGIA_STATIC, m_vStaticDonGia);
	DDX_Text(pDX, IDC_THANHTIENCHUATHUE_STATIC, m_vStaticThanhTienCT);
	DDX_Text(pDX, IDC_THUEVAT_STATIC, m_vStaticThue);
	DDX_Text(pDX, IDC_NGAY_STATIC, m_vStaticNgayXuat);
	DDX_Text(pDX, IDC_SOLUONOGKG_STATIC, m_vStaticSoLuongKgX);
	DDX_Text(pDX, IDC_SOLUONGCAY_STATIC2, m_vStaticSoLuongCayX);
	DDX_Text(pDX, IDC_COMMENT_STATIC, m_vStaticCommnet);
	DDX_Control(pDX, IDC_ADD_BUTTON2, m_cAddBtn);
	DDX_Control(pDX, IDC_EDIT_BUTTON, m_cEditBtn);
	DDX_Control(pDX, IDC_REMOVE_BUTTON, m_cRemoveBtn);
	DDX_Control(pDX, IDC_ALL_LIST, m_cAllList);
	DDX_Text(pDX, IDC_XUAT_STATIC, m_vStaticXuat);
	DDX_Text(pDX, IDC_NGAYNHAP_STATIC, m_vStaticNgayNhap);
	DDX_Control(pDX, IDC_TEN_HANG_FULL_EDIT, m_cTenHangFullEdit);
	DDX_Control(pDX, IDC_COMENT_EDIT, m_cCommentEdit);
	DDX_Control(pDX, IDC_XUAT_STATIC, m_cStaticXuat);
	DDX_Text(pDX, IDC_DOT_NHAP_EDIT, m_vDotNhap);
	DDX_Text(pDX, IDC_CHUNGTU_EDIT, m_vChungTu);
	DDX_DateTimeCtrl(pDX, IDC_NHAP_DATETIMEPICKER, m_vNgayNhap);
	DDX_Text(pDX, IDC_TEN_HANG_BRIF_EDIT, m_vTenHangBrif);
	DDX_Text(pDX, IDC_TEN_HANG_FULL_EDIT, m_vTenHangFull);
	DDX_Text(pDX, IDC_SO_LUONG_KG_EDIT, m_vSoLuongKgN);
	DDX_Text(pDX, IDC_SO_LUONG_CAY_EDIT, m_vSoLuongCayN);
	DDX_Text(pDX, IDC_DON_GIAI_EDIT, m_vDonGia);
	DDX_DateTimeCtrl(pDX, IDC_XUAT_DATETIMEPICKER, m_vNgayXuat);
	DDX_Text(pDX, IDC_SO_LUONG_KG_EDIT2, m_vSoLuongKgX);
	DDX_Text(pDX, IDC_SO_LUONG_CAY_EDIT2, m_vSoLuongCayX);
	DDX_Text(pDX, IDC_COMENT_EDIT, m_vComment);
	DDX_Control(pDX, IDC_DOT_NHAP_EDIT, m_cDotNhap);
	DDX_Control(pDX, IDC_CHUNGTU_EDIT, m_cChungTu);
	DDX_Control(pDX, IDC_NHAP_DATETIMEPICKER, m_cNgayNhap);
	DDX_Control(pDX, IDC_TEN_HANG_BRIF_EDIT, m_cTenHangBrif);
	DDX_Control(pDX, IDC_SO_LUONG_KG_EDIT, m_cSoLuongKgN);
	DDX_Control(pDX, IDC_SO_LUONG_CAY_EDIT, m_cSoLuongCayN);
	DDX_Control(pDX, IDC_DON_GIAI_EDIT, m_cDonGiaN);
	DDX_Control(pDX, IDC_THANH_TIEN_CHUA_THUE_EDIT, m_cThanhTienCT);
	DDX_Control(pDX, IDC_XUAT_DATETIMEPICKER, m_cNgayXuat);
	DDX_Control(pDX, IDC_SO_LUONG_KG_EDIT2, m_cSoLuongKgX);
	DDX_Control(pDX, IDC_SO_LUONG_CAY_EDIT2, m_cSoLuongCayX);
	DDX_Control(pDX, IDC_RADIO1, m_cThueRadio005);
	DDX_Control(pDX, IDC_RADIO2, m_cThueRadio010);
}


BEGIN_MESSAGE_MAP(CAllDlg, CDialog)
	ON_WM_SIZE()
//	ON_EN_CHANGE(IDC_COMENT_EDIT, &CAllDlg::OnEnChangeComentEdit)
//	ON_WM_ENABLE()
//ON_WM_INITMENU()
//ON_WM_INITMENUPOPUP()
//ON_WM_ACTIVATE()
//ON_WM_CREATE()
ON_BN_CLICKED(IDC_ADD_BUTTON2, &CAllDlg::OnBnClickedAddButton2)
ON_BN_CLICKED(IDC_RADIO1, &CAllDlg::OnBnClickedRadio1)
ON_BN_CLICKED(IDC_RADIO2, &CAllDlg::OnBnClickedRadio2)
//ON_WM_CREATE()
//ON_NOTIFY(LVN_ITEMCHANGED, IDC_ALL_LIST, &CAllDlg::OnLvnItemchangedAllList)
//ON_NOTIFY(NM_RDBLCLK, IDC_ALL_LIST, &CAllDlg::OnNMRDblclkAllList)
//ON_NOTIFY(NM_DBLCLK, IDC_ALL_LIST, &CAllDlg::OnNMDblclkAllList)
//ON_NOTIFY(NM_CLICK, IDC_ALL_LIST, &CAllDlg::OnNMClickAllList)
//ON_NOTIFY(NM_CLICK, IDC_ALL_LIST, &CAllDlg::OnNMClickAllList)
ON_NOTIFY(NM_DBLCLK, IDC_ALL_LIST, &CAllDlg::OnNMDblclkAllList)
//ON_NOTIFY(HDN_ITEMCLICK, 0, &CAllDlg::OnHdnItemclickAllList)
//ON_NOTIFY(LVN_ITEMCHANGED, IDC_ALL_LIST, &CAllDlg::OnLvnItemchangedAllList)
ON_BN_CLICKED(IDC_EDIT_BUTTON, &CAllDlg::OnBnClickedEditButton)
//ON_COMMAND(ID_FILE_EXIT, &CAllDlg::OnFileExit)
//ON_COMMAND(ID_OPTION_ADDMODE, &CAllDlg::OnOptionAddmode)
//ON_COMMAND(ID_OPTION_EDITMODE, &CAllDlg::OnOptionEditmode)
//ON_COMMAND(ID_OPTION_REMOVEMODE, &CAllDlg::OnOptionRemovemode)
//ON_COMMAND(ID_OPTION_ADDMODE, &CAllDlg::OnOptionAddmode)
ON_BN_CLICKED(IDC_REMOVE_BUTTON, &CAllDlg::OnBnClickedRemoveButton)
//ON_NOTIFY(NM_SETFOCUS, IDC_ALL_LIST, &CAllDlg::OnNMSetfocusAllList)
//ON_NOTIFY(HDN_ITEMCLICK, 0, &CAllDlg::OnHdnItemclickAllList)
ON_NOTIFY(NM_RCLICK, IDC_ALL_LIST, &CAllDlg::OnNMRClickAllList)
END_MESSAGE_MAP()


// CAllDlg message handlers

void CAllDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (::IsWindow(m_cTenHangFullEdit.GetSafeHwnd()))
	{
		resize(cx,cy);
	}
}

void CAllDlg::resize(int cx, int cy)
{
	int nWidth=495,nHeight=24;

	m_cTenHangFullEdit.MoveWindow(nWidth,28,cx-nWidth-7,nHeight);

	m_cStaticXuat.MoveWindow(nWidth,55,cx-nWidth-7,63);

	nWidth=9;
	m_cAllList.MoveWindow(nWidth,155,cx-nWidth-7,cy-155-nHeight-10);

	nWidth=60;
	m_cCommentEdit.MoveWindow(nWidth,125,cx-nWidth-7,nHeight);

	nWidth=55;

	cx-=7+nWidth;
	cy-=7+nHeight;
	m_cRemoveBtn.MoveWindow(cx,cy,nWidth,nHeight);
	cx-=7+nWidth;
	m_cEditBtn.MoveWindow(cx,cy,nWidth,nHeight);
	cx-=7+nWidth;
	m_cAddBtn.MoveWindow(cx,cy,nWidth,nHeight);
}

//void CAllDlg::OnEnChangeComentEdit()
//{
//	// TODO:  If this is a RICHEDIT control, the control will not
//	// send this notification unless you override the CDialog::OnInitDialog()
//	// function and call CRichEditCtrl().SetEventMask()
//	// with the ENM_CHANGE flag ORed into the mask.
//
//	// TODO:  Add your control notification handler code here
//	UpdateData(TRUE);
//}

//void CAllDlg::OnEnable(BOOL bEnable)
//{
//	CDialog::OnEnable(bEnable);
//
//	// TODO: Add your message handler code here
//
//}

//void CAllDlg::OnInitMenu(CMenu* pMenu)
//{
//	CDialog::OnInitMenu(pMenu);
//
//	// TODO: Add your message handler code here
//}

//void CAllDlg::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
//{
//	CDialog::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
//
//	// TODO: Add your message handler code here
//}

//void CAllDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
//{
//	CDialog::OnActivate(nState, pWndOther, bMinimized);
//
//	// TODO: Add your message handler code here
//}

//int CAllDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
//{
//	if (CDialog::OnCreate(lpCreateStruct) == -1)
//		return -1;
//
//	// TODO:  Add your specialized creation code here
//	set_comment();
//
//	return 0;
//}

void CAllDlg::set_comment(void)
{
	set_init_var();

	CString formatStr = _T("yyyy'-'MM'-'dd");
	m_cNgayNhap.SetFormat(formatStr);
	m_cNgayXuat.SetFormat(formatStr);

	m_vStaticChungTu=_T("Mã số chứng từ");
	m_vStaticCommnet=_T("Ghi chú");
	m_vStaticDonGia=_T("Đơn giá");
	m_vStaticDotNhap=_T("Đợt nhập");
	m_vStaticNgayXuat=_T("Ngày Xuất");
	m_vStaticNgayNhap=_T("Ngày Nhập");
	m_vStaticNhap=_T("Nhập");
	m_vStaticSoLuongKgN=_T("Số lượng (kg)");
	m_vStaticSoLuongCayN=_T("Số lượng (cây)");
	m_vStaticSoLuongCayX=_T("Số lượng (cây)");
	m_vStaticSoLuongKgX=_T("Số lượng (kg))");
	m_vStaticTenHangBrif=_T("Mã số hàng");
	m_vStaticTenHangFull=_T("Tên hàng");
	m_vStaticThanhTienCT=_T("Thàng tiền chưa thuế");
	m_vStaticThue=_T("Thuế");
	m_vStaticXuat=_T("Xuất");
	UpdateData(FALSE);
}

//BOOL CAllDlg::OnInitDialog()
//{
//	CDialog::OnInitDialog();
//
//	// TODO:  Add extra initialization here
//	set_comment();
//
//	return TRUE;  // return TRUE unless you set the focus to a control
//	// EXCEPTION: OCX Property Pages should return FALSE
//}

void CAllDlg::OnBnClickedAddButton2()
{
	// TODO: Add your control notification handler code here
	CString strArr[20];
	int		n_field;
	CString	q;
	UpdateData(TRUE);

	switch(tab_id)
	{
	case TAB_NHAP_ID:
		ptr_cKmysql->select_table(NHAP_TABLE);
		n_field=N_NHAP_FIELD;
		strArr[0]=m_vTenHangBrif;
		strArr[1]=m_vSoLuongKgN;
		strArr[2]=m_vDotNhap;
		strArr[3]=m_vSoLuongCayN;
		strArr[4]=m_vDonGia;
		strArr[5]=m_vThueNhap;
		strArr[6]=m_vComment;
// 		q.Format(_T("INSERT INTO %s VALUES('%s','%s','%s','%s','%s','%s','%s');"),
// 			CString(NHAP_TABLE),m_vTenHangBrif,m_vSoLuongKgN,m_vDotNhap,m_vSoLuongCayN,m_vDonGia,m_vThueNhap,m_vComment);
// 		if(ptr_cKmysql->query(q))
// 			AfxMessageBox(CString("da add thanh cong"));
// 		else
// 			AfxMessageBox(CString("ko add dc!"));
		break;
	case TAB_XUAT_ID:
		ptr_cKmysql->select_table(XUAT_TABLE);
		n_field=N_XUAT_FIELD;
		strArr[0]=m_vTenHangBrif;
		strArr[1]=m_vSoLuongKgX;
		strArr[2]=m_vDotNhap;
		strArr[3]=m_vSoLuongCayX;
		strArr[4]=m_vDonGia;
		strArr[5]=m_vThueNhap;
		strArr[6]=m_vNgayXuat;
		strArr[7]=m_vComment;
// 		q.Format(_T("INSERT INTO %s VALUES('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s');"),
// 			CString(XUAT_TABLE),m_vTenHangFull,m_vSoLuongKgX,m_vDotNhap,m_vSoLuongCayX,m_vDonGia,m_vThueNhap,m_vNgayXuat,m_vComment);
// 		if(ptr_cKmysql->query(q))
// 			AfxMessageBox(CString("da add thanh cong"));
// 		else
// 			AfxMessageBox(CString("ko add dc!"));
// 			break;
		break;
	case TAB_DOT_ID:
		n_field=4;
		ptr_cKmysql->select_table("dot");
		strArr[0]=m_vDotNhap;
		strArr[1]=m_vChungTu;
		strArr[2]=m_vNgayNhap;
		strArr[3]=m_vComment;
		break;
	case TAB_HANG_ID:
		ptr_cKmysql->select_table("hang");
		n_field=4;
		strArr[0]=m_vDotNhap;
		strArr[1]=m_vTenHangBrif;
		strArr[2]=m_vTenHangFull;
		strArr[3]=m_vComment;
		break;
	default:
		n_field=0;
		goto exit;
	}

	if(ptr_cKmysql->insert_row(strArr,n_field))
	{
		AfxMessageBox(_T("đã add xong"));
		add_row(strArr,n_field);
	}
	else
		AfxMessageBox(_T("không add được!"));
exit:
	;
}

//add row to list view
void CAllDlg::add_row(CString* contents, int n_col)
{
	for(int i=0;i<n_col;i++)
		add_item(total_row,i,contents[i]);
	total_row++;
}

void CAllDlg::OnBnClickedRadio1()
{
	// TODO: Add your control notification handler code here
	m_vThueNhap="0.05";
	m_cThueRadio010.SetCheck(BST_UNCHECKED);
}

void CAllDlg::OnBnClickedRadio2()
{
	// TODO: Add your control notification handler code here
	m_vThueNhap="0.1";
	m_cThueRadio005.SetCheck(BST_UNCHECKED);
}

// int CAllDlg::add_row(int table_id)
// {
// 	CString	q;
// 	switch(tab_id)
// 	{
// 	case TAB_NHAP_ID:
// 		q.Format(_T("INSERT INTO %s VALUES(%s, %s, %s, %s, %s, %s, %s);"),NHAP_TABLE,
// 			m_vTenHangFull,m_vSoLuongKgN,m_vDotNhap,m_vSoLuongCayN,m_vDonGia,m_vThueNhap,m_vComment);
// 		ptr_cKmysql->query(q);			
// 		break;
// 	}
// 	return 0;
// }

//int CAllDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
//{
//	if (CDialog::OnCreate(lpCreateStruct) == -1)
//		return -1;
//
//	// TODO:  Add your specialized creation code here
//	set_comment();
//	set_active_edit();
//
//	return 0;
//}
//set some edit text to read only
void CAllDlg::set_active_edit(void)
{
	CEdit** editL;
	CEdit*	dotEditL[]={&m_cTenHangFullEdit,&m_cTenHangBrif,&m_cSoLuongKgX,
		&m_cSoLuongKgN,&m_cSoLuongCayX,&m_cSoLuongCayN,
		&m_cDonGiaN,&m_cThanhTienCT};
	CEdit* hangEditL[]={&m_cChungTu,&m_cSoLuongKgX,
		&m_cSoLuongKgN,&m_cSoLuongCayX,&m_cSoLuongCayN,
		&m_cDonGiaN,&m_cThanhTienCT};
	int		n_edit=0;

	switch(tab_id)
	{
	case TAB_NHAP_ID:
		//change_mode(ADD_MODE);
		m_cChungTu.SetReadOnly(TRUE);
		m_cNgayNhap.ShowWindow(SW_HIDE);
		m_cTenHangFullEdit.SetReadOnly();
		m_cSoLuongCayX.SetReadOnly();
		m_cSoLuongKgX.SetReadOnly();
		m_cNgayXuat.ShowWindow(SW_HIDE);
		m_cThanhTienCT.SetReadOnly();
		n_edit=0;
		break;
	case TAB_XUAT_ID:
		//change_mode(ADD_MODE);
		m_cChungTu.SetReadOnly(TRUE);
		m_cNgayNhap.ShowWindow(SW_HIDE);
		m_cTenHangFullEdit.SetReadOnly();
		m_cSoLuongCayN.SetReadOnly();
		m_cSoLuongKgN.SetReadOnly();
		m_cThanhTienCT.SetReadOnly();
		n_edit=0;
		break;
	case TAB_DOT_ID:
		//change_mode(ADD_MODE);
		m_cNgayXuat.ShowWindow(SW_HIDE);
		editL=dotEditL;
		n_edit=8;
		break;
	case TAB_HANG_ID:
		//change_mode(ADD_MODE);
		m_cNgayNhap.ShowWindow(SW_HIDE);
		m_cNgayXuat.ShowWindow(SW_HIDE);
		editL=hangEditL;
		n_edit=7;
		break;
	}

	for(int i=0; i<n_edit; i++)
		editL[i]->SetReadOnly();
}

char* CAllDlg::CString2char(CString &str, char* buff)
{
	//INSERT INTO pet VALUES ('Puffball2'
	//,'Diane2','hamster2','f','1999-03-30',NULL
	//)
	/*int temp = sprintf(q,"INSERT INTO %s VALUES (%s",table,_row[0]);*/
	size_t newsizew = (str.GetLength() + 1)*2;
	size_t convertedCharsw = 0;
	wcstombs_s(&convertedCharsw, buff, newsizew, str, _TRUNCATE );

	return buff;
}

char* CAllDlg::CString2char(CString &str)
{
	char* buff;
	//INSERT INTO pet VALUES ('Puffball2'
	//,'Diane2','hamster2','f','1999-03-30',NULL
	//)
	/*int temp = sprintf(q,"INSERT INTO %s VALUES (%s",table,_row[0]);*/
	size_t newsizew = (str.GetLength() + 1)*2;
	buff = new	char[newsizew];
	size_t convertedCharsw = 0;
	wcstombs_s(&convertedCharsw, buff, newsizew, str, _TRUNCATE );

	return buff;
}

void CAllDlg::create_list_cols(void)
{
	CString dot_ch[]={_T("Đợt nhập"),_T("Mã số chứng từ"),_T("Ngày nhập"),_T("Ghi chú")};
	CString hang_ch[]={_T("Đợt nhập"),_T("Mã số hàng"),_T("Tên hàng"),_T("Ghi chú")};
	CString	nhap_ch[]={_T("Mã số hàng"),_T("Số lượng Kg"),_T("Đợt nhập"),_T("Số lượng Cây"),_T("Đơn giá"),_T("Thuế"),_T("Ghi chú")};
	CString xuat_ch[]={_T("Mã số hàng"),_T("Số lượng Kg"),_T("Đợt nhập"),_T("Số lượng Cây"),_T("Đơn giá"),_T("Thuế"),_T("Ngày xuất"),_T("Ghi chú")};
	CString	ton_ch[]={_T("Mã số hàng"),_T("Số lượng Kg"),_T("Số lượng Cây"),_T("Đơn giá"),_T("Thuế"),_T("Đợt nhập")};

	CString *col_heading=0;
	int		n_col=0;

	switch (tab_id)
	{
	case TAB_NHAP_ID:
// 		m_cAllList.InsertColumn(0,_T("Tên hàng"),LVCFMT_LEFT|LVCFMT_JUSTIFYMASK,LVCF_MINWIDTH);
// 		m_cAllList.InsertColumn(1,_T("Số lượng Kg"),LVCFMT_LEFT|LVCFMT_JUSTIFYMASK,LVCF_MINWIDTH);
// 		m_cAllList.InsertColumn(2,_T("Đợt nhập"),LVCFMT_LEFT|LVCFMT_JUSTIFYMASK,LVCF_MINWIDTH);
// 		m_cAllList.InsertColumn(3,_T("Số lượng Cây"),LVCFMT_LEFT|LVCFMT_JUSTIFYMASK,LVCF_MINWIDTH);
// 		m_cAllList.InsertColumn(4,_T("Đơn giá"),LVCFMT_LEFT|LVCFMT_JUSTIFYMASK,LVCF_MINWIDTH);
// 		m_cAllList.InsertColumn(5,_T("Thuế"),LVCFMT_LEFT|LVCFMT_JUSTIFYMASK,LVCF_MINWIDTH);
// 		m_cAllList.InsertColumn(6,_T("Ghi chú"),LVCFMT_LEFT|LVCFMT_JUSTIFYMASK,LVCF_MINWIDTH);
		col_heading=nhap_ch;
		n_col=7;
		break;
	case TAB_XUAT_ID:
// 		m_cAllList.InsertColumn(0,_T("Tên hàng"),LVCFMT_LEFT,LVCF_IDEALWIDTH);
// 		m_cAllList.InsertColumn(1,_T("Số lượng Kg"),LVCFMT_LEFT,LVCF_IDEALWIDTH);
// 		m_cAllList.InsertColumn(2,_T("Đợt nhập"),LVCFMT_LEFT,LVCF_IDEALWIDTH);
// 		m_cAllList.InsertColumn(3,_T("Số lượng Cây"),LVCFMT_LEFT,LVCF_IDEALWIDTH);
// 		m_cAllList.InsertColumn(4,_T("Đơn giá"),LVCFMT_LEFT,LVCF_IDEALWIDTH);
// 		m_cAllList.InsertColumn(5,_T("Thuế"),LVCFMT_LEFT,LVCF_IDEALWIDTH);
// 		m_cAllList.InsertColumn(6,_T("Ngày xuất"),LVCFMT_LEFT,LVCF_IDEALWIDTH);
// 		m_cAllList.InsertColumn(7,_T("Ghi chú"),LVCFMT_LEFT,LVCF_IDEALWIDTH);
		col_heading=xuat_ch;
		n_col=8;
		break;
	case TAB_DOT_ID:
		col_heading=dot_ch;
		n_col=4;
		break;
	case TAB_HANG_ID:
		col_heading=hang_ch;
		n_col=4;
		break;
	case TAB_TON_ID:
		col_heading=ton_ch;
		n_col=6;
		break;
	case TAB_ALL_ID:
		break;
	}

	for (int i=0; i<n_col; i++)
		m_cAllList.InsertColumn(i,col_heading[i],LVCFMT_LEFT,LVCF_MINWIDTH);
}

void CAllDlg::show_table(void)
{
	char* ton_q="select nhap.ms_hang,so_luong_kgN-IFNULL(so_luong_kgX,0) as soKG,\n"
		"so_luong_cayN-IFNULL(so_luong_cayX,0) as soCAY,\n"
		"don_giaN,thueN,nhap.dot_nhap\n"
		"from nhap left join xuat on nhap.ms_hang = xuat.ms_hang and nhap.dot_nhap =  xuat.dot_nhap\n"
		"order by nhap.dot_nhap;";

	switch(tab_id)
	{
	case TAB_NHAP_ID:
		if (ptr_cKmysql->query(CString("select * from nhap;")))
		{
			ptr_cKmysql->store_result();
			show_on_list();
		}
		break;
	case TAB_XUAT_ID:
		if (ptr_cKmysql->query(CString("select * from xuat;")))
		{
			ptr_cKmysql->store_result();
			show_on_list();
		}
		break;
	case TAB_DOT_ID:
		if(ptr_cKmysql->query(CString("select * from dot")))
		{
			ptr_cKmysql->store_result();
			show_on_list();
		}
		break;
	case TAB_HANG_ID:
		if (ptr_cKmysql->query(CString("select * from hang")))
		{
			ptr_cKmysql->store_result();
			show_on_list();
		}
		break;
	case TAB_TON_ID:
		if (ptr_cKmysql->query(ton_q))
		{
			ptr_cKmysql->store_result();
			show_on_list();
		}
		break;
	
	}

}

// show result on list view
void CAllDlg::show_on_list(void)
{
	MYSQL_ROW row;
	unsigned int num_fields;
	unsigned int i=0,j=0;
	clean_list();

	num_fields = mysql_num_fields(ptr_cKmysql->get_res());
	while ((row = mysql_fetch_row(ptr_cKmysql->get_res())))
	{
// 		unsigned long *lengths;
// 		lengths = mysql_fetch_lengths(ptr_cKmysql->get_res());
		for(i = 0; i < num_fields; i++)
		{			
			add_item(j,i,CString(row[i]));
		}
		j++;
	}
	total_row=j;
}

void CAllDlg::add_item(int row, int col, CString content)
{
	LVITEM Item;

	Item.state=LVIS_SELECTED | LVIS_FOCUSED;
/*LVNI_FOCUSED   | LVNI_SELECTED;*/
	Item.mask = LVIF_TEXT;
	Item.iItem = row;	
	Item.iSubItem = col;	
	Item.pszText = (LPTSTR)(LPCTSTR)(content);

	if(col!=0)
		m_cAllList.SetItem(&Item);
	else
		m_cAllList.InsertItem(&Item);
}

//void CAllDlg::OnLvnItemchangedAllList(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
//	// TODO: Add your control notification handler code here
//	
//	*pResult = 0;
//}

//void CAllDlg::OnNMRDblclkAllList(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<NMITEMACTIVATE>(pNMHDR);
//	// TODO: Add your control notification handler code here
//	/*show_table();*/
//	*pResult = 0;
//}

//void CAllDlg::OnNMDblclkAllList(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<NMITEMACTIVATE>(pNMHDR);
//	// TODO: Add your control notification handler code here
//	show_table();
//	*pResult = 0;
//}

//void CAllDlg::OnNMClickAllList(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<NMITEMACTIVATE>(pNMHDR);
//	// TODO: Add your control notification handler code here
//	*pResult = 0;
//}

//void CAllDlg::OnNMClickAllList(NMHDR *pNMHDR, LRESULT *pResult)
//{
///*	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<NMITEMACTIVATE>(pNMHDR);*/
//
//	// TODO: Add your control notification handler code here
//	/*show_table();*/
//	
//	*pResult = 0;
//}


void CAllDlg::OnNMDblclkAllList(NMHDR *pNMHDR, LRESULT *pResult)
{
	/*LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<NMITEMACTIVATE>(pNMHDR);*/
	// TODO: Add your control notification handler code here
	show_table();
	*pResult = 0;
}

//void CAllDlg::OnHdnItemclickAllList(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	/*LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);*/
//	// TODO: Add your control notification handler code here
//
//	*pResult = 0;
//}

//void CAllDlg::OnLvnItemchangedAllList(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	/*LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);*/
//	// TODO: Add your control notification handler code here
// 	CString*	EditL[20];
// 	int		n_edit=0;
// 	int		nCurSel = m_cAllList.GetNextItem(-1 , LVNI_FOCUSED   | LVNI_SELECTED);
// 
// 	if(nCurSel==-1)
// 		exit;
// 
// 	switch (tab_id)
// 	{
// 	case TAB_NHAP_ID:
// 		n_edit=N_NHAP_FIELD;
// 		/*EditL=new	CString*[N_NHAP_FIELD];*/
// 		EditL[0]=&m_vTenHangBrif;
// 		EditL[1]=&m_vSoLuongKgN;
// 		EditL[2]=&m_vDotNhap;
// 		EditL[3]=&m_vSoLuongCayN;
// 		EditL[4]=&m_vDonGia;
// 		EditL[5]=&m_vThueNhap;
// 		EditL[6]=&m_vComment;
// 		break;
// 	case TAB_XUAT_ID:
// 		n_edit=N_XUAT_FIELD;
// 		/*EditL=new	CString*[N_XUAT_FIELD];*/
// 		EditL[0]=&m_vTenHangBrif;
// 		EditL[1]=&m_vSoLuongKgX;
// 		EditL[2]=&m_vDotNhap;
// 		EditL[3]=&m_vSoLuongCayX;
// 		EditL[4]=&m_vDonGia;
// 		EditL[5]=&m_vThueNhap;
// 		EditL[6]=&m_vNgayXuat;
// 		EditL[7]=&m_vComment;
// 		break;
// 	case TAB_DOT_ID:
// 		n_edit=N_DOT_FIELD;
// 		/*EditL=new CString*[N_DOT_FIELD];*/
// 		EditL[0]=&m_vDotNhap;
// 		EditL[1]=&m_vChungTu;
// 		EditL[2]=&m_vNgayNhap;
// 		EditL[3]=&m_vComment;
// 		break;
// 	case TAB_HANG_ID:
// 		n_edit=N_HANG_FIELD;
// // 		EditL=new CString*[N_HANG_FIELD];
// // 		EditL=new CString*[N_DOT_FIELD];
// 		EditL[0]=&m_vDotNhap;
// 		EditL[1]=&m_vTenHangBrif;
// 		EditL[2]=&m_vTenHangFull;
// 		EditL[3]=&m_vComment;
// 		break;
// 	default:
// 		goto exit;
// 	}
// 
// 	for(int i=0; i<n_edit;i++)
// 	{
// 		CString text = m_cAllList.GetItemText(nCurSel,i);
// 		*EditL[i]=text;
// 	}
// 
// 	UpdateData(FALSE);
// 
// 	if(m_vThueNhap==CString("0.05"))
// 	{
// 		m_cThueRadio005.SetCheck(BST_CHECKED);
// 		m_cThueRadio010.SetCheck(BST_UNCHECKED);
// 	}
// 	else if (m_vThueNhap==CString("0.1"))
// 	{
// 		m_cThueRadio005.SetCheck(BST_UNCHECKED);
// 		m_cThueRadio010.SetCheck(BST_CHECKED);
// 	}
// 	else
// 	{
// 		m_cThueRadio010.SetCheck(BST_UNCHECKED);
// 		m_cThueRadio005.SetCheck(BST_UNCHECKED);
// 	}
// exit:
//	*pResult = 0;
//}

void CAllDlg::set_init_var(void)
{
	m_vThueNhap=CString("0.1");
	m_cThueRadio010.SetCheck(BST_CHECKED);
}

void CAllDlg::OnBnClickedEditButton()
{
	// TODO: Add your control notification handler code here
	CString strArr[20];
	int		n_field,nCurSel;
	CString	q;
	UpdateData(TRUE);

	switch(tab_id)
	{
	case TAB_NHAP_ID:
		ptr_cKmysql->select_table(NHAP_TABLE);
		q.Empty();
		q.Format(CString("DELETE FROM nhap WHERE dot_nhap='%s' AND ms_hang='%s';"),m_vDotNhap,m_vTenHangBrif);
		if(!ptr_cKmysql->query(q)) goto error_del;
		n_field=N_NHAP_FIELD;
		strArr[0]=m_vTenHangBrif;
		strArr[1]=m_vSoLuongKgN;
		strArr[2]=m_vDotNhap;
		strArr[3]=m_vSoLuongCayN;
		strArr[4]=m_vDonGia;
		strArr[5]=m_vThueNhap;
		strArr[6]=m_vComment;
		break;
	case TAB_XUAT_ID:
		ptr_cKmysql->select_table(XUAT_TABLE);
		q.Empty();
		q.Format(CString("DELETE FROM xuat WHERE dot_nhap='%s' AND ms_hang='%s';"),m_vDotNhap,m_vTenHangBrif);
		if(!ptr_cKmysql->query(q)) goto error_del;
		n_field=N_XUAT_FIELD;
		strArr[0]=m_vTenHangBrif;
		strArr[1]=m_vSoLuongKgX;
		strArr[2]=m_vDotNhap;
		strArr[3]=m_vSoLuongCayX;
		strArr[4]=m_vDonGia;
		strArr[5]=m_vThueNhap;
		strArr[6]=m_vNgayXuat;
		strArr[7]=m_vComment;
		break;
	case TAB_DOT_ID:
		n_field=4;
		ptr_cKmysql->select_table("dot");
		q.Empty();
		q.Format(CString("DELETE FROM dot WHERE dot_nhap='%s';"),m_vDotNhap);
		if(!ptr_cKmysql->query(q)) goto error_del;
		strArr[0]=m_vDotNhap;
		strArr[1]=m_vChungTu;
		strArr[2]=m_vNgayNhap;
		strArr[3]=m_vComment;
		break;
	case TAB_HANG_ID:
// 		ptr_cKmysql->select_table("hang");
// 		n_field=4;
// 		strArr[0]=m_vDotNhap;
// 		strArr[1]=m_vTenHangBrif;
// 		strArr[2]=m_vTenHangFull;
// 		strArr[3]=m_vComment;
		goto exit;
		break;
	default:
		n_field=0;
		goto exit;
	}

	if(ptr_cKmysql->insert_row(strArr,n_field))
	{
		AfxMessageBox(_T("đã sửa xong!"));
		nCurSel = m_cAllList.GetNextItem(-1 , LVNI_FOCUSED   | LVNI_SELECTED);
		edit_row(nCurSel,strArr,n_field);
	}
	else
	{
error_del:
		AfxMessageBox(_T("ko sửa được!"));
	}
exit:
	;
}

void CAllDlg::edit_row(int row, CString* contents, int n_col)
{
	for(int i=0;i<n_col;i++)
		edit_item(row,i,contents[i]);
}
void CAllDlg::edit_item(int row, int col, CString content)
{
	LVITEM Item;

	Item.state=LVIS_SELECTED | LVIS_FOCUSED;
	/*LVNI_FOCUSED   | LVNI_SELECTED;*/
	Item.mask = LVIF_TEXT;
	Item.iItem = row;	
	Item.iSubItem = col;	
	Item.pszText = (LPTSTR)(LPCTSTR)(content);

	m_cAllList.SetItem(&Item);
}
//void CAllDlg::OnFileExit()
//{
//	// TODO: Add your command handler code here
//	CWnd * parent = GetParentOwner();
//	parent->SendMessage(WM_CLOSE);
//}

//void CAllDlg::OnOptionAddmode()
//{
//	// TODO: Add your command handler code here
//	HMENU	hMenu;
//	CMenu*	cMenu=GetMenu();
//	//cMenu->CheckMenuItem(ID_OPTION_ADDMODE,MF_CHECKED);
//	cMenu->EnableMenuItem(ID_OPTION_EDITMODE,MF_DISABLED);
//
//
//	//cMenu->CheckMenuItem(ID_OPTION_EDITMODE,MF_UNCHECKED);
//	cMenu->EnableMenuItem(ID_OPTION_EDITMODE,MF_ENABLED);
//
//	//cMenu->CheckMenuItem(ID_OPTION_REMOVEMODE,MF_UNCHECKED);
//	cMenu->EnableMenuItem(ID_OPTION_REMOVEMODE,MF_ENABLED);
//}

//void CAllDlg::OnOptionEditmode()
//{
//	// TODO: Add your command handler code here
//	CMenu*	cMenu=GetMenu();
//	cMenu->EnableMenuItem(ID_OPTION_EDITMODE,MF_DISABLED);
//
//
//	//cMenu->CheckMenuItem(ID_OPTION_EDITMODE,MF_UNCHECKED);
//	cMenu->EnableMenuItem(ID_OPTION_EDITMODE,MF_ENABLED);
//
//	//cMenu->CheckMenuItem(ID_OPTION_REMOVEMODE,MF_UNCHECKED);
//	cMenu->EnableMenuItem(ID_OPTION_REMOVEMODE,MF_ENABLED);
//}

//void CAllDlg::OnOptionRemovemode()
//{
//	// TODO: Add your command handler code here
//	CMenu*	cMenu=GetMenu();
//	cMenu->EnableMenuItem(ID_OPTION_EDITMODE,MF_DISABLED);
//
//
//	//cMenu->CheckMenuItem(ID_OPTION_EDITMODE,MF_UNCHECKED);
//	cMenu->EnableMenuItem(ID_OPTION_EDITMODE,MF_ENABLED);
//
//	//cMenu->CheckMenuItem(ID_OPTION_REMOVEMODE,MF_UNCHECKED);
//	cMenu->EnableMenuItem(ID_OPTION_REMOVEMODE,MF_ENABLED);
//}

void CAllDlg::change_mode(int mode)
{
	switch (mode)
	{
	case ADD_MODE:
		m_cAddBtn.EnableWindow(TRUE);
		m_cEditBtn.EnableWindow(FALSE);
		m_cRemoveBtn.EnableWindow(FALSE);
		break;
	case EDIT_MODE:
		m_cAddBtn.EnableWindow(FALSE);
		m_cEditBtn.EnableWindow(TRUE);
		m_cRemoveBtn.EnableWindow(FALSE);
		break;
	case REMOVE_MODE:
		m_cAddBtn.EnableWindow(FALSE);
		m_cEditBtn.EnableWindow(FALSE);
		m_cRemoveBtn.EnableWindow(TRUE);
		break;
	}
}

//void CAllDlg::OnOptionAddmode()
//{
//	// TODO: Add your command handler code here
//	;
//}

void CAllDlg::OnBnClickedRemoveButton()
{
	// TODO: Add your control notification handler code here
	CString strArr[20];
	int		n_field,nCurSel;
	CString	q;
	UpdateData(TRUE);

	switch(tab_id)
	{
	case TAB_NHAP_ID:
		ptr_cKmysql->select_table(NHAP_TABLE);
		q.Empty();
		q.Format(CString("DELETE FROM nhap WHERE dot_nhap='%s' AND ms_hang='%s';"),m_vDotNhap,m_vTenHangBrif);
		//if(!ptr_cKmysql->query(q)) goto error_del;
		break;
	case TAB_XUAT_ID:
		ptr_cKmysql->select_table(XUAT_TABLE);
		q.Empty();
		q.Format(CString("DELETE FROM xuat WHERE dot_nhap='%s' AND ms_hang='%s';"),m_vDotNhap,m_vTenHangBrif);
		//if(!ptr_cKmysql->query(q)) goto error_del;
		break;
	case TAB_DOT_ID:
		n_field=4;
		ptr_cKmysql->select_table("dot");
		q.Empty();
		q.Format(CString("DELETE FROM dot WHERE dot_nhap='%s';"),m_vDotNhap);
		//if(!ptr_cKmysql->query(q)) goto error_del;
		break;
	case TAB_HANG_ID:
		// 		ptr_cKmysql->select_table("hang");
		// 		n_field=4;
		// 		strArr[0]=m_vDotNhap;
		// 		strArr[1]=m_vTenHangBrif;
		// 		strArr[2]=m_vTenHangFull;
		// 		strArr[3]=m_vComment;
		goto exit;
		break;
	default:
		n_field=0;
		goto exit;
	}

	if(ptr_cKmysql->query(q))
	{
		AfxMessageBox(_T("đã xóa xong!"));
		nCurSel = m_cAllList.GetNextItem(-1 , LVNI_FOCUSED   | LVNI_SELECTED);
		
		del_row(nCurSel);
	}
	else
	{
error_del:
		AfxMessageBox(_T("ko xóa được!"));
	}
exit:
	;
}

void CAllDlg::del_row(int row)
{
	m_cAllList.DeleteItem(row);
	total_row--;
}

void CAllDlg::clean_list(void)
{
	m_cAllList.DeleteAllItems();
	total_row=0;
}

//void CAllDlg::OnNMSetfocusAllList(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	// TODO: Add your control notification handler code here
//	*pResult = 0;
//}

//void CAllDlg::OnHdnItemclickAllList(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	/*LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);*/
//	// TODO: Add your control notification handler code here
//	*pResult = 0;
//}

void CAllDlg::OnNMRClickAllList(NMHDR *pNMHDR, LRESULT *pResult)
{
	/*LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<NMITEMACTIVATE>(pNMHDR);*/
	// TODO: Add your control notification handler code here

	CString*	EditL[20];
	int		n_edit=0;
	int		nCurSel = m_cAllList.GetNextItem(-1 , LVNI_FOCUSED   | LVNI_SELECTED);

	if(nCurSel==-1)
		exit;

	switch (tab_id)
	{
	case TAB_NHAP_ID:
		n_edit=N_NHAP_FIELD;
		/*EditL=new	CString*[N_NHAP_FIELD];*/
		EditL[0]=&m_vTenHangBrif;
		EditL[1]=&m_vSoLuongKgN;
		EditL[2]=&m_vDotNhap;
		EditL[3]=&m_vSoLuongCayN;
		EditL[4]=&m_vDonGia;
		EditL[5]=&m_vThueNhap;
		EditL[6]=&m_vComment;
		break;
	case TAB_XUAT_ID:
		n_edit=N_XUAT_FIELD;
		/*EditL=new	CString*[N_XUAT_FIELD];*/
		EditL[0]=&m_vTenHangBrif;
		EditL[1]=&m_vSoLuongKgX;
		EditL[2]=&m_vDotNhap;
		EditL[3]=&m_vSoLuongCayX;
		EditL[4]=&m_vDonGia;
		EditL[5]=&m_vThueNhap;
		EditL[6]=&m_vNgayXuat;
		EditL[7]=&m_vComment;
		break;
	case TAB_DOT_ID:
		n_edit=N_DOT_FIELD;
		/*EditL=new CString*[N_DOT_FIELD];*/
		EditL[0]=&m_vDotNhap;
		EditL[1]=&m_vChungTu;
		EditL[2]=&m_vNgayNhap;
		EditL[3]=&m_vComment;
		break;
	case TAB_HANG_ID:
		n_edit=N_HANG_FIELD;
// 		EditL=new CString*[N_HANG_FIELD];
// 		EditL=new CString*[N_DOT_FIELD];
		EditL[0]=&m_vDotNhap;
		EditL[1]=&m_vTenHangBrif;
		EditL[2]=&m_vTenHangFull;
		EditL[3]=&m_vComment;
		break;
	default:
		goto exit;
	}

	for(int i=0; i<n_edit;i++)
	{
		CString text = m_cAllList.GetItemText(nCurSel,i);
		*EditL[i]=text;
	}

	UpdateData(FALSE);

	if(m_vThueNhap==CString("0.05"))
	{
		m_cThueRadio005.SetCheck(BST_CHECKED);
		m_cThueRadio010.SetCheck(BST_UNCHECKED);
	}
	else if (m_vThueNhap==CString("0.1"))
	{
		m_cThueRadio005.SetCheck(BST_UNCHECKED);
		m_cThueRadio010.SetCheck(BST_CHECKED);
	}
	else
	{
		m_cThueRadio010.SetCheck(BST_UNCHECKED);
		m_cThueRadio005.SetCheck(BST_UNCHECKED);
	}
exit:
	*pResult = 0;
}
