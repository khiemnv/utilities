// CusQueryDlg.cpp : implementation file
//

#include "stdafx.h"
#include "qlXuatNhap.h"
#include "CusQueryDlg.h"

#ifndef INPUT_TAB_ID
#define INPUT_TAB_ID
#define TAB_DU_AN_ID	0	//khiem
#define TAB_DOT_NHAP_ID	1
#define TAB_HANG_ID		2
#define	TAB_NHAP_ID		3
#define TAB_DOT_XUAT_ID 4
#define TAB_XUAT_ID		5

#define TAB_TON_ID		6
#define TAB_ALL_ID		7
#endif
// CCusQueryDlg dialog

IMPLEMENT_DYNAMIC(CCusQueryDlg, CDialog)

CCusQueryDlg::CCusQueryDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCusQueryDlg::IDD, pParent)
	, m_pKmySql(NULL)
	, total_row(0)
	, m_tabID(0)
	, m_buffer(NULL)
{
	m_buffer=(TCHAR*)malloc(CCQD_QBUFF_SIZE*sizeof(TCHAR));
}

CCusQueryDlg::~CCusQueryDlg()
{
	free(m_buffer);
}

void CCusQueryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_QUERY_EDT, m_queryEdt);
	DDX_Control(pDX, IDC_RES_LIST, m_resList);
	DDX_Control(pDX, IDC_QUERY_BTN, m_queryBtn);
}


BEGIN_MESSAGE_MAP(CCusQueryDlg, CDialog)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_QUERY_BTN, &CCusQueryDlg::OnBnClickedQueryBtn)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_RES_LIST, &CCusQueryDlg::OnLvnItemchangedResList)
	//ON_EN_SETFOCUS(IDC_QUERY_EDT, &CCusQueryDlg::OnEnSetfocusQueryEdt)
	//ON_WM_LBUTTONDOWN()
	//ON_COMMAND(WM_LBUTTONDOWN,&CCusQueryDlg::OnLeftClick)
	ON_EN_CHANGE(IDC_QUERY_EDT, &CCusQueryDlg::OnEnChangeQueryEdt)
	//ON_MESSAGE(WM_SETFOCUS, &CCusQueryDlg::onMyMesg)
	//ON_CONTROL(EN_CHANGE,IDC_QUERY_EDT,&CCusQueryDlg::onMyContrl)
	ON_NOTIFY(EN_SETFOCUS,IDC_QUERY_EDT,&CCusQueryDlg::onMyNotify)

	ON_WM_NCLBUTTONDOWN()
	ON_NOTIFY(NM_DBLCLK, IDC_RES_LIST, &CCusQueryDlg::OnNMDblclkResList)
END_MESSAGE_MAP()


// CCusQueryDlg message handlers

void CCusQueryDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (::IsWindow(m_resList.GetSafeHwnd()))
	{
		resize(cx,cy);
	}
}

int CCusQueryDlg::resize(int cx, int cy)
{
	int x,y,nx,ny;
	cx=KKUNSCALE(cx);
	cy=KKUNSCALE(cy);
	switch(m_tabID)
	{
	case TAB_ALL_ID:
		//resize the btn
		x=cx-KK_R_SPACE-KK_NX_BTN;y=KK_T_SPACE;
		nx=KK_NX_BTN;ny=KK_NY_BTN;
		m_queryBtn.MoveWindow(KKSCALE(x),KKSCALE(y),KKSCALE(nx),KKSCALE(ny));
		//resize the query  edit
		nx=x-KK_L_SPACE-KK_X_SPACE;
		x=KK_L_SPACE;
		m_queryEdt.MoveWindow(KKSCALE(x),KKSCALE(y),KKSCALE(nx),KKSCALE(ny));
		//resize the list
		y=y+ny+KK_Y_SPACE;
		nx=cx-KK_L_SPACE-KK_R_SPACE;ny=cy-y-KK_B_SPACE;
		m_resList.MoveWindow(KKSCALE(x),KKSCALE(y),KKSCALE(nx),KKSCALE(ny));
		break;
	case TAB_TON_ID:
// 		//resize the btn
// 		x=cx-KK_R_SPACE-KK_NX_BTN;y=KK_T_SPACE;
// 		nx=KK_NX_BTN;ny=KK_NY_BTN;
// 		m_queryBtn.MoveWindow(KKSCALE(x),KKSCALE(y),KKSCALE(nx),KKSCALE(ny));
		//resize the query  edit
		nx=cx-KK_L_SPACE-KK_R_SPACE;ny=KK_NY_EDT;
		x=KK_L_SPACE;y=KK_T_SPACE;
		m_queryEdt.MoveWindow(KKSCALE(x),KKSCALE(y),KKSCALE(nx),KKSCALE(ny));
		//resize the list
		y=y+ny+KK_Y_SPACE;
		ny=cy-y-KK_B_SPACE;
		m_resList.MoveWindow(KKSCALE(x),KKSCALE(y),KKSCALE(nx),KKSCALE(ny));
		break;
	}
	return 0;
}

int CCusQueryDlg::resize(void)
{
	CRect wrect;
	GetWindowRect(&wrect);
	resize(wrect.Width(),wrect.Height());
	return 0;
}

int CCusQueryDlg::create(CTabCtrl *pTab, KMySQL *pKmysql,int tab_id,CRect &rect)
{
	m_pKmySql=pKmysql;
	Create(IDD,pTab);
	m_tabID=tab_id;
	MoveWindow(rect);
	if(m_tabID==TAB_TON_ID)
		m_queryBtn.ShowWindow(SW_HIDE);
	return 0;
}

void CCusQueryDlg::OnBnClickedQueryBtn()
{
	// TODO: Add your control notification handler code here
	m_queryEdt.GetWindowText(m_buffer,CKMYSQL_BUFF_LENGTH);
	if(m_pKmySql->query(m_buffer))
	{
		m_pKmySql->store_result();
		show_result();
	}	
}

void CCusQueryDlg::show_result(void)
{
	MYSQL_ROW row;
	unsigned int num_fields;
	unsigned int i=0,j=0;

	clean_list();

	create_cols();//tao cac cols

	num_fields = mysql_num_fields(m_pKmySql->get_res());
	
// 	for (int i=0; i<num_fields; i++)
// 		m_resList.InsertColumn(i,_T(""),LVCFMT_LEFT,LVCF_MINWIDTH);

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

void CCusQueryDlg::clean_list(void)
{
	m_resList.DeleteAllItems();
	int nColumnCount=0;
	CHeaderCtrl* pHeaderCtrl = m_resList.GetHeaderCtrl();

	if (pHeaderCtrl != NULL)
		nColumnCount = pHeaderCtrl->GetItemCount();

	// Delete all of the columns.
	for (int i=0; i < nColumnCount; i++)
	{
		m_resList.DeleteColumn(0);
	}

	total_row=0;
}

void CCusQueryDlg::add_item(int row, int col, CString content)
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

void CCusQueryDlg::create_cols(void)
{
	unsigned int num_fields;
	unsigned int i;
	MYSQL_FIELD *fields;

	size_t pReturnValue;
	size_t sizeInWords=CCQD_MAXHEADING_LEN;

	num_fields = mysql_num_fields(m_pKmySql->get_res());
	fields = mysql_fetch_fields(m_pKmySql->get_res());
	for(i = 0; i < num_fields; i++)
	{
		mbstowcs_s(&pReturnValue,m_buffer,sizeInWords,fields[i].name,_TRUNCATE);
		m_resList.InsertColumn(i,m_buffer,LVIF_TEXT,KK_COL_WIDTH(pReturnValue));
	}
}

BOOL CCusQueryDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_resList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CCusQueryDlg::OnLvnItemchangedResList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	int curSel;
	CString selRow;

	int nColumnCount;
	CHeaderCtrl* pHeaderCtrl = m_resList.GetHeaderCtrl();

	if (pHeaderCtrl != NULL)
		nColumnCount = pHeaderCtrl->GetItemCount();
	else
		nColumnCount=0;

	for(int i=0;i<nColumnCount;i++)
	{
		curSel=m_resList.GetNextItem(-1,LVIS_FOCUSED|LVIS_SELECTED);
		selRow.AppendFormat(_T("%s\t"),m_resList.GetItemText(curSel,i));
	}

	selRow.TrimRight('\t');
	m_queryEdt.SetWindowText(selRow);
	*pResult = 0;
}

void CCusQueryDlg::OnEnSetfocusQueryEdt()
{
	// TODO: Add your control notification handler code here
//	AfxMessageBox(_T("query edit get focus"));
// 	int temp=m_queryEdt.GetWindowTextLength();
// 	m_queryEdt.SetHighlight(0,temp-1);
	/*	m_queryEdt.SetHighlight(0,selRow.GetLength()-1);*/
// 	m_queryEdt.SetFocus();
//  	m_queryEdt.SetSel(0,-1,FALSE);
}

void CCusQueryDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	AfxMessageBox(_T("mouse left click"));
// 	if (ChildWindowFromPoint(point)==&m_queryEdt)
// 	{
// 		m_queryEdt.SetHighlight(0,m_queryEdt.GetWindowTextLength());;
// 	}
//	m_queryEdt.SetSel(0,-1,TRUE);

	CDialog::OnLButtonDown(nFlags, point);
}

void CCusQueryDlg::OnLeftClick()
{
	AfxMessageBox(_T("left btn click"));
}
void CCusQueryDlg::OnEnChangeQueryEdt()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	//AfxMessageBox(_T("en change"));
}

void CCusQueryDlg::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	AfxMessageBox(_T("non client left btn down"));
	CDialog::OnNcLButtonDown(nHitTest, point);
}

LRESULT CCusQueryDlg::onMyMesg(WPARAM wpa, LPARAM lpa)
{
	if((HWND)lpa==m_queryEdt.GetSafeHwnd())
		AfxMessageBox(_T("on my mesg"));
	return 0;
}
void CCusQueryDlg::onMyContrl()
{
	AfxMessageBox(_T("on my ctrl"));
}

void CCusQueryDlg::onMyNotify(NMHDR*pNotifyStruct,LRESULT*result)
{
	AfxMessageBox(_T("on my notify"));
	*result=0;
}
void CCusQueryDlg::OnNMDblclkResList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	static char* tinh_ton_q="select	t1.dn as 'dot nhap',t1.msh as 'ms hang',sln-IFNULL(slx,0) as 'so luong ton',dgn as 'don gia nhap',tn as 'thue nhap'\n"
		"from\n"
		"(select dot_nhap as dn,hang as msh,so_luong_n as sln, don_gia_n as dgn, thue_n as tn\n"
		"from nhap) as t1\n"
		"natural left join\n"
		"(select dot_nhap as dn,hang as msh,sum(so_luong_x) as slx, don_gia_x as dgx, thue_x as tx\n"
		"from xuat group by dot_nhap,hang) as t2\n"
		"order by t1.dn;"
		;
	static char * show_all=
		"select du_an,nx.dot_nhap,nx.hang,so_luong_n,don_gia_n,thue_n,chi_phi_phu_n,nx.dot_xuat,so_luong_x,don_gia_x,thue_x,chi_phi_phu_x from (select du_an,ms_dot_nhap as dot_nhap from dot_nhap) as dn natural left join (select nhap.hang,so_luong_n,don_gia_n,thue_n,chi_phi_phu_n,so_luong_x,don_gia_x,thue_x,chi_phi_phu_x,nhap.dot_nhap,xuat.dot_xuat from nhap left join xuat on nhap.dot_nhap=xuat.dot_nhap and nhap.hang=xuat.hang) as nx natural left join (select ms_dot_xuat as dot_xuat from dot_xuat) as dx;";

	switch(m_tabID)
	{
	case TAB_TON_ID:
		if(m_pKmySql->query(tinh_ton_q))
		{
			m_pKmySql->store_result();
			show_result();
		}	
		break;
	case TAB_ALL_ID:
		if(m_pKmySql->query(show_all))
		{
			m_pKmySql->store_result();
			show_result();
		}	
		break;
	}
	
	*pResult = 0;
}
