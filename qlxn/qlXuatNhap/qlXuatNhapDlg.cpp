// qlXuatNhapDlg.cpp : implementation file
//

#include "stdafx.h"
#include "qlXuatNhap.h"
#include "qlXuatNhapDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CqlXuatNhapDlg dialog




CqlXuatNhapDlg::CqlXuatNhapDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CqlXuatNhapDlg::IDD, pParent)
	, curTabSel(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CqlXuatNhapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MAIN_TAB, m_mainTab);
}

BEGIN_MESSAGE_MAP(CqlXuatNhapDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_NOTIFY(TCN_SELCHANGE, IDC_MAIN_TAB, &CqlXuatNhapDlg::OnTcnSelchangeMainTab)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CqlXuatNhapDlg message handlers

BOOL CqlXuatNhapDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	if(!init_database())
	{
		AfxMessageBox(CString("mysql chua chay"));
		EndDialog(1);
		return FALSE;
	}

	//dat comment cho tabs 
	//phai lam truoc thi adjustrect moi dc
	m_mainTab.InsertItem(0,_T("Đợt"));
	m_mainTab.InsertItem(1,_T("Tên hàng"));
	m_mainTab.InsertItem(2,_T("Nhập"));
	m_mainTab.InsertItem(3,_T("Xuất"));
	m_mainTab.InsertItem(4,_T("Tồn"));
	m_mainTab.InsertItem(5,_T("Khác"));

	m_dotTab.tabID=TAB_DOT_ID;
	m_dotTab.m_pKmySql=&m_kmysql;
	m_dotTab.Create(m_dotTab.IDD,&m_mainTab);	

	CRect tabRect;
	m_mainTab.GetClientRect(&tabRect);
	m_mainTab.AdjustRect(FALSE,&tabRect);

	m_hangTab.create(&m_mainTab,&m_kmysql,TAB_HANG_ID,tabRect);
	m_nhapTab.create(&m_mainTab,&m_kmysql,TAB_NHAP_ID,tabRect);
	m_xuatTab.create(&m_mainTab,&m_kmysql,TAB_XUAT_ID,tabRect);
	m_tonTab.create(&m_mainTab,&m_kmysql,TAB_TON_ID,tabRect);
	m_cusQTab.create(&m_mainTab,&m_kmysql,tabRect);

	
//resize and show
 	m_dotTab.MoveWindow(&tabRect);

	m_dotTab.ShowWindow(SW_SHOW);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CqlXuatNhapDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CqlXuatNhapDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CqlXuatNhapDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CqlXuatNhapDlg::OnTcnSelchangeMainTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	
	switch(curTabSel)
	{
	case 0:
		m_dotTab.ShowWindow(SW_HIDE);
		break;
	case 1:
		m_hangTab.ShowWindow(SW_HIDE);
		break;
	case TAB_NHAP_ID:
		m_nhapTab.ShowWindow(SW_HIDE);
		break;
	case TAB_XUAT_ID:
		m_xuatTab.ShowWindow(SW_HIDE);
		break;
	case TAB_TON_ID:
		m_tonTab.ShowWindow(SW_HIDE);
		break;
	case 5:
		m_cusQTab.ShowWindow(SW_HIDE);
		break;
	}

	curTabSel=m_mainTab.GetCurSel();

	switch(curTabSel)
	{
	case 0:
		m_dotTab.ShowWindow(SW_SHOW);
		break;
	case 1:
		m_hangTab.ShowWindow(SW_SHOW);
		break;
	case TAB_NHAP_ID:
		m_nhapTab.ShowWindow(SW_SHOW);
		break;
	case TAB_XUAT_ID:
		m_xuatTab.ShowWindow(SW_SHOW);
		break;
	case TAB_TON_ID:
		m_tonTab.ShowWindow(SW_SHOW);
		break;
	case 5:
		m_cusQTab.ShowWindow(SW_SHOW);
		break;
	}
	*pResult = 0;
}

void CqlXuatNhapDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if (::IsWindow(m_mainTab.GetSafeHwnd()))
	{
		resize();
	}
}

int CqlXuatNhapDlg::resize(void)
{
	CRect	newRect;

	GetClientRect(&newRect);

	AdjustWindowRect(&newRect,WS_GROUP|WS_POPUP|WS_SYSMENU,FALSE);

	//resize the main tab
	m_mainTab.MoveWindow(&newRect);

	m_mainTab.AdjustRect(FALSE,&newRect);

	//resize dot tab
	m_dotTab.MoveWindow(&newRect);
	m_hangTab.MoveWindow(&newRect);
	m_nhapTab.MoveWindow(&newRect);
	m_xuatTab.MoveWindow(&newRect);
	m_tonTab.MoveWindow(&newRect);
	m_cusQTab.MoveWindow(&newRect);
	return 0;
}

bool CqlXuatNhapDlg::init_database(void)
{
	FILE *f;
	char temp[MAX_PATH];
	CString SERVER_NAME;
	CString DB_USER;
	CString DB_USERPASS;
	CString DB_NAME;
	int		SERVER_PORT;
	CStringArray	tables,q_create_tbl,q_alter_tbl;


	int		n_table=0,n_QCreateTbl,
		n_QAlterTbl,
		i=0;

	fopen_s(&f,CONF_FILE,"rt");
	if (f==NULL)
	{
		AfxMessageBox(_T("không thấy file conf.txt!"));
		/*SendMessage(WM_CLOSE);*/
		return false;
	}
	else
	{
		//read config file
		//database info
		fscanf_s(f,"%s",temp,MAX_PATH);
		SERVER_NAME=CString(temp+COMMENT_LEN);
		fscanf_s(f,"%s",temp,MAX_PATH);
		DB_USER=CString(temp+COMMENT_LEN);
		fscanf_s(f,"%s",temp,MAX_PATH);
		DB_USERPASS=CString(temp+COMMENT_LEN);
		fscanf_s(f,"%s",temp,MAX_PATH);
		DB_NAME=CString(temp+COMMENT_LEN);
		fscanf_s(f,"%s %d",temp,MAX_PATH,&SERVER_PORT);
		//read table list
		fscanf_s(f,"%s %d",temp,MAX_PATH,&n_table);
		tables.RemoveAll();

		for(i=0;i<n_table;i++)
		{
			fscanf_s(f,"%s",temp,MAX_PATH);
			tables.Add(CString(temp+COMMENT_LEN));			
		}

		q_alter_tbl.RemoveAll();
		q_create_tbl.RemoveAll();
		//read query to create table
		fscanf_s(f,"%s %d",temp,MAX_PATH,&n_QCreateTbl);

		for(i=0;i<n_QCreateTbl;i++)
		{
			fscanf_s(f,"%s",temp,MAX_PATH);
			q_create_tbl.Add(CString(temp));
			q_create_tbl[i].Replace('*',' ');
		}
		//read query to alter table
		fscanf_s(f,"%s %d",temp,MAX_PATH,&n_QAlterTbl);

		for(i=0;i<n_QAlterTbl;i++)
		{
			fscanf_s(f,"%s",temp,MAX_PATH);
			q_alter_tbl.Add(CString(temp));
			q_alter_tbl[i].Replace('*',' ');
		}

		fclose(f);
		//init to query data base
		m_kmysql=KMySQL(
			SERVER_NAME,
			DB_USER,
			DB_USERPASS,
			SERVER_PORT,
			DB_NAME
			);

		//connect to server (chua chon database dau)
		if(!m_kmysql.connect())
		{
			AfxMessageBox(CString(m_kmysql.get_last_error()));
			return false;
		}

		//select db if it not exit, thi create no
		if(!m_kmysql.select_database())
		{
			//show_error_msgbox(m_kmysql.get_last_error());
			if(!m_kmysql.create_database())
			{
				AfxMessageBox(CString(m_kmysql.get_last_error()));
				/*fclose(f);*/
				return false;
			}				
			if(!m_kmysql.select_database())
			{
				AfxMessageBox(CString(m_kmysql.get_last_error()));
				/*fclose(f);*/
				return false;
			}
		}

		//if tables not exist create
		if(!m_kmysql.query_and_store("show tables;"))
		{
			AfxMessageBox(CString(m_kmysql.get_last_error()));
			/*fclose(f);*/
			return false;
		}
		else
		{
			i = (int) m_kmysql.get_row_count();
			if(i!=n_table)
			{
				for (i=0;i<n_QCreateTbl;i++)
					m_kmysql.query(q_create_tbl[i]);
				for(i=0;i<n_QAlterTbl;i++)
					m_kmysql.query(q_alter_tbl[i]);
			}
		}

		q_alter_tbl.RemoveAll();
		q_create_tbl.RemoveAll();
		return true;
	}
}
