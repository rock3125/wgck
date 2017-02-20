#include <precomp_header.h>

#include <windows.h>
#include <direct.h>
#include <process.h>
#include <shellapi.h>

#include <win32/events.h>

#include <systemDialogs/dialogAssist.h>
#include <dialogs/audioSettingsDlg.h>

#include <win32/resource.h>

//==========================================================================
//
// Window callback procedure
//
#ifndef _USEGUI
LRESULT CALLBACK AudioSettingsDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
AudioSettingsDialog* AudioSettingsDialog::ptr = NULL;
#endif

//==========================================================================
#ifdef _USEGUI
AudioSettingsDialog::AudioSettingsDialog( TEvent* _app, HINSTANCE _hInstance, HWND _parent )
	: hInstance( _hInstance ),
	  parent( _parent ),
	  audioSettingsPage( NULL ),
	  app( _app )
#else
AudioSettingsDialog::AudioSettingsDialog( HINSTANCE _hInstance, HWND _parent )
	: hInstance( _hInstance ),
	  parent( _parent )
#endif
{
#ifndef _USEGUI
	AudioSettingsDialog::ptr = this;
#endif
	initialised = false;

	playMusic = 1;
	playEffects = 1;
};


AudioSettingsDialog::~AudioSettingsDialog( void )
{
#ifdef _USEGUI
	if ( audioSettingsPage!=NULL )
		delete audioSettingsPage;
	audioSettingsPage = NULL;
#else
	AudioSettingsDialog::ptr = NULL;
#endif
};


void AudioSettingsDialog::Execute( void )
{
#ifdef _USEGUI
	if ( audioSettingsPage!=NULL )
		delete audioSettingsPage;
	audioSettingsPage = NULL;

	// Customise vehicle dialog
	AddItem( audioSettingsPage, new TGUI( app, guiWindow, 0, 0, 286, 384+38, -3, "Audio Settings" ) );

	okButton = new TGUI( app, guiButton, 210,320,50,25, -3, "OK" );
	cancelButton = new TGUI( app, guiButton, 210,350,50,25, -3, "Cancel" );

	AddItem( audioSettingsPage, okButton );
	AddItem( audioSettingsPage, cancelButton );
	okButton->ButtonKey( 13 );
	cancelButton->ButtonKey( VK_ESCAPE );
	
	audioSettingsPage->SetLocation( 120, 50 );

	initialised = true;
	TGUI* prevPage = app->CurrentPage();
	app->CurrentPage( audioSettingsPage );

	MSG msg;
	size_t fpsTimer = ::GetTickCount();
	bool closing = false;
	while ( !closing )
	{
		::ProcessMessages( app, msg, fpsTimer );
		if ( okButton->Clicked() )
		{
			app->CurrentPage( prevPage );
			closing = true;
		}
		if ( cancelButton->Clicked() )
		{
			app->CurrentPage( prevPage );
			closing = true;
		}
	}
#else
	if ( DialogBox( hInstance, 
                    MAKEINTRESOURCE(IDD_AUDIOSETTINGS), 
                    parent, (DLGPROC)AudioSettingsDlgProc)==IDOK) 
	{
		SetRegistryKey( "Software\\PDV\\Performance", "PlayMusic", playMusic );
		SetRegistryKey( "Software\\PDV\\Performance", "PlayEffects", playEffects );
	};
#endif
}

//==========================================================================
#ifndef _USEGUI
LRESULT CALLBACK AudioSettingsDlgProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch (message)
	{
		
		case WM_INITDIALOG:
			{
				AudioSettingsDialog::ptr->initialised = true;
				AudioSettingsDialog::ptr->enableMusicCheckBox = GetDlgItem( hwnd, IDC_ENABLEMUSIC );
				AudioSettingsDialog::ptr->enableEffectsCheckBox = GetDlgItem( hwnd, IDC_ENABLEEFFECTS );

				size_t playMusic, playEffects;
				if ( !GetRegistryKey( "Software\\PDV\\Performance", "PlayMusic", playMusic ) )
					playMusic = 1;
				if ( !GetRegistryKey( "Software\\PDV\\Performance", "PlayEffects", playEffects ) )
					playEffects = 1;

				AudioSettingsDialog::ptr->playMusic = playMusic;
				AudioSettingsDialog::ptr->playEffects = playEffects;

				ButtonSetCheck( AudioSettingsDialog::ptr->enableMusicCheckBox, playMusic==1 );
				ButtonSetCheck( AudioSettingsDialog::ptr->enableEffectsCheckBox, playEffects==1 );

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
						if ( ButtonGetCheck( AudioSettingsDialog::ptr->enableMusicCheckBox ) )
							AudioSettingsDialog::ptr->playMusic = 1;
						else
							AudioSettingsDialog::ptr->playMusic = 0;
						if ( ButtonGetCheck( AudioSettingsDialog::ptr->enableEffectsCheckBox ) )
							AudioSettingsDialog::ptr->playEffects = 1;
						else
							AudioSettingsDialog::ptr->playEffects = 0;
						EndDialog(hwnd, IDOK);
						return 0;
					}
					if ( LOWORD(wParam)==IDCANCEL )
					{
						EndDialog(hwnd, IDCANCEL);
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

