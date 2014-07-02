#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#ifndef _KMYSQL_
#define _KMYSQL_
#include "Kmysql.h"
#endif

#ifndef __KKSCALE
#define __KKSCALE
#define KKSCALE(x) ((x)*12/7)
#define KKUNSCALE(x) ((x)*7/12)
#define KK_Y_SPACE	2
#define KK_X_SPACE	2
#define KK_L_SPACE	7
#define KK_R_SPACE	7
#define KK_T_SPACE	7
#define KK_B_SPACE	2
#define KK_NY_BTN	14
#define KK_NX_BTN	50
#define KK_NY_EDT	12
#define KK_NY_STATIC 8

#define KK_COL_WIDTH(x)		((x)*6+10)
#endif

#define CCQD_MAXHEADING_LEN	20
#define CCQD_QBUFF_SIZE		CKMYSQL_BUFF_LENGTH
// CCusQueryDlg dialog

class CCusQueryDlg : public CDialog
{
	DECLARE_DYNAMIC(CCusQueryDlg)

public:
	CCusQueryDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCusQueryDlg();

// Dialog Data
	enum { IDD = IDD_CUSQUERY_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_queryEdt;
	CListCtrl m_resList;
	CButton m_queryBtn;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	int resize(int cx, int cy);
	int resize(void);
	int create(CTabCtrl *pTab, KMySQL *pKmysql,int tab_id,CRect &rect);
		KMySQL* m_pKmySql;
		afx_msg void OnBnClickedQueryBtn();
		void show_on_list(void);
		int total_row;
		void clean_list(void);
		void add_item(int row, int col, CString content);
		void create_cols(void);
		virtual BOOL OnInitDialog();
		int m_tabID;
		TCHAR* m_buffer;
		afx_msg void OnLvnItemchangedResList(NMHDR *pNMHDR, LRESULT *pResult);
		afx_msg void OnEnSetfocusQueryEdt();
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};
