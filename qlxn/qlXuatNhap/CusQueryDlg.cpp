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
{

}

CCusQueryDlg::~CCusQueryDlg()
{
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

int CCusQueryDlg::create(CTabCtrl *pTab, KMySQL *pKmysql, CRect &rect)
{
	m_pKmySql=pKmysql;
	Create(IDD,pTab);
	MoveWindow(rect);
	return 0;
}

void CCusQueryDlg::OnBnClickedQueryBtn()
{
	// TODO: Add your control notification handler code here
	TCHAR temp[MAX_PATH];
	ULONG n;
	m_queryEdt.GetWindowText(temp,MAX_PATH);
	if(m_pKmySql->query(temp))
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


	num_fields = mysql_num_fields(m_pKmySql->get_res());
	
	for (int i=0; i<num_fields; i++)
		m_resList.InsertColumn(i,_T(""),LVCFMT_LEFT,LVCF_MINWIDTH);

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
