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

#include <dialogs/singlePlayerDialog.h>
#include <dialogs/hostdialog.h>
#include <dialogs/joindialog.h>
#include <systemDialogs/aboutdlg.h>
#include <dialogs/downloaddlg.h>
#include <dialogs/videoSettings.h>
#include <dialogs/helpDlg.h>

#include <tank/tankapp.h>

#include <network/networking.h>
#include <network/packet.h>

#include <io.h>

//==========================================================================

#include <mmsystem.h>

//==========================================================================

const float Deg2Rad = 0.01745329252f;
const float Rad2Deg = 57.295779511273f;

//==========================================================================

void TApp::ReloadAsRequired( void )
{
	TString errStr;
	if ( singlePlayerMission || clientGame || serverGame )
	{
		if ( !ReloadObjects(errStr) )
		{
			errStr = "\n" + errStr + "\n";
			WriteString( errStr.c_str() );
			::PostQuitMessage(-1);
		}
	}
	else
	{
		if ( !ReloadMenu(errStr) )
		{
			errStr = "\n" + errStr + "\n";
			WriteString( errStr.c_str() );
			::PostQuitMessage(-1);
		}
	}
};

void TApp::DoMenu( int menu )
{
	TEvent::DoMenu( menu );

	if ( menu==IDM_SINGLEPLAYER ||
		 menu==IDM_HOST ||
		 menu==IDM_JOIN )
	{
		if ( nwClientActive() || clientGame )
		{
			char msg[256];
			strcpy(msg,STR(strAreYouSureClient));
			if ( ::Message( this, msg, STR(strWarning), MB_YESNO|MB_ICONWARNING )==IDYES )
			{
				nwStopNetwork();
			}
			else
				return;
		}
		if ( nwServerActive() || serverGame )
		{
			char msg[256];
			strcpy(msg, STR(strAreYouSureServer) );
			if ( ::Message( this, msg, STR(strWarning), MB_YESNO|MB_ICONWARNING )==IDYES )
			{
				nwStopNetwork();
			}
			else
				return;
		}

		if ( singlePlayerMission && (!gameOver && !missionAccomplished) )
		{
			char msg[256];
			strcpy(msg,STR(strAreYouSureSinglePlayer));
			if ( ::Message( this, msg, STR(strWarning), MB_YESNO|MB_ICONWARNING )!=IDYES )
				return;
			singlePlayerMission = false;
			StopMusic();
		}
		else if ( gameOver || missionAccomplished )
		{
			singlePlayerMission = false;
			StopMusic();
		}
	}

	switch( menu )
	{

	case IDM_DOWNLOAD:
		{
			showDownload = true;

			DownloadDialog dlg( this, hInstance, hWindow );
			if ( dlg.Execute() )
			{
				if ( ForceWindowed() )
				{
					ReloadAsRequired();
				}
				const char* url = "http://www.peter.co.nz";
				::ShellExecute( hWindow, "open", url, "", "", SW_SHOW );
			}

			showDownload = false;
			break;
		}

	case IDM_SINGLEPLAYER:
		{
			clientGame = false;
			serverGame = false;
			SinglePlayerDialog dlg( this, hInstance, hWindow, &stories );
			TGameParameters hp;
			if ( dlg.Execute( hp ) )
			{
				prepareClientServer = false;

				for ( size_t i=0; i<kMaxPlayers; i++ )
				{
					multiplayerNames[i] = "";
					multiplayerSlotInUse[i] = false;
					multiplayerPlaying[i] = false;
				}

				// accept story?
				TString errStr;
				stories.SetBinaries( missionControl );
				if ( stories.Execute(this,hp.map,errStr) )
				{
					map = hp.map;

					myId = 0;
					team[myId] = 0;

					operatorId = 1;
					if ( !ReloadObjects(errStr) )
					{
						::MessageBox( hWindow, errStr.c_str(), STR(strError), MB_OK | MB_ICONERROR );
						return;
					}

					TDefFile def;
					if ( !GetStoryDef( map, errStr, def ) )
					{
						::MessageBox( hWindow, errStr.c_str(), STR(strError), MB_OK | MB_ICONERROR );
						return;
					}

					// stop and start music according to settings
					StopMusic();
					if ( playMusic )
					{
						StartMusic( def.SoundTrack() );
					}

					gameType = 0;
					size_t team = 0;
					multiplayerSlotInUse[myId] = true;

					TLandscapeObject obj = landObj->GetSinglePlayer(team);

					PreCond( team < kMaxTeams );
					AddPlayer( myId, team, hp.character, hp.name );

					// setup other environmental factors
					// such as keys, menu visibility, and gameOver
					// and missionAccomplished flags
					keys = 0;
					showMenu = false;
					singlePlayerMission = true;
					gameOver = false;
					missionAccomplished = false;

					cameraType = 1;
					cam.Type(cameraType);
					showScores = false;
					showMap = false;

					// remove missionControlText
					stories.SetBinaries(NULL);
				}
				else
				{
					if ( errStr.length()>0 )
					{
						::MessageBox( hWindow, errStr.c_str(), STR(strError), MB_OK | MB_ICONERROR );	
					}
				}
			}
			break;
		}

	case IDM_HOST:
		{
			clientGame = false;
			serverGame = false;
			HostDialog dlg( this );
			TGameParameters hp;
			if ( dlg.Execute( hp ) )
			{
				nwStartHost( hp.gameName, hp.port );

				prepareClientServer = true;
				scoreUpdated = false;
				for ( size_t i=0; i<kMaxPlayers; i++ )
				{
					team[i] = 0;
					scores[i] = 0;
					multiplayerNames[i] = "";
					multiplayerSlotInUse[i] = false;
					multiplayerPlaying[i] = false;
				}

				gameType = hp.gameType;
				myTeam = hp.team;
				maxPoints = hp.maxPoints;

				myId = 0;
				team[myId] = hp.team;
				map = hp.map;
				multiplayerNames[myId] = hp.name;
				multiplayerSlotInUse[myId] = true;
				multiplayerPlaying[myId] = true;

				TString errStr;

				// doh!  reload objects destroys window -> network fails because
				// it uses the previous window for event pumping
				if ( !ReloadObjects(errStr) )
				{
					::MessageBox( hWindow, errStr.c_str(), STR(strError), MB_OK | MB_ICONERROR );	
					return;
				}

				PreCond( hp.team < kMaxTeams );
				AddPlayer( myId, hp.team, hp.character, hp.name );

				keys = 0;
				showMenu = false;
				singlePlayerMission = false;
				gameOver = false;
				missionAccomplished = false;

				cameraType = 1;
				cam.Type(cameraType);
				showScores = false;
				showMap = false;
				serverGame = true;
			}
			break;
		}


	case IDM_JOIN:
		{
			size_t i;

			clientGame = false;
			serverGame = false;
			clientReceivedWelcome = false;
			clientReceivedStart = false;
			clientError = false;
			JoinDialog dlg( this );

			for ( i=0; i<kMaxPlayers; i++ )
			{
				team[i] = 0;
				scores[i] = 0;
				multiplayerNames[i] = "";
				multiplayerSlotInUse[i] = false;
				multiplayerPlaying[i] = false;
			}

			TGameParameters jp;
			if ( dlg.Execute( jp ) )
			{
				gameType = jp.gameType;
				myTeam = jp.team;
				maxPoints = jp.maxPoints;
				team[myId] = jp.team;
				multiplayerNames[myId] = jp.name;
				multiplayerSlotInUse[myId] = true;
				map = jp.map;

				// load all necessairy objects & set res if necessairy
				TString errStr;
				if ( !ReloadObjects(errStr) )
				{
					::MessageBox( hWindow, errStr.c_str(), STR(strError), MB_OK | MB_ICONERROR );	
					return;
				}

				PreCond( jp.team < kMaxTeams );
				AddPlayer( myId, jp.team, jp.character, jp.name );

				keys = 0;
				showMenu = false;
				singlePlayerMission = false;
				gameOver = false;
				missionAccomplished = false;

				cameraType = 1;
				cam.Type(cameraType);
				showScores = false;
				showMap = false;

				// load all other characters of player details received
				for ( i=0; i<kMaxPlayers; i++ )
				{
					if ( myId!=i )
					{
						if ( multiplayerSlotInUse[i] )
						{
							PreCond( team[i] < kMaxTeams );
							AddPlayer( i, team[i], character[i], multiplayerNames[i] );
							TString msgStr = multiplayerNames[i] + " ";
							msgStr = msgStr + STR(strJoined);
							msg->AddMessage( msgStr );
						}
					}
				}

				// now send start message to indicate client is ready to go!
				SendClientStartMessageToServer();

				clientGame = true;

			}
			else // disconnect
			{
				clientGame = false;
				nwStopNetwork();
			}
			break;
		}

	case IDM_ABOUT:
		{
			showAbout = true;
			aboutAngle = 0;

			StopMusic();
			if ( playMusic )
			{
				if ( soundSystem!=NULL )
				if ( soundSystem->OpenStream( "data\\music\\aquarium.mp3" ) )
				{
					soundSystem->PlayStream();
				}
			}

			AboutDialog dlg( this, hInstance, hWindow, registeredMsg );
			dlg.Execute();
			showAbout = false;

			StopMusic();
			break;
		}

	case IDM_HELP:
		{
			HelpDialog dlg( this );
			dlg.Execute();
			break;
		}

	case IDM_VIDEOSETTINGS:
		{
			VideoSettingsDialog dlg( this, hInstance, hWindow, soundSystem );
			if ( dlg.Execute()==IDOK )
			{
				size_t newLanguageId;
				if ( dlg.LanguageChanged( newLanguageId ) )
				{
					TString errStr;
					LanguageId( newLanguageId );
					if ( !LoadAboutScreen( errStr ) )
					{
						errStr = "\n" + errStr + "\n";
						WriteString( errStr.c_str() );
						::PostQuitMessage(-1);
					}

					// re-load stories
					WriteString( "Language changed, re-loading story files\n" );
					if ( !stories.Reload( this, errStr ) )
					{
						errStr = "\n" + errStr + "\n";
						WriteString( errStr.c_str() );
						::PostQuitMessage(-1);
					}
				};

				TString errStr;
				// in game?  then reload objects completely
				if ( singlePlayerMission || clientGame || serverGame )
				{
					if ( dlg.NeedsReload() )
					if ( !ReloadGraphics(errStr) )
					{
						errStr = "\n" + errStr + "\n";
						WriteString( errStr.c_str() );
						::PostQuitMessage(-1);
					}
				}
				else // just doing a menu - just reload it
				{
					if ( dlg.NeedsReload() )
					{
						if ( !ReloadMenu(errStr) )
						{
							errStr = "\n" + errStr + "\n";
							WriteString( errStr.c_str() );
							::PostQuitMessage(-1);
						}

						// center the menu
						CenterMenu();
					}

					// get/update sound settings
					size_t playMusicSetting = 0;
					if ( !GetRegistryKey( "Software\\PDV\\Performance", "PlayMusic", playMusicSetting ) )
						playMusicSetting = 1;
					playMusic = (playMusicSetting==1);

					if ( !GetRegistryKey( "Software\\PDV\\Performance", "EffectsVolume", effectsVolume ) )
						effectsVolume = 255;
					if ( !GetRegistryKey( "Software\\PDV\\Performance", "MusicVolume", musicVolume ) )
						musicVolume = 128;

					if ( soundSystem!=NULL )
					{
						soundSystem->Volume( effectsVolume );
						soundSystem->StreamVolume( musicVolume );
					}
				}
			}
			break;
		}

	case ID_HELP_CONTROLS:
		{
			ControlsDialog dlg( this, hInstance, hWindow );
			dlg.Execute(userKeys);
			break;
		}

	case IDM_MANUAL:
		{
			char fname[256];
			strcpy( fname, "system\\data\\man\\man.htm" );
			if ( !TPersist::FileExists( fname ) )
			{
				strcpy( fname, "data\\man\\man.htm" );
			}
			ShellExecute( hWindow, "open", fname, "", "", SW_SHOW );
			break;
		}

	default:
		break;
	};
};


