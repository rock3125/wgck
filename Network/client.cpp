#include <precomp_header.h>

#include <network/client.h>
#include <network/packet.h>

//==========================================================================

TClient::TClient( void )
{
	port = 0;
	winsockStarted = false;
	clientConnected = false;

	for ( size_t i=0; i<kMaxPlayers; i++ )
	{
		players[i] = new DataParticulars(i+1);
		used[i] = false;
	}
};


TClient::~TClient( void )
{
	for ( size_t i=0; i<kMaxPlayers; i++ )
	{
		delete players[i];
	}
	if ( winsockStarted )
	{
		CloseWinsock();
	}
}


bool TClient::StartWinsock( TString& errStr )
{
	if ( winsockStarted )
	{
		errStr = "Already started";
		return false;
	}

	WORD wVersionRequested = MAKEWORD(1,1);
	WSADATA wsaData;

	//
	// Initialize WinSock and check version
	//
	int nRet = WSAStartup(wVersionRequested, &wsaData);
	if (wsaData.wVersion != wVersionRequested)
	{	
		errStr = "Wrong version of winsock";
		return false;
	}

	return true;
};


void TClient::CloseWinsock( void )
{
	if ( winsockStarted )
	{
		WSACleanup();
		WSASetLastError(0);
		winsockStarted = false;
	}
};


bool TClient::StartClient( const TString& server, size_t _port, TString& errStr )
{
	//
	// Find the server
	//
	port = _port;
    LPHOSTENT lpHostEntry;

	if ( clientConnected )
	{
		errStr = "Error, already connected";
		return false;
	}

	lpHostEntry = gethostbyname(server.c_str());
    if (lpHostEntry == NULL)
    {
        errStr = "gethostbyname()";
        return false;
    }

	//
	// Create a UDP/IP datagram socket
	//
	theSocket = socket(AF_INET,			// Address family
					   SOCK_DGRAM,		// Socket type
					   IPPROTO_UDP);	// Protocol
	if (theSocket == INVALID_SOCKET)
	{
		errStr = "socket()";
		return false;
	}

	saServer.sin_family = AF_INET;
	saServer.sin_addr = *((LPIN_ADDR)*lpHostEntry->h_addr_list);
										// ^ Server's address
	saServer.sin_port = htons(port);	// Port number from command line

	// disable blocking mode
	unsigned long param = 1;
	ioctlsocket( theSocket, FIONBIO, &param );

	clientConnected = true;
	WSASetLastError(0);
	return true;
}


void TClient::StopClient( void )
{
	if ( clientConnected )
	{
		closesocket(theSocket);
		WSASetLastError(0);
		clientConnected = false;
	}
};


size_t TClient::Read( byte* buffer, size_t size )
{
	if ( !clientConnected )
		return 0;

	int nFromLen = sizeof(struct sockaddr);
	int nRet = recvfrom(theSocket,			// Bound socket
					(char*)buffer,			// Receive buffer
					size,					// Size of buffer in bytes
					MSG_PEEK,				// Flags
					(LPSOCKADDR)&saServer,	// Buffer to receive client address 
					&nFromLen);				// Length of client address buffer
	if ( nRet==SOCKET_ERROR )
	{
		int err = WSAGetLastError();
		WSASetLastError(0);
		// WSAEWOULDBLOCK
		return 0;
	}
	if ( nRet==0 )
		return 0;

	nFromLen = sizeof(struct sockaddr);
	recvfrom(theSocket,						// Socket
			 (char*)buffer,					// Receive buffer
			 nRet,							// Length of receive buffer
			 0,								// Flags
			 (LPSOCKADDR)&saServer,			// Buffer to receive sender's address
			 &nFromLen);					// Length of address buffer
	if (nRet == SOCKET_ERROR)
	{
		WSASetLastError(0);
		return 0;
	}
	return size_t(nRet);
};


bool TClient::Write( const byte* buffer, size_t size, TString& errStr )
{
	if ( !clientConnected )
	{
		errStr = "not connected";
		return false;
	}

	int nRet = sendto(theSocket,			// Socket
				  (char*)buffer,			// Data buffer
				  size,						// Length of data
				  0,						// Flags
				  (LPSOCKADDR)&saServer,	// Server address
				  sizeof(struct sockaddr)); // Length of address
	if (nRet == SOCKET_ERROR)
	{
		errStr = "sendto()";
		closesocket(theSocket);
		clientConnected = false;
		WSASetLastError(0);
		return false;
	}
	return true;
};


bool TClient::Connected( void ) const
{
	return clientConnected;
}


void TClient::AddPlayer( byte playerId, byte _side, const TString& _name, 
						 const DataParticulars& d )
{
	used[playerId-1] = true;
	side[playerId-1] = _side;
	name[playerId-1] = _name;
	*players[playerId-1] = d;
};


void TClient::RemovePlayer( byte playerId )
{
	used[playerId-1] = false;
};


void TClient::SetData( byte playerId, const DataParticulars& d )
{
	*players[playerId-1] = d;
	tanks[playerId-1].Set( &d );
};


const DataParticulars& TClient::GetData( byte playerId ) const
{
	return *players[playerId-1];
};


const TString& TClient::Name( byte playerId ) const
{
	return name[playerId-1];
};

//==========================================================================

