// KKclassify.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "KKclassify.h"
#include "KKclassifyDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CKKclassifyApp

BEGIN_MESSAGE_MAP(CKKclassifyApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CKKclassifyApp construction

CKKclassifyApp::CKKclassifyApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CKKclassifyApp object

CKKclassifyApp theApp;


// CKKclassifyApp initialization

BOOL CKKclassifyApp::InitInstance()
{
	AfxOleInit();
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("KK Classify Applications"));

	CKKclassifyDlg dlg;
	m_pMainWnd = &dlg;
//	dlg.pConfig=&classifyConf;
//	updateClassifyConf(KKLOAD_CONF_FROM_INI);//get conf
	INT_PTR nResponse = dlg.DoModal();
//	updateClassifyConf(KKSAVE_CONF_TO_INI);//save conf

// 	if (nResponse == IDOK)
// 	{
// 		// TODO: Place code here to handle when the dialog is
// 		//  dismissed with OK
// 	}
// 	else if (nResponse == IDCANCEL)
// 	{
// 		// TODO: Place code here to handle when the dialog is
// 		//  dismissed with Cancel
// 	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

// bool CKKclassifyApp::updateClassifyConf(int getORwriteConf)
// {
// 	CString	section=_T("KKClassifyApp");
// 	CString entry=_T("ClassifyConf");
// 	CString content;
// 	int i;
// 
// 	switch(getORwriteConf)
// 	{
// 	case KKSAVE_CONF_TO_INI:
// 		//write pre path
// 		WriteProfileString(section,entry,classifyConf.path);
// 		//write list cac loai virus
// 		WriteProfileInt(section,entry,classifyConf.getSpeciesCount());
// 		for(i=0;i<classifyConf.getSpeciesCount();i++)
// 			WriteProfileString(section,entry,classifyConf.classifySpecies[i]);
// 		//write list sq tick states
// 		WriteProfileInt(section,entry,classifyConf.getSQTStateCount());
// 		for(i=0;i<classifyConf.getSQTStateCount();i++)
// 			WriteProfileString(section,entry,classifyConf.getSQTState(i));
// 		break;
// 	case KKLOAD_CONF_FROM_INI:
// 		classifyConf.path=GetProfileString(section,entry,0);
// 		//doc list cacs loai virus
// 		for(i=0;i < GetProfileInt(section,entry,0);i++)
// 			classifyConf.addClassifySpecies(GetProfileString(section,entry,0));
// 		//doc list cac sqtick states
// 		for(i=0;i < GetProfileInt(section,entry,0);i++)
// 			classifyConf.addSQTstate(GetProfileString(section,entry,0));
// 		break;
// 	}
// 	return false;
// }
