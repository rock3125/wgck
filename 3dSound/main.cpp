//
//	Minimal OpenGL interface for BC++ 5.02 - call SetupOpenGL to start OpenGL
//

//---------------------------------------------------------------------------

#include <windows.h>            // Window defines
#include <windowsx.h>
#include <math.h>				// Define for sine and cosine
#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>
#include <string.h>

//---------------------------------------------------------------------------

#include <gl\gl.h>
#include <gl\glu.h>

//---------------------------------------------------------------------------

class TSetupOpenGL
{
public:
	TSetupOpenGL( HWND );
	~TSetupOpenGL(void);

	bool properinit;
	char errstr[256];

	int  SetDCPixelFormat(HDC hDC);
	bool MakePalette(HDC SceneDC, int pf, HPALETTE &ScenePalette );

	HDC					hDC;
	HGLRC				hRC;
private:
};

//---------------------------------------------------------------------------

//
//	DLL hook into this system
//	Create the TApp class and return it
//
bool WINAPI SetupOpenGL( HWND hwnd, HGLRC& rc, char* _errstr )
{
	TSetupOpenGL* app = new TSetupOpenGL(hwnd);
	if ( app->properinit )
	{
		rc = app->hRC;
		_errstr[0] = 0;
		return true;
	}
	else
	{
		strcpy(_errstr,app->errstr);
		rc = NULL;
		delete app;
	}
	return false;
}

//---------------------------------------------------------------------------

TSetupOpenGL::TSetupOpenGL( HWND hWnd )
	: hDC(NULL),
	  hRC(NULL)
{
	errstr[0] = 0;
	properinit = false;

	// Store the device context
	hDC = GetDC(hWnd);
	int pf;
	if ( (pf=SetDCPixelFormat(hDC)) ==-1 )
		return;

	HPALETTE palette;
	if ( !MakePalette(hDC,pf, palette ) )
		return;

	hRC = wglCreateContext(hDC);
	if ( hRC==NULL )
	{
		sprintf(errstr,"Error: Render Context Creation failed for this window (0x%x)",hWnd);
		return;
	}

	DeleteDC(hDC);

	properinit = true;
}

//---------------------------------------------------------------------------

TSetupOpenGL::~TSetupOpenGL( void )
{
}

//---------------------------------------------------------------------------

//
//	Select the pixel format for a given device context
//	returns -1 on failure
//
int TSetupOpenGL::SetDCPixelFormat(HDC hDC)
{
	int nPixelFormat;

	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),			// Size of this structure
		1,                                      // Version of this structure
		PFD_DRAW_TO_WINDOW |                    // Draw to Window (not to bitmap)
		PFD_SUPPORT_OPENGL |					// Support OpenGL calls in window
		PFD_DOUBLEBUFFER,                       // Double buffered
		PFD_TYPE_RGBA,                          // RGBA Color mode
		32,                                     // Want 32 bit color
		0,0,0,0,0,0,                            // Not used to select mode
		0,0,                                    // Not used to select mode
		0,0,0,0,0,                              // Not used to select mode
		32,                                     // Size of depth buffer
		0,                                      // Not used to select mode
		0,                                      // Not used to select mode
		PFD_MAIN_PLANE,                         // Draw in main plane
		0,                                      // Not used to select mode
		0,0,0 };                                // Not used to select mode

	// Choose a pixel format that best matches that described in pfd
	nPixelFormat = ChoosePixelFormat(hDC, &pfd);

	if ( pfd.cDepthBits<=8 )
	{
		sprintf(errstr,"Error:  Your video card hardware does not support\na suitable size depth buffer");
		return -1;
	}

	// Set the pixel format for the device context
	SetPixelFormat(hDC, nPixelFormat, &pfd);

	return nPixelFormat;
}

//---------------------------------------------------------------------------

//
//	'MakePalette()' - Make a color palette for RGB colors if necessary.
//
bool TSetupOpenGL::MakePalette(HDC SceneDC, int pf, HPALETTE &ScenePalette )
{
	PIXELFORMATDESCRIPTOR pfd;

	ScenePalette = NULL;
	DescribePixelFormat(SceneDC, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

	//usingpalette = false;
	if ( !(pfd.dwFlags & PFD_NEED_PALETTE) )
		return true;

	sprintf(errstr,"This program does not support 256 colours or less.\nPlease switch to a higher colour depth before using this program.");
	return false;
}

//---------------------------------------------------------------------------

