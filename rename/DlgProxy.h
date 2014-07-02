// DlgProxy.h : header file
//

#if !defined(AFX_DLGPROXY_H__EC2856E2_838D_40C9_BB95_0DC4E1E3D5E7__INCLUDED_)
#define AFX_DLGPROXY_H__EC2856E2_838D_40C9_BB95_0DC4E1E3D5E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRenameDlg;

/////////////////////////////////////////////////////////////////////////////
// CRenameDlgAutoProxy command target

class CRenameDlgAutoProxy : public CCmdTarget
{
	DECLARE_DYNCREATE(CRenameDlgAutoProxy)

	CRenameDlgAutoProxy();           // protected constructor used by dynamic creation

// Attributes
public:
	CRenameDlg* m_pDialog;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRenameDlgAutoProxy)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CRenameDlgAutoProxy();

	// Generated message map functions
	//{{AFX_MSG(CRenameDlgAutoProxy)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	DECLARE_OLECREATE(CRenameDlgAutoProxy)

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CRenameDlgAutoProxy)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPROXY_H__EC2856E2_838D_40C9_BB95_0DC4E1E3D5E7__INCLUDED_)
