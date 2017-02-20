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

void TApp::RenderGameMenu( void )
{
	showMenu = true;

	SetupGL2d( vdepth, vwidth, vheight );
	glLoadIdentity();
	glEnable( GL_COLOR_MATERIAL );
	glColor3ub( 255,255,255 );

	if ( showStory )
	{
		stories.Draw( this );
	}
	else if ( showAbout )
	{
		float scale = 80 * (vwidth/640);
		if ( about!=NULL )
		{
			glPushMatrix();
			glTranslatef( 0,0, -(scale+10)*2 );
			about->StartTexture();
			about->Draw( vwidth,0,0,vheight );
			about->EndTexture();
			glPopMatrix();
		}
		if ( aboutLogo!=NULL )
		{
			glPushMatrix();
			glTranslatef( scale+30,vheight-(scale+20), -(scale+10) );
			glRotatef( 180 + aboutAngle, 0,1,0 );
			aboutLogo->Draw(scale);
			glPopMatrix();
		}
		RenderMenu( false );
	}
	else if ( showDownload )
	{
		float scale = 80 * (vwidth/640);
		if ( downloadScreen!=NULL )
		{
			glPushMatrix();
			glTranslatef( 0,0, -(scale+10)*2 );
			downloadScreen->StartTexture();
			downloadScreen->Draw( vwidth,0,0,vheight );
			downloadScreen->EndTexture();
			glPopMatrix();
		}
		RenderMenu( false );
	}
	else
	{
		// draw background bitmap
		if ( mainMenuBackground!=NULL )
		{
			glPushMatrix();
			glTranslatef( 0,0, -4 );
			mainMenuBackground->Draw( vwidth,0, 0, vheight );
			glPopMatrix();
		}

		// draw 3D text
		if ( title!=NULL )
		{
			glPushMatrix();

			float z = -3; // -390 + animCount*2;
			glTranslatef( vwidth*0.5f,vheight*0.85f, z );

			float scale = animCount*10;
			glScalef( scale,scale,scale );

			title->Draw();
			glPopMatrix();
		}

		glClear(GL_DEPTH_BUFFER_BIT);
		RenderMenu();

		msg->DisplayMessages();
	}
}

//==========================================================================

