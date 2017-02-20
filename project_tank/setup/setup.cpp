#include <precomp_header.h>

#include <win32/events.h>
#include <win32/win32.h>

#include <common/sounds.h>

#include <object/object.h>
#include <object/landscape.h>
#include <object/geometry.h>
#include <object/tank.h>
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

	bool StartupSystem( HWND hwnd, char* errStr );

	void Render( void );
	void Logic( void );

	void MouseDown( int button, int x, int y);
	void MouseUp( int button, int, int );
	void MouseMove( int x, int y, bool ctrlDown );
	void KeyDown( int kd );
	void KeyUp( int ku );

public:
    char		fileName[256];
    char		path[256];
	char		texturePath[256];
    
private:
	bool		showBoundingBox;
    bool 		leftDown;
	bool		rightDown;
	bool		ctrlDown;

    float		yAngle;
    float		xAngle;
	float		deltaXAngle;
	float		deltaYAngle;

	size_t		currentRenderDepth;

    float		lx;
    float		ly;
    float		lz;

	int			keys;
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
        strcpy( app1->path, "demoData" );
		strcpy( app1->texturePath, "demoData" );
    }
    app = app1;

    strcpy( appName, "Setup" );
    sprintf( appTitle, "Setup" );

	return true;
};

//==========================================================================

TApp::TApp( void )
{
};



TApp::~TApp( void )
{
}


bool TApp::StartupSystem( HWND _hwnd, char* errStr )
{
	if ( !TEvent::StartupSystem( _hwnd, errStr ) )
		return false;

    return true;
};



void TApp::Logic( void )
{
	TEvent::Logic();
};



void TApp::Render( void )
{
	glLoadIdentity();
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

