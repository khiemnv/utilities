// DefineSampleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "KKclassify.h"
#include "DefineSampleDlg.h"


// CDefineSampleDlg dialog

IMPLEMENT_DYNAMIC(CDefineSampleDlg, CDialog)

CDefineSampleDlg::CDefineSampleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDefineSampleDlg::IDD, pParent)
{

}

CDefineSampleDlg::~CDefineSampleDlg()
{
}

void CDefineSampleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDefineSampleDlg, CDialog)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CDefineSampleDlg message handlers

BOOL CDefineSampleDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_conf.conf_file=_T(KKDEFINESAMPLE_CONF_FILE);
	m_conf.readConf();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDefineSampleDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	m_conf.writeConf();
	CDialog::OnClose();
}
