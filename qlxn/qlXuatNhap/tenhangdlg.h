#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CTenHangDlg dialog

class CTenHangDlg : public CDialog
{
	DECLARE_DYNAMIC(CTenHangDlg)

public:
	CTenHangDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTenHangDlg();

// Dialog Data
/*	enum { IDD = IDD_TENHANG_DIALOG };*/

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_vStaticTenHangBrif;
//	CStatic m_vStaticTenHangFull;
	CString m_vStaticTenHangFull;
	CListCtrl m_cTenHangList;
	CEdit m_cTenHangFull;
	CEdit m_cTenHangBrif;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void resize(int cx, int cy);
	CButton m_cRemoveBtn;
	CButton m_cEditBtn;
	CButton m_cAddBtn;
};
