#include <precomp_header.h>

#include <Commdlg.h>
#include <shlobj.h>
#include <direct.h>
#include <Winbase.h>
#include <shellapi.h>

#include <viewer/main.h>

//#define _TEST

//==========================================================================
// used by AIs - overall logic counter
size_t logicCounter = 0;
size_t aiId = 1;

//==========================================================================

const float kPI = 3.1415927f;
const float degToRad = 0.01745329252033f;
const float radToDeg = 57.295779511273f;

TApp::TApp( HINSTANCE inst )
	: TEvent( inst, false )
{
	yAngle = 0.0f;
	xAngle = 0.0f;
	deltaXAngle = 0.0f;
	deltaYAngle = 0.0f;
    lx = 0.0f;
#ifdef _TEST
	ly = -100;
#else
    ly = 0.0f;
#endif
    lz = -5.0f;
	leftDown = false;
	showBoundingBox = false;
	keys = 0;
	currentLocation = NULL;
	landscape = false;
	animation = false;
	object = false;
	cobject = false;
	currentObject = 0;

	PostCond( GetRegistrySettings() );

	fileName = "scale_land.bin";
	path  ="data";
	texturePath = "data\\textures";

	paper = false; // paper colour background?
};


TApp::~TApp( void )
{
};


void TApp::SetCmdLine( const char* str )
{
	TEvent::SetCmdLine( str );

	if ( commandLine.NumItems( ' ' ) > 0 )
	{
		fileName = commandLine.GetItem( ' ', 0 );
		size_t len = fileName.length();
		while ( len>0 && fileName[len]!='\\' ) len--;
		if ( len>0 )
		{
			path = fileName.substr(0,len);
			fileName = fileName.substr(len+1);
		}
		else
			path = "";
		texturePath = commandLine.GetItem( ' ', 1 );
	}
};


bool TApp::StartupSystem( TString& errStr )
{
	if ( !TEvent::StartupSystem( errStr ) )
		return false;

	initialised = true;
	if ( !LoadObject( fileName, errStr ) )
	{
		::MessageBox( hWindow, errStr.c_str(), "Error", MB_OK | MB_ICONERROR );
	}

    return true;
};


bool TApp::OpenNewObject( void )
{
//	TEvent::OpenNewObject();

	char fname[1024];

	OPENFILENAME ofn;
	memset( &ofn, 0, sizeof(OPENFILENAME) );
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFilter  = "Binary Object\0*.bin\0Landscape Object\0*.lnd\0Animation\0*.anm\0All Files\0*.*\0";
	ofn.hwndOwner = Window();
	ofn.lpstrFileTitle = fname;
	ofn.nMaxFileTitle = 1024;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

	if ( GetOpenFileName(&ofn)==TRUE )
	{
		char temp[256];
		for ( size_t i=0; i<255; i++ ) temp[i] = 0;
		getcwd(temp,256);

		char textures[MAX_PATH];
		strcpy(textures,temp);
		BROWSEINFO bi;
		bi.hwndOwner = Window();
		bi.pidlRoot = NULL;
		bi.lpszTitle = "Select the texture folder for this object";
		bi.pszDisplayName = textures;
		bi.ulFlags = BIF_BROWSEINCLUDEFILES;
		bi.lpfn = NULL;

		LPITEMIDLIST list;
		if ( (list=SHBrowseForFolder( &bi ))!=NULL )
		{
			strcat( temp, "\\" );
			strcat( temp, bi.pszDisplayName );
			texturePath = temp;

			TString errStr;
			if ( !LoadObject( fname, errStr ) )
			{
				::MessageBox( Window(), errStr.c_str(), "Error", MB_OK | MB_ICONERROR );
				return false;
			}
			return true;
		}
	}
	return false;
};


bool TApp::LoadObject( const TString& fname, TString& errStr )
{
	landscape = false;
	animation = false;
	object = false;
	cobject = false;

	int index = strlen(fname.c_str())-4;
	if ( index>0 )
	{
		if ( stricmp(&fname[index],".lnd")==0 )
		{
			if ( !landObj.Loaded() )
			{
				WriteString( "Loading landscape object\n" );
				if ( !landObj.LoadBinary( fname, errStr, path, texturePath, this ) )
			    	return false;
				landscape = true;
				landObj.Information();
				landObj.RenderDepth(renderDepth);
			    ly = -landObj.MaxY();
			}
		}
		else if ( stricmp(&fname[index],".anm")==0 )
		{
			WriteString( "Loading animation object\n" );
			if ( !animationObj.LoadBinary( fname, errStr, path, texturePath ) )
		    	return false;
			animation = true;
			animationObj.StartAnimation( 0, 20 );
		}
		else if ( stricmp(&fname[index],".bin")==0 )
		{
			WriteString( "Loading BIN object\n" );
			if ( !obj.LoadBinary( fname, errStr, path, texturePath ) )
		    	return false;
			object = true;
			obj.Information();
		}
		else
		{
			WriteString( "Loading compound object\n" );
			if ( !cobj.LoadBinary( fname, errStr, path, texturePath ) )
		    	return false;
			cobject = true;
		}
	}
	return true;
};


void TApp::Logic( void )
{
	TEvent::Logic();

	if ( !showMenu )
	{
	xAngle += deltaXAngle;
	if ( landscape )
	{
		if ( xAngle > 80.0f ) xAngle = 80.0f;
		if ( xAngle < -80.0f ) xAngle = -80.0f;
	};

	yAngle += deltaYAngle;
	if ( yAngle>360.0f ) yAngle -= 360.0f;
	if ( yAngle<0.0f ) yAngle += 360.0f;

	deltaXAngle = 0.0f;
	deltaYAngle = 0.0f;

	float yangle = yAngle*degToRad;
	if ( !landscape )
		yangle = 0;
	
	float v1fcos = (float)cos(yangle);
	float v1fsin = (float)sin(yangle);

	float v2fcos = (float)cos(yangle + 0.5f*kPI );
	float v2fsin = (float)sin(yangle + 0.5f*kPI );

	float v3fcos = (float)cos(yangle - 0.5f*kPI );
	float v3fsin = (float)sin(yangle - 0.5f*kPI );

	// key codes
	float speed = 1.0f;
#ifdef _TEST
	speed = 5;
#else
	if ( !landscape )
		speed = 0.1f;
#endif

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
		if ( landscape )
		{
			lz = lz - v2fcos*speed;
			lx = lx + v2fsin*speed;
		}
		else
		{
			lz = lz + v2fcos*speed;
			lx = lx - v2fsin*speed;
		}
	}
	if ( keys & 2 )
	{
		if ( landscape )
		{
			lz = lz - v3fcos*speed;
			lx = lx + v3fsin*speed;
		}
		else
		{
			lz = lz + v3fcos*speed;
			lx = lx - v3fsin*speed;
		}
	}
	if ( keys & 16 )
		ly = ly - speed*0.25f;
	if ( keys & 32 )
		ly = ly + speed*0.25f;
	}
};

//==========================================================================

void TApp::Render( void )
{
	SetupGL3d( vdepth, vwidth, vheight );
    glLoadIdentity();

	if ( landscape && landObj.Initialised() )
	{
		glRotatef( xAngle, 1,0,0 );
		glRotatef( yAngle, 0,1,0 );
		glTranslatef( lx,ly,lz );

		landObj.Draw(lx,lz,yAngle,xAngle,0,0,false);
		landObj.Draw(lx,lz,yAngle,xAngle,0,0,true);
	}
	else if ( animation )
	{
		glTranslatef( lx,ly,lz );
		glRotatef( xAngle, 1,0,0 );
		glRotatef( yAngle, 0,1,0 );

		if ( animationObj.AnimationPaused() )
		{
			char msg[256];
			sprintf( msg, "frame %d", animationObj.AnimationIndex() );
			SetMessage( msg );
		}
		animationObj.Draw();
	}
	else if ( object )
	{
	    glTranslatef( lx,ly,lz );
		glRotatef( xAngle, 1,0,0 );
	    glRotatef( yAngle, 0,1,0 );

	    obj.Draw();
	}
	else if ( cobject )
	{
	    glTranslatef( lx,ly,lz );
		glRotatef( xAngle, 1,0,0 );
	    glRotatef( yAngle, 0,1,0 );

	    cobj.Draw( currentObject );

		if ( showBoundingBox )
		{
			float minx,miny,minz,maxx,maxy,maxz;
			cobj.GetBounds( currentObject, minx,miny,minz, maxx,maxy,maxz );
			DrawBoundingBox( minx,miny,minz, maxx,maxy,maxz, 255,255,255 );
		}
	}

	RenderMenu();
};

//==========================================================================

void TApp::MouseDown( int button, int x, int y)
{
	TEvent::MouseDown(button,x,y);
	if ( button==1 )
    {
    	leftDown = true;
        startX = x;
        startY = y;
    }
};


void TApp::MouseDown( int button, int x, int y, int dx, int dy )
{
	TEvent::MouseDown(button,x,y,dx,dy);
	if ( button==1 )
    {
    	leftDown = true;
        startX = dx;
        startY = dy;
    }
};


void TApp::MouseUp( int button, int x, int y )
{
	TEvent::MouseUp(button,x,y);
	if ( button==1 )
    	leftDown = false;
};


void TApp::MouseUp( int button, int x, int y, int dx, int dy )
{
	TEvent::MouseUp(button,x,y,dx,dy);
	if ( button==1 )
    	leftDown = false;
};


void TApp::MouseMove( int x, int y )
{
	TEvent::MouseMove(x,y);
	if ( leftDown )
    {
		deltaYAngle = float(x);
		deltaXAngle = float(y);
    }
};


void TApp::MouseMove( int x, int y, int dx, int dy )
{
	TEvent::MouseMove(x,y,dx,dy);
	if ( leftDown )
    {
		deltaYAngle = float(dx);
		deltaXAngle = float(dy);
    }
};


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

		case 'r':
		case 'R':
			{
				lx = 0;
				ly = 0;
				lz = -2;
				break;
			}

		case 'p':
		case 'P':
			{
				if ( paper )
				{
					paper = false;
				    glClearColor( 0.0f,0.0f,0.0f,0 );
				}
				else
				{
					paper = true;
				    glClearColor( 0.75f,0.75f,0.75f,0 );
				}
				break;
			}

		case 'a':
		case 'A':
			{
				keys = keys & ~16;
				if ( cobject )
				{
					if ( currentObject>0 )
					{
						currentObject--;
					}
					else
						currentObject = cobj.NumObjects() - 1;
				}
				break;
			}

		case 'z':
		case 'Z':
			{
				keys = keys & ~32;
				break;
			}

		case 's':
		case 'S':
			{
				animationObj.AnimationType(1);
				animationObj.PauseAnimation(true);
				animationObj.PreviousAnimation();
				if ( cobject )
				{
					currentObject++;
					if ( currentObject >= cobj.NumObjects() )
						currentObject = 0;
				}
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


// try and read resolution settings from registry
bool TApp::GetRegistrySettings( void )
{
	if ( !TEvent::GetRegistrySettings() )
	{
		return false;
	}
	size_t lfps;
	if ( GetRegistryKey( "Software\\PDV\\Performance", "LimitFPS", lfps ) )
		if ( lfps==1 )
			limitFPS = true;
		else
			limitFPS = false;
	return true;
};


bool TApp::AddTransport( TLandscapeObject& obj )
{
	return false;
};


TCompoundObject* TApp::GetObjectByName( const TString&, size_t, size_t)
{
	return NULL;
};


//==========================================================================

