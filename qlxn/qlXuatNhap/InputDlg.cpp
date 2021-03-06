#include "stdafx.h"
#include "qlXuatNhap.h"
#include "InputDlg.h"


// InputDlg dialog

IMPLEMENT_DYNAMIC(InputDlg, CDialog)

InputDlg::InputDlg(CWnd* pParent /*=NULL*/)
	: CDialog(InputDlg::IDD, pParent)
	, m_pKmySql(NULL)
	, tabID(0)
	, m_flagAC(0)
	, m_addNField(0)
	, m_addTable(_T(""))
{
	lcwnd=(CWnd**)malloc(N_INPUTDLG_CWND * sizeof(CWnd*) + 1);
}
// InputDlg::InputDlg(KMySQL* pKmysql)
// {
// 	m_pKmySql=pKmysql;
// }
InputDlg::~InputDlg()
{
	free(lcwnd);
}

void InputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ADD_BTN, m_addBtn);
	DDX_Control(pDX, IDC_STATIC_DOTNHAP, m_sDotNhap);
	DDX_Control(pDX, IDC_STATIC_MSCT, m_sMSCT);
	DDX_Control(pDX, IDC_STATIC_NGAY, m_sNGAY);
	DDX_Control(pDX, IDC_STATIC_SLKG, m_sSLKG);
	DDX_Control(pDX, IDC_STATIC_CAY, m_sSLCAY);
	DDX_Control(pDX, IDC_STATIC_DG, m_sDonGia);
	DDX_Control(pDX, IDC_STATIC_TTCT, m_sTTCT);
	DDX_Control(pDX, IDC_STATIC_THUE, m_sThue);
	DDX_Control(pDX, IDC_STATIC_MSH, m_sMSH);
	DDX_Control(pDX, IDC_STATIC_HANG, m_sHang);
	DDX_Control(pDX, IDC_STATIC_GHICHU, m_sGhiChu);
	DDX_Control(pDX, IDC_DOTNHAP_EDT, m_dotEdt);
	DDX_Control(pDX, IDC_MSCT_EDIT, m_msctEdt);
	DDX_Control(pDX, IDC_NGAY, m_ngay);
	DDX_Control(pDX, IDC_SLKG_EDIT, m_slkgEdt);
	DDX_Control(pDX, IDC_SLCAY_EDT, m_slcayEdt);
	DDX_Control(pDX, IDC_DG_EDT, m_donGia);
	DDX_Control(pDX, IDC_TTCT_EDT, m_TTCT);
	DDX_Control(pDX, IDC_THUE_EDT, m_thueEdt);
	DDX_Control(pDX, IDC_MSH_EDT, m_mshEdt);
	DDX_Control(pDX, IDC_TENHANG_EDT, m_tenHang);
	DDX_Control(pDX, IDC_GHICHU_EDT, m_ghiChu);
	DDX_Control(pDX, IDC_EDIT_BTN, m_editBtn);
	DDX_Control(pDX, IDC_REMOVE_BTN, m_removeBtn);
	DDX_Control(pDX, IDC_RES_LIST, m_resList);
}


BEGIN_MESSAGE_MAP(InputDlg, CDialog)
	ON_WM_SIZE()
	ON_EN_CHANGE(IDC_MSCT_EDIT, &InputDlg::OnEnChangeMsctEdit)
	ON_BN_CLICKED(IDC_ADD_BTN, &InputDlg::OnBnClickedAddBtn)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_RES_LIST, &InputDlg::OnLvnItemchangedResList)
	ON_NOTIFY(NM_DBLCLK, IDC_RES_LIST, &InputDlg::OnNMDblclkResList)
	ON_BN_CLICKED(IDC_REMOVE_BTN, &InputDlg::OnBnClickedRemoveBtn)
	ON_EN_CHANGE(IDC_DG_EDT, &InputDlg::OnEnChangeDgEdt)
	ON_BN_CLICKED(IDC_EDIT_BTN, &InputDlg::OnBnClickedEditBtn)
END_MESSAGE_MAP()


// InputDlg message handlers


BOOL InputDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	 CWnd * temp[]={
		(CWnd*)&m_sDotNhap,(CWnd*)&m_dotEdt,(CWnd*)&m_sMSCT,(CWnd*)&m_msctEdt,
		(CWnd*)&m_sMSH,(CWnd*)&m_mshEdt,
		(CWnd*)&m_sNGAY,(CWnd*)&m_ngay,
		(CWnd*)&m_sSLKG,(CWnd*)&m_slkgEdt,(CWnd*)&m_sSLCAY,(CWnd*)&m_slcayEdt,
		(CWnd*)&m_sDonGia,(CWnd*)&m_donGia,(CWnd*)&m_sTTCT,(CWnd*)&m_TTCT,
		(CWnd*)&m_sThue,(CWnd*)&m_thueEdt,
		(CWnd*)&m_sHang,(CWnd*)&m_tenHang,(CWnd*)&m_sGhiChu,(CWnd*)&m_ghiChu,
		(CWnd*)&m_resList,(CWnd*)&m_addBtn,(CWnd*)&m_editBtn,(CWnd*)&m_removeBtn};
	//set lcwnd value
	
	memcpy(lcwnd, temp, N_INPUTDLG_CWND * sizeof (CWnd*) );

	m_ngay.SetFormat(_T("yyyy'-'MM'-'dd"));
	//an cac ctrl ko dung
	hideCtrl();
	//set total row va tao col cho list view
	total_row=0;
	create_list_cols();
	m_resList.SetExtendedStyle(m_resList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	//set m_flagAC and m_addNField value
	switch(tabID)
	{
	case TAB_NHAP_ID:
		m_addTable=_T(NHAP_TABLE);
		m_addNField=N_NHAP_FIELD;
		m_flagAC=0x1115110;//0100 01 00 0101 0100 01 0001 0000
		break;
	case TAB_XUAT_ID:
		m_addTable=_T(XUAT_TABLE);
		m_addNField=N_XUAT_FIELD;
		m_flagAC=0x1155110;//0100 01 01 0101 0100 01 0001 0000
		break;
	case TAB_DOT_NHAP_ID:
		m_addNField=4;
		m_addTable=_T("dot");
		m_flagAC=0x01440010;//0101 00 01 0000 0000 00 0001 0000
		break;
	case TAB_HANG_ID:
		m_addTable=_T("hang");
		m_addNField=3;
		m_flagAC=0x0100050;//0000 01 00 0000 0000 00 0101 0000
		break;
	default:
		m_addNField=0;		
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void InputDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (::IsWindow(m_resList.GetSafeHwnd()))
	{
		resize(cx,cy);
	}
}

int InputDlg::resize(int cx, int cy)
{
	int x,y,nx,ny;
	cx=KKUNSCALE(cx);
	cy=KKUNSCALE(cy);
	//resize list and btns
	x=KK_L_SPACE;y=32;
	nx=cx-x-KK_R_SPACE;ny=cy-y-KK_NY_BTN-KK_B_SPACE;
	m_resList.MoveWindow(KKSCALE(x),KKSCALE(y),KKSCALE(nx),KKSCALE(ny));
	//resize btns
	x=cx-KK_R_SPACE-KK_NX_BTN;y=cy-KK_B_SPACE-KK_NY_BTN;
	ny=KK_NY_BTN;nx=KK_NX_BTN;
	m_removeBtn.MoveWindow(KKSCALE(x),KKSCALE(y),KKSCALE(nx),KKSCALE(ny));
	x-=KK_X_SPACE+KK_NX_BTN;
	m_editBtn.MoveWindow(KKSCALE(x),KKSCALE(y),KKSCALE(nx),KKSCALE(ny));
	x-=KK_X_SPACE+KK_NX_BTN;
	m_addBtn.MoveWindow(KKSCALE(x),KKSCALE(y),KKSCALE(nx),KKSCALE(ny));

	switch(tabID)
	{
	case TAB_DU_AN_ID:
	case TAB_HANG_ID:
		x=KK_L_SPACE;y=KK_T_SPACE;nx=45;ny=KK_NY_STATIC;
		m_sMSH.MoveWindow(KKSCALE(x),KKSCALE(y),KKSCALE(nx),KKSCALE(ny));
		y+=KK_Y_SPACE+KK_NY_STATIC;ny=KK_NY_EDT;
		m_mshEdt.MoveWindow(KKSCALE(x),KKSCALE(y),KKSCALE(nx),KKSCALE(ny));
		x+=nx+KK_X_SPACE;y=KK_T_SPACE;nx=100;ny=KK_NY_STATIC;
		m_sHang.MoveWindow(KKSCALE(x),KKSCALE(y),KKSCALE(nx),KKSCALE(ny));
		y+=KK_Y_SPACE+KK_NY_STATIC;ny=KK_NY_EDT;
		m_tenHang.MoveWindow(KKSCALE(x),KKSCALE(y),KKSCALE(nx),KKSCALE(ny));
	case TAB_DOT_XUAT_ID:
	case TAB_DOT_NHAP_ID:
		//resize ghi chu
		x=162;y=KK_T_SPACE;
		nx=24,ny=KK_NY_STATIC;
		m_sGhiChu.MoveWindow(KKSCALE(x),KKSCALE(y),KKSCALE(nx),KKSCALE(ny));
		y+=KK_Y_SPACE+KK_NY_STATIC;
		nx=cx-x-KK_L_SPACE;ny=KK_NY_EDT;
		m_ghiChu.MoveWindow(KKSCALE(x),KKSCALE(y),KKSCALE(nx),KKSCALE(ny));
		break;
	case TAB_NHAP_ID:
	case TAB_XUAT_ID:
		//resize ghi chu
		x=345;y=KK_T_SPACE;
		nx=24,ny=KK_NY_STATIC;
		m_sGhiChu.MoveWindow(KKSCALE(x),KKSCALE(y),KKSCALE(nx),KKSCALE(ny));
		y+=KK_Y_SPACE+KK_NY_STATIC;
		nx=cx-x-KK_L_SPACE;ny=KK_NY_EDT;
		m_ghiChu.MoveWindow(KKSCALE(x),KKSCALE(y),KKSCALE(nx),KKSCALE(ny));

		break;
	case TAB_TON_ID:
		x=KK_L_SPACE;y=KK_T_SPACE+KK_Y_SPACE+KK_NY_STATIC;
		nx=cx-x-KK_R_SPACE;ny=KK_NY_EDT;
		m_ghiChu.MoveWindow(KKSCALE(x),KKSCALE(y),KKSCALE(nx),KKSCALE(ny));
		break;
	}
	return 0;
}

int InputDlg::resize(void)
{
	CRect wrect;
	GetWindowRect(&wrect);
	resize(wrect.Width(),wrect.Height());
	return 0;
}

void InputDlg::OnEnChangeMsctEdit()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}
#define COMMENT_RANGE 22
//an cac static, control ko can thiet ung voi moi tab
//set caption cho cac static
int InputDlg::hideCtrl(void)
{
	
	TCHAR* comment[]={
		_T("Đợt nhập"),_T("MS chứng từ"),
		_T("MS hàng"),_T("Ngày"),_T("Số lượng Kg"),
		_T("Số lượng cây"),_T("Đơn giá"),_T("Thành tiền ct"),_T("Thuế"),
		_T("Tên hàng"),_T("Ghi chú")};

	int	n=N_INPUTDLG_CWND, //so cwnd cua input dlg
		i;
	UINT flag;			//0000 00 00 1111 1111 11 1111 0000 (1 la hide)
						//moi bit tuong ung voi 1 cwnd trong list cwnd
	UINT mask;			
	switch(tabID)
	{
	case TAB_DOT_NHAP_ID:		
		flag=0x033FFC0;	//0000 11 00 1111 1111 11 1100 0000
		break;
	case TAB_HANG_ID:
		flag=0x3CFFF00;	//1111 00 11 1111 1111 11 0000 0000
		break;
	case TAB_NHAP_ID:
		flag=0x0CC00C0;	//0011 00 11 0000 0000 00 1100 0000
		break;
	case TAB_XUAT_ID:
		flag=0x0C000C0;	//0011 00 00 0000 0000 00 1100 0000
		break;
	case TAB_TON_ID:
		flag=0x03FFFFE7;//1111 11 11 1111 1111 11 1110 0111
		break;
	default:
		flag=0;
	}
	
	//hide va set comment cho cac cwnd trong dlg
	mask=1;
	for(i=n-1;i>=0;i--)
	{				
		if(flag&mask) lcwnd[i]->ShowWindow(SW_HIDE);
		else
			if(i<=COMMENT_RANGE && i % 2 == 0) lcwnd[i]->SetWindowText(comment[i/2]);

		mask<<=1;
	}

	return 0;
}
int InputDlg::create(CTabCtrl* pTab,KMySQL* pKmysql,
					 int tab_id,CRect& rect)
{//phai create roi moi movewindow dc
	m_pKmySql=pKmysql;
	tabID=tab_id;
	Create(IDD,pTab);
	MoveWindow(rect);
	return 0;
}
void InputDlg::OnBnClickedAddBtn()
{
	// TODO: Add your control notification handler code here
	add_record();
}
//them 1 item vao list view
void InputDlg::add_item(int row, int col, CString content)
{
	LVITEM Item;

	Item.state=LVIS_SELECTED | LVIS_FOCUSED;
	/*LVNI_FOCUSED   | LVNI_SELECTED;*/
	Item.mask = LVIF_TEXT;
	Item.iItem = row;	
	Item.iSubItem = col;	
	Item.pszText = (LPTSTR)(LPCTSTR)(content);

	if(col!=0)
		m_resList.SetItem(&Item);
	else
		m_resList.InsertItem(&Item);
}
//them mot dong (nhieu item) vao list view
void InputDlg::add_row(CString* contents, int n_col)
{
	for(int i=0;i<n_col;i++)
		add_item(total_row,i,contents[i]);
	total_row++;
}
//tao cac cot cho list view ung voi moi tab
void InputDlg::create_list_cols(void)
{
	CString dot_ch[]={_T("Đợt nhập"),_T("Mã số chứng từ"),_T("Ngày nhập"),_T("Ghi chú")};
	CString hang_ch[]={_T("Mã số hàng"),_T("Tên hàng"),_T("Ghi chú")};
	CString	nhap_ch[]={_T("Đợt nhập"),_T("Mã số hàng"),_T("Số lượng Kg"),_T("Số lượng Cây"),_T("Đơn giá"),_T("Thuế"),_T("Ghi chú")};
	CString xuat_ch[]={_T("Đợt nhập"),_T("Mã số hàng"),_T("Số lượng Kg"),_T("Số lượng Cây"),_T("Đơn giá"),_T("Thuế"),_T("Ngày xuất"),_T("Ghi chú")};
	CString	ton_ch[]={_T("Đợt nhập"),_T("Mã số hàng"),_T("Số lượng Kg"),_T("Số lượng Cây"),_T("Đơn giá"),_T("Thuế")};

	CString *col_heading=0;
	int		n_col=0;

	switch (tabID)
	{
	case TAB_NHAP_ID:
		col_heading=nhap_ch;
		n_col=7;
		break;
	case TAB_XUAT_ID:
		col_heading=xuat_ch;
		n_col=8;
		break;
	case TAB_DOT_NHAP_ID:
		col_heading=dot_ch;
		n_col=4;
		break;
	case TAB_HANG_ID:
		col_heading=hang_ch;
		n_col=3;
		break;
	case TAB_TON_ID:
		col_heading=ton_ch;
		n_col=6;
		break;
	case TAB_ALL_ID:
		break;
	}

	for (int i=0; i<n_col; i++)
		m_resList.InsertColumn(i,col_heading[i],LVCFMT_LEFT,LVCF_MINWIDTH);
}
//show result of last query on list view
void InputDlg::show_on_list(void)
{
	MYSQL_ROW row;
	unsigned int num_fields;
	unsigned int i=0,j=0;
	clean_list();

	num_fields = mysql_num_fields(m_pKmySql->get_res());
	while ((row = mysql_fetch_row(m_pKmySql->get_res())))
	{
		for(i = 0; i < num_fields; i++)
		{			
			add_item(j,i,CString(row[i]));
		}
		j++;
	}
	total_row=j;
}

void InputDlg::clean_list(void)
{
	m_resList.DeleteAllItems();
	total_row=0;
}

//show noi dung cua table co ten ung voi tab len list view
void InputDlg::show_table(void)
{
// 	char* ton_q="select nhap.dot_nhap,nhap.ms_hang,so_luong_kgN-IFNULL(so_luong_kgX,0) as soKG,\n"
// 		"so_luong_cayN-IFNULL(so_luong_cayX,0) as soCAY,\n"
// 		"don_giaN,thueN\n"
// 		"from nhap left join xuat on nhap.ms_hang = xuat.ms_hang and nhap.dot_nhap =  xuat.dot_nhap\n"
// 		"order by nhap.dot_nhap;";
	char *ton_q="select	t1.dn,t1.msh,slkn-IFNULL(slkx,0) as slKG,slcn-IFNULL(slcx,0) as slc,dgn,tn\n"
		"from\n"
		"(select dot_nhap as dn,ms_hang as msh,so_luong_kgN as slkn,so_luong_cayN as slcn, don_giaN as dgn, thueN as tn\n"
		"from nhap) as t1\n"
		"natural left join\n"
		"(select dot_nhap as dn,ms_hang as msh,sum(so_luong_kgX) as slkx,sum(so_luong_cayX) as slcx, don_giaX as dgx, thueX as tx\n"
		"from xuat group by dot_nhap,ms_hang) as t2;"
		;

	switch(tabID)
	{
	case TAB_NHAP_ID:
		if (m_pKmySql->query(CString("select dot_nhap,ms_hang,so_luong_kgN,so_luong_cayN,don_giaN,thueN,commentN from nhap;")))
		{
			m_pKmySql->store_result();
			show_on_list();
		}
		break;
	case TAB_XUAT_ID:
		if (m_pKmySql->query(CString("select dot_nhap,ms_hang,ngay_xuat,so_luong_kgX,so_luong_cayX,don_giaX,thueX,commentX from xuat;")))
		{
			m_pKmySql->store_result();
			show_on_list();
		}
		break;
	case TAB_DOT_NHAP_ID:
		if(m_pKmySql->query(CString("select * from dot")))
		{
			m_pKmySql->store_result();
			show_on_list();
		}
		break;
	case TAB_HANG_ID:
		if (m_pKmySql->query(CString("select * from hang")))
		{
			m_pKmySql->store_result();
			show_on_list();
		}
		break;
	case TAB_TON_ID:
		if (m_pKmySql->query(ton_q))
		{
			m_pKmySql->store_result();
			show_on_list();
		}
		break;

	}
}

void InputDlg::OnLvnItemchangedResList(NMHDR *pNMHDR, LRESULT *pResult)
{
//	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	int		nCurSel = m_resList.GetNextItem(-1 , LVNI_FOCUSED   | LVNI_SELECTED);
	if(GetFocus()==&m_resList && nCurSel!=-1)
	{
		//AfxMessageBox(_T("item is select"));
		show_record();
		update_TTCTedt();
	}
	*pResult = 0;
}

void InputDlg::OnNMDblclkResList(NMHDR *pNMHDR, LRESULT *pResult)
{
//	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<NMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	show_table();
	*pResult = 0;
}

//them mot record vao table co noi dung ung voi seleted row tren list view
bool InputDlg::add_record(void)
{
	CString strArr[10];
	int		i,j,n=26;//,index[26];
	UINT mask;

	mask=1;j=m_addNField;
	for(i=n-1;i>=0;i--)
	{				
		if(m_flagAC&mask) 
		{
			j--;
			lcwnd[i]->GetWindowText(strArr[j]);			
		}		
		mask<<=1;
	}

	if(m_pKmySql->insert_row(m_addTable,strArr,m_addNField))
	{
		//AfxMessageBox(_T("đã add xong"));
		add_row(strArr,m_addNField);
		return true;
	}
	else
		AfxMessageBox(_T("không add được!"));
	return false;
}

bool InputDlg::show_record(void)
{
	int		currow,i,j,n=26;//,index[26];
	CString	temp;
	UINT mask;

	currow=m_resList.GetNextItem(-1,LVIS_FOCUSED|LVIS_SELECTED);

	switch(tabID){		
		case TAB_TON_ID:
			temp.Empty();
			for(j=0;j<N_TON_FIELD;j++)
				temp.AppendFormat(_T("%s\t"),m_resList.GetItemText(currow,j));
			m_ghiChu.SetWindowText(temp);
			break;
		default:
			mask=1;j=m_addNField;
			for(i=n-1;i>=0;i--)
			{				
				if(m_flagAC&mask) 
				{
					j--;
					//m_resList.GetItemText(currow,j);
					lcwnd[i]->SetWindowText(m_resList.GetItemText(currow,j));
				}		
				mask<<=1;
			}
	}
	
	return false;
}

void InputDlg::OnBnClickedRemoveBtn()
{
	// TODO: Add your control notification handler code here
	del_record();
}

//xoa mot row tren list view
void InputDlg::del_row(int row)
{
	m_resList.DeleteItem(row);
	total_row--;
}

void InputDlg::OnEnChangeDgEdt()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if(GetFocus()==&m_donGia)
	{
		update_TTCTedt();
	}	
}

//hien tong so tien chua thue = don gia * so luong
void InputDlg::update_TTCTedt(void)
{
	double thanhtien=0;
	TCHAR temp[MAX_PATH];
	m_donGia.GetWindowText(temp,MAX_PATH);
	thanhtien=atof(CKKString::cstrToAstr(temp));
	m_slkgEdt.GetWindowText(temp,MAX_PATH);
	thanhtien*=atof(CKKString::cstrToAstr(temp));
	swprintf_s(temp,MAX_PATH,_T("%.0f"),thanhtien);
	m_TTCT.SetWindowText(temp);
}

//xoa mot record trong table co noi dung tuong ung voi selected row tren list view
bool InputDlg::del_record(void)
{
	int nCurSel;
	CString	q;
	TCHAR	temp[MAX_PATH];

	switch(tabID)
	{
	case TAB_NHAP_ID:
	case TAB_XUAT_ID:		
		m_dotEdt.GetWindowText(temp,MAX_PATH);
		q.Format(CString("DELETE FROM %s WHERE dot_nhap='%s' "),m_addTable,temp);
		m_mshEdt.GetWindowText(temp,MAX_PATH);
		q.AppendFormat(_T("AND ms_hang='%s';"),temp);
		break;		
	case TAB_DOT_NHAP_ID:
		m_dotEdt.GetWindowText(temp,MAX_PATH);
		q.Format(CString("DELETE FROM dot WHERE dot_nhap='%s';"),temp);
		break;
	case TAB_HANG_ID:
		m_mshEdt.GetWindowText(temp,MAX_PATH);
		q.Format(CString("DELETE FROM hang WHERE ms_hang='%s';"),temp);
		break;
	}

	if(m_pKmySql->query(q))
	{
		nCurSel = m_resList.GetNextItem(-1 , LVNI_FOCUSED   | LVNI_SELECTED);
		del_row(nCurSel);
		return true;
	}
	else
		AfxMessageBox(_T("ko xóa được!"));
	return false;
}

void InputDlg::OnBnClickedEditBtn()
{
	// TODO: Add your control notification handler code here
	if(del_record())
		add_record();
}
