#include <precomp_header.h>

#ifdef _MESA
#include <win32MESA/events.h>
#else
#include <viewerWin32/events.h>
#include <viewerWin32/win32.h>
#endif

#include <object/object.h>
#include <object/landscape.h>
#include <object/geometry.h>
#include <object/anim.h>

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

	bool StartupSystem( HWND, char* errStr );

	void Render( void );
	void Logic( void );

	void MouseDown( int button, int x, int y);
	void MouseUp( int button, int, int );
	void MouseMove( int x, int y );
	void KeyDown( int kd );
	void KeyUp( int ku );

public:
    char		fileName[256];
    char		path[256];
	char		texturePath[256];
    
private:
	bool		showBoundingBox;
    bool 		leftDown;
    int 		startX;
    int			startY;

    float		yAngle;
    float		xAngle;
	float		deltaXAngle;
	float		deltaYAngle;

    float		lx;
    float		ly;
    float		lz;

	int			keys;
	bool		landscape;
	bool		animation;

	TObject				obj;
	TLandscape			landObj;
	TAnimation			animationObj;
	size_t				currentLocation;
};

//==========================================================================
//
//	All my apps get this function called first for creating the initial setup
//
bool InitialiseStartup( const char* cmdLine, const char* texturePath,
					    HINSTANCE hInst, TEvent*& app, char* appName, 
						char* appTitle, char* errStr )
{
	errStr[0] = 0;

	TApp* app1 = new TApp();
    PostCond( app1!=NULL );

	strcpy( app1->texturePath, texturePath );
    strcpy( app1->fileName, cmdLine );
    if ( app1->fileName[0]!=0 )
    {
    	// try and split path from filename
        int index = strlen(app1->fileName);
        while ( index>0 && app1->fileName[index]!='\\' ) index--;
        if ( app1->fileName[index]=='\\' )
        {
        	app1->fileName[index] = 0;
            strcpy( app1->path, app1->fileName );
            char temp[256];
            strcpy( temp, &app1->fileName[index+1] );
            strcpy( app1->fileName, temp );
        }
        else
        	app1->path[0] = 0;
    }
    else
    {
    	strcpy( app1->fileName, "ds9.bin" );
        strcpy( app1->path, "demodata" );
		strcpy( app1->texturePath, "demodata" );
    }
    app = app1;

    strcpy( appName, "GLViewer" );
    sprintf( appTitle, "OpenGL Viewer - %s", app1->fileName );

	return true;
};

//==========================================================================

TApp::TApp( void )
{
	yAngle = 0.0f;
	xAngle = 0.0f;
	deltaXAngle = 0.0f;
	deltaYAngle = 0.0f;
    lx = 0.0f;
    ly = 0.0f;
    lz = 0.0f;
	leftDown = false;
	showBoundingBox = false;
	keys = 0;
	currentLocation = NULL;
	landscape = false;
	animation = false;
};

//==========================================================================

TApp::~TApp( void )
{
};

//==========================================================================

bool TApp::StartupSystem( HWND, char* errStr )
{
	if ( !TEvent::StartupSystem( NULL, errStr ) )
		return false;

	ConsolePrint( "Use cursor keys, A, Z, and the mouse to view" );
	ConsolePrint( "Use ALT+C to show/hide console window" );
	ConsolePrint( "ALT+Enter to switch full screen & back" );

	int index = strlen(fileName)-4;
	if ( index>0 )
	{
		if ( stricmp(&fileName[index],".lnd")==0 )
		{
			landscape = true;
			if ( !landObj.Loaded() )
			{
				ConsolePrint( "Loading landscape object" );
				if ( !landObj.LoadBinary( fileName, errStr, path, texturePath ) )
			    	return false;
				landObj.Information();
				landObj.RenderDepth(renderDepth);
			    ly = -landObj.MaxY();
			}
		}
		else if ( stricmp(&fileName[index],".anm")==0 )
		{
			animation = true;
			ConsolePrint( "Loading animation object" );
			if ( !animationObj.LoadBinary( fileName, errStr, path, texturePath ) )
		    	return false;
			animationObj.StartAnimation( 0, 20 );
		}
		else
		{
			if ( !obj.Loaded() )
			{
				ConsolePrint( "Loading object" );
				if ( !obj.LoadBinary( fileName, errStr, path, texturePath ) )
			    	return false;
				obj.Information();
			}
		}
	}
    return true;
};

//==========================================================================

void TApp::Logic( void )
{
	TEvent::Logic();

	xAngle += deltaXAngle;
	if ( landscape )
	{
		if ( xAngle > 80.0f ) xAngle = 80.0f;
		if ( xAngle < -80.0f ) xAngle = -80.0f;
	};

	yAngle += deltaYAngle;
	deltaXAngle = 0.0f;
	deltaYAngle = 0.0f;
	if ( yAngle>360.0f ) yAngle -= 360.0f;
	if ( yAngle<0.0f ) yAngle += 360.0f;

    // movement
    float yangle = yAngle*degToRad;
	float v1fcos = (float)cos(yangle);
	float v1fsin = (float)sin(yangle);

	float v2fcos = (float)cos(yangle + 0.5f*kPI );
	float v2fsin = (float)sin(yangle + 0.5f*kPI );

	float v3fcos = (float)cos(yangle - 0.5f*kPI );
	float v3fsin = (float)sin(yangle - 0.5f*kPI );

	// key codes
	float speed = 1.0f;
	if ( keys & 4 )
	{
		lz = lz + v1fcos*speed;
		lx = lx - v1fsin*speed;
	}
	if ( keys & 8 )
	{
		lz = lz - v1fcos*speed;
		lx = lx + v1fsin*speed;
	}
	if ( keys & 1 )
	{
		lz = lz - v2fcos*speed;
		lx = lx + v2fsin*speed;
	}
	if ( keys & 2 )
	{
		lz = lz - v3fcos*speed;
		lx = lx + v3fsin*speed;
	}
	if ( keys & 16 )
		ly = ly - speed*0.25f;
	if ( keys & 32 )
		ly = ly + speed*0.25f;
};

//==========================================================================

void TApp::Render( void )
{
    glLoadIdentity();
	
	if ( landscape && landObj.Initialised() )
	{
		glRotatef( xAngle, 1,0,0 );
		glRotatef( yAngle, 0,1,0 );
		glTranslatef( lx,ly,lz );

		landObj.Draw(lx,ly,lz,yAngle,xAngle,0,0,showBoundingBox);
	}
	else if ( animation )
	{
		glRotatef( xAngle, 1,0,0 );
		glRotatef( yAngle, 0,1,0 );
		glTranslatef( lx,ly,lz );

		if ( animationObj.AnimationPaused() )
		{
			char msg[256];
			sprintf( msg, "frame %d", animationObj.AnimationIndex() );
			SetMessage( msg );
		}
		animationObj.Draw();
	}
	else if ( !landscape && !animation )
	{
		glRotatef( xAngle, 1,0,0 );
	    glRotatef( yAngle, 0,1,0 );
	    
		glTranslatef( lx,ly,lz );
	    obj.Draw(showBoundingBox);

		char msg[256];
		sprintf( msg, "%2.2f, %2.2f, %2.2f", lx,ly,lz );
		SetMessage(msg);
	}
};

//==========================================================================

void TApp::MouseDown( int button, int x, int y)
{
	if ( button==1 )
    {
    	leftDown = true;
        startX = x;
        startY = y;
    }
};

//==========================================================================

void TApp::MouseUp( int button, int, int )
{
	if ( button==1 )
    	leftDown = false;
};

//==========================================================================

void TApp::MouseMove( int x, int y )
{
	if ( leftDown )
    {
        int deltaX = x - startX;
        int deltaY = y - startY;
        startX = x;
        startY = y;

        deltaYAngle += float(deltaX);
        deltaXAngle += float(deltaY);
    }
};

//==========================================================================

void TApp::KeyDown( int kd )
{
	TEvent::KeyDown( kd );
	switch ( kd )
	{
		case VK_LEFT:
			keys = keys | 1;
			break;

		case VK_RIGHT:
			keys = keys | 2;
			break;

		case VK_UP:
			keys = keys | 4;
			break;

		case VK_DOWN:
			keys = keys | 8;
			break;

		case 'a':
		case 'A':
			keys = keys | 16;
			break;

		case 'z':
		case 'Z':
			keys = keys | 32;
			break;

		case 'b':
		case 'B':
			showBoundingBox = !showBoundingBox;
			break;
	}
};

//==========================================================================

void TApp::KeyUp( int ku )
{
	TEvent::KeyUp( ku );
	switch ( ku )
	{
		case VK_LEFT:
			keys = keys & ~1;
			break;

		case VK_RIGHT:
			keys = keys & ~2;
			break;

		case VK_UP:
			keys = keys & ~4;
			break;

		case VK_DOWN:
			keys = keys & ~8;
			break;

		case 'a':
		case 'A':
			keys = keys & ~16;
			break;

		case 'z':
		case 'Z':
			keys = keys & ~32;
			break;

		case 's':
		case 'S':
			{
				animationObj.AnimationType(1);
				animationObj.PauseAnimation(true);
				animationObj.PreviousAnimation();
			}
			break;

		case 'd':
		case 'D':
			{
				animationObj.AnimationType(1);
				animationObj.PauseAnimation(true);
				animationObj.NextAnimation();
			}
			break;

		case ' ':
			{
				animationObj.PauseAnimation(false);
				animationObj.StartAnimation();
			}
			break;
	}
};

//==========================================================================

