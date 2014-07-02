// QLXuatNhapDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "dotdlg.h"
#include "nhapdlg.h"
#include "tenhangdlg.h"
#include "tondlg.h"
#include "xuatdlg.h"
#include "alldlg.h"
/*#include "kmysql.h"*/


// CQLXuatNhapDlg dialog
class CQLXuatNhapDlg : public CDialog
{
// Construction
public:
	CQLXuatNhapDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_QLXUATNHAP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
//	afx_msg void OnPaint();
//	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	void initTabs(void);
	CTabCtrl m_cTabMain;
	CAllDlg* m_cDotTab;
	CAllDlg* m_cNhapTab;
	CAllDlg* m_cTenHangTab;
	CAllDlg* m_cTonTab;
	CAllDlg* m_cXuatTab;
private:
	int curTab;
	// show and hide tabs when current tab sel change
	void showHideTabs(void);
public:
	// //selected tab sel
	int curSel;
	afx_msg void OnTcnSelchangeTabMain(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
private:
	// resize all tabs
	void resize(void);
	bool resize(int cx, int cy);
public:
	CAllDlg* m_cAllTab;
	KMySQL m_cMySql;
	afx_msg void OnClose();
	void show_error_msgbox(CString error_msg);
	bool init_database(void);
	afx_msg void OnPaint();
	afx_msg void OnFileExit();
	afx_msg void OnOptionAddmode();
	afx_msg void OnOptionEditmode();
	afx_msg void OnOptionRemovemode();
	void change_tabsMode(int mode);
};
