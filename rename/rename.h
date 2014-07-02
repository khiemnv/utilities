// rename.h : main header file for the RENAME application
//

#if !defined(AFX_RENAME_H__A63E1753_493D_4C53_BE3C_696FDED18F8A__INCLUDED_)
#define AFX_RENAME_H__A63E1753_493D_4C53_BE3C_696FDED18F8A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CRenameApp:
// See rename.cpp for the implementation of this class
//

class CRenameApp : public CWinApp
{
public:
	CRenameApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRenameApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CRenameApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RENAME_H__A63E1753_493D_4C53_BE3C_696FDED18F8A__INCLUDED_)
