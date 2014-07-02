#pragma once
#include "kkdir.h"
#define KK_NDIR_BLOCK	10
class CKKDirArray
{
public:
	CKKDirArray(void);
	~CKKDirArray(void);
	//function
	BOOL	add(CKKDir &kk_dir);
	BOOL	add(CString path);
	//rebuild file list tat ca cac dir
	BOOL	refresh();
	int		getSize(){return total_dir;}
	//vars
	CKKDir*	dir_arr;
	int		total_dir;
	int		capable;
};
