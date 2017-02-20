#include <precomp_header.h>

#include <windows.h>

#include <win32/events.h>

#include <systemDialogs/dialogAssist.h>
#include <dialogs/SinglePlayerDialog.h>
//#include <dialogs/customiseVehicle.h>

#include <tank/tankapp.h>

#include <win32/story.h>
#include <object/character.h>

#include <win32/resource.h>

//==========================================================================

SinglePlayerDialog::SinglePlayerDialog( TApp* _app, 
									    HINSTANCE _hInstance, 
									    HWND _parent,
										TStories* _stories )
	: hInstance( _hInstance ),
	  parent( _parent ),
	  app( _app ),
	  hostPage( NULL ),
	  stories( _stories )
{
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
	selectedCharacter = -1;
	selectedMap = -1;
	prevSelectedMap = -1;
	initialised = false;

	yAngle = 0;
	singlePlayerDialogDepth = -98;
};

SinglePlayerDialog::~SinglePlayerDialog( void )
{
	if ( hostPage!=NULL )
		delete hostPage;
	hostPage = NULL;
};


bool SinglePlayerDialog::Initialised( void ) const
{
	return initialised;
};


void SinglePlayerDialog::Initialised( bool i )
{
	initialised = i;
};


bool SinglePlayerDialog::Execute( TGameParameters& hp )
{
	size_t i;

	if ( hostPage!=NULL )
		delete hostPage;
	hostPage = NULL;

	GetRegistrySettings();

	AddItem( hostPage, new TGUI( app, guiWindow, 0, 0, 383, 190, singlePlayerDialogDepth, STR(strSinglePlayer) ) );

	AddItem( hostPage, new TGUI( app, guiLabel, 26,20,27,14, singlePlayerDialogDepth, STR(strName), 255,255,255 ) );
	AddItem( hostPage, new TGUI( app, guiLabel, 12,40,26,14, singlePlayerDialogDepth, STR(strCharacter), 255,255,255 ) );
	AddItem( hostPage, new TGUI( app, guiLabel, 20,60,18,14, singlePlayerDialogDepth, STR(strMap), 255,255,255 ) );

	// Host dialog
	switch ( languageId )
	{
	default:
		{
			hostButton = new TGUI( app, guiButton, 265,154,50,25, singlePlayerDialogDepth, STR(strStart) );
			cancelButton = new TGUI( app, guiButton, 320,154,50,25, singlePlayerDialogDepth, STR(strCancel) );
			break;
		}
	case 1:
		{
			hostButton = new TGUI( app, guiButton, 215,154,70,25, singlePlayerDialogDepth, STR(strStart) );
			cancelButton = new TGUI( app, guiButton, 290,154,80,25, singlePlayerDialogDepth, STR(strCancel) );
			break;
		}
	};

	nameEdit = new TGUI( app, guiEdit, 70,20,160,14, singlePlayerDialogDepth );
	nameEdit->Text( hostPlayerName );
	AddItem( hostPage, nameEdit );

	characterDropDown = new TGUI( app, guiDropDown, 70,40,160,14, singlePlayerDialogDepth+64 );
	mapDropDown = new TGUI( app, guiDropDown, 70,60,160,14, singlePlayerDialogDepth+64 );

	AddItem( hostPage, characterDropDown );
	AddItem( hostPage, mapDropDown );

	for ( i=0; i<numModels; i++ )
	{
		TString name = modelNames[i];
		characters[i].Reset();
		characterDropDown->AddDropDownItem( name );
	}

	// add all landscapes found
	for ( i=0; i<stories->Count(); i++ )
	{
		TStory* story = stories->Story(i);
		mapDropDown->AddDropDownItem( story->ShortTitle() );
	}

	characterDropDown->SelectedItem( selectedCharacter );
	mapDropDown->SelectedItem( selectedMap );

	AddItem( hostPage, hostButton );
	AddItem( hostPage, cancelButton );
	hostButton->ButtonKey( 13 );
	cancelButton->ButtonKey( VK_ESCAPE );

	TCharacter* mdlChar = NULL;
	if ( selectedCharacter >= 0 || size_t(selectedCharacter) < numModels )
	{
		mdlChar = &characters[selectedCharacter];
	}
	charGraphic = new TGUI( app, guiCompoundObject,300,140,
							singlePlayerDialogDepth+32, 
							mdlChar );
	charGraphic->Rotate( 0,0,0 );
	charGraphic->Scale( 110 );
	AddItem( hostPage, charGraphic );

	hostPage->SetLocation( app->Width()*0.5f - 200, app->Height()*0.5f - 125 );

	TGUI* prevPage = app->CurrentPage();
	app->CurrentPage( hostPage );

	initialised = true;

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
			if ( selectedCharacter >= 0 || size_t(selectedCharacter) < numModels )
			{
				mdlChar = &characters[selectedCharacter];
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

	
	hp.name = hostPlayerName;
	hp.character = byte(selectedCharacter);
	hp.map = byte(selectedMap);

	SetRegistryKey( "Software\\PDV\\Performance", "PlayerName", hostPlayerName );
	SetRegistryKey( "Software\\PDV\\Performance", "Character", size_t(selectedCharacter) );
	SetRegistryKey( "Software\\PDV\\Performance", "Map", size_t(selectedMap) );
		
	return true;
}


void SinglePlayerDialog::UpdateHostButtons( void )
{
	if ( closing || !initialised )
		return;

	hostPlayerName = nameEdit->Text();

	selectedCharacter = characterDropDown->SelectedItem();
	selectedMap = mapDropDown->SelectedItem();
	if ( selectedMap!=prevSelectedMap )
	{
		prevSelectedMap = selectedMap;
	}

	if ( hostPlayerName.length()>0 && 
		 selectedCharacter!=-1 &&
		 selectedMap!=-1 )
	{
		if ( !hostButton->Enabled() )
			hostButton->EnableControl( true );
	}
	else
	{
		if ( hostButton->Enabled() )
			hostButton->EnableControl( false );
	}
};



void SinglePlayerDialog::GetRegistrySettings( void )
{
	TString name;
	size_t val;
	if ( GetRegistryKey( "Software\\PDV\\Performance", "PlayerName", name ) )
	{
		hostPlayerName = name;
	}
	if ( GetRegistryKey( "Software\\PDV\\Performance", "Map", val ) )
	{
		selectedMap = val;
	}
	if ( GetRegistryKey( "Software\\PDV\\Performance", "Character", val ) )
	{
		selectedCharacter = val;
	}
	if ( !GetRegistryKey( "Software\\PDV\\Performance", "Language", languageId ) )
	{
		languageId = 0;
	}
};
				

//==========================================================================

