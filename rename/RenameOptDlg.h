#if !defined(AFX_RENAMEOPTDLG_H__C5EA9802_49A2_4DDF_B02B_79724CF304DB__INCLUDED_)
#define AFX_RENAMEOPTDLG_H__C5EA9802_49A2_4DDF_B02B_79724CF304DB__INCLUDED_

#include "RenameAction.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RenameOptDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRenameOptDlg dialog

class CRenameOptDlg : public CDialog
{
// Construction
public:
	BOOL update_reninfo();
	RenameInfo *p_reninfo;
	CRenameOptDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRenameOptDlg)
	enum { IDD = IDD_RENOPT_DIALOG };
	CString	m_vTail;
	CString	m_vOldStr;
	CString	m_vNewStr;
	CString	m_vExeTails;
	BOOL	m_vReplace;
	BOOL	m_vCheckET;
	int		m_vAdd;
	int		m_vCut;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRenameOptDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRenameOptDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditTail();
	afx_msg void OnRadioCut();
	afx_msg void OnRadioAdd();
	afx_msg void OnCheckReplace();
	afx_msg void OnChangeEditOldstr();
	afx_msg void OnChangeEditNewstr();
	afx_msg void OnCheckExetails();
	afx_msg void OnChangeEditExetails();
	afx_msg void OnButtonSave();
	afx_msg void OnButtonCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	/*BOOL m_vRemove_;*/
	/*afx_msg void OnBnClickedCheck1();*/
	afx_msg void OnBnClickedCheckRemove();
private:
	void show_reninfo(void);
public:
	BOOL m_vRemove_;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RENAMEOPTDLG_H__C5EA9802_49A2_4DDF_B02B_79724CF304DB__INCLUDED_)
