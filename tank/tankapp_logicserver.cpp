#include <precomp_header.h>

#include <direct.h>
#include <process.h>
#include <shellapi.h>

#include <win32/events.h>
#include <win32/win32.h>
#include <win32/resource.h>

#include <common/sounds.h>

#include <object/geometry.h>
#include <object/tank.h>
#include <object/anim.h>
#include <object/camera.h>
#include <object/recordSystem.h>

#include <dialogs/hostdialog.h>
#include <dialogs/joindialog.h>
#include <systemDialogs/aboutdlg.h>
#include <dialogs/videoSettings.h>

#include <tank/tankapp.h>

#include <network/packet.h>

#include <io.h>

//==========================================================================

#include <mmsystem.h>

//==========================================================================

const float Deg2Rad = 0.01745329252f;
const float Rad2Deg = 57.295779511273f;

//==========================================================================

/*
void TApp::Logic( void )
{
	TEvent::Logic();

	size_t i;

	if ( initialised && hMusicThread==NULL && playMusic )
	{
		hMusicThread = CreateThread( NULL, 1024, MusicThreadProc, (LPVOID)this, 
									 NULL, &musicThreadId );
	}

	// handle menu
	if ( showMenu )
	{
		if ( currentPage==mainMenu )
		{
			if ( singlePlayerButton!=NULL )
			if ( singlePlayerButton->Clicked() )
			{
				DoMenu( IDM_SINGLEPLAYER );
			}
			if ( hostButton!=NULL )
			if ( hostButton->Clicked() )
			{
				DoMenu( IDM_HOST );
			}
			if ( joinButton!=NULL )
			if ( joinButton->Clicked() )
			{
				DoMenu( IDM_JOIN );
			}
			if ( controlsButton!=NULL )
			if ( controlsButton->Clicked() )
			{
				DoMenu( ID_HELP_CONTROLS );
			}
			if ( aboutButton!=NULL )
			if ( aboutButton->Clicked() )
			{
				DoMenu( IDM_ABOUT );
			}
			if ( videoSettingsButton!=NULL )
			if ( videoSettingsButton->Clicked() )
			{
				DoMenu( IDM_VIDEOSETTINGS );
			}
			keys = 0;
		}
	}


	if ( msg!=NULL )
		msg->ProcessDisplayMessages();

	size_t time = ::GetTickCount();
	if ( ShowScores() )
	if ( time > ShowTimeout() )
	{
		ShowScores( false );
	}

	if ( net!=NULL )
		while ( net->ProcessOutQueue() ) ;

	// host processing (iff I am host)
	if ( net!=NULL && net->serverStarted && net->MyId()>0 )
	{
		DataParticulars& player = players[net->MyId()-1];
		player.playerId = net->MyId();

		if ( (keys&aVehicle)>0 )
		{
			if ( player.vehicleId>0 )
			{
				characters[net->MyId()-1].JumpOutOfTank( tanks[player.vehicleId-1] );
				tanks[player.vehicleId-1].Occupied( false );
				player.vehicleId = 0;
			}
			else
			{
				// pick the vehicle I am inside of
				// that doesn't have anyone in it
				for ( size_t i=0; i<kMaxPlayers; i++ )
				{
					if ( net->Used(i) )
					if ( tanks[i].InsideVehicle( player.mx, player.my, player.mz ) )
					if ( !tanks[i].Occupied() )
					{
						tanks[i].Occupied( true );
						player.vehicleId = i+1;
					}
				}
			}

			keys = keys & ~aVehicle;
		}

		if ( player.vehicleId>0 )
		{
			TTank& tank = tanks[player.vehicleId-1];
			if ( rearm )
			{
				rearm = false;
	
				byte shield, fuel, shells;
				tanks[net->MyId()-1].GetDetails( shield, fuel, shells );
				shells += 10;
				if ( shells>50 )
					shells = 50;
				tanks[net->MyId()-1].SetDetails( shield, fuel, shells );
			}
			if ( refuel )
			{
				refuel = false;

				byte shield, fuel, shells;
				tanks[net->MyId()-1].GetDetails( shield, fuel, shells );
				fuel += 50;
				if ( fuel>100 )
					fuel = 100;
				tanks[net->MyId()-1].SetDetails( shield, fuel, shells );
			}
			player.destruct = selfDestruct;
		}

		// do self destruct
		if ( selfDestruct )
		{
			selfDestruct = false;

			if ( player.vehicleId>0 )
			{
				size_t vehicleId = player.vehicleId-1;
				tanks[vehicleId].Shield( 0 );
				tanks[vehicleId].UpdateShields();
				if ( scores[net->MyId()-1] > 2 )
				{
					scoreUpdated = true;
					scores[net->MyId()-1] -= 2;
				}
				else
				{
					scoreUpdated = true;
					scores[net->MyId()-1] = 0;
				}
			}
		}

		// deduct points of other players self-destructing
		for ( i=0; i<kMaxPlayers; i++ )
		{
			if ( net->Used(i) && (i+1)!=net->MyId() )
			{
				DataParticulars dat = GetData(i+1);
				if ( dat.destruct )
				if ( dat.vehicleId>0 )
				{
					dat.destruct = false;
					SetData( i+1, dat );

					if ( scores[i] > 2 )
					{
						scoreUpdated = true;
						scores[i] -= 2;
					}
					else
					{
						scoreUpdated = true;
						scores[i] = 0;
					}
				}
			}
		}

		// character logic
		bool fire = false;
		if ( !gameOver  && landObj!=NULL )
			fire = Logic( *landObj, players[net->MyId()-1], keys, ctrlDown );

		// do fire
		if ( fire )
		{
			DataFire fire;
			fire.playerId = net->MyId();
//			ProcessFire( player, fire, 1,1 ); // strength,range - changed!!!
		}

		DataScores spack;
		if ( scoreUpdated )
		{
			for ( size_t i=0; i<kMaxPlayers; i++ )
			{
				spack.scores[i] = scores[i];
			}
		}

		// send a list of coordinates to all players
		for ( i=0; i<kMaxPlayers; i++ )
		{
			// do this all the time
//			if ( net->Used(i) )
//				tanks[i].UpdateShields();

			if ( net->Used(i) && byte(i)!=(net->MyId()-1) )
			{
				DataGlobalCoords d;
				size_t cntr = 0;
				for ( size_t j=0; j<kMaxPlayers; j++ )
				{
					if ( i!=j && net->Used(j) )
					{
						if ( byte(j)==(net->MyId()-1) )
						{
							d.Add( player );
						}
						else
						{
							DataParticulars& dat = GetData(j+1);
							dat.playerId = j+1;
							d.Add( dat );
						}
						cntr++;
					}
				}
				if ( cntr>0 )
				{
					if ( scoreUpdated )
						d.SetScores( spack );
					byte buf[1024];
					size_t s = d.Set( buf );
					WriteId( byte(i+1), buf, s );
				}
			}
		}
		scoreUpdated = false;
	}
	// client processing (iff I am a client)
	else if ( net!=NULL && net->clientConnected && net->MyId()>0 )
	{
		DataParticulars& player = players[net->MyId()-1];
		player.playerId = net->MyId();

		if ( (keys&aJump)>0 )
		{

			if ( player.vehicleId>0 )
			{
				characters[net->MyId()-1].JumpOutOfTank( tanks[player.vehicleId-1] );
				tanks[player.vehicleId-1].Occupied( false );
				player.vehicleId = 0;
			}
			else
			{
				// pick the vehicle I am inside off
				// that doesn't have anyone in it
				for ( size_t i=0; i<kMaxPlayers; i++ )
				{
					if ( net->Used(i) )
					if ( tanks[i].InsideVehicle( player.mx, player.my, player.mz ) )
					if ( !tanks[i].Occupied() )
					{
						tanks[i].Occupied( true );
						player.vehicleId = i+1;
					}
				}
			}

			keys = keys & ~aJump;
		}

		if ( player.vehicleId>0 )
		{
			TTank& tank = tanks[player.vehicleId-1];
			if ( refuel )
			{
				scoreUpdated = true;
				refuel = false;

				byte shield, fuel, shells;
				tank.GetDetails( shield, fuel, shells );
				fuel += 50;
				if ( fuel>100 )
					fuel = 100;
				tank.SetDetails( shield, fuel, shells );
			}
			if ( rearm )
			{
				scoreUpdated = true;
				rearm = false;

				byte shield, fuel, shells;
				tank.GetDetails( shield, fuel, shells );
				shells += 10;
				if ( shells>50 )
					shells = 50;
				tank.SetDetails( shield, fuel, shells );
			}

			// scoring handled by the server
			player.destruct = selfDestruct;
			if ( selfDestruct )
			{
				selfDestruct = false;
				tank.Shield( 0 );
				tank.UpdateShields();
			}
		}

		bool fire = false;
		if ( !gameOver && landObj!=NULL )
			fire = Logic( *landObj, players[net->MyId()-1], keys, ctrlDown );

		// write my details to server
		if ( fire )
		{
			DataFire fire(net->MyId());
			fire.range = 1;
			fire.strength = 1;
			player.SetFire( fire ); // fire msgs piggy-back coord packages
		}
		byte buf[1024];
		size_t s = player.Set( buf );
		WriteId( net->ServerId(), buf, s );

		// update tank shields regardless of status

		for ( size_t i=0; i<kMaxPlayers; i++ )
		{
			if ( net->Used(i) )
				tanks[i].UpdateShields();
		}

	}
}

*/
