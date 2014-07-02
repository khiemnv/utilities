#include "server.h"

HWND		dlgHwnd=0,h_messlist=0,h_messtext=0;
HANDLE 		h_accept_thread=0,h_send_thread=0,
h_recv_thread=0,h_listen_thread=0;

SOCKET		m_socket;
WSADATA		wsaData;
UINT		timerID;

int total_connection=0;
SOCKET		m_socket2;
WSADATA		wsaData2;

int listening=0;

sockaddr_in service;
char		ipstr[32],portstr[8],send_buf[128]="Server:   ",recv_buf[128];
char		ipstr2[32],portstr2[8];
int			row=0;

struct sockaddr peer_name;
int peer_name_len=sizeof(peer_name);
SOCKADDR_IN *peer_sin = (SOCKADDR_IN*)&peer_name;


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
		SetWindowText(GetDlgItem(hwnd,IDPORT),"2222");
		break;
	case WM_TIMER:
		KillTimer(hwnd,timerID);
		lvi_additem(h_messlist,row++,"no connect");
		TerminateThread(h_recv_thread,0);
		break;
	case WM_CHAR:
		if ((int)wparam==0xd) 
		{
			SendMessage(hwnd,WM_COMMAND,(WPARAM)IDSEND,0);
			MessageBox(0,0,"enter press",0);
		}
		break;
	case WM_ACCEPTOK:
		if(total_connection==0)
		{
			getpeername(m_socket,&peer_name,&peer_name_len);
 			h_listen_thread=CreateThread(0,1024,(LPTHREAD_START_ROUTINE)&listen_thread,0,0,0);
			listening = 1;
		}
//inc total connection
		total_connection++;
//show peer sin		
		if(wparam == 0) 
			show_peer_sin(m_socket);
		else
		{
			show_peer_sin(m_socket2);
			send(m_socket2,"hello 2",7,0);
		}
//start recv_thread
		h_recv_thread=CreateThread(0,1024,(LPTHREAD_START_ROUTINE)&recv_thread,0,0,0);
		break;
	case WM_LISTENOK:
		//start accept_thread
		h_accept_thread=CreateThread(0,1024,(LPTHREAD_START_ROUTINE)&accept_thread,0,0,0);
		break;
	case WM_RECVOK:
		KillTimer(hwnd,timerID);
		//in mess tu buf len man hinh
		recv_buf[wparam]=0;
		lvi_additem(h_messlist,row++,recv_buf);	
		//start new recv_thread
		h_recv_thread=CreateThread(0,1024,(LPTHREAD_START_ROUTINE)&recv_thread,0,0,0);
		break;
	case WM_CLOSE:
		KillTimer(hwnd,timerID);
		EndDialog(hwnd,0);
		break;
	case WM_SENDOK:
		send_buf[wparam]=0;
		switch(lparam)
		{
		case L_CON2S:
			
			lvi_additem(h_messlist,row++,"close m_socket2");
			Sleep(SLEEP_TIME);
			closesocket(m_socket2);
			break;
		case L_CHG2S:
			
			lvi_additem(h_messlist,row++,"close m_socket");
			Sleep(SLEEP_TIME);
			closesocket(m_socket);
			break;
		default:
			//in mes vua send len list
			/*send_buf[wparam]=0;*/
			lvi_additem(h_messlist,row++,send_buf);
		}		
		break;
	case WM_COMMAND:
		int wID=LOWORD(wparam);
		DWORD optval = SO_REUSEADDR;
		switch (wID)
		{
		case IDSTART:
			//terminate accept and recv thread
			TerminateThread(h_recv_thread,0);
			TerminateThread(h_accept_thread,0);
			WSACleanup();

			lvi_additem(h_messlist,row++,"start service");

			SetTimer(hwnd,timerID,TIME_OUT,0);

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

			//set optiont to socket
			
// 			if(SOCKET_ERROR == setsockopt(m_socket,SOL_SOCKET,SO_REUSEADDR,(char*)&optval,sizeof(optval)))		
// 				show_wsa_error();
			// Bind the socket.
			GetDlgItemText(hwnd,IDIP,ipstr,32);
			GetDlgItemText(hwnd,IDPORT,portstr,8);

			strcpy(ipstr2,ipstr);
			strcpy(portstr2,portstr);

			service.sin_family=AF_INET;
			 /*service.sin_addr.s_addr=inet_addr(ipstr);*/
			get_in_addr(ipstr,service.sin_addr);

			service.sin_port = htons( atoi(portstr) );
			
			if(bind(m_socket,(SOCKADDR*) &service,sizeof(service))==SOCKET_ERROR)
			{
				show_wsa_error("bind error:");
				closesocket(m_socket);				
			}
			else
				show_bind_addr();
			//start listen_thread
			h_listen_thread=CreateThread(0,1024,(LPTHREAD_START_ROUTINE)&listen_thread,0,0,0);
			
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
void CALLBACK accept_thread()
{
	//hient thong bao "Waiting for a client to connect...\n"
	lvi_additem(h_messlist,row++,"waiting for a client to connect");

// Accept connections.
    SOCKET AcceptSocket;
   
        AcceptSocket = SOCKET_ERROR;
        while ( AcceptSocket == SOCKET_ERROR ) {
            if (total_connection==0)
				AcceptSocket = accept( m_socket, NULL, NULL );
			else
				AcceptSocket = accept(m_socket2,NULL,NULL);
        }
        lvi_additem(h_messlist,row++,"connected");

        if(total_connection==0)m_socket = AcceptSocket; 
		else 
		{
			m_socket2=AcceptSocket;
			SendMessage(dlgHwnd,WM_ACCEPTOK,2,0);
			ExitThread(0);
		}
    

	SendMessage(dlgHwnd,WM_ACCEPTOK,0,0);
	ExitThread(0);
}


void CALLBACK listen_thread ()
{
	lvi_additem(h_messlist,row++,"listening ... ");
	int temp;
	
	if(total_connection==0)
		temp=listen(m_socket,1);
	else
	{
		//initialize winsock
//  		if (0!=WSAStartup( MAKEWORD(2,2), &wsaData2 ))
//  			MessageBox(0,"initialize winsock error","error!",MB_OK);
		
		//tao socket
		m_socket2 = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
		if ( m_socket2 == INVALID_SOCKET )
		{
			MessageBox(0,"WSAGetLastError()","error!",MB_OK);
			WSACleanup();
		}
		
		//set option
		DWORD optval = SO_REUSEADDR;
		if (setsockopt(m_socket2,SOL_SOCKET,SO_REUSEADDR,(char*)&optval,sizeof(optval))==SOCKET_ERROR)
			show_wsa_error();

		// Bind the socket.
//  		GetDlgItemText(dlgHwnd,IDMESSTEXT,ipstr,32);
//  		GetDlgItemText(dlgHwnd,IDPORT,portstr,8);
// 		
// 		service.sin_family=AF_INET;
// 		service.sin_addr.s_addr=inet_addr(ipstr2);
// 		service.sin_port = htons( atoi(portstr2) );
		
		if(bind(m_socket2,(SOCKADDR*) &service,sizeof(service))==SOCKET_ERROR)
		{			
			show_wsa_error("bind fail:");
			closesocket(m_socket2);		
		}
		
		temp=listen(m_socket2,1);
	}

	if(temp == SOCKET_ERROR)
		show_wsa_error("listen error:");
	else
		SendMessage(dlgHwnd,WM_LISTENOK,0,0);	

	ExitThread(0);
}


int CALLBACK  send_thread()
{
	int len;
	int bytesend=SOCKET_ERROR;
//chuyen mes tu edit text vao buf
	len=GetDlgItemText(dlgHwnd,IDMESSTEXT,send_buf+10,128);
	len+=10;
//xoa edit text
	SetDlgItemText(dlgHwnd,IDMESSTEXT,"");
//neu la connect to server mess thi add them port, ip vao mess
	if(is_con2s_mess(send_buf)&&(total_connection>1))
	{
		len=make_con2s_mess(send_buf,m_socket2);
		bytesend=send(m_socket2,send_buf,len,0);

		if(SOCKET_ERROR == bytesend)
			show_wsa_error("send error!:");

		//TerminateThread(h_recv_thread,0);

		if (SOCKET_ERROR == closesocket(m_socket2))
			show_wsa_error("close socket 2 error!:");

		//WSACleanup();

		SendMessage(dlgHwnd,WM_SENDOK,bytesend,L_CON2S);

		ExitThread(0);
		return	bytesend;
	}
//neu la change to server mess
	if (is_chg2s_mess(send_buf))
	{
		bytesend=send(m_socket,send_buf,len,0);

		if(SOCKET_ERROR == bytesend)
			show_wsa_error("send error!:");

		//TerminateThread(h_recv_thread,0);

		if (SOCKET_ERROR ==closesocket(m_socket))
			show_wsa_error("close soket 1 error!:");

		//SendMessage(dlgHwnd,WM_SENDOK,bytesend,L_CHG2S);
		
		ExitThread(0);
		return	bytesend;
	}

	if(is_to_client_2(send_buf))
		bytesend=send(m_socket2,send_buf,len,0);
	else
		bytesend=send(m_socket,send_buf,len,0);

	SendMessage(dlgHwnd,WM_SENDOK,bytesend,0);

	ExitThread(0);
	return	bytesend;
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

int get_peer_name(SOCKET s)
{
	struct sockaddr name;
	int namelen=sizeof(name);
	int retval=getpeername(s,&name,&namelen);

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
			sprintf(client_port,"sa_family:%d",ntohs(name.sa_family));
			lvi_additem(h_messlist,row++,client_port);
			char *t = name.sa_data;
			u_short *sin_port=(u_short*)t;
			sprintf(client_port,"sin_port:%d sin_addr:%d.%d.%d.%d",ntohs(*sin_port),t[2],t[3],t[4],t[5]);

			lvi_additem(h_messlist,row++,client_port);
	}

	return retval;
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

//////////////////////////////////////////////////////////////////////////
bool is_con2s_mess(char *buff)
{
	char *con2s_mess="connect_to_serve";
	for (int i = 0; i < C2S_MESS_LEN; i++)
		if(con2s_mess[i] != buff[i+D_MESS])
			return false;
		return true;
}

int make_con2s_mess(char* buff, SOCKET s)
{
	sprintf(buff+D_PORT,"%d",ntohs(peer_sin->sin_port));
	sprintf(buff+D_IP,"%s",inet_ntoa(peer_sin->sin_addr));

	return 10+16+8+32;	//mess len used when send
}

void show_wsa_error()
{
	switch (WSAGetLastError())
	{
	case WSANOTINITIALISED:
		lvi_additem(h_messlist,row++,"A successful WSAStartup must occur before using this function.");
		break;
	case WSAENETDOWN:
		lvi_additem(h_messlist,row++,"The network subsystem has failed.");
		break;
	case WSAEINPROGRESS:
		lvi_additem(h_messlist,row++,"A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.");
		break;
	case WSAEADDRINUSE:
		lvi_additem(h_messlist,row++,"The socket's local address is already in use and the socket was not marked to allow address reuse with SO_REUSEADDR.");
		break;
	case WSAEINVAL:
		lvi_additem(h_messlist,row++,"The socket has not been bound with bind.");
		break;
	case WSAEISCONN:
		lvi_additem(h_messlist,row++,"The socket is already connected.");
		break;
	case WSAEMFILE:
		lvi_additem(h_messlist,row++,"No more socket descriptors are available.");
		break;
	case WSAENOBUFS:
		lvi_additem(h_messlist,row++,"No buffer space is available.");
		break;
	case WSAENOTSOCK:
		lvi_additem(h_messlist,row++,"The descriptor is not a socket.");
		break;
	case WSAEOPNOTSUPP:
		lvi_additem(h_messlist,row++,"The referenced socket is not of a type that supports the listen operation.");
		break;		
	case WSAEFAULT:
		lvi_additem(h_messlist,row++,"The name or the namelen parameter is not a valid part of the user address space");
		break;
	case WSAENETRESET:
		lvi_additem(h_messlist,row++,"Connection has timed out when SO_KEEPALIVE is set.");
		break;
	case WSAENOPROTOOPT:
		lvi_additem(h_messlist,row++,"The option is unknown or unsupported for the specified provider or socket (see SO_GROUP_PRIORITY limitations).");
		break;
	case WSAENOTCONN:
		lvi_additem(h_messlist,row++,"Connection has been reset when SO_KEEPALIVE is set.");
		break;	
	case WSAEWOULDBLOCK:
		lvi_additem(h_messlist,row++,"The socket is marked as nonblocking and SO_LINGER is set to a nonzero time-out value.");
		break;
	case WSAEINTR:
		lvi_additem(h_messlist,row++,"The (blocking) Windows Socket 1.1 call was canceled through WSACancelBlockingCall.");
		break;
	case WSAEACCES:
		lvi_additem(h_messlist,row++,"The requested address is a broadcast address, but the appropriate flag was not set. Call setsockopt with the SO_BROADCAST parameter to allow the use of the broadcast address.");
		break;
	case WSAESHUTDOWN:
		lvi_additem(h_messlist,row++,"The socket has been shut down; it is not possible to send on a socket after shutdown has been invoked with how set to SD_SEND or SD_BOTH.");
		break;
		
	case WSAEMSGSIZE:
		lvi_additem(h_messlist,row++,"The socket is message oriented, and the message is larger than the maximum supported by the underlying transport.");
		break;
		
	case WSAEHOSTUNREACH:
		lvi_additem(h_messlist,row++,"The remote host cannot be reached from this host at this time.");
		break;
		
	case WSAECONNABORTED:
		lvi_additem(h_messlist,row++,"The virtual circuit was terminated due to a time-out or other failure. The application should close the socket as it is no longer usable.");
		break;
		
	case WSAECONNRESET:
		lvi_additem(h_messlist,row++,"The virtual circuit was reset by the remote side executing a hard or abortive close.");
		break;
		
	case WSAETIMEDOUT:
		lvi_additem(h_messlist,row++,"The connection has been dropped, because of a network failure or because the system on the other end went down without notice.");
	break;
	default:
		lvi_additem(h_messlist,row++,"chua add loi nay");
	}
}

void show_bind_addr(){
	char temp[8];
	lvi_additem(h_messlist,row++,inet_ntoa(service.sin_addr));
	sprintf(temp,"%d",ntohs(service.sin_port));
	lvi_additem(h_messlist,row++,temp);
}

bool is_chg2s_mess(char*buff)
{
	char *chg2s_mess="change_to_server";
	for (int i = 0; i < C2S_MESS_LEN; i++)
		if(chg2s_mess[i] != buff[i+D_MESS])
			return false;
		return true;
}

void show_wsa_error(char*commend)
{
	lvi_additem(h_messlist,row++,commend);
	show_wsa_error();
}

bool get_in_addr(char *host_name,IN_ADDR &addr)
{
	DWORD dwError;

	struct hostent *remoteHost;
	/*struct in_addr addr;*/

	// If the user input is an alpha name for the host, use gethostbyname()
	// If not, get host by addr (assume IPv4)
	if (isalpha(host_name[0])) {        /* host address is a name */
		/*printf("Calling gethostbyname with %s\n", host_name);*/
		remoteHost = gethostbyname(host_name);
	} else {
		/*printf("Calling gethostbyaddr with %s\n", host_name);*/
		addr.s_addr = inet_addr(host_name);
		if (addr.s_addr == INADDR_NONE) {
			/*printf("The IPv4 address entered must be a legal address\n");*/
			return 1;
		} else
			remoteHost = gethostbyaddr((char *) &addr, 4, AF_INET);
	}

	if (remoteHost == NULL) {
		dwError = WSAGetLastError();
		if (dwError != 0) {
			if (dwError == WSAHOST_NOT_FOUND) {
				/*printf("Host not found\n");*/
				return 1;
			} else if (dwError == WSANO_DATA) {
				/*printf("No data record found\n");*/
				return 1;
			} else {
				/*printf("Function failed with error: %ld\n", dwError);*/
				return 1;
			}
		}
	} else {
		// 		printf("Function returned:\n");
		// 		printf("\tOfficial name: %s\n", remoteHost->h_name);
		// 		printf("\tAlternate names: %s\n", remoteHost->h_aliases);
		// 		printf("\tAddress type: ");
		switch (remoteHost->h_addrtype) {
		case AF_INET:
			/*			printf("AF_INET\n");*/
			break;
			// 		case AF_INET6:
			// 			printf("AF_INET\n");
			// 			break;
		case AF_NETBIOS:
			/*			printf("AF_NETBIOS\n");*/
			break;
		default:
			/*printf(" %d\n", remoteHost->h_addrtype);*/
			break;
		}
		/*printf("\tAddress length: %d\n", remoteHost->h_length);*/
		addr.s_addr = *(u_long *) remoteHost->h_addr_list[0];
		/*printf("\tFirst IP Address: %s\n", inet_ntoa(addr));*/
		return 0;
	}

}

bool is_to_client_1(char* buff)
{
	if(buff[10]=='1')
		return true;
	return false;
}
bool is_to_client_2(char* buff)
{
	if (buff[10]=='2')
		return true;
	return false;
}