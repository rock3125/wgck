#include <precomp_header.h>

//==========================================================================

#define _TESTTRIANGLE

#ifdef _TESTTRIANGLE

bool inside = false;

float v1[] = { 400,130,0 };
float v2[] = { 200,100,0 };
float v3[] = { 100,100,0 };

float n[]  = { 0,0,-1 };


// is p inside triangle v1,v2,v3 with normal n?
bool InsideTriangle( const float*p, const float* n, const float* _v1, 
					 const float* _v2, const float* _v3 )
{
	float nv1[3];
	float or[3];
	float v1[3], v2[3], v3[3];

	float cx,cy,cz;
	float scale = 1.1f; // adjustment to slightly enlarge triangle to make it work better

	cx = (_v1[0] + _v2[0] + _v3[0]) * 0.3333f;
	cy = (_v1[1] + _v2[1] + _v3[1]) * 0.3333f;
	cz = (_v1[2] + _v2[2] + _v3[2]) * 0.3333f;

	v1[0] = cx + ( _v1[0] - cx ) * scale;
	v1[1] = cy + ( _v1[1] - cy ) * scale;
	v1[2] = cz + ( _v1[2] - cz ) * scale;

	v2[0] = cx + ( _v2[0] - cx ) * scale;
	v2[1] = cy + ( _v2[1] - cy ) * scale;
	v2[2] = cz + ( _v2[2] - cz ) * scale;

	v3[0] = cx + ( _v3[0] - cx ) * scale;
	v3[1] = cy + ( _v3[1] - cy ) * scale;
	v3[2] = cz + ( _v3[2] - cz ) * scale;

	// nv1 is a normal, the cross product of n x v1v2
	// a x b = < a1b2 - a2b1, a2b0 - a0b2, a0b1 - a1b0 >
	float v1v2[3];
	v1v2[0] = v2[0] - v1[0];
	v1v2[1] = v2[1] - v1[1];
	v1v2[2] = v2[2] - v1[2];
	nv1[0] = n[1]*v1v2[2] - n[2]*v1v2[1];
	nv1[1] = n[2]*v1v2[0] - n[0]*v1v2[2];
	nv1[2] = n[0]*v1v2[1] - n[1]*v1v2[0];

	// plugin p into the normal equation with v1 to get an orientation
	or[0] = nv1[0] * (p[0] - v1[0]) + 
			nv1[1] * (p[1] - v1[1]) +
			nv1[2] * (p[2] - v1[2]);

	// same for v2v3
	float v2v3[3];
	v2v3[0] = v3[0] - v2[0];
	v2v3[1] = v3[1] - v2[1];
	v2v3[2] = v3[2] - v2[2];
	nv1[0] = n[1]*v2v3[2] - n[2]*v2v3[1];
	nv1[1] = n[2]*v2v3[0] - n[0]*v2v3[2];
	nv1[2] = n[0]*v2v3[1] - n[1]*v2v3[0];

	// plugin p into the normal equation with v1 to get an orientation
	or[1] = nv1[0] * (p[0] - v2[0]) + 
			nv1[1] * (p[1] - v2[1]) +
			nv1[2] * (p[2] - v2[2]);

	// early return?
	if ( (or[0]>=0 && or[1]<0) || (or[0]<=0 && or[1]>0) )
		return false;

	// same for v3v1
	float v3v1[3];
	v3v1[0] = v1[0] - v3[0];
	v3v1[1] = v1[1] - v3[1];
	v3v1[2] = v1[2] - v3[2];
	nv1[0] = n[1]*v3v1[2] - n[2]*v3v1[1];
	nv1[1] = n[2]*v3v1[0] - n[0]*v3v1[2];
	nv1[2] = n[0]*v3v1[1] - n[1]*v3v1[0];

	// plugin p into the normal equation with v1 to get an orientation
	or[2] = nv1[0] * (p[0] - v3[0]) + 
			nv1[1] * (p[1] - v3[1]) +
			nv1[2] * (p[2] - v3[2]);

	return ( ( or[0]<=0 && or[1]<=0 && or[2]<=0 ) ||
		     ( or[0]>=0 && or[1]>=0 && or[2]>=0 ) );
};

#endif

//==========================================================================

size_t aiId = 0;

#include <parser/lex.h>
#include <parser/parserMain.h>

#include <common/string.h>

#include <common/compatability.h>
#include <object/binobject.h>
#include <object/landscape.h>
#include <object/compoundObject.h>
#include <object/anim.h>
#include <object/deffile.h>
#include <systemDialogs/gui.h>

#include <parser/imp3ds.h>
#include <lib3ds/file.h>

#include <conio.h>
#include <direct.h>
#include <process.h>
#include <Commdlg.h>

const float kPI = 3.1415927f;
const float deg2Rad = 0.01745329252f;

//==========================================================================
//
// Entry point of all Windows programs
//
#ifndef _MESA

int APIENTRY WinMain( HINSTANCE hInst,
			   		  HINSTANCE hPrevInstance,
			   		  LPSTR lpCmdLine,
			   		  int nCmdShow )
{
	PreInit(hInst);

	// create app
	TApp* app = new TApp( hInst );
    PostCond( app!=NULL );
	SetApp( app );

	if ( !PostInit() )
	{
		return -1;
	}
	return StartGameSystem();
}

#endif

//==========================================================================

TApp::TApp( HINSTANCE hInst )
	: TEvent( hInst ),
	  transform( NULL )
{
	whoStr = "Game Object Converter Version 1.28\nWritten by Peter de Vocht, 2002\n\n";
	cursorFlash = 0;
	x = 10;
	y = 10;
	numStrings = 0;
	fontHeight = 13;
	multiplier = 5;
	historyIndex = 0;
	numHistory = 0;

    posx = 0;
    posy = 0;
    posz = -5;

	yangle = 0;
	xangle = 0;
	deltaXAngle = 0;
	deltaYAngle = 0;
	currentObject = 0;
	keys = 0;
	showBoundingBox = false;
	leftDown = false;
	startX = 0;
	startY = 0;
	paper = false;

	binObj = NULL;
	compObj = NULL;
	landObj = NULL;
	animObj = NULL;

	WriteString( whoStr.c_str() );
	WriteString( "type 'help' to view a list of the available commands\n" );
};

TApp::~TApp( void )
{
	if ( transform!=NULL )
		delete transform;
	transform = NULL;

	ClearObjects();
};

void TApp::ClearObjects( void )
{
	if ( landObj!=NULL )
	{
		delete landObj;
	}
	landObj = NULL;
	if ( binObj!=NULL )
	{
		delete binObj;
	}
	binObj = NULL;
	if ( compObj!=NULL )
	{
		delete compObj;
	}
	compObj = NULL;
	if ( animObj!=NULL )
	{
		delete animObj;
	}
	animObj = NULL;
};

bool TApp::OpenObject( TString& _fname )
{
	char fname[1024];

	OPENFILENAME ofn;
	memset( &ofn, 0, sizeof(OPENFILENAME) );
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFilter  = "Compound Object\0*.obj\0Binary Object\0*.bin\0Landscape Object\0*.lnd\0Animation\0*.anm\0All Files\0*.*\0";
	ofn.hwndOwner = Window();
	ofn.lpstrFileTitle = fname;
	ofn.nMaxFileTitle = 1024;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

	if ( GetOpenFileName(&ofn)==TRUE )
	{
		char dir[MAX_PATH];
		::getcwd( dir, MAX_PATH );
		_fname = dir;
		_fname = _fname + "\\" + fname;
		return true;
	}
	return false;
};


bool TApp::SaveObject( TString& _fname )
{
	char fname[1024];

	OPENFILENAME ofn;
	memset( &ofn, 0, sizeof(OPENFILENAME) );
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFilter  = "Compound Object\0*.obj\0Binary Object\0*.bin\0\0";
	ofn.hwndOwner = Window();
	ofn.lpstrFileTitle = fname;
	ofn.nMaxFileTitle = 1024;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

	if ( GetSaveFileName(&ofn)==TRUE )
	{
		char dir[MAX_PATH];
		::getcwd( dir, MAX_PATH );
		_fname = dir;
		_fname = _fname + "\\" + fname;
		return true;
	}
	return false;
};


bool TApp::OpenXObject( TString& _fname )
{
	char fname[1024];

	OPENFILENAME ofn;
	memset( &ofn, 0, sizeof(OPENFILENAME) );
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFilter  = "X-file\0*.x\0All Files\0*.*\0";
	ofn.hwndOwner = Window();
	ofn.lpstrFileTitle = fname;
	ofn.nMaxFileTitle = 1024;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

	if ( GetOpenFileName(&ofn)==TRUE )
	{
		char dir[MAX_PATH];
		::getcwd( dir, MAX_PATH );
		_fname = dir;
		_fname = _fname + "\\" + fname;
		return true;
	}
	return false;
};


bool TApp::Open3DSObject( TString& _fname )
{
	char fname[1024];

	OPENFILENAME ofn;
	memset( &ofn, 0, sizeof(OPENFILENAME) );
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFilter  = "3ds file\0*.3ds\0All Files\0*.*\0";
	ofn.hwndOwner = Window();
	ofn.lpstrFileTitle = fname;
	ofn.nMaxFileTitle = 1024;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

	if ( GetOpenFileName(&ofn)==TRUE )
	{
		char dir[MAX_PATH];
		::getcwd( dir, MAX_PATH );
		_fname = dir;
		_fname = _fname + "\\" + fname;
		return true;
	}
	return false;
};


bool TApp::LoadObject( const TString& fname, const TString& path, const TString& texturePath,
					   TString& errStr )
{
	ClearObjects();
	CloseTransformWindow();
	binfilename = "";

	int index = strlen(fname.c_str())-4;
	if ( index>0 )
	{
		if ( stricmp(&fname[index],".lnd")==0 )
		{
			landObj = new TLandscape();
			WriteString( "Loading landscape object\n" );
			if ( !landObj->LoadBinary( fname, errStr, path, texturePath, this ) )
			{
			    return false;
			}
			landObj->RenderDepth(25);
			posy = -landObj->MaxY();
		}
		else if ( stricmp(&fname[index],".anm")==0 )
		{
			animObj = new TAnimation();
			WriteString( "Loading animation object\n" );
			if ( !animObj->LoadBinary( fname, errStr, path, texturePath ) )
			{
		    	return false;
			}
			animObj->StartAnimation( 0, 20 );
		}
		else if ( stricmp(&fname[index],".bin")==0 )
		{
			binObj = new TBinObject();
			WriteString( "Loading BIN object\n" );
			if ( !binObj->LoadBinary( fname, errStr, path, texturePath ) )
			{
		    	return false;
			}
			binfilename = fname;
			SetupTransformWindow();
		}
		else if ( stricmp(&fname[index],".obj")==0 )
		{
			compObj = new TCompoundObject();
			WriteString( "Loading compound object\n" );
			if ( !compObj->LoadBinary( fname, errStr, path, texturePath ) )
			{
		    	return false;
			}
		}
		else
		{
			errStr = "file extension not recognised as a game object\n";
			return false;
		}
	}
	return true;
};


void TApp::Draw( void )
{
	float height = (vheight-32);

	bool cursorOn = true;
	if ( cursorFlash < 5 )
	{
		cursorOn = false;
	}
	else if ( cursorFlash > 10 )
	{
		cursorFlash = 0;
	}

	if ( ::wglMakeCurrent( hdc, hglrc )==TRUE )
	{
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		RenderMenu(false);

		SetupGL3d( vdepth, vwidth, vheight );
		glLoadIdentity();

		glEnable( GL_COLOR_MATERIAL );
		glColor3ub(255,255,255);

		if ( landObj!=NULL )
		{
			glRotatef( xangle, 1,0,0 );
			glRotatef( yangle, 0,1,0 );
			glTranslatef( posx,posy,posz );

			landObj->Draw(posx,0,posz,yangle,xangle,0,0,false);
			landObj->Draw(posx,0,posz,yangle,xangle,0,0,true);
		}
		else if ( animObj!=NULL )
		{
			glTranslatef( posx,posy,posz );
			glRotatef( xangle, 1,0,0 );
			glRotatef( yangle, 0,1,0 );

			if ( animObj->AnimationPaused() )
			{
				char msg[256];
				sprintf( msg, "frame %d", animObj->AnimationIndex() );
				SetMessage( msg );
			}
			animObj->Draw();
		}
		else if ( binObj!=NULL )
		{
			glTranslatef( posx,posy,posz );
			glRotatef( xangle, 1,0,0 );
			glRotatef( yangle, 0,1,0 );

			binObj->Draw();
		}
		else if ( compObj!=NULL )
		{
			glTranslatef( posx,posy,posz );
			glRotatef( xangle, 1,0,0 );
			glRotatef( yangle, 0,1,0 );

			compObj->Draw( currentObject );

			if ( showBoundingBox )
			{
				float minx,miny,minz,maxx,maxy,maxz;
				compObj->GetBounds( currentObject, minx,miny,minz, maxx,maxy,maxz );
				DrawBoundingBox( minx,miny,minz, maxx,maxy,maxz, 255,255,255 );
			}
		}

		glEnable( GL_COLOR_MATERIAL );
		glColor3ub(255,255,255);

		// write text
		SetupGL2d( Depth(), Width(), Height() );
		glLoadIdentity();

#ifdef _TESTTRIANGLE

		glTranslatef( 0,0,-1);
		glBegin( GL_LINES );

			glVertex3fv( v1 );
			glVertex3fv( v2 );

			glVertex3fv( v2 );
			glVertex3fv( v3 );

			glVertex3fv( v3 );
			glVertex3fv( v1 );

			if ( inside )
			{
				glVertex3f( 20,20,0 );
				glVertex3f( 40,20,0 );

				glVertex3f( 30,20,0 );
				glVertex3f( 30,40,0 );
			}
			else
			{
				glVertex3f( 20,30,0 );
				glVertex3f( 40,30,0 );
			}

		glEnd();
#endif 


		if ( paper )
		{
			glColor3ub( 0,0,0 );
		}
		else
		{
			glColor3ub( 255,255,255 );
		}

		float lx = 10;
		float ly = 10;
		for ( size_t i=0; i<numStrings; i++ )
		{
			if ( lines[i].length() > 0 )
			{
				Write( lx, height-ly, -4, lines[i] );
			}
			ly = ly + fontHeight;
		}

		TString display = str;
		if ( cursorOn )
		{
			display = display + "_";		
		}

		Write( x,height-y, -4, display );
	
		// swap double buffer
		::SwapBuffers(hdc);
	}
	else
	{
		DWORD err = ::GetLastError();
	}

	::ValidateRgn( hWindow, NULL );
};


void TApp::Logic( void )
{
	TEvent::Logic();

	// transform menu buttons?
	if ( transform!=NULL )
	{
		TransformLogic();
	};

	::InvalidateRect( hWindow, NULL, false );
	cursorFlash++;

	float _yangle = yangle*deg2Rad;
	if ( landObj==NULL )
		_yangle = 0;
	
	// key codes
	float speed = 1.0f;
	if ( landObj==NULL )
		speed = 0.1f;

	float v1fcos = (float)cos(_yangle);
	float v1fsin = (float)sin(_yangle);

	float v2fcos = (float)cos(_yangle + 0.5f*kPI );
	float v2fsin = (float)sin(_yangle + 0.5f*kPI );

	float v3fcos = (float)cos(_yangle - 0.5f*kPI );
	float v3fsin = (float)sin(_yangle - 0.5f*kPI );

	// movement
	if ( shiftDown )
	{
		posx = posx + v1fcos*speed*deltaYAngle;
		posy = posy - v1fcos*speed*deltaXAngle;
	}
	else if ( ctrlDown )
	{
		posz = posz + v1fcos*speed*deltaXAngle;
		posx = posx - v1fsin*speed*deltaYAngle;
	}
	else
	{
		xangle += deltaXAngle;
		if ( landObj!=NULL )
		{
			if ( xangle > 80.0f )  xangle = 80.0f;
			if ( xangle < -80.0f ) xangle = -80.0f;
		};

		yangle += deltaYAngle;
		if ( yangle>360.0f ) yangle -= 360.0f;
		if ( yangle<0.0f )   yangle += 360.0f;
	}
	deltaXAngle = 0.0f;
	deltaYAngle = 0.0f;

	if ( keys & aUp )
	{
		posz = posz + v1fcos*speed;
		posx = posx - v1fsin*speed;
	}
	if ( keys & aDown )
	{
		posz = posz - v1fcos*speed;
		posx = posx + v1fsin*speed;
	}
	if ( keys & aLeft )
	{
		if ( landObj!=NULL )
		{
			posz = posz - v2fcos*speed;
			posx = posx + v2fsin*speed;
		}
		else
		{
			posz = posz + v2fcos*speed;
			posx = posx - v2fsin*speed;
		}
	}
	if ( keys & aRight )
	{
		if ( landObj!=NULL )
		{
			posz = posz - v3fcos*speed;
			posx = posx + v3fsin*speed;
		}
		else
		{
			posz = posz + v3fcos*speed;
			posx = posx - v3fsin*speed;
		}
	}
	if ( keys & 16 )
		posy = posy - speed*0.25f;
	if ( keys & 32 )
		posy = posy + speed*0.25f;
};

void TApp::Font( HFONT _font )
{
	font = _font;
};

void TApp::KeyDown( size_t kd )
{
	TEvent::KeyDown( kd );
	switch ( kd )
	{
		case VK_LEFT:
			{
				if ( ctrlDown )
				{
					keys = keys | aLeft;
				}
				break;
			}

		case VK_RIGHT:
			{
				if ( ctrlDown )
				{
					keys = keys | aRight;
				}
				break;
			}

		case VK_UP:
			{
				if ( ctrlDown )
				{
					keys = keys | aUp;
				}
				else if ( shiftDown )
				{
					keys = keys | 16;
				}
				break;
			}

		case VK_DOWN:
			{
				if ( ctrlDown )
				{
					keys = keys | aDown;
				}
				else if ( shiftDown )
				{
					keys = keys | 32;
				}
				break;
			}
	}
};

void TApp::KeyUp( size_t keyUp )
{
	TEvent::KeyUp( keyUp );

	switch ( keyUp )
	{
		case VK_LEFT:
			{
				keys = keys & ~1;
				break;
			}

		case VK_RIGHT:
			{
				keys = keys & ~2;
				break;
			}

		case VK_UP:
			{
				if ( ctrlDown )
				{
					keys = keys & ~4;
				}
				else if ( shiftDown )
				{
					keys = keys & ~16;
				}
				else
				{
					if ( historyIndex > 0 )
					{
						historyIndex--;
					}
					str = history[historyIndex];
				}
				break;
			}

		case VK_DOWN:
			{
				if ( ctrlDown )
				{
					keys = keys & ~8;
				}
				else if ( shiftDown )
				{
					keys = keys & ~32;
				}
				else
				{
					if ( historyIndex < numHistory )
					{
						historyIndex++;
					}
					str = history[historyIndex];
				}
				break;
			}
	}
};

void TApp::KeyPress( size_t key )
{
	if ( transform!=NULL )
	{
		TEvent::KeyPress(key);
	}
	else
	{
		if ( key>=32 && key<=127 )
		{
			str = str + TString(char(key));
		}
		if ( key==13 )
		{
			// shift up?
			if ( (y+fontHeight*multiplier) > vheight )
			{
				for ( size_t i=0; i<kMaxStrings-1; i++ )
				{
					lines[i] = lines[i+1];
				}
				lines[numStrings] = str;
			}
			else
			{
				lines[numStrings++] = str;
				y = y + fontHeight;
			}
			AddToHistory( str );
			ProcessInput( str );
			str = "";
			x = 10;
		}
		if ( key==27 )
		{
			str = "";
			x = 10;
		}
		if ( key==8 )
		{
			if ( str.length() > 1 )
			{
				str = str.substr(0,str.length()-1);
			}
			else
			{
				str = "";
			}
		}
	}
};

void TApp::WriteString( const char* fmt, ... )
{
	char buf[1024];

	va_list ap;

	va_start(ap, fmt );
    vsprintf( buf, fmt, ap );
	va_end(ap);

	// write line by line according to CR
	char line[1024];
	size_t index = 0;
	size_t local = 0;
	while ( buf[index]!=0 )
	{
		if ( buf[index]>=32 && buf[index]<=127 )
		{
			line[local++] = buf[index];
		}
		else if ( buf[index]==13 || buf[index]==10 )
		{
			line[local] = 0;
			local = 0;
			ParserWrite( line );
		}
		index++;
	}
	if ( local>0 )
	{
		line[local] = 0;
		ParserWrite( line );
	}
};

void TApp::ParserWrite( const TString& str )
{
	// shift up?
	if ( (y+fontHeight*multiplier) > vheight )
	{
		for ( size_t i=0; i<kMaxStrings-1; i++ )
		{
			lines[i] = lines[i+1];
		}
		lines[numStrings] = str;
	}
	else
	{
		lines[numStrings++] = str;
		y = y + fontHeight;
	}
	x = 10;
};

void TApp::DoTransform( float* matrix,
					    float tx, float ty, float tz,
					    float rx, float ry, float rz,
					    float sx, float sy, float sz )
{
	glPushMatrix();
		glLoadIdentity();
		glTranslatef( tx, ty, tz );
		glRotatef( rz, 0,0,1 );
		glRotatef( ry, 0,1,0 );
		glRotatef( rx, 1,0,0 );
		glScalef( sx, sy, sz );
		glGetFloatv( GL_MODELVIEW_MATRIX, matrix );
	glPopMatrix();

	WriteString( "Transformation Matrix\n" );
	for ( size_t i=0; i<4; i++ )
	{
		WriteString( "%2.4f %2.4f %2.4f %2.4f\n",
					 matrix[i*4+0], matrix[i*4+1],
					 matrix[i*4+2], matrix[i*4+3] );
	}
};

bool TApp::DoBatch( const TString& fname )
{
   	WriteString( ">>> executing batch file \"%s\"\n", fname.c_str() );
	FILE* batchFile = fopen( fname.c_str(), "rb" );
	if ( batchFile!=NULL )
	{
		bool done = false;
		while ( !feof(batchFile) && !done )
		{
			WriteString( "\n" );
//			done = ProcessInput();
		}
		fclose( batchFile );
		batchFile = NULL;

		return done;
	}
	else
		WriteString( "can't open \"%s\"\n", fname.c_str() );
	return false;
};


void TApp::MouseDown( int button, int x, int y)
{
	TEvent::MouseDown(button,x,y);
	if ( button==1 && transform==NULL )
    {
    	leftDown = true;
        startX = x;
        startY = y;
    }
	else if ( button==2 )
	{
		ProcessInput( "show" );
	}
};


void TApp::MouseDown( int button, int x, int y, int dx, int dy )
{
	TEvent::MouseDown(button,x,y,dx,dy);
	if ( button==1 && transform==NULL )
    {
    	leftDown = true;
        startX = dx;
        startY = dy;
    }
};


void TApp::MouseUp( int button, int x, int y )
{
	TEvent::MouseUp(button,x,y);
	if ( button==1 && transform==NULL )
    	leftDown = false;
};


void TApp::MouseUp( int button, int x, int y, int dx, int dy )
{
	TEvent::MouseUp(button,x,y,dx,dy);
	if ( button==1 && transform==NULL )
    	leftDown = false;
};


void TApp::MouseMove( int x, int y )
{
	TEvent::MouseMove(x,y);
	if ( leftDown && transform==NULL )
    {
		deltaYAngle = float(x);
		deltaXAngle = float(y);
    }
};

void TApp::MouseMove( int x, int y, int dx, int dy )
{
#ifdef _TESTTRIANGLE
	float p[3];
	p[0] = float(x);
	p[1] = (vheight-24) - float(y);
	p[2] = 0;
	inside = ( InsideTriangle( p,n, v1,v2,v3 ) );
#endif

	TEvent::MouseMove(x,y,dx,dy);
	if ( leftDown && transform==NULL )
    {
		deltaYAngle = float(dx);
		deltaXAngle = float(dy);
    }
};

bool TApp::ProcessInput( const TString& line )
{
	bool finish = false;
    if ( line=="end" || line=="bye" ||
    	 line=="exit" || line=="quit" )
    {
        finish = true;
		::PostQuitMessage(-1);
    }
    else if ( line=="help" )
	{
		x = 10;
		y = 10;
		numStrings = 0;
        Help();
	}
    else if ( line=="cls" )
    {
		x = 10;
		y = 10;
		numStrings = 0;
    }
	else if ( line=="save" )
	{
		if ( binObj!=NULL )
		{
			TString fname;
			if ( SaveObject( fname ) )
			{
				binObj->SaveBinary( fname );
			}
		}
		if ( compObj!=NULL )
		{
			TString fname;
			if ( SaveObject( fname ) )
			{
				compObj->SaveBinary( fname, "data\\textures" );
			}
		}
	}
	else if ( line=="saveshadow" )
	{
		if ( binObj!=NULL && binfilename.length()>0 )
		{
			TString fname = binfilename.GetItem('.',0) + ".shd";
			TString errStr;
			if ( !binObj->SaveAsShadow( fname, errStr ) )
			{
				WriteString( "Error: %s\n", errStr.c_str() );
			}
			else
			{
				WriteString( "Created shadow file named \"%s\"\n", fname.c_str() );
			}
		}
		else
		{
			WriteString( "no binobject loaded to convert\n" );
		}
	}
    else if ( strncmp( line.c_str(), "show", 4 )==0 )
    {
		TString temp;
		TString fname, path, texturePath;
		TString p1, p2;

		if ( line.length() > 4 )
		{
			temp = &line[5];
			p1 = temp.GetItem( ' ', 0 );
			p2 = temp.GetItem( ' ', 1 );
		}

		if ( p1.length()==0 || p2.length()==0 )
		{
			if ( OpenObject( p1 ) )
			{
				p2 = "data\\textures";
			}
			else
			{
				return finish;
			}
		}
		::SplitPath( p1, path, fname );
		texturePath = p2;

		TString errStr;
		if ( !LoadObject( fname, path, texturePath, errStr ) )
		{
			WriteString( errStr.c_str() );
			ClearObjects();
		}
		posx = 0;
		posy = 0;
		posz = -5;

		yangle = 0;
		xangle = 0;
    }
	else if ( strcmp( line.c_str(), "reset" )==0 )
	{
		posx = 0;
		posy = 0;
		posz = -5;
	}
	else if ( strcmp( line.c_str(), "prev" )==0 )
	{
		if ( compObj!=NULL )
		{
			if ( currentObject>0 )
			{
				currentObject--;
			}
			else
				currentObject = compObj->NumObjects() - 1;
		}
	}
	else if ( strcmp( line.c_str(), "next" )==0 )
	{
		if ( compObj!=NULL )
		{
			currentObject++;
			if ( currentObject >= compObj->NumObjects() )
				currentObject = 0;
		}
	}
	else if ( strcmp( line.c_str(), "start anim" )==0 )
	{
		if ( animObj!=NULL )
		{
			animObj->AnimationType(1);
			animObj->PauseAnimation(true);
			animObj->PreviousAnimation();
		}
	}
	else if ( strcmp( line.c_str(), "pause anim" )==0 )
	{
		if ( animObj!=NULL )
		{
			animObj->AnimationType(1);
			animObj->PauseAnimation(true);
			animObj->NextAnimation();
		}
	}
	else if ( strcmp( line.c_str(), "cont anim" )==0 )
	{
		if ( animObj!=NULL )
		{
			animObj->PauseAnimation(false);
			animObj->StartAnimation();
		}
	}
	else if ( strcmp( line.c_str(), "paper" )==0 )
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
	}
	else if ( strcmp( line.c_str(), "box" )==0 )
	{
		showBoundingBox = !showBoundingBox;
	}
    else if ( strncmp( line.c_str(), "batch ", 6 )==0 )
    {
		finish = DoBatch( &line[6] );
    }
    else if ( strncmp( line.c_str(), "createmesh ", 11 )==0 )
    {
		Lex lex;
		TString errStr;
		TString temp;
		temp = &line[11];

		TString fname, strip, seaStr;
		fname = temp.GetItem( ' ', 0 );
		strip = temp.GetItem( ' ', 1 );
		seaStr = temp.GetItem( ' ', 2 );
		if ( fname.length()==0 )
		{
			WriteString( "\nError - first parameter must be bitmap name\n" );
			return finish;
		}
		if ( strip.length()==0 )
		{
			WriteString( "\nError - needs second parameter, strip file .bmp\n" );
			return finish;
		}
		if ( seaStr.length()==0 )
		{
			WriteString( "\nError - needs third parameter, integer seaHeight\n" );
			return finish;
		}
		size_t seaHeight = atoi( seaStr.c_str() );
		if ( !lex.CreateMesh( fname, strip, seaHeight, errStr ) )
		{
			WriteString( errStr.c_str() );
		}
		return finish;
    }
    else if ( line == "conv3ds" )
	{
		TString fname;
		if ( Open3DSObject( fname ) )
		{
			Lib3dsFile* obj3ds = lib3ds_file_load( fname.c_str() );
			if ( obj3ds!=NULL )
			{
				CloseTransformWindow();
				if ( binObj!=NULL )
				{
					delete binObj;
				}

				binObj = NULL;
				binObj = ToBin( fname, obj3ds );
				binObj->CenterAxes( true, true, true );
				binObj->Normalise();

				binfilename = fname.GetItem('.',0) + ".bin";
				SetupTransformWindow();
			}
			delete obj3ds;
		}
	}
    else if ( strncmp( line.c_str(), "convert", 7 )==0 || 
			  strncmp( line.c_str(), "convx",5 )==0 )
    {
   		Lex lex;

		binfilename = "";
		if ( line.length()==7 || line=="convx" )
		{
			TString fname, path;
			if ( OpenXObject( fname ) )
			{
				path = "data\\textures";

				CloseTransformWindow();
				if ( binObj!=NULL )
				{
					delete binObj;
				}

				TString str = fname + " " + path;
				str = str + " -centerx -centery -centerz -normalise";
				binObj = lex.DirectX( str );
				if ( binObj!=NULL )
				{
					binfilename = fname.GetItem('.',0) + ".bin";
					SetupTransformWindow();
				}
				else
        			WriteString( "%s\n", lex.ErrorString() );
			}
		}
		else
		{
			WriteString( "converting file \"%s\"\n", &line[8] );
			TBinObject* obj = lex.DirectX( &line[8] );
			if ( obj!=NULL )
			{
        		TString newFname = &line[8];
				newFname = newFname.GetItem('.',0) + ".bin";
        		WriteString( "saving as \"%s\"\n", newFname.c_str() );

        		if ( !obj->SaveBinary( newFname ) )
					WriteString( "error writing file %s\n", newFname.c_str() );
				//delete obj;
			}
			else
        		WriteString( "%s\n", lex.ErrorString() );
		}
    }
    else if ( strncmp( line.c_str(), "rename ", 7 )==0 )
    {
		TString str = &line[7];
		TString fname = str.GetItem(' ',0);
		TString to = str.GetItem(' ',1);

		if ( fname.length()==0 || to.length()==0 )
		{
			WriteString( "rename takes two parameters, file to rename from and new filename\n" );
		}
		else
		{
			if ( rename( fname.c_str(), to.c_str() )==0 )
			{
				WriteString( "success\n" );
			}
			else
			{
				WriteString( "FAILED\n" );
			}
		}
	}
    else if ( strncmp( line.c_str(), "delete ", 7 )==0 )
    {
		TString str = &line[7];
		if ( remove( str.c_str() )==0 )
		{
			WriteString( "success\n" );
		}
		else
		{
			WriteString( "FAILED\n" );
		}
	}
    else if ( strncmp( line.c_str(), "copy ", 5 )==0 )
    {
		TString str = &line[5];
		TString fname = str.GetItem(' ',0);
		TString to = str.GetItem(' ',1);

		if ( fname.length()==0 || to.length()==0 )
		{
			WriteString( "copy takes two parameters, file to copy from and new filename\n" );
		}
		else
		{
			FILE* fh = fopen( fname.c_str(), "rb" );
			if ( fh==NULL )
			{
				WriteString( "Error opening file %s\n", fname.c_str() );
			}
			else
			{
				FILE* fto = fopen( to.c_str(), "wb" );
				if ( fto==NULL )
				{
					WriteString( "Error opening file %s for writing\n", to.c_str() );
				}
				else
				{
					size_t bufSize = 1024;
					size_t numRead = 0;
					char buf[1024];
					do
					{
						numRead = fread( buf, 1, bufSize, fh );
						if ( numRead>0 )
							fwrite(buf,1,numRead,fto);
					}
					while (numRead==bufSize);
					fclose(fto);
					WriteString( "success\n" );
				}
				fclose(fh);
			}
		}
	}
    else if ( strncmp( line.c_str(), "convert2 ", 9 )==0 )
    {
        WriteString( "converting file \"%s\"\n", &line[9] );
    	Lex lex;
        TBinObject* obj = lex.DirectX( &line[9] );
        if ( obj!=NULL )
        {
        	TString newFname = &line[9];
			newFname = newFname.GetItem('.',0) + ".bin";
			// strip path from newFname
			size_t i = newFname.length();
			while ( i>0 && newFname[i]!='\\' ) i--;
			if ( newFname[i]=='\\' )
				newFname = newFname.substr(i+1);

			TString prefixpath = &line[9];
			prefixpath = prefixpath.GetItem(' ',2);
			if ( prefixpath.length() > 0 )
			{
				if ( prefixpath[prefixpath.length()]!='\\' )
					prefixpath = prefixpath + "\\";
				newFname = prefixpath + newFname;
			};
        	WriteString( "saving as \"%s\"\n", newFname.c_str() );

        	if ( !obj->SaveBinary( newFname ) )
				WriteString( "error writing file %s\n", newFname.c_str() );
            delete obj;
        }
        else
        	WriteString( "%s\n", lex.ErrorString() );
    }
	else if ( strncmp( line.c_str(), "parsegltext ", 12 )==0 )
	{
		TString temp = &line[12];

		TString inname = temp.GetItem(' ',0);
		TString reffile = temp.GetItem(' ',1);
		if ( inname.length()==0 || reffile.length()==0 )
		{
        	WriteString( "Error: takes 2 parameters, infile.txt, reference file.bin\n" );
		}
		else
		{
			TString errStr;
			TBinObject obj;
			TBinObject refObj;
			if ( !refObj.LoadBinary( reffile, errStr, "data\\binfiles", "data\\textures" ) )
			{
				WriteString( "%s\n", errStr.c_str() );
			}
			else if ( !ImportOpenGL( inname, obj, &refObj, errStr ) )
			{
				WriteString( "%s\n", errStr.c_str() );
			}
			else
			{
				TString newfname = inname.GetItem('.',0) + ".x";
				if ( !obj.SaveText( newfname, errStr ) )
				{
					WriteString( "%s\n", errStr.c_str() );
				}
			}
		}
	}
    else if ( strncmp( line.c_str(), "createobj ", 10 )==0 )
    {
		TString temp;
		TString objName;
		TString texturePath;

		TString names[256];
		size_t j;
		for ( j=0; j<256; j++ )
			names[j] = "";

		temp = &line[10];
		size_t count = temp.NumItems(' ');
		for ( j=0; j<count; j++ )
		{
			TString ttemp = temp.GetItem(' ',j);
			if ( j==0 )
			{
				objName = ttemp;
			}
			else if ( j==1 )
			{
				texturePath = ttemp;
			}
			else
			{
				names[j-2] = ttemp;
			}
		}

		if ( j<3 )
		{
			WriteString( "\nError: takes at least 3 parameters, objectName, texturePath, objects...\n" );
			return finish;
		}
		WriteString( "Creating Compound object %s\n", objName.c_str() );
		TCompoundObject obj;
		obj.Create( objName, count-2, texturePath, (TString*)names );
	}
    else if ( strncmp( line.c_str(), "createnobj ", 11 )==0 )
    {
		TString temp;
		TString objName;
		TString texturePath;

		TString names[256];
		size_t j;
		for ( j=0; j<256; j++ )
			names[j] = "";

		temp = &line[11];
		size_t count = temp.NumItems(' ');
		for ( j=0; j<count; j++ )
		{
			TString ttemp = temp.GetItem(' ',j);
			if ( j==0 )
			{
				objName = ttemp;
			}
			else if ( j==1 )
			{
				texturePath = ttemp;
			}
			else
			{
				names[j-2] = ttemp;
			}
		}

		if ( j<3 )
		{
			WriteString( "\nError: takes at least 3 parameters, objectName, texturePath, objects...\n" );
			return finish;
		}
		WriteString( "Creating Normalised Compound object %s\n", objName.c_str() );
		TCompoundObject obj;
		obj.Create( objName, count-2, texturePath, (TString*)names );

		TCompoundObject obj2;

		// strip path
		TString fname, path, errStr;
		size_t index = objName.length()-1;
		while ( index>0 && objName[index]!='\\' ) index--;
		if ( index>0 )
		{
			path = objName.substr(0,index);
			fname = objName.substr(index+1);
		}
		else
		{
			fname = objName;
		}
		if ( !obj2.LoadBinary( fname, errStr, path, texturePath ) )
		{
			WriteString( "Error: %s\n", errStr.c_str() );
		}
		obj2.Normalise();
		obj2.SaveBinary( objName, texturePath );
	}
	else if ( strncmp( line.c_str(), "transform", 9 )==0 )
	{
		if ( binObj!=NULL && transform==NULL )
		{
			SetupTransformWindow();
		}
	}
	else if ( strncmp( line.c_str(), "transformbin", 12 )==0 )
	{
		if ( line.length() <=13 )
		{
		}
		else
		{
			TString fileName;
			TString temp = &line[13];
			TString path;
			TString fname = temp.GetItem(' ',0);
			SplitPath( fname, path, fileName );
			TString tpath = temp.GetItem(' ',1);

			TString trn = temp.GetItem(' ',2);
			TString rot = temp.GetItem(' ',3);
			TString scl = temp.GetItem(' ',4);

			trn = trn.GetItem('=',1);
			rot = rot.GetItem('=',1);
			scl = scl.GetItem('=',1);

			if ( fileName.length()==0 || tpath.length()==0 ||
				 trn.length()==0 || rot.length()==0 || scl.length()==0 )
			{
				WriteString( "\nError: incorrect number of parameters\nType help from more information\n" );
				return finish;
			}

			TBinObject obj;
			TString errStr;
			if ( !obj.LoadBinary( fileName, errStr, path, tpath ) )
			{
				WriteString( "\nError: %s\n", errStr.c_str() );
			}

			float tx,ty,tz;
			float rx,ry,rz;
			float sx,sy,sz;
			float matrix[16];

			tx = float(atof(trn.GetItem(',',0).c_str()));
			ty = float(atof(trn.GetItem(',',1).c_str()));
			tz = float(atof(trn.GetItem(',',2).c_str()));

			rx = float(atof(rot.GetItem(',',0).c_str()));
			ry = float(atof(rot.GetItem(',',1).c_str()));
			rz = float(atof(rot.GetItem(',',2).c_str()));

			sx = float(atof(scl.GetItem(',',0).c_str()));
			sy = float(atof(scl.GetItem(',',1).c_str()));
			sz = float(atof(scl.GetItem(',',2).c_str()));

			DoTransform( matrix, tx,ty,tz, rx,ry,rz, sx,sy,sz );
			obj.MatrixMult( matrix );
			TString newFname = "t" + fileName;
			ConstructPath( newFname, path, newFname );
			if ( obj.SaveBinary( newFname ) )
			{
				WriteString( "done\n" );
			}
		}
	}
    else if ( strncmp( line.c_str(), "createanim ", 11 )==0 )
    {
		TString temp = &line[11];
		TString fname,anim,texturePath,numAnimsStr;
		size_t numAnims;

		fname = temp.GetItem(' ',0);
		anim = temp.GetItem(' ',1);
		texturePath = temp.GetItem(' ',2);
		numAnimsStr = temp.GetItem(' ',3);

		numAnims = atoi(numAnimsStr.c_str());
		if ( numAnims<=0 )
		{
			WriteString( "this command takes 4 parameters.  Type 'help'\n" );
			WriteString( "to find out what these parameters are.\n" );
			WriteString( "fourth parameter must be a number >= 1\n" );
			return finish;
		}

		TAnimation animation;
		TString errStr;
		if ( !animation.CreateAnimation( anim, texturePath, numAnims, errStr ) )
		{
			WriteString( errStr.c_str() );
			return finish;
		}

		if ( !animation.SaveBinary( fname ) )
		{
			WriteString( "could not save as \"%s\"\n", fname.c_str() );
		}
		else
		{
			WriteString( "done\n" );
		}
	}
	else if ( strncmp( line.c_str(), "matrix ", 7 )==0 )
	{
		TString temp = &line[7];
		TString trn = temp.GetItem(' ',0);
		TString rot = temp.GetItem(' ',1);
		TString scl = temp.GetItem(' ',2);
		if ( trn.length()==0 ||
			 rot.length()==0 ||
			 scl.length()==0 )
		{
			WriteString( "Error: takes 3 parameters\n" );
			return finish;
		}
		trn = trn.GetItem('=',1);
		rot = rot.GetItem('=',1);
		scl = scl.GetItem('=',1);

		float tx,ty,tz;
		float rx,ry,rz;
		float sx,sy,sz;
		float matrix[16];

		tx = float(atof(trn.GetItem(',',0).c_str()));
		ty = float(atof(trn.GetItem(',',1).c_str()));
		tz = float(atof(trn.GetItem(',',2).c_str()));

		rx = float(atof(rot.GetItem(',',0).c_str()));
		ry = float(atof(rot.GetItem(',',1).c_str()));
		rz = float(atof(rot.GetItem(',',2).c_str()));

		sx = float(atof(scl.GetItem(',',0).c_str()));
		sy = float(atof(scl.GetItem(',',1).c_str()));
		sz = float(atof(scl.GetItem(',',2).c_str()));

		DoTransform( matrix, tx,ty,tz, rx,ry,rz, sx,sy,sz );
	}
	else if ( strncmp( line.c_str(), "mdltobin ", 9 )==0 )
	{
		TString temp = &line[9];
		TString fname = temp.GetItem(' ',0);
		TString charName = temp.GetItem('\"',1);

		if ( fname.length()==0 || charName.length()==0 )
		{
			WriteString( "Error: incorrect number of parameters\n" );
			return finish;
		}

		// load mdl into memory
		TPersist file(fileRead);
		if ( !file.FileOpen( fname ) )
		{
			WriteString( "Error: can't open file \"%s\"\n", fname.c_str() );
			return finish;
		}
		size_t fs = file.FileSize() - 4;
		char id[4];
		file.FileRead( id,4 );
		if ( strncmp(id,"IDST",4)!=0 &&
			 strncmp(id,"IDSQ",4)!=0 )
		{
			file.FileClose();
			WriteString( "Error: \"%s\" is not an MDL file\n", fname.c_str() );
			return finish;
		}
		char* buf = new char[fs];
		PostCond( buf!=NULL );
		file.FileRead( buf, fs );
		file.FileClose();

		// write new file
		TPersist outfile(fileWrite);
		TString outfname = "ch_" + fname.GetItem('.',0) + ".bin";
		if ( !outfile.FileOpen( outfname ) ) 
		{
			delete buf;
			WriteString( "Error: \"%s\" can't open for write\n", outfname.c_str() );
			return finish;
		}
		// new header
		TString header = "PDVCHAR01";
		outfile.FileWrite( header.c_str(), header.length()+1 );
		// character name
		outfile.FileWrite( charName.c_str(), charName.length()+1 );
		// old mdl
		outfile.FileWrite( buf, fs );
		outfile.FileClose();
		WriteString( "Character converted to \"%s\"\n", outfname.c_str() );
	}
    return finish;
};


void TApp::Help( void )
{
    WriteString( "======================================================================\n");
	WriteString( whoStr.c_str() );
    WriteString( "======================================================================\n");
    WriteString( "end (or bye, or exit)\n");
    WriteString( "help\n");
    WriteString( "cls\n");
	WriteString( "convx\n" );
	WriteString( "conv3ds\n" );
	WriteString( "transform\n" );
	WriteString( "saveshadow\n" );
/*
    WriteString( "batch filename.txt\n");
    WriteString( "parsegltext import.txt reffile.bin\n");
    WriteString( "createobj objName texturepath file1.bin ...fileX.bin\n");
    WriteString( "createnobj objName texturepath file1.bin ...fileX.bin\n");
    WriteString( "convert filename.x texturepath [-merge]\n");
    WriteString( "        [-centerx] [-centery] [-centerz]\n");
    WriteString( "        [-normalise] [-createnormals]\n");
	WriteString( "        [-revw] [-minx0] [-miny0]\n");
	WriteString( "        [-minz0] [-maxx0] [-maxy0] [-maxz0]\n");
	WriteString( "        [-bounds(x1,y1,z1,x2,y2,z2)]\n");
	WriteString( "        [-flipuv] [-scale=#]\n");
    WriteString( "convert2 filename.x texturepath destPath ... \n");
    WriteString( "createanim filename.anm anim%%%%d.bin texturePath numAnims\n");
    WriteString( "show filename[.bin|.lnd|.anm|.obj]\n");
	WriteString( "createmesh land24.bmp strip.bmp seaHeight\n" );
	WriteString( "transformbin fname.bin texturePath t=x,y,z r=x,y,z s=x,y,z\n" );
	WriteString( "matrix t=x,y,z r=x,y,z s=x,y,z\n" );
*/
	WriteString( "mdltobin fname.mdl \"Character name\"\n" );
	WriteString( "rename file1 file2\n" );
	WriteString( "delete fileName\n" );
	WriteString( "copy file1 file2\n" );
	WriteString( "object commands: reset, prev, next, start anim, pause anim,\n" );
	WriteString( "cont anim, paper, box\n" );
    WriteString( "======================================================================\n");
    WriteString( "\n");
};

void TApp::AddToHistory( const TString& str )
{
	history[numHistory++] = str;
	if ( numHistory >= kMaxStrings )
	{
		numHistory = numHistory / 2;
		for ( size_t i=0; i<numHistory; i++ )
		{
			history[i] = history[i+numHistory];
		}
	}
	historyIndex = numHistory;
};

void TApp::CloseTransformWindow( void )
{
	if ( transform!=NULL )
		delete transform;
	transform = NULL;
	CurrentPage( NULL );
};

void TApp::SetupTransformWindow( void )
{
	float d = -2;

	// setup transform page
	transform = new TGUI( this, guiWindow, 0,0,270,270, d, "Transform Object" );
	transform->SetLocation( 100, 100 );
	showMenu = true;
	CurrentPage( transform );

	AddItem( transform, new TGUI( this, guiLabel, 20,30,80,16, d, "Translate", 255,255,255 ) );
	editTX = new TGUI( this, guiEdit,  50,50,50,16, d, "0" );
	editTY = new TGUI( this, guiEdit, 110,50,50,16, d, "0" );
	editTZ = new TGUI( this, guiEdit, 170,50,50,16, d, "0" );

	AddItem( transform, new TGUI( this, guiLabel, 20,80,80,16, d, "Rotate", 255,255,255 ) );
	editRX = new TGUI( this, guiEdit,  50,100,50,16, d, "0" );
	editRY = new TGUI( this, guiEdit, 110,100,50,16, d, "0" );
	editRZ = new TGUI( this, guiEdit, 170,100,50,16, d, "0" );

	AddItem( transform, new TGUI( this, guiLabel, 20,130,80,16, d, "Scale", 255,255,255 ) );
	editSX = new TGUI( this, guiEdit,  50,150,50,16, d, "1" );
	editSY = new TGUI( this, guiEdit, 110,150,50,16, d, "1" );
	editSZ = new TGUI( this, guiEdit, 170,150,50,16, d, "1" );

	flipuvButton = new TGUI( this, guiButton, 20,180, 50,25, d, "flip UV" );

	resetButton = new TGUI( this, guiButton, 20,210, 50,25, d, "Reset" );
	applyButton = new TGUI( this, guiButton, 80,210, 50,25, d, "Apply" );
	saveButton  = new TGUI( this, guiButton, 140,210, 50,25, d, "Save" );
	closeButton = new TGUI( this, guiButton, 200,210, 50,25, d, "Close" );

	AddItem( transform, editTX );
	AddItem( transform, editTY );
	AddItem( transform, editTZ );

	AddItem( transform, editRX );
	AddItem( transform, editRY );
	AddItem( transform, editRZ );

	AddItem( transform, editSX );
	AddItem( transform, editSY );
	AddItem( transform, editSZ );

	AddItem( transform, flipuvButton );

	AddItem( transform, resetButton );
	AddItem( transform, applyButton );
	AddItem( transform, saveButton );
	AddItem( transform, closeButton );
};

void TApp::TransformLogic( void )
{
	if ( closeButton->Clicked() )
	{
		CloseTransformWindow();
		return;
	}
	if ( flipuvButton->Clicked() && binObj!=NULL )
	{
		binObj->FlipUV();
	}
	if ( saveButton->Clicked() )
	{
		if ( binfilename.length()>0 && binObj!=NULL )
		{
			WriteString( "Saving object as \"%s\"\n", binfilename.c_str() );
			if ( !binObj->SaveBinary( binfilename ) )
			{
				WriteString( "error saving file \"%s\"\n", binfilename.c_str() );
			}
		}
	}
	if ( resetButton->Clicked() )
	{
		posx = 0;
		posy = 0;
		posz = -5;

		yangle = 0;
		xangle = 0;
		return;
	}
	if ( applyButton->Clicked() && binObj!=NULL )
	{
		float tx = float(atof(editTX->Text().c_str()));
		float ty = float(atof(editTY->Text().c_str()));
		float tz = float(atof(editTZ->Text().c_str()));

		float rx = float(atof(editRX->Text().c_str()));
		float ry = float(atof(editRY->Text().c_str()));
		float rz = float(atof(editRZ->Text().c_str()));

		float sx = float(atof(editSX->Text().c_str()));
		float sy = float(atof(editSY->Text().c_str()));
		float sz = float(atof(editSZ->Text().c_str()));

		float matrix[16];
		DoTransform( matrix, tx,ty,tz, rx,ry,rz, sx,sy,sz );
		binObj->MatrixMult( matrix );
	}
};

//==========================================================================
