#include <windows.h>
#include <commctrl.h>
#include "resource.h"

#include <winuser.h>
#include <stdio.h>

#pragma comment (lib,"ws2_32.lib")
#pragma  comment (lib, "comctl32.lib")

#define WM_ACCEPTOK		WM_USER+1
#define WM_RECVOK		WM_USER+2
#define WM_SENDOK		WM_USER+3
#define WM_LISTENOK		WM_USER+4

#define D_MESS			10
#define C2S_MESS_LEN	16
#define D_PORT			10+16
#define D_IP			10+16+8
#define TIME_OUT		10*60*1000
#define SLEEP_TIME		5*1000

#define L_CHG2S			1999
#define L_CON2S			2000

bool	lvi_additem(HWND hlist,int index, char * name );
void	CALLBACK	accept_thread();
void	CALLBACK	listen_thread();
int		CALLBACK	send_thread();
int		CALLBACK	recv_thread();
int get_peer_name(SOCKET s);
int show_peer_sin(SOCKET s);

bool is_chg2s_mess(char*buff);
bool is_con2s_mess(char* buff);
bool is_to_client_1(char* buff);
bool is_to_client_2(char* buff);
int make_con2s_mess(char* buff, SOCKET s);
bool get_in_addr(char *host_name,IN_ADDR &addr);

void show_wsa_error();
void show_wsa_error(char *commend);
void show_bind_addr();

// int total_connection=0;
// SOCKET		m_socket2;
// WSADATA		wsaData2;

LRESULT CALLBACK dlgproc(HWND hwnd,UINT mess,WPARAM wparam,LPARAM lparam);
