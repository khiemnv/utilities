#include "StdAfx.h"
#include "KKDirArray.h"

CKKDirArray::CKKDirArray(void)
{
	total_dir=0;
	capable=KK_NDIR_BLOCK;
	dir_arr	= new CKKDir[capable];
}

CKKDirArray::~CKKDirArray(void)
{
	delete	dir_arr;
}

BOOL	CKKDirArray::add(CString path)
{
	//neu array dir da day
	if(total_dir==capable)	
		return	FALSE;

	dir_arr[total_dir].pick(path);
	dir_arr[total_dir].listFiles();
	total_dir++;
	return TRUE;
}

BOOL CKKDirArray::add(CKKDir &kk_dir)
{
	if(total_dir==capable)	
		return	FALSE;

	dir_arr[total_dir]=kk_dir;
	total_dir++;
	return TRUE;
}
BOOL CKKDirArray::refresh()
{
	int i;
	for( i=0;i<total_dir;i++)
		dir_arr[i].listFiles();
	return i;
}

