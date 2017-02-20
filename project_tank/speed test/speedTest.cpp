#include <precomp_header.h>

#include <time.h>

#include <PerformanceWin32/events.h>
#include <PerformanceWin32/win32.h>

#include <common/sounds.h>

#include <object/object.h>
#include <object/landscape.h>
#include <object/geometry.h>
#include <object/tank.h>
#include <object/anim.h>

#include <performanceWin32/resource.h>

//==========================================================================

#include <mmsystem.h>

extern bool playMusic; // flag forces music on/off

//==========================================================================

const float kPI = 3.1415927f;
const float degToRad = 0.01745329252033f;
const float radToDeg = 57.295779511273f;

//==========================================================================
//
//	Example app object created, will receive all events from TEvent
//

class TApp : public TEvent
{
public:
	TApp( void );
    ~TApp( void );

	bool StartupSystem( HWND hwnd, TString& errStr );
	void Destroy( void );

	void Render( void );
	void Logic( void );

	void MouseDown( int button, int x, int y);
	void MouseUp( int button, int, int );
	void MouseMove( int x, int y, bool ctrlDown );
	void KeyDown( int kd );
	void KeyUp( int ku );

public:
    TString		fileName;
    TString		path;
	TString		texturePath;
    
private:
	bool		showBoundingBox;
    bool 		leftDown;
	bool		rightDown;

    float		yAngle;
    float		xAngle;
	float		deltaXAngle;
	float		deltaYAngle;

	size_t		currentRenderDepth;
	int			cameraLocation;
	float		radius;

    float		lx;
    float		ly;
    float		lz;

	int			keys;

	bool		ctrlDown;

	TBinObject	spaceStation;
	TTexture*	marsTexture;
	TTexture	credits;

	float		offsetx,offsety;
	float		turnAngle1;

	bool		mp3playing;

	size_t		currentTime;
};

//==========================================================================
//
//	All my apps get this function called first for creating the initial setup
//
bool InitialiseStartup( const TString& cmdLine, const TString& texturePath,
					    HINSTANCE hInst, TEvent*& app, TString& appName, 
						TString& appTitle, TString& errStr )
{
	errStr = "";

	TApp* app1 = new TApp();
    PostCond( app1!=NULL );

	app1->texturePath = texturePath;
    app1->fileName = cmdLine;
    if ( app1->fileName.length()>0 )
    {
    	// try and split path from filename
        int index = app1->fileName.length();
        while ( index>0 && app1->fileName[index]!='\\' ) index--;
        if ( app1->fileName[index]=='\\' )
        {
        	app1->fileName[index] = 0;
            app1->path = app1->fileName;
            TString temp;
            temp = &app1->fileName[index+1];
            app1->fileName = temp;
        }
        else
        	app1->path = "";
    }
    else
    {
    	app1->fileName = "station.bin";
        app1->path = "data";
		app1->texturePath = "data\\textures";
    }
    app = app1;

    appName = "PerformanceTest";
    appTitle = "Performance Test";

	return true;
};

//==========================================================================

TApp::TApp( void )
	: marsTexture(NULL)
{
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
};



TApp::~TApp( void )
{
}


void TApp::Destroy( void )
{
	soundSystem->Stop(0);
};


bool TApp::StartupSystem( HWND _hwnd, TString& errStr )
{
	if ( !TEvent::StartupSystem( _hwnd, errStr ) )
		return false;

	srand( (unsigned)GetTickCount() );

	ConsolePrint( "Use cursor keys, A, Z, and the mouse to view" );
	ConsolePrint( "Use ALT+C to show/hide console window" );
	ConsolePrint( "ALT+Enter to switch full screen & back\n" );

	int index = fileName.length()-4;
	if ( index>0 )
	{
		if ( marsTexture==NULL )
		{
			ConsolePrint( "Loading objects" );
			if ( !spaceStation.LoadBinary( fileName, errStr, path, texturePath ) )
		    	return false;

			numberOfPolygons = spaceStation.NumSurfaces();

			spaceStation.Information();
			currentRenderDepth = renderDepth;
			yAngle = 0;
			marsTexture = new TTexture();

			if ( rand() > (RAND_MAX/2) )
			{
				if ( !marsTexture->LoadBinary("universe1.jpg", path, errStr ) )
				{
					return false;
				}
			}
			else
			{
				if ( !marsTexture->LoadBinary("universe2.jpg", path, errStr ) )
				{
					return false;
				}
			}

			if ( !credits.LoadRGBAJPEG( IDR_SPACESTATION, errStr, 16 ) )
			{
				return false;
			}

			currentTime = ::GetTickCount();
		}
	}

#ifdef _DXSOUND_
	soundSystem = new TSound( _hwnd );
	PostCond( soundSystem!=NULL );
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
				PlaySound( "data\\mystic.wav", NULL, SND_ASYNC );
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
	glLoadIdentity();
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



void TApp::MouseMove( int x, int y, bool _ctrlDown )
{
	ctrlDown = _ctrlDown;
	deltaYAngle += float(x)*0.1f;
	deltaXAngle += float(y)*0.1f;
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

//==========================================================================

