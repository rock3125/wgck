#include <precomp_header.h>

#include <win32/events.h>
#include <win32/win32.h>
#include <win32/resource.h>

#include <common/sounds.h>

#include <object/geometry.h>
#include <object/tank.h>
#include <object/anim.h>
#include <object/camera.h>
#include <object/recordSystem.h>

#include <tank/tankapp.h>

#include <network/packet.h>

//==========================================================================

const float kPI = 3.1415927f;
const float degToRad = 0.01745329252033f;
const float radToDeg = 57.295779511273f;

//==========================================================================
//
//	All my apps get this function called first for creating the initial setup
//
bool InitialiseStartup( const TString& cmdLine, const TString& texturePath,
					    HINSTANCE hInst, TApp*& app, TString& appName, 
						TString& appTitle, bool fog, TString& errStr )
{
	errStr = "";

	TApp* app1 = new TApp(fog);
    PostCond( app1!=NULL );

   	app1->fileName = "landscape.lnd";
    app1->path = "data";
	app1->texturePath = "data\\textures";
    app = app1;

    appName = "TankGameDemo";
    appTitle = "Tank Game Demo";

	return true;
};

//==========================================================================

TApp::TApp( bool fog )
	: TEvent( fog )
{
	deltaXAngle = 0.0f;
	deltaYAngle = 0.0f;
	leftDown = false;
	rightDown = false;

	ctrlDown = false;

	showBoundingBox = false;
	keys = 0;
	currentLocation = NULL;

	selectedCamera = 0;

	showCoords = false;
};


TApp::~TApp( void )
{
}


void TApp::Destroy( void )
{
};


bool TApp::LoadTank( byte side, TTank& tank, TString& errStr )
{
	switch ( side )
	{
	case 0:
	{
		if ( !tank.LoadBinary( soundSystem, 0, 2, 1, "sherman.obj", "exhaust.anm", 
							   "sherman.anm", "explosion.anm", errStr, path, 
							   texturePath ) )
			return false;
		break;
	}
	case 1:
	{
		if ( !tank.LoadBinary( soundSystem, 3, 2, 1, "tiger.obj", "exhaust.anm", 
							   "tiger.anm", "explosion.anm", errStr, path, 
							   texturePath ) )
			return false;
		break;
	}
	default:
		return false;
	}
	return true;
};


bool TApp::StartupSystem( HWND _hwnd, TString& errStr )
{
	if ( !TEvent::StartupSystem( _hwnd, errStr ) )
		return false;

	WriteString( "Use cursor keys, A, Z, and the mouse to view" );
	WriteString( "Use ALT+C to show/hide console window" );
	WriteString( "ALT+Enter to switch full screen & back\n" );

	soundSystem = new TSound( _hwnd );
	PostCond( soundSystem!=NULL );

	soundSystem->Load( "data\\audio\\engine1.wav" );
	soundSystem->Load( "data\\audio\\gun1.wav" );
	soundSystem->Load( "data\\audio\\explode3.wav" );
	soundSystem->Load( "data\\audio\\engine2.wav" );

	int index = fileName.length()-4;
	if ( index>0 )
	{
		if ( !landObj.Loaded() )
		{
			unsigned char a = 0;
			if ( !(cameras[0].LoadRGBAJPEG( IDR_CAMERA1, errStr, 0,0,0, 0, 255, 16 ) &&
				 cameras[1].LoadRGBAJPEG( IDR_CAMERA1DIM, errStr, 0,0,0, 0, 255, 16 ) &&
				 cameras[2].LoadRGBAJPEG( IDR_CAMERA2, errStr, 0,0,0, 0, 255, 16 ) &&
				 cameras[3].LoadRGBAJPEG( IDR_CAMERA2DIM, errStr, 0,0,0, 0, 255, 16 ) ) )
				return false;

			WriteString( "Loading landscape object" );
			if ( !landObj.LoadBinary( "landscape.lnd", errStr, path, texturePath ) )
		    	return false;
			landObj.Information();
			currentRenderDepth = renderDepth;
		}
	}
    return true;
};


void TApp::Logic( TTank& tank, size_t numOthers, TTank** others )
{
	TEvent::Logic();

	//
	//	Do recording/playback stuff
	//
	float dxa = deltaXAngle;
	float dya = deltaYAngle;

	// move player
	tank.Move( landObj, keys, ctrlDown, dxa, dya, numOthers, others );

	if ( selectedCamera==0 )
	{
		cam.SetVehicleCamera( landObj, tank.MX(), tank.MY(), tank.MZ(), 
							  ctrlDown, deltaXAngle, deltaYAngle );
	}
	deltaXAngle = 0;
	deltaYAngle = 0;

	if ( soundSystem!=NULL )
		soundSystem->SetLocation( 0, 0, 0,0,25.0f ); // sound#, angle, x,y,z
};



void TApp::Render( TTank& tank, size_t numOthers, TTank** others )
{
	if ( showCoords )
	{
		char str[256];
		sprintf( str, "%2.1f, %2.1f, %2.1f", tank.MX(), tank.MY(), tank.MZ() );
		SetMessage( str );
	}

	{
		if ( landObj.Initialised() )
		{
			if ( selectedCamera==0 )
			{
			    glRotatef( cam.XAngle(), 1,0,0 );
				glRotatef( cam.YAngle(), 0,1,0 );
				glTranslatef( cam.CameraX(),
							  cam.CameraY(),
							  cam.CameraZ() );

				landObj.Draw( cam, tank.X(), tank.Z(),false);
				tank.Draw( cam.YAngle());

				// draw all other tanks
				for ( size_t i=0; i<numOthers; i++ )
				{
					others[i]->Draw( cam.YAngle() );
				}

				landObj.Draw( cam, tank.X(), tank.Z(),true);
			}
		}
	}

	// adjust render depth?
	if ( landObj.Initialised() )
	if ( currentRenderDepth!=renderDepth )
	{
		WriteString( "Changing render depth from %d to %d", currentRenderDepth, renderDepth );
		currentRenderDepth = renderDepth;
		landObj.RenderDepth( renderDepth );
	}
/*
	// show cameras
	if ( landObj.Initialised() )
	{
		float ysize;
		float xsize;

		glEnable( GL_BLEND );

		ysize = 0.0375f;
		xsize = 0.0375f;

		glLoadIdentity();
		glTranslatef( 0.625f,0.50f,-1.1f );
		glRotatef( 180, 0,1,0 );
		for ( size_t i=0; i<2; i++ )
		{
			size_t offset = 1;
			if ( i==selectedCamera )
				offset = 0;
			cameras[i*2+offset].Draw(-xsize,-ysize,xsize,ysize);
			glTranslatef( -xsize*2.0f,0,0 );
		}

		glDisable( GL_BLEND );
	}
*/
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
	{
		keys = keys | aFire;
	}
};



void TApp::MouseUp( int button, int, int )
{
	if ( button==1 )
    	leftDown = false;
	if ( button==2 )
    	rightDown = false;

	if ( !leftDown )
	{
		keys = keys & ~aFire;
	}
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
			{
				keys = keys | aUp;
				break;
			}

		case VK_DOWN:
			{
				keys = keys | aDown;
				break;
			}

		case VK_LEFT:
			{
				keys = keys | aLeft;
				break;
			}

		case VK_RIGHT:
			{
				keys = keys | aRight;
				break;
			}

		case ' ':
			{
				keys = keys | aFire;
				break;
			}
	}
};



void TApp::KeyUp( int ku )
{
	TEvent::KeyUp( ku );
	switch ( ku )
	{
		case VK_UP:
			{
				keys = keys & ~aUp;
				break;
			}

		case VK_DOWN:
			{
				keys = keys & ~aDown;
				break;
			}

		case VK_LEFT:
			{
				keys = keys & ~aLeft;
				break;
			}

		case VK_RIGHT:
			{
				keys = keys & ~aRight;
				break;
			}

		case ' ':
			{
				keys = keys & ~aFire;
				break;
			}

		case 'c':
		case 'C':
			{
				showCoords = !showCoords;
				break;
			}
	}
};

//==========================================================================

