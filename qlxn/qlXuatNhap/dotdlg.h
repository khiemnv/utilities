#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CDotDlg dialog

class CDotDlg : public CDialog
{
	DECLARE_DYNAMIC(CDotDlg)

public:
	CDotDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDotDlg();

// Dialog Data
/*	enum { IDD = IDD_DOT_DIALOG };*/

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnStnClicked65535();
	CString m_vStaticDotNhap;
	CString m_vStaticTenHang;
	CString m_vStaticNgayNhap;
	CString m_vStaticComment;
	afx_msg void OnSize(UINT nType, int cx, int cy);
private:
	void resize(int cx, int cy);
public:
	CListCtrl m_cDotList;
	CButton m_cRemoveBtn;
	CButton m_cEditBtn;
	CButton m_cAddBtn;
	CEdit m_cCommentEdit;
	CString m_vStaticChungTu;
};
