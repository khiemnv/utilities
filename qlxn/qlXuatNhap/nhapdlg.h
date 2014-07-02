#pragma once
#include "afxcmn.h"
#include "afxwin.h"

// CNhapDlg dialog

class CNhapDlg : public CDialog
{
	DECLARE_DYNAMIC(CNhapDlg)

public:
	CNhapDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CNhapDlg();

// Dialog Data
/*	enum { IDD = IDD_NHAP_DIALOG };*/

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_vStaticTenHang;
	CString m_vStaticSoLuongKg;
	CString m_vStaticSoLuongCay;
	CString m_vStaticDonGia;
	CString m_vStaticThanhTienCT;
	CString m_vStaticThueVAT;
	CString m_vStaticDotNhap;
	void resize(int cx, int cy);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CListCtrl m_cNhapList;
	CButton m_cRemoveBtn;
	CButton m_cEditBtn;
	CButton m_cAddBtn;
	afx_msg void OnBnClickedAddButton();
	CString m_vDotNhap;
	CString m_vTenHang;
	CString m_vSoLuongKg;
	CString m_vSoLuongCay;
	CString m_vDonGia;
	afx_msg void OnEnChangeDotNhapEdit();
	afx_msg void OnEnChangeTenhangEdit();
	afx_msg void OnStnClickedSoluongkgStatic();
	afx_msg void OnEnChangeSoLuongCayEdit();
	afx_msg void OnEnChangeDonGiaiEdit();
	float m_vThue;
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	CString m_vComment;
	/*CQLXuatNhapDlg* qlxn;*/
};
