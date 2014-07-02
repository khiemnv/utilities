// SettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "scheduler.h"
#include "SettingDlg.h"

#define KS_SETTING_CANCEL	1
#define KS_SETTING_SAVE		0

// CSettingDlg dialog

IMPLEMENT_DYNAMIC(CSettingDlg, CDialog)

CSettingDlg::CSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSettingDlg::IDD, pParent)
	, m_pKSSetting(NULL)
{

}

CSettingDlg::~CSettingDlg()
{
}

void CSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ALERT_BEFORE, m_alertBeforeTime);
	DDX_Control(pDX, IDC_ALERTBEFORE_CHK, m_alertBeforeChk);
}


BEGIN_MESSAGE_MAP(CSettingDlg, CDialog)
	ON_BN_CLICKED(IDC_SAVE_BTN, &CSettingDlg::OnBnClickedSaveBtn)
	ON_BN_CLICKED(IDC_CANCEL_BTN, &CSettingDlg::OnBnClickedCancelBtn)
//	ON_BN_DOUBLECLICKED(IDC_ALERTBEFORE_CHK, &CSettingDlg::OnBnDoubleclickedAlertbeforeChk)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


// CSettingDlg message handlers

void CSettingDlg::OnBnClickedSaveBtn()
{
	// TODO: Add your control notification handler code here
	COleDateTime temptime;
	BOOL tempchk;
	CWinApp* m_app = AfxGetApp();
	CString timestr;

	if (IsDlgButtonChecked(IDC_ALERTBEFORE_CHK))
		tempchk=BST_CHECKED;
	else
		tempchk=BST_UNCHECKED;

	m_alertBeforeTime.GetTime(temptime);
	m_pKSSetting->alertBeforeTime=temptime;
	//temptime.SetTime(0,0,0);
	timestr=temptime.Format(_T("%m/%d/%Y  %H:%M:%S"));
	m_app->WriteProfileString(_T("ks_setting"),_T("alert_before_time"),timestr);

	m_pKSSetting->alertBeforeChk=tempchk;
	m_app->WriteProfileInt(_T("ks_setting"),_T("alert_before_chk"),tempchk);

	EndDialog(KS_SETTING_SAVE); 
}

void CSettingDlg::OnBnClickedCancelBtn()
{
	// TODO: Add your control notification handler code here
	EndDialog(KS_SETTING_CANCEL);
}

BOOL CSettingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	COleDateTime temptime;

	// TODO:  Add extra initialization here
	m_alertBeforeTime.SetFormat(_T("HH:mm:ss"));

	m_alertBeforeTime.SetTime(m_pKSSetting->alertBeforeTime);
	m_alertBeforeChk.SetCheck(m_pKSSetting->alertBeforeChk);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

//void CSettingDlg::OnBnDoubleclickedAlertbeforeChk()
//{
//	// TODO: Add your control notification handler code here
//}

void CSettingDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	static BOOL dbclk = 1;
	COleDateTime temptime;

	dbclk ^= 1;
	if(dbclk)
		m_alertBeforeTime.SetTime(m_pKSSetting->alertBeforeTime);
	else
	{
		temptime.SetTime(0,0,0);
		m_alertBeforeTime.SetTime(temptime);
	}

	CDialog::OnLButtonDblClk(nFlags, point);
}
