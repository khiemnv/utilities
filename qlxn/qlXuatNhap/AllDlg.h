#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "resource.h"
#include "kmysql.h"
#include "afxdtctl.h"

//define dlg mode
#define ADD_MODE	1
#define EDIT_MODE	2
#define REMOVE_MODE	3
//define tab id
#define TAB_DOT_ID	0
#define TAB_HANG_ID	1
#define	TAB_NHAP_ID	2
#define TAB_XUAT_ID	3
#define TAB_TON_ID	4
#define TAB_ALL_ID	5
//define table name and number fields of table
#define NHAP_TABLE	"nhap"
#define XUAT_TABLE	"xuat"
#define DOT_TABLE	"dot"
#define HANG_TABLE	"hang"
#define N_NHAP_FIELD	7
#define N_XUAT_FIELD	8
#define N_DOT_FIELD		4
#define N_HANG_FIELD	4
#define N_ALL_FIELD		13
// CAllDlg dialog

class CAllDlg : public CDialog
{
	DECLARE_DYNAMIC(CAllDlg)

public:
	CAllDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAllDlg();

// Dialog Data
	enum { IDD = IDD_ALL_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
private:
	void resize(int cx, int cy);
public:
	CString m_vStaticDotNhap;
	CString m_vStaticChungTu;
	CString m_vStaticTenHangBrif;
	CString m_vStaticTenHangFull;
	CString m_vStaticNhap;
	CString m_vStaticSoLuongKgN;
	CString m_vStaticSoLuongCayN;
	CString m_vStaticDonGia;
	CString m_vStaticThanhTienCT;
	CString m_vStaticThue;
	CString m_vStaticNgayXuat;
	CString m_vStaticSoLuongKgX;
	CString m_vStaticSoLuongCayX;
	CString m_vStaticCommnet;
	CButton m_cAddBtn;
	CButton m_cEditBtn;
	CButton m_cRemoveBtn;
	CListCtrl m_cAllList;
	CString m_vStaticXuat;
	CString m_vStaticNgayNhap;
//	afx_msg void OnEnChangeComentEdit();
	CEdit m_cTenHangFullEdit;
	CEdit m_cCommentEdit;
	CStatic m_cStaticXuat;
	KMySQL* ptr_cKmysql;
//	afx_msg void OnEnable(BOOL bEnable);
//	afx_msg void OnInitMenu(CMenu* pMenu);
//	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
//	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
//	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void set_comment(void);
//	virtual BOOL OnInitDialog();
	int tab_id;
	afx_msg void OnBnClickedAddButton2();
	/*int add_row(CString table);*/
	CString m_vDotNhap;
	CString m_vChungTu;
	CString m_vNgayNhap;
	CString m_vTenHangBrif;
	CString m_vTenHangFull;
	CString m_vSoLuongKgN;
	CString m_vSoLuongCayN;
	CString m_vDonGia;
	CString m_vThueNhap;
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	CString m_vNgayXuat;
	CString m_vSoLuongKgX;
	CString m_vSoLuongCayX;
	CString m_vComment;
	/*int add_row(int table_id);*/
//	virtual BOOL OnInitDialog();
//	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void set_active_edit(void);
	CEdit m_cDotNhap;
	CEdit m_cChungTu;
	CDateTimeCtrl m_cNgayNhap;
	CEdit m_cTenHangBrif;
	CEdit m_cSoLuongKgN;
	CEdit m_cSoLuongCayN;
	CEdit m_cDonGiaN;
	CEdit m_cThanhTienCT;
	CDateTimeCtrl m_cNgayXuat;
	CEdit m_cSoLuongKgX;
	CEdit m_cSoLuongCayX;
	char* CString2char(CString &str, char* buff);
	char* CAllDlg::CString2char(CString &str);
	void create_list_cols(void);
	void show_table(void);
	// show result on list view
	void show_on_list(void);
	void add_item(int row, int col, CString content);
//	afx_msg void OnLvnItemchangedAllList(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnNMRDblclkAllList(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnNMDblclkAllList(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnNMClickAllList(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnNMClickAllList(NMHDR *pNMHDR, LRESULT *pResult);
	void add_row(CString* contents, int n_col);
	// tong so row trong list view
	int total_row;
	afx_msg void OnNMDblclkAllList(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnHdnItemclickAllList(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnLvnItemchangedAllList(NMHDR *pNMHDR, LRESULT *pResult);
	CButton m_cThueRadio005;
	CButton m_cThueRadio010;
	void set_init_var(void);
	afx_msg void OnBnClickedEditButton();
	void edit_row(int,CString*,int);
	void edit_item(int row, int col, CString content);
//	afx_msg void OnFileExit();
//	afx_msg void OnOptionAddmode();
//	afx_msg void OnOptionEditmode();
//	afx_msg void OnOptionRemovemode();
	void change_mode(int);
//	afx_msg void OnOptionAddmode();
	afx_msg void OnBnClickedRemoveButton();
	void del_row(int);
	void clean_list(void);
//	afx_msg void OnNMSetfocusAllList(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnHdnItemclickAllList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickAllList(NMHDR *pNMHDR, LRESULT *pResult);
};
