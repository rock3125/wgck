#include <windows.h>
#include <ddraw.h>

// Global interface pointers
LPDIRECTDRAW7					lpDD=NULL;				// DirectDraw Object
LPDIRECTDRAWSURFACE7			lpDDPrimary=NULL;		// DirectDraw Primary Surface
LPDIRECTDRAWSURFACE7			lpDDBack=NULL;			// DirectDraw Back Surface
LPDIRECTDRAWSURFACE7			lpBmp=NULL;				// Bitmap surface
LPDIRECTDRAWSURFACE7			lpTemp=NULL;			// Temporary surface
LPDIRECTDRAWCLIPPER				lpClip=NULL;			// DirectDraw Clipper Object
DDSURFACEDESC2					ddsd;
DDSCAPS2						ddscaps;

// Cleanup Function to Release and Switch modes
#define SafeRelease(x) if(x!=NULL) { x->Release(); x=NULL; }


void Cleanup( HWND hWnd )
{
	if(lpDD != NULL)
	{
		lpDD->SetCooperativeLevel(hWnd, DDSCL_NORMAL);
		SafeRelease(lpDDBack); 
		SafeRelease(lpDDPrimary); 
	}
}



// Change display properties and setup DirectDraw
bool Ddraw( HWND hWnd, bool fullScreen, int width, int height, int depth, char*& errStr )
{
	errStr = NULL;
	if (!fullScreen)
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
			errStr="Error: CreateClipper failed"; 
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
			errStr="Error: CreateSurface failed"; 
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
			errStr="Error: Create back surface failed"; 
			return false; 
		}
	}
	else
	{
		// Set the cooperative level for Fullscreen Mode
		int hRes= lpDD->SetCooperativeLevel(hWnd, DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN|DDSCL_ALLOWREBOOT);
		if ( hRes!=DD_OK ) 
		{ 
			errStr="Error: SetCooperativeLevel failed"; 
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
		ddsd.ddpfPixelFormat.dwRGBBitCount=depth;
	
		hRes= lpDD->SetDisplayMode(width,height,depth,0,0);
		if ( hRes!=DD_OK ) 
		{ 
			errStr="Error: could not set the required resolution"; 
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
			errStr="Error: CreateSurface failed"; 
			return false; 
		}
			
		ZeroMemory(&ddscaps, sizeof(ddscaps));
		ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
	
		// Create the Back Surface
		hRes = lpDDPrimary->GetAttachedSurface(&ddscaps, &lpDDBack);
		if ( hRes!=DD_OK ) 
		{ 
			errStr="Error: Create back surface failed"; 
			return false; 
		}
	}
	return true;
}

