// QLXuatNhapDlg.cpp : implementation file
//

#include "stdafx.h"
#include "QLXuatNhap.h"
#include "QLXuatNhapDlg.h"
/*#include "Kmysql.h"*/

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CQLXuatNhapDlg dialog




CQLXuatNhapDlg::CQLXuatNhapDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CQLXuatNhapDlg::IDD, pParent)
	, curTab(0)
	, curSel(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CQLXuatNhapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_MAIN, m_cTabMain);
}

BEGIN_MESSAGE_MAP(CQLXuatNhapDlg, CDialog)
//	ON_WM_PAINT()
//	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_MAIN, &CQLXuatNhapDlg::OnTcnSelchangeTabMain)
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_COMMAND(ID_FILE_EXIT, &CQLXuatNhapDlg::OnFileExit)
	ON_COMMAND(ID_OPTION_ADDMODE, &CQLXuatNhapDlg::OnOptionAddmode)
	ON_COMMAND(ID_OPTION_EDITMODE, &CQLXuatNhapDlg::OnOptionEditmode)
	ON_COMMAND(ID_OPTION_REMOVEMODE, &CQLXuatNhapDlg::OnOptionRemovemode)
END_MESSAGE_MAP()


// CQLXuatNhapDlg message handlers

#pragma comment (lib, "libmysql.lib")
BOOL CQLXuatNhapDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

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
	/*	init_database();*/
	initTabs();	
	/*m_cNhapTab->qlxn=this;*/

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CQLXuatNhapDlg::OnPaint()
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
//HCURSOR CQLXuatNhapDlg::OnQueryDragIcon()
//{
//	return static_cast<HCURSOR>(m_hIcon);
//}


#define DOT_TAB_ORDER		0
#define TEN_HANG_TAB_ORDER	1
#define NHAP_TAB_ORDER		2
#define XUAT_TAB_ORDER		3
#define TON_TAB_ORDER		4
#define ALL_TAB_ORDER		5

//create tabs
void CQLXuatNhapDlg::initTabs(void)
{
	//set comment static cho cac tab dlg
	//comment tab dot dlg
// 	m_cDotTab->m_vStaticDotNhap=_T("Đợt nhập");
// 	m_cDotTab->m_vStaticComment=_T("Ghi chú");
// 	m_cDotTab->m_vStaticNgayNhap=_T("Ngày nhập");
// 	m_cDotTab->m_vStaticChungTu=_T("Mã số chứng từ");
// 	//comment tab ten hang
// 	m_cTenHangTab->m_vStaticTenHangBrif=_T("Mã số hàng");
// 	m_cTenHangTab->m_vStaticTenHangFull=_T("Tên hàng");
// 	//commnet tab nhap
// 	m_cNhapTab->m_vStaticDonGia=_T("Đơn giá");
// 	m_cNhapTab->m_vStaticDotNhap=_T("Đợt nhập");
// 	m_cNhapTab->m_vStaticSoLuongCay=_T("Số lượng (cây)");
// 	m_cNhapTab->m_vStaticSoLuongKg=_T("Số lượng (kg)");
// 	m_cNhapTab->m_vStaticTenHang=_T("Tên hàng");
// 	m_cNhapTab->m_vStaticThanhTienCT=_T("Thành tiền chưa thuế");
// 	m_cNhapTab->m_vStaticThueVAT=_T("Thuế VAT");
// 	//comment tap ton
// 	m_cTonTab->m_vStaticDonGia=_T("Đơn giá (đ/kg)");
// 	m_cTonTab->m_vStaticDotNhap=_T("Đợt nhập");
// 	m_cTonTab->m_vStaticSoLuongCay=_T("Số lượng (cây)");
// 	m_cTonTab->m_vStaticSoLuongKg=_T("Số lượng (kg)");
// 	m_cTonTab->m_vStaticTenHang=_T("Tên hàng");
// 	m_cTonTab->m_vStaticThanhTienCT=_T("Thành tiền chưa thuế");
// 	//comment tap xuat
// 	m_cXuatTab->m_vStaticThanhTienCT=_T("Thành tiều chưa thuế");
// 	m_cXuatTab->m_vStaticThueVAT=_T("Thuế VAT");
// 	m_cXuatTab->m_vStaticTenHang=_T("Tên hàng");
// 	m_cXuatTab->m_vStaticSoLuongKg=_T("Số lượng (kg)");
// 	m_cXuatTab->m_vStaticSoLuongCay=_T("Số lượng (cây)");
// 	m_cXuatTab->m_vStaticDotNhap=_T("Đợt nhập");
// 	m_cXuatTab->m_vStaticNgay=_T("Ngày");
// 	m_cXuatTab->m_vStaticDonGia=_T("Đơn giá (đ/kg)");
	//commnet tabs all
// 	m_cAllTab->m_vStaticChungTu=_T("Mã số chứng từ");
// 	m_cAllTab->m_vStaticCommnet=_T("Ghi chú");
// 	m_cAllTab->m_vStaticDonGia=_T("Đơn giá");
// 	m_cAllTab->m_vStaticDotNhap=_T("Đợt nhập");
// 	m_cAllTab->m_vStaticNgayXuat=_T("Ngày Xuất");
// 	m_cAllTab->m_vStaticNgayNhap=_T("Ngày Nhập");
// 	m_cAllTab->m_vStaticNhap=_T("Nhập");
// 	m_cAllTab->m_vStaticSoLuongKgN=_T("Số lượng (kg)");
// 	m_cAllTab->m_vStaticSoLuongCayN=_T("Số lượng (cây)");
// 	m_cAllTab->m_vStaticSoLuongCayX=_T("Số lượng (cây)");
// 	m_cAllTab->m_vStaticSoLuongKgX=_T("Số lượng (kg))");
// 	m_cAllTab->m_vStaticTenHangBrif=_T("Mã số hàng");
// 	m_cAllTab->m_vStaticTenHangFull=_T("Tên hàng");
// 	m_cAllTab->m_vStaticThanhTienCT=_T("Thàng tiền chưa thuế");
// 	m_cAllTab->m_vStaticThue=_T("Thuế");
// 	m_cAllTab->m_vStaticXuat=_T("Xuất");

	m_cDotTab = new CAllDlg();
	m_cNhapTab = new CAllDlg();
	m_cXuatTab = new CAllDlg();
	m_cTonTab = new CAllDlg();
	m_cTenHangTab = new CAllDlg();
	m_cAllTab = new CAllDlg();

	//create tabs
	int retvl = m_cNhapTab->Create(m_cNhapTab->IDD,&m_cTabMain);
	retvl = m_cXuatTab->Create(m_cXuatTab->IDD,&m_cTabMain);
	retvl = m_cTonTab->Create(m_cTonTab->IDD,&m_cTabMain);
	retvl = m_cDotTab->Create(m_cDotTab->IDD,&m_cTabMain);
	retvl = m_cTenHangTab->Create(m_cTenHangTab->IDD,&m_cTabMain);
	retvl = m_cAllTab->Create(m_cAllTab->IDD,&m_cTabMain);

	//set ptr cKmysql to m_cKmySql, and tab_id
	m_cNhapTab->ptr_cKmysql=&m_cMySql;
	m_cNhapTab->tab_id=NHAP_TAB_ORDER;
	m_cNhapTab->set_comment();
	m_cNhapTab->set_active_edit();
	m_cNhapTab->create_list_cols();

	m_cXuatTab->ptr_cKmysql=&m_cMySql;
	m_cXuatTab->tab_id=XUAT_TAB_ORDER;
	m_cXuatTab->set_comment();
	m_cXuatTab->set_active_edit();
	m_cXuatTab->create_list_cols();

	m_cTonTab->ptr_cKmysql=&m_cMySql;
	m_cTonTab->tab_id=TON_TAB_ORDER;
	m_cTonTab->set_comment();
	m_cTonTab->set_active_edit();
	m_cTonTab->create_list_cols();

	m_cTenHangTab->ptr_cKmysql=&m_cMySql;
	m_cTenHangTab->tab_id=TEN_HANG_TAB_ORDER;
	m_cTenHangTab->set_comment();
	m_cTenHangTab->set_active_edit();
	m_cTenHangTab->create_list_cols();

	m_cDotTab->ptr_cKmysql=&m_cMySql;
	m_cDotTab->tab_id=DOT_TAB_ORDER;
	m_cDotTab->set_comment();
	m_cDotTab->set_active_edit();
	m_cDotTab->create_list_cols();

	m_cAllTab->ptr_cKmysql=&m_cMySql;
	m_cAllTab->tab_id=ALL_TAB_ORDER;
	m_cAllTab->set_comment();
	m_cAllTab->set_active_edit();
	m_cAllTab->create_list_cols();

	//dat comment cho tabs
	m_cTabMain.InsertItem(0,_T("Đợt"));
	m_cTabMain.InsertItem(1,_T("Tên hàng"));
	m_cTabMain.InsertItem(2,_T("Nhập"));
	m_cTabMain.InsertItem(3,_T("Xuất"));
	m_cTabMain.InsertItem(4,_T("Tồn"));
	m_cTabMain.InsertItem(5,_T("Tất cả"));

	//resize and show
	CRect tabRect;
	m_cTabMain.GetClientRect(&tabRect);
	m_cTabMain.AdjustRect(FALSE,&tabRect);

	m_cDotTab->MoveWindow(&tabRect);
	m_cNhapTab->MoveWindow(&tabRect);
	m_cXuatTab->MoveWindow(&tabRect);
	m_cTonTab->MoveWindow(&tabRect);
	m_cTenHangTab->MoveWindow(&tabRect);
	m_cAllTab->MoveWindow(&tabRect);

	//show and hide tabs
	m_cAllTab->ShowWindow(SW_SHOW);
	curTab=ALL_TAB_ORDER;
	m_cTabMain.SetCurFocus(curTab);

	//chose default option
	OnOptionAddmode();
}

// show and hide tabs when current tab sel change
void CQLXuatNhapDlg::showHideTabs(void)
{
	//hide cur tab
	switch(curTab)
	{
	case DOT_TAB_ORDER:
		m_cDotTab->ShowWindow(SW_HIDE);
		break;
	case TEN_HANG_TAB_ORDER:
		m_cTenHangTab->ShowWindow(SW_HIDE);
		break;
	case NHAP_TAB_ORDER:
		m_cNhapTab->ShowWindow(SW_HIDE);
		break;
	case XUAT_TAB_ORDER:
		m_cXuatTab->ShowWindow(SW_HIDE);
		break;
	case TON_TAB_ORDER:
		m_cTonTab->ShowWindow(SW_HIDE);
		break;
	case ALL_TAB_ORDER:
		m_cAllTab->ShowWindow(SW_HIDE);
		break;
	}
	//show coresponding tab
	switch(curSel)
	{
	case DOT_TAB_ORDER:
		m_cDotTab->ShowWindow(SW_SHOW);		
		break;
	case TEN_HANG_TAB_ORDER:
		m_cTenHangTab->ShowWindow(SW_SHOW);
		break;
	case NHAP_TAB_ORDER:
		m_cNhapTab->ShowWindow(SW_SHOW);
		break;
	case XUAT_TAB_ORDER:
		m_cXuatTab->ShowWindow(SW_SHOW);
		break;
	case TON_TAB_ORDER:
		m_cTonTab->ShowWindow(SW_SHOW);
		break;
	case ALL_TAB_ORDER:
		m_cAllTab->ShowWindow(SW_SHOW);
		break;
	}
	//set curTab = curSel
	curTab=curSel;
}

void CQLXuatNhapDlg::OnTcnSelchangeTabMain(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	curSel=m_cTabMain.GetCurSel();
	showHideTabs();
	*pResult = 0;
}

void CQLXuatNhapDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if (::IsWindow(m_cTabMain.GetSafeHwnd()))
	{
		resize(cx,cy);
	}

}

// resize all tabs
void CQLXuatNhapDlg::resize(void)
{
	CRect	newRect;

	GetClientRect(&newRect);

	AdjustWindowRect(&newRect,WS_GROUP|WS_POPUP|WS_SYSMENU,FALSE);

	//resize the main tab
	m_cTabMain.MoveWindow(&newRect);

	m_cTabMain.AdjustRect(FALSE,&newRect);

	//resize dot tab
	m_cDotTab->MoveWindow(&newRect);
	//resize ten hang tab
	m_cTenHangTab->MoveWindow(&newRect);
	//resize nhap tab
	m_cNhapTab->MoveWindow(&newRect);
	//resize xuat tab
	m_cXuatTab->MoveWindow(&newRect);
	//resize ton tab
	m_cTonTab->MoveWindow(&newRect);
}
#define MIN_MAIN_TAB_WIDTH	700
#define MIN_MAIN_TAB_HEIGHT	420

bool CQLXuatNhapDlg::resize(int cx, int cy)
{
// 	cy = cy > MIN_MAIN_TAB_HEIGHT ? cy : MIN_MAIN_TAB_HEIGHT;
// 	cx = cx > MIN_MAIN_TAB_WIDTH  ? cx : MIN_MAIN_TAB_WIDTH;
	if (cx<MIN_MAIN_TAB_WIDTH || cy<MIN_MAIN_TAB_HEIGHT)
		return	false;

	CRect	newRect(0,0,cx,cy);

	AdjustWindowRect(&newRect,WS_GROUP|WS_POPUP|WS_SYSMENU,FALSE);

	//resize the main tab
	m_cTabMain.MoveWindow(&newRect);

	m_cTabMain.AdjustRect(FALSE,&newRect);

	//resize dot tab
	m_cDotTab->MoveWindow(&newRect);
	//resize ten hang tab
	m_cTenHangTab->MoveWindow(&newRect);
	//resize nhap tab
	m_cNhapTab->MoveWindow(&newRect);
	//resize xuat tab
	m_cXuatTab->MoveWindow(&newRect);
	//resize ton tab
	m_cTonTab->MoveWindow(&newRect);
	//resize all tab
	m_cAllTab->MoveWindow(&newRect);
	return false;
}

void CQLXuatNhapDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	m_cMySql.close();

	CDialog::OnClose();
}

void CQLXuatNhapDlg::show_error_msgbox(CString error_msg)
{
	AfxMessageBox(error_msg,MB_OK,0);
}

#define CONF_FILE	"config.txt"
#define COMMENT_LEN 12
bool CQLXuatNhapDlg::init_database(void)
{
	FILE *f;
	char temp[MAX_PATH];
	CString SERVER_NAME;
	CString DB_USER;
	CString DB_USERPASS;
	CString DB_NAME;
	int		SERVER_PORT;
	CStringArray	tables;
	CString *queries;
	int		n_table=0,
			n_query,
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
		fscanf_s(f,"%s",temp,MAX_PATH);
		SERVER_NAME=CString(temp+COMMENT_LEN);
		fscanf_s(f,"%s",temp,MAX_PATH);
		DB_USER=CString(temp+COMMENT_LEN);
		fscanf_s(f,"%s",temp,MAX_PATH);
		DB_USERPASS=CString(temp+COMMENT_LEN);
		fscanf_s(f,"%s",temp,MAX_PATH);
		DB_NAME=CString(temp+COMMENT_LEN);
		fscanf_s(f,"%s %d",temp,MAX_PATH,&SERVER_PORT);

		fscanf_s(f,"%s %d",temp,MAX_PATH,&n_table);
		tables.RemoveAll();
		queries=new CString[n_table];

		for(i=0;i<n_table;i++)
		{
			fscanf_s(f,"%s",temp,MAX_PATH);
			tables.Add(CString(temp+COMMENT_LEN));			
		}

		fscanf_s(f,"%s %d",temp,MAX_PATH,&n_query);

		for(i=0;i<n_query;i++)
		{
			fscanf_s(f,"%s",temp,MAX_PATH);
			queries[i]=CString(temp);
			queries[i].Replace('*',' ');
		}

		fclose(f);
		//init to query data base
		m_cMySql=KMySQL(
			SERVER_NAME,
			DB_USER,
			DB_USERPASS,
			SERVER_PORT,
			DB_NAME
			);

		//connect to server (chua chon database dau)
		if(!m_cMySql.connect())
		{
			show_error_msgbox(CString(m_cMySql.get_last_error()));
			return false;
		}
	
		//select db if it not exit, thi create no
		if(!m_cMySql.select_database())
		{
			//show_error_msgbox(m_cMySql.get_last_error());
			if(!m_cMySql.create_database())
			{
				show_error_msgbox(CString(m_cMySql.get_last_error()));
				/*fclose(f);*/
				return false;
			}				
			if(!m_cMySql.select_database())
			{
				show_error_msgbox(CString(m_cMySql.get_last_error()));
				/*fclose(f);*/
				return false;
			}
		}

		//if tables not exist create
		if(!m_cMySql.query_and_store("show tables;"))
		{
			show_error_msgbox(CString(m_cMySql.get_last_error()));
			/*fclose(f);*/
			return false;
		}
		else
		{
			i = (int) m_cMySql.get_row_count();
			if(i!=n_table)
			{
				for (i=0;i<n_query;i++)
					m_cMySql.query(queries[i]);
			}
		}


		/*fclose(f);*/
		/*m_cMySql.close();*/
		return true;
	}
}

void CQLXuatNhapDlg::OnFileExit()
{
	// TODO: Add your command handler code here
	SendMessage(WM_CLOSE);
}

void CQLXuatNhapDlg::OnOptionAddmode()
{
	// TODO: Add your command handler code here
	CMenu *cMenu=GetMenu();

	cMenu->EnableMenuItem(ID_OPTION_ADDMODE,MF_GRAYED);
	cMenu->EnableMenuItem(ID_OPTION_EDITMODE,MF_ENABLED);
	cMenu->EnableMenuItem(ID_OPTION_REMOVEMODE,MF_ENABLED);
	
	change_tabsMode(ADD_MODE);
}

void CQLXuatNhapDlg::OnOptionEditmode()
{
	// TODO: Add your command handler code here
	CMenu *cMenu=GetMenu();

	cMenu->EnableMenuItem(ID_OPTION_ADDMODE,MF_ENABLED);
	cMenu->EnableMenuItem(ID_OPTION_EDITMODE,MF_GRAYED);
	cMenu->EnableMenuItem(ID_OPTION_REMOVEMODE,MF_ENABLED);	

	change_tabsMode(EDIT_MODE);
}

void CQLXuatNhapDlg::OnOptionRemovemode()
{
	// TODO: Add your command handler code here
	CMenu *cMenu=GetMenu();

	cMenu->EnableMenuItem(ID_OPTION_ADDMODE,MF_ENABLED);
	cMenu->EnableMenuItem(ID_OPTION_EDITMODE,MF_ENABLED);
	cMenu->EnableMenuItem(ID_OPTION_REMOVEMODE,MF_GRAYED);	

	change_tabsMode(REMOVE_MODE);
}

void CQLXuatNhapDlg::change_tabsMode(int mode)
{
	m_cNhapTab->change_mode(mode);
	m_cDotTab->change_mode(mode);
	m_cXuatTab->change_mode(mode);
	m_cTenHangTab->change_mode(mode);
}
