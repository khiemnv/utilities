// RenameAction.h: interface for the CRenameAction class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RENAMEACTION_H__2C7C09E7_F45F_48DC_B614_4875DE4F0CFC__INCLUDED_)
#define AFX_RENAMEACTION_H__2C7C09E7_F45F_48DC_B614_4875DE4F0CFC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Afxcoll.h>
#include "KKDir.h"

typedef struct _RENAME_INFO
{
	BOOL	add_cut;
	CString tail;
	BOOL	replace;
	CString old_str;
	CString new_str;
	BOOL	ren_exe;
	CStringArray	exe_tails;
	/*CStringList		exe_tails;*/
	CStringArray	old_names;
	CStringArray	new_names;
	CString	dir_path;
	BOOL	remove_;
}RenameInfo;

class CFileInfo
{
public:
	CString	fPath;
	CString fName;
};

class CTrim
{
public:
	CTrim()	{_taget=0;};
	CTrim(CString* ptr_taget) {_taget=ptr_taget;};
	CString*	execute();
	CString*	trimTail();

	CString*	_taget;
	CString	str;
	BOOL	left;
	BOOL	right;
};

class CRenameAction  
{
public:
	int list_file(CKKDir& kkdir);
	BOOL is_executable(CString);
	CString edit_name(CString);
	int		execute(CKKDir &kkdir);
	BOOL	update_conf();
	BOOL	read_conf();
	RenameInfo ren_info;
	CRenameAction();
	virtual ~CRenameAction();

};

#endif // !defined(AFX_RENAMEACTION_H__2C7C09E7_F45F_48DC_B614_4875DE4F0CFC__INCLUDED_)
