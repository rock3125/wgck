#include <precomp_header.h>

#include <win32/win32.h>
#include <win32/events.h>

#define SafeRelease(x) if(x!=NULL) { x->Release(); x=NULL; }

//==========================================================================
//
//	TEvent is a base class to the actual TApp that is created
//	in win32.  All its behaviour you're interested in should be 
//	overwritten by TApp and implemented according to taste.
//	DO NOT modify TEvent itself!
//
TEvent::TEvent( bool _fog )
	: initialised(false),
      fog( _fog ),
	  #ifdef _DXSOUND_
	  soundSystem(NULL),
	  #endif
	  lpDD(NULL),
	  lpDDPrimary(NULL),
	  lpDDBack(NULL),
	  lpClip(NULL),
      hwnd( NULL )
{
    initialised = true;
	finished = false;
};


//
//	Destruct, release dc and hglrc
//
TEvent::~TEvent( void )
{
    hwnd = NULL;
};


//
//	Setup openGL
//	initialise hglrc and set pixel format - must be called from WM_CREATE
//	in windows event loop - can't be called from anywhere else
//
bool TEvent::Setup( HWND _hwnd, float depth, int width, int height, TString& errStr )
{
	if ( !initialised )
    {
    	errStr = "Initialisation of setupGL.dll failed";
        return false;
    }
	ConsolePrint( "setting up OpenGL" );
	Window( _hwnd );

    // setup openGL etc.
    InitGL( depth, width, height );

    return true;
};


//
//	Initialise openGL settings
//
void TEvent::InitGL( float depth, int w, int h )
{
	vdepth = depth;
	vwidth = float(w);
	vheight = float(h);

	// set viewport size
	// set projection transformation matrix
	glViewport(0,0,(GLsizei)w, (GLsizei)h );
	SetupGL3d( depth, w, h );

	if ( useHint1 )
		glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
	if ( useHint2 )
		glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );

	glDrawBuffer(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_CULL_FACE); // culling on, colour on, depth testing on
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_POLYGON_SMOOTH);
	glEnable(GL_COLOR);

//	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	if ( useShadeModel )
		glShadeModel(GL_SMOOTH);
	if ( useColourMaterial )
	{
		glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);
		glEnable(GL_COLOR_MATERIAL);
	}
	if ( useLighting )
	{
		glEnable(GL_LIGHTING);
		glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 0);
		SetupLight();
		glEnable(GL_LIGHT0);
	}

	SetupFog( fog );

	glLoadIdentity();
}


//
//	setup single light source for now
//
void TEvent::SetupLight( void )
{
	GLfloat position[]	= { 1000.0f, 1000.0f, 1000.0f, 0.0f, 0.0f };
	GLfloat one[]		= { 0.9f, 0.9f, 0.9f, 0.0f };
	GLfloat half[]		= { 0.6f, 0.6f, 0.6f, 0.0f };

	// lighting setup
	glLightfv(GL_LIGHT0, GL_AMBIENT,  half );
	glLightfv(GL_LIGHT0, GL_POSITION, position );
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  one );
	glLightfv(GL_LIGHT0, GL_SPECULAR, one );
}



void TEvent::SetupFog( bool _fog )
{
	//
	// just in case we do want fog - here is the code
	//
	fog = _fog;
	if ( _fog )
	{
		glEnable(GL_FOG);

		GLint fogMode;
		GLfloat fogColor[4] = {0.8f, 0.8f, 0.8f, 0.2f};

		fogMode = GL_EXP2;
		glFogi(GL_FOG_MODE, 	fogMode );
		glFogf(GL_FOG_START, 	vdepth/5 );
		glFogf(GL_FOG_END,   	vdepth );
		glFogf(GL_FOG_DENSITY,  0.01f );
		glFogfv(GL_FOG_COLOR, 	fogColor);

		glClearColor(0.8f,0.8f,0.8f,0.2f);
		glHint(GL_FOG_HINT, GL_NICEST);
	}
	else
	{
	    glClearColor(0.15f,0.15f,0.25f,0); // black, alpha 0
	    glDisable(GL_FOG); // no fog
	}
}


//
//	Setup perspective view
//
void TEvent::SetupGL3d( float depth, int width, int height )
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f,float(width) / float(height), 1.0f, depth );
	glMatrixMode(GL_MODELVIEW);
}


//
//	Setup 2d ortho view
//
void TEvent::SetupGL2d( float depth, int width, int height )
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, float(width), 0, float(height), 1, depth );
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

    // call render code
    Render();

	// force openGL draw
	if ( useFlush )
		glFlush(); 
}



HWND TEvent::Window( void ) const
{
	return hwnd;
};



void TEvent::Window( HWND _hwnd )
{
	hwnd = _hwnd;
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
void TEvent::KeyDown( int kd )
{
};


//
//	Callback event - can be overwritten by user through inheritance
//
void TEvent::KeyUp( int ku )
{
};


//
//	Callback event - can be overwritten by user through inheritance
//
void TEvent::KeyPress( int kp )
{
};


//
//	Callback event - can be overwritten by user through inheritance
//
void TEvent::MouseDown( int button, int x, int y)
{
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
void TEvent::MouseMove( int x, int y, bool ctrlDown )
{
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
};


//
//	Callback event - can be overwritten by user through inheritance
//
bool TEvent::StartupSystem( HWND, TString& )
{
	return true;
};


//
//	Return index to best possible pixel format for running this app
//	windowed!
//
int TEvent::ChoosePixelFormatEx( HDC hdc,int& bpp,int& depth,
								 int& dbl,int& acc, int& qFlag,
								 int& accelType )
{ 
	int wbpp; 
	int wdepth; 
	int wdbl; 
	int wacc; 

	wbpp= bpp; 
	wdepth=depth;
	wdbl=dbl; 
	wacc=acc; 
	accelType = 0;

	PIXELFORMATDESCRIPTOR pfd; 
	
	ZeroMemory(&pfd,sizeof(pfd)); 
	pfd.nSize=sizeof(pfd); 
	pfd.nVersion=1;
	
	ConsolePrint("Enumerating pixel formats");

	int num = DescribePixelFormat(hdc,1,sizeof(pfd),&pfd);

	if (num==0) 
		return 0;
  
	int maxqual=0; 
	int maxindex=0;
	int max_bpp, max_depth, max_dbl, max_acc;
	bool mcd,icd,soft;
	
	for (int i=1; i<=num; i++)
	{
		ZeroMemory(&pfd,sizeof(pfd)); 
		pfd.nSize = sizeof(pfd); 
		pfd.nVersion=1;
    
		DescribePixelFormat(hdc,i,sizeof(pfd),&pfd);
		int bpp=pfd.cColorBits;
		int depth=pfd.cDepthBits;
		bool pal=(pfd.iPixelType==PFD_TYPE_COLORINDEX);

		mcd=((pfd.dwFlags & PFD_GENERIC_FORMAT) && (pfd.dwFlags & PFD_GENERIC_ACCELERATED));
		soft=((pfd.dwFlags & PFD_GENERIC_FORMAT) && !(pfd.dwFlags & PFD_GENERIC_ACCELERATED));
		icd=(!(pfd.dwFlags & PFD_GENERIC_FORMAT) && !(pfd.dwFlags & PFD_GENERIC_ACCELERATED));

		if ( mcd ) 
			accelType = 1;
		else if ( icd )
			accelType = 2;

		bool opengl = (pfd.dwFlags & PFD_SUPPORT_OPENGL)>0;
		bool window = (pfd.dwFlags & PFD_DRAW_TO_WINDOW)>0;
		bool bitmap = (pfd.dwFlags & PFD_DRAW_TO_BITMAP)>0;
		bool dbuff  = (pfd.dwFlags & PFD_DOUBLEBUFFER)>0;

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

//		ConsolePrint( "format %02d: OpenGL:%d, Windowed:%d, Bitmap:%d, Double buffered:%d, %s, bpp=%02d, depth=%02d, palette=%d",
//					  i, opengl, window, bitmap, dbuff, buf, bpp, depth, pal );
		
		//
		int q=0;
		if (opengl && window) 
			q=q+0x8000;
		if (wdepth==-1 || (wdepth>0 && depth>0)) 
			q=q+0x4000;
		if (wdbl==-1 || (wdbl==0 && !dbuff) || (wdbl==1 && dbuff)) 
			q=q+0x2000;
		if (wacc==-1 || (wacc==0 && soft) || (wacc==1 && (mcd || icd))) 
			q=q+0x1000;
		if (mcd || icd) 
			q=q+0x0040; 
		if (icd) 
			q=q+0x0002;
		if (wbpp==-1 || (wbpp==bpp)) 
			q=q+0x0800;
		if (bpp>=16) 
			q=q+0x0020; 
		if (bpp==16) 
			q=q+0x0008;
		if (wdepth==-1 || (wdepth==depth)) 
			q=q+0x0400;
		if (depth>=16) 
			q=q+0x0010; 
		if (depth==16) 
			q=q+0x0004;
		if (!pal) 
			q=q+0x0080;
		if (bitmap) 
			q=q+0x0001;

		if (q>maxqual) 
		{
			maxqual=q; 
			maxindex=i;
			max_bpp=bpp; 
			max_depth=depth; 
			max_dbl=dbuff?1:0; 
			max_acc=soft?0:1;
		}
	}

	if (maxindex==0) 
		return maxindex;

	bpp=max_bpp;
	depth=max_depth;
	dbl=max_dbl;
	acc=max_acc;
	qFlag = maxqual;
  
	return maxindex;
}


void TEvent::DeactivateWindow( HWND hWnd, bool& active, bool& fullScreen,
							   int Wxsize, int Wysize, int Wbitdepth )
{
	active = false;

	// swap back to desktop
	// if necessairy
	if ( fullScreen )
	{
		DXCleanup(hWnd);

		fullScreen = false;

		long style = WS_BORDER | WS_CAPTION | WS_SYSMENU |
					 WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | 
					 WS_VISIBLE;
		::SetWindowLong(hWnd,GWL_STYLE,style);
		::SetWindowPos(hWnd,HWND_TOP,0,0,Wxsize,Wysize,SWP_FRAMECHANGED|SWP_SHOWWINDOW);

		TString errStr;
		if ( !DXSetup(hWnd,fullScreen,Wxsize,Wysize,Wbitdepth,errStr) ) 
		{
			WriteString( errStr.c_str() );
			if ( !fullScreen )
			{
				DXCleanup( hWnd );
			}
			::PostQuitMessage(-1);
		}
	}
};


//
// Select the pixel format for a given device context
//
int TEvent::SetDCPixelFormat( HDC hDC, int bitDepth, TString& errStr )
{

	int bpp=-1; // don't care. (or a positive integer)
	int depth=-1; // don't care. (or a positive integer)
	int dbl=1; // we want double-buffering. (or -1 for 'don't care', or 0 for 'none')
	int acc=1; // we want acceleration. (or -1 or 0)
	int qFlag = 0; // quality bitmask
	int accelType = 0;
	int pf = ChoosePixelFormatEx(hDC,bpp,depth,dbl,acc,qFlag,accelType);

	if ( pf==0 )
	{
	   	errStr = "Can't find an accelerated openGL driver for this system";
		return 0;
	}
	TString buf;
	buf = "Selected pixel format " + IntToString(pf);
	WriteString( buf.c_str() );

	// set pixel format
	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd,sizeof(pfd)); 
	pfd.nSize = sizeof(pfd); 
	pfd.nVersion=1;

	DescribePixelFormat(hDC,pf,sizeof(pfd),&pfd);

	// set some unnecessairy fields back to 0
	pfd.cAccumBits = 0;
	pfd.cAccumAlphaBits = 0;
	pfd.cAccumBlueBits = 0;
	pfd.cAccumGreenBits = 0;
	pfd.cAccumRedBits = 0;

	if ( !useMask )
	{
		pfd.cRedBits = 0;
		pfd.cRedShift = 0;
		pfd.cGreenBits = 0;
		pfd.cGreenShift = 0;
		pfd.cBlueBits = 0;
		pfd.cBlueShift = 0;
	}
	if ( bitDepth!=0 )
		pfd.cDepthBits = bitDepth;

	if ( !SetPixelFormat(hDC,pf,&pfd ) )
	{
		errStr = "TEvent::SetPixelFormat failed (" + IntToString( GetLastError() );
		errStr = errStr + ")";
		return 0;
	}
	return pfd.cColorBits;
}


//
//	Switch full screen / windowed
//
void TEvent::SwitchFullScreen( HWND hWnd, bool& fullScreen, 
							   int Wxsize, int Wysize, int Wbdepth )
{
	DXCleanup(hWnd);

	fullScreen = !fullScreen;
	if ( fullScreen )
	{
		long style = WS_POPUP | WS_VISIBLE;
		::SetWindowLong(hWnd,GWL_STYLE,style);
		::SetWindowPos(hWnd,HWND_TOP,0,0,Wxsize,Wysize,SWP_FRAMECHANGED|SWP_SHOWWINDOW);
	}
	else
	{
		long style = WS_CAPTION | WS_SYSMENU |
					 WS_CLIPCHILDREN | WS_CLIPSIBLINGS | 
					 WS_VISIBLE | WS_OVERLAPPED | 
					 WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
		::SetWindowLong(hWnd,GWL_STYLE,style);
		::SetWindowPos(hWnd,HWND_TOP,0,0,Wxsize,Wysize,SWP_FRAMECHANGED|SWP_SHOWWINDOW);
	}

	TString errStr;
	if ( !DXSetup(hWnd,fullScreen,Wxsize,Wysize,Wbdepth,errStr) ) 
	{
		WriteString( errStr.c_str() );
		if ( !fullScreen )
		{
			DXCleanup( hWnd );
		}
		::PostQuitMessage(-1);
	}
};


void TEvent::ResetResolution( HWND hWnd, bool fullScreen, int Wxsize, int Wysize, 
							  float depth, int Wbdepth )
{
	::MoveWindow( hWnd, 0,0, int(Wxsize), int(Wysize), true );

	DXCleanup(hWnd);
	TString errStr;
	if ( !DXSetup(hWnd,fullScreen,int(Wxsize),int(Wysize),Wbdepth,errStr) ) 
	{
		WriteString( errStr.c_str());
		::PostQuitMessage(-1);
	}

	if ( !Setup( hWnd, depth, Wxsize, Wysize, errStr ) )
	{
		WriteString( errStr.c_str() );
		::PostQuitMessage(-1);
	}
};

//==========================================================================

// Cleanup Function to Release and Switch modes
void TEvent::DXCleanup( HWND hWnd )
{
	if(lpDD != NULL)
	{
		lpDD->SetCooperativeLevel(hWnd, DDSCL_NORMAL);
		SafeRelease(lpDDBack); 
		SafeRelease(lpDDPrimary); 
	}
}


// Change display properties and setup DirectDraw
bool TEvent::DXSetup( HWND hWnd, bool fullScreen, int width, int height, int bdepth, TString& errStr )
{
	errStr = "";
	if (!fullScreen && lpDD!=NULL )
	{
		// Set the cooperative level for Windowed mode
		int hRes = lpDD->SetCooperativeLevel(hWnd, DDSCL_NORMAL|DDSCL_ALLOWREBOOT);
		if ( hRes!=DD_OK )
		{ 
			errStr = "Error: SetCooperativeLevel failed";
			return false; 
		}

		// Create the clipper
		hRes = lpDD->CreateClipper(NULL,&lpClip,NULL);
		if ( hRes!=DD_OK ) 
		{ 
			errStr = "Error: CreateClipper failed";
			return false; 
		}
		lpClip->SetHWnd(0,hWnd); 

		// Create the primary surface
		memset(&ddsd,0,sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

		hRes=lpDD->CreateSurface( &ddsd, &lpDDPrimary, NULL );
		if ( hRes!=DD_OK ) 
		{ 
			errStr = "Error: CreateSurface failed";
			return false; 
		}

		// Set the Clipper for the primary surface
		lpDDPrimary->SetClipper(lpClip);
		lpClip->Release();
		lpClip = NULL;
		
		ddsd.dwFlags        = DDSD_WIDTH|DDSD_HEIGHT|DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN|DDSCAPS_3DDEVICE;

		// Set the dimensions of the back buffer.
		RECT rcScreen;
		GetClientRect( hWnd, &rcScreen );
		ClientToScreen( hWnd, (POINT*)&rcScreen.left );
		ddsd.dwWidth  = rcScreen.right - rcScreen.left;
		ddsd.dwHeight = rcScreen.bottom - rcScreen.top; 
 
		// Create the back buffer.
		hRes=lpDD->CreateSurface( &ddsd, &lpDDBack, NULL );
		if ( hRes!=DD_OK ) 
		{ 
			errStr = "Error: Create back surface failed";
			return false; 
		}
	}
	else if ( lpDD!=NULL )
	{
		// Set the cooperative level for Fullscreen Mode
		int hRes= lpDD->SetCooperativeLevel(hWnd, DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN|DDSCL_ALLOWREBOOT);
		if ( hRes!=DD_OK ) 
		{ 
			errStr = "Error: SetCooperativeLevel failed";
			return false; 
		}

		// Get the dimensions of the viewport and screen
		ZeroMemory((void*)&ddsd,sizeof ddsd);
		ddsd.dwSize=sizeof(DDSURFACEDESC2);
		ddsd.dwFlags=DDSD_HEIGHT|DDSD_WIDTH|DDSD_PIXELFORMAT;
		ddsd.dwWidth=width;
		ddsd.dwHeight=height;
		ddsd.ddpfPixelFormat.dwSize=sizeof(DDPIXELFORMAT);
		ddsd.ddpfPixelFormat.dwFlags=DDPF_RGB;
		ddsd.ddpfPixelFormat.dwRGBBitCount=bdepth;
	
		// set default if none set
		if ( bdepth==0 )
			bdepth = 16;

		hRes= lpDD->SetDisplayMode(width,height,bdepth,0,0);
		if ( hRes!=DD_OK ) 
		{ 
			errStr = "Error: could not set the required resolution";
			return false; 
		}

		// Create the Primary Surface with 1 back buffer
		ZeroMemory(&ddsd,sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP
			| DDSCAPS_COMPLEX | DDSCAPS_3DDEVICE | DDSCAPS_VIDEOMEMORY;
		ddsd.dwBackBufferCount = 1;
		hRes = lpDD->CreateSurface( &ddsd, &lpDDPrimary, NULL);
		if ( hRes!=DD_OK ) 
		{ 
			DXError( hRes, "CreateSurface", errStr ); 
			return false; 
		}
			
		ZeroMemory(&ddscaps, sizeof(ddscaps));
		ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
	
		// Create the Back Surface
		hRes = lpDDPrimary->GetAttachedSurface(&ddscaps, &lpDDBack);
		if ( hRes!=DD_OK ) 
		{ 
			errStr = "Error: Create back surface failed";
			return false; 
		}
	}
	return true;
}


void TEvent::DXError( int hRes, const TString& routine, TString& errStr )
{
	errStr = "\n************\nError: ";
	errStr = errStr + routine;
	errStr = errStr + ": ";
	switch ( hRes )
	{
		case DDERR_INCOMPATIBLEPRIMARY:
			{
				errStr = errStr + "DDERR_INCOMPATIBLEPRIMARY";
			}
			break;

		case DDERR_INVALIDCAPS:
			{
				errStr = errStr + "DDERR_INVALIDCAPS";
			}
			break;

		case DDERR_INVALIDOBJECT:
			{
				errStr = errStr + "DDERR_INVALIDOBJECT";
			}
			break;

		case DDERR_INVALIDPARAMS:
			{
				errStr = errStr + "DDERR_INVALIDPARAMS";
			}
			break;

		case DDERR_INVALIDPIXELFORMAT:
			{
				errStr = errStr + "DDERR_INVALIDPIXELFORMAT";
			}
			break;

		case DDERR_NOALPHAHW:
			{
				errStr = errStr + "DDERR_NOALPHAHW";
			}
			break;

		case DDERR_NOCOOPERATIVELEVELSET:
			{
				errStr = errStr + "DDERR_NOCOOPERATIVELEVELSET";
			}
			break;

		case DDERR_NODIRECTDRAWHW:
			{
				errStr = errStr + "DDERR_NODIRECTDRAWHW";
			}
			break;

		case DDERR_NOEMULATION:
			{
				errStr = errStr + "DDERR_NOEMULATION";
			}
			break;

		case DDERR_NOEXCLUSIVEMODE:
			{
				errStr = errStr + "DDERR_NOEXCLUSIVEMODE";
			}
			break;

		case DDERR_NOFLIPHW:
			{
				errStr = errStr + "DDERR_NOFLIPHW";
			}
			break;

		case DDERR_NOMIPMAPHW:
			{
				errStr = errStr + "DDERR_NOMIPMAPHW";
			}
			break;

		case DDERR_NOOVERLAYHW:
			{
				errStr = errStr + "DDERR_NOOVERLAYHW";
			}
			break;

		case DDERR_NOZBUFFERHW:
			{
				errStr = errStr + "DDERR_NOZBUFFERHW";
			}
			break;

		case DDERR_OUTOFMEMORY:
			{
				errStr = errStr + "DDERR_OUTOFMEMORY";
			}
			break;

		case DDERR_OUTOFVIDEOMEMORY:
			{
				errStr = errStr + "DDERR_OUTOFVIDEOMEMORY";
			}
			break;

		case DDERR_PRIMARYSURFACEALREADYEXISTS:
			{
				errStr = errStr + "DDERR_PRIMARYSURFACEALREADYEXISTS";
			}
			break;

		case DDERR_UNSUPPORTEDMODE:
			{
				errStr = errStr + "DDERR_UNSUPPORTEDMODE";
			}
			break;

		default:
			{
				errStr = errStr + "unknown";
			}
			break;
	}
	errStr = errStr + "\n";
}


bool TEvent::SetupDirectX( HWND hWindow, bool fullScreen, int Wxsize, int Wysize, 
						   int Wbitdepth, TString& errStr )
{
	int hRes=DirectDrawCreateEx(NULL,(LPVOID*)&lpDD,IID_IDirectDraw7,NULL);
	if ( hRes!=DD_OK) 
	{ 
		errStr = "Error: Could not create DirectX7 object";
		return false; 
	}

	if ( !DXSetup(hWindow,fullScreen,Wxsize,Wysize,Wbitdepth,errStr) ) 
	{
		WriteString( errStr.c_str() );
		return false;
	}
	return true;
};


void TEvent::VWidth( float w )
{
	vwidth = w;
};


void TEvent::VHeight( float h )
{
	vheight = h;
};


bool TEvent::Finished( void ) const
{
	return finished;
};

//==========================================================================




