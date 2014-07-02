#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CTonDlg dialog

class CTonDlg : public CDialog
{
	DECLARE_DYNAMIC(CTonDlg)

public:
	CTonDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTonDlg();

// Dialog Data
/*	enum { IDD = IDD_TON_DIALOG };*/

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_vStaticDotNhap;
	CString m_vStaticTenHang;
	CString m_vStaticSoLuongKg;
	CString m_vStaticSoLuongCay;
	CString m_vStaticDonGia;
	CString m_vStaticThanhTienCT;
	CListCtrl m_cTonList;
	CButton m_cAddBtn;
	CButton m_cEditBtn;
	CButton m_cRemoveBtn;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void resize(int cx, int cy);
};
