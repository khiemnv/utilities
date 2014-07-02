#include "c2s.h"

INT WINAPI WinMain(HINSTANCE 	hInstance,
				   HINSTANCE 	hPrevInstance,
				   LPSTR 		lpCmdLine,
				   INT  		nCmdShow)
{
	InitCommonControls();
	hInstance=GetModuleHandle(0);
	DialogBoxParam(hInstance, MAKEINTRESOURCE(CLIENTDLG),0,(DLGPROC )&dlgproc,0);
	ExitProcess(0);
	return 0;
}