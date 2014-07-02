#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "afxdtctl.h"
#include "Kmysql.h"
#include <math.h>
#include "afxcoll.h"
#ifdef _WIN32_WCE
#error "CDHtmlDialog is not supported for Windows CE."
#endif 

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
#endif

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

#define KK_CHAR_WIDTH		6
#define KK_COL_WIDTH(x)		((x)*6+15)
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
//other define
// #define COMMENT_RANGE		16
#define CKINPUT_CWND_N		22
#define CKINPUT_FLAGI_N		11
#define CKINPUT_FLAG_INDEX_END	-1
#define CKINPUT_FIELD_N			26
#define CKI_FB_P				-2
#define CKINPUT_VAL_LEN			50
#define CKINPUT_TON_QUERY_I		0

// CKInputDlg dialog

class CKInputDlg : public CDialog//, public CKKString
{
	DECLARE_DYNAMIC(CKInputDlg)

public:
	CKInputDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CKInputDlg();
// Overrides
// 	HRESULT OnButtonOK(IHTMLElement *pElement);
// 	HRESULT OnButtonCancel(IHTMLElement *pElement);

// Dialog Data
	enum { IDD = IDD_KINPUTDLG, IDH = IDR_HTML_KINPUTDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
/*	DECLARE_DHTML_EVENT_MAP()*/
public:
	KMySQL* m_pKmySql;
	int tabID;
	CButton m_addBtn;
	CButton m_editBtn;
	CButton m_removeBtn;
	CListCtrl m_resList;
	CDateTimeCtrl m_ngay;
	CEdit m_ghiChu;
	CStatic m_sGhiChu;
	CStatic m_label0;
	CStatic m_label1;
	CStatic m_label2;
	CStatic m_label3;
	CStatic m_label4;
	CStatic m_label5;
	CStatic m_label6;
	CEdit m_edit0;
	CEdit m_edit1;
	CEdit m_edit2;
	CEdit m_edit3;
	CEdit m_edit4;
	CEdit m_edit5;
	CEdit m_edit6;

	CDateTimeCtrl m_ngayKT;
	UINT m_flagLC;
	int* m_flagKI;
	UINT m_flagKC;
	int* m_flagEI;
	UINT m_flagEC;
	int* m_flagELI;
	// mang ten cac fields trong database
	CStringArray m_fields;
	CStringArray m_queries;
	// list of ctrl*
	CWnd** lcwnd;
	CString m_tableName;
	int m_fieldCount;
	int* m_flagAI;
	UINT m_flagAC;
	// dung de set labels
	int* m_flagLI;
	UINT m_flagHC;

	/*	int* m_flagAI;*/
	int add_col(CString heading);
	void add_item(int row, int col, CString& content);
	bool add_record(void);

	// them mot dong vao list
	void add_row(CString* contents, int n_col);
	void clean_list(void);
	int create(CTabCtrl* pTab, KMySQL* pKmysql, int tab_id, CRect& rect);
	int* create_flagI(
		int * flag_i,
		int c0=-1, int c1=-1, int c2=-1, int c3=-1, int c4=-1, 
		int c5=-1, int c6=-1, int c7=-1, int c8=-1, int c9=-1
		);
	int create_flags(void);
	/*int* create_flags(void);*/
	int* create_flagLabel(
		int c0=-1, int c1=-1, int c2=-1, int c3=-1, int c4=-1, 
		int c5=-1, int c6=-1, int c7=-1, int c8=-1, int c9=-1
		);
	void create_list_cols(void);
	//CString* m_fields;
	// make condition
	CString create_strC(void);
	// new value (used to edit an old record)
	CString create_strNV(void);
	bool del_record(void);
	bool edit_record(void);
	int hideCtrl(void);
	void init_var(void);
	afx_msg void OnBnClickedAddBtn();
	afx_msg void OnBnClickedEditBtn();
	afx_msg void OnBnClickedRemoveBtn();
	afx_msg void OnLvnItemchangedResList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkResList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	int resize(int cx, int cy);
	bool set_comment(void);
	bool show_selected_record(void);
	void show_on_list(void);
	void show_table(void);
	void update_TTCTedt(void);
	//TCHAR** m_fields;
	
	/*void add_row(CStringArray contents);*/
	bool m_editable;
	afx_msg void OnEnSetfocusEdit0();
	afx_msg void OnEnKillfocusEdit0();
	afx_msg void OnEnUpdateEdit0();
};
