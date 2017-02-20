#include <precomp_header.h>

#include <windows.h>
#include <direct.h>
#include <process.h>

#include <dialogs/dialogAssist.h>

#include <SetupWin32/resource.h>

HWND hVideoSetupDlg = NULL;

HWND hFullScreen = NULL;
HWND hRes1 = NULL;
HWND hRes2 = NULL;
HWND hRes3 = NULL;
HWND hRes4 = NULL;
HWND hBit1 = NULL;
HWND hBit2 = NULL;
HWND hBit3 = NULL;
HWND hBit4 = NULL;
HWND hFog  = NULL;

size_t currentRes;
size_t currentBits;
size_t currentFullScreen;
size_t currentFog;

//==========================================================================
//
// Window callback procedure
//
LRESULT CALLBACK VideoSetupDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

//
// Create
//
void CreateVideoSetupDlg( HINSTANCE hInstance, HWND parent )
{
	hVideoSetupDlg = CreateDialog( hInstance, MAKEINTRESOURCE(IDD_VIDEOSETUP), 
									parent, (DLGPROC)VideoSetupDlgProc);
	ShowWindow(hVideoSetupDlg, SW_SHOW);
}


void UpdateBitButtons( void )
{
	if ( ButtonGetCheck( hFullScreen ) )
	{
		EnableControl( hBit1, true );
		EnableControl( hBit2, true );
		EnableControl( hBit3, true );
		EnableControl( hBit4, true );
	}
	else
	{
		EnableControl( hBit1, false );
		EnableControl( hBit2, false );
		EnableControl( hBit3, false );
		EnableControl( hBit4, false );
	}
};


void GetButtons( void )
{
	if ( ButtonGetCheck( hRes1 ) )
		currentRes = 0;
	else if ( ButtonGetCheck( hRes2 ) )
		currentRes = 1;
	else if ( ButtonGetCheck( hRes3 ) )
		currentRes = 2;
	else if ( ButtonGetCheck( hRes4 ) )
		currentRes = 3;

	if ( ButtonGetCheck( hBit1 ) )
		currentBits = 0;
	else if ( ButtonGetCheck( hBit2 ) )
		currentBits = 1;
	else if ( ButtonGetCheck( hBit3 ) )
		currentBits = 2;
	else if ( ButtonGetCheck( hBit4 ) )
		currentBits = 3;

	if ( ButtonGetCheck( hFullScreen ) )
		currentFullScreen = 1;
	else
		currentFullScreen = 0;

	if ( ButtonGetCheck( hFog ) )
		currentFog = 1;
	else
		currentFog = 0;
};


void SetButtons( void )
{
	switch ( currentRes )
	{
		case 0:
			{
				ButtonSetCheck( hRes1, true );
				break;
			}
		case 1:
			{
				ButtonSetCheck( hRes2, true );
				break;
			}
		case 2:
			{
				ButtonSetCheck( hRes3, true );
				break;
			}
		case 3:
			{
				ButtonSetCheck( hRes4, true );
				break;
			}
	}
	switch ( currentBits )
	{
		case 0:
			{
				ButtonSetCheck( hBit1, true );
				break;
			}
		case 1:
			{
				ButtonSetCheck( hBit2, true );
				break;
			}
		case 2:
			{
				ButtonSetCheck( hBit3, true );
				break;
			}
		case 3:
			{
				ButtonSetCheck( hBit4, true );
				break;
			}
	}
	if ( currentFullScreen==0 )
	{
		ButtonSetCheck( hFullScreen, false );
	}
	else
	{
		ButtonSetCheck( hFullScreen, true );
	}
	if ( currentFog==0 )
	{
		ButtonSetCheck( hFog, false );
	}
	else
	{
		ButtonSetCheck( hFog, true );
	}

	UpdateBitButtons();
};

//==========================================================================

LRESULT CALLBACK VideoSetupDlgProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch (message)
	{
		
		case WM_INITDIALOG:
			{
				hRes1 = GetDlgItem( hwnd, IDC_640x480 );
				hRes2 = GetDlgItem( hwnd, IDC_800x600 );
				hRes3 = GetDlgItem( hwnd, IDC_1024x768 );
				hRes4 = GetDlgItem( hwnd, IDC_1280x1024 );

				hBit1 = GetDlgItem( hwnd, IDC_15 );
				hBit2 = GetDlgItem( hwnd, IDC_16 );
				hBit3 = GetDlgItem( hwnd, IDC_24 );
				hBit4 = GetDlgItem( hwnd, IDC_32 );

				hFullScreen = GetDlgItem( hwnd, IDC_FULLSCREEN );
				hFog = GetDlgItem( hwnd, IDC_FOG );

				if ( !GetRegistryKey( "Software\\PDV\\Performance", "Resolution", currentRes ) )
					currentRes = 0;
				if ( !GetRegistryKey( "Software\\PDV\\Performance", "BitDepth", currentBits ) )
					currentBits = 1;
				if ( !GetRegistryKey( "Software\\PDV\\Performance", "FullScreen", currentFullScreen ) )
					currentFullScreen = 0;
				if ( !GetRegistryKey( "Software\\PDV\\Performance", "Fog", currentFog ) )
					currentFog = 1;

				SetButtons();

				return 0;
			}

		case WM_CLOSE:
			{
				::EndDialog(hwnd,0);
			}
			break;

		case WM_DESTROY: 
			{
				PostQuitMessage(0); 
				return 0;
			}

		case WM_COMMAND:
			{
				if ( HIWORD(wParam)==BN_CLICKED )
				{
					if ( LOWORD(wParam)==IDCANCEL )
					{
						EndDialog(hwnd, 0);
						return 0;
					}
					else if ( LOWORD(wParam)==IDOK )
					{
						GetButtons();
						if ( !SetRegistryKey( "Software\\PDV\\Performance", "Resolution", currentRes ) ||
							 !SetRegistryKey( "Software\\PDV\\Performance", "BitDepth", currentBits ) ||
							 !SetRegistryKey( "Software\\PDV\\Performance", "FullScreen", currentFullScreen ) ||
							 !SetRegistryKey( "Software\\PDV\\Performance", "Fog", currentFog ) )
							MessageBox( hwnd, "Error setting registry values?", "Error", MB_OK|MB_ICONERROR );
						EndDialog(hwnd, 0);
						return 0;
					}
					else if ( LOWORD(wParam)==IDC_FULLSCREEN )
					{
						UpdateBitButtons();
					}
				}
				break;
			}

		case WM_SYSCOMMAND:
			{
				if (wParam==IDCANCEL)
				{
					EndDialog(hwnd, 0);
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

