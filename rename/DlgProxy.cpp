// DlgProxy.cpp : implementation file
//

#include "stdafx.h"
#include "rename.h"
#include "DlgProxy.h"
#include "renameDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRenameDlgAutoProxy

IMPLEMENT_DYNCREATE(CRenameDlgAutoProxy, CCmdTarget)

CRenameDlgAutoProxy::CRenameDlgAutoProxy()
{
	EnableAutomation();
	
	// To keep the application running as long as an automation 
	//	object is active, the constructor calls AfxOleLockApp.
	AfxOleLockApp();

	// Get access to the dialog through the application's
	//  main window pointer.  Set the proxy's internal pointer
	//  to point to the dialog, and set the dialog's back pointer to
	//  this proxy.
	ASSERT (AfxGetApp()->m_pMainWnd != NULL);
	ASSERT_VALID (AfxGetApp()->m_pMainWnd);
	ASSERT_KINDOF(CRenameDlg, AfxGetApp()->m_pMainWnd);
	m_pDialog = (CRenameDlg*) AfxGetApp()->m_pMainWnd;
	m_pDialog->m_pAutoProxy = this;
}

CRenameDlgAutoProxy::~CRenameDlgAutoProxy()
{
	// To terminate the application when all objects created with
	// 	with automation, the destructor calls AfxOleUnlockApp.
	//  Among other things, this will destroy the main dialog
	if (m_pDialog != NULL)
		m_pDialog->m_pAutoProxy = NULL;
	AfxOleUnlockApp();
}

void CRenameDlgAutoProxy::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}

BEGIN_MESSAGE_MAP(CRenameDlgAutoProxy, CCmdTarget)
	//{{AFX_MSG_MAP(CRenameDlgAutoProxy)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CRenameDlgAutoProxy, CCmdTarget)
	//{{AFX_DISPATCH_MAP(CRenameDlgAutoProxy)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IRename to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {8F93C913-D2E4-4700-909F-BA5BC7D8A104}
static const IID IID_IRename =
{ 0x8f93c913, 0xd2e4, 0x4700, { 0x90, 0x9f, 0xba, 0x5b, 0xc7, 0xd8, 0xa1, 0x4 } };

BEGIN_INTERFACE_MAP(CRenameDlgAutoProxy, CCmdTarget)
	INTERFACE_PART(CRenameDlgAutoProxy, IID_IRename, Dispatch)
END_INTERFACE_MAP()

// The IMPLEMENT_OLECREATE2 macro is defined in StdAfx.h of this project
// {CC36F2D8-179E-4867-AA76-127E36317EEA}
IMPLEMENT_OLECREATE2(CRenameDlgAutoProxy, "Rename.Application", 0xcc36f2d8, 0x179e, 0x4867, 0xaa, 0x76, 0x12, 0x7e, 0x36, 0x31, 0x7e, 0xea)

/////////////////////////////////////////////////////////////////////////////
// CRenameDlgAutoProxy message handlers
