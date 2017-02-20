#include <precomp_header.h>

#ifdef _MESA

//==========================================================================

#include <win32/win32.h>
#include <tank/tankApp.h>

//==========================================================================

size_t fpsTimer2 = 0;

//==========================================================================

GMDC		*gm;
GM_modeInfo	modeInfo;

GM_driverOptions driverOpt = 
	{
		true,			/* UseWinDirect		*/
		true,			/* UseDirectDraw	*/
		true,			/* UseVGA			*/
		true,			/* UseVGAX			*/
		true,			/* UseVBE			*/
		true,			/* UseVBEAF			*/
		true,			/* UseLinear		*/
		true,			/* UseFullscreenDIB	*/
		true,			/* UseHWOpenGL		*/
		MGL_GL_AUTO,	/* OpenGLType		*/
		GM_MODE_ALLBPP,	/* modeFlags		*/
	};

//==========================================================================

void ExitFunc( void )
{
	ReleaseResources();
};

void ActivateFunc( int activate )
{
	DoActivate( mainWindow, (activate==MGL_DEACTIVATE)?WA_INACTIVE:WA_ACTIVE );
}

void PaintFunc( void )
{
	DoPaint( mainWindow );
};

void TimerFunc( void )
{
	size_t time = LZTimerLap() / 1000;
	if ( app->LimitFPS() )
	{
		if ( time > fpsTimer2 )
		{
			fpsTimer2 = time + 66; // 15 fps for now
			DoTimer( mainWindow, 1 );
			WriteString( "%d\n", time );
		}
	}
	else
	{
		DoTimer( mainWindow, 1 );
	}
};

bool reinit( TString& errStr )
{
	int	flags = MGL_GL_DEPTH | MGL_GL_DOUBLE | MGL_GL_RGB | MGL_GL_ALPHA;

	if (!GM_startOpenGL((MGL_glContextFlagsType)flags))
	{
		WriteString(MGL_errorMsg(MGL_result()));
		GM_exit();
	}
	::SetWindowText(gm->mainWindow, STR(strWGI));

    if ( !app->Setup( gm->mainWindow, 450, 640, 480, errStr ) )
	{
		WriteString( errStr.c_str() );
		return false;
	}
	return true;
}

int APIENTRY WinMain( HINSTANCE hInst,
			   		  HINSTANCE hPrevInstance,
			   		  LPSTR lpCmdLine,
			   		  int nCmdShow )
{
	GM_setDriverOptions(&driverOpt);
	ZTimerInit();
	LZTimerOn();
	if ((gm = GM_init("WarGamesInc")) == NULL)
	{
		WriteString(MGL_errorMsg(MGL_result()));
		return -1;
	}

	PreInit(hInst);

	GM_init(STR(strWGI));
	GM_initWindowPos( 0, 0 );

	GM_setExitFunc( ExitFunc );
	GM_setDrawFunc( PaintFunc );
	GM_setGameLogicFunc( TimerFunc );

	GM_registerEventProc( WndProc );

	if ( GM_findMode( &modeInfo,640,480,32) )
	{
			int startWindowed = true;

			GM_chooseMode(&modeInfo,&startWindowed);
			if ( !GM_setMode(&modeInfo,startWindowed,2,false) )
			{
				WriteString(MGL_errorMsg(MGL_result()));
				return -1;
			}

			TString errStr;
			if ( !reinit(errStr) )
			{
				WriteString(MGL_errorMsg(MGL_result()));
				return -1;
			}

			mainWindow = gm->mainWindow;
			PostCond( mainWindow!=NULL );
			app->Window( mainWindow );

			if ( !PostInit() )
			{
				ExitFunc();
				return -1;
			}
			else
			{
				LZTimerOn();
				GM_mainLoop();
			}
	}
	return 0;
}

#endif


