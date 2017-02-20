#include <precomp_header.h>

#include <win32/win32.h>
#include <tank/tankapp.h>

//==========================================================================
//
// Entry point of all Windows programs
//
#ifndef _MESA

int APIENTRY WinMain( HINSTANCE hInst,
			   		  HINSTANCE hPrevInstance,
			   		  LPSTR lpCmdLine,
			   		  int nCmdShow )
{
	PreInit(hInst);

	// create app
	TApp* app = new TApp( hInst, RegisteredMessage() );
    PostCond( app!=NULL );
	SetApp( app );

	if ( !PostInit() )
	{
		return -1;
	}
	return StartGameSystem();
}

#endif

//==========================================================================
