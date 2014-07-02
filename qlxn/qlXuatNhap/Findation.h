#pragma once


// CFindation dialog

class CFindation : public CDialog//, public CKDlgExt
{
	DECLARE_DYNAMIC(CFindation)

public:
	CFindation(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFindation();

// Dialog Data
	enum { IDD = IDD_FINDATION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
