#pragma once
#include "kkdefinesampleconf.h"


// CDefineSampleDlg dialog

class CDefineSampleDlg : public CDialog
{
	DECLARE_DYNAMIC(CDefineSampleDlg)

public:
	CDefineSampleDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDefineSampleDlg();

// Dialog Data
	enum { IDD = IDD_DEFINESAMPLE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CKKDefineSampleConf m_conf;
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
};
