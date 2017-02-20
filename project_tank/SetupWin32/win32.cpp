#include <precomp_header.h>

#include <dos.h>
#include <ddraw.h>

#include <SetupWin32/win32Resource.rh>
#include <common/compatability.h>
#include <SetupWin32/events.h>
#include <SetupWin32/resource.h>
#include <landscape/main.h>
#include <jpeglib/tjpeg.h>
#include <SetupWin32/win32.h>

#include <direct.h>
#include <process.h>
#include <shellapi.h>
#include <Mmsystem.h>

#include <dialogs/dialogAssist.h>


//==========================================================================
//
//	System global vars
//

char			versionString[] = "Setup v1.02";

char 			appName[256];
char 			appTitle[256];
char			msgStr[256];

int				Wxsize = 640;
int				Wysize = 480;
int				Wbitdepth = 0;
float			Wdepth = 450.0f;
int				kFPS = 20;
bool			fullScreen = false;
bool			minimised = false;
bool			askOnce = false;
bool			mouseCaptured = false;
bool			fog = false;
int				mouseButton = 0;
int				prevXPos = 0;
int				prevYPos = 0;

RECT			desktopRect;
HINSTANCE		hInstance = NULL;
HWND			hWindow = NULL;
HWND			hConsole = NULL;
HWND			hConsoleEdit = NULL;
HDC				hdc = NULL;
HGLRC			hglrc = NULL;
LPDIRECTDRAW	lpdd = NULL;
HMENU			mainMenu = NULL;
HBITMAP			aboutBitmap = NULL;
HCURSOR			defaultCursor = NULL;

bool			firstTimeFlag = true;
bool			active = true;
bool			consoleVisible = true;
bool			showAbout = false;

HBRUSH			blackBrush = NULL;
HFONT			arialFont = NULL;
int				fontSize = 14;

TJpeg			aboutJPEG;

HWND			hVersion = NULL;

//==========================================================================
//
//	Timer stuff
//
DWORD			lastSecond;
DWORD			startTime;
int				frameCounter;
int				fixedFrameCounter = 0;
int				numSeconds;

//==========================================================================
//
//	Compatability with qland
//
bool			useMask = true;
bool			forceDepth16 = false;
bool			useHint1 = false;
bool			useHint2 = false;
bool			useFlush = true;
bool			useShadeModel = true;
bool			useColourMaterial = true;
bool			useLighting = true;
bool			useNormals = true;

//==========================================================================
//
// Entry point of all Windows programs
//

int APIENTRY WinMain( HINSTANCE hInst,
			   		  HINSTANCE hPrevInstance,
			   		  LPSTR lpCmdLine,
			   		  int nCmdShow )
{
    hInstance = hInst;

#if defined(_USECONSOLE)
	SetLogFile();
#else
	char logfile[256];
	GetTempPath( 255, logfile );
	strcat( logfile, "setuplog.txt" );
	SetLogFile( logfile );
#endif

	strcpy( appName, "Setup" );	
	strcpy( appTitle, "Excession Engine Setup" );	
	if ( !SetupWindow() )
		return false;
	
	char errStr[256];
	errStr[0] = 0;
	if ( !aboutJPEG.Load(hInstance,MAKEINTRESOURCE(IDR_SPLASHSCREEN),errStr) )
	{
		WriteString( errStr );
		return false;
	}

	MSG  msg;
	::PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE );
    while( WM_QUIT != msg.message  )
    {
		BOOL bGotMsg = ::PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE );
		if( bGotMsg )
        {
			// Translate and dispatch the message
			::TranslateMessage( &msg );
			::DispatchMessage( &msg );
        }
		else
			::WaitMessage();
    }

    return 0;
}

//==========================================================================

//
// Window procedure, handles all messages for this program
//
LRESULT CALLBACK WndProc(   HWND    hWnd,
							UINT    message,
							WPARAM  wParam,
							LPARAM  lParam	)
{
	switch (message)
	{
		case WM_CREATE:
		case WM_INITDIALOG:
			{
				DoCreate(hWnd);
			}
			break;

		case WM_GETDLGCODE:
			{
				MSG* msg = (MSG*)lParam;
				if ( msg!=NULL )
				if ( msg->message==WM_COMMAND )
				{
					DoMenu( msg->hwnd, msg->wParam );
				}
				break;
			}
		
		case WM_CLOSE:
			{
				::PostQuitMessage(0);
			}
			break;

		case WM_SIZE:
			{
				DoSize( hWnd, wParam );
			}
			break;

		case WM_ACTIVATE:
			{
				DoActivate( hWnd, wParam );
			}
			break;

        case WM_PAINT:
			{
				DoPaint( hWnd );
				return 0;
			}
        	break;

		case WM_KEYDOWN:
			{
				DoKeyDown( hWnd, wParam );
			}
			break;

		case WM_SYSKEYUP:
			{
				DoSysKeyUp( hWnd, wParam );
			}
			break;

		case WM_KEYUP:
			{
				DoKeyUp( hWnd, wParam );
			}
			break;

		case WM_CHAR:
			{
				DoKeyPress( hWnd, wParam );
			}
			break;

		// Window is being destroyed, cleanup
		case WM_DESTROY:
			{
				DoDestroy( hWnd );
			}
			break;

		// timer is set to fps, calling the gameLogic and gameRenderers
		case WM_TIMER:
			{
				DoTimer( hWnd, wParam );
			}
			break;

		// pass on mousemovements
		case WM_MOUSEMOVE:
			{
				DoMouseMove( hWnd, wParam, lParam );
			}
			break;

		// and mouse buttons
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
			{
				DoMouseButtonDown( hWnd, message, wParam, lParam );
			}
			break;

		// pass on mouse buttons
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
			{
				DoMouseButtonUp( hWnd, message, wParam, lParam );
			}
			break;

		case WM_ENTERMENULOOP:
			{
				active = false;
			}
			break;

		case WM_EXITMENULOOP:
			{
				active = true;
			}
			break;

		case WM_COMMAND:
			{
				DoMenu( hWnd, wParam );
			}
			break;

//		default:   // Passes it on if unproccessed
//		    return (DefWindowProc(hWnd, message, wParam, lParam));

	}
    return (0L);
}

//==========================================================================
//
//	Setup the windows class, the window, fonts, bitmaps, and directX
//

bool SetupWindow( void )
{
	// use arial font
	arialFont = ::CreateFont( fontSize,0,0,0,FW_DONTCARE,false,false,false,ANSI_CHARSET,
							  OUT_RASTER_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
							  DEFAULT_PITCH,"Arial" );
	// back brush to clear background
	blackBrush = HBRUSH(::GetStockObject(BLACK_BRUSH));

	aboutBitmap = 0; //::LoadBitmap( hInstance, MAKEINTRESOURCE(IDB_ABOUTBITMAP) );

	mainMenu = NULL; //::LoadMenu( hInstance, MAKEINTRESOURCE(IDR_MENU1) );
	defaultCursor = ::GetCursor();

	//
	// Register the window class
	//
	WNDCLASSEX wc;
	wc.cbSize			= sizeof(WNDCLASSEX);
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc		= (WNDPROC)WndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hIcon			= ::LoadIcon( hInstance, MAKEINTRESOURCE(IDI_ICON1) );
	wc.hCursor			= ::LoadCursor(NULL,IDC_ARROW);
	wc.hbrBackground	= blackBrush;
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= appName;
	wc.hIconSm			= NULL;

	if(::RegisterClassEx(&wc) == 0)
    {
		DWORD errNum = ::GetLastError();
    	WriteString( "Failed to register windows class (%ld)", errNum );
		return false;
    }

	HWND desktop = GetDesktopWindow();
	GetWindowRect( desktop, &desktopRect );

	HWND hwnd = CreateDialog( hInstance, MAKEINTRESOURCE(IDD_SETUP), 
							  NULL, (DLGPROC)WndProc);

	// Display the window
	::ShowWindow(hwnd,SW_SHOW);
	::UpdateWindow(hwnd);

	// If window was not created, quit
	if( hwnd == NULL )
    {
    	WriteString( "Window creation failed (%d)", GetLastError() );
		return false;
    }

    hWindow = hwnd;

	return true;
};



bool DoCreate( HWND hWnd )
{
	hVersion = GetDlgItem( hWnd, IDC_VERSION );
	EditPrintSingle( hVersion, versionString );
	
    hdc = ::GetDC( hWnd );
	if ( hdc==NULL )
	{
		WriteString( "Error getting window DC" );
		::PostQuitMessage(-1);
		return false;
	}

	RECT r;
	GetWindowRect( hWnd, &r );

	int left   = (desktopRect.right-desktopRect.left) / 2 - (r.right-r.left)/2;
	int width  = (r.right-r.left);
	int top    = (desktopRect.bottom-desktopRect.top) / 2 - (r.bottom-r.top)/2;
	int height = (r.bottom-r.top);

	MoveWindow( hWnd, left, top, width, height, true );

	return true;
};


void DoSize( HWND hWnd, WPARAM wParam )
{
	if ( wParam==SIZE_MINIMIZED )
	{
		minimised = true;
	}
	else
	{
		minimised = false;
		if ( !active )
			active = true;
	}
};


void DoActivate( HWND hWnd, WPARAM wParam )
{
	if ( wParam==WA_INACTIVE )
	{
		active = false;
		DoPaint( hWnd );
	}
	else if ( !minimised )
	{
		active = true;
		startTime = ::GetTickCount();
	}
};


void DoPaint( HWND hWnd )
{
//	::ValidateRgn( hWnd, NULL );
};


void DoKeyDown( HWND hWnd, WPARAM wParam )
{
};


void DoKeyUp( HWND hWnd, WPARAM wParam )
{
}


void DoSysKeyUp( HWND hWnd, WPARAM wParam )
{
};


void DoKeyPress( HWND hWnd, WPARAM wParam )
{
};
	

void DoDestroy( HWND hWnd )
{
	// Tell the application to terminate after the window
	// is gone.
	::PostQuitMessage(-1);
};


void DoTimer( HWND hWnd, WPARAM wParam )
{
	int timer_id = (int)wParam;
	if ( active )
	if ( timer_id==1 )
	{
	}
};


void DoMouseMove( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
};



void DoMouseButtonDown( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
};


void DoMouseButtonUp( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
}


void DoMenu( HWND hWnd, WPARAM wParam )
{
	if ( HIWORD(wParam)==BN_CLICKED )
	{
		switch ( LOWORD(wParam) )
		{
			case IDC_README:
				{
					ShellExecute( hWnd, "open", "readme.txt",
								  "", "", SW_SHOW );
					break;
				}

			case IDCANCEL:
				{
					::PostQuitMessage(0);
					break;
				}

			case IDC_VIDEO:
				{
					CreateVideoSetupDlg(hInstance,hWnd);
					break;
				}

			case IDC_PERFORMANCE:
				{
					CreatePerformanceDlg(hInstance,hWnd);
					break;
				}

			case IDOK:
				{
					size_t val;
					bool success = false;
					if ( GetRegistryKey( "Software\\PDV\\Performance", "TestCompleted", val ) )
					if ( val>0 )
						success = true;

					if ( !success )
					{
						char msg[1024];
						sprintf( msg, "%s%s%s",
							"It looks like you haven't tested this machine for this\n",
							"software yet.  Are you sure you want to proceed with\n",
							"playing this game?" );
						if ( MessageBox( hWnd, msg, "Information",
										 MB_YESNO|MB_ICONINFORMATION )==IDNO )
							return;
					}

					char buf[256];
					char olddir[256];
					char dir[256];
			        getcwd(buf,255);
					strcpy( olddir, buf );
					strcat(buf,"\\system" );
					strcpy(dir, buf);
					strcat(buf,"\\demo.exe" );

					if ( !TPersist::FileExists( buf ) )
					{
						strcpy( buf, olddir );
						strcat(buf,"\\demo.exe" );
					}
					else
					{
						chdir( dir );
					}

					spawnl( _P_NOWAIT, buf, "demo.exe", NULL);
					::PostQuitMessage(0);
					break;
				}

			case IDC_ABOUT:
				{
					CreateAboutDlg( hInstance, hWnd, &aboutJPEG );
					break;
				}

			default:
				break;
		}
	}
	else
	{
		// menu ids
	}
};


//==========================================================================
