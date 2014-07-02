#include "client.h"

HWND		dlgHwnd=0,h_messlist=0,h_messtext=0;
HANDLE 		h_connect_thread=0,h_send_thread=0,h_recv_thread=0;
SOCKET		m_socket;
WSADATA		wsaData;
UINT		timerID;

sockaddr_in clientService;
char		ipstr[32],portstr[8],send_buf[128]="Client:   ",recv_buf[128];
int			row=0;


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
		lvi_additem(h_messlist,row++,recv_buf);	
		//start new recv_thread
		h_recv_thread=CreateThread(0,1024,(LPTHREAD_START_ROUTINE)&recv_thread,0,0,0);
		break;
	case WM_CLOSE:
		KillTimer(hwnd,timerID);
		EndDialog(hwnd,0);
		break;
	case WM_SENDOK:
		//in mes vua send len list
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

			//SetTimer(hwnd,timerID,3000,0);

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
	SetTimer(dlgHwnd,timerID,3000,0);

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
	SetTimer(dlgHwnd,timerID,30000,0);
	int byterecv=SOCKET_ERROR;

	while (SOCKET_ERROR==byterecv)			//neu recv error thi lap lai 
		{byterecv=recv(m_socket,recv_buf,128,0);}

	SendMessage(dlgHwnd,WM_RECVOK,byterecv,0);

	ExitThread(0);
	return byterecv;
}


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