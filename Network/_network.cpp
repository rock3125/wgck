#include <precomp_header.h>

#include <network/network.h>

// Broadcast to Local Network
// A broadcast can be made to the locally attached network by setting sa_netnum 
// to binary zeros and sa_nodenum to binary ones. This broadcast may be sent to 
// the primary network for the device, or to all locally attached networks at the 
// option of the service provider. Broadcasts to the local network are not propagated 
// by routers. 


//==========================================================================

TNetwork::TNetwork( TApp* _app, const short port )
	: kGamePort( port ),
	  app( _app )
{
	winsockStarted = false;
	clientStarted = false;
	clientConnected = false;
	serverStarted = false;
	readyToWrite = false;
	scoreUpdated = false;

	// start comms (both client & server)
	TString errStr;
	if ( !StartWinsock(errStr) )
	{
		errStr = "\n" + errStr + "\n";
		WriteString( errStr.c_str() );
		PostCond( false );
	}
};


TNetwork::~TNetwork( void )
{
	StopNetwork();
};


void TNetwork::StopNetwork( void )
{
	CloseSocket();

	// shutdown comms
	CloseWinsock();
}


void TNetwork::CloseSocket( SOCKET sock )
{
	closesocket(sock);
};


void TNetwork::CloseSocket( void )
{
	closesocket(tcpInSock);
	WSASetLastError(0);
	clientStarted = false;
	clientConnected = false;
	serverStarted = false;

	for ( size_t i=0; i<kMaxPlayers; i++ )
	{
		if ( sockets[i]!=INVALID_SOCKET )
		{
			used[i] = false;
			closesocket(sockets[i]);
			sockets[i] = INVALID_SOCKET;
		}
	}

	WSASetLastError(0);
};


bool TNetwork::StartWinsock( TString& errStr )
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


void TNetwork::CloseWinsock( void )
{
	if ( winsockStarted )
	{
		WSACleanup();
		winsockStarted = false;
	}
};


void TNetwork::DoSocket( HWND hWnd, TApp* app, WPARAM wParam, LPARAM lParam )
{
	int error = WSAGETSELECTERROR(lParam);
	int event = WSAGETSELECTEVENT(lParam);

	SOCKET socket = wParam;

	switch ( event )
	{
		case FD_WRITE:	// set initially, and everytime the queue is ready to receive new data
						// after an error occurs (WSAWOULDBLOCK is set in send if can't send
			{
				readyToWrite = true;
				break;
			}

		case FD_CONNECT:
			{
				//
				// we are being notified of the completion of a connect.
				//
				if ( error==0 )
				{
					if ( clientStarted && !clientConnected )
					{
						clientConnected = true;
						app->ConnectionCompleted( socket );
					}
				}
				else
				{
					clientConnected = false;
					clientStarted = false;
					CloseSocket();
				}
				break;
			}

		case FD_ACCEPT:
			{
				// a new TCP connection has come in.  Accept it, set
				// up the socket info
				if ( error==0 )
				{
					SOCKET newSock = 0;
					size_t index = 0;
					if ( !app->AcceptPlayer(socket,newSock,index) )
					{
						closesocket( socket );
					}
					else
					{
						sockets[index-1] = newSock;
					}
				}
				else
				{
				}
				break;
			}
		
		case FD_READ:
			{
				if ( clientConnected )
				{
					while ( DoClientRead( socket, app )>0 );
				}
				else if ( serverStarted )
				{
					while ( DoServerRead( socket, app )>0 );
				}
				break;
			}
			
		case FD_CLOSE:
			{
				//
				// the other side closed the connection.  close the socket,
				// to clean up internal resources.
				//
				app->DropPlayer( socket );
				break;
			}

		default:
			break;
	}
}

			
		
bool TNetwork::StartServer( HWND hWnd, TString& errStr )
{
	int nRc;
	struct sockaddr_in myaddr;

	memset(&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = PF_INET;
	myaddr.sin_port = htons(kGamePort);

	WriteString( "Network: Starting server on port %d\n", kGamePort );

	if ( !serverStarted )
	{
		tcpInSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(tcpInSock != SOCKET_ERROR)
		{
			nRc = bind(tcpInSock, (struct sockaddr FAR *)&myaddr,
						  sizeof(myaddr));
			listen( tcpInSock, SOMAXCONN );

			//
			// Use WSAAsyncSelect() to notify ourselves of network
			// events.  Since this is UDP, we'll never see FD_CONNECT,
			// FD_ACCEPT, or FD_CLOSE, but we can still register for
			// them.
			//
			nRc = WSAAsyncSelect( tcpInSock,
									hWnd,				// we want this window to be recipient
									WM_SOCKMESSAGE,		// message to send
									FD_READ  |			// tell me when new data's here
									FD_WRITE |			// tell me when there's buffer space
									FD_CONNECT |		// tell me when the below connect is done
									FD_ACCEPT |
									FD_CLOSE );			// tell me if the other side closes
			sockets[serverId-1] = tcpInSock;
			serverStarted = true;
		}
		else
			return false;
	}
	return serverStarted;
}
	 

bool TNetwork::StartClient( HWND hWindow, const TString& server, TString& errStr )
{
	if ( clientStarted || clientConnected )
		return true;

	// get server's details (address)
    LPHOSTENT lpHostEntry;
	lpHostEntry = gethostbyname(server.c_str());
    if (lpHostEntry == NULL)
    {
        errStr = "gethostbyname()";
        return false;
    }

	WriteString( "Network: Starting client connection to %s on port %d\n", server.c_str(), kGamePort );

	SOCKADDR_IN remoteAddress;
	remoteAddress.sin_family = AF_INET;
	remoteAddress.sin_addr = *((LPIN_ADDR)*lpHostEntry->h_addr_list);
	remoteAddress.sin_port = htons(kGamePort);

	// initialize the structure that will be passed to bind
	// as the local address.  We'll take any local address
	// and any local port.
	SOCKADDR_IN  myaddr;
	myaddr.sin_port = 0;
	myaddr.sin_addr.s_addr = 0;
	myaddr.sin_family = PF_INET;
				
	// create a socket.
	tcpInSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if ( tcpInSock==INVALID_SOCKET )
	{
		errStr = "socket()";
		return false;
	}

	// set the socket to non-blocking
	unsigned long dontblock = 1;
	ioctlsocket( tcpInSock, FIONBIO, &dontblock );
				
	// bind the socket to the local address
	int nRc = bind( tcpInSock, (struct sockaddr FAR *)&myaddr, sizeof(myaddr) );
	if(nRc == SOCKET_ERROR)
	{
		errStr = "bind()";
		closesocket(tcpInSock);
		tcpInSock = INVALID_SOCKET;
		return false;
	}
				
	//
	// notify WinSock that we would like to know when the connection
	// operation is complete.  If we're using UDP, it probably wouldn't
	// block anyway, but that's not important.  When we receive this
	// notification, we'll know we're ready to go
	//
	nRc = WSAAsyncSelect( tcpInSock,
							hWindow,			// we want this window to be recipient
							WM_SOCKMESSAGE,		// message to send
							FD_READ  |			// tell me when new data's here
							FD_WRITE |			// tell me when there's buffer space
							FD_CONNECT |		// tell me when the below connect is done
							FD_CLOSE );			// tell me if the other side closes
				
	if( nRc==SOCKET_ERROR )
	{
		errStr = "WSAASyncSelect()";
		closesocket(tcpInSock);
		tcpInSock = INVALID_SOCKET;
		return false;
	}
	
	sockets[serverId-1] = tcpInSock;
	clientStarted = true;

	//
	// calling connect on a UDP socket will just set the default
	// address to send to, so we can use send() as well as
	// sendto() for udp.  It's merely a convenience, and not
	// necessary for UDP.
	//
	nRc = connect( tcpInSock,
					  (struct sockaddr FAR *) &(remoteAddress),
					  sizeof(remoteAddress));

	if( nRc==SOCKET_ERROR && WSAGetLastError()!=WSAEWOULDBLOCK )
	{
		closesocket(tcpInSock);
		tcpInSock = INVALID_SOCKET;
		errStr = "Could not connect";
	}
	return true;
};


bool TNetwork::ProcessOutQueue( void )
{
	byte buf[1024];
	SOCKET sock;
	size_t size;
	if ( readyToWrite ) // ready to write is true if we can write
	{
		if ( outqueue.GetNext( sock, size, buf ) ) // get data if possible, but don't remove it
		{
			int cnt  = send( sock, (char*)buf, size, 0 );
			if ( cnt==SOCKET_ERROR ) // socket out queue full?
			{
				size_t err = WSAGetLastError();
				if ( err==WSAEWOULDBLOCK ) // set if queue full
					readyToWrite = false; // wait for this flag to clear
				else
				{
					outqueue.RemoveMessage(); // message could not be sent, remove it from the queue
					app->DropPlayer( sock );
					WSASetLastError(0);
				}
			}
			else
				outqueue.RemoveMessage(); // message was send, remove it from the queue
			return readyToWrite; // return whether we are ready for the next send
		}
	}
	return false;
};


size_t TNetwork::DoClientRead( SOCKET sock, TApp* app )
{
	byte buf[4096];
	int s = Read( sock, buf, 4096 );
	size_t initSize = s;
	size_t index = 0;
	while ( s>0 )
	{
		int sub = app->ProcessData( sock, s, &buf[index] );
		// more traffic?
		if ( s>0 )
			s -= sub;
		index += sub;
	}
	return initSize;
};


size_t TNetwork::DoServerRead( SOCKET sock, TApp* app )
{
	byte buf[4096];
	int s = Read( sock, buf, 4096);
	size_t originalSize = s;
	int index = 0;
	while ( s>0 )
	{
		int sub = app->ProcessData( sock, s, &buf[index] );
		// more traffic to come?
		if ( s>0 )
			s -= sub;
		index += sub;
	}
	return originalSize;
};


void TNetwork::SetupGuest( HWND hWnd, const TString&  _playerName, 
						   size_t port, const TString& _hostName )
{
	WriteString( "starting network join\n" );

	playerName = _playerName;
	hostName = _hostName;
	kGamePort = port;

	for ( size_t i=0; i<kMaxPlayers; i++ )
		used[i] = false;

	// now we join a host
	TString errStr;
	WriteString( "joining server\n" );
	if ( !StartClient( hWnd, hostName, errStr ) )
	{
		TString msg = "Could not connect to " + hostName + ".\n";
		msg = msg + "(" + errStr + ")";
		::Message( app, msg.c_str(), "Error", MB_OK | MB_ICONERROR );
	}
	WriteString( "network join done\n\n" );
};


void TNetwork::SetupHost( HWND hWnd, TApp* app, const TString&  _playerName, 
						  const TString& _gameName, size_t port )
{
	WriteString( "starting network host\n" );
	for ( size_t i=0; i<kMaxPlayers; i++ )
		used[i] = false;

	kGamePort = port;
	myId = serverId; // always the server!
	used[myId-1] = true;

	playerName = _playerName;
	gameName = _gameName;
	names[myId-1] = _playerName;

	// now we are hosting
	TString errStr;
	TString hostIP;
	WriteString( "starting tcp/ip server\n" );
	if ( !StartServer( hWnd, errStr ) )
	{
		TString msg = "Error hosting game: " + errStr;
		::Message( app, msg.c_str(), "Error",
					  MB_OK | MB_ICONERROR );
		CloseSocket();
	}
	WriteString( "network host done\n\n" );
};


size_t TNetwork::Read( SOCKET socket, byte* buffer, size_t size )
{
	int nRc = recv( socket, (char*)buffer, size, 0 );
	if (nRc == SOCKET_ERROR)
	{
		size_t err = WSAGetLastError();
		if ( err==WSAEWOULDBLOCK )
		{
			return 0;
		}
		else
		{
			WSASetLastError(0);
			return 0;
		}
	}
	return size_t(nRc);
};


void TNetwork::WriteSock( SOCKET sock, const byte* buffer, size_t size )
{
	// add a message to the queue for writing
	outqueue.Add( sock, size, buffer );
};


byte TNetwork::MyId( void ) const
{
	return myId;
};

void TNetwork::MyId( byte id )
{
	myId = id;
};

byte TNetwork::ServerId( void ) const
{
	return serverId;
};

void TNetwork::ServerId( byte id )
{
	serverId = id;
};

byte TNetwork::ClientSide( void ) const
{
	return clientSide;
};

void TNetwork::ClientSide( byte cside )
{
	clientSide = cside;
};

byte TNetwork::ClientTeam( void ) const
{
	return clientTeam;
};

void TNetwork::ClientTeam( byte cteam )
{
	clientTeam = cteam;
};

SOCKET TNetwork::Sockets( byte index ) const
{
	return sockets[index];
};

void TNetwork::Sockets( byte index, SOCKET s )
{
	sockets[index] = s;
};

bool TNetwork::Used( byte index ) const
{
	return used[index];
};

void TNetwork::Used( byte index, bool u )
{
	used[index] = u;
};

const TString& TNetwork::PlayerName( void ) const
{
	return playerName;
};

void TNetwork::PlayerName( const TString& n )
{
	playerName = n;
};

const TString& TNetwork::HostName( void ) const
{
	return hostName;
};

void TNetwork::HostName( const TString& h )
{
	hostName = h;
};

const TString& TNetwork::GameName( void ) const
{
	return gameName;
};

void TNetwork::GameName( const TString& n )
{
	gameName = n;
};

const TString& TNetwork::Names( byte index ) const
{
	return names[index];
};

void TNetwork::Names( byte index, const TString& n )
{
	names[index] = n;
};

//==========================================================================

