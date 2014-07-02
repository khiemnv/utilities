// qlXuatNhapDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "inputdlg.h"
#include "cusquerydlg.h"
#include "kinputdlg.h"
#include "calculation.h"
#include "taohoadon.h"
//#include "kmysql.h"

#define CONF_FILE	"config.txt"
#define COMMENT_LEN 12
// CqlXuatNhapDlg dialog
class CqlXuatNhapDlg : public CDialog
{
// Construction
public:
	CqlXuatNhapDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_QLXUATNHAP_DIALOG };

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
	CTabCtrl m_mainTab;
	//InputDlg m_dotNhapTab;
	//InputDlg m_hangTab;
	afx_msg void OnTcnSelchangeMainTab(NMHDR *pNMHDR, LRESULT *pResult);
	int curTabSel;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	int resize(void);
	KMySQL m_kmysql;
	//InputDlg m_nhapTab;
	//InputDlg m_xuatTab;
	//InputDlg m_tonTab;
	bool init_database(void);
	CCusQueryDlg m_cusQTab;
	//InputDlg m_dotXuatTab;
	//int m_duAnTab;
	//InputDlg m_duAnTab;
	InputDlg* m_pInputDlgs;
	afx_msg void OnClose();
	//InputDlg m_dotNhapTab;
	CKInputDlg m_duAnTab;
	CKInputDlg m_dotNhapTab;
	CKInputDlg m_nhapTab;
	CKInputDlg m_hangTab;
	CKInputDlg m_dotXuatTab;
	CKInputDlg m_xuatTab;
	CCusQueryDlg m_tonTab;
	// tinh so du
	//int m_calculator;
	afx_msg void OnToolsCalculation();
	CCalculation m_calculation;
	afx_msg void OnToolsTinhsodu();
	afx_msg void OnToolsTaohoadon();
	CTaoHoaDon m_taoHoaDon;
};
