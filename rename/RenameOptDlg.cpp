// RenameOptDlg.cpp : implementation file
//

#include "stdafx.h"
#include "rename.h"
#include "RenameOptDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRenameOptDlg dialog


CRenameOptDlg::CRenameOptDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRenameOptDlg::IDD, pParent)
	, m_vRemove_(FALSE)
	/*, m_vRemove_(FALSE)*/
{
	//{{AFX_DATA_INIT(CRenameOptDlg)
	m_vTail = _T("");
	m_vOldStr = _T("");
	m_vNewStr = _T("");
	m_vExeTails = _T("");
	m_vReplace = FALSE;
	m_vCheckET = FALSE;
	m_vAdd = -1;
	m_vCut = -1;
	//}}AFX_DATA_INIT
}


void CRenameOptDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRenameOptDlg)
	DDX_Text(pDX, IDC_EDIT_TAIL, m_vTail);
	DDX_Text(pDX, IDC_EDIT_OLDSTR, m_vOldStr);
	DDX_Text(pDX, IDC_EDIT_NEWSTR, m_vNewStr);
	DDX_Text(pDX, IDC_EDIT_EXETAILS, m_vExeTails);
	DDX_Check(pDX, IDC_CHECK_REPLACE, m_vReplace);
	DDX_Check(pDX, IDC_CHECK_EXETAILS, m_vCheckET);
	DDX_Radio(pDX, IDC_RADIO_ADD, m_vAdd);
	DDX_Radio(pDX, IDC_RADIO_CUT, m_vCut);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_CHECK_REMOVE_, m_vRemove_);
}


BEGIN_MESSAGE_MAP(CRenameOptDlg, CDialog)
	//{{AFX_MSG_MAP(CRenameOptDlg)
	ON_EN_CHANGE(IDC_EDIT_TAIL, OnChangeEditTail)
	ON_BN_CLICKED(IDC_RADIO_CUT, OnRadioCut)
	ON_BN_CLICKED(IDC_RADIO_ADD, OnRadioAdd)
	ON_BN_CLICKED(IDC_CHECK_REPLACE, OnCheckReplace)
	ON_EN_CHANGE(IDC_EDIT_OLDSTR, OnChangeEditOldstr)
	ON_EN_CHANGE(IDC_EDIT_NEWSTR, OnChangeEditNewstr)
	ON_BN_CLICKED(IDC_CHECK_EXETAILS, OnCheckExetails)
	ON_EN_CHANGE(IDC_EDIT_EXETAILS, OnChangeEditExetails)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnButtonCancel)
	//}}AFX_MSG_MAP
/*	ON_BN_CLICKED(IDC_CHECK1, &CRenameOptDlg::OnBnClickedCheck1)*/
	ON_BN_CLICKED(IDC_CHECK_REMOVE_, &CRenameOptDlg::OnBnClickedCheckRemove)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRenameOptDlg message handlers

BOOL CRenameOptDlg::update_reninfo()
{
	WCHAR temp[MAX_PATH];

	p_reninfo->add_cut=m_vAdd;
	p_reninfo->tail=m_vTail;
	p_reninfo->replace=m_vReplace;
	p_reninfo->old_str=m_vOldStr;
	p_reninfo->new_str=m_vNewStr;
	p_reninfo->ren_exe=m_vCheckET;
	p_reninfo->remove_=m_vRemove_;

	p_reninfo->exe_tails.RemoveAll();
	m_vExeTails.TrimLeft();
	while (!m_vExeTails.IsEmpty())
	{
		swscanf_s(m_vExeTails,_T("%s"),temp,MAX_PATH);
		p_reninfo->exe_tails.Add(temp);
		m_vExeTails.TrimLeft(temp);
		m_vExeTails.TrimLeft();
	}	

	return	TRUE;
}

//DEL void CRenameOptDlg::OnOK() 
//DEL {
//DEL 	// TODO: Add extra validation here
//DEL 	update_reninfo();
//DEL 	CDialog::OnOK();
//DEL }

BOOL CRenameOptDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	show_reninfo();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRenameOptDlg::OnChangeEditTail() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
}

void CRenameOptDlg::OnRadioCut() 
{
	// TODO: Add your control notification handler code here
	m_vCut=0;
	m_vAdd=1;
	UpdateData(FALSE);
}

void CRenameOptDlg::OnRadioAdd() 
{
	// TODO: Add your control notification handler code here
	m_vCut=1;
	m_vAdd=0;
	UpdateData(FALSE);
}

void CRenameOptDlg::OnCheckReplace() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
}

void CRenameOptDlg::OnChangeEditOldstr() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
}

void CRenameOptDlg::OnChangeEditNewstr() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
}

void CRenameOptDlg::OnCheckExetails() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
}

void CRenameOptDlg::OnChangeEditExetails() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
}

void CRenameOptDlg::OnButtonSave() 
{
	// TODO: Add your control notification handler code here
	update_reninfo();
	EndDialog(1);
}

void CRenameOptDlg::OnButtonCancel() 
{
	// TODO: Add your control notification handler code here
	EndDialog(0);
}

// void CRenameOptDlg::OnBnClickedCheck1()
// {
// 	// TODO: Add your control notification handler code here
// 	/*UpdateData(TRUE);*/
// }
// 
void CRenameOptDlg::OnBnClickedCheckRemove()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
}

void CRenameOptDlg::show_reninfo(void)
{
	int i;

	m_vAdd=p_reninfo->add_cut;
	m_vCut=!m_vAdd;
	m_vTail=p_reninfo->tail;

	m_vReplace=p_reninfo->replace;
	m_vOldStr=p_reninfo->old_str;
	m_vNewStr=p_reninfo->new_str;

	m_vCheckET=p_reninfo->ren_exe;

	m_vRemove_=p_reninfo->remove_;

	for(i=0; i< p_reninfo->exe_tails.GetSize();i++)
		m_vExeTails+=p_reninfo->exe_tails.GetAt(i)+' ';

	UpdateData(FALSE);
}
