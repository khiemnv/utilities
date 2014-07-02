#pragma once
#include "kmysql.h"


// CTaoHoaDon dialog

class CTaoHoaDon : public CDialog
{
	DECLARE_DYNAMIC(CTaoHoaDon)

public:
	CTaoHoaDon(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTaoHoaDon();

// Dialog Data
	enum { IDD = IDD_TOAHOADON };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	//KMySQL m_pKmySql;
	KMySQL* m_pKmySql;
};
