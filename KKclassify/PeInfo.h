#pragma once
#include "afxcmn.h"
#include "listctrlext.h"


// CPeInfo dialog
#ifndef __KKSCALCE
#define __KKSCALCE
#define KKSCALCE(x) ((x)*14/11)
#define KKUNSCALCE(x) ((x)*11/14)
#define KK_Y_SPACE	5
#define KK_X_SPACE	2
#define KK_L_SPACE	7
#define KK_R_SPACE	7
#define KK_T_SPACE	7
#define KK_B_SPACE	2
#define KK_NY_BTN	20
#define KK_NY_EDT	17
#define KK_NY_CMB	14
#define KK_NX_TREE	130
#define KK_NY_C		32
#endif


class CPeInfo : public CDialog
{
	DECLARE_DYNAMIC(CPeInfo)

public:
	CPeInfo(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPeInfo();

// Dialog Data
	enum { IDD = IDD_PEINFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	bool resize(int cx, int cy);
	CListCtrl m_peDescript;
	CTreeCtrl m_peTree;
	//CListCtrlExt m_listExt;
	virtual BOOL OnInitDialog();
};
