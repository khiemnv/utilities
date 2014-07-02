// renameDlg.h : header file
//

#if !defined(AFX_RENAMEDLG_H__6FF7EFAD_9929_4737_A123_019ADC70A930__INCLUDED_)
#define AFX_RENAMEDLG_H__6FF7EFAD_9929_4737_A123_019ADC70A930__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RenameOptDlg.h"
#include "RenameAction.h"	// Added by ClassView
#include "afxwin.h"
#include "kkdir.h"
class CRenameDlgAutoProxy;

/////////////////////////////////////////////////////////////////////////////
// CRenameDlg dialog

class CRenameDlg : public CDialog
{
	DECLARE_DYNAMIC(CRenameDlg);
	friend class CRenameDlgAutoProxy;

// Construction
public:
	void update_list_file();
	void add_item(int , int , CString );
	void create_col();
/*	CString dir_path;*/
	BOOL get_folder();
	CRenameAction ren_action;
	CRenameDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CRenameDlg();

// Dialog Data
	//{{AFX_DATA(CRenameDlg)
	enum { IDD = IDD_RENAME_DIALOG };
	CListCtrl	m_cListFile;
	CEdit	m_cEditPath;
	CString	m_vEditPath;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRenameDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CRenameDlgAutoProxy* m_pAutoProxy;
	HICON m_hIcon;

	BOOL CanExit();

	// Generated message map functions
	//{{AFX_MSG(CRenameDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnOptRename();
	afx_msg void OnButtonClose();
	afx_msg void OnButtonPath();
	afx_msg void OnChangeEditPath();
	afx_msg void OnButtonRename();
	afx_msg void OnClickListFile(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void resize(void);
	CStatic m_cStaticPath;
	CButton m_cBtnPath;
	CButton m_cBtnRename;
	CButton m_cBtnClose;
	afx_msg void OnStnClickedStaticPath();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	void refresh(void);
	CKKDir m_cPickedDir;
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnNMDblclkListFile(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnOptionRefresh();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RENAMEDLG_H__6FF7EFAD_9929_4737_A123_019ADC70A930__INCLUDED_)
