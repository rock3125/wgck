#include <precomp_header.h>

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

#include <network/networking.h>
#include <network/packet.h>

#include <object/createlandscape.h>

//==========================================================================

#include <mmsystem.h>

//==========================================================================
// The long night has come

const float Deg2Rad = 0.01745329252f;
const float Rad2Deg = 57.295779511273f;

const size_t mg42Character = 2;

//==========================================================================
// used by AIs - overall logic counter

size_t logicCounter = 0;
size_t aiId = 1;

//==========================================================================

TApp::TApp( HINSTANCE hInstance, TString _registeredMsg )
	: msg(NULL),
	  TEvent( hInstance, false ),
	  cloudTexture( NULL ),
	  landObj(NULL),
	  mainMenuBackground(NULL),
	  title(NULL),
	  missionControl(NULL),
	  gameOverText(NULL),
	  missionAccomplishedText(NULL),
	  sky(NULL),

	  explosion(NULL),
//	  explosionCircle(NULL),

	  bomb(NULL),
	  shell(NULL),
	  about(NULL),
	  downloadScreen(NULL),
	  aboutLogo(NULL),

	  parachute(NULL),

	  hMusicThread( NULL ),
	  resources(NULL),
	  gameObjects( NULL ),

	  sequenceLocations( NULL )
{
	TString errStr;
	nwStartWinsock( errStr );

	size_t i;

	prepareClientServer = false;
	tipsShown = false;

	numSequenceLocations = 0;
	myId = 0;
	operatorId = 1;

	// set icons to zero
	iconPath = "data\\icons";
	for ( i=0; i<6; i++ )
	{
		iconPerson[i] = NULL;
	};
	for ( i=0; i<7; i++ )
	{
		iconWeapon[i] = NULL;
	};
	for ( i=0; i<15; i++ )
	{
		iconVehicle[i] = NULL;
	};

	iconWeaponName[0] = "gun_magnum.jpg";
	iconWeaponName[1] = "gun_lasergun.jpg";
	iconWeaponName[2] = "gun_bazooka.jpg";
	iconWeaponName[3] = "gun_dynamite.jpg";
	iconWeaponName[4] = "gun_shell.jpg";
	iconWeaponName[5] = "gun_bomb.jpg";
	iconWeaponName[6] = "gun_mg.jpg";

	iconPersonName[0] = "sol_mgerman.jpg";
	iconPersonName[1] = "sol_famerican.jpg";
	iconPersonName[2] = "sol_mamerican.jpg";
	iconPersonName[3] = "sol_fgerman.jpg";
	iconPersonName[4] = "sol_menglish.jpg";
	iconPersonName[5] = "sol_fenglish.jpg";

	iconVehicleName[0] = "v_churchil.jpg";
	iconVehicleName[1] = "v_hetzer.jpg";
	iconVehicleName[2] = "v_tiger.jpg";
	iconVehicleName[3] = "v_sherman.jpg";
	iconVehicleName[4] = "v_flak1.jpg";
	iconVehicleName[5] = "v_flak2.jpg";
	iconVehicleName[6] = "v_gun1.jpg";
	iconVehicleName[7] = "v_gun2.jpg";
	iconVehicleName[8] = "v_mg42.jpg";
	iconVehicleName[9] = "v_triplane.jpg";
	iconVehicleName[10] = "v_me262.jpg";
	iconVehicleName[11] = "v_p51.jpg";
	iconVehicleName[12] = "v_spitfire.jpg";
	iconVehicleName[13] = "v_stuka.jpg";
	iconVehicleName[14] = "v_biplane.jpg";

	numResources = 0;
	showTarget = false;
	showAbout = false;
	showDownload = false;
	leftButtonDown = false;

	for ( i=0; i<kNumExplosionAnimations; i++ )
		explosionFlame[i] = NULL;

	registeredMsg = _registeredMsg;

	mp3playing = false;
	singlePlayerMission = false;
	clientGame = false;
	serverGame = false;
	animCount = 0;
	aboutAngle = 0;

	maxPoints = 50;
	gameOver = false;
	missionAccomplished = false;
	missionAccomplished = false;
	map = 0;
	clientReceivedWelcome = false;
	clientReceivedStart = false;
	clientError = false;

	backgroundBitmap = "day.bin";

	// atmos sound
	backgroundSound = sdaytime;
	backgroundSoundPlaying = false;
	backgroundSoundChannel = -1;

    path = "data";
	texturePath = "data\\textures";

	SetLogFile( "_enginelog.txt" );

	myTeam = 0;

	deltaXAngle = 0.0f;
	deltaYAngle = 0.0f;
	leftDown = false;
	rightDown = false;

	showBoundingBox = false;
	keys = 0;

	musicThreadId = 0;
	cameraType = 1;
	cam.Type(cameraType);

	lazy = false;

	showScores = false;
	showMap = false;

	msg = new TMessage();
	PostCond( msg!=NULL );

	for ( i=0; i<kMaxPlayers; i++ )
	{
		team[i] = 0;
		scores[i] = 0;
		multiplayerSlotInUse[i] = false;
		multiplayerPlaying[i] = false;
	}

	for ( i=0; i<kMaxObjects; i++ )
	{
		objects[i] = NULL;
	}
	for ( i=0; i<kMaxWeapons; i++ )
	{
		weapons[i] = NULL;
	}

	minMapScale = 0.02f;
	maxMapScale = 0.03f;
	mapScaleIncrease = 0.001f;
	mapScale = minMapScale;

	numObjects = 0;
	numModels = 0;
	numWeapons = 0;
	numTanks = 0;
	numAITanks = 0;
	numAIPlanes = 0;
	numAIFlak = 0;
	numAIArtillery = 0;
	numAIMG42 = 0;
	numPlanes = 0;
	numFlak = 0;
	numMG42s = 0;
	numArtillery = 0;
	numV2 = 0;

	lastProgress = 0;
	progressSize = 0;

	numGameObjects = 0;

	tips.LoadTips( "data\\tips.txt", errStr );
};


TApp::~TApp( void )
{
	size_t i;

	nwStopNetwork();
	nwStopWinsock();

	if ( mainMenuBackground!=NULL )
		delete mainMenuBackground;
	mainMenuBackground = NULL;

	if ( hMusicThread!=NULL )
	{
		DeleteObject( hMusicThread );
		hMusicThread = NULL;
	}

	if ( gameObjects!=NULL )
	{
		delete []gameObjects;
	};
	gameObjects = NULL;

	if ( mp3playing )
	{
		mp3playing = false;
		//PlaySound( NULL, NULL, SND_ASYNC );
		if ( soundSystem!=NULL )
		{
			soundSystem->StopStream();
		}
	}
	for ( i=0; i<numObjects; i++ )
	{
		if ( objects[i]!=NULL )
			delete objects[i];
		objects[i] = NULL;
	}
	for ( i=0; i<kMaxWeapons; i++ )
	{
		if ( weapons[i]!=NULL )
			delete weapons[i];
		weapons[i] = NULL;
	}

	if ( msg!=NULL )
		delete msg;
	msg = NULL;

	if ( msg!=NULL )
		delete msg;
	msg = NULL;

	if ( landObj!=NULL )
		delete landObj;
	landObj = NULL;

	if ( about!=NULL )
		delete about;
	about = NULL;

	if ( downloadScreen!=NULL )
		delete downloadScreen;
	downloadScreen = NULL;

	if ( aboutLogo!=NULL )
		delete aboutLogo;
	aboutLogo = NULL;

	if ( cloudTexture!=NULL )
		delete cloudTexture;
	cloudTexture = NULL;

	if ( sky!=NULL )
		delete sky;
	sky = NULL;

	if ( explosion!=NULL )
		delete explosion;
	explosion = NULL;

//	if ( explosionCircle!=NULL )
//		delete explosionCircle;
//	explosionCircle = NULL;

	if ( bomb!=NULL )
		delete bomb;
	bomb = NULL;

	if ( shell!=NULL )
		delete shell;
	shell = NULL;

	if ( parachute!=NULL )
		delete parachute;
	parachute = NULL;

	for ( i=0; i<6; i++ )
	{
		if ( iconPerson[i]!=NULL )
			delete iconPerson[i];
		iconPerson[i] = NULL;
	};
	for ( i=0; i<7; i++ )
	{
		if ( iconWeapon[i]!=NULL )
			delete iconWeapon[i];
		iconWeapon[i] = NULL;
	};
	for ( i=0; i<15; i++ )
	{
		if ( iconVehicle[i]!=NULL )
			delete iconVehicle[i];
		iconVehicle[i] = NULL;
	};

	for ( i=0; i<kNumExplosionAnimations; i++ )
	{
		if ( explosionFlame[i]!=NULL )
			delete explosionFlame[i];
		explosionFlame[i] = NULL;
	}

	if ( title!=NULL )
		delete title;
	title = NULL;

	if ( missionControl!=NULL )
		delete missionControl;
	missionControl = NULL;

	if ( gameOverText!=NULL )
		delete gameOverText;
	gameOverText = NULL;

	if ( resources!=NULL )
		delete []resources;
	resources = NULL;

	if ( missionAccomplishedText!=NULL )
		delete missionAccomplishedText;
	missionAccomplishedText = NULL;

	if ( sequenceLocations!=NULL )
		delete []sequenceLocations;
	sequenceLocations = NULL;
}


bool TApp::LoadAboutScreen( TString& errStr )
{
	if ( about!=NULL )
		delete about;
	about = NULL;
	if ( downloadScreen!=NULL )
		delete downloadScreen;
	downloadScreen = NULL;

	about = new TTexture();
	if ( !about->LoadBinary( STR(strAboutScreen), "data\\backgrounds", errStr, false ) )
		return false;

	downloadScreen = new TTexture();
	if ( !downloadScreen->LoadBinary( STR(strDownloadScreen), "data\\backgrounds", errStr, false ) )
		return false;

	PostCond( about!=NULL );
	return true;
};


bool TApp::ReloadMenu( TString& errStr )
{
	WriteString( "ReloadMenu:\n" );

	// must clear cache
	TTexture::ClearCache();

	if ( !ResetResolution(NULL,errStr) )
	{
		errStr = "\n" + errStr + "\n";
		WriteString( errStr.c_str() );
		return false;
	}

	leftButtonDown = false;

	::SetProgressMessage( STR(strLoadingObjects) );
	::SetProgress(0);

	WriteString( "loading background\n" );
	if ( mainMenuBackground!=NULL )
		delete mainMenuBackground;
	mainMenuBackground = new TTexture();
	if ( !mainMenuBackground->LoadBinary( "background01.jpg", "data\\backgrounds", errStr ) )
		return false;

	// load stories
	WriteString( "Loading story files\n" );
	if ( !stories.Load( this, errStr ) )
	{
		return false;
	}

	::SetProgress(25);

	WriteString( "loading text\n" );
	if ( title!=NULL )
		delete title;
	title = new TBinObject();
	if ( !title->LoadBinary( "3d_title.bin", errStr, path, texturePath ) )
		return false;

	if ( missionControl!=NULL )
		delete missionControl;
	missionControl = new TBinObject();
	if ( !missionControl->LoadBinary( "missioncontrol.bin", errStr, path, texturePath ) )
		return false;

	// set story binaries
	stories.SetBinaries( missionControl );

	if ( !LoadAboutScreen( errStr ) )
		return false;

	if ( aboutLogo!=NULL )
		delete aboutLogo;
	aboutLogo = new TBinObject();
	if ( !aboutLogo->LoadBinary( "xe_text.bin", errStr, path, texturePath ) )
		return false;

	::SetProgress(100);

	return true;
};


bool TApp::ResetObjects( TString& errStr )
{
	leftButtonDown = false;

	numTanks = 0;
	numPlanes = 0;
	numFlak = 0;
	numArtillery = 0;
	numMG42s = 0;
	numAITanks = 0;
	numAIPlanes = 0;
	numAIFlak = 0;
	numAIArtillery = 0;
	numAIMG42 = 0;
	numObjects = 0;
	numModels = 0;
	numV2 = 0;

	// get story - client/server stories are different from singlePlayer stories
	TDefFile def;
	if ( !GetStoryDef( map, errStr, def ) )
	{
		return false;
	}

	numSequenceLocations = 0;
	if ( sequenceLocations!=NULL )
	{
		delete []sequenceLocations;
		sequenceLocations = NULL;
	}
	if ( def.NumSequenceLocations() > 0 )
	{
		numSequenceLocations = def.NumSequenceLocations();
		sequenceLocations = new TConnections[numSequenceLocations];
		PostCond( sequenceLocations!=NULL );
		for ( size_t i=0; i<numSequenceLocations; i++ )
		{
			sequenceLocations[i] = def.SequenceConnection(i);
			sequenceLocations[i].visited = false;
		}
	};

	// clear object cache & re-setup landscape objects
	for ( size_t i=0; i<numObjects; i++ )
	{
		if ( objects[i]!=NULL )
			delete objects[i];
		objects[i] = NULL;
	}

	PreCond( landObj!=NULL );
	if ( !landObj->SetupLandscapeObjects( def, this, errStr ) )
	{
		return false;
	}
	landObj->RenderDepth( renderDepth );

	fog = false;
	switch (def.TimeOfDay())
	{
	case 0: // day
		{
			lightingSetting = 1;
			backgroundBitmap = "day.bin";
			backgroundSound = sdaytime;
			break;
		}
	case 1: // night
		{
			lightingSetting = 0.5f;
			backgroundBitmap = "night.bin";
			backgroundSound = snighttime;
			break;
		}
	case 2: // fog
		{
			backgroundBitmap = ""; // "fog.bin";
			lightingSetting = 1;
			backgroundSound = sfog;
			fog = true;
			break;
		}
	default:
		{
			lightingSetting = 1;
			backgroundBitmap = "";
			backgroundSound = 0;
			break;
		}
	}
	SetupFog( fog );

	if ( sky!=NULL )
		delete sky;
	if ( backgroundBitmap.length()>0 )
	{
		sky = new TBinObject();
		if (!sky->LoadBinary( backgroundBitmap, errStr, "data", "data\\textures" ) )
		{
			delete sky;
			sky = NULL;
			return false;
		}
	}
	else
	{
		sky = NULL;
	}

	// get resources for internal cache
	if ( resources!=NULL )
		delete []resources;
	resources = NULL;
	numResources = landObj->GetResourceObjects(NULL);
	if ( numResources>0 )
	{
		resources = new TLandscapeObject*[numResources];
		PostCond( resources!=NULL );
		landObj->GetResourceObjects( resources );
	}

	// set parameters accordingly
	params.numFlak = numFlak;
	params.numPlanes = numPlanes;
	params.numTanks = numTanks;
	params.numArtillery = numArtillery;
	params.numMG42s = numMG42s;
	params.flak = flak;
	params.tanks = tanks;
	params.planes = planes;
	params.artillery = artillery;
	params.mg42 = mg42;

	for ( i=0; i<kMaxPlayers; i++ )
	{
		characters[i].VehicleId( 0 );
		characters[i].VehicleType( vtNone );
	};

	// player access - changed slightly - slots in use is now used
	// by both multi- and single-player games
	params.numPlayers = kMaxPlayers;
	params.players = characters;
	params.inUse = multiplayerSlotInUse;

	return true;
};


bool TApp::ReloadObjects( TString& errStr )
{
	size_t i;

	// must clear cache
	TTexture::ClearCache();

	WriteString( "ReloadBinaries:\n" );

	if ( !ResetResolution(NULL,errStr) )
	{
		errStr = "\n" + errStr + "\n";
		WriteString( errStr.c_str() );
		return false;
	}

	::SetProgressMessage( STR(strLoadingObjects) );
	::SetProgress(0);

	WriteString( "loading background\n" );
	if ( mainMenuBackground!=NULL )
		delete mainMenuBackground;
	mainMenuBackground = new TTexture();
	if ( !mainMenuBackground->LoadBinary( "background01.jpg", "data\\backgrounds", errStr ) )
		return false;

	if ( !LoadAboutScreen( errStr ) )
		return false;

	WriteString( "loading in game text\n" );

	if ( title!=NULL )
		delete title;
	title = new TBinObject();
	if ( !title->LoadBinary( "3d_title.bin", errStr, path, texturePath ) )
		return false;

	if ( missionControl!=NULL )
		delete missionControl;
	missionControl = new TBinObject();
	if ( !missionControl->LoadBinary( "missioncontrol.bin", errStr, path, texturePath ) )
		return false;

	// set story binaries
	stories.SetBinaries( missionControl );

	if ( gameOverText!=NULL )
		delete gameOverText;
	gameOverText = new TBinObject();
	if ( !gameOverText->LoadBinary( "gameover.bin", errStr, path, texturePath ) )
		return false;

	if ( missionAccomplishedText!=NULL )
		delete missionAccomplishedText;
	missionAccomplishedText = new TBinObject();
	if ( !missionAccomplishedText->LoadBinary( "missionaccomplished.bin", errStr, path, texturePath ) )
		return false;

	if ( parachute!=NULL )
		delete parachute;
	parachute = new TBinObject();
	if ( !parachute->LoadBinary( "para.bin", errStr, "data", texturePath ) )
		return false;

	// load weapons
	WriteString( "loading weapons\n" );
	for ( i=0; i<kMaxWeapons; i++ )
	{
		if ( weapons[i]!=NULL )
			delete weapons[i];
		weapons[i] = NULL;
	}
	weapons[0] = new TBinObject();
	if ( !weapons[0]->LoadBinary( "magnum.bin", errStr, path, texturePath ) )
		return false;
	weapons[1] = new TBinObject();
	if ( !weapons[1]->LoadBinary( "lasergun.bin", errStr, path, texturePath ) )
		return false;
	weapons[2] = new TBinObject();
	if ( !weapons[2]->LoadBinary( "bazooka.bin", errStr, path, texturePath ) )
		return false;
	weapons[3] = new TBinObject();
	if ( !weapons[3]->LoadBinary( "explosives.bin", errStr, path, texturePath ) )
		return false;
	numWeapons = 4;

	WriteString( "loading ammunition\n" );
	if ( bomb!=NULL )
		delete bomb;
	bomb = new TBinObject();
	if ( !bomb->LoadBinary( "bomb.bin", errStr, path, texturePath ) )
		return false;

	if ( shell!=NULL )
		delete shell;
	shell = new TBinObject();
	if ( !shell->LoadBinary( "shell.bin", errStr, path, texturePath ) )
		return false;

	for ( i=0; i<6; i++ )
	{
		if ( iconPerson[i]!=NULL )
			delete iconPerson[i];
		iconPerson[i] = new TTexture();
		if ( !iconPerson[i]->LoadBinary( iconPersonName[i], iconPath, errStr, false ) )
			return false;
	};
	for ( i=0; i<7; i++ )
	{
		if ( iconWeapon[i]!=NULL )
			delete iconWeapon[i];
		iconWeapon[i] = new TTexture();
		if ( !iconWeapon[i]->LoadBinary( iconWeaponName[i], iconPath, errStr, false ) )
			return false;
	};
	for ( i=0; i<15; i++ )
	{
		if ( iconVehicle[i]!=NULL )
			delete iconVehicle[i];
		iconVehicle[i] = new TTexture();
		if ( !iconVehicle[i]->LoadBinary( iconVehicleName[i], iconPath, errStr, false ) )
			return false;
	};

	::SetProgress(20);

	WriteString( "loading landscape objects\n" );
	WriteString( "creating map & loading objects\n" );

	if ( landObj!=NULL )
		delete landObj;
	landObj = NULL;

	lastProgress = 20;
	progressSize = 60;

	TDefFile def;
	if ( !GetStoryDef( map, errStr, def ) )
	{
		return false;
	}

	landObj = CreateLandscapeObject( def, errStr, 0 );
	if ( landObj==NULL )
	{
		return false;
	}
	
	if ( !ResetObjects( errStr ) )
	{
		return false;
	}

	// calculate min and max render depths
/*
	float depth = float(landObj->DivX() + landObj->DivZ()) * 0.5f;
	MinRenderDepth( size_t( depth / 8.0f ) );
	if ( MinRenderDepth()<6 )
		MinRenderDepth( 6 );
	MaxRenderDepth( size_t( depth / 4.0f ) );
	if ( MaxRenderDepth()<10 )
		MaxRenderDepth( 10 );
	RenderDepth( landObj->RenderDepth());
*/
	::SetProgress(80);

	WriteString( "setting up landscape AI\n" );
	landscapeAI.SetupPaths( *landObj );

	::SetProgress(85);

	WriteString( "Loading explosion object\n" );
	if ( explosion!=NULL )
		delete explosion;
	explosion = new TBinObject();
	if (!explosion->LoadBinary( "explosion.bin", errStr, "data", "data\\textures" ) )
	{
		delete explosion;
		explosion = NULL;
		return false;
	}

//	WriteString( "Loading explosion circle jpeg\n" );
//	if ( explosionCircle!=NULL )
//		delete explosionCircle;
//	explosionCircle = new TTexture();
//	if (!explosionCircle->LoadRGBAJPEG( "explCircle.jpg", "data\\textures", errStr,
//										0,0,0, 0,100, 100 ) )
//	{
//		delete explosionCircle;
//		explosionCircle = NULL;
//		return false;
//	}

	WriteString( "Loading explosion flame jpegs (7)\n" );
	for ( i=0; i<kNumExplosionAnimations; i++ )
	{
		if ( explosionFlame[i]!=NULL )
			delete explosionFlame[i];

		explosionFlame[i] = new TTexture();
		TString fname;
		if ( i<10 )
		{
			fname = "expl" + Int2Str(i) + ".tga";
		}
		else
		{
			fname = "expl" + Int2Str(i) + ".tga";
		}
		if (!explosionFlame[i]->LoadBinary( fname, "data\\textures", errStr ) )
		{
			delete explosionFlame[i];
			explosionFlame[i] = NULL;
			return false;
		}
	}
	::SetProgress(95);

	// reset planes etc.
	for ( i=0; i<kMaxPlayers; i++ )
	{
		tanks[i].SetSoundSystem( soundSystem );
		tanks[i].SetApp( this );
		tanks[i].SetProjectile( grShell );

		projectiles[i].Explosion( soundSystem, 8, explosion, explosionFlame );

		projectiles[i].SetApp( this );
		
		planes[i].SetSoundSystem( soundSystem );
		planes[i].SetProjectile( grBomb );
		planes[i].SetApp( this );

		flak[i].SetSoundSystem( soundSystem );
		flak[i].SetProjectile( grShell );
		flak[i].SetApp( this );

		artillery[i].SetSoundSystem( soundSystem );
		artillery[i].SetProjectile( grShell );
		artillery[i].SetApp( this );

		mg42[i].SetSoundSystem( soundSystem );
		mg42[i].SetApp( this );

		v2[i].SetSoundSystem( soundSystem );

		// graphic and scale
		explosives[i].Explosion( weapons[3], 0.02f );
		explosives[i].SetApp( this );
		explosives[i].InUse( false );

		size_t vehicleId = characters[i].VehicleId();
		switch ( characters[i].VehicleType() )
		{
		case vtTank:
			{
				tanks[vehicleId] = characters[i];
				break;
			}
		case vtPlane:
			{
				planes[vehicleId] = characters[i];
				break;
			}
		case vtFlak:
			{
				flak[vehicleId] = characters[i];
				break;
			}
		case vtArtillery:
			{
				artillery[vehicleId] = characters[i];
				break;
			}
		case vtMG42:
			{
				mg42[vehicleId] = characters[i];
				break;
			}
		}
	}

	::SetProgress(100);
	return true;
};


bool TApp::ReloadGraphics( TString& errStr )
{
	size_t i;

	WriteString( "ReloadGraphics:\n" );

	if ( !ResetResolution(NULL,errStr) )
	{
		errStr = "\n" + errStr + "\n";
		WriteString( errStr.c_str() );
		return false;
	}

	::SetProgressMessage( STR(strLoadingObjects) );
	::SetProgress(0);

	WriteString( "loading background\n" );
	if ( mainMenuBackground!=NULL )
		delete mainMenuBackground;
	mainMenuBackground = new TTexture();
	if ( !mainMenuBackground->LoadBinary( "background01.jpg", "data\\backgrounds", errStr ) )
		return false;

	if ( !LoadAboutScreen( errStr ) )
		return false;

	WriteString( "loading in game text\n" );

	if ( title!=NULL )
		delete title;
	title = new TBinObject();
	if ( !title->LoadBinary( "3d_title.bin", errStr, path, texturePath ) )
		return false;

	if ( missionControl!=NULL )
		delete missionControl;
	missionControl = new TBinObject();
	if ( !missionControl->LoadBinary( "missioncontrol.bin", errStr, path, texturePath ) )
		return false;

	// set story binaries
	stories.SetBinaries( missionControl );

	if ( gameOverText!=NULL )
		delete gameOverText;
	gameOverText = new TBinObject();
	if ( !gameOverText->LoadBinary( "gameover.bin", errStr, path, texturePath ) )
		return false;

	if ( missionAccomplishedText!=NULL )
		delete missionAccomplishedText;
	missionAccomplishedText = new TBinObject();
	if ( !missionAccomplishedText->LoadBinary( "missionaccomplished.bin", errStr, path, texturePath ) )
		return false;

	if ( parachute!=NULL )
		delete parachute;
	parachute = new TBinObject();
	if ( !parachute->LoadBinary( "para.bin", errStr, "data", texturePath ) )
		return false;

	::SetProgress(10);

	// load weapons
	WriteString( "loading weapons\n" );
	for ( i=0; i<kMaxWeapons; i++ )
	{
		if ( weapons[i]!=NULL )
			delete weapons[i];
		weapons[i] = NULL;
	}
	weapons[0] = new TBinObject();
	if ( !weapons[0]->LoadBinary( "magnum.bin", errStr, path, texturePath ) )
		return false;
	weapons[1] = new TBinObject();
	if ( !weapons[1]->LoadBinary( "lasergun.bin", errStr, path, texturePath ) )
		return false;
	weapons[2] = new TBinObject();
	if ( !weapons[2]->LoadBinary( "bazooka.bin", errStr, path, texturePath ) )
		return false;
	weapons[3] = new TBinObject();
	if ( !weapons[3]->LoadBinary( "explosives.bin", errStr, path, texturePath ) )
		return false;
	numWeapons = 4;

	WriteString( "loading ammunition\n" );
	if ( bomb!=NULL )
		delete bomb;
	bomb = new TBinObject();
	if ( !bomb->LoadBinary( "bomb.bin", errStr, path, texturePath ) )
		return false;

	if ( shell!=NULL )
		delete shell;
	shell = new TBinObject();
	if ( !shell->LoadBinary( "shell.bin", errStr, path, texturePath ) )
		return false;

	for ( i=0; i<6; i++ )
	{
		if ( iconPerson[i]!=NULL )
			delete iconPerson[i];
		iconPerson[i] = new TTexture();
		if ( !iconPerson[i]->LoadBinary( iconPersonName[i], iconPath, errStr, false ) )
			return false;
	};
	for ( i=0; i<7; i++ )
	{
		if ( iconWeapon[i]!=NULL )
			delete iconWeapon[i];
		iconWeapon[i] = new TTexture();
		if ( !iconWeapon[i]->LoadBinary( iconWeaponName[i], iconPath, errStr, false ) )
			return false;
	};
	for ( i=0; i<15; i++ )
	{
		if ( iconVehicle[i]!=NULL )
			delete iconVehicle[i];
		iconVehicle[i] = new TTexture();
		if ( !iconVehicle[i]->LoadBinary( iconVehicleName[i], iconPath, errStr, false ) )
			return false;
	};

	if ( sky!=NULL )
		delete sky;
	if ( backgroundBitmap.length()>0 )
	{
		sky = new TBinObject();
		if (!sky->LoadBinary( backgroundBitmap, errStr, "data", "data\\textures" ) )
		{
			delete sky;
			sky = NULL;
			return false;
		}
	}
	else
	{
		sky = NULL;
	}

	::SetProgress(30);

	WriteString( "reloading characters\n" );
	for ( i=0; i<kMaxPlayers; i++ )
	{
		if ( multiplayerSlotInUse[i] )
		{
			if ( !characters[i].ReloadCharacter( errStr ) )
			{
				return false;
			}
			// reset other player graphic attributes
			characters[i].SetSoundSystem( soundSystem );
			characters[i].SetWeapons( numWeapons, weapons );
			characters[i].SetSounds( swalk, sgun, smachinegun, 
									 scanon, grShell, parachute );
		}
	};

	::SetProgress(50);

	WriteString( "Loading explosion object\n" );
	if ( explosion!=NULL )
		delete explosion;
	explosion = new TBinObject();
	if (!explosion->LoadBinary( "explosion.bin", errStr, "data", "data\\textures" ) )
	{
		delete explosion;
		explosion = NULL;
		return false;
	}

//	WriteString( "Loading explosion circle jpeg\n" );
//	if ( explosionCircle!=NULL )
//		delete explosionCircle;
//	explosionCircle = new TTexture();
//	if (!explosionCircle->LoadRGBAJPEG( "explCircle.jpg", "data\\textures", errStr,
//										0,0,0, 0,100, 100 ) )
//	{
//		delete explosionCircle;
//		explosionCircle = NULL;
//		return false;
//	}

	WriteString( "Loading explosion flame jpegs (7)\n" );
	for ( i=0; i<kNumExplosionAnimations; i++ )
	{
		if ( explosionFlame[i]!=NULL )
			delete explosionFlame[i];

		explosionFlame[i] = new TTexture();
		TString fname;
		if ( i<10 )
		{
			fname = "expl" + Int2Str(i) + ".tga";
		}
		else
		{
			fname = "expl" + Int2Str(i) + ".tga";
		}
		if (!explosionFlame[i]->LoadBinary( fname, "data\\textures", errStr ) )
		{
			delete explosionFlame[i];
			explosionFlame[i] = NULL;
			return false;
		}
	}

	::SetProgress(60);

	// reload landObj's textures
	if ( !landObj->Reload(errStr) )
	{
		return false;
	}

	// reload complete object cache for landscape objects
	lastProgress = 60;
	progressSize = 35;

	for ( i=0; i<numObjects; i++ )
	{
		PreCond( objects[i]!=NULL );
		if ( !objects[i]->Reload(errStr) )
		{
			return false;
		}

		float perc = float(i) / float(numObjects);
		size_t progress = lastProgress + size_t(float(progressSize)*perc);
		::SetProgress( progress );
	}

	::SetProgress(95);

	// reset planes etc graphics only.
	for ( i=0; i<kMaxPlayers; i++ )
	{
		tanks[i].SetSoundSystem( soundSystem );
		tanks[i].SetApp( this );
		tanks[i].SetProjectile( grShell );

		projectiles[i].Explosion( soundSystem, 8, explosion, explosionFlame );
		projectiles[i].SetApp( this );

		if ( projectiles[i].InUse() && projectiles[i].GraphicId()!=-1 )
		{
			projectiles[i].Graphic( ProjectileNameToBin(projectiles[i].GraphicId()) );
		}
		else
		{
			projectiles[i].Graphic( NULL );
		};

		tanks[i].SetProjectile( grShell );
		
		planes[i].SetSoundSystem( soundSystem );
		planes[i].SetProjectile( grBomb );
		planes[i].SetApp( this );

		flak[i].SetSoundSystem( soundSystem );
		flak[i].SetProjectile( grShell );
		flak[i].SetApp( this );

		artillery[i].SetSoundSystem( soundSystem );
		artillery[i].SetProjectile( grShell );
		artillery[i].SetApp( this );

		mg42[i].SetSoundSystem( soundSystem );
		mg42[i].SetApp( this );

		v2[i].SetSoundSystem( soundSystem );

		// graphic and scale
		explosives[i].Explosion( weapons[3], 0.02f );
		explosives[i].SetApp( this );
	}

	::SetProgress(100);
	return true;
};


void TApp::Destroy( void )
{
};


bool TApp::StartupSystem( TString& errStr )
{
	WriteString( "StartupSystem\n" );
	if ( !TEvent::StartupSystem( errStr ) )
	{
		return false;
	}

	// this will setup the window
	if ( !ReloadMenu( errStr ) )
		return false;

	::SetProgress(50);

	if ( playEffects )
	{
		soundSystem = new TSound( hWindow );
		if ( soundSystem->Initialised() )
		{
			WriteString( "loading sound samples\n" );
			soundSystem->Load( "data\\audio\\engine1.wav", true, errStr );
			soundSystem->Load( "data\\audio\\engine2.wav", true, errStr );
			soundSystem->Load( "data\\audio\\gun2.wav", false, errStr );
			soundSystem->Load( "data\\audio\\gunshot.wav", false, errStr );
			soundSystem->Load( "data\\audio\\footstep.wav", true, errStr );
			soundSystem->Load( "data\\audio\\machineGun.wav", true, errStr );
			soundSystem->Load( "data\\audio\\propellor.wav", true, errStr );
			soundSystem->Load( "data\\audio\\jetEngine.wav", true, errStr );
			soundSystem->Load( "data\\audio\\explode2.wav", false, errStr );
			soundSystem->Load( "data\\audio\\mg42.wav", true, errStr );

			::SetProgress(80);

			soundSystem->Load( "data\\audio\\daytime.mp3", true, errStr );
			soundSystem->Load( "data\\audio\\nighttime.mp3", true, errStr );
			soundSystem->Load( "data\\audio\\fog1.mp3", true, errStr );

			soundSystem->Volume( effectsVolume );
			soundSystem->StreamVolume( musicVolume );
		}
		else
		{
			WriteString( "\nWarning: sound system failed\n" );
			delete soundSystem;
			soundSystem = NULL;
		}
	}
	else
	{
		soundSystem = NULL;
	}

	::SetProgress(100);

	WriteString( "StarupSystem done\n\n" );

    return true;
};


void TApp::MouseDown( int button, int x, int y)
{
	TEvent::MouseDown(button,x,y);

	if ( button==1 )
    {
		KeyDown( VK_LBUTTON );
    	leftDown = true;
    }
	if ( button==2 )
    {
		KeyDown( VK_RBUTTON );
    	rightDown = true;
    }
};


void TApp::MouseDown( int button, int x, int y, int dx, int dy )
{
	TEvent::MouseDown(button,x,y,dx,dy);
	if ( button==1 )
	{
		leftButtonDown = true;
	}
};


void TApp::MouseUp( int button, int x, int y, int dx, int dy )
{
	TEvent::MouseUp(button,x,y,dx,dy);
	if ( button==1 )
	{
		leftButtonDown = false;
	}
};


void TApp::MouseMove( int x, int y, int dx, int dy )
{
	TEvent::MouseMove(x,y,dx,dy);
};


void TApp::MouseUp( int button, int x, int y )
{
	TEvent::MouseUp(button,x,y);

	if ( button==1 )
    {
		KeyUp( VK_LBUTTON );
    	leftDown = false;
    }
	if ( button==2 )
    {
		KeyUp( VK_RBUTTON );
    	rightDown = false;
    }
};



void TApp::MouseMove( int x, int y )
{
	TEvent::MouseMove(x,y);

	if ( userKeys.reverseMouse )
	{
		deltaYAngle -= float(x)*0.1f;
	}
	else
	{
		deltaYAngle += float(x)*0.1f;
	}
	deltaXAngle += float(y)*0.1f;
};



void TApp::KeyDown( size_t kd )
{
	TEvent::KeyDown( kd );

	if ( kd==userKeys.up )
	{
		keys = keys | aUp;
	}
	else if ( kd==userKeys.down )
	{
		keys = keys | aDown;
	}
	else if ( kd==userKeys.left )
	{
		keys = keys | aLeft;
	}
	else if ( kd==userKeys.right )
	{
		keys = keys | aRight;
	}
	else if ( kd==userKeys.jump )
	{
		keys = keys | aJump;
	}
	else if ( kd==userKeys.leftMouse || leftButtonDown )
	{
		keys = keys | aFire;
	}
	else if ( kd==VK_ESCAPE )
	{
		if ( !gameOver && !missionAccomplished )
		{
			animCount = 0;
		}
		if ( showMenu )
		{
			if ( MainMenu() )
			{
				if ( soundSystem!=NULL )
				{
					soundSystem->ResumeStream();
				}
				SetupFog( fog );
				showMenu = false;
			}
		}
		else
		{
			// stop and start music according to settings
			if ( soundSystem!=NULL )
			{
				soundSystem->PauseStream();
				StopSounds();
			}
			SetupFog( false );
			showMenu = true;
		}
	}
	else if ( kd==userKeys.scores )
	{
		ShowScores( !ShowScores() );
		if ( ShowScores() )
			ShowTimeout( ::GetTickCount() + 3000 );
	}
	else if ( kd==userKeys.map )
	{
		ShowMap( !ShowMap() );
	}
	else if ( kd==VK_F1 )
	{
		cameraType = 0;
		cam.Type( cameraType );
	}
	else if ( kd==VK_F2 )
	{
		cameraType = 1;
		cam.Type( cameraType );
	}
	else if ( kd==VK_F3 )
	{
		cameraType = 2;
		cam.Type( cameraType );
	}
	else if ( kd==VK_F4 )
	{
		cameraType = 3;
		cam.Type( cameraType );
	}
	else if ( kd==VK_F5 )
	{
		cameraType = 4;
		cam.Type( cameraType );
	}
	else if ( kd==VK_F6 )
	{
		cameraType = 5;
		cam.Type( cameraType );
	}
	else if ( kd==VK_F7 )
	{
		cameraType = 6;
		cam.Type( cameraType );
	}
	else if ( kd==VK_F8 )
	{
		keys = keys | aExplode;
	}
	else if ( kd>='0' && kd<='4' )
	{
		size_t gunId = kd - '0';

		// always zero - the local player is always player 0
		// and s/he only receives keyboard input
		TCharacter& data = characters[myId];
		switch ( data.VehicleType() )
		{
		case vtNone:
			{
				data.SelectedGun( gunId-1 );
				break;
			}
		case vtPlane:
			{
				if ( gunId>0 && gunId<4 )
				{
					TPlane& plane = planes[data.VehicleId()];
					plane.SelectedGun( gunId-1 );
				}
				break;
			}
		}
	}
};



void TApp::KeyUp( size_t ku )
{
	TEvent::KeyUp( ku );

//	if ( ku==VK_SHIFT )
//	{
//		showTarget = false;
//	}

	if ( ku==userKeys.up )
	{
		keys = keys & ~aUp;
	}
	else if ( ku==userKeys.down )
	{
		keys = keys & ~aDown;
	}
	else if ( ku==userKeys.left )
	{
		keys = keys & ~aLeft;
	}
	else if ( ku==userKeys.right )
	{
		keys = keys & ~aRight;
	}
	else if ( ku==userKeys.jump )
	{
		keys = keys & ~aJump;
	}
	else if ( ku==userKeys.leftMouse )
	{
		keys = keys & ~aFire;
	}
	else if ( !leftButtonDown && ku!=userKeys.leftMouse )
	{
		keys = keys & ~aFire;
	}
};

// try and read resolution settings from registry
bool TApp::GetRegistrySettings( void )
{
	TEvent::GetRegistrySettings();

	size_t val;
	lazy = false;
	if ( GetRegistryKey( "Software\\PDV\\Performance", "Lazy", val ) )
	{
		if ( val==1 )
			lazy = true;
	}
	
	// get user's keys
	userKeys.LoadFromRegistry();

	return true;
};

bool TApp::ShowScores( void ) const
{
	return showScores;
};

void TApp::ShowScores( bool sc )
{
	showScores = sc;
};

bool TApp::ShowMap( void ) const
{
	return showMap;
};

void TApp::ShowMap( bool sm )
{
	showMap = sm;
};

void TApp::ShowTimeout( size_t t )
{
	showTimeout = t;
};

size_t TApp::ShowTimeout( void ) const
{
	return showTimeout;
};

void TApp::MapTimeout( size_t t )
{
	mapTimeout = t;
};

size_t TApp::MapTimeout( void ) const
{
	return mapTimeout;
};

void TApp::KeyPress( size_t key )
{
	TEvent::KeyPress( key );
};


size_t TApp::NumObjects( void ) const
{
	return numObjects;
};


TString TApp::GetObjectName( size_t index ) const
{
	PreCond( index<numObjects );
	return objectNames[index];
};


TCompoundObject* TApp::GetObjectByName( const TString& name, 
									    size_t objectIndex, 
										size_t numLandscapeObjects )
{
	// update progress
	if ( numObjects>0 && objectIndex>0 )
	{
		float perc = float(objectIndex) / float(numLandscapeObjects);
		size_t progress = lastProgress + size_t(float(progressSize)*perc);
		::SetProgress( progress );
	}

	for ( size_t i=0; i<numObjects; i++ )
	{
		if ( objectNames[i]==name )
			return objects[i];
	}

	// not yet in cache - load the object seperate
	if ( objects[numObjects]!=NULL )
	{
		delete objects[numObjects];
	}
	objects[numObjects] = new TCompoundObject();
	TString errStr;
	if ( objects[numObjects]->LoadBinary( name, errStr, path, texturePath ) )
	{
		objectNames[numObjects] = name;
		numObjects++;
	}
	else
	{
		WriteString( "\nFatal error loading object %s (%s)\n", name.c_str(), errStr.c_str() );
		return NULL;
	}
	return objects[numObjects-1];
};


bool TApp::AddTransport( TLandscapeObject& v2obj, TLandscapeObject& target )
{
	TVehicleSettings* set = vehicles.Find( v2obj.Name() );
	if ( set!=NULL )
	{
		if ( set->IsV2() )
		{
			if ( numV2 < kMaxTanks )
			{
				v2[numV2].Init();
				v2[numV2] = *set;
				v2[numV2] = v2obj;
				v2[numV2].SetTarget( target.TX(),
									 target.TY(),
									 target.TZ() );
				v2[numV2].AIID( numV2 );
				numV2++;
				return true;
			}
		}
	}
	return false;
};


bool TApp::AddTransport( TLandscapeObject& obj )
{
	TVehicleSettings* set = vehicles.Find( obj.Name() );
	if ( set!=NULL )
	{
		if ( set->IsTank() )
		{
			if ( numTanks < kMaxTanks )
			{
				tanks[numTanks] = *set;
				tanks[numTanks] = obj;
				landObj->GetLORTriangle( tanks[numTanks] );
				tanks[numTanks].CommitMove();

				if ( tanks[numTanks].IsaAI() )
				{
					aitanks[numAITanks].SetVehicle( tanks[numTanks] );
					aitanks[numAITanks] = obj;
					tanks[numTanks].AIID( numAITanks );
					numAITanks++;
				}

				// see if there is an object with the same name
				bool found = false;
				for ( size_t i=0; i<numTanks && !found; i++ )
				{
					if ( tanks[i].Name()==tanks[numTanks].Name() )
					{
						found = true;
						float tx,ty,tz, bx,by,bz;
						tanks[i].GetTurretBarrel( tx,ty,tz, bx,by,bz );
						tanks[numTanks].SetTurretBarrel( tx,ty,tz, bx,by,bz );
					}
				}
				numTanks++;
			}
		}
		if ( set->IsPlane() )
		{
			if ( numPlanes < kMaxTanks )
			{
				planes[numPlanes] = *set;
				planes[numPlanes] = obj;

				if ( planes[numPlanes].IsaAI() )
				{
					aiplanes[numAIPlanes].SetVehicle( planes[numPlanes] );
					aiplanes[numAIPlanes] = obj;
					planes[numPlanes].AIID( numAIPlanes );
					numAIPlanes++;
				}

				// see if there is an object with the same name
				bool found = false;
				for ( size_t i=0; i<numPlanes && !found; i++ )
				{
					if ( planes[i].Name()==planes[numPlanes].Name() )
					{
						found = true;
						float tx,ty,tz, px,py,pz;
						planes[i].GetTailProp( tx,ty,tz, px,py,pz );
						planes[numPlanes].SetTailProp( tx,ty,tz, px,py,pz );
					}
				}
				numPlanes++;
			}
		}
		if ( set->IsFlak() )
		{
			if ( numFlak < kMaxTanks )
			{
				flak[numFlak] = *set;
				flak[numFlak] = obj;

				if ( flak[numFlak].IsaAI() )
				{
					aiflak[numAIFlak].SetVehicle( flak[numFlak] );
					aiflak[numAIFlak] = obj;
					flak[numFlak].AIID( numAIFlak );
					numAIFlak++;
				}
				numFlak++;
			}
		}
		if ( set->IsArtillery() )
		{
			if ( numArtillery < kMaxTanks )
			{
				artillery[numArtillery] = *set;
				artillery[numArtillery] = obj;

				if ( artillery[numArtillery].IsaAI() )
				{
					// fixed character for now #2 - is last one
					TString errStr;
					TCharacter* char1 = new TCharacter();
					if ( !char1->Load( GetCharacterFilename(mg42Character), errStr ) )
					{
						WriteString( errStr.c_str() );
						PreCond( "AddTransport: load character for artillery failed"==NULL );
					}
					char1->SetWeapons( numWeapons, weapons );
					char1->SetSounds( swalk, sgun, smachinegun, 
									 scanon, grShell, parachute );
					char1->SetApp( this );
					char1->SetSoundSystem( soundSystem );
					char1->Reset();
					*char1 = obj;
					char1->SetupForMG42();

					aiartillery[numAIArtillery].SetVehicle( artillery[numArtillery] );
					artillery[numAIArtillery].SetSoldier( char1 );
					aiartillery[numAIArtillery] = obj;
					artillery[numArtillery].AIID( numAIArtillery );
					artillery[numArtillery].CharacterOwner( true );
					numAIArtillery++;
					operatorId++;
				}
				numArtillery++;
			}
		}
		if ( set->IsMG42() )
		{
			if ( numMG42s < kMaxTanks )
			{
				mg42[numMG42s] = *set;
				mg42[numMG42s] = obj;

				if ( mg42[numMG42s].IsaAI() )
				{
					// fixed character for now #2 - is last one
					TString errStr;
					TCharacter* char1 = new TCharacter();
					if ( !char1->Load( GetCharacterFilename(mg42Character), errStr ) )
					{
						WriteString( errStr.c_str() );
						PreCond( "AddTransport: load character for artillery failed"==NULL );
					}
					char1->SetWeapons( numWeapons, weapons );
					char1->SetSounds( swalk, sgun, smachinegun, 
									  scanon, grShell, parachute );
					char1->SetApp( this );
					char1->SetSoundSystem( soundSystem );
					char1->Reset();
					*char1 = obj;
					char1->SetupForMG42();

					aimg42[numAIMG42].SetVehicle( mg42[numMG42s] );
					mg42[numMG42s].SetSoldier( char1 );
					mg42[numMG42s].CharacterOwner( true );
					aimg42[numAIMG42] = obj;
					mg42[numMG42s].AIID( numAIMG42 );
					numAIMG42++;
				}
				numMG42s++;
			}
		}
		return true;
	}
	return false;
};


TBinObject* TApp::ProjectileNameToBin( size_t graphicName ) const
{
	TBinObject* projectileGraphic = NULL;
	switch ( graphicName )
	{
	case grShell:
	{
		projectileGraphic = shell;
		break;
	};
	case grBomb:
	{
		projectileGraphic = bomb;
		break;
	};
	}
	return projectileGraphic;
};

size_t TApp::ProjectileBinToName( TBinObject* graphic ) const
{
	if ( graphic==shell )
		return grShell;
	else if ( graphic==bomb )
		return grBomb;
	return grNone;
};

// fire a projectile along dirnMatrix, for a certain range, at a certain
// speed, causing damage on impact.  If (speed==0) -> instantaneous hit
TProjectile* TApp::FireProjectile( TPoint& start, TPoint& dirn,
								   size_t graphicName, size_t range, 
								   float damage )
{
	// projectile has speed - so it must travel
	// and it must be drawn - enter it into the
	// list of objects under that heading

	// find first not in use if possible
	bool found = false;
	size_t index = 0;
	for ( size_t i=0; i<kMaxPlayers && !found; i++ )
	{
		if ( !projectiles[i].InUse() )
		{
			found = true;
			index = i;
		}
	}

	if ( found )
	{
		projectiles[index].Range( range );
		projectiles[index].Damage( damage );
		projectiles[index].Graphic( ProjectileNameToBin(graphicName) );
		projectiles[index].GraphicId( graphicName );
		projectiles[index].StartPoint( start );
		projectiles[index].Dirn( dirn, cam.CameraAngle() );
		projectiles[index].Start();

		// do it over the network too
		if ( serverGame || clientGame )
		{
			NetworkFireProjectile( projectiles[index] );
		}
		return &projectiles[index];
	}
	return NULL;
};


TProjectile* TApp::FireProjectile( const byte* buf )
{
	PreCond( buf!=NULL );
	PreCond( buf[0]==id_projectile );

	// find first not in use if possible
	bool found = false;
	size_t index = 0;
	for ( size_t i=0; i<kMaxPlayers && !found; i++ )
	{
		if ( !projectiles[i].InUse() )
		{
			found = true;
			index = i;
		}
	}

	if ( found )
	{
		projectiles[index].netGet( buf );
		projectiles[index].Start();
		return &projectiles[index];
	}
	return NULL;
};


// special case of fire for MDL characters, immediate fire
void TApp::Fire( TPoint& start, TPoint& dirn, size_t range, size_t damage )
{
	TPoint prev = start;
	bool firstTime = true;
	while ( range>0 )
	{
		// check whether we hit something
		if ( !firstTime )
		{
			size_t objId;
			landObj->ObjCollision( prev, start, objId );
			if ( objId>0 )
			{
				range = 0;
				landObj->CalculateExplosionDamage( this, objId-1, damage );
			}
			else
			{
				float tempY = start.y;
				landObj->GetY( start.x,tempY,start.z);
				if ( tempY > start.y )
				{
					range = 0;
				}
				else
				{
					size_t i;

					for ( i=0; i<kMaxPlayers; i++ )
					{
						// check if we get hit by machine gun fire
						if ( characters[i].VehicleType()==vtNone )
						if ( characters[i].InsideVehicle(start) )
						{
							range = 0;
							if ( characters[i].Strength() > damage )
								characters[i].Strength( characters[i].Strength() - damage );
							else
								characters[i].Strength(0);
						}
					}

					// check hit any objects
					for ( i=0; i<numPlanes; i++ )
					{
						if ( planes[i].Strength()>0 )
						{
							if ( planes[i].InsideVehicle( start ) )
							{
								range = 0;
								if ( planes[i].Strength() > damage )
									planes[i].Strength( planes[i].Strength() - damage );
								else
									planes[i].Strength(0);
							}
						}
					}
					// check hit any objects
					for ( i=0; i<numTanks; i++ )
					{
						if ( tanks[i].Strength()>0 )
						{
							if ( tanks[i].InsideVehicle( start ) )
							{
								range = 0;
								if ( tanks[i].Strength() > damage )
									tanks[i].Strength( tanks[i].Strength() - damage );
								else
									tanks[i].Strength(0);
							}
						}
					}
					// check hit any objects
					for ( i=0; i<numFlak; i++ )
					{
						if ( flak[i].Strength()>0 )
						{
							if ( flak[i].InsideVehicle( start ) )
							{
								range = 0;
								if ( flak[i].Strength() > damage )
									flak[i].Strength( flak[i].Strength() - damage );
								else
									flak[i].Strength(0);
							}
						}
					}
					// check hit any objects
					for ( i=0; i<numArtillery; i++ )
					{
						if ( artillery[i].Strength()>0 )
						{
							if ( artillery[i].InsideVehicle( start ) )
							{
								range = 0;
								if ( artillery[i].Strength() > damage )
									artillery[i].Strength( artillery[i].Strength() - damage );
								else
									artillery[i].Strength(0);
							}
						}
					}
					// check hit any objects
					for ( i=0; i<numMG42s; i++ )
					{
						if ( mg42[i].Strength()>0 )
						{
							if ( mg42[i].InsideVehicle( start ) )
							{
								range = 0;
								if ( mg42[i].Strength() > damage )
									mg42[i].Strength( mg42[i].Strength() - damage );
								else
									mg42[i].Strength(0);
							}
						}
					}
					// check hit any objects
					for ( i=0; i<numV2; i++ )
					{
						if ( v2[i].Strength()>0 )
						{
							if ( v2[i].InsideVehicle( start ) )
							{
								range = 0;
								if ( v2[i].Strength() > damage )
									v2[i].Strength( mg42[i].Strength() - damage );
								else
									v2[i].Strength(0);
							}
						}
					}


				}
			}
		}
		prev = start;
		start = start + dirn;
		if ( range>0 )
			range--;
		firstTime = false;
	}
}


// calculate damage for all involved, set explosions,
// where necessairy
void TApp::ExplodeAt( const TPoint& point, size_t explosionType, float damage )
{
	// find first not in use if possible
	bool found = false;
	size_t index = 0;
	for ( size_t i=0; i<kMaxPlayers && !found; i++ )
	{
		if ( !projectiles[i].InUse() )
		{
			found = true;
			index = i;
		}
	}
	projectiles[index].StartPoint( point );
	projectiles[index].Range( 0 );
	projectiles[index].Scale( 0 );
	projectiles[index].Damage( damage );
	projectiles[index].Graphic( NULL );
	projectiles[index].GraphicId( -1 );
	projectiles[index].Start();
	projectiles[index].Explode( true, explosionType ); // must be last in setup!
};


void TApp::CalculateExplosionDamage( const TPoint& point,
									 float damageFactor )
{
	size_t i;
	for ( i=0; i<numPlanes; i++ )
	{
		if ( planes[i].Strength()>0 )
			planes[i].CalculateDamage( point, damageFactor );
	}
	for ( i=0; i<numFlak; i++ )
	{
		if ( flak[i].Strength()>0 )
			flak[i].CalculateDamage( point, damageFactor );
	}
	for ( i=0; i<numArtillery; i++ )
	{
		if ( artillery[i].Strength()>0 )
			artillery[i].CalculateDamage( point, damageFactor );
	}
	for ( i=0; i<kMaxPlayers; i++ )
	{
		explosives[i].CalculateDamage( point, damageFactor );
	}
	for ( i=0; i<numTanks; i++ )
	{
		if ( tanks[i].Strength()>0 )
			tanks[i].CalculateDamage( point, damageFactor );
	}
	for ( i=0; i<numMG42s; i++ )
	{
		if ( mg42[i].Strength()>0 )
			mg42[i].CalculateDamage( point, damageFactor );
	}
	for ( i=0; i<numV2; i++ )
	{
		if ( v2[i].Strength()>0 )
			v2[i].CalculateDamage( point, damageFactor );
	}
	landObj->CalculateExplosionDamage( this, point, damageFactor );


	for ( i=0; i<kMaxPlayers; i++ )
	if ( multiplayerSlotInUse[i] )
	{
		size_t vehicleId = characters[i].VehicleId();
		switch ( characters[i].VehicleType() )
		{
		case vtNone:
			{
				if ( characters[i].Strength()>0 )
					characters[i].CalculateDamage( point, damageFactor );
				break;
			}
		case vtTank:
			{
				if ( !tanks[vehicleId].CanEnter() )
				{
					characters[i].Strength(0);
				}
				break;
			}
		case vtPlane:
			{
				if ( planes[vehicleId].Strength()==0 )
				{
					characters[i].Strength(0);
				}
				break;
			}
		case vtFlak:
			{
				if ( flak[vehicleId].Strength()==0 )
				{
					characters[i].Strength(0);
				}
				break;
			}
		case vtArtillery:
			{
				if ( artillery[vehicleId].Strength()==0 )
				{
					characters[i].Strength(0);
				}
				break;
			}
		case vtMG42:
			{
				if ( mg42[vehicleId].Strength()==0 )
				{
					characters[i].Strength(0);
				}
				break;
			}
		}
	}
};


void TApp::ResetGameObjects( void )
{
	numGameObjects = 0;
	if ( gameObjects!=NULL )
	{
		delete []gameObjects;
	};
	gameObjects = NULL;
};


void TApp::AddGameObject( TLandscapeObject* obj )
{
	PreCond( numGameObjects<kMaxGameObjects );
	if ( gameObjects==NULL )
	{
		gameObjects = new TLandscapeObject*[kMaxGameObjects];
	}
	gameObjects[numGameObjects++] = obj;
};


void TApp::GameStatus( bool& gameOver, bool& missionAccomplished ) const
{
	size_t i;
	gameOver = false;
	missionAccomplished = true;
	for ( i=0; i<numGameObjects; i++ )
	{
		if ( gameObjects[i]->GameImportance()==1 ) // 1=must be destroyed
		{
			if ( gameObjects[i]->Strength()>0 )
			{
				missionAccomplished = false;
			}
		}
		else if ( gameObjects[i]->GameImportance()==2 ) // 2=must not be destroyed
		{
			if ( gameObjects[i]->Strength()==0 )
			{
				gameOver = true;
			}
		}
	}
	for ( i=0; i<numTanks; i++ )
	{
		if ( tanks[i].GameImportance()==1 ) // 1=must be destroyed
		{
			if ( tanks[i].Strength()>0 )
			{
				missionAccomplished = false;
			}
		}
		else if ( tanks[i].GameImportance()==2 ) // 2=must not be destroyed
		{
			if ( tanks[i].Strength()==0 )
			{
				gameOver = true;
			}
		}
	}
	for ( i=0; i<numPlanes; i++ )
	{
		if ( planes[i].GameImportance()==1 ) // 1=must be destroyed
		{
			if ( planes[i].Strength()>0 )
			{
				missionAccomplished = false;
			}
		}
		else if ( planes[i].GameImportance()==2 ) // 2=must not be destroyed
		{
			if ( planes[i].Strength()==0 )
			{
				gameOver = true;
			}
		}
	}
	for ( i=0; i<numFlak; i++ )
	{
		if ( flak[i].GameImportance()==1 ) // 1=must be destroyed
		{
			if ( flak[i].Strength()>0 )
			{
				missionAccomplished = false;
			}
		}
		else if ( flak[i].GameImportance()==2 ) // 2=must not be destroyed
		{
			if ( flak[i].Strength()==0 )
			{
				gameOver = true;
			}
		}
	}
	for ( i=0; i<numArtillery; i++ )
	{
		if ( artillery[i].GameImportance()==1 ) // 1=must be destroyed
		{
			if ( artillery[i].Strength()>0 )
			{
				missionAccomplished = false;
			}
		}
		else if ( artillery[i].GameImportance()==2 ) // 2=must not be destroyed
		{
			if ( artillery[i].Strength()==0 )
			{
				gameOver = true;
			}
		}
	}
	for ( i=0; i<numMG42s; i++ )
	{
		if ( mg42[i].GameImportance()==1 ) // 1=must be destroyed
		{
			if ( mg42[i].Strength()>0 )
			{
				missionAccomplished = false;
			}
		}
		else if ( mg42[i].GameImportance()==2 ) // 2=must not be destroyed
		{
			if ( mg42[i].Strength()==0 )
			{
				gameOver = true;
			}
		}
	}
	for ( i=0; i<numV2; i++ )
	{
		if ( v2[i].GameImportance()==1 ) // 1=must be destroyed
		{
			if ( v2[i].Strength()>0 )
			{
				missionAccomplished = false;
			}
		}
		else if ( v2[i].GameImportance()==2 ) // 2=must not be destroyed
		{
			if ( v2[i].Strength()==0 )
			{
				gameOver = true;
			}
		}
	}

	// character 0 is me - if I die -  its game over for me
	if ( characters[myId].Strength()==0 )
	{
		gameOver = true;
	}

	// check missionaccomplished against sequences and multi-players
	if ( missionAccomplished )
	{
		for ( i=0; i<numSequenceLocations && missionAccomplished; i++ )
		{
			if ( !sequenceLocations[i].visited )
			{
				missionAccomplished = false;
			}
		}

		// all other teams must be destroyed before your mission is
		// really accomplished in a multiplayer game
		if ( clientGame || serverGame )
		{
			for ( i=0; i<kMaxPlayers; i++ )
			{
				if ( i!=myId )
				if ( multiplayerPlaying[i] )
				{
					if ( characters[i].Team!=characters[myId].Team() &&
						 characters[i].Strength() > 0 )
					{
						missionAccomplished = false;
					}
				}
			}
		}
	}
};

TPlane* TApp::Planes( void )
{
	return planes;
};

size_t TApp::NumPlanes( void )
{
	return numPlanes;
};

TTank* TApp::Tanks( void )
{
	return tanks;
};

size_t TApp::NumTanks( void )
{
	return numTanks;
};

TFlak* TApp::Flak( void )
{
	return flak;
};

size_t TApp::NumFlak( void )
{
	return numFlak;
};

size_t TApp::NumArtillery( void )
{
	return numArtillery;
};

TArtillery* TApp::Artillery( void )
{
	return artillery;
};

size_t TApp::NumMG42s( void )
{
	return numMG42s;
};

TMG42* TApp::MG42s( void )
{
	return mg42;
};

AITank* TApp::AITanks( void ) 
{
	return aitanks;
};

AIPlane* TApp::AIPlanes( void ) 
{
	return aiplanes;
};

AIFlak* TApp::AIFlaks( void )
{
	return aiflak;
};

AIArtillery* TApp::AIArtilleries( void )
{
	return aiartillery;
};

AIMG42* TApp::AIMG42s( void )
{
	return aimg42;
};

void TApp::DropExplosive( float x, float y, float z, size_t timer )
{
	bool found = false;
	for ( size_t i=0; i<kMaxPlayers && !found; i++ )
	{
		if ( explosives[i].Ready() )
		{
			found = true;
			explosives[i].SetXYZ( x,y,z );
			explosives[i].StartTimer( timer );

			// do it over the network too
			if ( serverGame || clientGame )
			{
				NetworkDropExplosive( explosives[i] );
			}
		}
	}
};

TExplosive* TApp::DropExplosive( const byte* buf )
{
	PreCond( buf!=NULL );
	PreCond( buf[0]==id_explosive );

	// find first not in use if possible
	bool found = false;
	size_t index = 0;
	for ( size_t i=0; i<kMaxPlayers && !found; i++ )
	{
		if ( explosives[i].Ready() )
		{
			found = true;
			index = i;
		}
	}

	if ( found )
	{
		explosives[index].netGet( buf );
		explosives[index].StartTimer();
		return &explosives[index];
	}
	return NULL;
};


void TApp::StopMusic( void )
{
	// stop any playing sound
	if ( soundSystem!=NULL )
	{
		soundSystem->StopStream();
		soundSystem->CloseStream();
	}
};

void TApp::StopSounds( void )
{
	// stop all sounds
	size_t i;
	for ( i=0; i<numTanks; i++ )
	{
		tanks[i].StopSounds();
	}
	for ( i=0; i<numPlanes; i++ )
	{
		planes[i].StopSounds();
	}
	for ( i=0; i<numFlak; i++ )
	{
		flak[i].StopSounds();
	}
	for ( i=0; i<numArtillery; i++ )
	{
		artillery[i].StopSounds();
	}
	for ( i=0; i<numMG42s; i++ )
	{
		mg42[i].StopSounds();
	}
	for ( i=0; i<numV2; i++ )
	{
		v2[i].StopSounds();
	}
	for ( i=0; i<kMaxPlayers; i++ )
	{
		if ( multiplayerSlotInUse[i] )
		{
			characters[i].StopSounds();
		}
	}

	// background sound
	if ( backgroundSoundPlaying )
	{
		if ( soundSystem!=NULL )
		{
			soundSystem->Stop( backgroundSoundChannel );
		}
		backgroundSoundPlaying = false;
	}
};

void TApp::StartMusic( size_t track )
{
	StopMusic();

	TString fname;
	if ( track>0 )
	{
		TString title;
		soundTracks.Get( track-1, fname, title );
		fname = "data\\music\\" + fname;
	}
	if ( (track>0) && playMusic )
	{
		if ( soundSystem!=NULL )
		if ( soundSystem->OpenStream( fname ) )
		{
			soundSystem->PlayStream();
		}
	}
};

TLandscape* TApp::LandObj( void )
{
	return landObj;
};

//==========================================================================

