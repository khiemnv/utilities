// Calculation.cpp : implementation file
//

#include "stdafx.h"
#include "qlXuatNhap.h"
#include "Calculation.h"


// CCalculation dialog

IMPLEMENT_DYNAMIC(CCalculation, CDialog)

CCalculation::CCalculation(CWnd* pParent /*=NULL*/)
	: CDialog(CCalculation::IDD, pParent)
	, m_pKmySql(NULL)
{

}

CCalculation::~CCalculation()
{
}

void CCalculation::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCalculation, CDialog)
END_MESSAGE_MAP()


// CCalculation message handlers
