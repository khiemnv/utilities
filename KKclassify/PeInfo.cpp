// PeInfo.cpp : implementation file
//

#include "stdafx.h"
#include "KKclassify.h"
#include "PeInfo.h"


// CPeInfo dialog

IMPLEMENT_DYNAMIC(CPeInfo, CDialog)

CPeInfo::CPeInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CPeInfo::IDD, pParent)
{

}

CPeInfo::~CPeInfo()
{
}

void CPeInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PEdescript, m_peDescript);
	DDX_Control(pDX, IDC_PE_TREE, m_peTree);
}


BEGIN_MESSAGE_MAP(CPeInfo, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CPeInfo message handlers

void CPeInfo::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	// TODO: Add your message handler code here
	if(IsWindow(m_peDescript.GetSafeHwnd()))
	{
		resize(cx,cy);
	}
}

bool CPeInfo::resize(int cx, int cy)
{
	int x,y,nx,ny;
	cx=KKUNSCALCE(cx);
	cy=KKUNSCALCE(cy);
	//resize the tree
	x=KK_L_SPACE;y=KK_T_SPACE;
	nx=KK_NX_TREE;ny=cy-y-KK_B_SPACE;
	m_peTree.MoveWindow(KKSCALCE(x),KKSCALCE(y),KKSCALCE(nx),KKSCALCE(ny));
	//resize the list
	x+=nx+KK_X_SPACE;
	nx=cx-x-KK_R_SPACE;
	m_peDescript.MoveWindow(KKSCALCE(x),KKSCALCE(y),KKSCALCE(nx),KKSCALCE(ny));
	//resize the listext
	//x+=nx+KK_X_SPACE;
	//m_listExt.MoveWindow(KKSCALCE(x),KKSCALCE(y),KKSCALCE(nx),KKSCALCE(ny));
	return false;
}
#ifndef LISTEXT_ID
#define LISTEXT_ID	WM_USER+1
#endif
BOOL CPeInfo::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	CRect rect=CRect(0,0,100,100);
	//m_listExt.Create(m_peDescript.GetStyle(),rect,this,LISTEXT_ID);

	//tree
	m_peTree.InsertItem(_T("item 1"),TVI_ROOT,TVI_LAST);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
