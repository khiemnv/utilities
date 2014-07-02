// KKclassifyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "KKclassify.h"
#include "KKclassifyDlg.h"

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


// CKKclassifyDlg dialog




CKKclassifyDlg::CKKclassifyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKKclassifyDlg::IDD, pParent)
	, presel(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CKKclassifyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MAIN_TAB, m_cMainTab);
}

BEGIN_MESSAGE_MAP(CKKclassifyDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_NOTIFY(TCN_SELCHANGE, IDC_MAIN_TAB, &CKKclassifyDlg::OnTcnSelchangeMainTab)
	ON_WM_SIZE()
	ON_COMMAND(ID_FILE_EXIT, &CKKclassifyDlg::OnFileExit)
	ON_WM_CLOSE()
	ON_COMMAND(ID_CONFIG_DEFINESAMPLE, &CKKclassifyDlg::OnConfigDefinesample)
END_MESSAGE_MAP()


// CKKclassifyDlg message handlers

BOOL CKKclassifyDlg::OnInitDialog()
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
	
	createTab();

	resize();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CKKclassifyDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CKKclassifyDlg::OnPaint()
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
HCURSOR CKKclassifyDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CKKclassifyDlg::OnTcnSelchangeMainTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	int cursel = m_cMainTab.GetCurSel();
	
	switch(presel)
	{
		case CLASSIFY_TAB_ID: m_classifyTab.ShowWindow(SW_HIDE);
			break;
//		case 1: m_dfsample.ShowWindow(SW_HIDE);
//			break;
		case PEINFO_TAB_ID:
			m_peInfoTab.ShowWindow(SW_HIDE);
			break;
	}

	presel = cursel;

	switch (cursel)
	{
		case CLASSIFY_TAB_ID: m_classifyTab.ShowWindow(SW_SHOW);
			break;
//		case 1: m_dfsample.ShowWindow(SW_SHOW);
//			break;
		case PEINFO_TAB_ID:
			m_peInfoTab.ShowWindow(SW_SHOW);
			break;
	}
	*pResult = 0;
}

void CKKclassifyDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here

	resize(cx,cy);
}

bool CKKclassifyDlg::resize(int cx, int cy)
{
	CRect newRect=CRect(0,0,cx,cy);

	if(!IsWindow(m_cMainTab.GetSafeHwnd()))	
		return false;	
	
	AdjustWindowRect(&newRect,WS_GROUP|WS_POPUP|WS_SYSMENU,FALSE);
	m_cMainTab.MoveWindow(&newRect);

	m_cMainTab.AdjustRect(FALSE,&newRect);
	m_classifyTab.MoveWindow(&newRect);

//	m_dfsample.MoveWindow(newRect);
	m_peInfoTab.MoveWindow(&newRect);
	return true;
}

int CKKclassifyDlg::createTab(void)
{
	m_cMainTab.InsertItem(CLASSIFY_TAB_ID,_T("Classify"));
	m_cMainTab.InsertItem(PEINFO_TAB_ID,_T("PE Info"));

	CTabCtrl* pMTab=&m_cMainTab;

	m_classifyTab.Create(m_classifyTab.IDD,&m_cMainTab);
	m_classifyTab.ShowWindow(SW_SHOW);
	m_peInfoTab.Create(m_peInfoTab.IDD,&m_cMainTab);

	CRect tabrect;
	m_cMainTab.GetClientRect(&tabrect);
	m_cMainTab.AdjustRect(FALSE,&tabrect);
	m_peInfoTab.MoveWindow(&tabrect);
	m_classifyTab.MoveWindow(&tabrect);

	//setTabTitle(0,_T("bab"));
	return 0;
}

void CKKclassifyDlg::OnFileExit()
{
	// TODO: Add your command handler code here
	SendMessage(WM_CLOSE);
}

void CKKclassifyDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	//m_cClassify.config.updateCtrl(KKGET_DLG_VALUE);
	//m_cClassify.config.writeConf();

	//m_dfsample.m_conf.writeConf();
	//m_cClassify.config.saveConf(pConfig);
	closeTab();
	CDialog::OnClose();
}
BOOL CKKClassifyConf::loadConf(CKKClassifyConf* pClassifyConf)
{
	if(pClassifyConf==NULL)
		return FALSE;
	path=pClassifyConf->path;
	SQTstates.Copy(pClassifyConf->SQTstates);
	classifySpecies.Copy(pClassifyConf->classifySpecies);
	return TRUE;
}
BOOL CKKClassifyConf::saveConf(CKKClassifyConf* pClassifyConf)
{
	if(pClassifyConf==NULL)
		return FALSE;
	pClassifyConf->path=path;
	pClassifyConf->SQTstates.Copy(SQTstates);
	pClassifyConf->classifySpecies.Copy(classifySpecies);
	return TRUE;
}

bool CKKclassifyDlg::resize()
{
	CRect nrect;
	GetClientRect(&nrect);
	return resize(nrect.Width(),nrect.Height());
}
void CKKclassifyDlg::OnConfigDefinesample()
{
	// TODO: Add your command handler code here
	m_dfsample.DoModal();
}

void CKKclassifyDlg::closeTab(void)
{
	m_classifyTab.SendMessage(WM_CLOSE);
	m_peInfoTab.SendMessage(WM_CLOSE);
	//m_dfsample.CloseWindow();
}

CString CKKclassifyDlg::setTabTitle(int tabID,CString newTitle)
{
	TCITEM	tcitem;
	TCHAR temp[MAX_PATH];
	tcitem.pszText=temp;

	m_cMainTab.GetItem(tabID,&tcitem);
	tcitem.mask=tcitem.mask|TCIF_TEXT;
	tcitem.pszText=newTitle.GetBuffer();
	m_cMainTab.SetItem(tabID,&tcitem);
	return CString();
}
