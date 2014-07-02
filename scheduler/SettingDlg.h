#ifndef _KS_SETTING_FILE_H_
#define _KS_SETTING_FILE_H_
#pragma once
#include "afxdtctl.h"
#include "afxwin.h"

//
typedef struct ks_setting_tag {
	UINT alertBeforeChk;
	COleDateTime alertBeforeTime;
}KS_SETTING;
// CSettingDlg dialog

class CSettingDlg : public CDialog
{
	DECLARE_DYNAMIC(CSettingDlg)

public:
	CSettingDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSettingDlg();

// Dialog Data
	enum { IDD = IDD_SETTING_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CDateTimeCtrl m_alertBeforeTime;
	KS_SETTING* m_pKSSetting;
	afx_msg void OnBnClickedSaveBtn();
	afx_msg void OnBnClickedCancelBtn();
	virtual BOOL OnInitDialog();
	CButton m_alertBeforeChk;
//	afx_msg void OnBnDoubleclickedAlertbeforeChk();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};
#endif