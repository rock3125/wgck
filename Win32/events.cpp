#include <precomp_header.h>

#include <direct.h>
#include <process.h>
#include <shellapi.h>
#include <io.h>

#include <win32/win32.h>
#include <win32/events.h>
#include <jpeglib/tjpeg.h>

//==========================================================================
//
//	TEvent is a base class to the actual TApp that is created
//	in win32.  All its behaviour you're interested in should be 
//	overwritten by TApp and implemented according to taste.
//	DO NOT modify TEvent itself!
//
TEvent::TEvent( HINSTANCE _hInstance, bool _fog )
	: initialised(false),
	  hInstance( _hInstance ),
      fog( _fog ),
	  arial(NULL),
#ifdef _DXSOUND_
	  soundSystem(NULL),
#endif
	  hdc( NULL ),
	  hglrc( NULL ),

	  currentPage(NULL),

#ifndef _PARSER
	  prevPage1(NULL),
	  prevPage2(NULL),
	  mainMenu( NULL ),
	  videoSettingsButton( NULL ),
	  hostButton( NULL ),
	  singlePlayerButton( NULL ),
	  joinButton( NULL ),
	  controlsButton( NULL ),
	  aboutButton( NULL ),
	  downloadButton( NULL ),
	  exitButton( NULL ),
#endif
      hWindow( NULL )
{
	gameType = 0;

	expired = false;
	showMenu = false;
	showStory = false;
	antiAlias = true;
	isSoftware = false;
	newInstallation = false;
	shadowLevel = 0;
	resolutionCounter = 0;
	languageId = 0;

	dialog1 = NULL;
	dialog2 = NULL;

	mainMenuDepth = -100;

#ifdef _VIEWER
	versionString = "Excession Engine v 1.14 Viewer";
    appName = "GameObjectViewer";
    appTitle = "Game Object Viewer";
#endif
#ifdef _EDITOR
	versionString = "Excession Engine v 1.14 Editor";
    appName = "WorldBuilder";
    appTitle = "World Builder";
#endif
#ifdef _TANKGAME
	versionString = "Excession Engine v 1.14";
	gameVersionString = "War Games Inc v 1.08";
    appName = "ExcessionEngine";
    appTitle = "War Games Inc.";
#endif
#ifdef _SPEEDTEST
	versionString = "Excession Engine v 1.14 Performance Test";
    appName = "PerformanceTest";
    appTitle = "Performance Test";
#endif
#ifdef _PARSER
	versionString = "Excession Engine v 1.14 Parser";
    appName = "Parser";
    appTitle = "Parser";
#endif

	finished = false;
	lightingSetting = 1.0f;

	playMusic = true;
	playEffects = true;
	effectsVolume = 255;
	musicVolume = 128;

	vwidth = 640;
	vheight = 480;
	vbitdepth = 0;
	vzdepth = 0;
	vdepth = 450.0f;

	kMinRenderDepth = 5;
	kMaxRenderDepth = 25;
	renderDepth = kMaxRenderDepth;
	currentRenderDepth = renderDepth;

	limitFPS = true;
	fullScreen = false;
	minimised = false;

	ctrlDown = false;
	shiftDown = false;
	active = true;

	arial = new Font();
};


//
//	Destruct, release dc and hglrc
//
TEvent::~TEvent( void )
{
#ifdef _USEGUI
	if ( mainMenu!=NULL )
		delete mainMenu;
	mainMenu = NULL;
#endif
    hWindow = NULL;

#ifdef _DXSOUND_
	if ( soundSystem!=NULL )
		delete soundSystem;
	soundSystem = NULL;
#endif

	if ( arial!=NULL )
		delete arial;
	arial = NULL;
};


bool TEvent::isExtensionSupported( const char* extensions, const char *extension )
{
	PreCond( extension!=NULL );

	if ( extensions==NULL )
	{
		return false;
	}

	const GLubyte *start;
	GLubyte *where;

	// Extension names should not have spaces.
	where = (GLubyte *) strchr(extension, ' ');
	if ( where || *extension == '\0' )
	{
		return false;
	}

	// It takes a bit of care to be fool-proof about parsing the
    // OpenGL extensions string.  Don't be fooled by sub-strings,
	start = (const GLubyte*)extensions;
	do
	{
		char buf[256];
		size_t index = 0;
		while ( index<255 && start[index]!=0 && start[index]!=' ' )
		{
			buf[index] = char(start[index++]);
		}
		buf[index] = 0;

		WriteString( "%s\n", buf );
		where = (GLubyte *)strstr(buf,extension);
		if ( where!=NULL )
		{
			return true;
		}
		start = &start[index+1];
	}
	while ( start[0]!=0 );
	return false;
}


bool TEvent::GetModels( size_t& numModels, TCharacter* models, 
						 TString* modelNames, TString& errStr )
{
	numModels = 0;

	_finddata_t fd;
	long fh = _findfirst( "data\\characters\\*.bin", &fd );
	if ( fh!=-1 )
	{
		TString errStr;

		TString fname = "data\\characters\\" + TString(fd.name);
		modelNames[numModels++] = fname;
		int fnd;
		do
		{
			fnd = _findnext( fh, &fd );
			if ( fnd==0 )
			{
				TString fname = "data\\characters\\" + TString(fd.name);
				modelNames[numModels++] = fname;
			}
		}
		while (fnd==0 && numModels<kMaxModels);
		_findclose(fh);
	}

	// sort the sortedNames array, quick bubbleSort
	size_t i;
	for ( i=0; i<numModels; i++ )
	for ( size_t j=i+1; j<numModels; j++ )
	{
		if ( i!=j )
		{
			TString t1 = modelNames[i];
			TString t2 = modelNames[j];

			// swap?
			if ( stricmp( t1.c_str(), t2.c_str() ) > 0 )
			{
				modelNames[j] = t1;
				modelNames[i] = t2;
			}
		}
	}

	for ( i=0; i<numModels; i++ )
	{
		if ( !models[i].Load( modelNames[i], errStr ) )
		{
			return false;
		}
		modelNames[i] = models[i].Name();
	}
	return true;
};

TString TEvent::GetCharacterFilename( size_t characterId )
{
	TString modelNames[kMaxModels];
	size_t numModels = 0;

	_finddata_t fd;
	long fh = _findfirst( "data\\characters\\*.bin", &fd );
	if ( fh!=-1 )
	{
		TString errStr;

		TString fname = "data\\characters\\" + TString(fd.name);
		modelNames[numModels++] = fname;
		int fnd;
		do
		{
			fnd = _findnext( fh, &fd );
			if ( fnd==0 )
			{
				TString fname = "data\\characters\\" + TString(fd.name);
				modelNames[numModels++] = fname;
			}
		}
		while (fnd==0 && numModels<kMaxModels);
		_findclose(fh);
	}

	PreCond( characterId < numModels );

	// sort the sortedNames array, quick bubbleSort
	size_t i;
	for ( i=0; i<numModels; i++ )
	for ( size_t j=i+1; j<numModels; j++ )
	{
		if ( i!=j )
		{
			TString t1 = modelNames[i];
			TString t2 = modelNames[j];

			// swap?
			if ( stricmp( t1.c_str(), t2.c_str() ) > 0 )
			{
				modelNames[j] = t1;
				modelNames[i] = t2;
			}
		}
	}
	return modelNames[characterId];
};


void TEvent::GetMultiPlayerMaps( TDefFile* maps, size_t& numMaps )
{
	bool found = true;
	numMaps = 0;
	do
	{
		TString fname = "story";
		if ( (numMaps+1)<10 )
		{
			fname = fname + "0";
			fname = fname + Int2Str(numMaps+1);
		}
		else
		{
			fname = fname + Int2Str(numMaps+1);
		}
		fname = fname + ".def";

		TDefFile def;
		TString errStr;
		found = def.LoadBinary( fname, "data\\stories", errStr, true );
		if ( found )
		{
			if ( maps!=NULL )
			{
				maps[numMaps] = def;
			}
			numMaps++;
		}
	}
	while ( found );
};


void TEvent::AdjustLightingByTime( size_t hours, size_t mins )
{
	float time;

	if ( hours>8 && hours<17 )
		lightingSetting = 1.0f;
	if ( (hours >=0 && hours<=6) || (hours>=23) )
		lightingSetting = 0.01f;
	if ( hours>=19 && hours<23 )
		lightingSetting = 0.3f;
	if ( hours==7 || hours==8 )
	{
		time = float((hours-7)*60 + mins) / 120.0f;
		lightingSetting = 0.3f + time * 0.7f;
	}
	if ( hours==17 || hours==18 )
	{
		time = float((hours-17)*60 + mins) / 120.0f;
		lightingSetting = 1.0f - time * 0.7f;
	}
	
	SetupLight(false);
	SetupFog(fog);
};


void TEvent::SetCmdLine( const char* str )
{
	commandLine = str;
};

bool TEvent::ForceWindowed( void )
{
	if ( FullScreen() )
	{
		FullScreen( false );
		size_t fs = 0;
		SetRegistryKey( "Software\\PDV\\Performance", "FullScreen", fs );
		return true;
	}
	return false;
};

bool TEvent::LoadFont( TString& errStr )
{
	if ( arial!=NULL )
		delete arial;
	arial = new Font();

	WriteString( "loading font\n" );
//	if ( !arial->Load( "data\\arial10.fnt", errStr ) )
	if ( !arial->Load( "data\\arial12.fnt", errStr ) )
	{
		errStr = "Error loading arial font (" + errStr + ")";
		WriteString( errStr.c_str() );
		return false;
	}
	return true;
};


//
//	Setup openGL
//	initialise hglrc and set pixel format - must be called from WM_CREATE
//	in windows event loop - can't be called from anywhere else
//
bool TEvent::Setup( HWND _hwnd, float depth, int width, int height, TString& errStr )
{
	WriteString( "initiating TEvent::Setup\n" );

	// get system information
	WriteString( "\nSystem information\n" );
	SYSTEM_INFO info;
	::GetSystemInfo( &info );
	switch ( info.wProcessorArchitecture )
	{
	case PROCESSOR_ARCHITECTURE_INTEL:
		{
			WriteString( "architecture: intel\n" );
			switch ( info.wProcessorLevel )
			{
			case 3:
				WriteString( "Processor: i386\n" );
				break;
			case 4:
				WriteString( "Processor: i486\n" );
				break;
			case 5:
				WriteString( "Processor: Pentium\n" );
				break;
			case 6:
				WriteString( "Processor: Pentium II or Pentium Pro\n" );
				break;
			case 7:
				WriteString( "Processor: Pentium III\n" );
				break;
			default:
				WriteString( "Processor: unknown\n" );
				break;
			}
			break;
		}
	case PROCESSOR_ARCHITECTURE_MIPS:
		WriteString( "architecture: mips\n" );
		break;
	case PROCESSOR_ARCHITECTURE_ALPHA:
		WriteString( "architecture: alpha\n" );
		break;
	case PROCESSOR_ARCHITECTURE_PPC:
		WriteString( "architecture: ppc\n" );
		break;
	case PROCESSOR_ARCHITECTURE_UNKNOWN:
		WriteString( "architecture: unknown\n" );
		break;
	};
	WriteString( "number of processors: %d\n", info.dwNumberOfProcessors );
	WriteString( "processor revision: 0x%04x\n", info.wProcessorRevision );

    // setup openGL etc.
    InitGL( depth, width, height );

	WriteString( "TEvent::Setup done\n\n" );
    return true;
};


//
//	Initialise openGL settings
//
void TEvent::InitGL( float depth, int w, int h )
{
	WriteString( "\nInitGL\n" );

	vdepth = depth;
	vwidth = float(w);
	vheight = float(h);

	// get OpenGL details
	WriteString( "OpenGL particulars\n" );
	const char* str;
	const char* vendorStr = NULL;
	str = (const char*)glGetString(GL_VENDOR);
	if ( str!=NULL )
	{
		WriteString( "Vendor = %s\n", str );
		vendorStr = str;
	}
	str = (const char*)glGetString(GL_RENDERER);
	if ( str!=NULL )
		WriteString( "Renderer = %s\n", str );
	str = (const char*)glGetString(GL_VERSION);
	if ( str!=NULL )
		WriteString( "Version = %s\n", str );

	// software:
	// Vendor = Microsoft Corporation
	// Renderer = GDI Generic
	if ( vendorStr!=NULL )
	if ( strnicmp( vendorStr, "microsoft corporation", 21 )==0 )
	{
		isSoftware = true;
	}

	// set viewport size
	// set projection transformation matrix
	RECT rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = w;
	rect.bottom = h;
#ifdef _EDITOR
	if ( hWindow!=NULL )
	{
		::GetClientRect(hWindow,&rect);
	}
#endif
	glViewport( rect.left,rect.top,rect.right,rect.bottom );
	SetupGL3d( vdepth, vwidth, vheight );
	
	glDrawBuffer(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_CULL_FACE);
	glFrontFace( GL_CCW );
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR);
//	glEnable(GL_NORMALIZE); // adjust normals auto
//	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glColorMaterial( GL_FRONT, GL_AMBIENT_AND_DIFFUSE );
	glEnable( GL_COLOR_MATERIAL );

	AntiAlias( antiAlias );
	SetupLight(false);
	SetupFog( fog );

	glLoadIdentity();

	WriteString( "exit InitGL\n\n" );
}


//
//	setup single light source for now
//
void TEvent::SetupLight( bool menu )
{
	float ambient[4] = {0.2f,0.2f,0.2f,1};

	glEnable(GL_LIGHTING);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 0 );
	glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, 0 );

	glLightModelfv( GL_LIGHT_MODEL_AMBIENT, ambient );

	if ( menu )
	{
		GLfloat position[]	= { 0, 0, -vdepth*10, 0, 0 };
		GLfloat zero[]		= { 0,0,0,1 };
		GLfloat one[]		= { 1,1,1,1 };

		// lighting setup
		glLightfv(GL_LIGHT0, GL_POSITION, position );
		glLightfv(GL_LIGHT0, GL_AMBIENT,  one );

		glLighti(GL_LIGHT0, GL_SHININESS, 2 );
		glLightfv(GL_LIGHT0, GL_SPECULAR, zero );
		glLightfv(GL_LIGHT0, GL_DIFFUSE,  zero );
	}
	else
	{
		GLfloat position[]	= { 0.0f, 100000.0f, 0.0f, 0.0f, 0.0f };

		GLfloat zero[]		= { 0,0,0,1 };
		GLfloat one[]		= { lightingSetting, lightingSetting, 
								lightingSetting, 0 };
		GLfloat half[]		= { 0.6f*lightingSetting, 0.6f*lightingSetting, 
								0.6f*lightingSetting, 0.0f };

		// lighting setup
		glLightfv(GL_LIGHT0, GL_POSITION, position );
		glLightfv(GL_LIGHT0, GL_AMBIENT,  one );

		glLighti(GL_LIGHT0, GL_SHININESS, 2 );
		glLightfv(GL_LIGHT0, GL_DIFFUSE,  zero );
		glLightfv(GL_LIGHT0, GL_SPECULAR, zero );
	}
	glEnable(GL_LIGHT0);
}



void TEvent::SetupFog( bool _fog )
{
	if ( _fog )
	{
		glEnable(GL_FOG);

		GLint fogMode;

		fogMode = GL_EXP2;
		glFogi(GL_FOG_MODE, 	fogMode );
		glFogf(GL_FOG_START, 	vdepth/5 );
		glFogf(GL_FOG_END,   	(vdepth-2) );
		glFogf(GL_FOG_DENSITY,  0.004f );

		GLfloat fogColor[4] = { 0.8f*lightingSetting, 0.8f*lightingSetting, 
								0.8f*lightingSetting, 0.2f*lightingSetting };

		glFogfv(GL_FOG_COLOR, 	fogColor);
		glClearColor( 0.8f*lightingSetting,0.8f*lightingSetting,
					  0.8f*lightingSetting,0.2f*lightingSetting );

#if defined(_VIEWER) || defined(_PARSER)
	    glClearColor( 0.0f,0.0f,0.0f,0 );
#endif
	}
	else
	{
#ifdef _TANKGAME
	    glClearColor( 0.15f*lightingSetting,0.15f*lightingSetting,
					  0.25f*lightingSetting,0 );
#endif
#ifdef _SPEEDTEST
	    glClearColor( 0.05f,0.05f,0.05f,0 );
//	    glClearColor( 1,1,1,0 );
#endif
#if defined(_VIEWER) || defined(_PARSER)
	    glClearColor( 0.0f,0.0f,0.0f,0 );
#endif
#ifdef _EDITOR
	    glClearColor( 0.3f,0.3f,0.5f,0 );
#endif
	    glDisable(GL_FOG); // no fog
	}
}


//
//	Setup perspective view
//
void TEvent::SetupGL3d( float depth, float width, float height )
{
	SetupLight(false);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f,width / height, 1.0f, depth );
	glMatrixMode(GL_MODELVIEW);
}


//
//	Setup 2d ortho view
//
void TEvent::SetupGL2d( float depth, float width, float height )
{
	SetupLight(true);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, 0, height, 1, depth );
	glMatrixMode(GL_MODELVIEW);
}


//
//	Setup 2d ortho view
//
void TEvent::SetupGL2dNormal( float depth, float width, float height )
{
	SetupLight(true);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, height, 0, 1, depth );
	glMatrixMode(GL_MODELVIEW);
}

//
//	Clear openGL backbuffer, render the system at (x,z)
//	and draw the lot.  Keep track of the real fps calculations
//	and swap double buffered screen
//
void TEvent::Draw(void)
{
	// Draw the frame
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	SetupGL3d( vdepth, vwidth, vheight );

    // call render code
    Render();
}



HWND TEvent::Window( void ) const
{
	return hWindow;
};



void TEvent::Window( HWND _hwnd )
{
	hWindow = _hwnd;
};


//
//	Callback event - can be overwritten by user through inheritance
//
void TEvent::Destroy( void )
{
};


//
//	Callback event - can be overwritten by user through inheritance
//
void TEvent::KeyDown( size_t kd )
{
	if ( showMenu && currentPage!=NULL )
		currentPage->KeyDown(kd);
};


//
//	Callback event - can be overwritten by user through inheritance
//
void TEvent::KeyUp( size_t ku )
{
	if ( showMenu && currentPage!=NULL )
		currentPage->KeyUp(ku);
};


//
//	Callback event - can be overwritten by user through inheritance
//
void TEvent::KeyPress( size_t kp )
{
	if ( showMenu && currentPage!=NULL )
		currentPage->KeyPress(kp,ctrlDown);
};


//
//	Callback event - can be overwritten by user through inheritance
//
void TEvent::MouseDown( int button, int x, int y)
{
};

void TEvent::MouseMove( HWND hwnd, int x, int y, int dx, int dy )
{
};

void TEvent::MouseUp( HWND hwnd, int button, int x, int y, int dx, int dy )
{
};

void TEvent::MouseDown( HWND hwnd, int button, int x, int y, int dx, int dy )
{
};

//
//	Callback event - can be overwritten by user through inheritance
//
void TEvent::MouseDown( int button, int x, int y, int dx, int dy)
{
	if ( showMenu && currentPage!=NULL )
	{
		int adjy = y;
#ifndef _MESA
		if ( !fullScreen )
			adjy += 24;
#endif
		currentPage->MouseDown(x,adjy);
	}
};


//
//	Callback event - can be overwritten by user through inheritance
//
void TEvent::MouseUp( int button, int x, int y)
{
};


//
//	Callback event - can be overwritten by user through inheritance
//
void TEvent::MouseUp( int button, int x, int y, int dx, int dy )
{
	if ( showMenu && currentPage!=NULL )
	{
		int adjy = y;
#ifndef _MESA
		if ( !fullScreen )
			adjy += 24;
#endif
		currentPage->MouseUp(x,adjy);
	}
};


//
//	Callback event - can be overwritten by user through inheritance
//
void TEvent::MouseMove( int x, int y )
{
};


//
//	Callback event - can be overwritten by user through inheritance
//
void TEvent::MouseMove( int x, int y, int dx, int dy )
{
	if ( showMenu && currentPage!=NULL )
	{
		int adjy = y;
#ifndef _MESA
		if ( !fullScreen )
			adjy += 24;
#endif
		currentPage->MouseMove(x,adjy);
	}
};


//
//	Perform the actual rendering
//	Callback event - can be overwritten by user through inheritance
//
void TEvent::Render( void )
{
};


//
//	Perform the actual game logic
//	Callback event - can be overwritten by user through inheritance
//
void TEvent::Logic( void )
{
	shiftDown	= (::GetAsyncKeyState(VK_SHIFT)&0x8000)>0;
	ctrlDown	= (::GetAsyncKeyState(VK_CONTROL)&0x8000)>0;

#ifdef _USEGUI
	if ( showMenu )
	{
		if ( currentPage==mainMenu )
		{
			if ( exitButton!=NULL )
			if ( exitButton->Clicked() )
			{
				::PostQuitMessage( 0 );
#ifdef _MESA
				GM_exit();
#endif
			}
		}
	}
#endif
};


//
//	Callback event - can be overwritten by user through inheritance
//
bool TEvent::StartupSystem( TString& )
{
	return true;
};


//
//	Callback event - can be overwritten by user through inheritance
//
void TEvent::DoMenu( int )
{
};


//
//	Return index to best possible pixel format for running this app
//	returns 0 on failure
//	windowed!  bpp = bits per pixel, depth = bit depth, dbl = double
//  buffered {0,1}, acc = accelerated {0,1}, qFlag = best score,
//	accelType = { software=0, mcd=1, icd=2 }
//
int TEvent::ChoosePixelFormatEx( HDC _hdc )
{ 
	static size_t arraySize = 100;

	TString* list = new TString[arraySize];
	PostCond( list!=NULL );		// temporary list array for holding
								// output results to log - fixed in size for now

	PIXELFORMATDESCRIPTOR pfd; 
	
	ZeroMemory(&pfd,sizeof(pfd)); 
	pfd.nSize=sizeof(pfd); 
	pfd.nVersion=1;
	
	WriteString("\n\nEnumerating pixel formats\n"); // to log

	size_t num = DescribePixelFormat(_hdc,1,sizeof(pfd),&pfd);
	if (num==0)
	{
		delete []list;
		return 0;
	}

	// make sure we stick inside the array
	if ( num>=arraySize )
		num = arraySize-1;
  
	size_t maxqual = 0; 
	size_t maxindex = 0; // index of winner
	bool mcd,icd,soft;
	
	for ( size_t i=1; i<=num; i++)
	{
		ZeroMemory(&pfd,sizeof(pfd)); 
		pfd.nSize = sizeof(pfd); 
		pfd.nVersion=1;
    
		DescribePixelFormat(_hdc,i,sizeof(pfd),&pfd);
		int bpp=pfd.cColorBits;
		int depth=pfd.cDepthBits;
		bool pal=(pfd.iPixelType==PFD_TYPE_COLORINDEX);

		mcd=((pfd.dwFlags & PFD_GENERIC_FORMAT) && (pfd.dwFlags & PFD_GENERIC_ACCELERATED));
		soft=((pfd.dwFlags & PFD_GENERIC_FORMAT) && !(pfd.dwFlags & PFD_GENERIC_ACCELERATED));
		icd=(!(pfd.dwFlags & PFD_GENERIC_FORMAT) && !(pfd.dwFlags & PFD_GENERIC_ACCELERATED));

		int accelType = 0;
		if ( mcd ) 
			accelType = 1;
		else if ( icd )
			accelType = 2;

		bool opengl = (pfd.dwFlags & PFD_SUPPORT_OPENGL)>0;
		bool window = (pfd.dwFlags & PFD_DRAW_TO_WINDOW)>0;
		bool bitmap = (pfd.dwFlags & PFD_DRAW_TO_BITMAP)>0;
		bool dbuff  = (pfd.dwFlags & PFD_DOUBLEBUFFER)>0;
		bool stncl  = (pfd.cStencilBits > 0 );

		TString buf;
		switch (accelType)
		{
			case 1:
				buf = "mcd";
				break;
			case 2:
				buf = "icd";
				break;
			default:
				buf = "software";
				break;
		}

		// create capabilities string		
		TString str;
		str = "format " + Int2Str(i) + ":";
		if ( window )
			str = str + "windowed, ";
		else
			str = str + "not windowed, ";
		if ( opengl )
			str = str + "OpenGL, ";
		else
			str = str + "no OpenGL, ";
		if ( bitmap )
			str = str + "draw to bitmap, ";
		else
			str = str + "not draw to bitmap, ";
		if ( stncl )
			str = str + "has stencil, ";
		else
			str = str + "no stencil, ";
		if ( dbuff )
			str = str + "double buffered, ";
		else
			str = str + "not double buffered, ";
		str = str + buf + ", ";
		str = str + Int2Str(bpp) + " colour bits, ";
		str = str + Int2Str(depth) + " bit depth buffer\n";
		list[i-1] = str;

		// rate the viewport
		size_t q = 0;
		if ( opengl )
		{
			q += 0x8000;
		}
		if ( icd )
		{
			q += 0x4000;
		}
		if ( mcd )
		{
			q += 0x2000;
		}
		if ( dbuff )
		{
			q += 0x1000;
		}
		if ( window )
		{
			q += 0x0800;
		}
		if ( bpp>=24 )
		{
			q += 0x0400;
		}
		if ( bpp==16 )
		{
			q += 0x0200;
		}
		if ( depth>=24 )
		{
			q += 0x0100;
		}
		if ( depth==16 )
		{
			q += 0x0080;
		}
		if ( stncl )
		{
			q += 0x0040;
		}
		if ( bitmap )
		{
			q += 0x0020;
		}
		if ( pal )
		{
			q = 0;
		}

		if ( q > maxqual ) 
		{
			maxqual=q; 
			maxindex=i;
		}
	}

	// failed?
	if (maxindex==0) 
	{
		delete []list;
		return maxindex;
	}

	// output results to log
	if ( maxindex>0 && maxindex<arraySize )
	{
		WriteString( list[maxindex-1].c_str() );
	}
	else
	{
		WriteString( "no suitable support found\n" );
	}

	delete []list;

	return maxindex;
}

bool TEvent::ChangeResolution( TString& errStr )
{
	if ( fullScreen )
	{
		DEVMODE dm;
		dm.dmSize=sizeof(dm);
		dm.dmPelsWidth = size_t(vwidth);
		dm.dmPelsHeight = size_t(vheight);
		dm.dmBitsPerPel = vbitdepth;
		dm.dmDisplayFrequency = vfreq;
		dm.dmFields=DM_PELSWIDTH|DM_PELSHEIGHT|DM_BITSPERPEL|DM_DISPLAYFREQUENCY;
		long res = ::ChangeDisplaySettings( &dm, CDS_FULLSCREEN );
		if ( res!=DISP_CHANGE_SUCCESSFUL )
		{
			switch ( res )
			{
			case DISP_CHANGE_RESTART:
				{
					errStr = "ChangeDisplaySettings: The computer must be restarted in order for the graphics mode to work.\n";
					break;
				}
			case DISP_CHANGE_BADFLAGS:
				{
					errStr = "ChangeDisplaySettings: An invalid set of flags was passed in.\n";
					break;
				}
			case DISP_CHANGE_BADPARAM:
				{
					errStr = "ChangeDisplaySettings: An invalid parameter was passed in. This can include an invalid flag or combination of flags.\n";
					break;
				}
			case DISP_CHANGE_FAILED:
				{
					errStr = "ChangeDisplaySettings: The display driver failed the specified graphics mode.\n";
					break;
				}
			case DISP_CHANGE_BADMODE:
				{
					errStr = "ChangeDisplaySettings: The graphics mode is not supported.\n";
					break;
				}
			}
			return false;
		}
	}
	else // windowed - go back to original screen setting
	{
		ResetScreenToStartupMode();
	}
	return true;
};

bool TEvent::DeactivateWindow( HWND hWnd, TString& errStr )
{
	if ( hWnd==hWindow )
	{
		// swap back to desktop
		// if necessairy
/*
		if ( fullScreen )
		{
			fullScreen = false;
			long style = WS_BORDER | WS_CAPTION | WS_SYSMENU |
						 WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | 
						 WS_VISIBLE;
			::SetWindowLong(hWnd,GWL_STYLE,style);
			::SetWindowPos(hWnd,HWND_TOP,0,0,int(vwidth),int(vheight),SWP_FRAMECHANGED|SWP_SHOWWINDOW);
		}
*/
	}
	return true;
};


//
// Select the pixel format for a given device context
//
int TEvent::SetDCPixelFormat( HDC hDC, int bitDepth, int zDepth, TString& errStr )
{
	int pf = ChoosePixelFormatEx(hDC);
	if ( pf==0 )
	{
	   	errStr = "TEvent::SetDCPixelFormat: Can't find an accelerated openGL driver for this system";
		return 0;
	}

	TString buf;
	buf = "Selected pixel format " + IntToString(pf) + "\n";
	WriteString( buf.c_str() );

	// set pixel format
	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd,sizeof(pfd)); 
	pfd.nSize = sizeof(pfd); 
	pfd.nVersion=1;

	DescribePixelFormat(hDC,pf,sizeof(pfd),&pfd);
	pfd.dwFlags = pfd.dwFlags | PFD_SWAP_EXCHANGE;

	if ( !SetPixelFormat(hDC,pf,&pfd ) )
	{
		errStr = "TEvent::SetPixelFormat failed (" + IntToString( GetLastError() );
		errStr = errStr + ")";
		return 0;
	}
	return pfd.cColorBits;
}


HWND TEvent::SetupWindow( HMENU mainMenu, TString& errStr )
{
	WriteString( "SetupWindow\n" );

	// setup stuff
	// Create the main application window
	long windowStyle;
	if ( fullScreen )
	{
		windowStyle = WS_POPUP | WS_VISIBLE;
	}
	else
	{
		windowStyle = WS_CAPTION | WS_SYSMENU |
					  WS_CLIPCHILDREN | WS_CLIPSIBLINGS | 
					  WS_VISIBLE | WS_OVERLAPPED | 
					  WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
	}

	HWND hwnd = ::CreateWindow(
				ApplicationName().c_str(),
				ApplicationTitle().c_str(),
				// OpenGL requires WS_CLIPCHILDREN and WS_CLIPSIBLINGS in windowed mode
                windowStyle,
				// Window position and size
				//CW_USEDEFAULT, CW_USEDEFAULT,
				0,0,
				int(vwidth), int(vheight),
				NULL,
				mainMenu,
				hInstance,
				NULL);

	// If window was not created, quit
	if( hwnd==NULL )
    {
    	errStr = "Window creation failed (" + Int2Str(GetLastError()) + ")";
		return NULL;
    }

	mainWindow = hwnd;

	// Display the window
	::ShowWindow(hwnd,SW_SHOW);
	::UpdateWindow(hwnd);

	WriteString( "Created window\n" );
	WriteString( "SetupWindow done\n\n" );
	hWindow = hwnd;
	return hwnd;
};


bool TEvent::ResetResolution( HMENU menu, TString& errStr )
{
#ifndef _MESA
	initialised = false;

	// remove old settings
	if ( hglrc!=NULL )
	{
		::wglDeleteContext(hglrc);
		hglrc = NULL;
	}

	// destroy window so SetPixelFormat can be re-applied
	if ( hWindow!=NULL )
	{
		::DestroyWindow( hWindow );
		hWindow = NULL;
		mainWindow = NULL;
		hdc = NULL;
	}

	WriteString( "Starting resolution reset\n" );
	GetRegistrySettings();

	// create window?
	if ( hWindow==NULL )
	{
		SetupWindow( menu, errStr );

	    hdc = ::GetDC( hWindow );
		if ( hdc==NULL )
		{
			WriteString( "Error getting window DC" );
			return false;
		}
	}
	if ( hWindow==NULL )
		return false;

	::MoveWindow( hWindow, 0,0, int(vwidth), int(vheight), true );

	if ( !ChangeResolution(errStr) )
	{
		WriteString( errStr.c_str() );
		return false;
	}

	::MoveWindow( hWindow, 0,0, int(vwidth), int(vheight), true );

	WriteString ("getting window dc\n" );

	// anti aliased?
	if ( antiAlias )
	{
		typedef const char* (*proc)(HDC);
		proc wglGetExtensionsStringARB;
		wglGetExtensionsStringARB = (proc)wglGetProcAddress("wglGetExtensionsStringARB" );

		typedef BOOL (*wglcpf)(HDC,const int*,const FLOAT*,UINT,int*,UINT*);
		wglcpf wglChoosePixelFormatARB;
		wglChoosePixelFormatARB = (wglcpf)wglGetProcAddress("wglChoosePixelFormatARB" );

		typedef BOOL (*wglgpf)(HDC,int,int,UINT,const int*,int*);
		wglgpf wglGetPixelFormatAttribivARB;
		wglGetPixelFormatAttribivARB = (wglgpf)wglGetProcAddress("wglGetPixelFormatAttribivARB");

		if ( wglGetExtensionsStringARB!=NULL )
		{
			const char* exts = wglGetExtensionsStringARB(hdc);
			if ( isExtensionSupported( exts, "WGL_ARB_pixel_format" ) &&
				 isExtensionSupported( exts, "WGL_ARB_multisample" ) )
			{
			}
		}
	}

	WriteString( "selecting pixel format\n" );
	if ( SetDCPixelFormat( hdc, vbitdepth, vzdepth, errStr )==0 )
	{
		WriteString( errStr.c_str() );
		return false;
	}

	WriteString( "creating render context\n" );
	hglrc = ::wglCreateContext( hdc );
	DWORD err = ::GetLastError();
	if ( hglrc==NULL )
	{
		TString msg = "\nError creating GLRC (wglCreateContext) (" + Int2Str(err) + ")\n";
		WriteString( msg.c_str() );
		return false;
	}

	WriteString( "activating render context\n" );
	if ( !::wglMakeCurrent( hdc, hglrc ) )
	{
		TString msg = "\nwglMakeCurrent failed (" + Int2Str(GetLastError()) + ")\n";
		WriteString( msg.c_str() );
		return false;
	}

    if ( !Setup( hWindow, Depth(), int(vwidth), int(vheight), errStr ) )
	{
		WriteString( errStr.c_str() );
		return false;
	}
#endif

#ifdef _USEGUI
	if ( currentPage==NULL )
		SetupMenus();
#endif

	// reload all textures and stuff
	if ( !LoadFont(errStr ) )
		return false;

	initialised = true;
	return true;
};

bool TEvent::EnumResolutions( void )
{
	ClearResolutions();

	DEVMODE currentMode;
	currentMode.dmSize = sizeof( currentMode );

	resolutionCounter = 0;
	size_t index = 0;
	while ( EnumDisplaySettings( NULL, index, &currentMode )==TRUE 
			&& resolutionCounter < kMaxResolutions )
	{
		if ( currentMode.dmPelsWidth>=640 && currentMode.dmPelsHeight>=480 &&
			 currentMode.dmBitsPerPel>8 && 
			 currentMode.dmDisplayFrequency==initialScreenMode.dmDisplayFrequency )
		{
			modes[resolutionCounter] = currentMode;
			resolutionCounter++;
		}
		index++;
	};
	return ( resolutionCounter>0 );
};


void TEvent::ClearResolutions( void )
{
	resolutionCounter = 0;
};

size_t TEvent::NumResolutions( void ) const
{
	return resolutionCounter;
};

void TEvent::GetResolution( size_t index, size_t& width, size_t& height, 
						    size_t& bitDepth, size_t& zDepth, size_t& freq )
{
	PreCond( index < resolutionCounter );
	width = modes[index].dmPelsWidth;
	height = modes[index].dmPelsHeight;
	bitDepth = modes[index].dmBitsPerPel;
	freq = modes[index].dmDisplayFrequency;
	zDepth = 16;
};


void TEvent::Width( float w )
{
	vwidth = w;
};


void TEvent::Height( float h )
{
	vheight = h;
};

float TEvent::Width( void ) const
{
	return vwidth;
};

float TEvent::Height( void ) const
{
	return vheight;
};

bool TEvent::Finished( void ) const
{
	return finished;
};

void TEvent::Finished( bool _finished )
{
	finished = _finished;
};

size_t TEvent::BitDepth( void ) const
{
	return vbitdepth;
};

float TEvent::Depth( void ) const
{
	return vdepth;
};

bool TEvent::FullScreen( void ) const
{
	return fullScreen;
};

void TEvent::FullScreen( bool f )
{
	fullScreen = f;
};

bool TEvent::Minimised( void ) const
{
	return minimised;
};

void TEvent::Minimised( bool m )
{
	minimised = m;
};

bool TEvent::LimitFPS( void ) const
{
	return limitFPS;
};

void TEvent::LimitFPS( bool l )
{
	limitFPS = l;
};

const TString& TEvent::ApplicationName( void ) const
{
	return appName;
};

const TString& TEvent::ApplicationTitle( void ) const
{
	return appTitle;
};

size_t TEvent::LogicFPS( void ) const
{
	return kLogicFPS;
};

const TString& TEvent::VersionString( void ) const
{
	return versionString;
};

HDC TEvent::HDc( void ) const
{
	return hdc;
};

void TEvent::HDc( HDC dc )
{
	hdc = dc;
};

HGLRC TEvent::HGLRc( void ) const
{
	return hglrc;
};

void TEvent::HGLRc( HGLRC gl )
{
	hglrc = gl;
};

size_t TEvent::RenderDepth( void ) const
{
	return renderDepth;
};

void TEvent::RenderDepth( size_t rd )
{
	renderDepth = rd;
};

size_t TEvent::MinRenderDepth( void ) const
{
	return kMinRenderDepth;
};

void TEvent::MinRenderDepth( size_t min )
{
	kMinRenderDepth = min;
};

size_t TEvent::MaxRenderDepth( void ) const
{
	return kMaxRenderDepth;
};

void TEvent::MaxRenderDepth( size_t max )
{
	kMaxRenderDepth = max;
};

void TEvent::SetMessage( const TString& msg )
{
	msgStr = msg;
};

void TEvent::WriteMessage( void )
{
	Write( 80,float(vheight)-60,-10, msgStr );
};

void TEvent::SetCountDown( const TString& msg )
{
	cdStr = msg;
};

void TEvent::WriteCountDown( void )
{
	Write( float(vwidth)-120,float(vheight)-60,-10, cdStr );
};

/*
void TEvent::Fog( bool f )
{
	fog = f;
};

bool TEvent::Fog( void ) const
{
	return fog;
};
*/

HINSTANCE TEvent::Instance( void ) const
{
	return hInstance;
};

Font& TEvent::Arial( void )
{
	PreCond( arial!=NULL );
	return *arial;
};


void TEvent::Write( float x, float y, float z, const TString& msg )
{
	Write( x,y,z, msg.c_str() );
};

size_t TEvent::TextWidth( const TString& text )
{
	PreCond( arial!=NULL );
	return size_t( arial->GetWidth( text ) );
}

void TEvent::WriteReverse( float x, float y, float z, const TString& msg )
{
	WriteReverse( x,y,z, msg.c_str() );
};


void TEvent::Write( float x, float y, float z, const char* msg )
{
	arial->Write( x,y,z, msg );
};

void TEvent::WriteReverse( float x, float y, float z, const char* msg )
{
	arial->WriteReverse( x,y,z, msg );
};

void TEvent::CenterMenu( void )
{
#ifdef _USEGUI
	if ( mainMenu!=NULL )
	{
		mainMenu->CenterMenu();
	}
#endif
};


void TEvent::RenderMenu( bool showAllPages )
{
	if ( showMenu )
	{
		if ( prevPage2!=NULL && showAllPages )
			prevPage2->Draw(true);
		if ( prevPage1!=NULL && showAllPages )
			prevPage1->Draw(true);
		if ( currentPage!=NULL )
			currentPage->Draw(false);
	}
};


void TEvent::CurrentPage( TGUI* page )
{
	if ( page==prevPage1 )
	{
		prevPage1 = prevPage2;
		prevPage2 = NULL;
	}
	else
	{
		prevPage2 = prevPage1;
		prevPage1 = currentPage;
	}
	currentPage = page;
};


TGUI* TEvent::CurrentPage( void ) const
{
	return currentPage;
};


void TEvent::AddItem( TGUI*& page, TGUI* item )
{
	if ( page==NULL )
		page = item;
	else
	{
		if ( item->itemType==guiCompoundObject )
		{
			item->next = page;
			page = item;
		}
		else
		{
			TGUI* list = page;
			while ( list->next!=NULL )
				list = list->next;
			list->next = item;
		}
	}
};


void TEvent::SetupMenus( void )
{
#ifdef _USEGUI
	if ( mainMenu!=NULL )
		delete mainMenu;
	mainMenu = NULL;

	// Setup dialog
	AddItem( mainMenu, new TGUI( this, guiWindow, 0,0, 250, 346, mainMenuDepth, STR(strSetup) ) );
	AddItem( mainMenu, new TGUI( this, guiLabel, 105,325,65,14, mainMenuDepth, versionString, 255,255,255 ) );

	switch ( languageId )
	{
	default: // english
		{
			videoSettingsButton = new TGUI( this, guiButton, 30,25,182,25, mainMenuDepth, STR(strSettings) );
			singlePlayerButton = new TGUI( this, guiButton, 30,55,182,25, mainMenuDepth, STR(strSinglePlayer) );
			hostButton = new TGUI( this, guiButton, 30,85,182,25, mainMenuDepth, STR(strHostNetworkGame) );
			joinButton = new TGUI( this, guiButton, 30,115,182,25, mainMenuDepth, STR(strJoinNetworkGame) );
			controlsButton = new TGUI( this, guiButton, 30,145,182,25, mainMenuDepth, STR(strControls) );
			aboutButton = new TGUI( this, guiButton, 30,175,182,25, mainMenuDepth, STR(strAbout) );
			downloadButton = new TGUI( this, guiButton, 30,205,182,25, mainMenuDepth, STR(strDownloadExtras) );
			helpButton = new TGUI( this, guiButton, 30,235,182,25, mainMenuDepth, STR(strHelp) );
			exitButton = new TGUI( this, guiButton, 30,290,182,25, mainMenuDepth, STR(strExit) );

			exitButton->ButtonKey( 'X' );
			break;
		}
	case 1: // german
		{
			videoSettingsButton = new TGUI( this, guiButton, 25,25,192,25, mainMenuDepth, STR(strSettings) );
			singlePlayerButton = new TGUI( this, guiButton, 25,55,192,25, mainMenuDepth, STR(strSinglePlayer) );
			hostButton = new TGUI( this, guiButton, 25,85,192,25, mainMenuDepth, STR(strHostNetworkGame) );
			joinButton = new TGUI( this, guiButton, 25,115,192,25, mainMenuDepth, STR(strJoinNetworkGame) );
			controlsButton = new TGUI( this, guiButton, 25,145,192,25, mainMenuDepth, STR(strControls) );
			aboutButton = new TGUI( this, guiButton, 25,175,192,25, mainMenuDepth, STR(strAbout) );
			downloadButton = new TGUI( this, guiButton, 25,205,192,25, mainMenuDepth, STR(strDownloadExtras) );
			helpButton = new TGUI( this, guiButton, 30,235,182,25, mainMenuDepth, STR(strHelp) );
			exitButton = new TGUI( this, guiButton, 25,290,192,25, mainMenuDepth, STR(strExit) );

			exitButton->ButtonKey( 'B' );
			break;
		}
	}

	AddItem( mainMenu, videoSettingsButton );
	AddItem( mainMenu, singlePlayerButton );
	AddItem( mainMenu, hostButton );
	AddItem( mainMenu, joinButton );
	AddItem( mainMenu, controlsButton );
	AddItem( mainMenu, aboutButton );
	AddItem( mainMenu, downloadButton );
	AddItem( mainMenu, helpButton );
	AddItem( mainMenu, exitButton );

//	hostButton->EnableControl(false);
//	joinButton->EnableControl(false);

	currentPage = mainMenu;
	currentPage->SetLocation( vwidth*0.5f - 125, vheight*0.5f - 100 );

	CenterMenu();
#endif
};


bool TEvent::ShowMenu( void ) const
{
	return showMenu;
};


void TEvent::ShowMenu( bool _showMenu )
{
	showMenu = _showMenu;
};


bool TEvent::MainMenu( void ) const
{
#ifdef _USEGUI
	return (showMenu && currentPage==mainMenu);
#else
	return true;
#endif
};


bool TEvent::GetRegistrySettings( void )
{
	// need to enumerate resolutions?
	if ( resolutionCounter==0 )
	{
		WriteString( "enumerating possible resolutions\n" );
		if ( !EnumResolutions() )
		{
			return false;
		}
	}

	size_t res;

	// get registry res preference
	if ( GetRegistryKey( "Software\\PDV\\Performance", "Resolution", res ) )
	{
		size_t w,h;
		GetResolution( res, w, h, vbitdepth, vzdepth, vfreq );
		vwidth = float(w);
		vheight = float(h);
	}
	else
	{
		// pick the first 800x600 we can find - otherwise set res=0
		int index = -1;
		size_t w,h;
		for ( size_t i=0; i<resolutionCounter && index==-1; i++ )
		{
			GetResolution( i, w, h, vbitdepth, vzdepth, vfreq );
			if ( w==800 && h==600 )
			{
				index = i;
			}
		}
		if ( index==-1 )
		{
			index = 0;
		}

		GetResolution( index, w,h, vbitdepth, vzdepth, vfreq );
		vwidth = float(w);
		vheight = float(h);

		SetRegistryKey( "Software\\PDV\\Performance", "Resolution", index );
	}

	if ( !GetRegistryKey( "Software\\PDV\\Performance", "Language", languageId ) )
	{
		languageId = 0;
		newInstallation = true;
	}

	if ( !GetRegistryKey( "Software\\PDV\\Performance", "ShadowLevel", shadowLevel ) )
	{
		shadowLevel = 0;
	}

	size_t fs;
	if ( GetRegistryKey( "Software\\PDV\\Performance", "FullScreen", fs ) )
	{
		if ( fs==1 )
			fullScreen = true;
		else
			fullScreen = false;
	}
	else // default value
	{
		fullScreen = true;
	}

#if defined(_EDITOR) || defined(_VIEWER)
	vwidth = 640;
	vheight = 480;
	vbitdepth = 32;
	vfreq = initialScreenMode.dmDisplayFrequency;
	fullScreen = false;
#endif
#if defined(_PARSER)
	vwidth = 800;
	vheight = 600;
	vbitdepth = 32;
	vfreq = initialScreenMode.dmDisplayFrequency;
	fullScreen = false;
#endif

	if ( GetRegistryKey( "Software\\PDV\\Performance", "AntiAliased", fs ) )
		if ( fs==1 )
			antiAlias = true;
		else
			antiAlias = false;

	AntiAlias( antiAlias );

	size_t lfps;
	if ( GetRegistryKey( "Software\\PDV\\Performance", "LimitFPS", lfps ) )
		if ( lfps==1 )
			limitFPS = true;
		else
			limitFPS = false;

	size_t currMusic;
	if ( GetRegistryKey( "Software\\PDV\\Performance", "PlayMusic", currMusic ) )
		if ( currMusic==1 )
			playMusic = true;
		else
			playMusic = false;

	if ( GetRegistryKey( "Software\\PDV\\Performance", "PlayEffects", currMusic ) )
		if ( currMusic==1 )
			playEffects = true;
		else
			playEffects = false;

	if ( !GetRegistryKey( "Software\\PDV\\Performance", "EffectsVolume", effectsVolume ) )
		effectsVolume = 255;

	if ( !GetRegistryKey( "Software\\PDV\\Performance", "MusicVolume", musicVolume ) )
		musicVolume = 128;

	if ( !GetRegistryKey( "Software\\PDV\\Performance", "RenderDepth", renderDepth ) )
		renderDepth = (kMinRenderDepth+kMaxRenderDepth) / 2;

	return true;
}


bool TEvent::Initialised( void ) const
{
	return initialised;
};

void TEvent::Initialised( bool i )
{
	initialised = i;
};

bool TEvent::CtrlDown( void ) const
{
	return ctrlDown;
};

void TEvent::CtrlDown( bool c )
{
	ctrlDown = c;
};

bool TEvent::ShiftDown( void ) const
{
	return shiftDown;
};

void TEvent::ShiftDown( bool c )
{
	shiftDown = c;
};

HWND TEvent::Dialog1( void )
{
	return dialog1;
};

void TEvent::Dialog1( HWND d )
{
	dialog1 = d;
};

HWND TEvent::Dialog2( void )
{
	return dialog2;
};

void TEvent::Dialog2( HWND d )
{
	dialog2 = d;
};

bool TEvent::ShowStory( void ) const
{
	return showStory;
};

void TEvent::ShowStory( bool st )
{
	showStory = st;
};

void TEvent::AntiAlias( bool _antiAlias )
{
	glShadeModel(GL_SMOOTH); // default anyway
	antiAlias = _antiAlias;
	if ( antiAlias )
	{
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_POINT_SMOOTH);
//		glEnable(GL_POLYGON_SMOOTH); 
	}
	else
	{
		glDisable(GL_POINT_SMOOTH);
		glDisable(GL_LINE_SMOOTH);
//		glDisable(GL_POLYGON_SMOOTH); 
	};
};

bool TEvent::AntiAlias( void ) const
{
	return antiAlias;
};

byte TEvent::EffectsVolume( void ) const
{
	return effectsVolume;
};

byte TEvent::MusicVolume( void ) const
{
	return musicVolume;
};

size_t TEvent::LanguageId( void ) const
{
	return languageId;
}

void TEvent::LanguageId( size_t _languageId )
{
	languageId = _languageId;
	SetupMenus();
};

bool TEvent::IsSoftware( void ) const
{
	return isSoftware;
};

bool TEvent::NewInstallation( void )
{
	if ( !GetRegistryKey( "Software\\PDV\\Performance", "Language", languageId ) )
	{
		languageId = 0;
		newInstallation = true;
	}
	return newInstallation;
};


void TEvent::Expired( bool _expired )
{
	expired = _expired;
};

bool TEvent::Expired( void ) const
{
	return expired;
};

TString TEvent::GameVersionString( void ) const
{
	return gameVersionString;
};

void TEvent::RegisteredMessage( const TString& _registeredMsg )
{
	registeredMsg = _registeredMsg;
};

bool TEvent::Active( void ) const
{
	return active;
};

void TEvent::Active( bool _active )
{
	active = _active;
};

size_t TEvent::ShadowLevel( void ) const
{
	return shadowLevel;
};

void TEvent::ShadowLevel( size_t _shadowLevel )
{
	shadowLevel = _shadowLevel;
};

void TEvent::AddSmoke( float x, float y, float z, 
					   float dx, float dy, float dz )
{
	TSmoke::Add( x,y,z, dx,dy,dz );
};

TProjectile* TEvent::FireProjectile( TPoint& start, TPoint& dirn,
									 size_t graphicName, size_t range, 
									 float damage )
{
	return NULL;
};

TProjectile* TEvent::FireProjectile( const byte* buf )
{
	return NULL;
}

void TEvent::ExplodeAt( const TPoint& point, size_t explosionType, float damage )
{
};

void TEvent::Fire( TPoint& start, TPoint& dirn, size_t range, size_t damage )
{
};

void TEvent::DropExplosive( float x, float y, float z, size_t timer )
{
};

TExplosive* TEvent::DropExplosive( const byte* buf )
{
	return NULL;
}

bool TEvent::AddTransport( TLandscapeObject& v2, TLandscapeObject& target )
{
	return false;
}

bool TEvent::AddTransport( TLandscapeObject& obj )
{
	return false;
}

TCompoundObject* TEvent::GetObjectByName( const TString& name, size_t objectIndex, 
										  size_t numObjects )
{
	return NULL;
}

void TEvent::ResetGameObjects( void )
{
};

byte TEvent::Map( void ) const
{
	return map;
};

void TEvent::CalculateExplosionDamage( const TPoint& point, float damageFactor )
{
};

void TEvent::AddGameObject( TLandscapeObject* obj )
{
};

TBinObject* TEvent::ProjectileNameToBin( size_t graphicName ) const
{
	return NULL;
};

size_t TEvent::ProjectileBinToName( TBinObject* graphic ) const
{
	return 0;
};

void TEvent::ReloadAsRequired( void )
{
};

TTips& TEvent::Tips( void )
{
	return tips;
};

size_t TEvent::MyId( void ) const
{
	return myId;
};

byte TEvent::GameType( void ) const
{
	return gameType;
};

#ifdef _USEGUI
TMapPlayerInfo TEvent::LandscapeMapDef( void )
{
	TDefFile def;
	TString errStr;
	PreCond( GetStoryDef( map, errStr, def ) );
	return def.GetPlayerInfo();
};

bool TEvent::GetStoryDef( size_t map, TString& errStr, TDefFile& def )
{
	if ( prepareClientServer )
	{
		TString storyName = "story";
		size_t index = map+1;
		if ( index<10 )
		{
			storyName = storyName + "0" + Int2Str(index);
		}
		else
		{
			storyName = storyName + Int2Str(index);
		}
		storyName = storyName + ".def";
		return def.LoadBinary( storyName, "data\\stories", errStr, true );
	}
	else
	{
		TStory* story = stories.Story(map);
		PostCond( story!=NULL );
		TString defFile = story->Map();
		return def.LoadBinary( defFile, "data\\stories", errStr, true );
	}
};
#endif

void TEvent::ClientNetworkLogic( void )
{
};

void TEvent::SetupGuest( const TString& _playerName, size_t port, 
						 const TString& _hostName )
{
};

bool TEvent::ClientReceivedWelcome( void ) const
{
	return false;
};

bool TEvent::ClientReceivedStart( void ) const
{
	return false;
};

bool TEvent::ClientError( void ) const
{
	return false;
};

void TEvent::ClientError( bool )
{
};

TString TEvent::ClientErrorStr( void ) const
{
	return "";
};

bool TEvent::Modified( void ) const
{
	return false;
};

void TEvent::Modified( bool _modified )
{
};

void TEvent::SaveDefn( void )
{
};

TLandscape* TEvent::LandObj( void )
{
	return NULL;
};

//==========================================================================

