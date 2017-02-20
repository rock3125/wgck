#include <precomp_header.h>

#include <dos.h>
#include <ddraw.h>

#include <viewerWin32/win32Resource.rh>
#include <common/compatability.h>
#include <viewerWin32/events.h>
#include <viewerWin32/resource.h>
#include <landscape/main.h>
#include <jpeglib/tjpeg.h>
#include <viewerWin32/win32.h>


void CreatePerformanceDlg( HINSTANCE hInstance, HWND parent );

//==========================================================================
//
//	System global vars
//

size_t			kMinRenderDepth = 3;
size_t			kMaxRenderDepth = 10;

char			versionString[] = "Viewer v1.0";

TEvent*			app = NULL; // application object

char 			appName[256];
char 			appTitle[256];
char			msgStr[256];

int				Wxsize = 640;
int				Wysize = 480;
int				Wbitdepth = 0;
size_t			renderDepth = kMaxRenderDepth;
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

HINSTANCE		hInstance = NULL;
HWND			hWindow = NULL;
HWND			hConsole = NULL;
HWND			hConsoleEdit = NULL;
HDC				hdc = NULL;
HGLRC			hglrc = NULL;
LPDIRECTDRAW	lpdd = NULL;
HMENU			mainMenu = NULL;
HBITMAP			aboutBitmap = NULL;
TJpeg			aboutJPEG;
HCURSOR			defaultCursor = NULL;

bool			firstTimeFlag = true;
bool			active = true;
bool			consoleVisible = true;
bool			showAbout = false;

HBRUSH			blackBrush = NULL;
HFONT			arialFont = NULL;
int				fontSize = 14;

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
    char errStr[256];
	char fname[256];
	char texturePath[256];

    hInstance = hInst;

#if defined(_USECONSOLE)
	SetLogFile();
#else
	SetLogFile("viewerlog.txt");
#endif

	ParseCommandLine( lpCmdLine, fname, texturePath );

	// call down to user code to initialise this system
	// doh!
    InitialiseStartup( fname, texturePath, hInst, app, appName, appTitle, errStr );
    PostCond( app!=NULL );
    if ( errStr[0]!=0 )
    {
    	WriteString( errStr );
        return -1;
    }

	if ( !SetupWindow() )
		return false;
		
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
		else if ( active )
		{
			::InvalidateRect( hWindow, NULL, FALSE );
		}
		else if ( !active ) // be nice!
		{
			::WaitMessage();
		}
    }

	// restore mode if necessairy
	app->DXCleanup(hWindow);

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
			{
				DoCreate(hWnd);
			}
			break;

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

		default:   // Passes it on if unproccessed
		    return (DefWindowProc(hWnd, message, wParam, lParam));

	}
    return (0L);
}

//==========================================================================
//
//	Setup the windows class, the window, fonts, bitmaps, and directX
//

bool SetupWindow( void )
{
    char errStr[256];

	// use arial font
	arialFont = ::CreateFont( fontSize,0,0,0,FW_DONTCARE,false,false,false,ANSI_CHARSET,
							  OUT_RASTER_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
							  DEFAULT_PITCH,"Arial" );
	// back brush to clear background
	blackBrush = HBRUSH(::GetStockObject(BLACK_BRUSH));

	if ( !aboutJPEG.Load(hInstance,MAKEINTRESOURCE(IDR_SPLASHSCREEN),errStr) )
	{
		WriteString( errStr );
		return false;
	}

	aboutBitmap = 0; //::LoadBitmap( hInstance, MAKEINTRESOURCE(IDB_ABOUTBITMAP) );

	mainMenu = ::LoadMenu( hInstance, MAKEINTRESOURCE(IDR_MENU1) );
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
	wc.hbrBackground	= NULL;
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= appName;
	wc.hIconSm			= NULL;

	if(::RegisterClassEx(&wc) == 0)
    {
		DWORD errNum = ::GetLastError();
    	WriteString( "Failed to register windows class (%ld)", errNum );
		return false;
    }

	// setup stuff
	// Create the main application window
	long windowStyle;
	if ( fullScreen )
	{
		windowStyle = WS_POPUP | WS_VISIBLE;
	}
	else
	{
		windowStyle = WS_CAPTION | WS_SYSMENU |
					  WS_CLIPCHILDREN | WS_CLIPSIBLINGS | 
					  WS_VISIBLE | WS_OVERLAPPED | 
					  WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
	}

	HWND hwnd = ::CreateWindow(
				appName,
				appTitle,
				// OpenGL requires WS_CLIPCHILDREN and WS_CLIPSIBLINGS in windowed mode
                windowStyle,
				// Window position and size
				CW_USEDEFAULT, CW_USEDEFAULT,
				Wxsize, Wysize,
				NULL,
				mainMenu,
				hInstance,
				NULL);

	// Display the window
	::ShowWindow(hwnd,SW_SHOW);
	::UpdateWindow(hwnd);

	// If window was not created, quit
	if( hwnd == NULL )
    {
    	WriteString( "Window creation failed (%d)", GetLastError() );
		return false;
    }

	// set global used by ConsolePrint()
    hWindow = hwnd;

	if ( !app->SetupDirectX(hWindow,fullScreen,Wxsize,Wysize,Wbitdepth,errStr) )
	{
		WriteString( errStr );
		return false;
	}

    //
    //	Setup the keyread/move transmit timer
    //
    float fps = 1000.0f;
    fps = fps / float(kFPS);
    if ( SetTimer( hwnd,1,int(fps),NULL)==0 )
	{
		app->DXCleanup(hWindow);
		::DestroyWindow(hWindow);
		WriteString( "Timer start failed (%d)", GetLastError() );
		return false;
	}

    //
	//	Setup/initialise openGL
	//
	if ( !app->StartupSystem(hWindow,errStr) )
	{
		app->DXCleanup(hWindow);
		::DestroyWindow(hWindow);
		WriteString( errStr );
		return false;
	}
	lastSecond = ::GetTickCount() + 1000;
	startTime = ::GetTickCount();
	frameCounter = 0;


	// explain command line options
	ConsolePrint( "Command line options:" );
	ConsolePrint( "\nthe following switches must follow the filename parameter" );
	ConsolePrint( "-640x480          res switch" );
	ConsolePrint( "-800x600          res switch" );
	ConsolePrint( "-1024x768         res switch" );
	ConsolePrint( "-1280x1024        res switch" );
	ConsolePrint( "-fullscreen       start in fullscreen mode" );
	ConsolePrint( "-15				 15 bit depth buffer" );
	ConsolePrint( "-16				 16 bit depth buffer" );
	ConsolePrint( "-24				 24 bit depth buffer" );
	ConsolePrint( "-32				 32 bit depth buffer" );
	ConsolePrint( "-log=logfile.txt  output info to logfile" );

	return true;
};



bool DoCreate( HWND hWnd )
{
	#if defined(_USECONSOLE)
	ConsoleCreate( hWindow );
	#endif
	ConsolePrint( versionString );
    ConsolePrint( "Please wait, initialising system..." );
	char errStr[256];

    hdc = ::GetDC( hWnd );
	if ( hdc==NULL )
	{
		WriteString( "Error getting window DC" );
		::PostQuitMessage(-1);
		return false;
	}

	if ( app->SetDCPixelFormat( hdc, Wbitdepth, errStr )==0 )
	{
		WriteString( errStr );
		::PostQuitMessage(-1);
		return false;
	}

	hglrc = ::wglCreateContext( hdc );
	if ( hglrc==NULL )
	{
		WriteString( "Error creating GLRC" );
		::PostQuitMessage(-1);
		return false;
	}

	if ( !::wglMakeCurrent( hdc, hglrc ) )
	{
		WriteString( "wglMakeCurrent failed (%ld)", GetLastError() );
		::PostQuitMessage(-1);
		return false;
	}

    if ( !app->Setup( hWnd, Wdepth, Wxsize, Wysize, errStr ) )
	{
		WriteString( errStr );
		::PostQuitMessage(-1);
		return false;
	}

	return true;
};


void DoSize( HWND hWnd, WPARAM wParam )
{
	if ( wParam==SIZE_MINIMIZED )
	{
		minimised = true;
		if ( active )
		{
			app->DeactivateWindow(hWnd,active,fullScreen,Wxsize,Wysize,Wbitdepth);
		}
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
		app->DeactivateWindow(hWnd,active,fullScreen,Wxsize,Wysize,Wbitdepth);
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
	if ( active )
	{
   		if ( app!=NULL )
		{
			app->Draw();
			::ShowCursor(FALSE);
			::SwapBuffers(hdc);
			::ShowCursor(TRUE);
		}

		// fps determination
		DWORD timer = ::GetTickCount();
		frameCounter++;
		if ( timer > lastSecond )
		{
			lastSecond = timer + 1000;
			fixedFrameCounter = frameCounter;
			numSeconds++;
	
			// after 10 seconds - start adaptive frame measuring
			DWORD dt = timer-startTime;
			float fps = float(fixedFrameCounter) / float(numSeconds);
			if ( dt > 10000 )
			{
				if ( fps < 20.0f && renderDepth>kMinRenderDepth )
				{
					renderDepth--;
				}
				else if ( fps > 25.0f && renderDepth<kMaxRenderDepth )
				{
					renderDepth++;
				}
			}

			// after 30 seconds, ask once if want to quit if fps<10
			if ( !askOnce )
			if ( dt>30000 && fps<10.0f )
			{
				char msg[256];
				strcpy(msg,"This computer is not fast enough to\n");
				strcat(msg,"Play this game.  We recommend you upgrade\n");
				strcat(msg,"your computer and play this game at a later\n");
				strcat(msg,"stage.  You can choose to continue however.\n\n");
				strcat(msg,"Do you want to continue playing?");

				if ( ::MessageBox( hWnd, msg, "Information", MB_OK | MB_ICONINFORMATION | MB_YESNO )==IDNO )
				{
					::PostQuitMessage(-1);
					return;
				}
				askOnce = true;
			}
		}

		if ( numSeconds>0 )
		{
			char buf[256];
			float fps = float(fixedFrameCounter) / float(numSeconds);
			sprintf(buf,"%2.2f fps", fps );
			::SetBkMode( hdc,TRANSPARENT);
			::SetTextColor( hdc,RGB(255,255,255));
			::SelectObject( hdc, arialFont );
			::TextOut( hdc, 10,10, buf, strlen(buf) );

			if ( msgStr[0]!=0 )
				::TextOut( hdc, 80,10, msgStr, strlen(msgStr) );
		}
	}
	else
	{
		app->Draw();

		::ShowCursor(FALSE);
		::SwapBuffers(hdc);
		::ShowCursor(TRUE);

		if ( showAbout )
		{
			RECT rect;
			rect.left = Wxsize/2-aboutJPEG.Width()/2;
			rect.top  = Wysize/2-aboutJPEG.Height()/2;
			rect.right = rect.left + aboutJPEG.Width();
			rect.bottom = rect.top + aboutJPEG.Height();

			aboutJPEG.Draw(hdc,rect);
			/*
			HDC bitmapDC = ::CreateCompatibleDC(hdc);

			// get information on bitmap
			BITMAP bm;
			::GetObject(aboutBitmap, sizeof(BITMAP), (LPVOID)&bm);
   
			::SelectObject( bitmapDC, aboutBitmap );
			::BitBlt( hdc, 
					  Wxsize/2-bm.bmWidth/2,
					  Wysize/2-bm.bmHeight/2,
					  bm.bmWidth,
					  bm.bmHeight,
					  bitmapDC, 0,0, SRCCOPY );
			::DeleteDC(bitmapDC);
			*/
		};
	}

	::ValidateRgn( hWnd, NULL );
};


void DoKeyDown( HWND hWnd, WPARAM wParam )
{
	if ( showAbout )
	{
		active = true;
		showAbout = false;
	}

	if ( wParam==VK_ESCAPE && mouseCaptured )
	{
		mouseCaptured = false;
		::ReleaseCapture();
		::SetCursor( defaultCursor );
		::SetMenu(hWnd,mainMenu);
	}
	if ( app!=NULL )
		app->KeyDown((int)wParam);
};


void DoKeyUp( HWND hWnd, WPARAM wParam )
{
	if ( app!=NULL )
		app->KeyUp((int)wParam);
}


void DoSysKeyUp( HWND hWnd, WPARAM wParam )
{
	#if defined(_USECONSOLE)
	if ( wParam=='C' || wParam=='c' )
	{
		consoleVisible = !consoleVisible;
		if ( consoleVisible )
			ConsoleShow();
		else
			ConsoleHide();
	}
	#endif

	if ( wParam==13 )
	{
		app->SwitchFullScreen(hWnd,fullScreen,Wxsize,Wysize,Wbitdepth);
		if ( fullScreen )
			SetMenuItemTick(ID_VIEW_FULLSCREEN);
		else
			ClearMenuItemTick(ID_VIEW_FULLSCREEN);
	}
};


void DoKeyPress( HWND hWnd, WPARAM wParam )
{
	if ( app!=NULL )
		app->KeyPress( (int)wParam );
};
	

void DoDestroy( HWND hWnd )
{
	// Deselect the current rendering context and delete it
	KillTimer(hWnd,1);

	if ( hdc!=NULL )
	{
		wglMakeCurrent(hdc,NULL);
		if ( hWnd!=NULL )
			::ReleaseDC( hWnd, hdc );
   			hdc = NULL;
	}

	if ( hglrc!=NULL )
	{
		wglDeleteContext(hglrc);
		hglrc = NULL;
	}

	if ( app!=NULL )
       	app->Destroy();

	delete app;
	app = NULL;

	// Tell the application to terminate after the window
	// is gone.
	::PostQuitMessage(-1);
};


void DoTimer( HWND hWnd, WPARAM wParam )
{
	int timer_id = (int)wParam;
	if ( active )
	if ( timer_id==1 )
	if ( app!=NULL )
	{
		app->Logic();

		if ( mouseCaptured && mouseButton==0 )
		{
			RECT rect;
			::GetWindowRect( hWnd, &rect );
			int xp = (rect.left + rect.right) / 2;
			int yp = (rect.top + rect.bottom) / 2;
			::SetCursorPos( xp, yp );
			POINT point;
			point.x = xp;
			point.y = yp;
			::ScreenToClient(hWnd,&point);
			prevXPos = point.x;
			prevYPos = point.y;
		}

		if ( app->Finished() )
			::PostQuitMessage(0);
	}
};


void DoMouseMove( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	if ( mouseCaptured )
	{
		int xPos = LOWORD(lParam);  // horizontal position of cursor
		int yPos = HIWORD(lParam);  // vertical position of cursor
		int dx = xPos - prevXPos;
		int dy = yPos - prevYPos;
		prevYPos = yPos;
		prevXPos = xPos;

		if ( app!=NULL )
			app->MouseMove(dx,dy,(wParam&MK_CONTROL)>0);
	}
};



void DoMouseButtonDown( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	if ( showAbout )
	{
		active = true;
		showAbout = false;
	}

	if ( !mouseCaptured )
	{
		::SetCapture(hWnd);
		::SetCursor( NULL );
		::SetMenu(hWnd,NULL);

		// re-init openGL to use the newly available space
		char errStr[256];
		if ( !app->Setup( hWnd, Wdepth, Wxsize, Wysize, errStr ) )
		{
			WriteString( errStr );
			::PostQuitMessage(-1);
		}

		mouseCaptured = true;
		mouseButton = 0;
	}
	else
	{
		int xPos = LOWORD(lParam);  // horizontal position of cursor
		int yPos = HIWORD(lParam);  // vertical position of cursor
		int dx = xPos - prevXPos;
		int dy = yPos - prevYPos;
		prevYPos = yPos;
		prevXPos = xPos;

		if ( message==WM_LBUTTONDOWN )
		{
			mouseButton = 1;
            if ( app!=NULL )
				app->MouseDown(1,dx,dy);
		}
		else
		{
			mouseButton = 2;
            if ( app!=NULL )
				app->MouseDown(2,dx,dy);
		}
	}
};


void DoMouseButtonUp( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	if ( mouseCaptured )
	{
		mouseButton = 0;
		int xPos = LOWORD(lParam);  // horizontal position of cursor
		int yPos = HIWORD(lParam);  // vertical position of cursor
		if ( message==WM_LBUTTONUP )
		{
            if ( app!=NULL )
				app->MouseUp(1,xPos,yPos);
		}
		else
		{
            if ( app!=NULL )
				app->MouseUp(2,xPos,yPos);
		}
	}
}


void DoMenu( HWND hWnd, WPARAM wParam )
{
	switch(LOWORD(wParam))
	{
		case IDM_TESTSPEED:
			{
				CreatePerformanceDlg( hInstance, hWnd );
				break;
			}

		case ID_VIEW_FOG:
			{
				fog = !fog;
				if ( app!=NULL )
				{
					app->SetupFog( fog );
					if ( fog )
						SetMenuItemTick(ID_VIEW_FOG);
					else
						ClearMenuItemTick(ID_VIEW_FOG);
				}
			}
			break;

		case ID_VIEW_RESOLUTION_640X480:
			{
				Wxsize = 640;
				Wysize = 480;
				app->ResetResolution(hWnd,fullScreen,Wxsize,Wysize,Wdepth,Wbitdepth);

				SetMenuItemTick(ID_VIEW_RESOLUTION_640X480);
				ClearMenuItemTick(ID_VIEW_RESOLUTION_800X600);
				ClearMenuItemTick(ID_VIEW_RESOLUTION_1024X768);
				ClearMenuItemTick(ID_VIEW_RESOLUTION_1280X1024);
			}
			break;

		case ID_VIEW_RESOLUTION_800X600:
			{
				Wxsize = 800;
				Wysize = 600;
				app->ResetResolution(hWnd,fullScreen,Wxsize,Wysize,Wdepth,Wbitdepth);

				ClearMenuItemTick(ID_VIEW_RESOLUTION_640X480);
				SetMenuItemTick(ID_VIEW_RESOLUTION_800X600);
				ClearMenuItemTick(ID_VIEW_RESOLUTION_1024X768);
				ClearMenuItemTick(ID_VIEW_RESOLUTION_1280X1024);
			}
			break;

		case ID_VIEW_RESOLUTION_1024X768:
			{
				Wxsize = 1024;
				Wysize = 768;
				app->ResetResolution(hWnd,fullScreen,Wxsize,Wysize,Wdepth,Wbitdepth);

				ClearMenuItemTick(ID_VIEW_RESOLUTION_640X480);
				ClearMenuItemTick(ID_VIEW_RESOLUTION_800X600);
				SetMenuItemTick(ID_VIEW_RESOLUTION_1024X768);
				ClearMenuItemTick(ID_VIEW_RESOLUTION_1280X1024);
			}
			break;

		case ID_VIEW_RESOLUTION_1280X1024:
			{
				Wxsize = 1280;
				Wysize = 1024;
				app->ResetResolution(hWnd,fullScreen,Wxsize,Wysize,Wdepth,Wbitdepth);

				ClearMenuItemTick(ID_VIEW_RESOLUTION_640X480);
				ClearMenuItemTick(ID_VIEW_RESOLUTION_800X600);
				ClearMenuItemTick(ID_VIEW_RESOLUTION_1024X768);
				SetMenuItemTick(ID_VIEW_RESOLUTION_1280X1024);
			}
			break;

		case ID_FILE_NEWGAME:
			{
			}
			break;

		case ID_VIEW_FULLSCREEN:
			{
				app->SwitchFullScreen(hWnd,fullScreen,Wxsize,Wysize,Wbitdepth);
				if ( fullScreen )
					SetMenuItemTick(ID_VIEW_FULLSCREEN);
				else
					ClearMenuItemTick(ID_VIEW_FULLSCREEN);
			}
			break;

		case ID_FILE_QUIT:
			{
				::PostQuitMessage(0);
			}
			break;

		case ID_HELP_ABOUT:
			{
				::DrawMenuBar( hWnd );
				::InvalidateRect( hWnd, NULL, TRUE );
				active = false;
				showAbout = true;
			}
			break;

		default:
			break;
	}
};


//==========================================================================
//
// Window callback procedure for console
//
LRESULT CALLBACK ConsoleProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

const int SCROLLBACK_MAX_LINES = 600;
const int SCROLLBACK_DEL_CHARS = 500;

char *TranslateString( char *inbuf, char* outbuf )
{
	int		i, l;

	l = strlen(inbuf);
	char* out = outbuf;
	for ( i=0; i<l ;i++ )
	{
		if ( inbuf[i]=='\n' )
		{
			*out++ = '\r';
			*out++ = '\n';
		}
		else
			*out++ = inbuf[i];
	}
	*out++ = '\r';
	*out++ = '\n';
	*out++ = 0;
	return outbuf;
}

//==========================================================================
//
// Console Printf
//
void ConsolePrint( char *text, ... )
{
	va_list argptr;
	char	buf[1024];
	char	out[1024];
#if defined(_USECONSOLE)
	LRESULT	result;
	DWORD	oldPosS, oldPosE;
#endif

	va_start (argptr,text);
	vsprintf (buf, text,argptr);
	va_end (argptr);

	TranslateString(buf,out);
	WriteString( out );

#if defined(_USECONSOLE)
	result = SendMessage (hConsoleEdit, EM_GETLINECOUNT, 0, 0);
	if( result>SCROLLBACK_MAX_LINES && hConsoleEdit!=NULL )
	{
		char replaceText[5];
		
		replaceText[0] = '\0';

		SendMessage(hConsoleEdit, WM_SETREDRAW, (WPARAM)0, (LPARAM)0);
		SendMessage(hConsoleEdit, EM_GETSEL, (WPARAM)&oldPosS, (LPARAM)&oldPosE);
		SendMessage(hConsoleEdit, EM_SETSEL, 0, SCROLLBACK_DEL_CHARS);
		SendMessage(hConsoleEdit, EM_REPLACESEL, (WPARAM)0, (LPARAM)replaceText);
		SendMessage(hConsoleEdit, EM_SETSEL, oldPosS, oldPosE);
		SendMessage(hConsoleEdit, WM_SETREDRAW, (WPARAM)1, (LPARAM)0);
	}
	if ( hConsoleEdit!=NULL )
		SendMessage (hConsoleEdit, EM_REPLACESEL, 0, (LPARAM)out);
#endif
}

//==========================================================================
//
// Console Clear
//
void ConsoleClear( void )
{
	char text[4];
	text[0] = 0;
	SendMessage( hConsoleEdit, WM_SETTEXT, 0, (LPARAM)text );
}

//==========================================================================
//
// Create Console Window
//
void ConsoleCreate( HWND parent )
{
	hConsole = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_CONSOLEDIALOG), parent, (DLGPROC)ConsoleProc);
	ConsoleShow();
}

//==========================================================================

void ConsoleShow( void )
{
	ShowWindow(hConsole, SW_SHOW);
};

//==========================================================================

void ConsoleHide( void )
{
	ShowWindow(hConsole, SW_HIDE);
};

//==========================================================================
//
// Window callback for console
//
LRESULT CALLBACK ConsoleProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch (message)
	{
		
		case WM_INITDIALOG:
			{
				hConsoleEdit = GetDlgItem( hwnd, IDC_CONSOLEEDIT );
				return 0;
			}

		case WM_DESTROY: 
			{
				PostQuitMessage(0); 
				return 0;
			}

		case WM_SYSCOMMAND:
			{
				if (wParam == SC_CLOSE)
				{
					EndDialog(hwnd, 0);
					return 0;
				}
			}
			break;

		case WM_CTLCOLOREDIT:
			{
				SetBkColor( (HDC)wParam, RGB(0,0,0));
				SetTextColor( (HDC)wParam, RGB(224,224,224) );
				return (LRESULT)blackBrush;
			}

		case WM_SYSKEYUP:
			{
				if ( wParam=='C' || wParam=='c' )
				{
					consoleVisible = !consoleVisible;
					if ( consoleVisible )
						ConsoleShow();
					else
						ConsoleHide();
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

//==========================================================================

void SetMessage( char* msg )
{
	if ( msg==NULL )
		msgStr[0] = 0;
	else
		strcpy(msgStr,msg);
};


void ParseCommandLine( const char* lpCmdLine, char* fname, char* texturePath )
{
	char switches[256];

	// parse command line into to sections, first section filename + path
	// second section switches etc.
	fname[0] = 0;
	switches[0] = 0;
	texturePath[0] = 0;
	size_t i = 0;
	if ( lpCmdLine[0]!='-' )
	{
		while (lpCmdLine[i]!=' ' && lpCmdLine[i]!=0 ) i++;
		if ( lpCmdLine[i]==' ' )
		{
			strncpy(fname,lpCmdLine,i);
			fname[i] = 0;
			strcpy(texturePath,&lpCmdLine[i+1]);
	
			i = 0;
			while (texturePath[i]!=' ' && texturePath[i]!=0 ) i++;
			if ( texturePath[i]==' ' )
			{
				texturePath[i] = 0;
				strcpy(switches,&texturePath[i+1]);
			}
			else
			{
				strcpy(switches,lpCmdLine);
			}
		}
		else
		{
			if ( lpCmdLine[0]!='-' )
				strcpy(fname,lpCmdLine);
			else
				strcpy(switches,lpCmdLine);
		}
	}
	else
		strcpy( switches, lpCmdLine );

	char str[256];
	strcpy( str, switches );

	bool found = false;
	int index = 0;
	while ( !found )
	{
		if ( str[index]==0 )
			found = true;
		else
		{
			// found command line option
			if ( str[index]=='-' )
			{
				int i = 0;
				char buf[256];
				while ( str[index]!=0 && str[index]!=' ' )
					buf[i++] = str[index++];
				buf[i] = 0;

				if ( strcmp( buf, "-nomask" )==0 )
				{
					useMask = false;
				}
				else if ( strncmp( buf, "-renderdepth=", 13 )==0 )
				{
					renderDepth = atoi(&buf[13]);
					if ( renderDepth<=0 )
						renderDepth = 6;
				}
				else if ( strncmp( buf, "-log=", 5 )==0 )
				{
					char tmp[256];
					strcpy(tmp,&buf[5]);
					size_t i = 0;
					while ( tmp[i]!=' ' && tmp[i]!=0 )
						i++;
					tmp[i] = 0;
					SetLogFile( tmp );
				}
				else if ( strcmp( buf, "-640x480" )==0 )
				{
					Wxsize = 640;
					Wysize = 480;
				}
				else if ( strcmp( buf, "-800x600" )==0 )
				{
					Wxsize = 800;
					Wysize = 600;
				}
				else if ( strcmp( buf, "-1024x768" )==0 )
				{
					Wxsize = 1024;
					Wysize = 768;
				}
				else if ( strcmp( buf, "-1280x1024" )==0 )
				{
					Wxsize = 1280;
					Wysize = 1024;
				}
				else if ( strcmp( buf, "-15" )==0 )
				{
					Wbitdepth = 15;
				}
				else if ( strcmp( buf, "-16" )==0 )
				{
					Wbitdepth = 16;
				}
				else if ( strcmp( buf, "-24" )==0 )
				{
					Wbitdepth = 24;
				}
				else if ( strcmp( buf, "-32" )==0 )
				{
					Wbitdepth = 32;
				}
				else if ( strcmp( buf, "-fullscreen" )==0 )
				{
					fullScreen = true;
				}
			}
		}
		if ( str[index]!=0 )
			index++;
	}
};


void SetMenuItemTick( UINT menuItem )
{
	MENUITEMINFO info;
	info.cbSize = sizeof(MENUITEMINFO);
	info.fMask = MIIM_STATE;
	info.fState = MFS_CHECKED;
	::SetMenuItemInfo( mainMenu, menuItem, FALSE, &info );
	::DrawMenuBar(hWindow);
};


void ClearMenuItemTick( UINT menuItem )
{
	MENUITEMINFO info;
	info.cbSize = sizeof(MENUITEMINFO);
	info.fMask = MIIM_STATE;
	info.fState = MFS_UNCHECKED;
	::SetMenuItemInfo( mainMenu, menuItem, FALSE, &info );
	::DrawMenuBar(hWindow);
};


//==========================================================================
