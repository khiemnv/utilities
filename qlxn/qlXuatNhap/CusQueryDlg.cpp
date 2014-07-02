// CusQueryDlg.cpp : implementation file
//

#include "stdafx.h"
#include "qlXuatNhap.h"
#include "CusQueryDlg.h"


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
	ON_EN_SETFOCUS(IDC_QUERY_EDT, &CCusQueryDlg::OnEnSetfocusQueryEdt)
	ON_WM_LBUTTONDOWN()
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
	return 0;
}

void CCusQueryDlg::OnBnClickedQueryBtn()
{
	// TODO: Add your control notification handler code here
	m_queryEdt.GetWindowText(m_buffer,CKMYSQL_BUFF_LENGTH);
	if(m_pKmySql->query(m_buffer))
	{
		m_pKmySql->store_result();
		show_on_list();
	}	
}

void CCusQueryDlg::show_on_list(void)
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
	//AfxMessageBox(_T("query edit get focus"));
	//m_queryEdt.SetHighlight(0,m_queryEdt.GetWindowTextLength());
}

void CCusQueryDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
//	AfxMessageBox(_T("mouse left click"));
// 	if (ChildWindowFromPoint(point)==&m_queryEdt)
// 	{
// 		m_queryEdt.SetHighlight(0,m_queryEdt.GetWindowTextLength());;
// 	}
//	m_queryEdt.SetHighlight(0,m_queryEdt.GetWindowTextLength());;
	CDialog::OnLButtonDown(nFlags, point);
}
