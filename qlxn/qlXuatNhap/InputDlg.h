#pragma once
//#include "Kmysql.h"
#ifndef _KMYSQL_
#define _KMYSQL_
#include "Kmysql.h"
#endif
#include "afxwin.h"
#include "afxdtctl.h"
#include "afxcmn.h"
#include "KKString.h"

//define dlg mode
#define ADD_MODE	1
#define EDIT_MODE	2
#define REMOVE_MODE	3
//define tab id
#ifndef INPUT_TAB_ID
#define INPUT_TAB_ID
#define TAB_DU_AN_ID	0	//khiem
#define TAB_DOT_NHAP_ID	1
#define TAB_HANG_ID		2
#define	TAB_NHAP_ID		3
#define TAB_DOT_XUAT_ID 4
#define TAB_XUAT_ID		5

#define TAB_TON_ID		6
#define TAB_ALL_ID		7
#define TAB_SODU		8
#endif

//define table name and number fields of table
#define DU_AN_TABLE		"du_an"
#define NHAP_TABLE		"nhap"
#define XUAT_TABLE		"xuat"
#define DOT_NHAP_TABLE	"dot_nhap"
#define DOT_XUAT_TABLE	"dot_xuat"
#define HANG_TABLE		"hang"

#define N_HANG_FIELD		4
#define N_NHAP_FIELD		7
#define N_XUAT_FIELD		8
#define N_DOT_NHAP_FIELD	5
#define N_DOT_XUAT_FIELD	5
#define N_DU_AN_FIELD		5

#define N_TON_FIELD			13
//define cho ham resize
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
#define KK_NX_DATE	60
#endif
//
#define N_INPUTDLG_CWND 26

// InputDlg dialog

class InputDlg : public CDialog
{
	DECLARE_DYNAMIC(InputDlg)

public:
	InputDlg(CWnd* pParent = NULL);   // standard constructor

	virtual ~InputDlg();

// Dialog Data
	enum { IDD = IDD_INPUT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	KMySQL* m_pKmySql;
	int tabID;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CButton m_addBtn;
	CStatic m_sDotNhap;
	CStatic m_sMSCT;
	CStatic m_sNGAY;
	CStatic m_sSLKG;
	CStatic m_sSLCAY;
	CStatic m_sDonGia;
	CStatic m_sTTCT;
	CStatic m_sThue;
	CStatic m_sMSH;
	CStatic m_sHang;
	CStatic m_sGhiChu;
	CEdit m_dotEdt;
	CEdit m_msctEdt;
	CDateTimeCtrl m_ngay;
	CEdit m_slkgEdt;
	CEdit m_slcayEdt;
	CEdit m_donGia;
	CEdit m_TTCT;
	CEdit m_thueEdt;
	CEdit m_mshEdt;
	CEdit m_tenHang;
	CEdit m_ghiChu;
	CButton m_editBtn;
	CButton m_removeBtn;
	CListCtrl m_resList;
	int resize(int cx, int cy);
	int resize(void);
	afx_msg void OnEnChangeMsctEdit();
	int hideCtrl(void);
	int create(CTabCtrl* tab,KMySQL* pKmysql,int tab_id,CRect& rect);
	CWnd** lcwnd; //list of all ctrls
	afx_msg void OnBnClickedAddBtn();
	/*int m_listExt;*/
	void add_item(int row, int col, CString content);
	void add_row(CString* contents, int n_col);
	// tong so row trong list view
	int total_row;
	void create_list_cols(void);

// 		(CWnd*)&m_sDotNhap,(CWnd*)&m_dotEdt,(CWnd*)&m_sMSCT,(CWnd*)&m_msctEdt,
// 		(CWnd*)&m_sNGAY,(CWnd*)&m_ngay,
// 		(CWnd*)&m_sSLKG,(CWnd*)&m_slkgEdt,(CWnd*)&m_sSLCAY,(CWnd*)&m_slcayEdt,
// 		(CWnd*)&m_sDonGia,(CWnd*)&m_donGia,(CWnd*)&m_sTTCT,(CWnd*)&m_TTCT,
// 		(CWnd*)&m_sThue,(CWnd*)&m_thueEdt,(CWnd*)&m_sMSH,(CWnd*)&m_mshEdt,
// 		(CWnd*)&m_sHang,(CWnd*)&m_tenHang,(CWnd*)&m_sGhiChu,(CWnd*)&m_ghiChu,
// 		(CWnd*)&m_resList,(CWnd*)&m_addBtn,(CWnd*)&m_editBtn,(CWnd*)&m_removeBtn};

	void show_on_list(void);
	void clean_list(void);
	void show_table(void);
	afx_msg void OnLvnItemchangedResList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkResList(NMHDR *pNMHDR, LRESULT *pResult);
	bool add_record(void);
	UINT m_flagAC;
	int m_addNField;
	CString m_addTable;
	bool show_record(void);
	afx_msg void OnBnClickedRemoveBtn();
	void del_row(int row);
	afx_msg void OnEnChangeDgEdt();
	void update_TTCTedt(void);
	bool del_record(void);
	afx_msg void OnBnClickedEditBtn();
// 	InputDlg m_dotXuatTab;
// 	InputDlg m_duAnTab;
};
