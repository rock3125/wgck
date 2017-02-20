#include <precomp_header.h>

#include <windows.h>

#include <systemDialogs/dialogAssist.h>
#include <dialogs/joinDialog.h>
//#include <dialogs/customiseVehicle.h>

#include <network/networking.h>

#include <tank/tankapp.h>
#include <object/character.h>

#include <win32/resource.h>

//==========================================================================
// the long night has come

JoinDialog::JoinDialog( TApp* _app )
	: app( _app ),
	  joinPage( NULL )
{
	PreCond( app!=NULL );

	TString errStr;
	if ( !app->GetModels( numModels, characters, modelNames, errStr ) )
	{
		WriteString( errStr.c_str() );
		PostCond( "SinglePlayerDialog: GetModels() error\n"==NULL );
	}
	if ( numModels==0 )
	{
		PostCond( "\nError: found zero models for game\n"==NULL );
	}

	languageId = 0;

	numMaps = 0;
	app->GetMultiPlayerMaps( NULL, numMaps );
	if ( numMaps==0 )
	{
		PostCond( "\nError: found zero multi-player maps for game\n"==NULL );
	}
	maps = new TDefFile[numMaps];
	PostCond( maps!=NULL );
	app->GetMultiPlayerMaps( maps, numMaps );

	joined = false;
	startJoin = false;
	timer = 0;

	startConnect = false;
	connected = false;

	redraw = false;

	yAngle = 0;
	joinDialogDepth = -98;
};


JoinDialog::~JoinDialog( void )
{
	if ( joinPage!=NULL )
		delete joinPage;
	joinPage = NULL;

	if ( maps!=NULL )
		delete []maps;
	maps = NULL;
};


void JoinDialog::SetupGuest( const TString&  _playerName, size_t port, const TString& _hostName )
{
	app->SetupGuest( _playerName, port, _hostName );
};


bool JoinDialog::Execute( TGameParameters& _gp )
{
	if ( joinPage!=NULL )
		delete joinPage;
	joinPage = NULL;

	// set game parameters
	gp = _gp;
	GetRegistrySettings();

	// just in case
	::nwStopNetwork();

	AddItem( joinPage, new TGUI( app, guiWindow, 0, 0, 403, 277, joinDialogDepth, STR(strJoinExisting) ) );
	AddItem( joinPage, new TGUI( app, guiLabel, 76,20,27,14, joinDialogDepth, STR(strName), 255,255,255 ) );

	// Join dialog
	switch ( languageId )
	{
	default:
		{
			AddItem( joinPage, new TGUI( app, guiLabel, 16,40,50,14, joinDialogDepth, STR(strHostName), 255,255,255 ) );
			AddItem( joinPage, new TGUI( app, guiLabel, 36,60,43,14, joinDialogDepth, STR(strTCPIPPort), 255,255,255 ) );
			AddItem( joinPage, new TGUI( app, guiLabel, 12,210,26,14, joinDialogDepth, STR(strCharacter), 255,255,255 ) );
			AddItem( joinPage, new TGUI( app, guiLabel, 14,230,24,14, joinDialogDepth, STR(strTeam), 255,255,255 ) );

			connectButton = new TGUI( app, guiButton, 260,20,120,25, joinDialogDepth, STR(strConnect) );
			searchButton  = new TGUI( app, guiButton, 260,50,120,25, joinDialogDepth, STR(strSearchForGames) );
			joinButton = new TGUI( app, guiButton, 285,244,50,25, joinDialogDepth, STR(strJoin) );
			cancelButton = new TGUI( app, guiButton, 340,244,50,25, joinDialogDepth, STR(strCancel) );
			break;
		}
	case 2:
	case 1:
		{
			AddItem( joinPage, new TGUI( app, guiLabel, 26,40,50,14, joinDialogDepth, STR(strHostName), 255,255,255 ) );
			AddItem( joinPage, new TGUI( app, guiLabel, 36,70,43,14, joinDialogDepth, STR(strTCPIPPort), 255,255,255 ) );
			AddItem( joinPage, new TGUI( app, guiLabel, 12,220,26,14, joinDialogDepth, STR(strCharacter), 255,255,255 ) );
			AddItem( joinPage, new TGUI( app, guiLabel, 14,240,24,14, joinDialogDepth, STR(strTeam), 255,255,255 ) );

			connectButton = new TGUI( app, guiButton, 255,20,130,25, joinDialogDepth, STR(strConnect) );
			searchButton  = new TGUI( app, guiButton, 255,50,130,25, joinDialogDepth, STR(strSearchForGames) );
			joinButton = new TGUI( app, guiButton, 235,244,70,25, joinDialogDepth, STR(strJoin) );
			cancelButton = new TGUI( app, guiButton, 310,244,80,25, joinDialogDepth, STR(strCancel) );
			break;
		}
	}

	nameEdit = new TGUI( app, guiEdit, 120,20,131,14, joinDialogDepth );
	hostNameEdit = new TGUI( app, guiEdit, 120,40,131,14, joinDialogDepth );
	hostNameEdit->MaxText( 40 );

	switch ( languageId )
	{
	default:
		{
			characterDropDown = new TGUI( app, guiDropDown, 70,210,90,14, joinDialogDepth );
			teamDropDown = new TGUI( app, guiDropDown, 70,230,90,14, joinDialogDepth );
			lbSearch = new TGUI( app, guiListbox, 20,90,200,120, joinDialogDepth );
			portEdit = new TGUI( app, guiEdit, 120,60,47,14, joinDialogDepth );
			break;
		}
	case 2:
	case 1:
		{
			characterDropDown = new TGUI( app, guiDropDown, 70,220,90,14, joinDialogDepth );
			teamDropDown = new TGUI( app, guiDropDown, 70,240,90,14, joinDialogDepth );
			lbSearch = new TGUI( app, guiListbox, 20,90,200,120, joinDialogDepth );
			portEdit = new TGUI( app, guiEdit, 120,70,47,14, joinDialogDepth );
			break;
		}
	}

	joinButton->ButtonKey( 13 );
	cancelButton->ButtonKey( VK_ESCAPE );

	AddItem( joinPage, nameEdit );
	AddItem( joinPage, hostNameEdit );
	AddItem( joinPage, portEdit );
	AddItem( joinPage, connectButton );
	AddItem( joinPage, searchButton );
	AddItem( joinPage, lbSearch );
	AddItem( joinPage, characterDropDown );
	AddItem( joinPage, teamDropDown );
	AddItem( joinPage, joinButton );
	AddItem( joinPage, cancelButton );

	for ( size_t i=0; i<numModels; i++ )
	{
		TString name = modelNames[i];
		characters[i].Reset();
		characterDropDown->AddDropDownItem( name );
	}

	characterDropDown->SelectedItem( gp.character );
	teamDropDown->SelectedItem( gp.team );
	nameEdit->Text( gp.name );
	hostNameEdit->Text( gp.hostName );
	portEdit->Text( Int2Str(gp.port) );

	TCharacter* mdlChar = NULL;
	if ( gp.character >= 0 || size_t(gp.character) < numModels )
	{
		mdlChar = &characters[gp.character];
	}
	charGraphic = new TGUI( app, guiCompoundObject,300,220,joinDialogDepth+32, 
							mdlChar );
	charGraphic->Rotate( 0,0,0 );
	charGraphic->Scale( 110 );
	AddItem( joinPage, charGraphic );

	joinPage->SetLocation( 100, 150 );

	TGUI* prevPage = app->CurrentPage();
	app->CurrentPage( joinPage );

	MSG msg;
	size_t fpsTimer = ::GetTickCount();
	size_t animTimer = ::GetTickCount() + 50;
	bool closing = false;
	float rot = 0;
	while ( !closing )
	{
		// process networking at the same time
		app->ClientNetworkLogic();

		size_t time = ::GetTickCount();
		::ProcessMessages( app, msg, fpsTimer );
		if ( msg.message==WM_QUIT || msg.message==WM_CLOSE )
		{
			closing = true;
			app->CurrentPage( prevPage );
			::PostQuitMessage( 0 );
		}
		UpdateJoinButtons();
		charGraphic->Rotate( 0,rot,0 );
		if ( time > animTimer )
		{
			mdlChar = NULL;
			if ( gp.character >= 0 || size_t(gp.character) < numModels )
			{
				mdlChar = &characters[gp.character];
			}
			if ( charGraphic->GetModel()!=mdlChar )
			{
				charGraphic->SetModel( mdlChar );
			}
			animTimer = time + 50;
			rot = rot + 1;
		}
		if ( searchButton->Clicked() )
		{
			searchButton->EnableControl( false );
			::InvalidateRect( app->Window(), NULL, true );
			::ProcessMessages( app, msg, fpsTimer );

			lbSearch->ClearItems();
			if ( ::nwFindHost( hostNames, hostIPs, hostPorts, numPlayers, gameNames ) )
			{
				size_t cnt = hostNames.NumItems(',');

				// max of 4 games for now on display - so I don't have to scroll
				if ( cnt>4 )
					cnt = 4;

				for ( size_t i=0; i<cnt; i++ )
				{
					TString item;
					item = Int2Str(i+1) + ". " + gameNames.GetItem(',',i);
					lbSearch->AddListBoxItem( item );
					
					item = "   on ";
					TString name = hostNames.GetItem(',',i);
					if ( name.length()==0 )
					{
						name = hostIPs.GetItem(',',i);
					}
					item = item + name + " (";
					item = item + numPlayers.GetItem(',',i) + " players)";
					lbSearch->AddListBoxItem( item );
				}
			}
			else
			{
				lbSearch->AddListBoxItem( STR(strNoGamesFound) );
			}

			searchButton->EnableControl( true );
			::InvalidateRect( app->Window(), NULL, true );
		};
		
		if ( lbSearch->SelectedItem()>=0 )
		{
			if ( hostNames.NumItems(',')>0 )
			{
				gp.hostName = hostIPs.GetItem(',',lbSearch->SelectedItem() );
				if ( gp.hostName.length()==0 )
				{
					gp.hostName = hostNames.GetItem(',',lbSearch->SelectedItem() );
				}
				gp.port = atoi( hostPorts.GetItem(',',lbSearch->SelectedItem()).c_str() );
				hostNameEdit->Text( gp.hostName );
				portEdit->Text( Int2Str(gp.port) );
			}
		}

		if ( connectButton->Clicked() )
		{
			connectButton->EnableControl( false );
			searchButton->EnableControl( false );
			timer = ::GetTickCount() + 15000; // 15 seconds timeout
			startJoin = true;
			gp.name = nameEdit->Text();
			gp.hostName = hostNameEdit->Text();
			gp.port = atoi(portEdit->Text().c_str());
			SetupGuest( gp.name, gp.port, gp.hostName );
			::InvalidateRect( app->Window(), NULL, true );
		}
		if ( joinButton->Clicked() )
		{
			joinButton->EnableControl( false );
			searchButton->EnableControl( false );
			timer = ::GetTickCount() + 15000; // 15 seconds timeout
			startConnect = true;
			connected = false;

			// send another packet explaining how we would like to start
			// (team positions if appropriate)
			byte buf[kNetworkBufferSize];

			size_t team = DropDownToTeamId();
			DataICanJoin reply( app->MyId(), true, team );
			reply.character = gp.character;
			size_t s = reply.Set(buf);
			::nwWriteNetworkBuffer( buf, s );
		}
		if ( connected )
		{
			closing = true;
		}
		if ( cancelButton->Clicked() )
		{
			::nwStopNetwork();
			app->CurrentPage( prevPage );
			return false;
		}
	}
	app->CurrentPage( prevPage );

	if ( gp.gameType==1 )
	{
		// convert team index to actual team id
		PreCond( gp.map < int(numMaps) );
		TMapPlayerInfo mapInfo = maps[gp.map].GetPlayerInfo();
		PreCond( mapInfo.numTeams>0 );
		int count = 0;
		bool finished = false;
		for ( i=0; i<kMaxTeams && !finished; i++ )
		{
			if ( mapInfo.teamCount[i]>0 && gp.team==count )
			{
				gp.team = i;
				finished = true;
			}
			else if ( mapInfo.teamCount[i]>0 )
			{
				count++;
			}
		}
		PostCond( finished );
	}
	else
	{
		gp.team = 0;
	}

	SetRegistryKey( "Software\\PDV\\Performance", "PlayerName", gp.name );
	SetRegistryKey( "Software\\PDV\\Performance", "HostName", gp.hostName );
	SetRegistryKey( "Software\\PDV\\Performance", "Character", size_t(gp.character) );
	SetRegistryKey( "Software\\PDV\\Performance", "port", gp.port );
	SetRegistryKey( "Software\\PDV\\Performance", "Team", gp.team );

	// write game parameters back
	_gp = gp;

	return true;
};


void JoinDialog::UpdateJoinButtons( void )
{
	if ( !joined && app->ClientReceivedWelcome() )
	{
		redraw = true;
		joined = true;
		::InvalidateRect( app->Window(), NULL, true );

		teamDropDown->ClearItems();
		gp.gameType = app->GameType();
		gp.map = app->Map();

		TMapPlayerInfo mapInfo = app->LandscapeMapDef();
		PreCond( mapInfo.numTeams>0 );
		if ( mapInfo.teamCount[redTeam]>0 )
		{
			teamDropDown->AddDropDownItem( STR(strRedTeam) );
		}
		if ( mapInfo.teamCount[greenTeam]>0 )
		{
			teamDropDown->AddDropDownItem( STR(strGreenTeam) );
		}
		if ( mapInfo.teamCount[blueTeam]>0 )
		{
			teamDropDown->AddDropDownItem( STR(strBlueTeam) );
		}
		if ( mapInfo.teamCount[yellowTeam]>0 )
		{
			teamDropDown->AddDropDownItem( STR(strYellowTeam) );
		}
	}

	if ( startJoin && !joined && app->ClientError() )
	{
		app->ClientError( false );
		startJoin = false;
		::Message( app, app->ClientErrorStr(), STR(strError), MB_OK | MB_ICONERROR );
		::nwStopNetwork();
		if ( !connectButton->Enabled() )
		{
			connectButton->EnableControl( true );
			searchButton->EnableControl( true );
		}
	}

	if ( !joined && startJoin )
	if ( ::GetTickCount() > timer )
	{
		startJoin = false;
		timer = 0;
		TString msg;
		msg = STR(strServerNotResponding);
		::Message( app, msg, STR(strError), MB_OK | MB_ICONERROR );
		::nwStopNetwork();
		if ( !connectButton->Enabled() )
		{
			connectButton->EnableControl( true );
			searchButton->EnableControl( true );
		}
		return;
	}

	if ( !connected && app->ClientReceivedStart() )
	{
		redraw = true;
		connected = true;
		::InvalidateRect( app->Window(), NULL, true );
	}

	if ( startConnect && !connected && app->ClientError() )
	{
		app->ClientError( false );
		startConnect = false;
		::Message( app, app->ClientErrorStr(), STR(strError), MB_OK | MB_ICONERROR );
		if ( !joinButton->Enabled() )
		{
			joinButton->EnableControl( false );
			searchButton->EnableControl( true );
		}
	}

	if ( !connected && startConnect )
	if ( ::GetTickCount() > timer )
	{
		startConnect = false;
		timer = 0;
		TString msg;
		msg = STR(strServerNotResponding);
		::Message( app, msg, STR(strError), MB_OK | MB_ICONERROR );
		if ( !joinButton->Enabled() )
		{
			joinButton->EnableControl( false );
			searchButton->EnableControl( true );
		}
		return;
	}

//	joined = app->ClientReceivedWelcome();
//	connected = app->ClientReceivedStart();

	gp.name = nameEdit->Text();
	gp.hostName = hostNameEdit->Text();
	gp.port = atoi(portEdit->Text().c_str());
	if ( characterDropDown->SelectedItem()!=gp.character )
	{
		::InvalidateRect( app->Window(), NULL, true );
	}

	gp.character = characterDropDown->SelectedItem();
	gp.team = teamDropDown->SelectedItem();

	if ( gp.name.length()>0 && 
		 gp.hostName.length()>0 && 
		 gp.port>0 )
	{
		if ( !startJoin )
		{
			if ( !connectButton->Enabled() )
			{
				connectButton->EnableControl( true );
				searchButton->EnableControl( true );
			}
		}
	}
	else
	{
		if ( connectButton->Enabled() )
		{
			connectButton->EnableControl( false );
		}
	}

	if ( joined && !startConnect )
	{		
		if ( connectButton->Enabled() )
		{
			connectButton->EnableControl( false );
			searchButton->EnableControl( false );
		}
		if ( !joinButton->Enabled() )
		{
			if ( gp.gameType==0 )
			{
				joinButton->EnableControl( true );
			}
			else
			{
				if ( teamDropDown->SelectedItem()>=0 )
				{
					joinButton->EnableControl( true );
				}
			}
			searchButton->EnableControl( true );
		}
	}
	else
	{
		if ( joinButton->Enabled() )
		{
			joinButton->EnableControl( false );
		}
	}

	if ( redraw )
	{
		redraw = false;

		bool enableTeam = (gp.gameType!=0);
		if ( teamDropDown->Enabled()!=enableTeam )
			teamDropDown->EnableControl( enableTeam );
		if ( nameEdit->Enabled() )
			nameEdit->EnableControl( false );
		if ( hostNameEdit->Enabled() )
			hostNameEdit->EnableControl( false );
		if ( portEdit->Enabled() )
			portEdit->EnableControl( false );
	}
};

void JoinDialog::GetRegistrySettings( void )
{
	TString name, host;
	if ( GetRegistryKey( "Software\\PDV\\Performance", "PlayerName", name ) )
	{
		gp.name = name;
	}
	if ( GetRegistryKey( "Software\\PDV\\Performance", "HostName", host ) )
	{
		gp.hostName = host;
	}
	size_t val;
	if ( GetRegistryKey( "Software\\PDV\\Performance", "Character", val ) )
	{
		gp.character = val;
	}

	if ( !GetRegistryKey( "Software\\PDV\\Performance", "Language", languageId ) )
	{
		languageId = 0;
	}

	gp.port = 2000; // default
	if ( GetRegistryKey( "Software\\PDV\\Performance", "Port", val ) )
	{
		gp.port = val;
	}

	// depends on map - not predictable
	gp.team = -1;
};

size_t JoinDialog::DropDownToTeamId( void )
{
	if ( gp.gameType==1 )
	{
		PreCond( gp.map < int(numMaps) );
		TMapPlayerInfo mapInfo = maps[gp.map].GetPlayerInfo();
		PreCond( mapInfo.numTeams>0 );
		int count = 0;
		bool finished = false;
		for ( size_t i=0; i<kMaxTeams && !finished; i++ )
		{
			if ( mapInfo.teamCount[i]>0 && gp.team==count )
			{
				return i;
			}
			else if ( mapInfo.teamCount[i]>0 )
			{
				count++;
			}
		}
		PostCond( finished );
	}
	else
	{
		gp.team = 0;
	}
	return gp.team;
};

//==========================================================================

