#pragma once


// CCalculation dialog

class CCalculation : public CDialog
{
	DECLARE_DYNAMIC(CCalculation)

public:
	CCalculation(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCalculation();

// Dialog Data
	enum { IDD = IDD_CALCULATION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
