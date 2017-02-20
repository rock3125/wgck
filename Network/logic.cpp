#include <precomp_header.h>

#include <dialogs/dialogAssist.h>
#include <dialogs/joinDialog.h>
#include <dialogs/hostDialog.h>

#include <network/network.h>

#define _TEST

//==========================================================================

const size_t kTimeoutPeriod = 30000;
const short kGamePort = 2000;

//==========================================================================

TNetwork::TNetwork( void )
{
	map = 0;
	myId = 0;

	winsockStarted = false;
	clientStarted = false;
	serverStarted = false;

	// start comms (both client & server)
	TString errStr;
	if ( !StartWinsock(errStr) )
	{
		WriteString( errStr.c_str() );
		PostCond( false );
	}
};


TNetwork::~TNetwork( void )
{
	StopNetwork();
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


bool TNetwork::StartServer( HWND hWindow, TString& errStr )
{
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
		errStr = "socket() failed";
		return false;
	}

	
	//
	// Fill in the address structure
	//
	saServer.sin_family = AF_INET;
	saServer.sin_addr.s_addr = INADDR_ANY;	// Let WinSock assign address
	saServer.sin_port = htons(kGamePort);	// Use port passed from user

	//
	// bind the name to the socket
	//
	int nRet;

	nRet = bind(theSocket,				// Socket descriptor
				(LPSOCKADDR)&saServer,  // Address to bind to
				sizeof(struct sockaddr)	// Size of address
				);
	if ( nRet == SOCKET_ERROR )
	{
		errStr = "bind() failed";
		closesocket(theSocket);
		return false;
	}

	size_t flags = FD_CONNECT | FD_CLOSE | FD_READ | FD_WRITE | FD_ACCEPT;
	int res = WSAAsyncSelect( theSocket, hWindow, WM_SOCKMESSAGE, flags );
	if ( res==SOCKET_ERROR )
	{
		errStr = "WSAAsyncSelect() failed";
		return false;
	}

	serverStarted = true;
	WSASetLastError(0);

	return true;
}


void TNetwork::DoSocket( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	int error = WSAGETSELECTERROR(lParam);
	int event = WSAGETSELECTEVENT(lParam);

	SOCKET socket = wParam;

	switch ( event )
	{
	case FD_READ:
		{
			break;
		}
	case FD_WRITE:
		{
			break;
		}
	case FD_ACCEPT:
		{
			break;
		}
	case FD_CONNECT:
		{
			break;
		}
	case FD_CLOSE:
		{
			break;
		}
	};
};



void TNetwork::StopNetwork( void )
{
	if ( client.Connected() )
		client.StopClient();

	if ( server.Started() )
		server.StopServer();

	// shutdown comms
	CloseWinsock();
}


void TNetwork::Logic( bool active, TApp* app )
{
	if ( client.Connected() )
	{
		DoClientLogic( app );
	}
	else if ( server.Started() )
	{
		DoServerLogic( app );
	}

	if ( server.Started() && myId>0 )
	{
		TTank* list[kMaxPlayers];
		size_t cntr = 0;
		for ( size_t i=1; i<kMaxPlayers; i++ )
			if ( server.used[i] && server.active[i] )
			{
				list[cntr] = &server.tanks[i];
				cntr++;
			}
		app->Logic( server.tanks[myId-1], cntr, list );
	}
	else if ( client.Connected() && myId>0 )
	{
		TTank* list[kMaxPlayers];
		size_t cntr = 0;
		for ( size_t i=1; i<kMaxPlayers; i++ )
			if ( client.used[i] )
			{
				list[cntr] = &client.tanks[i];
				cntr++;
			}
		app->Logic( client.tanks[myId-1], cntr, list );
	}
}


void TNetwork::Render( TApp* app )
{
	size_t i;

	if ( server.Started() && myId>0 )
	{
		TTank* list[kMaxPlayers];
		size_t cntr = 0;
		for ( i=1; i<kMaxPlayers; i++ )
			if ( server.used[i] && server.active[i] )
			{
				list[cntr] = &server.tanks[i];
				cntr++;
			
			}
#ifndef _TEST
		app->Render( server.tanks[myId-1], cntr, list );
#else
		app->SetupGL2d( 450, 640, 480 );
		glEnable( GL_COLOR_MATERIAL );
		glColor3ub( 0,0,0 );

		Write( 260,420,-10, "S E R V E R" );

		for ( i=0; i<kMaxPlayers; i++ )
		{
			glLoadIdentity();
			float x = float(i%4) * 160.0f;
			float y = 400-float(i/4) * 100.0f;
			glTranslatef( x,y, -10.0f );

			if ( server.used[i] && server.active[i] )
			{
				TString msg;
				Write( 0,0,-10, server.name[i] );
				msg = "side " + Int2Str( server.side[i] );
				Write( 0,-12,-10, msg );
				DataParticulars dat(myId);
				if ( byte(i)==(myId-1) )
				{
					TTank& tank = server.tanks[i];
					tank.Get( dat );
				}
				else
					dat = *server.players[i];

				msg = "(mx,my,mz) " + FloatToString( 1,1, dat.mx );
				msg = msg + "," + FloatToString( 1,1, dat.my );
				msg = msg + "," + FloatToString( 1,1, dat.mz );
				Write( 0,-24,-10, msg );

				msg = "(xa,ya,za) " + FloatToString( 1,1, dat.xa );
				msg = msg + "," + FloatToString( 1,1, dat.ya );
				msg = msg + "," + FloatToString( 1,1, dat.za );
				Write( 0,-36,-10, msg );

				msg = "(ba,ta) " + FloatToString( 1,1, dat.ba );
				msg = msg + "," + FloatToString( 1,1, dat.ta );
				Write( 0,-48,-10, msg );
			}
			else
			{
				TString msg = "[" + Int2Str( i ) + " available]";
				Write( 0,0, -10, msg );
			}
		}
#endif
	}
	else if ( client.Connected() && myId>0 )
	{
		TTank* list[kMaxPlayers];
		size_t cntr = 0;
		for ( i=0; i<kMaxPlayers; i++ )
			if ( client.used[i] && byte(i)!=(myId-1) )
			{
				list[cntr] = &client.tanks[i];
				cntr++;
			}
#ifndef _TEST
		app->Render( client.tanks[myId-1], cntr, list );
#else
		app->SetupGL2d( 450, 640, 480 );
		glEnable( GL_COLOR_MATERIAL );
		glColor3ub( 0,0,0 );

		Write( 260,420,-10, "C L I E N T" );

		for ( i=0; i<kMaxPlayers; i++ )
		{
			glLoadIdentity();
			float x = float(i%4) * 160.0f;
			float y = 400-float(i/4) * 100.0f;
			glTranslatef( x,y, -10.0f );

			if ( client.used[i] )
			{
				TString msg;
				Write( 0,0,-10, client.name[i] );
				msg = "side " + Int2Str( client.side[i] );
				Write( 0,-12,-10, msg );
				DataParticulars dat(myId);
				if ( byte(i)==(myId-1) )
				{
					TTank& tank = client.tanks[i];
					tank.Get( dat );
				}
				else
					dat = *client.players[i];

				msg = "(mx,my,mz) " + FloatToString( 1,1, dat.mx );
				msg = msg + "," + FloatToString( 1,1, dat.my );
				msg = msg + "," + FloatToString( 1,1, dat.mz );
				Write( 0,-24,-10, msg );

				msg = "(xa,ya,za) " + FloatToString( 1,1, dat.xa );
				msg = msg + "," + FloatToString( 1,1, dat.ya );
				msg = msg + "," + FloatToString( 1,1, dat.za );
				Write( 0,-36,-10, msg );

				msg = "(ba,ta) " + FloatToString( 1,1, dat.ba );
				msg = msg + "," + FloatToString( 1,1, dat.ta );
				Write( 0,-48,-10, msg );
			}
			else
			{
				TString msg = "[" + Int2Str( i ) + " available]";
				Write( 0,0, -10, msg );
			}
		}
#endif
	}
};


void TNetwork::DoClientLogic( TApp* app )
{
	byte buf[1024];
	size_t s = client.Read( buf, 1024 );
	while ( s>0 )
	{
		byte type = buf[0];
		switch ( type )
		{
		case s_youcan:
		{
			DataYouCan resp;
			resp.Get( buf );
			if ( resp.accept )
			{
				TString msg = resp.reason;
				AddMessage( msg );

				// create myself a tank
				TString errStr;
				myId = resp.playerId;
				TTank& tank = client.tanks[myId-1];
				if ( !app->LoadTank( side[myId-1], tank, errStr ) )
				{
					::MessageBox( NULL, errStr.c_str(), "Error",
								  MB_OK | MB_ICONERROR );
				}
				tank.X( 90 + float(myId)*10 );
				tank.Z( 140 );

				DataParticulars dat(myId);
				tank.Get( dat );
				dat.playerId = resp.playerId;
				DataICanJoin reply(resp.playerId,true,dat);
				size_t s = reply.Set(buf);
				client.Write( buf, s, errStr );
				myId = resp.playerId;

				// set my slot as taken
				dat.playerId = myId;
				client.AddPlayer( myId, 
								  side[myId-1],
								  playerName,
								  dat );
			}
			else
			{
				client.StopClient();
				TString msg = "Server refused: " + resp.reason;
				AddMessage( msg );
			}
			break;
		}
		case s_quit:
		{
			DataServerQuit resp;
			resp.Get( buf );
			client.StopClient();
			TString msg = "Server quit: " + resp.reason;
			AddMessage( msg );
			break;
		}
		case s_globalcoords:
		{
			DataGlobalCoords resp;
			resp.Get( buf );
			for ( size_t i=0; i<resp.cntr; i++ )
			{
				client.SetData( resp.data[i]->playerId, *resp.data[i] );
			}
			break;
		}
		case s_welcome:
		{
			DataWelcome resp;
			resp.Get( buf );
			for ( size_t i=0; i<resp.cntr; i++ )
			{
				byte playerId = resp.data[i]->playerId;
				client.AddPlayer( playerId, 
								  resp.side[i],
								  resp.name[i], 
								  *resp.data[i] );
				TTank& tank = client.tanks[playerId-1];
				byte side = resp.side[i];
				TString errStr;
				if ( !app->LoadTank( side, tank, errStr ) )
				{
					::MessageBox( NULL, errStr.c_str(), "Error",
								  MB_OK | MB_ICONERROR );
				}
				TString msg = resp.name[i] + " joined";
				AddMessage( msg );
			}
			break;
		}
		case s_otherplayerjoined:
		{
			DataOtherPlayerJoined resp(0);
			resp.Get( buf );
			
			byte playerId = resp.data.playerId;
			client.AddPlayer( playerId, 
							  resp.side,
							  resp.name, 
							  resp.data );
			TTank& tank = client.tanks[playerId-1];
			byte side = resp.side;
			TString errStr;
			if ( !app->LoadTank( side, tank, errStr ) )
			{
				::MessageBox( NULL, errStr.c_str(), "Error",
							  MB_OK | MB_ICONERROR );
			}
			TString msg = resp.name + " joined";
			AddMessage( msg );
			break;
		}
		case s_otherplayerquit:
		{
			DataOtherPlayerQuit resp;
			resp.Get( buf );
			client.used[ resp.playerId-1 ] = false;
			TString msg = client.name[resp.playerId-1] + " quit";
			AddMessage( msg );
			break;
		}
		default:
			break;
		};

		// more traffic?
		s = client.Read( buf, 1024 );
	}

	// write my details to server
	if ( myId>0 )
	{
		TTank& tank = client.tanks[myId-1];
		DataParticulars dtls(myId);
		dtls.playerId = myId;
		tank.Get( dtls );
		s = dtls.Set( buf );
		TString errStr;
		client.Write( buf, s, errStr );
	}
};


void TNetwork::DoServerLogic( TApp* app )
{
	size_t i;
	byte buf[1024];
	SOCKADDR_IN clientAddr;
	size_t s = server.Read( clientAddr, buf, 1024 );
	while ( s>0 )
	{
		byte type = buf[0];
		switch ( type )
		{
		case c_canijoin:
		{
			DataCanIJoin resp;
			resp.Get( buf );

			TString msg, errStr;
			byte playerId = server.CheckNewPlayer( resp.name, resp.side, clientAddr, errStr );
			if ( playerId==0 )
			{
				msg = "the name you're using has already been taken.";
				DataYouCan reply( false, map, playerId, msg );
				size_t s = reply.Set(buf);
				server.Write( clientAddr, buf, s );
			}
			else
			{
				msg = "welcome to " + gameName;
				DataYouCan reply( true, map, playerId, msg );
				size_t s = reply.Set(buf);
				server.Write( clientAddr, buf, s );
				msg = resp.name + " is joining";
				AddMessage( msg );
			}
			break;
		}
		case c_icanjoin:
		{
			size_t i;
			DataICanJoin resp(0);
			resp.Get( buf );
			byte playerId = resp.playerId;
			TString name;
			server.AddNewPlayer( playerId, name );
			*server.players[playerId-1] = resp.data;
			TString msg = name + " has joined";
			AddMessage( msg );

			// add a tank for this person to the server list
			TString errStr;
			if ( !app->LoadTank( server.side[playerId-1], server.tanks[playerId-1], errStr ) )
			{
				::MessageBox( NULL, errStr.c_str(), "Error",
							  MB_OK | MB_ICONERROR );
			}

			DataWelcome wlcm;
			for ( i=0; i<kMaxPlayers; i++ )
			if ( server.used[i] && server.active[i] && 
				 (i+1)!=playerId )
			{
				if ( byte(i)==(myId-1) )
				{
					DataParticulars dat(myId);
					dat.playerId = myId;
					server.tanks[i].Get( dat );
					wlcm.Add( server.name[i],
							  server.side[i],
							  dat );
				}
				else
				{
					DataParticulars* dat = server.players[i];
					dat->playerId = i+1;
					wlcm.Add( server.name[i],
							  server.side[i],
							  *dat );
				}
			}
			size_t s = wlcm.Set( buf );
			server.Write( playerId, buf, s );

			// write a c_otherplayerjoined to all other clients
			for ( i=1; i<kMaxPlayers; i++ )
			{
				if ( (i+1)!=playerId )
				if ( server.used[i] && server.active[i] )
				{
					DataOtherPlayerJoined msg(playerId);
					msg.side = server.side[playerId-1];
					msg.data = *server.players[playerId-1];
					msg.name = server.name[playerId-1];
					size_t s = msg.Set( buf );
					server.Write( i+1, buf, s );
				}
			};

			break;
		}
		case c_quit:
		{
			DataClientQuit resp;
			resp.Get( buf );
			server.RemovePlayer( resp.playerId );

			// notify all other players of this sad event
			for ( size_t i=1; i<kMaxPlayers; i++ )
			{
				if ( server.active[i] && server.used[i] )
				{
					DataOtherPlayerQuit msg( resp.playerId );
					size_t s = msg.Set( buf );
					server.Write( i+1, buf, s );
				}
			}
			break;
		}
		case c_myparticulars:
		{
			DataParticulars resp(0);
			resp.Get( buf );
			server.SetData( resp.playerId, resp );
			break;
		}
		default:
			break;
		};

		// more traffic to come?
		s = server.Read( clientAddr, buf, 1024 );
	}

	// go through all slots and kick people that haven't responded
	// skip 0, because thats me - the server
	size_t time = GetTickCount();
	for ( i=1; i<kMaxPlayers; i++ )
	{
		if ( server.used[i] )
		{
			if ( (time-server.commsTime[i]) > kTimeoutPeriod )
			{
				server.used[i] = false;
				TString msg = "Dropped " + server.Name(i+1);
				msg = msg + ", no longer communicating";
				AddMessage( msg );

				// notify all other players of this sad event
				for ( size_t j=1; j<kMaxPlayers; j++ )
				{
					if ( j!=i && server.active[j] && server.used[j] )
					{
						DataOtherPlayerQuit msg( i+1 );
						size_t s = msg.Set( buf );
						server.Write( j+1, buf, s );
					}
				}
			}
		}
	}

	// send a list of coordinates to all players
	for ( i=1; i<kMaxPlayers; i++ )
	{
		if ( server.used[i] && server.active[i] )
		{
			DataGlobalCoords d;
			size_t cntr = 0;
			for ( size_t j=0; j<kMaxPlayers; j++ )
			{
				if ( i!=j && server.used[j] && server.active[j] )
				{
					if ( byte(j)==(myId-1) )
					{
						DataParticulars dat(myId);
						server.tanks[j].Get( dat );
						d.Add( dat );
					}
					else
					{
						DataParticulars& dat = server.GetData(j+1);
						dat.playerId = j+1;
						d.Add( dat );
					}
					cntr++;
				}
			}
			if ( cntr>0 )
			{
				s = d.Set( buf );
				server.Write( byte(i+1), buf, s );
			}
		}
	}
};


void TNetwork::JoinGame( HINSTANCE hInstance, HWND hWnd )
{
	if ( server.Started() )
	{
		TString msg = "You are already hosting a game.\n";
		msg = msg + "Are you sure you want to join another?\n\n";
		msg = msg + "all existing players will be disconnected";
		if ( ::MessageBox( hWnd, msg.c_str(), "Warning", MB_ICONWARNING |
							MB_YESNO ) == IDNO )
		{
			return;
		}
		server.StopServer();
	}

	if ( client.Connected() )
	{
		TString msg = "You have already joined a game.\n";
		msg = msg + "Are you sure you want to join another?\n\n";
		msg = msg + "you willd be disconnected from your current game";
		if ( ::MessageBox( hWnd, msg.c_str(), "Warning", MB_ICONWARNING |
							MB_YESNO ) == IDNO )
		{
			return;
		}
		client.StopClient();
	}

	JoinDialog dlg( hInstance, hWnd );
	if ( dlg.Execute( playerName, hostName, side[myId-1] ) )
	{
		// now we join a host
		TString errStr;
		if ( !client.StartClient( hostName, kGamePort, errStr ) )
		{
			TString host = hostName;
			TString msg = "Could not connect to " + host + ".\n";
			msg = msg + "(" + errStr + ")";
			::MessageBox( hWnd, msg.c_str(), "Error", MB_OK | MB_ICONERROR );
		}
		else
		{
			DataCanIJoin send(playerName, side[myId-1] );
			byte buf[1024];
			size_t s = send.Set( buf );
			TString errStr;
			if ( !client.Write( buf, s, errStr ) )
			{
				TString host = hostName;
				TString msg = "Error talking to " + host + ".\n";
				msg = msg + "(" + errStr + ")";
				::MessageBox( hWnd, msg.c_str(), "Error", MB_OK | MB_ICONERROR );
			}
		}
	}
}


void TNetwork::HostGame( HINSTANCE hInstance, HWND hWnd, TApp* app )
{
	if ( client.Connected() )
	{
		client.StopClient();
	}

	if ( server.Started() )
	{
		TString msg = "You are already hosting a game.\n";
		msg = msg + "Are you sure you want to start a new one?\n\n";
		msg = msg + "all existing players will be disconnected";
		if ( ::MessageBox( hWnd, msg.c_str(), "Warning", MB_ICONWARNING |
							MB_YESNO ) == IDNO )
		{
			return;
		}
		server.StopServer();
	}

	HostDialog dlg( hInstance, hWnd );
	byte _side;
	if ( dlg.Execute( playerName, gameName, _side, map ) )
	{
		myId = 1; // always the server!
		server.used[0] = true;
		server.active[0] = true;
		server.commsTime[0] = GetTickCount();
		server.side[0] = _side;
		server.name[0] = playerName;

		// now we are hosting
		TString errStr;
		TString hostIP;
		if ( !server.StartServer( kGamePort, errStr ) )
		{
			TString msg = "Error hosting game: " + errStr;
			::MessageBox( hWnd, msg.c_str(), "Error",
						  MB_OK | MB_ICONERROR );
		}
		else
		{
			TTank& tank = server.tanks[myId-1];
			if ( !app->LoadTank( side[myId-1], tank, errStr ) )
			{
				::MessageBox( hWnd, errStr.c_str(), "Error",
							  MB_OK | MB_ICONERROR );
			}
			tank.X( 70 );
			tank.Z( 120 );
		}
	}
}

//==========================================================================

