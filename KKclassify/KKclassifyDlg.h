// KKclassifyDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "classifydlg.h"
#include "definesampledlg.h"
#include "kkpeinfo.h"
#include "peinfo.h"

#ifndef KKCLASSIFY_TAB_ID
#define KKCLASSIFY_TAB_ID
#define CLASSIFY_TAB_ID	0
#define PEINFO_TAB_ID	1
#endif
// CKKclassifyDlg dialog
class CKKclassifyDlg : public CDialog
{
// Construction
public:
	CKKclassifyDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_KKCLASSIFY_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl m_cMainTab;
	afx_msg void OnTcnSelchangeMainTab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	bool resize(int cx, int cy);
	bool resize();
	int createTab(void);
	CClassifyDlg m_classifyTab;
	afx_msg void OnFileExit();
	afx_msg void OnClose();
	//var
	CKKClassifyConf *pConfig;
	CDefineSampleDlg m_dfsample;
	int presel;
	afx_msg void OnConfigDefinesample();
	void closeTab(void);
	//KKPeInfo m_peInfo;
	CPeInfo m_peInfoTab;
	CString setTabTitle(int tabID,CString newTitle);
};
