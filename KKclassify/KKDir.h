#pragma once
#include <shlwapi.h>

class CKKDir
{
public:
	CKKDir(void);
	void operator=(CKKDir &kkdir);
	~CKKDir(void);
	// get directory staic function
	static CString pickDir(void);
	//get dir tu hop dlg chon folder
	BOOL	pick();
	//get dir tu cedit chua path
	BOOL	pick(CEdit* pathEdit);
	//get dir tu mot cstring co the la filename hoac dir name
	BOOL	pick(CString path);
	//liet ke cac file trong thu muc
	int		listFiles();
	CString getDir(){return dirName;}
	CString	getFFileName(int n);
	CString getFile(int n);
	int		getNFile();
	int		addFile(CString fileName);
	
	//vars
	CString	dirName;
	CStringArray	fileList;
};
