#include <precomp_header.h>

#include <dos.h>

#include <win32/win32Resource.rh>
#include <common/compatability.h>
#include <win32/events.h>
#include <win32/resource.h>
#include <jpeglib/tjpeg.h>
#include <win32/win32.h>
#include <direct.h>

#include <object/geometry.h>

#if defined(_TANKGAME) || defined(_EDITOR)
#define _USEREGISTRATION
#endif
#if defined(_EDITOR) || defined(_VIEWER) || defined(_SPEEDTEST) || defined(_PARSER)
#define _HASMENU
#endif

#ifdef _SPEEDTEST
#include <dialogs/dialogAssist.h>
#include <speed test/speedtest.h>
#endif
#ifdef _TANKGAME
#include <tank/tankApp.h>
#include <systemDialogs/dialogAssist.h>
#include <systemDialogs/languageDialog.h>
#endif
#ifdef _VIEWER
#include <viewer/main.h>
#endif
#ifdef _EDITOR
#include <landscape/main.h>
#endif
#ifdef _TIMELIMIT
#include <dialogs/timeLimit.h>
#endif
#ifdef _PARSER
#include <parser/parserMain.h>
#endif

//==========================================================================
//
//	System global vars
//
TEvent* app = NULL;

// engine trivial vars
#ifdef _TIMELIMIT
size_t	secureDay = 50+1;
size_t	secureMonth = 100+6;
size_t	secureYear = 1500 + 2002;
#endif

HBRUSH		blackBrush = NULL;
HBRUSH		blueBrush = NULL;
HPEN		whitePen = NULL;
HFONT		font = NULL;
HMENU		mainMenu = NULL;
HINSTANCE	hInstance = NULL;
MSG			msg;
HCURSOR		defaultCursor = NULL;
size_t		fpsTimer = 0;
size_t		frameCounter = 0;
DWORD		startTime;
DWORD		startTimerTime;
int			fixedFrameCounter;
bool		firstTime = true;
bool		mouseCaptured = false;
int			mouseButton = 0;
float		fps = 0;
int			prevXPos = 0;
int			prevYPos = 0;
HWND		mainWindow = NULL;
TString		_dir;

bool		registered;
bool		expired;
TString		registeredMsg;

size_t		progress = 0;
TString		progressMsg;
DWORD		backoffTimer = 0; // renderdepth timer
size_t		timeLimitCounter = 0;

DEVMODE		initialScreenMode; // initial screen mode

void WaitMessage( HWND hWnd );

#ifdef _TIMELIMIT
size_t expiryDay = 1;
size_t expiryMonth = 6;
size_t expiryYear = 2002;
#endif

//==========================================================================

TString	RegisteredMessage( void )
{
	return registeredMsg;
};

void SetApp( TEvent* _app )
{
	app = _app;
};

void PreInit( HINSTANCE hInst )
{
    TString errStr;
	TString fname;
	TString texturePath;

	// get cwd
	char cwd[1024];
	_getcwd( cwd, 1024 );
	_dir = cwd;

	expired = false;
	hInstance = hInst;

#if defined(_USEREGISTRATION)
	size_t languageId;
	if ( !GetRegistryKey( "Software\\PDV\\Performance", "Language", languageId ) )
	{
		languageId = 0;
	}

	registered = false;
	registeredMsg = STR(strUnregistered);
	expired = false;
#endif
};


bool PostInit( void )
{
#if defined(_EDITOR)
	TString name, email;
	if ( !IsRegistered( name, email ) )
	{
		::MessageBox( NULL, STR(strOnlyRegistered), STR(strError), MB_OK|MB_ICONERROR );
		exit(0);
	}
#endif

#if defined(_TIMELIMIT)
	SYSTEMTIME systemTime;
	GetSystemTime( &systemTime );
	
	size_t expiry = expiryYear * 12 * 31 + expiryMonth * 31 + expiryDay;
	size_t today  = systemTime.wYear * 12 * 31 +
					systemTime.wMonth * 31 +
					systemTime.wDay;
	bool isOk = today < expiry;

	if ( isOk )
	{
		TString date = Int2Str( expiryDay ) + " ";
		switch ( expiryMonth )
		{
		case 1: { date = date + STR(strJan); break; };
		case 2: { date = date + STR(strFeb); break; };
		case 3: { date = date + STR(strMar); break; };
		case 4: { date = date + STR(strApr); break; };
		case 5: { date = date + STR(strMay); break; };
		case 6: { date = date + STR(strJun); break; };
		case 7: { date = date + STR(strJul); break; };
		case 8: { date = date + STR(strAug); break; };
		case 9: { date = date + STR(strSep); break; };
		case 10: { date = date + STR(strOct); break; };
		case 11: { date = date + STR(strNov); break; };
		case 12: { date = date + STR(strDec); break; };
		}
		date = date + " " + Int2Str( expiryYear );

		TString msg = STR(strProgramWillExpire) + date;
		TimeLimitDialog tl(hInst,msg);
		tl.Execute();
	}
	else
	{
		::MessageBox( NULL, STR(strHasExpired), STR(strError), MB_OK|MB_ICONERROR );
		return 0;
	}
#endif


#ifdef _VIEWER
	app->SetCmdLine( lpCmdLine );
	SetLogFile( "_viewerlog.txt" );
#endif
#ifdef _PARSER
	SetLogFile( "_parserlog.txt" );
#endif
#ifdef _EDITOR
	SetLogFile( "_editorlog.txt" );
#endif

#ifdef _SPEEDTEST
	app->SetCmdLine( lpCmdLine );
	SetLogFile( "_performancelog.txt" );
#endif

	//
	// Create window class only once
	//
#ifdef _PARSER
	font = ::CreateFont( 14,0,0,0,FW_DONTCARE,false,false,false,ANSI_CHARSET,
						  OUT_RASTER_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
						  DEFAULT_PITCH,"Courier New" );
#else
	font = ::CreateFont( 12,0,0,0,FW_DONTCARE,false,false,false,ANSI_CHARSET,
						  OUT_RASTER_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
						  DEFAULT_PITCH,"Arial" );
#endif

	// back brush to clear background
	blackBrush = HBRUSH(::GetStockObject(BLACK_BRUSH));
	whitePen = HPEN(::GetStockObject(WHITE_PEN));
	blueBrush = ::CreateSolidBrush( RGB(20,20,200) );

#if defined(_HASMENU)
	mainMenu = ::LoadMenu( app->Instance(), MAKEINTRESOURCE(IDR_MENU1) );
#endif
#if defined(_PARSER)
	mainMenu = NULL;
#endif
#ifdef _EDITOR
	mainMenu = ::LoadMenu( app->Instance(), MAKEINTRESOURCE(IDM_MAPEDITOR) );
#endif

	defaultCursor = ::LoadCursor( app->Instance(), IDC_ARROW );

#ifdef _TANKGAME
	// new installations get to pick their language
	if ( app->NewInstallation() )
	{
		LanguageDialog dlg(hInstance);
		if ( !dlg.Execute() )
		{
			exit(0);
		}
	}
#endif

	// get initial screen settings
	EnumDisplaySettings( NULL, ENUM_CURRENT_SETTINGS, &initialScreenMode );
	// continue and create windows class
	WriteString( "Created windows class\n" );

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
#ifdef _PARSER
	wc.hbrBackground	= blackBrush;
#else
	wc.hbrBackground	= NULL;
#endif
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= app->ApplicationName().c_str();
	wc.hIconSm			= NULL;

	TString errStr;
	if(::RegisterClassEx(&wc) == 0)
    {
		DWORD errNum = ::GetLastError();
    	errStr = "Failed to register windows class (" + Int2Str( errNum ) + ")";
		if ( app!=NULL )
			delete app;
		app = NULL;
		return false;
    }

	{
		bool reset = false;

#if defined(_HASMENU)
		reset = true;
#endif

		if ( reset )
		if ( !app->ResetResolution(mainMenu,errStr) )
		{
			errStr = "\n" + errStr + "\n";
			WriteString( errStr.c_str() );
			if ( app!=NULL )
				delete app;
			app = NULL;
			return false;
		}
	}

	if ( app->LimitFPS() )
	{
		fpsTimer = GetTickCount() + 67;
	}

	//	Setup/initialise client loaders etc.
	if ( !app->StartupSystem(errStr) )
	{
		WriteString( "\n*** Error in StartupSystem (%s)\n", errStr.c_str() );
		::DestroyWindow(app->Window());
		if ( app!=NULL )
			delete app;
		app = NULL;
		return false;
	}

	if ( app->IsSoftware() )
	{
		size_t languageId = app->LanguageId();
		TString msg = STR(strNoOpenGL);
		::Message( app, msg, STR(strWarning), MB_OK|MB_ICONWARNING );
	}

	return true;
};


void ReleaseResources( void )
{
	// restore mode if necessairy
	ResetScreenToStartupMode();

	// free app
	if ( app!=NULL )
		delete app;
	app = NULL;

	// delete GDI objects
	if ( font!=NULL )
		::DeleteObject(font);
	font = NULL;

	if ( blackBrush!=NULL )
		::DeleteObject(blackBrush);
	blackBrush = NULL;

	if ( whitePen!=NULL )
		::DeleteObject(whitePen);
	whitePen = NULL;

	if ( blueBrush!=NULL )
		::DeleteObject(blueBrush);
	blueBrush = NULL;
};

HRESULT StartGameSystem( void )
{
	WriteString( "entering main system loop\n\n" );
	do
    {
		::ProcessMessages( app, msg, fpsTimer );
    }
    while ( msg.message!=WM_QUIT );

	ReleaseResources();

	return msg.wParam;
};

//==========================================================================

void ResetScreenToStartupMode( void )
{
	::ChangeDisplaySettings( &initialScreenMode, 0 );
};

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
				break;
			}

		case WM_CLOSE:
			{
				DoExit( hWnd );
				break;
			}

		case WM_SIZE:
			{
				DoSize( hWnd, wParam );
				break;
			}

		case WM_ACTIVATE:
			{
				DoActivate( hWnd, wParam );
				break;
			}

        case WM_PAINT:
			{
				DoPaint( hWnd );
	        	break;
			}

		case WM_KEYDOWN:
			{
				DoKeyDown( hWnd, wParam );
				break;
			}

		case WM_KEYUP:
			{

				DoKeyUp( hWnd, wParam );
				break;
			}

		case WM_CHAR:
			{
				DoKeyPress( hWnd, wParam );
				break;
			}

		case WM_TIMER:
			{
				DoTimer( hWnd, wParam ); // gameLogic and gameRenderers
				break;
			}

		case WM_MOUSEMOVE:
			{
				DoMouseMove( hWnd, wParam, lParam );
				break;
			}

		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
			{
				DoMouseButtonDown( hWnd, message, wParam, lParam );
				break;
			}

		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
			{
				DoMouseButtonUp( hWnd, message, wParam, lParam );
				break;
			}

		case WM_COMMAND:
			{
				DoMenu( hWnd, wParam );
				break;
			}

		default:
			{
				return (DefWindowProc(hWnd, message, wParam, lParam)); // Passes it on if unproccessed
			}

	}
    return (0L); // assume processed
}

//==========================================================================

bool DoCreate( HWND hWnd )
{
	WriteString( "Initiating WM_CREATE\n" );

	//
	//	Setup the keyread/move transmit timer
	//
	WriteString( "Setting up logic timer\n" );
	float fps = 1000.0f;
	fps = fps / float(app->LogicFPS());
	if ( ::SetTimer( hWnd,1,int(fps),NULL)==0 )
	{
		::DestroyWindow(hWnd);
		hWnd = NULL;
		TString errStr = "Timer start failed (" + Int2Str(GetLastError()) + ")\n";
		WriteString( errStr.c_str() );
		return false;
	}
	WriteString( "timer setup done\n\n" );

#ifdef _SPEEDTEST
	if ( !CreatePerformanceDlg( hInstance, hWnd ) )
		exit(0);
#endif

	startTime = ::GetTickCount();
	startTimerTime = ::GetTickCount();

	WriteString( app->VersionString().c_str() );
	WriteString( "\n" );
	TString errStr;

	startTime = ::GetTickCount();
	startTimerTime = ::GetTickCount();

	frameCounter = 0;

	mainWindow = hWnd;

	WriteString( "WM_CREATE done\n\n" );
	return true;
};


void DoExit( HWND hWnd )
{
#ifdef _EDITOR
	if ( app->Modified() )
	{
		if ( ::MessageBox( hWnd, STR(strSaveChanges), STR(strWarning), MB_YESNO | MB_ICONWARNING )==IDYES )
		{
			app->SaveDefn();
		}
	}
#endif
	DoDestroy(hWnd);
};


void DoSize( HWND hWnd, WPARAM wParam )
{
#ifdef _TIMELIMIT
	// extra traps
	if ( expiryYear!=(secureYear-1500) ||
		 expiryMonth!=(secureMonth-100) ||
		 expiryDay!=(secureDay-50) )
	{
		exit(0);
	}
#endif

	if ( wParam==SIZE_MINIMIZED )
	{
		app->Minimised( true );
		if ( app->Active() )
		{
			TString errStr;
			if ( !app->DeactivateWindow( hWnd, errStr) )
			{
				errStr = "\n" + errStr + "\n";
				WriteString( errStr.c_str() );
				::PostQuitMessage(-1);
			}
		}
	}
	else
	{
		app->Minimised( false );
		if ( !app->Active() )
			app->Active( true );
	}
};


void DoActivate( HWND hWnd, WPARAM wParam )
{
	if ( app==NULL )
		return;

	if ( wParam==WA_INACTIVE && app->Active() )
	{
		if ( app->FullScreen() )
		{
			TString errStr;
			if ( !app->DeactivateWindow( hWnd,errStr ) )
			{
				errStr = "\n" + errStr + "\n";
				WriteString( errStr.c_str() );
				::PostQuitMessage(-1);
				return;
			}
			DoPaint( hWnd );
			ResetScreenToStartupMode();
			::ShowWindow( hWnd, SW_MINIMIZE );
		}
		app->Active( false );
	}
	else if ( wParam==WA_ACTIVE || wParam==WA_CLICKACTIVE )
	{
		TString errStr;
		if ( !app->Active() )
		{
			if ( !app->ChangeResolution(errStr) )
			{
				WriteString( errStr.c_str() );
				::PostQuitMessage(-1);
				return;
			}
		}
		
		::SetForegroundWindow( hWnd );
		::ShowWindow( hWnd, SW_RESTORE );

		app->Active( true );

		startTimerTime = ::GetTickCount();
		startTime = ::GetTickCount();

		::InvalidateRect( hWnd, NULL, TRUE );
	}
};


void DoPaint( HWND hWnd )
{
#ifdef _PARSER
	app->Draw();
#else
	if ( app->Initialised() )
	{
#ifndef _MESA
		// openGL specific
		::wglMakeCurrent( app->HDc(), app->HGLRc() );
#endif
		app->Expired( expired );
		if ( app->Active() )
		{
			// fps determination
			frameCounter++;

			DWORD timer = ::GetTickCount();
			DWORD deltat = timer - startTimerTime;
			if ( deltat>1000 )
			{
				startTimerTime = timer;
				float numSec = float(deltat) / 1000.0f;

				fps = float(frameCounter) / numSec;
				frameCounter = 0;
			}
			if ( app!=NULL )
			{
				app->Draw();
			}
		}
		else
		{
			if ( app!=NULL )
			{
				app->Draw();
			}
		}

		app->SetupGL2d( app->Depth(), app->Width(), app->Height() );
		glLoadIdentity();

/*
		{
			char buf[256];
			sprintf(buf,"%2.1f fps", fps );

			glDisable(GL_BLEND);
			glEnable(GL_COLOR_MATERIAL);

			glColor3ub(255,255,255);
#ifdef _MESA
			app->Write( 10,float(app->Height())-20,-10, buf );
#else
			app->Write( 10,float(app->Height())-40,-10, buf );
#endif
		}
*/
		// display messages and countdowns
		app->WriteMessage();
		app->WriteCountDown();

		// swap double buffer
#ifdef _MESA
		::GM_swapBuffers(MGL_waitVRT);
#else
		::SwapBuffers(app->HDc());
		::ValidateRgn( hWnd, NULL );
#endif

#if defined(_USEREGISTRATION)
		if ( expired && timeLimitCounter > 1000 )
		{
			size_t languageId = app->LanguageId();
			TString msg = STR(strHasExpired);
			::MessageBox( hWnd, msg.c_str(), STR(strError), MB_OK|MB_ICONERROR );
			exit(0);
		}
#endif
	}
	else
	{
		WaitMessage(hWnd);
	}
#endif
};


bool CheckRego( void )
{
	size_t languageId;
	if ( !GetRegistryKey( "Software\\PDV\\Performance", "Language", languageId ) )
	{
		languageId = 0;
	}

	TString name, email;
	if ( IsRegistered(name,email) )
	{
		registered = true;
		expired = false;
		registeredMsg = STR(strRegisteredTo) + name;
		app->RegisteredMessage( registeredMsg );
		return true;
	}
	else
	{
		registeredMsg = STR(strUnregistered);
		registered = false;
		app->RegisteredMessage( registeredMsg );
		expired = HasExpired();
	}
	return false;
};


void WaitMessage( HWND hWnd )
{
	HDC dc;

#ifdef _MESA
	dc = MGL_getWinDC( gm->dc );
#else
	dc = app->HDc();
#endif
	if ( dc==NULL || hWnd==NULL )
		return;

	// use offscreen bitmap
	RECT rect;
	::GetClientRect( hWnd, &rect );
	int screenW = rect.right-rect.left;
	int screenH = rect.bottom-rect.top;
	HBITMAP bitmap = ::CreateCompatibleBitmap( dc, screenW, screenH );
	HDC bitmapDC = ::CreateCompatibleDC(dc);

	::SelectObject( bitmapDC, bitmap );
	::FillRect( bitmapDC, &rect, blackBrush );

	// draw progress bar
	int barWidth = 400;
	int left = int(app->Width() / 2) - (barWidth/2);
	int top  = int(app->Height() / 2) + 80;
	int width = barWidth + 4;
	int height = 12;
	::SelectObject( bitmapDC, whitePen );
	::MoveToEx( bitmapDC, left,top, NULL );
	::LineTo( bitmapDC, left+width, top );
	::LineTo( bitmapDC, left+width, top+height );
	::LineTo( bitmapDC, left, top+height );
	::LineTo( bitmapDC, left, top );

	if ( progress>0 )
	{
		RECT rect;
		rect.left = left + 2;
		rect.top = top + 2;
		rect.right = rect.left + progress*4 + 1;
		rect.bottom = rect.top + height - 3;
		::FillRect( bitmapDC, &rect, blueBrush );
	}

	::SelectObject( bitmapDC, font );
	::SetTextColor( bitmapDC, RGB(255,255,255) );
	::SetBkColor( bitmapDC, RGB(0,0,0) );
#ifdef _USEREGISTRATION
	if ( expired )
	{
		app->Active( false );
		app->LimitFPS( true );
		size_t languageId = app->LanguageId();
		TString msg1 = STR(strEvalExpired1);
		TString msg2 = STR(strEvalExpired2);
		::TextOut( bitmapDC, int(app->Width())/2 - 150,
				   int(app->Height())/2 - 30,
				   msg1.c_str(), msg1.length() );
		::TextOut( bitmapDC, int(app->Width())/2 - 150,
				   int(app->Height())/2 - 10,
				   msg2.c_str(), msg2.length() );
	}
	else
#endif
	{
		size_t languageId = app->LanguageId();

		switch ( languageId )
		{
		case 0: // English and Dutch
		case 2:
			{
				::TextOut( bitmapDC, int(app->Width())/2 - 50,
						   int(app->Height())/2 - 30,
						   STR(strPleaseWait), LEN(strPleaseWait) );
				::TextOut( bitmapDC, int(app->Width())/2 - 50,
						   int(app->Height())/2 - 10,
						   progressMsg.c_str(), progressMsg.length() );
				break;
			}
		case 1: // German
			{
				::TextOut( bitmapDC, int(app->Width())/2 - 65,
						   int(app->Height())/2 - 30,
						   STR(strPleaseWait), LEN(strPleaseWait) );
				::TextOut( bitmapDC, int(app->Width())/2 - 65,
						   int(app->Height())/2 - 10,
						   progressMsg.c_str(), progressMsg.length() );
				break;
			}
		}
	}

	::BitBlt( dc, 0,0,screenW,screenH, bitmapDC,0,0, SRCCOPY );

	::DeleteDC(bitmapDC);
	::DeleteObject( bitmap );
};

void ReleaseMouse( void )
{
	mouseCaptured = false;
#if !defined(_VIEWER) && !defined(_EDITOR)
	::SetCursor( defaultCursor );
#endif
#if defined(_HASMENU)
	::SetMenu(app->Window(),mainMenu);
#endif
	::ReleaseCapture();
};

void DoKeyDown( HWND hWnd, WPARAM wParam )
{
#ifdef _SPEEDTEST
	if ( wParam==VK_ESCAPE )
	{
		PostQuitMessage(-1);
	}
#endif

	if ( app!=NULL )
	{
		size_t key = (size_t)wParam;
		app->KeyDown(key);
	}
	if ( wParam==VK_ESCAPE && app->ShowMenu() )
	{
		ReleaseMouse();
	}
};


void DoKeyUp( HWND hWnd, WPARAM wParam )
{
	if ( app!=NULL )
	{
		size_t key = (size_t)wParam;
		app->KeyUp(key);
	}
#if defined(_USEREGISTRATION)
	if ( expired && timeLimitCounter > 1000 )
	{
		size_t languageId = app->LanguageId();
		::MessageBox( hWnd, STR(strHasExpired2), STR(strError), MB_OK|MB_ICONERROR );
		exit(0);
	}
#endif
}


void DoKeyPress( HWND hWnd, WPARAM wParam )
{
#ifdef _TIMELIMIT
	// extra traps
	if ( expiryYear!=(secureYear-1500) ||
		 expiryMonth!=(secureMonth-100) ||
		 expiryDay!=(secureDay-50) )
	{
		exit(0);
	}
#endif
	if ( app!=NULL )
		app->KeyPress( (size_t)wParam );
};
	

void DoDestroy( HWND hWnd )
{
#ifndef _MESA
	// Deselect the current rendering context and delete it
	KillTimer(hWnd,1);

	if ( app->HDc()!=NULL )
	{
		::wglMakeCurrent(app->HDc(),NULL);
		if ( hWnd!=NULL )
			::ReleaseDC( hWnd, app->HDc() );
   			app->HDc( NULL );
	}

	if ( app->HGLRc()!=NULL )
	{
		wglDeleteContext(app->HGLRc());
		app->HGLRc( NULL );
	}
#endif
	mainMenu = NULL;

	// Tell the application to terminate after the window
	// is gone.
	::PostQuitMessage(0);
};


void DoTimer( HWND hWnd, WPARAM wParam )
{
	int timer_id = (int)wParam;

	timeLimitCounter++;
#if defined(_USEREGISTRATION)
	TString name, email;
	if ( timeLimitCounter==10 )
	if ( !registered )
	{
		size_t languageId = app->LanguageId();
		if ( IsRegistered(name,email) )
		{
			registered = true;
			registeredMsg = STR(strRegisteredTo) + name;
		}
		else
		{
			registeredMsg = STR(strUnregistered);
			expired = HasExpired();
		}
		app->RegisteredMessage( registeredMsg );
	}
#endif
#if defined(_TIMELIMIT)
	if ( (timeLimitCounter%500)==0 )
	{
		SYSTEMTIME systemTime;
		GetSystemTime( &systemTime );
		size_t expiry = expiryYear * 12 * 31 + expiryMonth * 31 + expiryDay;
		size_t today  = systemTime.wYear * 12 * 31 +
						systemTime.wMonth * 31 +
						systemTime.wDay;
		bool isOk = today < expiry;

		if ( !isOk )
		{
			exit(0);
		}
	}
#endif
#if defined(_USEREGISTRATION)
	if ( timeLimitCounter>10 && (timeLimitCounter%931)==0 )
	if ( registered )
	{
		if ( !IsRegistered(name,email) )
		{
			exit(0);
		}
	}
#endif

	if ( firstTime && app->Initialised() )
	{
		firstTime = false;
		if ( app->FullScreen() && !mouseCaptured && !app->ShowMenu() )
		{
#if !defined(_HASMENU)
			::SetCapture(hWnd);
			::SetCursor( NULL );
			::SetMenu(hWnd,NULL);
#endif
			mouseCaptured = true;
		}
	};

	if ( timer_id==1 )
	if ( app!=NULL )
	{
		app->Logic();
		if ( app->Active() )
		if ( mouseCaptured && mouseButton==0 )
		{
			RECT rect;
			::GetWindowRect( hWnd, &rect );
			int xp = (rect.left + rect.right) / 2;
			int yp = (rect.top + rect.bottom) / 2;
#if !defined(_HASMENU)
			::SetCursorPos( xp, yp );
#endif
			POINT point;
			point.x = xp;
			point.y = yp;
			::ScreenToClient(hWnd,&point);
			prevXPos = point.x;
			prevYPos = point.y;
		}

		if ( app->Finished() )
		{
#ifdef _SPEEDTEST
			size_t stfps = app->PolygonsPerSecond(fps);
			SetRegistryKey( "Software\\PDV\\Performance", "TestCompleted", stfps );
			TString rating;
			GetRating( rating );
			::KillTimer( hWnd, 1 );
			::MessageBox( hWnd, rating.c_str(), "Test Completed", MB_ICONINFORMATION | MB_OK );
#endif
			::PostQuitMessage(0);
		}
	}

#if defined(_TIMELIMIT)
	if ( (timeLimitCounter%850)==0 )
	{
		// extra traps
		if ( expiryYear!=(secureYear-1500) ||
			 expiryMonth!=(secureMonth-100) ||
			 expiryDay!=(secureDay-50) )
		{
			exit(0);
		}
	}
#endif

#if defined(_USEREGISTRATION)
	if ( timeLimitCounter>10 && (timeLimitCounter%1931)==0 )
	if ( registered )
	{
		if ( !IsRegistered(name,email) )
		{
			exit(0);
		}
	}
#endif

};


void DoMouseMove( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	if ( app->Active() )
	{
		int xPos = LOWORD(lParam);  // horizontal position of cursor
		int yPos = HIWORD(lParam);  // vertical position of cursor
		int dx = xPos - prevXPos;
		int dy = yPos - prevYPos;
		prevYPos = yPos;
		prevXPos = xPos;

#if defined(_EDITOR)
		if ( app!=NULL )
			app->MouseMove( hWnd,xPos,yPos,dx,dy );
#endif
		if ( app!=NULL )
			app->MouseMove(xPos,yPos,dx,dy);
		if ( app!=NULL )
			app->MouseMove(dx,dy);
	}
};



void DoMouseButtonDown( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
#if !defined(_EDITOR) && !defined(_PARSER)
	if ( !mouseCaptured && app->Active() && !app->ShowMenu() )
	{
#if !defined(_VIEWER) && !defined(_PARSER)
		::SetCapture(hWnd);
		::SetCursor( NULL );
		::SetMenu(hWnd,NULL);
#endif
		mouseCaptured = true;
		mouseButton = 0;
	}
	else if ( app->Active() )
#endif
	{
		if ( mouseCaptured && app->ShowMenu() )
		{
			mouseCaptured = false;
#if !defined(_EDITOR) && !defined(_VIEWER) && !defined(_PARSER)
			::SetCapture(hWnd);
			::SetCursor( defaultCursor );
#endif
		}

		int xPos = LOWORD(lParam);  // horizontal position of cursor
		int yPos = HIWORD(lParam);  // vertical position of cursor
		int dx = xPos - prevXPos;
		int dy = yPos - prevYPos;
		prevYPos = yPos;
		prevXPos = xPos;

		if ( message==WM_LBUTTONDOWN )
		{
			mouseButton = 1;
#if defined(_EDITOR)
			if ( app!=NULL )
				app->MouseDown( hWnd,1,xPos,yPos,dx,dy );
#endif
            if ( app!=NULL )
				app->MouseDown(1,xPos,yPos,dx,dy);
            if ( app!=NULL )
				app->MouseDown(1,dx,dy);
		}
		else
		{
			mouseButton = 2;
#if defined(_EDITOR)
			if ( app!=NULL )
				app->MouseDown( hWnd,2,xPos,yPos,dx,dy );
#endif
            if ( app!=NULL )
				app->MouseDown(2,xPos,yPos,dx,dy);
            if ( app!=NULL )
				app->MouseDown(2,dx,dy);
		}
	}
};


void DoMouseButtonUp( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
//	if ( mouseCaptured && app->Active() )
	{
		mouseButton = 0;
		int xPos = LOWORD(lParam);  // horizontal position of cursor
		int yPos = HIWORD(lParam);  // vertical position of cursor
		int dx = xPos - prevXPos;
		int dy = yPos - prevYPos;
		prevYPos = yPos;
		prevXPos = xPos;
		if ( message==WM_LBUTTONUP )
		{
#if defined(_EDITOR)
			if ( app!=NULL )
				app->MouseUp( hWnd,1,xPos,yPos,dx,dy );
#endif
            if ( app!=NULL )
				app->MouseUp(1,xPos,yPos,dx,dy);
            if ( app!=NULL )
				app->MouseUp(1,dx,dy);
		}
		else
		{
#if defined(_EDITOR)
			if ( app!=NULL )
				app->MouseUp( hWnd,2,xPos,yPos,dx,dy );
#endif
            if ( app!=NULL )
				app->MouseUp(2,xPos,yPos,dx,dy);
            if ( app!=NULL )
				app->MouseUp(2,dx,dy);
		}
	}
#if defined(_USEREGISTRATION)
	if ( expired && timeLimitCounter > 1000 )
	{
		size_t languageId = app->LanguageId();
		::MessageBox( hWnd, STR(strHasExpired2), STR(strError), MB_OK|MB_ICONERROR );
		exit(0);
	}
#endif
}


void DoMenu( HWND hWnd, WPARAM wParam )
{
	TString errStr;

	switch(LOWORD(wParam))
	{
		case ID_FILE_QUIT:
			{
				if ( app!=NULL )
					app->Destroy();

				::PostQuitMessage(0);
			}
			break;

		default:
			{
				if ( app!=NULL )
				{
					app->DoMenu( LOWORD(wParam) );
				}
				break;
			}
	}
};


void SetProgressMessage( const TString& msg )
{
	progressMsg = msg;
};


void SetProgress( size_t _progress )
{
	size_t fpsTimer = ::GetTickCount();
	progress = _progress;
	if ( progress>100 )
		progress = 100;
	WaitMessage( app->Window() );
};


void SetMenuItemTick( UINT menuItem )
{
#ifndef _USEGUI
	MENUITEMINFO info;
	info.cbSize = sizeof(MENUITEMINFO);
	info.fMask = MIIM_STATE;
	info.fState = MFS_CHECKED;
	::SetMenuItemInfo( mainMenu, menuItem, FALSE, &info );
	::DrawMenuBar(app->Window());
#endif
};


void ClearMenuItemTick( UINT menuItem )
{
#ifndef _USEGUI
	MENUITEMINFO info;
	info.cbSize = sizeof(MENUITEMINFO);
	info.fMask = MIIM_STATE;
	info.fState = MFS_UNCHECKED;
	::SetMenuItemInfo( mainMenu, menuItem, FALSE, &info );
	::DrawMenuBar(app->Window());
#endif
};


void SetMessage( const TString& msg )
{
	if ( app!=NULL )
		app->SetMessage( msg );
};


TMessage::TMessage( void )
{
	maxMsg = 5;
	msgIndex = 0;
	msgTimer = 0;
};


TMessage::~TMessage( void )
{
};


void TMessage::AddMessage( const char* msg )
{
	TString m = msg;
	AddMessage( m );
};


void TMessage::AddMessage( const TString& message )
{
	msgTimer = 0;
	if ( msgIndex >= maxMsg )
	{
		msgIndex = maxMsg - 1;
		for ( size_t i=0; i<(maxMsg-1); i++ )
			msg[i] = msg[i+1];
	}
	msg[msgIndex] = message;
	msgIndex++;
};


void TMessage::ProcessDisplayMessages( void )
{
	msgTimer++;
	if ( msgTimer>50 ) // about 4 seconds - depends on logic rate
	{
		msgTimer = 0;
		if ( msgIndex>0 )
			msgIndex--;

		for ( size_t i=0; i<(maxMsg-1); i++ )
			msg[i] = msg[i+1];
		msg[maxMsg-1] = "";
	}
};


void TMessage::DisplayMessages( void ) const
{
	for ( size_t i=0; i<maxMsg; i++ )
		app->Write( 160, float(app->Height()-(80+i*14)), -10, msg[i] );
};

//==========================================================================

