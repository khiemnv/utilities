#pragma once


// KKPeInfo dialog

class KKPeInfo : public CDialog
{
	DECLARE_DYNAMIC(KKPeInfo)

public:
	KKPeInfo(CWnd* pParent = NULL);   // standard constructor
	virtual ~KKPeInfo();

// Dialog Data
	enum { IDD = IDD_PEINFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
