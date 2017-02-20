#include <precomp_header.h>

#include <win32/win32.h>
#include <win32/events.h>
#include <common/sounds.h>

#include <object/object.h>
#include <landscape/landscape.h>
#include <landscape/main.h>

//==========================================================================

const int kKeyRight	= 1;
const int kKeyLeft	= 2;
const int kKeyUp	= 4;
const int kKeyDown	= 8;
const double kPI	= 3.14159265;

//==========================================================================
//
//	Example app object created, will receive all events from TEvent
//

class TApp : public TEvent
{
public:
	TApp( char* errStr=NULL, bool fog = false );
    ~TApp( void );

    bool Setup( HWND _hwnd, float depth, float widht, float height, char* errStr );
    void Destroy( void );
	void Render( void );
	void Logic( void );

	void KeyUp( int kp );
	void KeyDown( int kp );
	void MouseDown( int button, int x, int y);
	void MouseUp( int button, int, int );
	void MouseMove( int x, int y );

private:
	void Movement( float yAngle, float& dirX, float& dirZ );

private:
    bool 		leftDown;
    bool		rightDown;
	bool		showBoundingBoxes;
    int 		startX;
    int			startY;
	float		deltaXAngle;
	float		deltaYAngle;
    int			keys;
    bool		fog;

#ifdef _DXSOUND_
    TSound*		soundSystem;
    int			soundCounter;
    bool		firstTime;
#endif
    TLandscape	landscape;

    TObject		sherman;
	bool		shermanMoveFwd;
	bool		shermanMoveBwd;
	bool		shermanLeft;
	bool		shermanRight;

    TObject		tiger;
    TVector		camera;
};

//==========================================================================
//
//	All my apps get this function called first for creating the initial setup
//

bool InitialiseStartup( char const*, char const*, HINSTANCE hInst, 
					    TEvent*& app, char* appName, char* appTitle, 
						char* errStr )
{
	errStr[0] = 0;

	app = new TApp( errStr );
    PostCond( app!=NULL );

    strcpy( appName, "GLLandscape" );
    strcpy( appTitle, "OpenGL Landscape Test" );

    return true;
};

//==========================================================================

TApp::TApp( char* errStr, bool _fog )
	: TEvent( errStr, _fog ),
#ifdef _DXSOUND_
      soundCounter(0),
      firstTime(true),
#endif
      fog( _fog )
{
	deltaXAngle = 0.0f;
	deltaYAngle = 0.0f;
    keys = 0;
    leftDown = false;
    rightDown = false;
	showBoundingBoxes = false;

	shermanMoveFwd = false;
	shermanMoveBwd = false;
	shermanLeft = false;
	shermanRight = false;
};

//==========================================================================

TApp::~TApp( void )
{
};

//==========================================================================

bool TApp::Setup( HWND _hwnd, float depth, float width, float height, char* errStr )
{
	if ( !TEvent::Setup( _hwnd, depth, width, height, errStr ) )
    	return false;

    ConsolePrint( "loading landscape" );
    if ( !landscape.Load( 1, errStr ) )
    	return false;

    ConsolePrint( "loading sherman.bin" );
    if ( !sherman.LoadBinary( "sherman.bin", errStr, "data\\objects", "data\\textures" ) )
    	return false;
	sherman.Information();

    ConsolePrint( "loading tiger.bin" );
    if ( !tiger.LoadBinary( "tiger.bin", errStr, "data\\objects", "data\\textures" ) )
    	return false;
	tiger.Information();
    tiger.X( 425 );
    tiger.Z( 225 );

#ifdef _DXSOUND_
	soundSystem = new TSound( _hwnd );
	soundSystem->Load( "data\\audio\\engine1.wav" );
	soundSystem->Load( "data\\audio\\engine2.wav" );
#endif

    return true;
};

//==========================================================================

void TApp::Destroy( void )
{
	TEvent::Destroy();
}

//==========================================================================

void TApp::Movement( float yAngle, float& dirX, float& dirZ )
{
    // movement
    float yangle = float(( yAngle / 180.0f) * kPI);
	dirZ = -(float)cos(yangle);
	dirX = (float)sin(yangle);
}

//==========================================================================

void TApp::Logic( void )
{
	TEvent::Logic();

	// camera angle movement
    camera.Yangle( camera.Yangle() - deltaXAngle );
    camera.Xangle( camera.Xangle() + deltaYAngle );
	deltaXAngle = 0.0f;
	deltaYAngle = 0.0f;

	// clip camera up & down to 165 degree field of vision
	if ( camera.Xangle() < -75.0f ) camera.Xangle( -75.0f ); // upwards
	if ( camera.Xangle() >  90.0f ) camera.Xangle(  90.0f ); // downwards

	// key codes
	float speed = 1.0f;
	float dirx,dirz;
	if ( keys & kKeyUp )
	{
		Movement( camera.Yangle(), dirx, dirz );
		camera.X( camera.X() + dirx*speed );
		camera.Z( camera.Z() + dirz*speed );
	}
	if ( keys & kKeyDown )
	{
		Movement( camera.Yangle()+180.0f, dirx, dirz );
		camera.X( camera.X() + dirx*speed );
		camera.Z( camera.Z() + dirz*speed );
	}
	if ( keys & kKeyLeft )
	{
		Movement( camera.Yangle()+90.0f, dirx, dirz );
		camera.X( camera.X() + dirx*speed );
		camera.Z( camera.Z() + dirz*speed );
	}
	if ( keys & kKeyRight )
	{
		Movement( camera.Yangle()-90.0f, dirx, dirz );
		camera.X( camera.X() + dirx*speed );
		camera.Z( camera.Z() + dirz*speed );
	}

#ifdef _DXSOUND_
	soundSystem->SetLocation( 0, camera.Yangle(), sherman.X()-camera.X(), 0, camera.Z()-sherman.Z() );
	soundSystem->SetLocation( 1, camera.Yangle(), tiger.X()-camera.X(), 0, camera.Z()-tiger.Z() );

    if ( firstTime )
    {
		soundSystem->Play( 0, true );
		soundSystem->Play( 1, true );
        firstTime = false;
    }
#endif

	// sherman movement
	speed = 0.1f;
	if ( shermanMoveFwd )
	{
		Movement( sherman.Yangle(), dirx, dirz );
		sherman.X( sherman.X() + dirx*speed );
		sherman.Z( sherman.Z() + dirz*speed );
	}
	if ( shermanMoveBwd )
	{
		Movement( sherman.Yangle()+180.0f, dirx, dirz );
		sherman.X( sherman.X() + dirx*speed );
		sherman.Z( sherman.Z() + dirz*speed );
	}
	if ( shermanLeft )
		sherman.Yangle( sherman.Yangle() + 3.0f );
	if ( shermanRight )
		sherman.Yangle( sherman.Yangle() - 3.0f );
};

//==========================================================================

void TApp::Render( void )
{
	TEvent::Render();

	//
	//	NB. order is vital in the following set of code
	//
	//-------------------------------------------------

	glLoadIdentity();
	glRotatef( camera.Xangle(),1,0,0);

	// set height (depth)
    float tx;
    float ty;
    float tz;
    landscape.GetTranslation( camera.X(), -camera.Z(), tx, ty, tz ); // -sherman.X(), sherman.Z()
	glTranslatef(0,ty-5,0);

	// hdg for all
	glRotatef( 180-camera.Yangle(),0,1,0);

    // translate and draw landscape
    glPushMatrix();
		glTranslatef(-tx,-camera.Y(),-tz);
	    landscape.Draw( camera.Yangle(), camera.X(), -camera.Z() );
    glPopMatrix();

    // translate and draw other objects
    glPushMatrix();
		float cx,cy,cz;
		sherman.GetCenter(cx,cy,cz);
	    landscape.GetTranslation( sherman.X()+cx, sherman.Z()+cz, tx, ty, tz );
	    glTranslatef( sherman.X()-camera.X(), -ty+1, camera.Z()-sherman.Z() );
		// rotate around its center
		glTranslatef(cx,-sherman.MinY(),cz);

	    float xAngle = landscape.GetXAngle( sherman );
	    float zAngle = landscape.GetZAngle( sherman );

		glRotatef(sherman.Yangle()-90.0f,0,1,0);
		glRotatef(xAngle,1,0,0);
		glRotatef(zAngle,0,0,1);

	    sherman.Draw(showBoundingBoxes);

    glPopMatrix();

    glPushMatrix();
	    landscape.GetTranslation( tiger.X(), tiger.Z(), tx, ty, tz );
	    glTranslatef( tiger.X()-camera.X(), -ty, camera.Z()-tiger.Z() );
	    tiger.Draw(showBoundingBoxes);
    glPopMatrix();
};

//==========================================================================

void TApp::MouseDown( int button, int x, int y)
{
	if ( button==1 )
    	leftDown = true;
	if ( button==2 )
    	rightDown = true;
    startX = x;
    startY = y;
};

//==========================================================================

void TApp::MouseUp( int button, int, int )
{
	if ( button==1 )
    	leftDown = false;
	if ( button==2 )
    	rightDown = false;
};

//==========================================================================

void TApp::MouseMove( int x, int y )
{
    int deltaX = x - startX;
    int deltaY = y - startY;
    
	if ( leftDown )
    {
        startX = x;
        startY = y;
		deltaXAngle += float(deltaX);
		deltaYAngle += float(deltaY);
    }
};

//==========================================================================

void TApp::KeyUp( int kp )
{
	TEvent::KeyUp(kp);

	switch (kp)
	{
		case VK_LEFT:
			keys = keys & (~kKeyLeft);
			break;
		case VK_RIGHT:
			keys = keys & (~kKeyRight);
			break;
		case VK_UP:
			keys = keys & (~kKeyUp);
			break;
		case VK_DOWN:
			keys = keys & (~kKeyDown);
			break;

		case 'w':
		case 'W':
			shermanMoveFwd = false;
			break;

		case 'a':
		case 'A':
			shermanLeft = false;
			break;

		case 'd':
		case 'D':
			shermanRight = false;
			break;

		case 'x':
		case 'X':
			shermanMoveBwd = false;
			break;

		case 'b':
		case 'B':
			showBoundingBoxes = !showBoundingBoxes;
			break;
	}
};

//==========================================================================

void TApp::KeyDown( int kp )
{
	TEvent::KeyDown(kp);
    
	switch (kp)
	{
		case VK_LEFT:
			keys = keys | kKeyLeft;
			break;
		case VK_RIGHT:
			keys = keys | kKeyRight;
			break;
		case VK_UP:
			keys = keys | kKeyUp;
			break;
		case VK_DOWN:
			keys = keys | kKeyDown;
			break;

		case 'w':
		case 'W':
			shermanMoveFwd = true;
			break;

		case 'a':
		case 'A':
			shermanLeft = true;
			break;

		case 'd':
		case 'D':
			shermanRight = true;
			break;

		case 'x':
		case 'X':
			shermanMoveBwd = true;
			break;
	}
};

//==========================================================================


