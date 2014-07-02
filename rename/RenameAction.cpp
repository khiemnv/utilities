// RenameAction.cpp: implementation of the CRenameAction class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "rename.h"
#include "RenameAction.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRenameAction::CRenameAction()
{

}

CRenameAction::~CRenameAction()
{

}

BOOL CRenameAction::read_conf()
{
	FILE *f;
	TCHAR temp[MAX_PATH];
	int		i;

	fopen_s(&f,"ren_conf.txt","rt+");
	if(f!=NULL)
	{
		fwscanf_s(f,_T("%s %d"),temp,MAX_PATH,&ren_info.add_cut);
		fwscanf_s(f,_T("%s"),temp,MAX_PATH);
		ren_info.tail=temp+8;

		fwscanf_s(f,_T("%s %d"),temp,MAX_PATH,&ren_info.replace);
		fwscanf_s(f,_T("%s"),temp,MAX_PATH);
		ren_info.old_str=temp+8;

		fwscanf_s(f,_T("%s"),temp,MAX_PATH);
		ren_info.new_str=temp+8;

		fwscanf_s(f,_T("%s %d"),temp,MAX_PATH,&ren_info.ren_exe);
		fwscanf_s(f,_T("%s %d"),temp,MAX_PATH,&i);
		ren_info.exe_tails.RemoveAll();
		while(i>0)
		{
			i--;
			fwscanf_s(f,_T("%s"),temp,MAX_PATH);
			ren_info.exe_tails.Add(temp);
		}

		fwscanf_s(f,_T("%s"),temp,MAX_PATH);
		ren_info.dir_path=temp;
		ren_info.dir_path.Replace('*',' ');

		fwscanf_s(f,_T("%s %d"),temp,MAX_PATH,&ren_info.remove_);

		fclose(f);
		return TRUE;
	}

	return FALSE;
}

BOOL CRenameAction::update_conf()
{
	FILE *f;
	int i;
// 	char buff[MAX_PATH];
// 	size_t convertedChar;

	fopen_s(&f,"ren_conf.txt","wt+");
	if (f!=NULL)
	{
		fwprintf_s(f,_T("add_cut: %d\n"),ren_info.add_cut);
		fwprintf_s(f,_T("tail---:%s\n"),ren_info.tail);

		fwprintf_s(f,_T("replace: %d\n"),ren_info.replace);
		fwprintf_s(f,_T("old_str:%s\n"),ren_info.old_str);
		fwprintf_s(f,_T("new_str:%s\n"),ren_info.new_str);

		fwprintf_s(f,_T("chk_et-: %d\n"),ren_info.ren_exe);
		fwprintf_s(f,_T("ex_tail: %d\n"),ren_info.exe_tails.GetSize());
		for (i=0;i<ren_info.exe_tails.GetSize();i++)
		{
			fwprintf_s(f,_T("%s\n"),ren_info.exe_tails.GetAt(i));
		}

		ren_info.dir_path.Replace(' ','*');
// 		convertedChar=0;
// 		wcstombs_s(&convertedChar,buff,MAX_PATH,ren_info.dir_path,_TRUNCATE);
		fwprintf_s(f,_T("%s\n"),ren_info.dir_path);

		fwprintf_s(f,_T("remove_: %d\n"),ren_info.remove_);

		fclose(f);
		return TRUE;
	}
	return FALSE;
}

int CRenameAction::execute(CKKDir &kkdir)
{
	int i;
	int count=0;
	CString oldName;
	CString newName;
	TCHAR buff[MAX_PATH];
	int nBuffLength=MAX_PATH;


	if(AfxMessageBox(_T("you sure to rename these file?"),MB_YESNO,0)==IDYES)
	{
		GetCurrentDirectory(nBuffLength,buff);
		SetCurrentDirectory(kkdir.getDir());
		for(i=0; i<ren_info.old_names.GetSize(); i++)
		{
			if(ren_info.old_names[i]==ren_info.new_names[i])
				continue;
			//oldName=kkdir.getFFileName(i);
			/*int j=oldName.ReverseFind('\\');*/
			//newName=oldName.Left(oldName.ReverseFind('\\'));

			//newName.AppendFormat(_T("\\%s"),ren_info.new_names[i]);
			if (GetFileAttributes(ren_info.old_names[i])!= FILE_ATTRIBUTE_DIRECTORY)
			{
				SetFileAttributes(ren_info.old_names[i],FILE_ATTRIBUTE_NORMAL);
				MoveFile(ren_info.old_names[i],ren_info.new_names[i]);
				count++;
			}
		}
		newName.Format(_T("renamed files: %d"),count);
		AfxMessageBox(newName,MB_OK,0);

		SetCurrentDirectory(buff);
		return count;
	}
	return -1;
}

CString CRenameAction::edit_name(CString name)
{
	int i;

	if(ren_info.remove_==1)
	{
		i=name.FindOneOf(_T("_"));
		if(i != -1)
		{
			name.SetAt(i,'*');
			name.Remove('_');
			name.SetAt(i,'_');
		}
	}

	if(ren_info.add_cut==0)
	{
		if( (!ren_info.ren_exe) || is_executable(name))
		name=name+ren_info.tail;
	}
	else
		name.TrimRight(ren_info.tail);
	
	if(ren_info.replace == 1)
		name.Replace(ren_info.old_str,ren_info.new_str);
	
	return name;
}

BOOL CRenameAction::is_executable(CString fname)
{
	int i;

	fname.MakeLower();
	CString tail=fname.Mid(fname.Find('.'));

// 	for(i=0;i<ren_info.exe_tails.GetSize();i++)
// 		if(fname.Find(ren_info.exe_tails.GetAt(i)) != -1)
// 			return	TRUE;
	for (i=0;i<ren_info.exe_tails.GetSize();i++)
		if(tail==ren_info.exe_tails.GetAt(i))
			return	TRUE;

	return FALSE;
}

int CRenameAction::list_file(CKKDir& kkdir)
{
	CString	fName;
// 	CFileFind finder;
// 	BOOL bWorking = finder.FindFile(ren_info.dir_path+"\\*.*");
// 	
	ren_info.old_names.RemoveAll();
	ren_info.new_names.RemoveAll();
	
// 	while (bWorking)
// 	{
// 		bWorking = finder.FindNextFile();
// 		if (finder.IsDots())
// 			continue;
// 		if (finder.IsDirectory())
// 			continue;
// 		
// 		fName=finder.GetFileName();
// 		ren_info.old_names.Add(CString(fName));
// 		ren_info.new_names.Add(edit_name(fName));
//	}
	for(int i=0;i<kkdir.getNFile();i++)
	{
		fName=kkdir.getFile(i);
		ren_info.old_names.Add(CString(fName));
		ren_info.new_names.Add(edit_name(fName));
	}

	return ren_info.old_names.GetSize();
}
