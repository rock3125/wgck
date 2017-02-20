#include <precomp_header.h>

#include <direct.h>
#include <process.h>
#include <shellapi.h>

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

#include <network/packet.h>

#include <io.h>

//==========================================================================

#include <mmsystem.h>

//==========================================================================

const float Deg2Rad = 0.01745329252f;
const float Rad2Deg = 57.295779511273f;

//==========================================================================

void TApp::LogicGameMenu( void )
{
	if ( currentPage==mainMenu )
	{
		if ( singlePlayerButton!=NULL )
		if ( singlePlayerButton->Clicked() )
		{
			DoMenu( IDM_SINGLEPLAYER );
		}
		if ( hostButton!=NULL )
		if ( hostButton->Clicked() )
		{
			DoMenu( IDM_HOST );
		}
		if ( joinButton!=NULL )
		if ( joinButton->Clicked() )
		{
			DoMenu( IDM_JOIN );
		}
		if ( controlsButton!=NULL )
		if ( controlsButton->Clicked() )
		{
			DoMenu( ID_HELP_CONTROLS );
		}
		if ( aboutButton!=NULL )
		if ( aboutButton->Clicked() )
		{
			DoMenu( IDM_ABOUT );
		}
		if ( downloadButton!=NULL )
		if ( downloadButton->Clicked() )
		{
			DoMenu( IDM_DOWNLOAD );
		}
		if ( videoSettingsButton!=NULL )
		if ( videoSettingsButton->Clicked() )
		{
			DoMenu( IDM_VIDEOSETTINGS );
		}
		if ( helpButton!=NULL )
		if ( helpButton->Clicked() )
		{
			DoMenu( IDM_HELP );
		}
		keys = 0;
	}

	if ( animCount<30 )
		animCount++;
}

//==========================================================================
