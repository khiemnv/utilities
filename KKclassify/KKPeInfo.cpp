// KKPeInfo.cpp : implementation file
//

#include "stdafx.h"
#include "KKclassify.h"
#include "KKPeInfo.h"


// KKPeInfo dialog

IMPLEMENT_DYNAMIC(KKPeInfo, CDialog)

KKPeInfo::KKPeInfo(CWnd* pParent /*=NULL*/)
	: CDialog(KKPeInfo::IDD, pParent)
{

}

KKPeInfo::~KKPeInfo()
{
}

void KKPeInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(KKPeInfo, CDialog)
END_MESSAGE_MAP()


// KKPeInfo message handlers
