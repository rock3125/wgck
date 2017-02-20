#include <precomp_header.h>

#include <windows.h>

#include <tank/tankapp.h>

#include <systemDialogs/dialogAssist.h>
#include <dialogs/hostDialog.h>

#include <object/character.h>
#include <network/networking.h>

#include <win32/resource.h>

//==========================================================================

HostDialog::HostDialog( TApp* _app )
	: app( _app ),
	  hostPage( NULL ),
	  maps( NULL )
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

	yAngle = 0;
	hostDialogDepth = -98;
};

HostDialog::~HostDialog( void )
{
	if ( hostPage!=NULL )
		delete hostPage;
	hostPage = NULL;

	if ( maps!=NULL )
		delete []maps;
	maps = NULL;
};


bool HostDialog::Execute( TGameParameters& _gp )
{
	size_t i;

	if ( hostPage!=NULL )
		delete hostPage;
	hostPage = NULL;

	// copy game parameters
	gp = _gp;

	GetRegistrySettings();

	// Host dialog
	// Video dialog
	TString hostName, ipStr, hostStr;
	::nwGetLocalNetDetails( hostName, ipStr );

	AddItem( hostPage, new TGUI( app, guiWindow, 0, 0, 403, 297, hostDialogDepth, STR(strHostSettings) ) );
	AddItem( hostPage, new TGUI( app, guiLabel, 36,20,27,14, hostDialogDepth, STR(strName), 255,255,255 ) );

	AddItem( hostPage, new TGUI( app, guiLabel, 12,40,42,14, hostDialogDepth, STR(strGameName), 255,255,255 ) );
	AddItem( hostPage, new TGUI( app, guiLabel, 235,20,43,14, hostDialogDepth, STR(strTCPIPPort), 255,255,255 ) );
	AddItem( hostPage, new TGUI( app, guiLabel, 12,80,26,14, hostDialogDepth, STR(strCharacter), 255,255,255 ) );
	AddItem( hostPage, new TGUI( app, guiLabel, 20,100,18,14, hostDialogDepth, STR(strMap), 255,255,255 ) );

	AddItem( hostPage, new TGUI( app, guiLabel, 18,120,21,14, hostDialogDepth, STR(strGameType), 255,255,255 ) );
	AddItem( hostPage, new TGUI( app, guiLabel, 14,140,24,14, hostDialogDepth, STR(strTeam), 255,255,255 ) );

	switch ( languageId )
	{
	default:
		{
			hostButton = new TGUI( app, guiButton, 285,264,50,25, hostDialogDepth, STR(strStartHost) );
			cancelButton = new TGUI( app, guiButton, 340,264,50,25, hostDialogDepth, STR(strCancel) );

			hostStr = STR(strHostDetails) + hostName + " (" + ipStr + ")";
			AddItem( hostPage, new TGUI( app, guiLabel, 14,270,24,14, hostDialogDepth, hostStr, 255,255,255 ) );
			break;
		}
	case 2:
	case 1:
		{
			hostButton = new TGUI( app, guiButton, 145,264,160,25, hostDialogDepth, STR(strStartHost) );
			cancelButton = new TGUI( app, guiButton, 310,264,80,25, hostDialogDepth, STR(strCancel) );

			hostStr = STR(strHostDetails) + hostName + " (" + ipStr + ")";
			AddItem( hostPage, new TGUI( app, guiLabel, 14,180,24,14, hostDialogDepth, hostStr, 255,255,255 ) );
			break;
		}
	}

	nameEdit = new TGUI( app, guiEdit, 80,20,131,14, hostDialogDepth );
	nameEdit->Text( gp.name );
	nameEdit->MaxText( 10 );

	gameNameEdit = new TGUI( app, guiEdit, 80,40,131,14, hostDialogDepth );
	gameNameEdit->MaxText( 20 );
	gameNameEdit->Text( gp.gameName );

	portEdit = new TGUI( app, guiEdit, 300,20,47,14, hostDialogDepth );
	portEdit->Text( Int2Str(gp.port) );
	portEdit->MaxText( 5 );

	AddItem( hostPage, nameEdit );
	AddItem( hostPage, gameNameEdit );
	AddItem( hostPage, portEdit );

	characterDropDown = new TGUI( app, guiDropDown, 70,80,140,14, hostDialogDepth );
	mapDropDown = new TGUI( app, guiDropDown, 70,100,140,14, hostDialogDepth );
	typeDropDown = new TGUI( app, guiDropDown, 70,120,140,14, hostDialogDepth );
	teamDropDown = new TGUI( app, guiDropDown, 70,140,140,14, hostDialogDepth );

	AddItem( hostPage, characterDropDown );
	AddItem( hostPage, mapDropDown );
	AddItem( hostPage, typeDropDown );
	AddItem( hostPage, teamDropDown );

	for ( i=0; i<numMaps; i++ )
	{
		TString name = maps[i].IslandName();
		mapDropDown->AddDropDownItem( name );
	}

	for ( i=0; i<numModels; i++ )
	{
		TString name = modelNames[i];
		characters[i].Reset();
		characterDropDown->AddDropDownItem( name );
	}

	typeDropDown->AddDropDownItem( STR(strGameType1) );
	typeDropDown->AddDropDownItem( STR(strGameType2) );

	// get team info
	if ( gp.map>=0 )
	{
		PreCond( gp.map < int(numMaps) );
		TMapPlayerInfo mapInfo = maps[gp.map].GetPlayerInfo();
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

	characterDropDown->SelectedItem( gp.character );
	mapDropDown->SelectedItem( gp.map );
	typeDropDown->SelectedItem( gp.gameType );
	teamDropDown->SelectedItem( gp.team );

	AddItem( hostPage, hostButton );
	AddItem( hostPage, cancelButton );
	hostButton->ButtonKey( 13 );
	cancelButton->ButtonKey( VK_ESCAPE );

	TCharacter* mdlChar = NULL;
	if ( gp.character >= 0 || size_t(gp.character) < numModels )
	{
		mdlChar = &characters[gp.character];
	}
	charGraphic = new TGUI( app, guiCompoundObject,300,220,hostDialogDepth+32, 
							mdlChar );
	charGraphic->Rotate( 0,0,0 );
	charGraphic->Scale( 110 );
	AddItem( hostPage, charGraphic );

	hostPage->SetLocation( 100, 180 );

	TGUI* prevPage = app->CurrentPage();
	app->CurrentPage( hostPage );

	MSG msg;
	size_t fpsTimer = ::GetTickCount();
	size_t animTimer = ::GetTickCount() + 50;
	closing = false;
	float rot = 0;
	while ( !closing )
	{
		size_t time = ::GetTickCount();
		::ProcessMessages( app, msg, fpsTimer );
		if ( msg.message==WM_QUIT || msg.message==WM_CLOSE )
		{
			closing = true;
			app->CurrentPage( prevPage );
			::PostQuitMessage( 0 );
		}
		UpdateHostButtons();
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
		if ( hostButton->Clicked() )
		{
			closing = true;
		}
		if ( cancelButton->Clicked() )
		{
			app->CurrentPage( prevPage );
			return false;
		}
	}
	app->CurrentPage( prevPage );

	// convert team index to actual team id
	if ( gp.gameType==1 )
	{
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
	SetRegistryKey( "Software\\PDV\\Performance", "GameName", gp.gameName );
	SetRegistryKey( "Software\\PDV\\Performance", "Character", size_t(gp.character) );
	SetRegistryKey( "Software\\PDV\\Performance", "MPMap", size_t(gp.map) );
//	SetRegistryKey( "Software\\PDV\\Performance", "MaxPoints", size_t(gp.maxPoints) );
	SetRegistryKey( "Software\\PDV\\Performance", "Hosting", 1 );
	SetRegistryKey( "Software\\PDV\\Performance", "GameType", gp.gameType );
	SetRegistryKey( "Software\\PDV\\Performance", "Team", gp.team );
	SetRegistryKey( "Software\\PDV\\Performance", "port", gp.port );

	// write game parameters back
	_gp = gp;

	return true;
}


void HostDialog::UpdateHostButtons( void )
{
	if ( closing )
		return;

	gp.name = nameEdit->Text();
	gp.gameName = gameNameEdit->Text();
	gp.port = atoi(portEdit->Text().c_str());
//	gp.maxPoints = atoi( pointsEdit->Text().c_str() );

	gp.character = characterDropDown->SelectedItem();
	int smap = mapDropDown->SelectedItem();
	if ( smap!=gp.map )
	{
		gp.map = smap;

		teamDropDown->ClearItems();

		// get team info
		if ( gp.map>=0 )
		{
			PreCond( gp.map < int(numMaps) );

			TMapPlayerInfo mapInfo = maps[gp.map].GetPlayerInfo();
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
	}

	gp.gameType = typeDropDown->SelectedItem();
	if ( gp.gameType==1 )
	{
		if ( !teamDropDown->Enabled() )
		{
			teamDropDown->EnableControl( true );
		}
	}
	else
	{
		if ( teamDropDown->Enabled() )
		{
			teamDropDown->EnableControl( false );
		}
	}
	gp.team = teamDropDown->SelectedItem();

	if ( gp.gameName.length()>0 &&
		 gp.character!=-1 &&
		 gp.map!=-1 && 
		 gp.port>0 &&
//		 gp.maxPoints>0 &&
		 gp.gameType!=-1 )
	{
		if ( !hostButton->Enabled() )
		{
			hostButton->EnableControl( true );
		}
	}
	else
	{
		if ( hostButton->Enabled() )
		{
			hostButton->EnableControl( false );
		}
	}

	if ( gp.team==-1 && gp.gameType==1 )
	{
		if ( hostButton->Enabled() )
		{
			hostButton->EnableControl( false );
		}
	}
};


void HostDialog::GetRegistrySettings( void )
{
	TString name, game;
	if ( GetRegistryKey( "Software\\PDV\\Performance", "PlayerName", name ) )
	{
		gp.name = name;
	}
	if ( GetRegistryKey( "Software\\PDV\\Performance", "GameName", game ) )
	{
		gp.gameName = game;
	}
	size_t val;
	if ( GetRegistryKey( "Software\\PDV\\Performance", "Character", val ) )
	{
		gp.character = val;
	}

	// no teams - depends on map
	gp.team = -1;

	if ( GetRegistryKey( "Software\\PDV\\Performance", "MPMap", val ) )
	{
		gp.map = val;
	}
	gp.port = 2000; // default
	if ( GetRegistryKey( "Software\\PDV\\Performance", "Port", val ) )
	{
		gp.port = val;
	}
	if ( GetRegistryKey( "Software\\PDV\\Performance", "GameType", val ) )
	{
		gp.gameType = val;
	}
	if ( !GetRegistryKey( "Software\\PDV\\Performance", "Language", languageId ) )
	{
		languageId = 0;
	}
//	if ( GetRegistryKey( "Software\\PDV\\Performance", "MaxPoints", val ) )
//	{
//		gp.maxPoints = val;
//	}
};

//==========================================================================

TGameParameters::TGameParameters( void )
{
	port = 0;
	character = 0;
	team = 0;
	map = 0;
	gameType = 0;
	maxPoints = 0;
};


TGameParameters::~TGameParameters( void )
{
};

TGameParameters::TGameParameters( const TGameParameters& gp )
{
	operator=(gp);
}

const TGameParameters& TGameParameters::operator=( const TGameParameters& gp )
{
	name = gp.name;
	gameName = gp.gameName;
	hostName = gp.hostName;
	port = gp.port;

	character = gp.character;
	map = gp.map;
	team = gp.team;
	gameType = gp.gameType;
	
	maxPoints = gp.maxPoints;

	return *this;
};

//==========================================================================
