#include <precomp_header.h>

#include <racewin32/events.h>
#include <racewin32/win32.h>
#include <racewin32/resource.h>

#include <common/sounds.h>

#include <object/binobject.h>
#include <object/landscape.h>
#include <object/geometry.h>
#include <object/tank.h>
#include <object/anim.h>
#include <object/camera.h>
#include <object/racecar.h>

#include <object/recordSystem.h>

bool pbRecordCar1 = false;
bool pbPlaybackCar1 = true;
bool pbRecordCar2 = false;
bool pbPlaybackCar2 = true;
bool pbRecordCam = false;
bool pbPlaybackCam = true;

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
	TApp( bool fog );
    ~TApp( void );

	bool StartupSystem( HWND hwnd, TString& errStr );

	void Render( void );
	void Logic( void );

	void MouseDown( int button, int x, int y);
	void MouseUp( int button, int, int );
	void MouseMove( int x, int y, bool ctrlDown );
	void KeyDown( int kd );
	void KeyUp( int ku );

	void Destroy( void );

public:
    TString		fileName;
    TString		path;
	TString		texturePath;

private:
	void		DrawWaterMark( void );
	void		DrawSpeedometer( void );
	void		DrawCameras( void );

private:
	const char* finishedMessage;

	bool		showBoundingBox;
	bool		showCoords;
    bool 		leftDown;
	bool		rightDown;

	bool		initialised;

	float		deltaXAngle;
	float		deltaYAngle;

	size_t		currentRenderDepth;

	size_t				keys;

	TRaceCar			car1;
	TRaceCar			car2;
	TCamera				carCam;

	float				speed; // speed - 0..100

	TLandscape			landObj;

	TTexture			speedoBackplate1;
	TTexture			speedoBackplate2;
	TTexture			waterMark;
	TTexture			cameras[4];

	size_t				selectedCamera;
	bool				switchingCamera;
	size_t				switchingIndex;
	size_t				targetCamera;
	size_t				switchingNumSteps;

	size_t				currentLocation;
	
	bool				ctrlDown;

	long				timeout;
};

//==========================================================================
//
//	All my apps get this function called first for creating the initial setup
//
bool InitialiseStartup( const TString& cmdLine, const TString& texturePath,
					    HINSTANCE hInst, TEvent*& app, TString& appName, 
						TString& appTitle, bool fog, TString& errStr )
{
	errStr = "";

	TApp* app1 = new TApp(fog);
    PostCond( app1!=NULL );

   	app1->fileName = "landscape.lnd";
    app1->path = "data";
	app1->texturePath = "data\\textures";
    app = app1;

    appName = "RallyDemo";
    appTitle = "Rally Demo";

	return true;
};

//==========================================================================

TApp::TApp( bool fog )
	: car1( 0.75f ),
	  car2( 0.75f ),
	  TEvent( fog )
{
	finishedMessage = "This demo has finished.  Click the OK button to quit this application.\nThank you for your time.\n\nPeter de Vocht, 7 August 2001";

	deltaXAngle = 0.0f;
	deltaYAngle = 0.0f;
	leftDown = false;
	rightDown = false;

	initialised = false;

	ctrlDown = false;
	showCoords = false;

	showBoundingBox = false;
	keys = 0;
	currentLocation = NULL;

	selectedCamera = 0;
	targetCamera = 0;
	switchingCamera = false;
	switchingIndex = 0;
	switchingNumSteps = 10;

	speed = 0;
};


TApp::~TApp( void )
{
}


void TApp::Destroy( void )
{
	TString errStr;
	if ( pbRecordCar1 )
	{
		car1.StopRecording( errStr );
	}
	if ( pbRecordCar2 )
	{
		car2.StopRecording( errStr );
	}
	if ( pbRecordCam )
	{
		carCam.StopRecording( errStr );
	}
};


//
//	Load all necessairies on mode switch/startup
//
bool TApp::StartupSystem( HWND _hwnd, TString& errStr )
{
	if ( !TEvent::StartupSystem( _hwnd, errStr ) )
		return false;

	WriteString( "Use cursor keys, A, Z, and the mouse to view" );
	WriteString( "Use ALT+C to show/hide console window" );
	WriteString( "ALT+Enter to switch full screen & back\n" );

	soundSystem = new TSound( _hwnd );
	PostCond( soundSystem!=NULL );

	int index = fileName.length()-4;
	if ( index>0 )
	{
		if ( !initialised )
		{
			soundSystem->Load( "data\\audio\\idle.wav" );
			soundSystem->Load( "data\\audio\\idle.wav" );
			soundSystem->Load( "data\\audio\\driving1.wav" );
			soundSystem->Load( "data\\audio\\driving2.wav" );

			if ( !waterMark.LoadRGBAJPEG( IDR_OCES, errStr, 0,0,0, 0, 50, 16 ) )
				return false;

			if ( !(cameras[0].LoadRGBAJPEG( IDR_CAMERA1, errStr, 0,0,0, 0, 255, 16 ) &&
				 cameras[1].LoadRGBAJPEG( IDR_CAMERA1DIM, errStr, 0,0,0, 0, 255, 16 ) &&
				 cameras[2].LoadRGBAJPEG( IDR_CAMERA2, errStr, 0,0,0, 0, 255, 16 ) &&
				 cameras[3].LoadRGBAJPEG( IDR_CAMERA2DIM, errStr, 0,0,0, 0, 255, 16 ) ) )
				return false;

			unsigned char a = 175;
			if ( !speedoBackplate1.LoadRGBAJPEG( IDR_SPEEDOBACKPLATE1, errStr, a ) )
				return false;

			if ( !speedoBackplate2.LoadRGBAJPEG( IDR_SPEEDOBACKPLATE2, errStr, a ) )
				return false;

			WriteString( "Loading landscape object" );
			if ( !landObj.LoadBinary( "landscape.lnd", errStr, path, texturePath ) )
		    	return false;
			landObj.Information();
			currentRenderDepth = renderDepth;

			if ( !car1.LoadBinary( soundSystem, 0,2,"racecar1.obj", "wheel.bin", errStr,
								  path, texturePath ) )
				return false;

			if ( !car2.LoadBinary( soundSystem, 1,3,"racecar2.obj", "wheel.bin", errStr,
								  path, texturePath ) )
				return false;

			if ( pbRecordCar1 )
			{
				car1.SetupRecording( "data\\rallycar1rec.rcd" );
			}
			else if ( pbPlaybackCar1 )
			{
				car1.SetupPlayback( "data\\rallycar1rec.rcd", errStr );
			}
			if ( pbRecordCar2 )
			{
				car2.SetupRecording( "data\\rallycar2rec.rcd" );
			}
			else if ( pbPlaybackCar2 )
			{
				car2.SetupPlayback( "data\\rallycar2rec.rcd", errStr );
			}

			if ( pbPlaybackCam )
			{
				carCam.SetupPlayback( "data\\rallycam.rcd", errStr );
			}
			else if ( pbRecordCam )
			{
				carCam.SetupRecording( "data\\rallycam.rcd" );
			}

			car1.X( 202.0f );
			car1.Z( -85.0f );
			car2.X( 202.0f );
			car2.Z( -90.0f );
		}
	}
	initialised = true;
    return true;
};

void TApp::Logic( void )
{
	if ( !initialised )
		return;

	TEvent::Logic();

	size_t car1Keys = 0;
	bool   car1CtrlDown = false;
	float  car1dxa = 0;
	float  car1dya = 0;

	size_t car2Keys = 0;
	bool   car2CtrlDown = false;
	float  car2dxa = 0;
	float  car2dya = 0;

	if ( pbPlaybackCar1 )
	{
		long time = ::GetTickCount();
		if ( !car1.PlaybackFinished() )
		{
			timeout = time + 10000;
		}

		if ( car1.PlaybackFinished() && (time>timeout) )
		{
			::MessageBox( Window(), finishedMessage, 
						  "Demo Finished", MB_OK | MB_ICONINFORMATION );
			::exit(0);
		}
	}

	if ( selectedCamera==0 )
	{
		car1Keys = keys;
		car1CtrlDown = ctrlDown;
		car1dxa = deltaXAngle;
		car1dya = deltaYAngle;
	}
	else if ( selectedCamera==1 )
	{
		car2Keys = keys;
		car2CtrlDown = ctrlDown;
		car2dxa = deltaXAngle;
		car2dya = deltaYAngle;
	}

	car1.Move( landObj, car1Keys, car1CtrlDown, car1dxa, car1dya );
	car2.Move( landObj, car2Keys, car2CtrlDown, car2dxa, car2dya );

	// move players
	// camera control
	float mx,my,mz;
	if ( !switchingCamera )
	{
		if ( selectedCamera==0 )
		{
			speed = (float(fabs(car1.Speed())) / car1.MaxSpeed()) * 100.0f;
			mx = car1.MX();
			my = car1.MY();
			mz = car1.MZ();
		}
		else if ( selectedCamera==1 )
		{
			speed = (float(fabs(car2.Speed())) / car2.MaxSpeed()) * 100.0f;
			mx = car2.MX();
			my = car2.MY();
			mz = car2.MZ();
		}
	}
	else
	{
		if ( targetCamera==0 )
		{
			speed = (float(fabs(car1.Speed())) / car1.MaxSpeed()) * 100.0f;
			float partial = float(switchingIndex) / float(switchingNumSteps);
			mx = car2.MX() + (car1.MX()-car2.MX()) * partial;
			my = car2.MY() + (car1.MY()-car2.MY()) * partial;
			mz = car2.MZ() + (car1.MZ()-car2.MZ()) * partial;
		}
		else if ( targetCamera==1 )
		{
			speed = (float(fabs(car2.Speed())) / car2.MaxSpeed()) * 100.0f;
			float partial = float(switchingIndex) / float(switchingNumSteps);
			mx = car1.MX() + (car2.MX()-car1.MX()) * partial;
			my = car1.MY() + (car2.MY()-car1.MY()) * partial;
			mz = car1.MZ() + (car2.MZ()-car1.MZ()) * partial;
		}
		switchingIndex++;
		if ( switchingIndex >= switchingNumSteps )
		{
			switchingCamera = false;
			selectedCamera = targetCamera;
		}
	}
	carCam.SetVehicleCamera( landObj, mx,my,mz, ctrlDown, deltaXAngle, deltaYAngle );


	if ( pbRecordCam )
	{
		carCam.Record( selectedCamera );
	}
	else if ( pbPlaybackCam )
	{
		carCam.Playback( selectedCamera );
	};

	// set deltas back to zero for next round
	deltaXAngle = 0;
	deltaYAngle = 0;

	if ( soundSystem!=NULL )
	{
		soundSystem->SetLocation( 0, 0, 
								  (car1.MX()-carCam.CameraX())*0.01f,
								  (car1.MY()-carCam.CameraY())*0.01f,
								  (car1.MZ()-carCam.CameraZ())*0.01f ); // sound#, angle, x,y,z
		soundSystem->SetLocation( 1, 90, 
								  (car2.MX()-carCam.CameraX())*0.01f,
								  (car2.MY()-carCam.CameraY())*0.01f,
								  (car2.MZ()-carCam.CameraZ())*0.01f ); // sound#, angle, x,y,z
		soundSystem->SetLocation( 2, 0, 
								  (car1.MX()-carCam.CameraX())*0.01f,
								  (car1.MY()-carCam.CameraY())*0.01f,
								  (car1.MZ()-carCam.CameraZ())*0.01f ); // sound#, angle, x,y,z
		soundSystem->SetLocation( 3, 90,
								  (car2.MX()-carCam.CameraX())*0.01f,
								  (car2.MY()-carCam.CameraY())*0.01f,
								  (car2.MZ()-carCam.CameraZ())*0.01f ); // sound#, angle, x,y,z
	}
};



void TApp::Render( void )
{
	if ( !initialised )
		return;

	glLoadIdentity();
	{
		if ( selectedCamera==0 )
		{
			carCam.Draw( car1.X(), car1.Z(), landObj );
		}
		else if ( selectedCamera==1 )
		{
			carCam.Draw( car2.X(), car2.Z(), landObj );
		}
		car1.Draw();
		car2.Draw();
	}

	// adjust render depth?
	if ( currentRenderDepth!=renderDepth )
	{
		ConsolePrint( "Changing render depth from %d to %d", currentRenderDepth, renderDepth );
		currentRenderDepth = renderDepth;
		landObj.RenderDepth( renderDepth );
	}

	// show watermark
	DrawWaterMark();
	DrawSpeedometer();
	DrawCameras();

	if ( showCoords )
	{
		char msg[256];
		sprintf( msg, "%2.2f, %2.2f, %2.2f", car1.MX(), car1.MY(), car1.MZ() );
		SetMessage( msg );
	}
};


void TApp::DrawWaterMark( void )
{
	float ysize = 0.075f;
	float xsize = 0.150f;

	glLoadIdentity();
	glTranslatef( 0.66f,-0.56f,-1.1f );
	glRotatef( 180, 0,1,0 );
	glEnable( GL_BLEND );
	waterMark.Draw(-xsize,-ysize,xsize,ysize);
	glDisable( GL_BLEND );
};



void TApp::DrawCameras( void )
{
	float ysize = 0.0375f;
	float xsize = 0.0375f;

	glLoadIdentity();
	glEnable( GL_BLEND );
	glTranslatef( 0.675f,0.47f,-1.1f );
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
};


void TApp::DrawSpeedometer( void )
{
	glEnable( GL_BLEND );

	// draw speedo backplate
	float xsize = 0.15f;
	float ysize = 0.15f;
	glLoadIdentity();
	glTranslatef( -0.6f,-0.40f,-1.1f );
	glRotatef( 180, 0,1,0 );
	if ( selectedCamera==0 )
		speedoBackplate1.Draw(-xsize,-ysize,xsize,ysize);
	else if ( selectedCamera==1 )
		speedoBackplate2.Draw(-xsize,-ysize,xsize,ysize);

	glDisable( GL_BLEND );


	// draw speedo indicator
	glLoadIdentity();
	glTranslatef( -0.6f,-0.40f,-1.099f );
	float speedAngle = 175 - (speed/100.0f)*270.0f;
	glRotatef( speedAngle, 0,0,1 );
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LINE_SMOOTH);
	glLineWidth(2);
	if ( selectedCamera==0 )
		glColor3ub( 100, 20, 20 );
	else if ( selectedCamera==1 )
		glColor3ub( 100, 100, 20 );
	glBegin( GL_LINES );
		glVertex3f( xsize*0.55f, ysize*0.55f,0 );
		glVertex3f( -xsize*0.1f, -ysize*0.1f, 0 );
	glEnd();
	glDisable(GL_LINE_SMOOTH);
	glLineWidth(1);
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
};



void TApp::MouseUp( int button, int, int )
{
	if ( button==1 )
    	leftDown = false;
	if ( button==2 )
    	rightDown = false;
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

		case '1':
			{
				targetCamera = 0;
				switchingCamera = (targetCamera!=selectedCamera);
				switchingIndex = 0;
				keys = 0;
				break;
			}

		case '2':
			{
				targetCamera = 1;
				switchingCamera = (targetCamera!=selectedCamera);
				switchingIndex = 0;
				keys = 0;
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

