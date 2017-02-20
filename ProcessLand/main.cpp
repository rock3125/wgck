#include <precomp_header.h>

#ifdef _MESA
#include <win32MESA/events.h>
#else
#include <win32/events.h>
#include <win32/win32.h>
#endif

#include <object/geometry.h>
#include <object/object.h>
#include <object/landscapeObject.h>


#define kPI 3.1415927f

//==========================================================================
//
//	Example app object created, will receive all events from TEvent
//

class TApp : public TEvent
{
public:
	TApp( void );
    ~TApp( void );

	bool StartupSystem( char* errStr );

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

    TLandscapeObject	obj;
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
    	strcpy( app1->fileName, "land.lnd" );
        strcpy( app1->path, "data" );
		strcpy( app1->texturePath, "data\\textures" );
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
};

//==========================================================================

TApp::~TApp( void )
{
};

//==========================================================================

bool TApp::StartupSystem( char* errStr )
{
	if ( !TEvent::StartupSystem( errStr ) )
		return false;

	ConsolePrint( "Use cursor keys, A, Z, and the mouse to view" );
	ConsolePrint( "Use ALT+C to show/hide console window" );
	ConsolePrint( "ALT+Enter to switch full screen & back" );
/*
	TObject tempObj;
	ConsolePrint( "Converting object... please wait..." );
	if ( !tempObj.LoadBinary( "land.bin", errStr, "data", "data\\textures") )
    	return false;
	tempObj.Information();

	obj.DivideMeshes( tempObj, 64, 64 );

	obj.MaterialList( tempObj.MaterialList() );
	tempObj.MaterialList(NULL);

	obj.SaveBinary("data\\land.lnd");
*/
	if ( !obj.LoadBinary( fileName, errStr, path, texturePath ) )
		return false;
	obj.Information();
	obj.RenderDepth( 12 );

	ly = -obj.MaxY();

    return true;
};

//==========================================================================

void TApp::Logic( void )
{
	TEvent::Logic();

	xAngle += deltaXAngle;
	yAngle += deltaYAngle;
	deltaXAngle = 0.0f;
	deltaYAngle = 0.0f;

    // movement
    float yangle = float(( (yAngle) / 180.0f) * kPI );
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
		ly = ly - speed/4;
	if ( keys & 32 )
		ly = ly + speed/4;
};

//==========================================================================

void TApp::Render( void )
{
	//
	//	NB. order is vital in the following set of code
	//
	//-------------------------------------------------

	float cx,cy,cz;
	obj.GetCenter( cx, cy, cz );

	glLoadIdentity();
	glRotatef( xAngle,1,0,0);
	glRotatef( yAngle,0,1,0);

    // translate and draw landscape
    glPushMatrix();
	    glTranslatef( lx-cx,ly-cy,lz-cz );
		obj.Draw(lx,lz,showBoundingBox);
    glPopMatrix();

	glPushMatrix();
//		tank1.CurrentLocation( obj.FindTriangle(tank1.CurrentLocation(),tank1.X(),tank1.Y(),tank1.Z()) );
//		glTranslatef( tank1.X()-lx, ly-tank1.Y(), lz-tank1.Z() );
//		tank1.Draw();
	glPopMatrix();
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
	}
};

//==========================================================================

