// CheckPE.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include "KKFile.h"

int _tmain(int argc, _TCHAR* argv[])
{
	char filename[MAX_PATH];

	printf("Input file name: ");
	gets(filename);

	CKKFile tulu(filename);

	printf("FileName: %s\nMD5: %s\nPack: %d\n",tulu.fileName,tulu.MD5hexview,tulu.packed);
	if (tulu.PEInjected)
		printf("=======\nCo the nhiem lay file\n\n%s",tulu.PEInjectNote);
	printf("\nPackinfo: %s\n",tulu.Packinfo);

	if (tulu.Overlay == -1)
		printf("Detect Overlay Error!\n");
	printf("Detect Overlay result: %d\n",tulu.Overlay);

	return 0;
}