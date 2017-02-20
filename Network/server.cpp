#include <precomp_header.h>

#include <network/server.h>
#include <network/packet.h>

//==========================================================================

TServer::TServer( void )
{
	port = 0;
	winsockStarted = false;
	serverStarted = false;

	for ( size_t i=0; i<kMaxPlayers; i++ )
	{
		players[i] = new DataParticulars(i+1);
		used[i] = false;
	}
};


TServer::~TServer( void )
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


bool TServer::StartServer( short nPort, TString& errStr )
{
	port = nPort;

	if ( serverStarted )
	{
		errStr = "Error, server already started";
		return false;
	}

	if ( !winsockStarted )
	if ( !StartWinsock( errStr ) )
		return false;

	//
	// Create a UDP/IP datagram socket
	//
	theSocket = socket(AF_INET,		// Address family
					   SOCK_DGRAM,  // Socket type
					   IPPROTO_UDP);// Protocol
	if (theSocket == INVALID_SOCKET)
	{
		errStr = "socket()";
		return false;
	}

	
	//
	// Fill in the address structure
	//
	saServer.sin_family = AF_INET;
	saServer.sin_addr.s_addr = INADDR_ANY; // Let WinSock assign address
	saServer.sin_port = htons(nPort);	   // Use port passed from user

	//
	// bind the name to the socket
	//
	int nRet;

	nRet = bind(theSocket,				// Socket descriptor
				(LPSOCKADDR)&saServer,  // Address to bind to
				sizeof(struct sockaddr)	// Size of address
				);
	if (nRet == SOCKET_ERROR)
	{
		errStr = "bind()";
		closesocket(theSocket);
		return false;
	}

	// disable blocking mode
	unsigned long param = 1;
	int res = ioctlsocket( theSocket, FIONBIO, &param );
	if ( res!=0 )
		return false;

	param = 1;
	res = ioctlsocket( theSocket, SIO_ENABLE_CIRCULAR_QUEUEING, &param );
	if ( res!=0 )
		return false;

/*
	// get server name and IP
	int nLen;
	nLen = sizeof(SOCKADDR);
	char szBuf[256];

	nRet = gethostname(szBuf, sizeof(szBuf));
	if (nRet == SOCKET_ERROR)
	{
		errStr = "gethostname()";
		closesocket(theSocket);
		return false;
	}
	hostName = szBuf;
	hostIP = IntToString(saServer.sin_addr.S_un.S_un_b.s_b1) + ".";
	hostIP = hostIP + IntToString(saServer.sin_addr.S_un.S_un_b.s_b2) + ".";
	hostIP = hostIP + IntToString(saServer.sin_addr.S_un.S_un_b.s_b3) + ".";
	hostIP = hostIP + IntToString(saServer.sin_addr.S_un.S_un_b.s_b4);
*/
	serverStarted = true;
	WSASetLastError(0);

	return true;
}


void TServer::StopServer( void )
{
	if ( serverStarted )
	{
		closesocket(theSocket);
		serverStarted = false;
	}
};


size_t TNetwork::Read( SOCKADDR_IN& address, byte* buffer, size_t size )
{
	int nLen, nRet;

	if ( !serverStarted )
		return 0;

	nLen = sizeof(struct sockaddr);
	nRet = recvfrom(theSocket,			// Bound socket
					(char*)buffer,		// Receive buffer
					size,					// Size of buffer in bytes
					MSG_PEEK,				// Flags
					(LPSOCKADDR)&address,	// Buffer to receive client address 
					&nLen);					// Length of client address buffer
	if ( nRet==SOCKET_ERROR )
	{
		int err = WSAGetLastError();
		WSASetLastError(0);
		// WSAEWOULDBLOCK
		return 0;
	}
	if ( nRet==0 )
		return 0;

	nLen = sizeof(struct sockaddr);
	nRet = recvfrom(theSocket, (char*)buffer, size, 0,
					(LPSOCKADDR)&address, &nLen);

	return size_t(nRet);
};


void TNetwork::Write( SOCKADDR_IN& address, byte* buffer, size_t size )
{
	if ( !serverStarted && !clientStarted )
		return;

	int nLen = sizeof( clientAddress );
	int cnt  = sendto(theSocket, (char*)buffer, size, 0, 
					  (LPSOCKADDR)&address, nLen );
	if ( cnt==SOCKET_ERROR )
	{
		WSAGetLastError();
		WSASetLastError(0);
	}
};


void TServer::Write( byte playerId, byte* buffer, size_t size )
{
	if ( !serverStarted )
		return;
	if ( !used[playerId-1] )
		return;
	Write( clientAddresses[playerId-1], buffer, size );
};


bool TServer::Started( void ) const
{
	return serverStarted;
};


byte TServer::CheckNewPlayer( const TString& _name, const byte& _side, 
							  SOCKADDR_IN& clientAddress, TString& errStr )
{
	TString _nm = _name.lcase();

	size_t firstAvail = 0;
	bool duplicateName = false;
	for ( size_t i=0; i<kMaxPlayers; i++ )
	{
		if ( !used[i] && firstAvail==0 )
		{
			firstAvail = i+1;
		}
		if ( !used[i] && name[i].length() > 0 )
		{
			if ( name[i].lcase() == _nm )
				duplicateName = true;
		}
	}
	if ( duplicateName )
	{
		errStr = "that name is already taken";
		return 0;
	}
	if ( firstAvail==0 )
	{
		errStr = "the game is full";
		return 0;
	}

	// slot in the new player
	commsTime[firstAvail-1] = ::GetTickCount();
	name[firstAvail-1] = _name;
	used[firstAvail-1] = true;
	side[firstAvail-1] = _side;
	active[firstAvail-1] = false;
	memcpy( &clientAddresses[firstAvail-1], &clientAddress, sizeof(clientAddress) );

	return byte(firstAvail);
};


void TServer::AddNewPlayer( byte playerId, TString& _name )
{
	active[playerId-1] = true;
	_name = name[playerId-1];
	commsTime[playerId-1] = ::GetTickCount();
};


void TServer::RemovePlayer( byte playerId )
{
	active[playerId-1] = false;
	used[playerId-1] = false;
	commsTime[playerId-1] = ::GetTickCount(); // disconnect time
};


void TServer::SetData( byte playerId, const DataParticulars& d )
{
	commsTime[playerId-1] = ::GetTickCount();
	*players[playerId-1] = d;
	tanks[playerId-1].Set( &d );
};


DataParticulars& TServer::GetData( byte playerId ) const
{
	return *players[playerId-1];
};


const TString& TServer::Name( byte playerId ) const
{
	return name[playerId-1];
};

//==========================================================================

