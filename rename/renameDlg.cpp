// renameDlg.cpp : implementation file
//

#include "stdafx.h"
#include "rename.h"
#include "renameDlg.h"
#include "DlgProxy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRenameDlg dialog

IMPLEMENT_DYNAMIC(CRenameDlg, CDialog);

CRenameDlg::CRenameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRenameDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRenameDlg)
	m_vEditPath = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pAutoProxy = NULL;
}

CRenameDlg::~CRenameDlg()
{
	// If there is an automation proxy for this dialog, set
	//  its back pointer to this dialog to NULL, so it knows
	//  the dialog has been deleted.
	if (m_pAutoProxy != NULL)
		m_pAutoProxy->m_pDialog = NULL;
}

void CRenameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRenameDlg)
	DDX_Control(pDX, IDC_LIST_FILE, m_cListFile);
	DDX_Control(pDX, IDC_EDIT_PATH, m_cEditPath);
	DDX_Text(pDX, IDC_EDIT_PATH, m_vEditPath);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_STATIC_PATH, m_cStaticPath);
	DDX_Control(pDX, IDC_BUTTON_PATH, m_cBtnPath);
	DDX_Control(pDX, IDC_BUTTON_RENAME, m_cBtnRename);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_cBtnClose);
}

BEGIN_MESSAGE_MAP(CRenameDlg, CDialog)
	//{{AFX_MSG_MAP(CRenameDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(M_OPT_RENAME, OnOptRename)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, OnButtonClose)
	ON_BN_CLICKED(IDC_BUTTON_PATH, OnButtonPath)
	ON_EN_CHANGE(IDC_EDIT_PATH, OnChangeEditPath)
	ON_BN_CLICKED(IDC_BUTTON_RENAME, OnButtonRename)
	ON_NOTIFY(NM_CLICK, IDC_LIST_FILE, OnClickListFile)
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
	ON_STN_CLICKED(IDC_STATIC_PATH, &CRenameDlg::OnStnClickedStaticPath)
	ON_WM_DROPFILES()
	ON_WM_CHAR()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_FILE, &CRenameDlg::OnNMDblclkListFile)
	ON_COMMAND(ID_OPTION_REFRESH, &CRenameDlg::OnOptionRefresh)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRenameDlg message handlers

BOOL CRenameDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	ren_action.read_conf();
	m_cPickedDir.pick(ren_action.ren_info.dir_path);
//	m_cPickedDir.listFiles();
	
	//set list style
	m_cListFile.SetExtendedStyle(m_cListFile.GetStyle()|LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);

	create_col();
	m_cEditPath.SetWindowText(ren_action.ren_info.dir_path);
	resize();
	refresh();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRenameDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

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

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CRenameDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

// Automation servers should not exit when a user closes the UI
//  if a controller still holds on to one of its objects.  These
//  message handlers make sure that if the proxy is still in use,
//  then the UI is hidden but the dialog remains around if it
//  is dismissed.

//DEL void CRenameDlg::OnClose() 
//DEL {
//DEL 	EndDialog(0);
//DEL 		CDialog::OnClose();
//DEL }

//DEL void CRenameDlg::OnOK() 
//DEL {
//DEL 	
//DEL 		CDialog::OnOK();
//DEL }

//DEL void CRenameDlg::OnCancel() 
//DEL {
//DEL 	if (CanExit())
//DEL 		CDialog::OnCancel();
//DEL }

BOOL CRenameDlg::CanExit()
{
	// If the proxy object is still around, then the automation
	//  controller is still holding on to this application.  Leave
	//  the dialog around, but hide its UI.
	if (m_pAutoProxy != NULL)
	{
		ShowWindow(SW_HIDE);
		return FALSE;
	}

	return TRUE;
}

void CRenameDlg::OnOptRename() 
{
	// TODO: Add your command handler code here
	CRenameOptDlg ren_opt_dlg;
	ren_opt_dlg.p_reninfo=&ren_action.ren_info;

	ren_opt_dlg.DoModal();
}

//DEL void CRenameDlg::OnClose() 
//DEL {
//DEL 	// TODO: Add your control notification handler code here
//DEL 	
//DEL }

void CRenameDlg::OnButtonClose() 
{
	// TODO: Add your control notification handler code here
	ren_action.ren_info.dir_path=m_cPickedDir.getDir();
	ren_action.update_conf();
	EndDialog(0);	
}

BOOL CRenameDlg::get_folder()
{
	BROWSEINFO bi = { 0 };
    TCHAR path[MAX_PATH];
    bi.lpszTitle = _T("Pick a Directory";)
    bi.pszDisplayName = path;
    LPITEMIDLIST pidl = SHBrowseForFolder ( &bi );
    if ( pidl != 0 )
    {
        // get the name of the folder
        SHGetPathFromIDList(pidl,path);
		/*ren_action.ren_info.dir_path=path;*/
		m_cEditPath.SetWindowText(path);
		
        IMalloc * imalloc = 0;
        if ( SUCCEEDED( SHGetMalloc ( &imalloc )) )
        {
            imalloc->Free ( pidl );
            imalloc->Release ( );
        }
		
		return	TRUE;
    }
	return FALSE;
}

void CRenameDlg::OnButtonPath() 
{
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	if(m_cPickedDir.pick())
	{
		m_cEditPath.SetWindowText(m_cPickedDir.dirName);
		m_cPickedDir.listFiles();
		update_list_file();
	}
}

void CRenameDlg::create_col()
{
	m_cListFile.InsertColumn(0,_T(""),LVCFMT_LEFT,20,0);
	m_cListFile.InsertColumn(1,_T("Old Names"),LVCFMT_LEFT,100,0);
	m_cListFile.InsertColumn(2,_T("New Names"),LVCFMT_LEFT,100,0);
}

void CRenameDlg::add_item(int row, int col, CString content)
{
	LVITEM Item;
	
	Item.mask = LVIF_TEXT;	
	Item.iItem = row;	
	Item.iSubItem = col;	
	Item.pszText = content.GetBuffer(content.GetLength());

	if(col!=0)
		m_cListFile.SetItem(&Item);
	else
		m_cListFile.InsertItem(&Item);
}

void CRenameDlg::OnChangeEditPath() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here

	if (m_cEditPath.GetFocus()==&m_cEditPath)
	{
		m_cPickedDir.pick(&m_cEditPath);
		m_cPickedDir.listFiles();
		update_list_file();
	}	
}

void CRenameDlg::update_list_file()
{
	int i,row=0;
	char order[10];
	CString temp;

	//m_cPickedDir.listFiles();
	ren_action.list_file(m_cPickedDir);

	m_cListFile.DeleteAllItems();

	for(i=0;i<ren_action.ren_info.old_names.GetSize();i++)
	{	
		_itoa_s(i+1,order,10);
		add_item(row,0,order);
		add_item(row,1,ren_action.ren_info.old_names.GetAt(i));
		add_item(row++,2,ren_action.ren_info.new_names.GetAt(i));
	}
}

void CRenameDlg::OnButtonRename() 
{
	// TODO: Add your control notification handler code here
	ren_action.execute(m_cPickedDir);
}

void CRenameDlg::OnClickListFile(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here

	*pResult = 0;
}

void CRenameDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if(::IsWindow(m_cListFile.GetSafeHwnd()))
		resize();
}

void CRenameDlg::resize(void)
{
	CRect	rect;
			
	/*GetWindowRect(&rect);*/
	GetClientRect(&rect);

	int cy=rect.Height();	
	int cx=rect.Width();

	cy=cy;
	cx=cx;

	int x=5,y=5;
	int n_height=22;
	int n_width=25;

	m_cStaticPath.MoveWindow(x,y,n_width,n_height,TRUE);
	
	x = x + n_width + 5;
	n_width=30;	

	n_width = cx - x - 5 - n_width - 5;

	m_cEditPath.MoveWindow(x,y,n_width,n_height,TRUE);

	x = x + n_width +5;
	n_width = 30;

	m_cBtnPath.MoveWindow(x,y,n_width,n_height,TRUE);
 
	y = y + n_height + 5;
	x=5;

	n_width = cx - 10;
	n_height = cy - y - 5 - n_height -5;

	m_cListFile.MoveWindow(x,y,n_width,n_height,TRUE);
	m_cListFile.SetColumnWidth(0,20);
	n_width = n_width/2 - 12;
	m_cListFile.SetColumnWidth(1,n_width);
	m_cListFile.SetColumnWidth(2,n_width);

	n_width = 55;

	x = cx - n_width -5;
	y = y + n_height +5;
	n_height = 22;
	m_cBtnClose.MoveWindow(x,y,n_width,n_height,1);
	x = x - 10 - n_width;
	m_cBtnRename.MoveWindow(x,y,n_width,n_height,1);
}

void CRenameDlg::OnStnClickedStaticPath()
{
	// TODO: Add your control notification handler code here
}

void CRenameDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: Add your message handler code here and/or call default
	WCHAR	fileName[MAX_PATH];
	int count = DragQueryFileW(hDropInfo,-1,fileName,MAX_PATH);
	int i=0;

	POINT dragPoint;
	if(DragQueryPoint(hDropInfo,&dragPoint))
	{
		if(ChildWindowFromPoint(dragPoint)==&m_cEditPath)
		{
			m_cEditPath.SetFocus();
			DragQueryFile(hDropInfo,0,fileName,MAX_PATH);
			if(PathIsDirectory(fileName))
				m_cEditPath.SetWindowText(fileName);
			else
			{
				// 				CString path=fileName;
				// 				i=path.ReverseFind('\\');
				i=wcslen(fileName);
				for(;fileName[i]!='\\';i--)
					fileName[i]=0;
				m_cEditPath.SetWindowText(fileName);
			}
		}
		else
			for(i=0;i<count;i++)
			{
				DragQueryFileW(hDropInfo,i,fileName,MAX_PATH);
				m_cPickedDir.addFile(fileName);
			}
	}

	update_list_file();
	CDialog::OnDropFiles(hDropInfo);
}

void CRenameDlg::refresh(void)
{
	m_cPickedDir.listFiles();
	update_list_file();
}

void CRenameDlg::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	switch(nChar)
	{
	case VK_DELETE:
		AfxMessageBox(_T("key del press"));
		break;
	}

	CDialog::OnChar(nChar, nRepCnt, nFlags);
}

void CRenameDlg::OnNMDblclkListFile(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
		update_list_file();
	*pResult = 0;
}

void CRenameDlg::OnOptionRefresh()
{
	// TODO: Add your command handler code here
	refresh();
}
