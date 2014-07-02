#include <windows.h>
#include <commctrl.h>
#include "resource.h"
#include <stdio.h>
#include <string.h>
#include <winuser.h>
//#include <winsock2.h>
//#include <ws2tcpip.h>


#pragma comment (lib,"ws2_32.lib")
#pragma  comment (lib, "comctl32.lib")

#define WM_CONNECTOK	WM_USER+1
#define WM_RECVOK		WM_USER+2
#define WM_SENDOK		WM_USER+3
#define WM_LISTENOK		WM_USER+4
#define WM_ACCEPTOK		WM_USER+5


#define D_MESS			10
#define C2S_MESS_LEN	16
#define D_PORT			10+16
#define D_IP			10+16+8
#define TIME_OUT		10*60*1000

LRESULT CALLBACK dlgproc(HWND hwnd,UINT mess,WPARAM wparam,LPARAM lparam);

bool	lvi_additem(HWND hlist,int index, char * name );
void CALLBACK connect_thread();
void CALLBACK send_thread();
int CALLBACK recv_thread();
int show_local_sin(SOCKET s);
bool is_chg2s_mess();
void CALLBACK listen_thread ();
void	CALLBACK	accept_thread();
int show_peer_sin(SOCKET s);
bool change_to_server(HWND hwnd);
bool connect_to_server(HWND hwnd);
bool is_con2s_mess();
bool get_in_addr(char *host_name,IN_ADDR &addr);