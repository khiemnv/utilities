#include <windows.h>
#include <commctrl.h>
#include "resource.h"
#include <stdio.h>

#include <winuser.h>

#pragma comment (lib,"ws2_32.lib")
#pragma  comment (lib, "comctl32.lib")

#define WM_CONNECTOK	WM_USER+1
#define WM_RECVOK		WM_USER+2
#define WM_SENDOK		WM_USER+3

LRESULT CALLBACK dlgproc(HWND hwnd,UINT mess,WPARAM wparam,LPARAM lparam);

bool	lvi_additem(HWND hlist,int index, char * name );
void CALLBACK connect_thread();
void CALLBACK send_thread();
int CALLBACK recv_thread();
int show_local_sin(SOCKET s);