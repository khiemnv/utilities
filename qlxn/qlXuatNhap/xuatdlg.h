#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CXuatDlg dialog

class CXuatDlg : public CDialog
{
	DECLARE_DYNAMIC(CXuatDlg)

public:
	CXuatDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CXuatDlg();

// Dialog Data
/*	enum { IDD = IDD_XUAT_DIALOG };*/

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_vStaticTenHang;
	CString m_vStaticNgay;
	CString m_vStaticSoLuongKg;
	CString m_vStaticSoLuongCay;
	CString m_vStaticDonGia;
	CString m_vStaticThanhTienCT;
	CString m_vStaticThueVAT;
	CString m_vStaticDotNhap;
	CListCtrl m_cXuatList;
	CButton m_cAddBtn;
	CButton m_cEditBtn;
	CButton m_cRemoveBtn;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void resize(int cx, int cy);
	afx_msg void OnLvnItemchangedXuatList(NMHDR *pNMHDR, LRESULT *pResult);
/*	CQLXuatNhapDlg* qlxn;*/
};
