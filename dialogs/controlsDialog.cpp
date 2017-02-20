#include <precomp_header.h>

#include <windows.h>
#include <direct.h>
#include <process.h>
#include <shellapi.h>

#include <win32/events.h>
#include <systemDialogs/dialogAssist.h>
#include <dialogs/controlsDialog.h>

#include <win32/resource.h>

//==========================================================================
//
// Window callback procedure
//
#ifndef _USEGUI
LRESULT CALLBACK ControlsDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
ControlsDialog* ControlsDialog::ptr = NULL;
#endif

//==========================================================================

ControlsDialog::ControlsDialog( TEvent* _app, HINSTANCE _hInstance, HWND _parent )
	: hInstance( _hInstance ),
	  parent( _parent ),
	  app( _app ),
	  controlsPage( NULL )
{
#ifndef _USEGUI
	ControlsDialog::ptr = this;
#endif
	initialised = false;
	languageId = app->LanguageId();
};


ControlsDialog::~ControlsDialog( void )
{
#ifndef _USEGUI
	ControlsDialog::ptr = NULL;
#endif
	if ( controlsPage!=NULL )
		delete controlsPage;
	controlsPage = NULL;
};


void ControlsDialog::Execute( UserKeys& keys )
{
#ifdef _USEGUI
	if ( controlsPage!=NULL )
		delete controlsPage;
	controlsPage = NULL;

	// Controls dialog
	AddItem( controlsPage, new TGUI( app, guiWindow, 0, 0, 340, 340, -3, STR(strGameControls) ) );
	reverseMouse = new TGUI( app, guiCheckBox, 20,220,110,14, -3, STR(strMirrorMouse) );

	buttonCustomise1 = new TGUI( app, guiButton, 110,20,75,20, -3, STR(strCustomise) );
	buttonCustomise2 = new TGUI( app, guiButton, 110,45,75,20, -3, STR(strCustomise) );
	buttonCustomise3 = new TGUI( app, guiButton, 110,70,75,20, -3, STR(strCustomise) );
	buttonCustomise4 = new TGUI( app, guiButton, 110,95,75,20, -3, STR(strCustomise) );
	buttonCustomise11 = new TGUI( app, guiButton, 110,120,75,20, -3, STR(strCustomise) );
	buttonCustomise5 = new TGUI( app, guiButton, 110,145,75,20, -3, STR(strCustomise) );
	buttonCustomise7 = new TGUI( app, guiButton, 110,170,75,20, -3, STR(strCustomise) );
//	buttonCustomise8 = new TGUI( app, guiButton, 110,220,75,20, -3, STR(strCustomise) );

	AddItem( controlsPage, new TGUI( app, guiLabel, 195,20,68,14, -3, STR(strTurnLeft), 255,255,255 ) );
	AddItem( controlsPage, new TGUI( app, guiLabel, 195,45,68,14, -3, STR(strTurnRight), 255,255,255 ) );
	AddItem( controlsPage, new TGUI( app, guiLabel, 195,70,68,14, -3, STR(strForwards), 255,255,255 ) );
	AddItem( controlsPage, new TGUI( app, guiLabel, 195,95,68,14, -3, STR(strBackwards), 255,255,255 ) );
	AddItem( controlsPage, new TGUI( app, guiLabel, 195,120,68,14, -3, STR(strJump), 255,255,255 ) );
	AddItem( controlsPage, new TGUI( app, guiLabel, 195,145,49,14, -3, STR(strFire), 255,255,255 ) );
	AddItem( controlsPage, new TGUI( app, guiLabel, 195,170,68,14, -3, STR(strInGameMap), 255,255,255 ) );
//	AddItem( controlsPage, new TGUI( app, guiLabel, 195,220,52,14, -3, "show scores", 255,255,255 ) );

	editLeft = new TGUI( app, guiLabel, 20,20,80,14, -3, KeyToString(languageId,keys.left), 255,255,255 );
	editRight = new TGUI( app, guiLabel, 20,45,80,14, -3, KeyToString(languageId,keys.right), 255,255,255 );
	editUp = new TGUI( app, guiLabel, 20,70,80,14, -3, KeyToString(languageId,keys.up), 255,255,255 );
	editDown = new TGUI( app, guiLabel, 20,95,80,14, -3, KeyToString(languageId,keys.down), 255,255,255 );
	editJump = new TGUI( app, guiLabel, 20,120,80,14, -3, KeyToString(languageId,keys.jump), 255,255,255 );
	editLeftMouse = new TGUI( app, guiLabel, 20,145,80,14, -3, KeyToString(languageId,keys.leftMouse), 255,255,255 );
	editMap = new TGUI( app, guiLabel, 20,170,80,14, -3, KeyToString(languageId,keys.map), 255,255,255 );
//	editScores = new TGUI( app, guiLabel, 20,220,80,14, -3, KeyToString(languageId,keys.scores), 255,255,255 );

	AddItem( controlsPage, editLeft );
	AddItem( controlsPage, editRight );
	AddItem( controlsPage, editUp );
	AddItem( controlsPage, editDown );
	AddItem( controlsPage, editJump );
	AddItem( controlsPage, editLeftMouse );
	AddItem( controlsPage, editMap );
	AddItem( controlsPage, reverseMouse );
//	AddItem( controlsPage, editScores );

	AddItem( controlsPage, buttonCustomise1 );
	AddItem( controlsPage, buttonCustomise2 );
	AddItem( controlsPage, buttonCustomise3 );
	AddItem( controlsPage, buttonCustomise4 );
	AddItem( controlsPage, buttonCustomise5 );
	AddItem( controlsPage, buttonCustomise7 );
//	AddItem( controlsPage, buttonCustomise8 );
	AddItem( controlsPage, buttonCustomise11 );

	reverseMouse->Checked( keys.reverseMouse );

	okButton = new TGUI( app, guiButton, 110,305,50,25, -3, STR(strOK) );
	AddItem( controlsPage, okButton );
	okButton->ButtonKey( VK_ESCAPE );

	controlsPage->SetLocation( 120, 120 );

	initialised = true;
	TGUI* prevPage = app->CurrentPage();
	app->CurrentPage( controlsPage );

	MSG msg;
	size_t fpsTimer = ::GetTickCount();
	bool closing = false;
	while ( !closing )
	{
		::ProcessMessages( app, msg, fpsTimer );
		if ( msg.message==WM_QUIT || msg.message==WM_CLOSE )
		{
			closing = true;
			app->CurrentPage( prevPage );
			::PostQuitMessage( 0 );
		}
		if ( okButton->Clicked() )
		{
			keys.reverseMouse = reverseMouse->Checked();

			app->CurrentPage( prevPage );
			keys.SaveToRegistry();
			closing = true;
		}

		// custom buttons & keys
		if ( buttonCustomise1->Clicked() )
		{
			size_t key = GetUserDefinedKey(app);
			if ( key>0 )
			{
				TString keyStr;
				keyStr = KeyToString( languageId, key );
				if ( keyStr.length()>0 )
				{
					editLeft->Text( keyStr );
					keys.left = key;
				}
			}
		}
		else if ( buttonCustomise2->Clicked() )
		{
			size_t key = GetUserDefinedKey(app);
			if ( key>0 )
			{
				TString keyStr;
				keyStr = KeyToString( languageId, key );
				if ( keyStr.length()>0 )
				{
					editRight->Text( keyStr );
					keys.right = key;
				}
			}
		}
		else if ( buttonCustomise3->Clicked() )
		{
			size_t key = GetUserDefinedKey(app);
			if ( key>0 )
			{
				TString keyStr;
				keyStr = KeyToString( languageId, key );
				if ( keyStr.length()>0 )
				{
					editUp->Text( keyStr );
					keys.up = key;
				}
			}
		}
		else if ( buttonCustomise4->Clicked() )
		{
			size_t key = GetUserDefinedKey(app);
			if ( key>0 )
			{
				TString keyStr;
				keyStr = KeyToString( languageId, key );
				if ( keyStr.length()>0 )
				{
					editDown->Text( keyStr );
					keys.down = key;
				}
			}
		}
		else if ( buttonCustomise5->Clicked() )
		{
			size_t key = GetUserDefinedKey(app);
			if ( key>0 )
			{
				TString keyStr;
				keyStr = KeyToString( languageId, key );
				if ( keyStr.length()>0 )
				{
					editLeftMouse->Text( keyStr );
					keys.leftMouse = key;
				}
			}
		}
		else if ( buttonCustomise7->Clicked() )
		{
			size_t key = GetUserDefinedKey(app);
			if ( key>0 )
			{
				TString keyStr;
				keyStr = KeyToString( languageId, key );
				if ( keyStr.length()>0 )
				{
					editMap->Text( keyStr );
					keys.map = key;
				}
			}
		}
/*
		else if ( buttonCustomise8->Clicked() )
		{
			size_t key = GetUserDefinedKey(app);
			if ( key>0 )
			{
				TString keyStr;
				if ( languageId==0 )
					keyStr = KeyToString( key );
				else
					keyStr = KeyToGermanString( key );
				if ( keyStr.length()>0 )
				{
					editScores->Text( keyStr );
					keys.scores = key;
				}
			}
		}
*/
		else if ( buttonCustomise11->Clicked() )
		{
			size_t key = GetUserDefinedKey(app);
			if ( key>0 )
			{
				TString keyStr;
				keyStr = KeyToString( languageId, key );
				if ( keyStr.length()>0 )
				{
					editJump->Text( keyStr );
					keys.jump = key;
				}
			}
		}
	}
#else
	if ( DialogBox( hInstance, 
                    MAKEINTRESOURCE(IDD_CONTROLS), 
                    parent, (DLGPROC)ControlsDlgProc)==IDOK) 
	{
	};
#endif
}

//==========================================================================

#ifndef _USEGUI
LRESULT CALLBACK ControlsDlgProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch (message)
	{
		
		case WM_INITDIALOG:
			{
				ControlsDialog::ptr->initialised = true;
				return 0;
			}

		case WM_CLOSE:
			{
				::EndDialog(hwnd,IDCANCEL);
			}

		case WM_COMMAND:
			{
				if ( HIWORD(wParam)==BN_CLICKED )
				{
					if ( LOWORD(wParam)==IDOK )
					{
						EndDialog(hwnd, IDOK);
						return 0;
					}
				}
				break;
			}

		case WM_SYSCOMMAND:
			{
				if (wParam==IDCANCEL)
				{
					EndDialog(hwnd, IDCANCEL);
					return 0;
				}
			}
			break;

		case WM_SYSKEYUP:
			{
				if ( wParam=='C' || wParam=='c' )
				{
				}
			}
			break;

		default:
			{
				return 0;
			}
	}
	return 0;
}
#endif

//==========================================================================

UserKeys::UserKeys( void )
{
	left = VK_LEFT;
	right = VK_RIGHT;
	up = VK_UP;
	down = VK_DOWN;
	jump = 32;
	map = 'M';
	destruct = 'X';
	scores = 'S';
	leftMouse = 13; //VK_LBUTTON;
	rightMouse = VK_RBUTTON;
	reverseMouse = false;
};

UserKeys::~UserKeys( void )
{
};

UserKeys::UserKeys( const UserKeys& uk )
{
	operator=(uk);
};

const UserKeys& UserKeys::operator=( const UserKeys& uk )
{
	left = uk.left;
	right = uk.right;
	up = uk.up;
	down = uk.down;
	map = uk.map;
	jump = uk.jump;
	destruct = uk.destruct;
	scores = uk.scores;
	leftMouse = uk.leftMouse;
	rightMouse = uk.rightMouse;
	reverseMouse = uk.reverseMouse;
	return uk;
};

bool UserKeys::Defined( size_t key )
{
	if ( key==left || key==right || key==up || key==down ||
		 key==map || key==destruct || key==scores ||
		 key==leftMouse || key==rightMouse || key==jump )
		return true;
	return false;
};

void UserKeys::LoadFromRegistry( void )
{
	size_t key;
	if ( GetRegistryKey( "Software\\PDV\\Performance", "KeyUp", key ) )
		up = key;
	if ( GetRegistryKey( "Software\\PDV\\Performance", "KeyLeft", key ) )
		left = key;
	if ( GetRegistryKey( "Software\\PDV\\Performance", "KeyDown", key ) )
		down = key;
	if ( GetRegistryKey( "Software\\PDV\\Performance", "KeyRight", key ) )
		right = key;
	if ( GetRegistryKey( "Software\\PDV\\Performance", "KeyJump", key ) )
		jump = key;
	if ( GetRegistryKey( "Software\\PDV\\Performance", "KeyMap", key ) )
		map = key;
	if ( GetRegistryKey( "Software\\PDV\\Performance", "KeyLM", key ) )
		leftMouse = key;
	if ( GetRegistryKey( "Software\\PDV\\Performance", "KeyRM", key ) )
		rightMouse = key;
	if ( GetRegistryKey( "Software\\PDV\\Performance", "KeyDestruct", key ) )
		destruct = key;
	if ( GetRegistryKey( "Software\\PDV\\Performance", "KeyScores", key ) )
		scores = key;
	if ( GetRegistryKey( "Software\\PDV\\Performance", "ReverseMouse", key ) )
		reverseMouse = (key==1);
};

void UserKeys::SaveToRegistry( void )
{
	SetRegistryKey( "Software\\PDV\\Performance", "KeyUp", up );
	SetRegistryKey( "Software\\PDV\\Performance", "KeyDown", down );
	SetRegistryKey( "Software\\PDV\\Performance", "KeyLeft", left );
	SetRegistryKey( "Software\\PDV\\Performance", "KeyRight", right );
	SetRegistryKey( "Software\\PDV\\Performance", "KeyJump", jump );

	SetRegistryKey( "Software\\PDV\\Performance", "KeyLM", leftMouse );
	SetRegistryKey( "Software\\PDV\\Performance", "KeyRM", rightMouse );

	SetRegistryKey( "Software\\PDV\\Performance", "KeyMap", map );
	SetRegistryKey( "Software\\PDV\\Performance", "KeyScores", scores );
	SetRegistryKey( "Software\\PDV\\Performance", "KeyDestruct", destruct );

	SetRegistryKey( "Software\\PDV\\Performance", "ReverseMouse", reverseMouse?1:0 );
};

//==========================================================================
