#include "c2s.h"

HWND		dlgHwnd=0,h_messlist=0,h_messtext=0;
HANDLE 		h_connect_thread=0,h_send_thread=0,h_recv_thread=0,
h_listen_thread=0,h_accept_thread=0;
SOCKET		m_socket;
WSADATA		wsaData;
UINT		timerID;

sockaddr_in clientService;
sockaddr_in service;
char		ipstr[32],portstr[8],send_buf[128]="Client:   ",recv_buf[128];
int			row=0;
char *chg2s_mess="change_to_server";
char *con2s_mess="connect_to_serve";


LRESULT CALLBACK dlgproc(HWND hwnd,UINT mess,WPARAM wparam,LPARAM lparam)
{
	LVITEM		lvi;
	LVCOLUMN	lvc;

	switch(mess)
	{
	case WM_INITDIALOG:
		dlgHwnd=hwnd;
		h_messlist=GetDlgItem(dlgHwnd,IDMESSLIST);
		h_messtext=GetDlgItem(dlgHwnd,IDMESSTEXT);

		//create col
		lvc.mask=LVCF_TEXT+LVCF_WIDTH;
		lvc.cx=300;
		lvc.pszText="all mess";
		ListView_InsertColumn(h_messlist,0,&lvc);

		//init lvi set
		lvi.mask=LVIF_TEXT;
		lvi.cchTextMax=128;
		lvi.iSubItem=0;

		SetWindowText(GetDlgItem(hwnd,IDIP),"127.0.0.1");
		SetWindowText(GetDlgItem(hwnd,IDPORT),"1000");
		break;
	case WM_TIMER:
		TerminateThread(h_connect_thread,0);
		KillTimer(hwnd,timerID);
		lvi_additem(h_messlist,row++,"no connect");
		TerminateThread(h_send_thread,0);
		TerminateThread(h_recv_thread,0);
		break;
	case WM_CONNECTOK:
		KillTimer(hwnd,timerID);
		//hien thong bao connected
		lvi_additem(h_messlist,row++,"connected");
		show_local_sin(m_socket);
		//terminate recent recv_thread		(chu y)
		TerminateThread((HANDLE)h_recv_thread,0);

		//start new recv_thread
		h_recv_thread=CreateThread(0,1024,(LPTHREAD_START_ROUTINE)&recv_thread,0,0,0);
		break;
	case WM_RECVOK:
		KillTimer(hwnd,timerID);
		//in mess tu buf len man hinh
		
		if(is_chg2s_mess())
		{
			lvi_additem(h_messlist,row++,"changing to server ... ");
			change_to_server(hwnd);
		}
		else if(is_con2s_mess())
		{
			lvi_additem(h_messlist,row++,"connect to peer server ...");
			connect_to_server(hwnd);
		}
		else
		{
			recv_buf[wparam]=0;
			lvi_additem(h_messlist,row++,recv_buf);
			//start new recv_thread
			h_recv_thread=CreateThread(0,1024,(LPTHREAD_START_ROUTINE)&recv_thread,0,0,0);
		}
		break;
	case WM_LISTENOK:
		//teminate recent listen_thread,recv_thread	(co the ko can thiet)
		//start accept_thread
		h_accept_thread=CreateThread(0,1024,(LPTHREAD_START_ROUTINE)&accept_thread,0,0,0);
		break;
	case WM_ACCEPTOK:
		//lvi_additem(h_messlist,row++,"connected");
		//terminate recent accept (co the ko can thiet)
		//start recv_thread
		show_peer_sin(m_socket);
		h_recv_thread=CreateThread(0,1024,(LPTHREAD_START_ROUTINE)&recv_thread,0,0,0);
		break;
	case WM_CLOSE:
		KillTimer(hwnd,timerID);
		EndDialog(hwnd,0);
		break;
	case WM_SENDOK:
		//in mes vua send len list
		send_buf[wparam]=0;
		lvi_additem(h_messlist,row++,send_buf);
		break;
	case WM_COMMAND:
		int wID=LOWORD(wparam);
		switch (wID)
		{
		case IDCONNECT:
			//terminte thread
			TerminateThread(h_connect_thread,0);
			TerminateThread(h_recv_thread,0);
			WSACleanup();

			//SetTimer(hwnd,timerID,TIME_OUT,0);

			//initialize winsock
			if (0!=WSAStartup( MAKEWORD(2,2), &wsaData ))
				MessageBox(0,"initialize winsock error","error!",MB_OK);
			
			//tao socket
			m_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
			if ( m_socket == INVALID_SOCKET )
			{
				MessageBox(0,"WSAGetLastError()","error!",MB_OK);
				WSACleanup();
        	}
			//hien thong bao dang connect
			lvi_additem(h_messlist,row++,"connecting");
			//start new connect_thread
			h_connect_thread=CreateThread(NULL,1024,(LPTHREAD_START_ROUTINE )&connect_thread,0,0,0);
			break;

		case IDSEND:
			SetFocus(h_messtext);
			//terminate recent send_thread
			TerminateThread((HANDLE)h_send_thread,0);
			//start new send_thread
			h_send_thread=CreateThread(NULL,1024,(LPTHREAD_START_ROUTINE )&send_thread,0,0,0);
			break;
		}
		return 1;
	}
	return 0;
}


///////////////////////
void CALLBACK connect_thread()
{
	SetTimer(dlgHwnd,timerID,TIME_OUT,0);

	GetDlgItemText(dlgHwnd,IDIP,ipstr,32);
	GetDlgItemText(dlgHwnd,IDPORT,portstr,8);

	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr(ipstr);
	clientService.sin_port = htons( atoi(portstr) );

	while(SOCKET_ERROR==connect( m_socket, (SOCKADDR*) &clientService, sizeof(clientService) ));		//lap lai neu co loi

	SendMessage(dlgHwnd,WM_CONNECTOK,0,0);

	ExitThread(0);
	//return 0;
}

void CALLBACK  send_thread()
{
	int len;
	int bytesend=SOCKET_ERROR;
	//chuyen mes tu edit text vao buf
	len=GetDlgItemText(dlgHwnd,IDMESSTEXT,send_buf+10,128);
	len+=10;
	//xoa edit text
	SetDlgItemText(dlgHwnd,IDMESSTEXT,"");
//	while(bytesend=SOCKET_ERROR)
		bytesend=send(m_socket,send_buf,len,0);

	SendMessage(dlgHwnd,WM_SENDOK,bytesend,0);

	ExitThread(0);
	//return 0;
}

int CALLBACK recv_thread()
{
	SetTimer(dlgHwnd,timerID,TIME_OUT,0);
	int byterecv=SOCKET_ERROR;

	while (SOCKET_ERROR==byterecv)			//neu recv error thi lap lai 
		{byterecv=recv(m_socket,recv_buf,128,0);}

	SendMessage(dlgHwnd,WM_RECVOK,byterecv,0);

	ExitThread(0);
	return byterecv;
}

//////////////////////////////////////////////////////////////////////////
bool	lvi_additem(HWND hlist,int index, char * name )
{
	LV_ITEM	lvi;
	lvi.mask=LVIF_TEXT;
	lvi.iItem=index;
	lvi.pszText=name;
	lvi.iSubItem=0;
	ListView_InsertItem(hlist,&lvi);
	ListView_Scroll(h_messlist,0,16);
	return true;
}

int show_local_sin(SOCKET s)
{
	struct sockaddr name;
	int namelen=sizeof(name);
	int retval=getsockname(s,&name,&namelen);

	SOCKADDR_IN *_sin = (SOCKADDR_IN*)&name;
	switch(retval){
		case WSANOTINITIALISED:
			lvi_additem(h_messlist,row++,"A successful WSAStartup call must occur before using this function.");
			break;
		case WSAENETDOWN:
			lvi_additem(h_messlist,row++,"The network subsystem has failed.");
			break;
		case WSAEFAULT:
			lvi_additem(h_messlist,row++,"The name or the namelen parameter is not in a valid part of the user address space, or the namelen parameter is too small.");
			break;
		case WSAEINPROGRESS:
			lvi_additem(h_messlist,row++,"A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.");
			break;
		case WSAEINVAL:
			lvi_additem(h_messlist,row++,"The socket has not been bound to an address with bind, or ADDR_ANY is specified in bind but connection has not yet occurred.");
			break;
		case WSAENOTSOCK:
			lvi_additem(h_messlist,row++,"The descriptor is not a socket.");
			break;
		default:
			lvi_additem(h_messlist,row++,"get local name success");
			char client_port[100];
			sprintf(client_port,"sin_family:%d",_sin->sin_family);
			lvi_additem(h_messlist,row++,client_port);
			//char *t = name.sa_data;
			//u_short *sin_port=(u_short*)t;
			sprintf(client_port,"sin_port:%d sin_addr:%s",ntohs(_sin->sin_port),inet_ntoa(_sin->sin_addr));

			lvi_additem(h_messlist,row++,client_port);
	}

	return retval;
}
//////////////////////////////////////////////////////////////////////////
bool is_chg2s_mess()
{
	for (int i = 0; i < C2S_MESS_LEN; i++)
		if(chg2s_mess[i] != recv_buf[i+D_MESS])
			return false;
	return true;
}

bool change_to_server(HWND hwnd){

//get local socket name
	struct sockaddr name;
	int namelen=sizeof(name);
	int retval=getsockname(m_socket,&name,&namelen);
	
	SOCKADDR_IN *_sin = (SOCKADDR_IN*)&name;
	
	service.sin_family=AF_INET;
	service.sin_addr = _sin->sin_addr;
	service.sin_port = _sin->sin_port;
//close socket
	closesocket(m_socket);
//clean
	WSACleanup();
	//terminate accept and recv thread
	TerminateThread(h_recv_thread,0);
	TerminateThread(h_accept_thread,0);
	WSACleanup();

	lvi_additem(h_messlist,row++,"start service");

	SetTimer(hwnd,timerID,60000,0);

	//initialize winsock
	if (0!=WSAStartup( MAKEWORD(2,2), &wsaData ))
		MessageBox(0,"initialize winsock error","error!",MB_OK);

	//tao socket
	m_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if ( m_socket == INVALID_SOCKET )
	{
		MessageBox(0,"WSAGetLastError()","error!",MB_OK);
		WSACleanup();
	}

	// Bind the socket.
	//GetDlgItemText(hwnd,IDMESSTEXT,ipstr,32);
	//GetDlgItemText(hwnd,IDPORT,portstr,8);
// 	strncpy(ipstr,recv_buf+D_IP,32);
// 	strncpy(portstr,recv_buf+D_PORT,8);

	if(bind(m_socket,(SOCKADDR*) &service,sizeof(service))==SOCKET_ERROR)
	{
		lvi_additem(h_messlist,row++,"bind failed");
		closesocket(m_socket);
	}
	//start listen_thread
	h_listen_thread=CreateThread(0,1024,(LPTHREAD_START_ROUTINE)&listen_thread,0,0,0);
	return true;
}
//////////////////////////////////////////////////////////////////////////
bool is_con2s_mess()
{
	for (int i = 0; i < C2S_MESS_LEN; i++)
		if(con2s_mess[i] != recv_buf[i+D_MESS])
			return false;
		return true;
}

bool connect_to_server(HWND hwnd)
{
	//terminte thread
	TerminateThread(h_connect_thread,0);
	TerminateThread(h_recv_thread,0);

	closesocket(m_socket);

	WSACleanup();
	
	//SetTimer(hwnd,timerID,TIME_OUT,0);
	
	//initialize winsock
	if (0!=WSAStartup( MAKEWORD(2,2), &wsaData ))
		MessageBox(0,"initialize winsock error","error!",MB_OK);
	
	//tao socket
	m_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if ( m_socket == INVALID_SOCKET )
	{
		MessageBox(0,"WSAGetLastError()","error!",MB_OK);
		WSACleanup();
	}

	//hien thong bao dang connect
	lvi_additem(h_messlist,row++,"connecting");
			
	//start connect

	SetTimer(dlgHwnd,timerID,TIME_OUT,0);
	
// 	GetDlgItemText(dlgHwnd,IDIP,ipstr,32);
// 	GetDlgItemText(dlgHwnd,IDPORT,portstr,8);
	strncpy(ipstr,recv_buf+D_IP,32);
	strncpy(portstr,recv_buf+D_PORT,8);

	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr(ipstr);
	clientService.sin_port = htons( atoi(portstr) );
	
	while(SOCKET_ERROR==connect( m_socket, (SOCKADDR*) &clientService, sizeof(clientService) ));		//lap lai neu co loi
	
	SendMessage(dlgHwnd,WM_CONNECTOK,0,0);
	
	//ExitThread(0);
	//return 0;
	return true;
}
//////////////////////////////////////////////////////////////////////////
void CALLBACK listen_thread ()
{
	if (listen(m_socket,1)==SOCKET_ERROR) 
		lvi_additem(h_messlist,row++,"listen error");		//neu listen error thi lap lai
	else
		SendMessage(dlgHwnd,WM_LISTENOK,0,0);	

	ExitThread(0);
}
void CALLBACK accept_thread()
{
	//hient thong bao "Waiting for a client to connect...\n"
	lvi_additem(h_messlist,row++,"waiting for a client to connect");

	// Accept connections.
	SOCKET AcceptSocket;
	while (1) {
		AcceptSocket = SOCKET_ERROR;
		while ( AcceptSocket == SOCKET_ERROR ) {
			AcceptSocket = accept( m_socket, NULL, NULL );
		}
		lvi_additem(h_messlist,row++,"connected");
		m_socket = AcceptSocket; 
		break;
	}

	SendMessage(dlgHwnd,WM_ACCEPTOK,0,0);
	ExitThread(0);
}
int show_peer_sin(SOCKET s)
{
	struct sockaddr name;
	int namelen=sizeof(name);
	int retval=getpeername(s,&name,&namelen);

	SOCKADDR_IN *_sin = (SOCKADDR_IN*)&name;
	switch(retval){
		case WSANOTINITIALISED:
			lvi_additem(h_messlist,row++,"A successful WSAStartup call must occur before using this function.");
			break;
		case WSAENETDOWN:
			lvi_additem(h_messlist,row++,"The network subsystem has failed.");
			break;
		case WSAEFAULT:
			lvi_additem(h_messlist,row++,"The name or the namelen parameter is not in a valid part of the user address space, or the namelen parameter is too small.");
			break;
		case WSAEINPROGRESS:
			lvi_additem(h_messlist,row++,"A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.");
			break;
		case WSAENOTCONN:
			lvi_additem(h_messlist,row++,"The socket is not connected.");
			break;
		case WSAENOTSOCK:
			lvi_additem(h_messlist,row++,"The descriptor is not a socket.");
			break;
		default:
			lvi_additem(h_messlist,row++,"get peer name success");
			char client_port[100];
			sprintf(client_port,"sin_family:%d",_sin->sin_family);
			lvi_additem(h_messlist,row++,client_port);
			//char *t = name.sa_data;
			//u_short *sin_port=(u_short*)t;
			sprintf(client_port,"sin_port:%d sin_addr:%s",ntohs(_sin->sin_port),inet_ntoa(_sin->sin_addr));

			lvi_additem(h_messlist,row++,client_port);
	}

	return retval;
}