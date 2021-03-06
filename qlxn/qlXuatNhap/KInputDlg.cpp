// KInputDlg.cpp : implementation file
//

#include "stdafx.h"
#include "qlXuatNhap.h"
#include "KInputDlg.h"


// CKInputDlg dialog

IMPLEMENT_DYNAMIC(CKInputDlg, CDialog)

CKInputDlg::CKInputDlg(CWnd* pParent /*=NULL*/)
	//: CDHtmlDialog(CKInputDlg::IDD, CKInputDlg::IDH, pParent)
	: CDialog(CKInputDlg::IDD, pParent)
	, m_pKmySql(NULL)
	, tabID(0)
	, m_fieldCount(0)
	, m_flagAC(0)
	, lcwnd(NULL)
	, m_tableName(_T(""))
	, m_flagLI(NULL)
	, m_flagHC(0)
	, m_flagAI(NULL)
	, m_flagLC(0)
	, m_flagKI(NULL)
	, m_flagKC(0)
	, m_flagEI(NULL)
	, m_flagEC(0)
	, m_flagELI(NULL)
	, m_editable(false)
{
	lcwnd=(CWnd**)malloc(CKINPUT_CWND_N * sizeof(CWnd*) + 1);
	m_flagLI=(int*)malloc(CKINPUT_FLAGI_N*sizeof(int)+1);
	m_flagAI=(int*)malloc(CKINPUT_FLAGI_N*sizeof(int)+1);
	m_flagKI=(int*)malloc(CKINPUT_FLAGI_N*sizeof(int)+1);
	m_flagEI=(int*)malloc(CKINPUT_FLAGI_N*sizeof(int)+1);
	m_flagELI=(int*)malloc(CKINPUT_FLAGI_N*sizeof(int)+1);
}

CKInputDlg::~CKInputDlg()
{
	free(lcwnd);
	free(m_flagLI);
	free(m_flagAI);
	free(m_flagKI);
	free(m_flagEI);
	free(m_flagELI);
}

void CKInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ADD_BTN, m_addBtn);
	DDX_Control(pDX, IDC_EDIT_BTN, m_editBtn);
	DDX_Control(pDX, IDC_REMOVE_BTN, m_removeBtn);
	DDX_Control(pDX, IDC_RES_LIST, m_resList);
	DDX_Control(pDX, IDC_DATETIMEPICKER_0, m_ngay);
	DDX_Control(pDX, IDC_EDIT_7, m_ghiChu);
	DDX_Control(pDX, IDC_STATIC_LABEL_7, m_sGhiChu);
	DDX_Control(pDX, IDC_STATIC_LABEL_0, m_label0);
	DDX_Control(pDX, IDC_STATIC_LABEL_1, m_label1);
	DDX_Control(pDX, IDC_STATIC_LABEL_2, m_label2);
	DDX_Control(pDX, IDC_STATIC_LABEL_3, m_label3);
	DDX_Control(pDX, IDC_STATIC_LABEL_4, m_label4);
	DDX_Control(pDX, IDC_STATIC_LABEL_5, m_label5);
	DDX_Control(pDX, IDC_STATIC_LABEL_6, m_label6);
	DDX_Control(pDX, IDC_EDIT_0, m_edit0);
	DDX_Control(pDX, IDC_EDIT_1, m_edit1);
	DDX_Control(pDX, IDC_EDIT_2, m_edit2);
	DDX_Control(pDX, IDC_EDIT_3, m_edit3);
	DDX_Control(pDX, IDC_EDIT_4, m_edit4);
	DDX_Control(pDX, IDC_EDIT_5, m_edit5);
	DDX_Control(pDX, IDC_EDIT_6, m_edit6);
	DDX_Control(pDX, IDC_DATETIMEPICKER_1, m_ngayKT);
}

BOOL CKInputDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	TCHAR* date_format=_T("yyyy'-'MM'-'dd");

	init_var();

	m_ngay.SetFormat(date_format);
	m_ngayKT.SetFormat(date_format);

	create_flags();
	//an cac ctrl ko dung va set label cho các edit
	hideCtrl();
	set_comment();
	//set total row va tao col cho list view
	create_list_cols();
	m_resList.SetExtendedStyle(m_resList.GetExtendedStyle() 
		| LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CKInputDlg, CDialog)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_RES_LIST, &CKInputDlg::OnLvnItemchangedResList)
	ON_BN_CLICKED(IDC_ADD_BTN, &CKInputDlg::OnBnClickedAddBtn)
	ON_WM_SIZE()
	ON_NOTIFY(NM_DBLCLK, IDC_RES_LIST, &CKInputDlg::OnNMDblclkResList)
	ON_BN_CLICKED(IDC_REMOVE_BTN, &CKInputDlg::OnBnClickedRemoveBtn)
	ON_BN_CLICKED(IDC_EDIT_BTN, &CKInputDlg::OnBnClickedEditBtn)
	ON_EN_SETFOCUS(IDC_EDIT_0, &CKInputDlg::OnEnSetfocusEdit0)
	ON_EN_KILLFOCUS(IDC_EDIT_0, &CKInputDlg::OnEnKillfocusEdit0)
	ON_EN_UPDATE(IDC_EDIT_0, &CKInputDlg::OnEnUpdateEdit0)
END_MESSAGE_MAP()

// BEGIN_DHTML_EVENT_MAP(CKInputDlg)
// //	DHTML_EVENT_ONCLICK(_T("ButtonOK"), OnButtonOK)
// //	DHTML_EVENT_ONCLICK(_T("ButtonCancel"), OnButtonCancel)
// END_DHTML_EVENT_MAP()



// CKInputDlg message handlers
// 
// HRESULT CKInputDlg::OnButtonOK(IHTMLElement* /*pElement*/)
// {
// 	OnOK();
// 	return S_OK;
// }

// HRESULT CKInputDlg::OnButtonCancel(IHTMLElement* /*pElement*/)
// {
// 	OnCancel();
// 	return S_OK;
// }

int CKInputDlg::add_col(CString heading)
{
	CHeaderCtrl* pHeaderCtrl = m_resList.GetHeaderCtrl();
	if (pHeaderCtrl != NULL)
		return m_resList.InsertColumn(pHeaderCtrl->GetItemCount(),heading
		,LVCFMT_LEFT,KK_COL_WIDTH(heading.GetLength()));
	return 0;
}

void CKInputDlg::add_item(int row, int col, CString& content)
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

bool CKInputDlg::add_record(void)
{
	// 	CString strArr[11];
	// 	int i,*j=m_flagAI;
	// 	UINT mask = (UINT)pow(2,double(CKINPUT_CWND_N-1));
	// 	for( i=0;i<CKINPUT_CWND_N;i++)
	// 	{				
	// 		if(m_flagAC&mask) 
	// 		{
	// 			lcwnd[i]->GetWindowText(strArr[*j]);
	// 			j++;
	// 		}
	// 		mask>>=1;
	//  	}

	int *j=m_flagAI,i=0;
	CString strArr[CKINPUT_FLAGI_N];
	//TCHAR buff[CKINPUT_VAL_LEN];

	if (tabID==TAB_TON_ID)
		return false;

	while(*j!=CKINPUT_FLAG_INDEX_END)
	{
		lcwnd[*j]->GetWindowText(strArr[i]);
		i++;
		j++;
	}

	if(m_pKmySql->insert_row(m_tableName,strArr,m_fieldCount))
	{
		//AfxMessageBox(_T("đã add xong"));
		add_row(strArr,m_fieldCount);
		return true;
	}
	else
		AfxMessageBox(_T("không add được!"));
	return false;
}

// them mot dong vao list
void CKInputDlg::add_row(CString* contents, int n_col)
{
	int total_row=m_resList.GetItemCount();
	for(int i=0;i<n_col;i++)
		add_item(total_row,i,contents[i]);
}

int CKInputDlg::create(CTabCtrl* pTab, KMySQL* pKmysql, int tab_id, CRect& rect)
{
	m_pKmySql=pKmysql;
	tabID=tab_id;
	if(!Create(IDD,pTab))
		return 0;
	MoveWindow(rect);
	return 1;
}

int* CKInputDlg::create_flagI(
							  int* flag_i,
							  int c0, int c1, int c2, int c3, int c4, 
							  int c5, int c6, int c7, int c8, int c9
							  )
{
	flag_i[0]=c0;
	flag_i[1]=c1;
	flag_i[2]=c2;
	flag_i[3]=c3;
	flag_i[4]=c4;
	flag_i[5]=c5;
	flag_i[6]=c6;
	flag_i[7]=c7;
	flag_i[8]=c8;
	flag_i[9]=c9;
	flag_i[10]=CKINPUT_FLAG_INDEX_END;
	return flag_i;
}

int CKInputDlg::create_flags(void)
{
	switch(tabID)
	{
		/*tested*/	
	case TAB_DU_AN_ID:
		m_flagHC=0x17F00;		//0000 0101 1111 1100 00 0000
		m_flagLC=0x2A8020;		//1010 1010 0000 0000 10 0000
		/*LI*/	create_flagI(m_flagLI,8,15,10,11,3);

		m_flagAC=0x1400D0;		//0101 0000 0000 0011 01 0000
		/*AI*/	create_flagI(m_flagAI,1,3,14,15,17);

		m_flagKC=0x100000;		//0100 0000 0000 0000 00 0000
		/*KI*/	create_flagI(m_flagKI,14);

		m_flagEC=0x400D0;		//0001 0000 0000 0011 01 0000
		/*EI*/	create_flagI(m_flagEI,22,16,17,8);
		/*ELI*/	create_flagI(m_flagELI,1,2,3,4);

		m_tableName=_T(DU_AN_TABLE);
		m_fieldCount=N_DU_AN_FIELD;
		break;

		/*tested*/
	case TAB_DOT_NHAP_ID:
		m_flagHC=0x7F40;
		m_flagLC=0x2A8020;
		/*LI*/	create_flagI(m_flagLI,8,6,12,4,3);

		m_flagAC=0x150090;
		/*AI*/	create_flagI(m_flagAI,1,3,14,5,17);

		m_flagKC=0x40000;
		/*KI*/	create_flagI(m_flagKI,12);

		m_flagEC=0x110090;
		/*EI*/	create_flagI(m_flagEI,7,10,18,8);
		/*ELI*/	create_flagI(m_flagELI,0,3,2,4);

		m_tableName=_T(DOT_NHAP_TABLE);
		m_fieldCount=N_DOT_NHAP_FIELD;		
		break;

		/*tested*/
	case TAB_HANG_ID:
		m_flagHC=0xFFC0;
		m_flagLC=0x2A0020;
		/*LI*/	create_flagI(m_flagLI,9,16,2,3);

		m_flagAC=0;
		/*AI*/	create_flagI(m_flagAI,1,3,5,17);

		m_flagKC=0x100000;
		/*KI*/	create_flagI(m_flagKI,15);

		m_flagEC=0x50010;
		/*EI*/	create_flagI(m_flagEI,23,4,8);
		/*ELI*/	create_flagI(m_flagELI,1,2,3);

		m_tableName=_T(HANG_TABLE);
		m_fieldCount=N_HANG_FIELD;
		break;

		/*tested*/
	case TAB_NHAP_ID:
		m_flagHC=0x3C0;
		m_flagLC=0x2AA820;
		/*LI*/	create_flagI(m_flagLI,6,9,14,1,17,0,3);

		m_flagAC=0;
		/*AI*/	create_flagI(m_flagAI,1,3,5,7,9,11,17);

		m_flagKC=0x140000;
		/*KI*/	create_flagI(m_flagKI,5,9);

		m_flagEC=0x15410;
		/*EI*/	create_flagI(m_flagEI,20,2,24,0,8);
		/*ELI*/	create_flagI(m_flagELI,2,3,4,5,6);

		m_tableName=_T(NHAP_TABLE);
		m_fieldCount=N_NHAP_FIELD;
		break;

		/*tested*/
	case TAB_DOT_XUAT_ID:
		m_flagHC=0x7F40;
		m_flagLC=0x2A8020;
		/*LI*/	create_flagI(m_flagLI,8,7,13,5,3);

		m_flagAC=0;
		/*AI*/	create_flagI(m_flagAI,1,3,14,5,17);

		m_flagKC=0x40000;
		/*KI*/	create_flagI(m_flagKI,13);

		m_flagEC=0x110090;
		/*EI*/	create_flagI(m_flagEI,7,11,19,8);
		/*ELI*/	create_flagI(m_flagELI,0,3,2,4);

		m_tableName=_T(DOT_XUAT_TABLE);
		m_fieldCount=N_DOT_XUAT_FIELD;
		break;

		/*tested*/
	case TAB_XUAT_ID:
		m_flagHC=0xC0;
		m_flagLC=0x2AAA20;
		/*LI*/	create_flagI(m_flagLI,6,9,7,14,1,17,0,3);

		m_flagAC=0;
		/*AI*/	create_flagI(m_flagAI,1,3,5,7,9,11,13,17);

		m_flagKC=0x150000;
		/*KI*/	create_flagI(m_flagKI,5,9,6);

		m_flagEC=0x5510;
		/*EI*/	create_flagI(m_flagEI,21,3,25,1,8);
		/*ELI*/	create_flagI(m_flagELI,3,4,5,6,7);

		m_tableName=_T(XUAT_TABLE);
		m_fieldCount=N_XUAT_FIELD;
		break;
// 	case TAB_TON_ID:
// 		break;
	}
	return 0;
}

// make condition
CString CKInputDlg::create_strC(void)
{
	CString conditon=_T("(");
	TCHAR buff[CKINPUT_VAL_LEN];

	int *j =m_flagKI;
	UINT mask=(UINT)pow(2,double(CKINPUT_CWND_N));
	for(int i=0;i<CKINPUT_CWND_N
		&& *j != CKINPUT_FLAG_INDEX_END 
		;i++)
	{	
		mask>>=1;			
		if(m_flagKC&mask)
		{
			lcwnd[i]->GetWindowText(buff,CKINPUT_VAL_LEN);
			if(buff[0]==0)//neu key la str rong thi thoat
				return CString();
			conditon.AppendFormat(_T("%s='%s' AND "),m_fields[*j],buff);			
			j++;
		}
	}
	conditon.TrimRight(_T(" AND "));
	conditon.AppendChar(')');
	return conditon;
}

// new value (used to edit an old record)
CString CKInputDlg::create_strNV(void)
{
	CString newVal=_T("");
	TCHAR buff[CKINPUT_VAL_LEN];

	int *j =m_flagEI;
	UINT mask=(UINT)pow(2,double(CKINPUT_CWND_N));
	for(int i=0;i<CKINPUT_CWND_N
		//&& *j != CKINPUT_FLAG_INDEX_END 
		;i++)
	{	
		mask>>=1;			
		if(m_flagEC&mask)
		{
			lcwnd[i]->GetWindowText(buff,CKINPUT_VAL_LEN);
			if(buff[0]==0)//neu key la str rong
				newVal.AppendFormat(_T("%s=NULL,"),m_fields[*j]);
			else 
				newVal.AppendFormat(_T("%s='%s',"),m_fields[*j],buff);			
			j++;
		}
	}
	newVal.TrimRight(',');
	return newVal;
}

void CKInputDlg::create_list_cols(void)
{
	// 	CString dot_ch[]={_T("Đợt nhập"),_T("Mã số chứng từ"),_T("Ngày nhập"),_T("Ghi chú")};
	// 	CString hang_ch[]={_T("Mã số hàng"),_T("Tên hàng"),_T("Ghi chú")};
	// 	CString	nhap_ch[]={_T("Đợt nhập"),_T("Mã số hàng"),_T("Số lượng Kg"),_T("Số lượng Cây"),_T("Đơn giá"),_T("Thuế"),_T("Ghi chú")};
	// 	CString xuat_ch[]={_T("Đợt nhập"),_T("Mã số hàng"),_T("Số lượng Kg"),_T("Số lượng Cây"),_T("Đơn giá"),_T("Thuế"),_T("Ngày xuất"),_T("Ghi chú")};
	// 	CString	ton_ch[]={_T("Đợt nhập"),_T("Mã số hàng"),_T("Số lượng Kg"),_T("Số lượng Cây"),_T("Đơn giá"),_T("Thuế")};
	// 
	// 	CString *col_heading=0;
	// 	int		n_col=0;
	// 
	// 	switch (tabID)
	// 	{
	// 	case TAB_DU_AN_ID:
	// 
	// 		break;
	// 	case TAB_NHAP_ID:
	// 		col_heading=nhap_ch;
	// 		n_col=7;
	// 		break;
	// 	case TAB_XUAT_ID:
	// 		col_heading=xuat_ch;
	// 		n_col=8;
	// 		break;
	// 	case TAB_DOT_NHAP_ID:
	// 		col_heading=dot_ch;
	// 		n_col=4;
	// 		break;
	// 	case TAB_HANG_ID:
	// 		col_heading=hang_ch;
	// 		n_col=3;
	// 		break;
	// 	case TAB_TON_ID:
	// 		col_heading=ton_ch;
	// 		n_col=6;
	// 		break;
	// 	case TAB_ALL_ID:
	// 		break;
	// 	}
	// 
	// 	for (int i=0; i<n_col; i++)
	// 		m_resList.InsertColumn(i,col_heading[i],LVCFMT_LEFT,LVCF_MINWIDTH);
}

void CKInputDlg::clean_list(void)
{
	m_resList.DeleteAllItems();
}

bool CKInputDlg::del_record(void)
{
	int nCurSel;
	CString	q;
	switch(tabID)
	{
	case TAB_TON_ID:
		return false;
	default:
		q.Format(_T("DELETE FROM %s WHERE %s ")
			,m_tableName,create_strC());
	}

	if(m_pKmySql->query(q))
	{
		nCurSel = m_resList.GetNextItem(
			-1, LVNI_FOCUSED   | LVNI_SELECTED);
		m_resList.DeleteItem(nCurSel);
		return true;
	}
	else
		AfxMessageBox(_T("ko xóa được!"));
	return false;
}

bool CKInputDlg::edit_record(void)
{
	CString	q;
	TCHAR	buff[CKINPUT_VAL_LEN];
	int CurSel,i,*j=m_flagELI;
	UINT mask = (UINT)pow(2,double(CKINPUT_CWND_N-1));

	switch(tabID)
	{
	case TAB_TON_ID:
		return false;
	default:
		q.Format(_T("UPDATE %s SET %s WHERE %s")
			,m_tableName,create_strNV(),create_strC());
	}

	if(m_pKmySql->query(q))
	{
		CurSel = m_resList.GetNextItem(-1 , LVNI_FOCUSED | LVNI_SELECTED);
		for( i=0;i<CKINPUT_CWND_N;i++)
		{	
			if(m_flagEC&mask)
			{
				lcwnd[i]->GetWindowText(buff,CKINPUT_VAL_LEN);
				m_resList.SetItemText(CurSel,*j,buff);
				j++;
			}
			mask>>=1;
		}
		return true;
	}
	else
		AfxMessageBox(_T("ko sửa được!"));
	return false;
}

int CKInputDlg::hideCtrl(void)
{
	UINT mask;
	mask=(UINT)pow(2,double(CKINPUT_CWND_N-1));
	for(int i=0;i<CKINPUT_CWND_N;i++)
	{				
		if(m_flagHC&mask) lcwnd[i]->ShowWindow(SW_HIDE);
		mask>>=1;
	}
	return 0;
}

void CKInputDlg::init_var(void)
{
	CWnd * temp[]={
		(CWnd*)&m_label0,(CWnd*)&m_edit0,
		(CWnd*)&m_label1,(CWnd*)&m_edit1,	//4
		(CWnd*)&m_label2,(CWnd*)&m_edit2,
		(CWnd*)&m_label3,(CWnd*)&m_edit3,	//8
		(CWnd*)&m_label4,(CWnd*)&m_edit4,
		(CWnd*)&m_label5,(CWnd*)&m_edit5,	//12
		(CWnd*)&m_label6,(CWnd*)&m_edit6,
		(CWnd*)&m_ngay,(CWnd*)&m_ngayKT,	//16
		(CWnd*)&m_sGhiChu,(CWnd*)&m_ghiChu,
		(CWnd*)&m_resList,(CWnd*)&m_addBtn,	//20
		(CWnd*)&m_editBtn,(CWnd*)&m_removeBtn};	//22
		//set lcwnd value
		lcwnd=(CWnd**)malloc(CKINPUT_CWND_N * sizeof(CWnd*) + 1);
		memcpy(lcwnd, temp, CKINPUT_CWND_N * sizeof (CWnd*) );

		TCHAR* strTemp[]={
			_T("chi_phi_phu_n"),
			_T("chi_phi_phu_x"),
			_T("don_gia_n"),
			_T("don_gia_x"),
			_T("don_vi"),
			_T("dot_nhap"),
			_T("dot_xuat"),
			_T("du_an"),
			_T("ghi_chu"),
			_T("hang"),
			_T("hoa_don_nhap"),
			_T("hoa_don_xuat"),
			_T("ms_dot_nhap"),
			_T("ms_dot_xuat"),
			_T("ms_du_an"),
			_T("ms_hang"),
			_T("ngay_bat_dau"),
			_T("ngay_ket_thuc"),
			_T("ngay_nhap"),
			_T("ngay_xuat"),
			_T("so_luong_n"),
			_T("so_luong_x"),
			_T("ten_du_an"),
			_T("ten_hang"),
			_T("thue_n"),
			_T("thue_x")
		};
		for(int i=0;i<CKINPUT_FIELD_N;i++)
			m_fields.Add(strTemp[i]);

		char *ton_q="select	t1.dn,t1.msh,slkn-IFNULL(slkx,0) as slKG,slcn-IFNULL(slcx,0) as slc,dgn,tn\n"
			"from\n"
			"(select dot_nhap as dn,ms_hang as msh,so_luong_kgN as slkn,so_luong_cayN as slcn, don_giaN as dgn, thueN as tn\n"
			"from nhap) as t1\n"
			"natural left join\n"
			"(select dot_nhap as dn,ms_hang as msh,sum(so_luong_kgX) as slkx,sum(so_luong_cayX) as slcx, don_giaX as dgx, thueX as tx\n"
			"from xuat group by dot_nhap,ms_hang) as t2;"
			;

		TCHAR *wcstr;
		size_t pReturnValue=0;
		size_t sizeInWords=strlen(ton_q)+1;

		wcstr=(TCHAR*)malloc(sizeInWords*sizeof(TCHAR));
		mbstowcs_s(&pReturnValue,wcstr,sizeInWords,ton_q,_TRUNCATE);
		m_queries.Add(wcstr);

		// 	switch(tabID)
		// 	{
		// 	case TAB_XUAT_ID:
		// 		m_tableName=_T(XUAT_TABLE);
		// 		m_fieldCount=N_XUAT_FIELD;
		// //		m_flagAC=0x1155110;//0100 01 01 0101 0100 01 0001 0000
		// 		break;
		// 	case TAB_DOT_NHAP_ID:
		// 		m_fieldCount=4;
		// 		m_tableName=_T("dot");
		// //		m_flagAC=0x01440010;//0101 00 01 0000 0000 00 0001 0000
		// 		break;
		// 	case TAB_HANG_ID:
		// 		m_tableName=_T("hang");
		// 		m_fieldCount=3;
		// //		m_flagAC=0x0100050;//0000 01 00 0000 0000 00 0101 0000
		// 		break;
		// 	default:
		// 		m_fieldCount=0;		
		// 	}
}

int CKInputDlg::resize(int cx, int cy)
{
	int x,y,nx,ny,xl;
	UINT mask=(UINT)pow(2,double(CKINPUT_CWND_N));
	int	i,n=CKINPUT_CWND_N; //so cwnd cua input dlg
		

	CRect rect;
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
	case TAB_TON_ID:
		x=KK_L_SPACE;y=KK_T_SPACE+KK_Y_SPACE+KK_NY_STATIC;
		nx=cx-x-KK_R_SPACE;ny=KK_NY_EDT;
		m_ghiChu.MoveWindow(KKSCALE(x),KKSCALE(y),KKSCALE(nx),KKSCALE(ny));
		break;
	default:
		nx=(cx-KK_L_SPACE-KK_R_SPACE)/m_fieldCount - KK_X_SPACE;
		ny=KK_NY_EDT;
		rect.SetRect(0,0,KKSCALE(nx),KKSCALE(ny));
		x=KK_L_SPACE;
		xl=KK_L_SPACE;

		for(i=0;i<CKINPUT_CWND_N;i++)
		{
			mask>>=1;
			if(m_flagLC&mask)
			{
				rect.MoveToXY(KKSCALE(xl),KKSCALE(KK_T_SPACE));
				xl+=KK_X_SPACE+nx;
				lcwnd[i]->MoveWindow(rect);
			}
		}

		for(i=0;i<m_fieldCount-1;i++)
		{
			rect.MoveToXY(KKSCALE(x),KKSCALE(KK_T_SPACE+KK_NY_STATIC+KK_Y_SPACE));
			x+=KK_X_SPACE+nx;
			lcwnd[m_flagAI[i]]->MoveWindow(rect);
		}

		nx=cx-KK_R_SPACE-x;ny=KK_NY_EDT;y=KK_T_SPACE+KK_NY_STATIC+KK_Y_SPACE;
		lcwnd[m_flagAI[i]]->MoveWindow(KKSCALE(x),KKSCALE(y),KKSCALE(nx),KKSCALE(ny));
	}
	return 0;
}

//create cols in result list
bool CKInputDlg::set_comment(void)
{
	TCHAR* comment[]={
		_T("Chi phí phụ"),
		_T("Đơn giá"),
		_T("Đơn vị"),
		_T("Ghi chú"),
		_T("Hóa đơn nhập"),
		_T("Hóa đơn xuất"),
		_T("MS đợt nhập *"),
		_T("MS đợt xuất *"),
		_T("MS dự án *"),
		_T("MS hàng *"),
		_T("Ngày bắt đầu"),
		_T("Ngày kết thúc"),
		_T("Ngày nhập"),
		_T("Ngày xuất"),
		_T("Số lượng"),
		_T("Tên dự án"),
		_T("Tên hàng"),
		_T("Thuế"),
	};
	
	int *j =m_flagLI;
	UINT mask=(UINT)pow(2,double(CKINPUT_CWND_N));
	for(int i=0;i<CKINPUT_CWND_N
		//&& *j != CKINPUT_FLAG_INDEX_END 
		;i++)
	{	
		mask>>=1;			
		if(m_flagLC&mask)
		{
			lcwnd[i]->SetWindowText(comment[*j]);
			add_col(comment[*j]);
			j++;
		}
	}
	return false;
}

void CKInputDlg::show_on_list(void)
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
}

bool CKInputDlg::show_selected_record(void)
{
	int curSel=m_resList.GetNextItem(-1,LVIS_FOCUSED|LVIS_SELECTED);
	//int *j=m_flagAI;
	// 	UINT mask=(UINT)pow(2,double(CKINPUT_CWND_N));
	// 
	// 	for(int i=0;i<CKINPUT_CWND_N
	// 		//&& *j != CKINPUT_FLAG_INDEX_END 
	// 		;i++)
	// 	{	
	// 		mask>>=1;			
	// 		if(m_flagAC&mask)
	// 		{	
	// 			lcwnd[i]->SetWindowText(m_resList.GetItemText(curSel,j));
	// 			j++;
	// 		}
	// 	}	
	for(int i=0;i<m_fieldCount;i++)
	{
		lcwnd[m_flagAI[i]]->SetWindowText(m_resList.GetItemText(curSel,i));
	}
	return false;
}

void CKInputDlg::show_table(void)
{
	// 	char *ton_q="select	t1.dn,t1.msh,slkn-IFNULL(slkx,0) as slKG,slcn-IFNULL(slcx,0) as slc,dgn,tn\n"
	// 		"from\n"
	// 		"(select dot_nhap as dn,ms_hang as msh,so_luong_kgN as slkn,so_luong_cayN as slcn, don_giaN as dgn, thueN as tn\n"
	// 		"from nhap) as t1\n"
	// 		"natural left join\n"
	// 		"(select dot_nhap as dn,ms_hang as msh,sum(so_luong_kgX) as slkx,sum(so_luong_cayX) as slcx, don_giaX as dgx, thueX as tx\n"
	// 		"from xuat group by dot_nhap,ms_hang) as t2;"
	// 		;
	CString q;

	switch(tabID)
	{
	case TAB_TON_ID:
		if (m_pKmySql->query(m_queries[CKINPUT_TON_QUERY_I]))
		{
			m_pKmySql->store_result();
			show_on_list();
		}
		break;
	default:
		q.Format(_T("SELECT * FROM %s;"),m_tableName);
		if (m_pKmySql->query(q))
		{
			m_pKmySql->store_result();
			show_on_list();
		}
	}
}

// void CKInputDlg::add_row(CStringArray* contents)
// {
// 	int total_row=m_resList.GetItemCount();
// 	CString temp;
// 	for(int i=0;i<contents->GetCount();i++)
// 	{
// 		temp=contents->GetAt(i);
// 		add_item(total_row,i,temp);
// 	}
// }

void CKInputDlg::OnBnClickedAddBtn()
{
	// TODO: Add your control notification handler code here
	add_record();
}

void CKInputDlg::OnBnClickedEditBtn()
{
	// TODO: Add your control notification handler code here
	edit_record();
}

void CKInputDlg::OnBnClickedRemoveBtn()
{
	// TODO: Add your control notification handler code here
	del_record();
}

void CKInputDlg::OnLvnItemchangedResList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	int	nCurSel = m_resList.GetNextItem(-1 , LVNI_FOCUSED   | LVNI_SELECTED);
	if(GetFocus()==&m_resList && nCurSel!=-1)
	{
		//AfxMessageBox(_T("item is select"));
		show_selected_record();
		update_TTCTedt();
	}
	*pResult = 0;
}

void CKInputDlg::OnNMDblclkResList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	show_table();
	*pResult = 0;
}

void CKInputDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (::IsWindow(m_resList.GetSafeHwnd()))
	{
		resize(cx,cy);
	}
}

void CKInputDlg::update_TTCTedt(void)
{
	//	double thanhtien=0;
	//	TCHAR temp[20];
	//	m_donGia.GetWindowText(temp,20);
	//	thanhtien=atof(CKKString::cstrToAstr(temp));
	//	m_slkgEdt.GetWindowText(temp,20);
	//	thanhtien*=atof(CKKString::cstrToAstr(temp));
	//	swprintf_s(temp,20,_T("%.0f"),thanhtien);
	//	m_TTCT.SetWindowText(temp);
}

void CKInputDlg::OnEnSetfocusEdit0()
{
	// TODO: Add your control notification handler code here
	//AfxMessageBox(_T("edit0 set focus"));
}

void CKInputDlg::OnEnKillfocusEdit0()
{
	// TODO: Add your control notification handler code here
	//AfxMessageBox(_T("edit0 kill focus"));
}

void CKInputDlg::OnEnUpdateEdit0()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.

	// TODO:  Add your control notification handler code here
	//CString temp;
	//m_edit0.GetWindowText(temp);
	//AfxMessageBox(CString("Up date edit0")+temp);
}
