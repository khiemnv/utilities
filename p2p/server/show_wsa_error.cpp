#include "server.h"

// void show_wsa_error()
// {
// 	switch (WSAGetLastError())
// 	{
// 	case WSANOTINITIALISED:
// 		lvi_additem(h_messlist,row++,"A successful WSAStartup must occur before using this function.");
// 		break;
// 	case WSAENETDOWN:
// 		lvi_additem(h_messlist,row++,"The network subsystem has failed.");
// 		break;
// 	case WSAEINPROGRESS:
// 		lvi_additem(h_messlist,row++,"A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.");
// 		break;
// 	case WSAEADDRINUSE:
// 		lvi_additem(h_messlist,row++,"The socket's local address is already in use and the socket was not marked to allow address reuse with SO_REUSEADDR.");
// 		break;
// 	case WSAEINVAL:
// 		lvi_additem(h_messlist,row++,"The socket has not been bound with bind.");
// 		break;
// 	case WSAEISCONN:
// 		lvi_additem(h_messlist,row++,"The socket is already connected.");
// 		break;
// 	case WSAEMFILE:
// 		lvi_additem(h_messlist,row++,"No more socket descriptors are available.");
// 		break;
// 	case WSAENOBUFS:
// 		lvi_additem(h_messlist,row++,"No buffer space is available.");
// 		break;
// 	case WSAENOTSOCK:
// 		lvi_additem(h_messlist,row++,"The descriptor is not a socket.");
// 		break;
// 	case WSAEOPNOTSUPP:
// 		lvi_additem(h_messlist,row++,"The referenced socket is not of a type that supports the listen operation.");
// 		break;		
// 	case WSAEFAULT:
// 		lvi_additem(h_messlist,row++,"The name or the namelen parameter is not a valid part of the user address space");
// 		break;
// 	}
// }