#include <precomp_header.h>

//==========================================================================

#include <winsock.h>

#include <win32/win32.h>

#include <common/compatability.h>
#include <common/string.h>

#include <network/networking.h>
#include <network/packet.h>

//==========================================================================

const size_t kQueueSize = 8;

const size_t kFindHostTimeout = 5000;

TString				resolveStr = "pdv_resolve_me_1_0";
HANDLE				hNetworkThread = NULL;
HANDLE				hNetworkRespondThread = NULL;
unsigned long		networkThreadId = 0;
bool				winsockStarted = false;
bool				stopComms = false; // stops the threads if set to true
bool				serverActive = false;
bool				clientActive = false;

// udp port used for broadcasts & resolving servers
const size_t		lookupPort = 3015;

//==========================================================================

DWORD WINAPI NetworkThreadServerRespondProc( LPVOID lpParameter );

enum bufferStatus { bsFree, bsFull, bsProcessed };

struct NetworkInitSettings
{
	bool	isClient;
	bool	clientConnected;
	TString gameName;
	TString	server;
	TString errStr;
	size_t	port;
};

struct ClientQueue
{
	byte	in[kQueueSize*kNetworkBufferSize];
	size_t	inSize[kQueueSize];
	size_t	inFree;
	size_t	inCurrent;

	byte	out[kQueueSize*kNetworkBufferSize];
	size_t	outSize[kQueueSize];
	size_t	outFree;
	size_t	outCurrent;
};

struct ServerQueue
{
	SOCKET	sockets[kMaxPlayers];

	byte	in[kMaxPlayers][kQueueSize*kNetworkBufferSize];
	size_t	inSize[kMaxPlayers][kQueueSize];
	size_t	inFree[kMaxPlayers];
	size_t	inCurrent[kMaxPlayers];

	byte	out[kMaxPlayers][kQueueSize*kNetworkBufferSize];
	size_t	outSize[kMaxPlayers][kQueueSize];
	size_t	outFree[kMaxPlayers];
	size_t	outCurrent[kMaxPlayers];
};

NetworkInitSettings	nwis;
ClientQueue			client;
ServerQueue			server;

//==========================================================================

void InitQueues( void )
{
	size_t i, j;

	client.inFree = 0;
	client.inCurrent = 0;
	client.outFree = 0;
	client.outCurrent = 0;

	for ( i=0; i<kQueueSize; i++ )
	{
		client.inSize[i] = 0;
		client.outSize[i] = 0;
	}

	for ( i=0; i<kMaxPlayers; i++ )
	{
		server.inFree[i] = 0;
		server.inCurrent[i] = 0;
		server.outFree[i] = 0;
		server.outCurrent[i] = 0;

		for ( j=0; j<kQueueSize; j++ )
		{
			server.inSize[i][j] = 0;
			server.outSize[i][j] = 0;
		}
	}
};

// skip next item in client queue or return false if none available
bool SkipNextClientInQueue( void )
{
	if ( client.inFree == client.inCurrent )
	{
		return false;
	}
	client.inFree = (client.inFree+1) % kQueueSize;
	return true;
};


// copy item from client queue or return false if none available
bool GetNextClientInQueue( size_t& bufSize, byte* data )
{
	if ( client.inFree == client.inCurrent )
	{
		bufSize = 0;
		return false;
	}
	bufSize = client.inSize[client.inCurrent];
	memcpy( data, &client.in[ client.inCurrent * kNetworkBufferSize ], bufSize );
	client.inCurrent = (client.inCurrent + 1) % kQueueSize;
	return true;
};

bool GetNextClientOutQueue( size_t& bufSize, byte* data )
{
	if ( client.outFree == client.outCurrent )
	{
		bufSize = 0;
		return false;
	}
	bufSize = client.outSize[client.outCurrent];
	memcpy( data, &client.out[ client.outCurrent * kNetworkBufferSize ], bufSize );
	client.outCurrent = (client.outCurrent + 1) % kQueueSize;
	return true;
};

// add new item to client queue from the outside (coming in)
void AddClientInQueue( size_t size, const byte* data )
{
	// impossible to be greater (except through read buffer overflow)
	if ( size < kNetworkBufferSize )
	{
		client.inSize[ client.inFree ] = size;
		memcpy( &client.in[ client.inFree * kNetworkBufferSize ], data, size );
		client.inFree = (client.inFree + 1) % kQueueSize;
		if ( client.inFree==client.inCurrent )
		{
			client.inCurrent = (client.inCurrent + 1) % kQueueSize;
		}
	}
};

// add new item to client queue from the outside (coming in)
void AddClientOutQueue( size_t size, const byte* data )
{
	PreCond( size < kNetworkBufferSize );

	client.outSize[ client.outFree ] = size + sizeof(size_t);
	
	byte* p = &client.out[ client.outFree * kNetworkBufferSize ];
	size_t *ps = (size_t*)p;
	*ps = size;
	p = &p[sizeof(size_t)];
	memcpy( p, data, size );

	client.outFree = (client.outFree + 1) % kQueueSize;
	if ( client.outFree==client.outCurrent )
	{
		client.outCurrent = (client.outCurrent + 1) % kQueueSize;
	}
};

// skip next item in client queue or return false if none available
bool SkipNextServerInQueue( size_t sId )
{
	if ( server.inFree[sId] == server.inCurrent[sId] )
	{
		return false;
	}
	server.inFree[sId] = (server.inFree[sId]+1) % kQueueSize;
	return true;
};


// copy item from client queue or return false if none available
bool GetNextServerInQueue( size_t sId, size_t& bufSize, byte* data )
{
	if ( server.inFree[sId] == server.inCurrent[sId] )
	{
		bufSize = 0;
		return false;
	}
	bufSize = server.inSize[sId][server.inCurrent[sId]];
	memcpy( data, &server.in[sId][ server.inCurrent[sId] * kNetworkBufferSize ], bufSize );
	server.inCurrent[sId] = (server.inCurrent[sId] + 1) % kQueueSize;
	return true;
};

bool GetNextServerOutQueue( size_t sId, size_t& bufSize, byte* data )
{
	if ( server.outFree[sId] == server.outCurrent[sId] )
	{
		bufSize = 0;
		return false;
	}
	bufSize = server.outSize[sId][server.outCurrent[sId]];
	memcpy( data, &server.out[sId][ server.outCurrent[sId] * kNetworkBufferSize ], bufSize );
	server.outCurrent[sId] = (server.outCurrent[sId] + 1) % kQueueSize;
	return true;
};

void AddServerInQueue( size_t sId, size_t size, const byte* data )
{
	// impossible to be greater (except through read buffer overflow)
	if ( size < kNetworkBufferSize )
	{
		server.inSize[sId][ server.inFree[sId] ] = size;
		memcpy( &server.in[sId][ server.inFree[sId] * kNetworkBufferSize ], data, size );
		server.inFree[sId] = (server.inFree[sId] + 1) % kQueueSize;
		if ( server.inFree[sId]==server.inCurrent[sId] )
		{
			server.inCurrent[sId] = (server.inCurrent[sId] + 1) % kQueueSize;
		}
	}
};

void AddServerOutQueue( size_t sId, size_t size, const byte* data )
{
	PreCond( size < kNetworkBufferSize );

	server.outSize[sId][ server.outFree[sId] ] = size + sizeof(size_t);

	byte* p = &server.out[sId][ server.outFree[sId] * kNetworkBufferSize ];
	size_t *ps = (size_t*)p;
	*ps = size;
	p = &p[sizeof(size_t)];
	memcpy( p, data, size );

	server.outFree[sId] = (server.outFree[sId] + 1) % kQueueSize;
	if ( server.outFree[sId]==server.outCurrent[sId] )
	{
		server.outCurrent[sId] = (server.outCurrent[sId] + 1) % kQueueSize;
	}
};

//==========================================================================

bool nwStartWinsock( TString& errStr )
{
	if ( !winsockStarted )
	{
		winsockStarted = true;
		serverActive = false;
		clientActive = false;

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
	}
	return true;
};

// shutdown winsock
void nwStopWinsock( void )
{
	// stop threads
	nwStopNetwork();

	serverActive = false;
	clientActive = false;
	::WSACleanup();
};

void nwStopNetwork( void )
{
	stopComms = true;
	if ( hNetworkThread!=NULL )
	{
		::CloseHandle( hNetworkThread );
	}
	hNetworkThread = NULL;

	if ( hNetworkRespondThread!=NULL )
	{
		::CloseHandle( hNetworkRespondThread );
	}
	hNetworkRespondThread = NULL;

	// wait for the network to go down properly
	// since it is a thread
	::Sleep( 100 );
};

bool nwGetLocalNetDetails( TString& hostName, TString& ipStr )
{
	TString errStr;
	if ( !nwStartWinsock( errStr ) )
	{
		return false;
	}

	char buf[kNetworkBufferSize];
	int res = ::gethostname( buf, kNetworkBufferSize );
	if ( res!=SOCKET_ERROR )
	{
		hostName = buf;
		LPHOSTENT lpHostEntry;
		lpHostEntry = ::gethostbyname(buf);
		if ( lpHostEntry!=NULL )
		{
			SOCKADDR_IN remoteAddress;
			remoteAddress.sin_addr = *((LPIN_ADDR)*lpHostEntry->h_addr_list);

			long lip = remoteAddress.sin_addr.S_un.S_addr;
			TString ipStr2 = Int2Str(lip&255) + ".";
			ipStr2 = ipStr2 + Int2Str((lip>>8)&255) + ".";
			ipStr2 = ipStr2 + Int2Str((lip>>16)&255) + ".";
			ipStr2 = ipStr2 + Int2Str((lip>>24)&255);

			ipStr = ipStr2;

			return true;
		}
	}
	return false;
};

DWORD WINAPI NetworkThreadProc( LPVOID lpParameter )
{
	NetworkInitSettings* nwis = (NetworkInitSettings*)lpParameter;
	TString errStr;

	size_t languageId;
	if ( !GetRegistryKey( "Software\\PDV\\Performance", "Language", languageId ) )
	{
		languageId = 0;
	}

	clientActive = false;
	serverActive = false;

	if ( !nwStartWinsock(errStr) )
	{
		nwis->errStr = errStr;
		return 0;
	}

	// assume we want to do this
	stopComms = false;

	if ( nwis->isClient )
	{
		LPHOSTENT lpHostEntry;
		lpHostEntry = ::gethostbyname(nwis->server.c_str());
		if (lpHostEntry == NULL)
		{
			nwis->errStr = "gethostbyname()";
			nwis->clientConnected = false;
			return 0;
		}

		SOCKADDR_IN remoteAddress;
		remoteAddress.sin_family = AF_INET;
		remoteAddress.sin_addr = *((LPIN_ADDR)*lpHostEntry->h_addr_list);
		remoteAddress.sin_port = htons(nwis->port);

		// initialize the structure that will be passed to bind
		// as the local address.  We'll take any local address
		// and any local port.
		SOCKADDR_IN  myaddr;
		myaddr.sin_port = 0;
		myaddr.sin_addr.s_addr = 0;
		myaddr.sin_family = PF_INET;
					
		// create a socket.
		SOCKET tcpInSock = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		if ( tcpInSock==INVALID_SOCKET )
		{
			nwis->errStr = "socket()";
			nwis->clientConnected = false;
			return 0;
		}

		// set the socket to blocking - just for connecting
		unsigned long dontblock = 0;
		::ioctlsocket( tcpInSock, FIONBIO, &dontblock );

		BOOL keepAlive = TRUE;
		::setsockopt( tcpInSock, IPPROTO_TCP, SO_KEEPALIVE, (const char*)&keepAlive, sizeof(BOOL) );

		// bind the socket to the local address
		int nRc = ::bind( tcpInSock, (struct sockaddr FAR *)&myaddr, sizeof(myaddr) );
		if(nRc == SOCKET_ERROR)
		{
			::closesocket(tcpInSock);
			nwis->errStr = "bind()";
			nwis->clientConnected = false;
			return 0;
		}
					
		//
		// calling connect on a UDP socket will just set the default
		// address to send to, so we can use send() as well as
		// sendto() for udp.  It's merely a convenience, and not
		// necessary for UDP.
		//
		nRc = ::connect( tcpInSock,
						 (struct sockaddr FAR *) &(remoteAddress),
						 sizeof(remoteAddress) );

		if( nRc==SOCKET_ERROR )
		{
			long err = WSAGetLastError();
			if ( err==WSAECONNREFUSED )
			{
				nwis->errStr = nwis->server + STR(strRefusedConnection);
			}
			else
			{
				nwis->errStr = STR(strCouldNotConnect) + nwis->server;
			}
			::closesocket(tcpInSock);
			tcpInSock = INVALID_SOCKET;
			nwis->clientConnected = false;
			return 0;
		}

		// set the socket to non-blocking
		dontblock = 1;
		::ioctlsocket( tcpInSock, FIONBIO, &dontblock );

		byte buf[kNetworkBufferSize*4];

		clientActive = true;
		while ( tcpInSock!=INVALID_SOCKET && !stopComms )
		{
			::Sleep( 20 ); // run at 50 fps

			// received something?
			int size = ::recv( tcpInSock, (char*)buf, kNetworkBufferSize*4, 0 );
			if ( size==SOCKET_ERROR || size==0 )
			{
				long err = ::WSAGetLastError();
				if ( err!=WSAEWOULDBLOCK || size==0 )
				{
					::closesocket( tcpInSock );
					tcpInSock = INVALID_SOCKET;
					nwis->errStr = "server disconnected";
					nwis->clientConnected = false;

					// set packet to reflect the server quit in the queue
					DataQuit resp;
					resp.response = nwis->errStr;
					size_t size = resp.Set( buf );
					AddClientInQueue( size, buf );
					clientActive = false;

					return 1;
				}
			}
			else
			{
				size_t totalSize = 0;
				do
				{
					byte* p = &buf[totalSize];
					size_t msgSize = *(size_t*)p;
					p = &buf[totalSize+sizeof(size_t)];

					AddClientInQueue( msgSize, p );

					totalSize += sizeof(size_t) + msgSize;
				}
				while ( totalSize < size_t(size) );
			}

			size_t packetSize;
			if ( GetNextClientOutQueue( packetSize, buf ) )
			{
				size = ::send( tcpInSock, (char*)buf, packetSize, 0 );
				if ( size==SOCKET_ERROR )
				{
					long err = ::WSAGetLastError();
					if ( err!=WSAEWOULDBLOCK )
					{
						::closesocket( tcpInSock );
						tcpInSock = INVALID_SOCKET;
						nwis->errStr = "server disconnected";
						nwis->clientConnected = false;

						// set packet to reflect the server quit in the queue
						DataQuit resp;
						resp.response = nwis->errStr;
						size_t size = resp.Set( buf );
						AddClientInQueue( size, buf );
						clientActive = false;

						return 1;
					}
				}
			}
		};
		clientActive = false;
		nwis->clientConnected = false;
		if ( tcpInSock!=INVALID_SOCKET )
		{
			::closesocket( tcpInSock );
			tcpInSock = INVALID_SOCKET;
		}
	}
	else
	{
		int nRc;
		struct sockaddr_in myaddr;

		memset(&myaddr, 0, sizeof(myaddr));
		myaddr.sin_family = PF_INET;
		myaddr.sin_port = htons(nwis->port);

		SOCKET tcpInSock = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		if( tcpInSock != SOCKET_ERROR)
		{
			// set the socket to non-blocking
			unsigned long dontblock = 1;
			::ioctlsocket( tcpInSock, FIONBIO, &dontblock );

			// keep socket alive auto
			BOOL keepAlive = TRUE;
			::setsockopt( tcpInSock, IPPROTO_TCP, SO_KEEPALIVE, (const char*)&keepAlive, sizeof(BOOL) );

			nRc = ::bind(tcpInSock, (struct sockaddr FAR *)&myaddr,
						  sizeof(myaddr));
			::listen( tcpInSock, SOMAXCONN );

			byte buf[kNetworkBufferSize*4];
			serverActive = true;
			while ( !stopComms )
			{
				::Sleep( 20 ); // run at 50 fps

				// accept new connection?
				SOCKET sock = ::accept( tcpInSock, NULL, NULL);
				if ( sock!=INVALID_SOCKET )
				{
					// find first available slot
					bool found = false;
					for ( size_t i=1; i<kMaxPlayers && !found; i++ )
					{
						if ( server.sockets[i]==INVALID_SOCKET )
						{
							found = true;
							server.sockets[i] = sock;
							server.inFree[i] = 0;
							server.outFree[i] = 0;
							server.inCurrent[i] = 0;
							server.outCurrent[i] = 0;
						}
					}

					// can't connect - all slots full!
					if ( !found )
					{
						::closesocket( sock );
					}
				};

				// read from all available sockets to see if there's a message
				// queueing for me and write any messages waiting to be send
				for ( size_t i=1; i<kMaxPlayers; i++ )
				{
					if ( server.sockets[i]!=INVALID_SOCKET )
					{
						// read
						int size = ::recv( server.sockets[i], (char*)buf, kNetworkBufferSize*4, 0 );
						if ( size==SOCKET_ERROR || size==0 )
						{
							long err = ::WSAGetLastError();
							if ( err!=WSAEWOULDBLOCK || size==0 )
							{
								::closesocket( server.sockets[i] );
								server.sockets[i] = INVALID_SOCKET;

								// set packet to reflect the server quit in the queue
								DataQuit resp;
								resp.playerId = i;
								size_t size = resp.Set( buf );
								AddServerInQueue( i, size, buf );
							}
						}
						else 
						{
							size_t totalSize = 0;
							do
							{
								byte* p = &buf[totalSize];
								size_t msgSize = *(size_t*)p;
								p = &buf[totalSize+sizeof(size_t)];

								AddServerInQueue( i, msgSize, p );

								totalSize += sizeof(size_t) + msgSize;
							}
							while ( totalSize < size_t(size) );
						}

						// write
						size_t packetSize;
						if ( GetNextServerOutQueue( i, packetSize, buf ) )
						{
							size = ::send( server.sockets[i], (char*)buf, packetSize, 0 );
							if ( size==SOCKET_ERROR )
							{
								long err = ::WSAGetLastError();
							if ( err!=WSAEWOULDBLOCK )
								{
									::closesocket( server.sockets[i] );
									server.sockets[i] = INVALID_SOCKET;

									// set packet to reflect the server quit in the queue
									DataQuit resp;
									resp.playerId = i;
									size_t size = resp.Set( buf );
									AddServerInQueue( i, size, buf );
								}
							}
						}
					}
				}
			};
			serverActive = false;

			// close comms
			if ( tcpInSock!=INVALID_SOCKET )
			{
				::closesocket( tcpInSock );
				tcpInSock = INVALID_SOCKET;
			}
			for ( size_t i=1; i<kMaxPlayers; i++ )
			{
				if ( server.sockets[i]!=INVALID_SOCKET )
				{
					::closesocket( server.sockets[i] );
					server.sockets[i] = INVALID_SOCKET;
				}
			}
		}
	}
	return 1;
};

//==========================================================================

bool nwIsClientRunning( TString& errStr )
{
	if ( !nwis.clientConnected )
	{
		errStr = nwis.errStr;
		return false;
	}
	return true;
};

// server is always connected if asked (socketId==0 == server)
bool nwIsConnected( size_t socketId )
{
	if ( socketId==0 )
		return true;

	return server.sockets[socketId]!=INVALID_SOCKET;
};

bool nwReceiveBufferFull( void )
{
	return (client.inFree!=client.inCurrent);
};

bool nwReceiveBufferFull( size_t socketId )
{
	return (server.inFree[socketId]!=server.inCurrent[socketId]);
};

bool nwReadNetworkBuffer( byte* buf, size_t& size )
{
	return GetNextClientInQueue( size, buf );
};

bool nwReadNetworkBuffer( size_t socketId, byte* buf, size_t& size )
{
	return GetNextServerInQueue( socketId, size, buf );
};

void nwWriteNetworkBuffer( const byte* buf, size_t size )
{
	AddClientOutQueue( size, buf );
};

void nwWriteNetworkBuffer( size_t socketId, const byte* buf, size_t size )
{
	AddServerOutQueue( socketId, size, buf );
};

//==========================================================================
// main entry functions into the network system
//
//    1. nwStartClient()
//	  2. nwStartHost()
//	  3. nwFindHost()
//

void nwStartClient( const TString& str, size_t port )
{
	if ( hNetworkThread==NULL )
	{
		serverActive = false;
		clientActive = false;
		nwis.clientConnected = true;
		nwis.port = port;
		nwis.server = str;
		nwis.isClient = true;
		InitQueues();
		hNetworkThread = ::CreateThread( NULL, 1024, NetworkThreadProc, (LPVOID)&nwis, 
										 NULL, &networkThreadId );
	}
};

void nwStartHost( const TString& gameName, size_t port )
{
	if ( hNetworkThread==NULL )
	{
		serverActive = false;
		clientActive = false;
		for ( size_t i=0; i<kMaxPlayers; i++ )
		{
			server.sockets[i] = INVALID_SOCKET;
			InitQueues();
		}
		nwis.gameName = gameName;
		nwis.port = port;
		nwis.isClient = false;
		hNetworkThread = ::CreateThread( NULL, 1024, NetworkThreadProc, (LPVOID)&nwis, 
										 NULL, &networkThreadId );
		hNetworkRespondThread = ::CreateThread( NULL, 1024, NetworkThreadServerRespondProc, (LPVOID)&nwis,
												NULL, NULL );
	}
};

bool nwFindHost( TString& hostNames, TString& hostIPs, 
				 TString& hostPorts, TString& numPlayers,
				 TString& gameNames )
{
	TString errStr;
	if ( !nwStartWinsock( errStr ) )
	{
		return false;
	}

	// initialize the structure that will be passed to bind
	// as the local address.  We'll take any local address
	// and any local port.
	SOCKADDR_IN  myaddr;
	memset(&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = PF_INET;
	myaddr.sin_port = htons(lookupPort);
				
	// create a socket.
	SOCKET tcpInSock = ::socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if ( tcpInSock==INVALID_SOCKET )
	{
		long err = WSAGetLastError();
		return false;
	}

	BOOL broadcast = TRUE;
	int nRc = ::setsockopt( tcpInSock, SOL_SOCKET, SO_BROADCAST, (const char*)&broadcast, sizeof(BOOL) );
	if ( nRc==SOCKET_ERROR )
	{
		long err = WSAGetLastError();
		return false;
	}

	// set the socket to blocking - just for connecting
	unsigned long dontblock = 1;
	::ioctlsocket( tcpInSock, FIONBIO, &dontblock );

	// bind the socket to the local address
	nRc = ::bind( tcpInSock, (struct sockaddr FAR *)&myaddr, sizeof(myaddr) );
	if(nRc == SOCKET_ERROR)
	{
		long err = ::WSAGetLastError();
		return false;
	}

	SOCKADDR_IN remoteAddress;
	remoteAddress.sin_family = AF_INET;
	remoteAddress.sin_addr.S_un.S_addr = 0xffffffff; // broadcast
	remoteAddress.sin_port = htons(lookupPort);

	nRc = ::sendto( tcpInSock, resolveStr.c_str(), resolveStr.length()+1, 0,
					(const struct sockaddr*)&remoteAddress, sizeof(remoteAddress) );
	if( nRc==SOCKET_ERROR )
	{
		::closesocket( tcpInSock );
		return false;
	}

	// wait for a reply for 10 seconds max
	DWORD time = ::GetTickCount() + kFindHostTimeout;
	bool found = false;
	char buf[kNetworkBufferSize];

	::listen( tcpInSock, SOMAXCONN );

	// reset all vars
	hostNames = "";
	hostIPs = "";
	hostPorts = "";
	numPlayers = "";
	gameNames = "";

	while ( time > ::GetTickCount() && !found )
	{
		::Sleep(100);

		int count = ::recv( tcpInSock, buf, kNetworkBufferSize, 0 );
		if ( count>0 )
		if ( buf[0]=='=' )
		{
			TString msg = &buf[1];

			TString ipStr = msg.GetItem(',',0);

			if ( hostPorts.length()>0 )
			{
				hostPorts = hostPorts + "," + msg.GetItem(',',1);
			}
			else
			{
				hostPorts = msg.GetItem(',',1);
			}

			if ( hostNames.length()>0 )
			{
				hostNames = hostNames + "," + msg.GetItem(',',2);
			}
			else
			{
				hostNames = msg.GetItem(',',2);
			};

			if ( numPlayers.length()>0 )
			{
				numPlayers = numPlayers + "," + msg.GetItem(',',3);
			}
			else
			{
				numPlayers = msg.GetItem(',',3);
			}

			if ( gameNames.length()>0 )
			{
				gameNames = gameNames + "," + msg.GetItem(',',4);
			}
			else
			{
				gameNames = msg.GetItem(',',4);
			}

			long ip = atoi(ipStr.c_str());
			ipStr = Int2Str(ip&255) + ".";
			ipStr = ipStr + Int2Str((ip>>8)&255) + ".";
			ipStr = ipStr + Int2Str((ip>>16)&255) + ".";
			ipStr = ipStr + Int2Str((ip>>24)&255);

			if ( hostIPs.length() > 0 )
			{
				hostIPs = hostIPs + "," + ipStr;
			}
			else
			{
				hostIPs = ipStr;
			}
		}
	};

	::closesocket( tcpInSock );

	return (hostIPs.length()>0);
};

//==========================================================================
// this special routine waits for datagrams and replies
// to broadcast messages and sends back its details
DWORD WINAPI NetworkThreadServerRespondProc( LPVOID lpParameter )
{
	NetworkInitSettings* nwis = (NetworkInitSettings*)lpParameter;

	struct sockaddr_in myaddr;

	while ( !winsockStarted )
	{
		::Sleep(100);
	};

	memset(&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = PF_INET;
	myaddr.sin_port = htons(lookupPort);

	SOCKET tcpInSock = ::socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if( tcpInSock != SOCKET_ERROR)
	{
		unsigned long dontblock = 1;
		::ioctlsocket( tcpInSock, FIONBIO, &dontblock );

		BOOL broadcast = TRUE;
		int nRc = ::setsockopt( tcpInSock, SOL_SOCKET, SO_BROADCAST, (const char*)&broadcast, sizeof(BOOL) );
		if ( nRc==SOCKET_ERROR )
		{
			::closesocket( tcpInSock );
			return 0;
		}
		nRc = ::bind(tcpInSock, (struct sockaddr FAR *)&myaddr,
					  sizeof(myaddr));

		::listen( tcpInSock, SOMAXCONN );
		char buf[kNetworkBufferSize];
		while ( !stopComms )
		{
			::Sleep( 100 ); // wait
			int count = ::recv( tcpInSock, buf, kNetworkBufferSize, 0 );
			if ( size_t(count)==(resolveStr.length()+1) )
			if ( resolveStr==buf )
			{
				// send back a message with my address
				int res = ::gethostname( buf, kNetworkBufferSize );
				if ( res!=SOCKET_ERROR )
				{
					TString hostName = buf;
					LPHOSTENT lpHostEntry;
					lpHostEntry = ::gethostbyname(nwis->server.c_str());
					if ( lpHostEntry!=NULL )
					{
						SOCKADDR_IN remoteAddress;
						remoteAddress.sin_family = AF_INET;
						remoteAddress.sin_addr = *((LPIN_ADDR)*lpHostEntry->h_addr_list);
						remoteAddress.sin_port = htons(lookupPort);
						TString msg = "=" + Int2Str(remoteAddress.sin_addr.S_un.S_addr);
						msg = msg + "," + Int2Str(nwis->port);
						msg = msg + "," + hostName;

						// also return how many players on the system right now
						size_t playerCount = 1;  // for server
						for ( size_t i=1; i<kMaxPlayers; i++ )
						{
							if ( server.sockets[i]!=INVALID_SOCKET )
							{
								playerCount++;
							}
						}
						msg = msg + "," + Int2Str(playerCount);
						msg = msg + "," + nwis->gameName;

						// broadcast this information back
						SOCKADDR_IN broadCast;
						broadCast.sin_family = AF_INET;
						broadCast.sin_addr.S_un.S_addr = 0xffffffff; // broadcast
						broadCast.sin_port = htons(lookupPort);
						nRc = ::sendto( tcpInSock, msg.c_str(), msg.length()+1, 0, 
										(const struct sockaddr*)&broadCast, sizeof(broadCast) );
					}
				}
			}
		}

		// stop comms
		::closesocket( tcpInSock );
	}
	return 0;
};

bool nwServerActive( void )
{
	return serverActive;
}

bool nwClientActive( void )
{
	return clientActive;
};

//==========================================================================

