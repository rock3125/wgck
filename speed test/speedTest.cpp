#include <precomp_header.h>

#include <time.h>

#include <speed test/speedtest.h>
#include <Win32/win32.h>

#include <common/sounds.h>

#include <object/binobject.h>
#include <object/landscape.h>
#include <object/geometry.h>
#include <object/tank.h>
#include <object/anim.h>

#include <Win32/resource.h>

//==========================================================================

#include <mmsystem.h>

//==========================================================================

const float kPI = 3.1415927f;
const float degToRad = 0.01745329252033f;
const float radToDeg = 57.295779511273f;

//==========================================================================

TApp::TApp( HINSTANCE hInstance )
	: marsTexture(NULL),
	  TEvent( hInstance )
{
	fog = false;

	versionString = "Excession Engine Performance Test 1.04";

	yAngle = 180.0f;
	xAngle = 0.0f;
	deltaXAngle = 0.0f;
	deltaYAngle = 0.0f;
    lx = -23.0f;
    ly = 0.0f;
    lz = -142.0f;
	leftDown = false;
	rightDown = false;
	ctrlDown = false;
	showBoundingBox = false;
	keys = 0;
	radius = 15.0f;
	cameraLocation = -1;
	offsetx = 0;
	offsety = 0;
	mp3playing = false;
	turnAngle1 = 0;

	limitFPS = false;
	kFPS = 15; // logic rate

	doSpeedTest = false;
	size_t stfps = 0;
	SetRegistryKey( "Software\\PDV\\Performance", "TestCompleted", stfps );
};



TApp::~TApp( void )
{
}


size_t TApp::PolygonsPerSecond( float fps ) const
{
	float pps = fps * float(numberOfPolygons);
	return size_t(pps);
};


void TApp::Destroy( void )
{
	if ( soundSystem!=NULL )
		soundSystem->Stop(0);
};


bool TApp::StartupSystem( TString& errStr )
{
	if ( !TEvent::StartupSystem( errStr ) )
		return false;

	srand( (unsigned)GetTickCount() );

	{
		if ( marsTexture==NULL )
		{
			WriteString( "Loading objects\n" );
			if ( !spaceStation.LoadBinary( "station.bin", errStr, "data", "data\\textures" ) )
		    	return false;

			numberOfPolygons = spaceStation.NumSurfaces();

			currentRenderDepth = renderDepth;
			yAngle = 0;
			marsTexture = new TTexture();

			if ( rand() > (RAND_MAX/2) )
			{
				if ( !marsTexture->LoadBinary("universe1.jpg", "data\\textures", errStr ) )
				{
					return false;
				}
			}
			else
			{
				if ( !marsTexture->LoadBinary("universe2.jpg", "data\\textures", errStr ) )
				{
					return false;
				}
			}

			if ( !credits.LoadRGBAJPEG( hInstance, IDR_SPACESTATION, errStr, 0,0,0, 0, 75, 32 ) )
			{
				return false;
			}

			currentTime = ::GetTickCount();
		}
	}

#ifdef _DXSOUND_
//	soundSystem = new TSound( _hwnd );
//	PostCond( soundSystem!=NULL );
#endif
    return true;
};



void TApp::Logic( void )
{
	TEvent::Logic();

	long time = ::GetTickCount();

	// run for 30 seconds
	if ( marsTexture!=NULL )
	{
		if ( (time-currentTime) > 40000 )
		{
			finished = true;
			if ( playMusic )
				PlaySound( NULL, NULL, SND_ASYNC );
		}

		if ( !finished && playMusic )
		{
			if ( !mp3playing )
			{
				mp3playing = true;
				PlaySound( "data\\audio\\mystic.wav", NULL, SND_ASYNC );
			}
		}
	}

	yAngle += 1.0f;
	offsetx += 0.00015f;
	offsety += 0.0004f;
	turnAngle1 += 1.0f;
};


void TApp::Render( void )
{
/*
	SetupGL2d( vdepth, vwidth, vheight );
	glLoadIdentity();

	glTranslatef( 200,200,-50 );
	glRotatef( -90.0f, 1,0,0 ); // re-orient object up
	glRotatef( 90, 0,0,1 ); // rotate object to front
	glScalef( 3,3,3 );
	aeonFlux.Draw();
*/
	if ( !finished )
	{
		if ( marsTexture!=NULL )
		{

			glPushMatrix();
				glTranslatef( 0,0,-6 );
				glRotatef( 180, 0,1,0 );

				float w = 12; 
				float h = 12;
					glEnable(GL_COLOR_MATERIAL);
				glColor3ub(255,255,255);
				glScalef( 0.85f-offsetx, 0.85f-offsetx, 0.85f-offsetx );
				marsTexture->Draw( -w*0.5f,-h*0.5f, w*0.5f,h*0.5f );
			glPopMatrix();

			glTranslatef( offsetx*10.0f,0,-3 );
			glRotatef( yAngle,0,1,0);
			glRotatef( turnAngle1, 0,0,1 );
			glScalef( 1+offsety,1+offsety,1+offsety );
			spaceStation.Draw();

			// draw credits for Rob
			float ysize = 0.0375f;
			float xsize = 0.15f;
			glLoadIdentity();
			glTranslatef( 0.70f,-0.60f,-1.1f );
			glRotatef( 180, 0,1,0 );
			glEnable( GL_BLEND );
			credits.Draw(-xsize,-ysize,xsize,ysize);
			glDisable( GL_BLEND );
		}
	}
};



void TApp::MouseDown( int button, int x, int y)
{
	if ( button==1 )
    {
    	leftDown = true;
    }
	if ( button==2 )
    {
    	rightDown = true;
    }

	if ( leftDown )
		keys = keys | 16;
};



void TApp::MouseUp( int button, int, int )
{
	if ( button==1 )
    	leftDown = false;
	if ( button==2 )
    	rightDown = false;

	if ( !leftDown )
		keys = keys & ~16;
};



void TApp::MouseMove( int x, int y )
{
	deltaYAngle += float(x)*0.1f;
	deltaXAngle += float(y)*0.1f;
};


void TApp::MouseDown( int button, int x, int y, int dx, int dy )
{
};


void TApp::MouseUp( int button, int, int, int, int )
{
};


void TApp::MouseMove( int x, int y, int, int )
{
};


void TApp::KeyDown( int kd )
{
	TEvent::KeyDown( kd );
	switch ( kd )
	{
		case VK_UP:
			keys = keys | 1;
			break;

		case VK_DOWN:
			keys = keys | 2;
			break;

		case VK_LEFT:
			keys = keys | 4;
			break;

		case VK_RIGHT:
			keys = keys | 8;
			break;

		case ' ':
			keys = keys | 16;
			break;

		case 'b':
		case 'B':
			showBoundingBox = !showBoundingBox;
			break;
	}
};



void TApp::KeyUp( int ku )
{
	TEvent::KeyUp( ku );
	switch ( ku )
	{
		case VK_UP:
			keys = keys & ~1;
			break;

		case VK_DOWN:
			keys = keys & ~2;
			break;

		case VK_LEFT:
			keys = keys & ~4;
			break;

		case VK_RIGHT:
			keys = keys & ~8;
			break;

		case ' ':
			keys = keys & ~16;
			break;
	}
};


TFont& TApp::Arial( void )
{
	return arial;
};

bool TApp::Initialised( void ) const
{
	return initialised;
};

void TApp::Initialised( bool i )
{
	initialised = i;
};

// try and read resolution settings from registry
bool TApp::GetRegistrySettings( void )
{
	TEvent::GetRegistrySettings();

	size_t currMusic;
	if ( GetRegistryKey( "Software\\PDV\\Performance", "PlayMusic", currMusic ) )
		if ( currMusic==1 )
			playMusic = true;
		else
			playMusic = false;

	return true;
};


TCompoundObject* TApp::GetObjectByName( const TString& name )
{
	return NULL;
};

void TApp::AddTransport( TLandscapeObject )
{
};

//==========================================================================

