#include <precomp_header.h>

#include <win32/events.h>
#include <win32/win32.h>
#include <win32/resource.h>

#include <common/sounds.h>

#include <object/object.h>
#include <object/landscape.h>
#include <object/geometry.h>
#include <object/tank.h>
#include <object/anim.h>

#include <object/recordSystem.h>

bool pbRecordSherman = false;
bool pbPlaybackSherman = true;
bool pbRecordTiger = false;
bool pbPlaybackTiger = false;

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

	size_t				shermanKeys;
	size_t				tigerKeys;

    TTank				sherman;
	TAnimation			shermanAnimation;

    TTank				tiger;
	TAnimation			tigerAnimation;

	TAnimation			explosion;
	TAnimation			exhaust;

	TLandscape			landObj;

	TTankRecord			shermanRecording;
	TTankRecord			tigerRecording;

	TTexture			waterMark;
	TTexture			cameras[8];
	size_t				selectedCamera;

	size_t				currentLocation;
	bool				explode;
	
	bool				shermanEngineOn;
	bool				tigerEngineOn;

	bool				shermanPrevEngineOn;
	bool				tigerPrevEngineOn;

	bool				shermanFireOn;
	bool				shermanPrevFireOn;
	bool				tigerFireOn;
	bool				tigerPrevFireOn;

	bool				shermanCtrlDown;
	bool				tigerCtrlDown;

	bool				showXYZ;

	long				reloadTime;
	long				lastFire;
	long				endExplosion;
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

    appName = "TankGameDemo";
    appTitle = "Tank Game Demo";

	return true;
};

//==========================================================================

TApp::TApp( bool fog )
	: sherman( 0.25f ),
	  tiger( 0.25f ),
	  TEvent( fog )
{
	shermanPrevEngineOn = false;
	shermanEngineOn = false;

	tigerEngineOn = false;
	tigerPrevEngineOn = false;

	shermanFireOn = false;
	shermanPrevFireOn = false;

	tigerFireOn = false;
	tigerPrevFireOn = false;

	yAngle = 180.0f;
	xAngle = 0.0f;
	deltaXAngle = 0.0f;
	deltaYAngle = 0.0f;
	leftDown = false;
	rightDown = false;

	shermanCtrlDown = false;
	tigerCtrlDown = false;

	showBoundingBox = false;
	shermanKeys = 0;
	tigerKeys = 0;
	radius = 15.0f;
	cameraLocation = -1;
	currentLocation = NULL;
	lastFire = 0;
	reloadTime = 2000; // two seconds on trigger delay

	showXYZ = false;
	explode = false;

	selectedCamera = 0;
};


TApp::~TApp( void )
{
}


void TApp::Destroy( void )
{
	TString errStr;
	if ( pbRecordSherman )
		shermanRecording.SaveBinary( "shermanTestRecording.rcd", errStr );
	if ( pbRecordTiger )
		tigerRecording.SaveBinary( "tigerTestRecording.rcd", errStr );
};


bool TApp::StartupSystem( HWND _hwnd, TString& errStr )
{
	if ( !TEvent::StartupSystem( _hwnd, errStr ) )
		return false;

	WriteString( "Use cursor keys, A, Z, and the mouse to view" );
	WriteString( "Use ALT+C to show/hide console window" );
	WriteString( "ALT+Enter to switch full screen & back\n" );

	int index = fileName.length()-4;
	if ( index>0 )
	{
		if ( !landObj.Loaded() )
		{
			if ( pbPlaybackSherman )
				shermanRecording.LoadBinary( "shermanTestRecording.rcd" );

			if ( !waterMark.LoadRGBAJPEG( IDR_OCES, errStr, 128 ) )
				return false;

			unsigned char a = 150;
			if ( !(cameras[0].LoadRGBAJPEG( IDR_CAMERA1, errStr, a ) &&
				 cameras[1].LoadRGBAJPEG( IDR_CAMERA1DIM, errStr, a ) &&
				 cameras[2].LoadRGBAJPEG( IDR_CAMERA2, errStr, a ) &&
				 cameras[3].LoadRGBAJPEG( IDR_CAMERA2DIM, errStr, a ) &&
				 cameras[4].LoadRGBAJPEG( IDR_CAMERA3, errStr, a ) &&
				 cameras[5].LoadRGBAJPEG( IDR_CAMERA3DIM, errStr, a ) &&
				 cameras[6].LoadRGBAJPEG( IDR_CAMERA4, errStr, a ) &&
				 cameras[7].LoadRGBAJPEG( IDR_CAMERA4DIM, errStr, a )) )
				return false;

			WriteString( "Loading landscape object" );
			if ( !landObj.LoadBinary( "landscape.lnd", errStr, path, texturePath ) )
		    	return false;
			landObj.Information();
			currentRenderDepth = renderDepth;
		    ly = -landObj.MaxY() + 15.0f;
			if ( !exhaust.LoadBinary( "exhaust.anm", errStr, path, texturePath ) )
				return false;
			if ( !explosion.LoadBinary( "explosion.anm", errStr, path, texturePath ) )
				return false;

			if ( !sherman.LoadBinary( "sherman.obj", errStr, path, texturePath, &exhaust ) )
				return false;
			if ( !shermanAnimation.LoadBinary( "sherman.anm", errStr, path, texturePath ) )
				return false;

			if ( !tiger.LoadBinary( "tiger.obj", errStr, path, texturePath, &exhaust ) )
				return false;
			if ( !tigerAnimation.LoadBinary( "tiger.anm", errStr, path, texturePath ) )
				return false;

			lx = -74.0f;
			lz = -91.0f;
			sherman.X( lx );
			sherman.Z( lz );
			tiger.X( lx - 3.0f );
			tiger.Z( lz );
			yAngle = 0;
		}
	}

#ifdef _DXSOUND_
	soundSystem = new TSound( _hwnd );
	PostCond( soundSystem!=NULL );

	soundSystem->Load( "data\\audio\\engine1.wav" );
	soundSystem->Load( "data\\audio\\gun1.wav" );
	soundSystem->Load( "data\\audio\\explode3.wav" );
#endif
    return true;
};



void TApp::Logic( void )
{
	TEvent::Logic();

	long time = ::GetTickCount();

	shermanEngineOn = ((shermanKeys&15)!=0);
	tigerEngineOn = ((tigerKeys&15)!=0);

	shermanFireOn = ((shermanKeys&aFire)!=0) && ((time-lastFire)>0) && explosion.AnimationDone();
	float speed = 0.25f;

	if ( pbRecordSherman )
	{
		unsigned char button = 0;
		if ( leftDown )
			button |= 1;
		if ( rightDown )
			button |= 2;

		shermanRecording.Add( shermanKeys, shermanCtrlDown, button, deltaXAngle, deltaYAngle );
	}
	else if ( pbPlaybackSherman )
	{
		leftDown = false;
		rightDown = false;
		unsigned char button = 0;
		shermanRecording.Get( shermanKeys, shermanCtrlDown, button, deltaXAngle, deltaYAngle );
		if ( (button&1)>0 )
			leftDown = true;
		if ( (button&2)>0 )
			rightDown = true;

		// re-evaluate these
		shermanEngineOn = ((shermanKeys&15)!=0);
		shermanFireOn = ((shermanKeys&aFire)!=0) && ((time-lastFire)>0) && explosion.AnimationDone();
	}

	if ( (shermanKeys&aExplode)>0 )
	{
		if ( !explode && !soundSystem->IsPlaying(2) )
		{
			explode = true;
			endExplosion = ::GetTickCount() + 5000;
			if ( selectedCamera==1 )
				shermanAnimation.StartAnimation( 0, 20 );
			else
				tigerAnimation.StartAnimation( 0, 20 );
			soundSystem->SetLocation( 2, 0, 0, 0, 25.0f );
			soundSystem->Play( 2, false );
		}
	};

	sherman.Move( landObj, shermanKeys, speed, shermanFireOn, shermanCtrlDown );
	sherman.PauseAnimations( !shermanEngineOn );

	tiger.Move( landObj, tigerKeys, speed, tigerFireOn, tigerCtrlDown );
	tiger.PauseAnimations( !tigerEngineOn );

	if ( shermanFireOn )
	{
		explosion.StartAnimation();

		explosion.Zangle(90);
		explosion.Xangle(90);
		explosion.Yangle(90);
		if ( selectedCamera==0 )
		{
			explosion.SetPos( sherman.TargetX(),
							  sherman.TargetY()+0.5f,
							  sherman.TargetZ() );
		}
		else
		{
			explosion.SetPos( tiger.TargetX(),
							  tiger.TargetY()+0.5f,
							  tiger.TargetZ() );
		}
	}

	if ( shermanCtrlDown )
	{
		float ta = sherman.TurretAngle();
		ta += deltaYAngle;
		sherman.TurretAngle( ta );

		float ba = sherman.BarrelAngle();
		ba += deltaXAngle;
		if ( ba > 10.0f )
			ba = 10.0f;
		if ( ba<-30.0f )
			ba = -30.0f;
		sherman.BarrelAngle( ba );
	}
	else if ( tigerCtrlDown )
	{
		float ta = tiger.TurretAngle();
		ta += deltaYAngle;
		tiger.TurretAngle( ta );

		float ba = tiger.BarrelAngle();
		ba += deltaXAngle;
		if ( ba > 10.0f )
			ba = 10.0f;
		if ( ba<-30.0f )
			ba = -30.0f;
		tiger.BarrelAngle( ba );
	}
	else
	{
		radius += deltaXAngle;
		if ( radius<2.5f )
			radius = 2.5f;
		if ( radius>25.0f )
			radius = 25.0f;

		yAngle += deltaYAngle;
		if ( yAngle>360.0f ) yAngle -= 360.0f;
		if ( yAngle<0.0f ) yAngle += 360.0f;
	}

	deltaXAngle = 0.0f;
	deltaYAngle = 0.0f;

    float yangle = yAngle*degToRad;
	float v1fcos = (float)cos(yangle)*radius;
	float v1fsin = (float)sin(yangle)*radius;

	float vx,vz;
	if ( explode )
	{
		if ( selectedCamera==0 )
			sherman.CurrentIndex(2);
		else
			tiger.CurrentIndex(2);
	}
	else
	{
		if ( selectedCamera==0 )
			sherman.CurrentIndex(0);
		else
			tiger.CurrentIndex(0);
	}

	if ( selectedCamera==0 )
	{
		lx = -sherman.MX() + v1fsin;
		lz = -sherman.MZ() - v1fcos;
		vx = sherman.MX() - v1fsin;
		vz = sherman.MZ() + v1fcos;
	}
	else
	{
		lx = -tiger.MX() + v1fsin;
		lz = -tiger.MZ() - v1fcos;
		vx = tiger.MX() - v1fsin;
		vz = tiger.MZ() + v1fcos;
	}

	landObj.GetLORTriangle( vx,ly,vz, cameraLocation );
	landObj.BridgeHeight( vx,ly,vz );
	ly = -ly - 5.0f;

	if ( selectedCamera==0 )
		xAngle = -float(atan( (sherman.MY()+ly) / radius )) * radToDeg;
	else
		xAngle = -float(atan( (tiger.MY()+ly) / radius )) * radToDeg;

#ifdef _DXSOUND_
	soundSystem->SetLocation( 0, 0, 0,0,25.0f ); // sound#, angle, x,y,z

	if ( shermanEngineOn!=shermanPrevEngineOn )
	{
		shermanPrevEngineOn = shermanEngineOn;

		if ( shermanEngineOn )
			soundSystem->Play( 0, true );
		else
			soundSystem->Stop( 0 );
	}

	if ( shermanFireOn )
	{
		lastFire = time + reloadTime;
		shermanPrevFireOn = shermanFireOn;
		shermanKeys = shermanKeys & ~aFire;

		if ( shermanFireOn )
			soundSystem->Play(1);

		// gun recoil
		if ( selectedCamera==0 )
		{
			float yangle = sherman.Yangle() * degToRad;
			float v1fcos = -(float)cos(yangle);
			float v1fsin = (float)sin(yangle);
			sherman.X( sherman.X() + v1fsin*0.1f );
			sherman.Z( sherman.Z() + v1fcos*0.1f );
		}
		else
		{
			float yangle = tiger.Yangle() * degToRad;
			float v1fcos = -(float)cos(yangle);
			float v1fsin = (float)sin(yangle);
			tiger.X( tiger.X() + v1fsin*0.1f );
			tiger.Z( tiger.Z() + v1fcos*0.1f );
		}
	}
	else if ( shermanPrevFireOn )
	{
		shermanPrevFireOn = false;

		// gun recoil
		if ( selectedCamera==0 )
		{
			float yangle = sherman.Yangle() * degToRad;
			float v1fcos = -(float)cos(yangle);
			float v1fsin = (float)sin(yangle);
			sherman.X( sherman.X() - v1fsin*0.1f );
			sherman.Z( sherman.Z() - v1fcos*0.1f );
		}
		else
		{
			float yangle = tiger.Yangle() * degToRad;
			float v1fcos = -(float)cos(yangle);
			float v1fsin = (float)sin(yangle);
			tiger.X( tiger.X() - v1fsin*0.1f );
			tiger.Z( tiger.Z() - v1fcos*0.1f );
		}
	}

#endif
	if ( explode )
	{
		if ( (time-endExplosion) > 0 )
			explode = false;
	}
};



void TApp::Render( void )
{
	glLoadIdentity();
	{
		if ( landObj.Initialised() )
		{
		    glRotatef( xAngle, 1,0,0 );
			glRotatef( yAngle, 0,1,0 );
			glTranslatef( lx,ly,lz );

			if ( selectedCamera==0 )
			{
				landObj.Draw(lx,ly,lz,yAngle,xAngle,
							 sherman.X(), sherman.Z(),
							 showBoundingBox);
				sherman.Draw( explode );
				tiger.Draw( false );

				if ( explode )
				{
					shermanAnimation.SetOrientationToVehicle( sherman );
					shermanAnimation.Draw();
				}
			}
			else if ( selectedCamera==1 )
			{
				landObj.Draw(lx,ly,lz,yAngle,xAngle,
							 tiger.X(), tiger.Z(),
							 showBoundingBox);
				tiger.Draw( explode );
				sherman.Draw( false );

				if ( explode )
				{
					tigerAnimation.SetOrientationToVehicle( tiger );
					tigerAnimation.Draw();
				}
			}

			if ( !explosion.AnimationDone() )
			{
				explosion.Yangle( yAngle+90 );
				explosion.Draw();
			}
		}
	}

	// adjust render depth?
	if ( landObj.Initialised() )
	if ( currentRenderDepth!=renderDepth )
	{
		ConsolePrint( "Changing render depth from %d to %d", currentRenderDepth, renderDepth );
		currentRenderDepth = renderDepth;
		landObj.RenderDepth( renderDepth );
	}

	if ( showXYZ )
	{
		TString buf;
		buf = FormatString( "%2.1f, %2.1f, %2.1f", lx,ly,lz );
		SetMessage( buf );
	}

	// show watermark
	if ( landObj.Initialised() )
	{
		float ysize = 0.0375f;
		float xsize = 0.3f;
		glLoadIdentity();
		glTranslatef( 0.55f,-0.60f,-1.1f );
		glRotatef( 180, 0,1,0 );
		glEnable( GL_BLEND );
		waterMark.Draw(-xsize,-ysize,xsize,ysize);

		ysize = 0.0375f;
		xsize = 0.0375f;

		glLoadIdentity();
		glTranslatef( 0.55f,0.50f,-1.1f );
		glRotatef( 180, 0,1,0 );
		for ( size_t i=0; i<4; i++ )
		{
			size_t offset = 1;
			if ( i==selectedCamera )
				offset = 0;
			cameras[i*2+offset].Draw(-xsize,-ysize,xsize,ysize);
			glTranslatef( -xsize*2.0f,0,0 );
		}

		glDisable( GL_BLEND );
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
	{
		if ( selectedCamera==0 )
			shermanKeys = shermanKeys | aFire;
		else if ( selectedCamera==1 )
			tigerKeys = tigerKeys | aFire;
	}
};



void TApp::MouseUp( int button, int, int )
{
	if ( button==1 )
    	leftDown = false;
	if ( button==2 )
    	rightDown = false;

	if ( leftDown )
	{
		if ( selectedCamera==0 )
			shermanKeys = shermanKeys & ~aFire;
		else if ( selectedCamera==1 )
			tigerKeys = tigerKeys & ~aFire;
	}
};



void TApp::MouseMove( int x, int y, bool _ctrlDown )
{
	if ( selectedCamera==0 )
		shermanCtrlDown = _ctrlDown;
	else
		tigerCtrlDown = _ctrlDown;

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
				if ( selectedCamera==0 )
				{
					shermanKeys = shermanKeys | aUp;
				}
				else if ( selectedCamera==1 )
				{
					tigerKeys = tigerKeys | aUp;
				}
				break;
			}

		case VK_DOWN:
			{
				if ( selectedCamera==0 )
				{
					shermanKeys = shermanKeys | aDown;
				}
				else if ( selectedCamera==1 )
				{
					tigerKeys = tigerKeys | aDown;
				}
				break;
			}

		case VK_LEFT:
			{
				if ( selectedCamera==0 )
				{
					shermanKeys = shermanKeys | aLeft;
				}
				else if ( selectedCamera==1 )
				{
					tigerKeys = tigerKeys | aLeft;
				}
				break;
			}

		case VK_RIGHT:
			{
				if ( selectedCamera==0 )
				{
					shermanKeys = shermanKeys | aRight;
				}
				else if ( selectedCamera==1 )
				{
					tigerKeys = tigerKeys | aRight;
				}
				break;
			}

		case ' ':
			{
				if ( selectedCamera==0 )
				{
					shermanKeys = shermanKeys | aFire;
				}
				else if ( selectedCamera==1 )
				{
					tigerKeys = tigerKeys | aFire;
				}
				break;
			}

		case 'x':
		case 'X':
			{
				if ( selectedCamera==0 )
				{
					shermanKeys = shermanKeys | aExplode;
				}
				else if ( selectedCamera==1 )
				{
					tigerKeys = tigerKeys | aExplode;
				}
				break;
			}

		case 'c':
		case 'C':
			{
				showXYZ = !showXYZ;
			}
			break;
	}
};



void TApp::KeyUp( int ku )
{
	TEvent::KeyUp( ku );
	switch ( ku )
	{
		case VK_UP:
			{
				if ( selectedCamera==0 )
				{
					shermanKeys = shermanKeys & ~aUp;
				}
				else if ( selectedCamera==1 )
				{
					tigerKeys = tigerKeys & ~aUp;
				}
				break;
			}

		case VK_DOWN:
			{
				if ( selectedCamera==0 )
				{
					shermanKeys = shermanKeys & ~aDown;
				}
				else if ( selectedCamera==1 )
				{
					tigerKeys = tigerKeys & ~aDown;
				}
				break;
			}

		case VK_LEFT:
			{
				if ( selectedCamera==0 )
				{
					shermanKeys = shermanKeys & ~aLeft;
				}
				else if ( selectedCamera==1 )
				{
					tigerKeys = tigerKeys & ~aLeft;
				}
				break;
			}

		case VK_RIGHT:
			{
				if ( selectedCamera==0 )
				{
					shermanKeys = shermanKeys & ~aRight;
				}
				else if ( selectedCamera==1 )
				{
					tigerKeys = tigerKeys & ~aRight;
				}
				break;
			}

		case ' ':
			{
				if ( selectedCamera==0 )
				{
					shermanKeys = shermanKeys & ~aFire;
				}
				else if ( selectedCamera==1 )
				{
					tigerKeys = tigerKeys & ~aFire;
				}
				break;
			}

		case 'x':
		case 'X':
			{
				if ( selectedCamera==0 )
				{
					shermanKeys = shermanKeys & ~aExplode;
				}
				else if ( selectedCamera==1 )
				{
					tigerKeys = tigerKeys & ~aExplode;
				}
				break;
			}

		case '1':
			{
				selectedCamera = 0;
				tigerKeys = 0;
				break;
			}

		case '2':
			{
				selectedCamera = 1;
				shermanKeys = 0;
				break;
			}

		case '3':
			{
				selectedCamera = 2;
				tigerKeys = 0;
				shermanKeys = 0;
				break;
			}

		case '4':
			{
				selectedCamera = 3;
				tigerKeys = 0;
				shermanKeys = 0;
				break;
			}
	}
};

//==========================================================================

