#include "StdAfx.h"
#include "KKDir.h"

CKKDir::CKKDir(void)
{
}

CKKDir::~CKKDir(void)
{
}

void CKKDir::operator=(CKKDir &kkdir)
{
	int i;
	dirName=kkdir.getDir();
	fileList.RemoveAll();
	for(i=0;i<kkdir.getNFile();i++)
		fileList.Add(kkdir.getFile(i));
}

// get directory
CString CKKDir::pickDir(void)
{
	BROWSEINFO	bi = { 0 };
	TCHAR		dirName[MAX_PATH];
	bi.lpszTitle = _T("Pick a Directory");
	bi.pszDisplayName = dirName;
	LPITEMIDLIST pidl = SHBrowseForFolder ( &bi );
	if ( pidl != 0 )
	{
		// get the name of the folder
		SHGetPathFromIDList(pidl,dirName);

		IMalloc * imalloc = 0;
		if ( SUCCEEDED( SHGetMalloc ( &imalloc )) )
		{
			imalloc->Free ( pidl );
			imalloc->Release ( );
		}

		return	CString(dirName);
	}
	return CString();
}
//pick a dir by sh brower for folder
BOOL CKKDir::pick()
{
	BROWSEINFO	bi = { 0 };
	TCHAR		path[MAX_PATH];
	bi.lpszTitle = _T("Pick a Directory");
	bi.pszDisplayName = path;
	LPITEMIDLIST pidl = SHBrowseForFolder ( &bi );
	if ( pidl != 0 )
	{
		// get the name of the folder
		SHGetPathFromIDList(pidl,path);

		IMalloc * imalloc = 0;
		if ( SUCCEEDED( SHGetMalloc ( &imalloc )) )
		{
			imalloc->Free ( pidl );
			imalloc->Release ( );
		}
		
		dirName=path;
		return	TRUE;
	}
	return FALSE;
}
//get dir from cedit
BOOL	CKKDir::pick(CEdit* pathEdit)
{
	CString path;
	pathEdit->GetWindowText(path);
	//neu path is folder
	if(!PathIsDirectory(path))
		return FALSE;
	dirName=path;
	return TRUE;
}
int	CKKDir::listFiles()
{
	CString		fName;
	CFileFind	finder;
	BOOL		bWorking = finder.FindFile(dirName+_T("\\*.*"));

	fileList.RemoveAll();

	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		if (finder.IsDots())
			continue;
		if (finder.IsDirectory())
			continue;

		fName=finder.GetFileName();
		fileList.Add(CString(fName));
	}

	return fileList.GetSize();
}

BOOL	CKKDir::pick(CString fileName)
{
	while(!PathIsDirectory(fileName))
	{
		int i=fileName.ReverseFind('\\');
		fileName=fileName.Left(i);
		if(fileName.IsEmpty()) 
			break;
	}
	
	if(PathIsDirectory(fileName))
		dirName=fileName;
	else
		return FALSE;
	return TRUE;
}

CString	CKKDir::getFFileName(int n)
{
	CString fileName=fileList.GetAt(n);
	CString	path;
	if(PathFileExists(fileName))
		return	fileName;
	else
	{
		path=dirName;
		path.TrimRight('\\');
		path.AppendFormat(_T("\\%s"),fileName);
		return	path;
	}	
}
CString CKKDir::getFile(int n)
{
	return	fileList.GetAt(n);
}
int		CKKDir::getNFile()
{
	return fileList.GetSize();
}
int		CKKDir::addFile(CString fileName)
{
	fileList.Add(fileName);
	return	fileList.GetSize();
}
