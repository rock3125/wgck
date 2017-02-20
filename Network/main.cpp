#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include <common\compatability.h>
#include <common\string.h>

#include "networking.h"

TString _dir;

//==========================================================================

void InitQueues( void );
bool SkipNextClientQueue( void );
bool NextClientQueue( size_t& bufSize, byte* data );
void AddClientQueue( size_t size, const byte* data );



int main( int argc, char* argv[] )
{
	bool isHost = false;
	bool isClient = false;
	bool isHostSearch = false;

	if ( argc!=4 && argc!=3 && argc!=2 )
	{
		printf("testnet client hostip hostport\n");
		printf("testnet host port\n" );
		printf("testnet findhost\n" );
		return 1;
	}

	if ( strcmp(argv[1],"client")==0 && argc==4 )
	{
		printf( "starting client, connecting to %s, port %s\n", argv[2], argv[3] );
		isClient = true;
	}
	if ( strcmp(argv[1],"host")==0 && argc==3 )
	{
		printf( "starting host on port %s\n", argv[2] );
		isHost = true;
	}
	if ( strcmp(argv[1],"findhost")==0 && argc==2 )
	{
		printf( "searching for hosts\n" );
		isHostSearch = true;
	}
	if ( !isHost && !isClient && !isHostSearch )
	{
		printf("testnet client hostip hostport\n");
		printf("testnet host port\n" );
		printf("testnet findhost\n" );
		return 1;
	}

	if ( isClient )
	{
		size_t id = 0;
		size_t port = size_t(atoi(argv[3]));
		nwStartClient( argv[2], port );
		printf( "client starting\n\n" );

		TString errStr;
		TString msg = "message from client";
		while ( nwIsClientRunning(errStr) )
		{
			char buf[1024];
			size_t size;
			while ( nwReadNetworkBuffer((byte*)buf,size) )
			{
				printf( "client: server message %d: %s\n", id, buf );
				id++;
			}
			nwWriteNetworkBuffer((byte*)msg.c_str(),msg.length()+1);

			// 15 fps
			::Sleep( 67 );
		};
		printf( "client finished: %s\n", errStr.c_str() );
	}
	else if ( isHost )
	{
		size_t port = size_t(atoi(argv[2]));
		nwStartHost( "nettest game", port );
		printf( "server starting\n\n" );

		TString errStr;
		TString msg = "message from server";
		while ( true )
		{
			for ( size_t i=0; i<kMaxPlayers; i++ )
			{
				char buf[1024];
				size_t size;
				if ( nwReadNetworkBuffer(i,(byte*)buf,size) )
				{
					printf( "server: message from client %d: %s\n", i, buf );
					nwWriteNetworkBuffer(i,(byte*)msg.c_str(),msg.length()+1);
				}
			}

			// 15 fps
			::Sleep( 67 );

		};
		printf( "server finished: %s\n", errStr.c_str() );
	}
	else
	{
		TString hostNames, hostIPs, hostPorts, numPlayers, gameNames;
		if ( nwFindHost( hostNames, hostIPs, hostPorts, numPlayers, gameNames ) )
		{
			printf( "found games called (%s) on (%s) (%s:%s) with (%s) players\n", 
					gameNames.c_str(), hostNames.c_str(), hostIPs.c_str(), 
					hostPorts.c_str(), numPlayers.c_str() );
		}
		else
		{
			printf( "no hosts found\n" );
		}
	}

	return 0;
};

//==========================================================================
