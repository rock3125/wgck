#ifndef __MAIN_WIN32_H__
#define __MAIN_WIN32_H__

//==========================================================================

#ifdef _DXSOUND_
#include <common/sounds.h>
#endif
#include <ddraw.h>

//==========================================================================
//
//	TEvent is a base class to the actual TApp that is created
//	in win32.  All its behaviour you're interested in should be 
//	overwritten by TApp and implemented according to taste.
//	DO NOT modify TEvent itself!
//

class TEvent
{
public:
	TEvent( bool fog=false );
    virtual ~TEvent( void );

    // Viewport setup
	virtual void 	SetupGL3d( float depth, int width, int height ); // perspective
	virtual void 	SetupGL2d( float depth, int width, int height ); // orthographic

    // Event system
    virtual bool	Setup( HWND _hwnd, float depth, int widht, int height, TString& errStr );
	virtual bool	StartupSystem( HWND hwnd, TString& errStr );

	// setup DirectX 7
	bool	SetupDirectX( HWND hWindow, bool fullScreen, int Wxsize, int Wysize, 
						  int Wbitdepth, TString& errStr );
	void	DXCleanup( HWND hWnd );

	// windows management routines for switching modes etc.
	int		SetDCPixelFormat( HDC hDC, int bitDepth, TString& errStr );
	void	DeactivateWindow( HWND hWnd, bool& active, bool& fullScreen,
							  int Wxsize, int Wysize, int Wbitdepth );
	void	SwitchFullScreen( HWND hWnd, bool& fullScreen, 
							  int Wxsize, int Wysize, int Wdepth );
	void	ResetResolution( HWND hWnd, bool fullScreen, 
							 int Wxsize, int Wysize, float depth, int Wbdepth );

    virtual void	Destroy( void );
	virtual void 	KeyDown( int );
	virtual void 	KeyUp( int );
	virtual void 	KeyPress( int );
	virtual void 	MouseDown( int, int, int );
	virtual void 	MouseUp( int, int, int );
	virtual void 	MouseMove( int, int, bool );

    virtual void 	Render(void); // overwrite & put your openGL drawing code in here
	virtual void 	Draw(void); // do not overwrite unless you're know what your doing
    							// draw calls render if you need to overwrite something
								// overwrite render instead
	virtual void	Logic(void); // process key logic - timed @ 20fps currently

	virtual void 	SetupFog( bool _fog ); // setup fog

	void			VWidth( float w );
	void			VHeight( float h );

    // Get main window handle
    HWND	Window( void ) const;

	// force end of program?
	bool Finished( void ) const;
	size_t PolygonsPerSecond( float fps ) const;

private:
    void	Window( HWND _hwnd ); // set window handle
	void 	InitGL( float depth, int w, int h );
	void 	SetupLight( void );

	int		ChoosePixelFormatEx( HDC hdc,int& bpp,int& depth,
								 int& dbl,int& acc, int& qFlag,
								 int& accelType );

	bool DXSetup( HWND hWnd, bool fullScreen, int width, int height, int depth, 
				  TString& errStr );
	void DXError( int hRes, const TString& routine, TString& errStr );

private:
    HWND			hwnd;			// window Handle
	bool			fog;			// fog y/n
    bool			initialised;	// system properly initialised
	float			vdepth;			// viewport dimensions
	float			vwidth;			// 0..width, 0..height
	float			vheight;		// 1..depth (z axis)
	bool			finished;		// end program?
	size_t			numberOfPolygons;

	#ifdef _DXSOUND_
	friend class TApp;
    TSound*		soundSystem;
	#endif

	LPDIRECTDRAW7					lpDD;			// DirectDraw Object
	LPDIRECTDRAWSURFACE7			lpDDPrimary;	// DirectDraw Primary Surface
	LPDIRECTDRAWSURFACE7			lpDDBack;		// DirectDraw Back Surface
	LPDIRECTDRAWCLIPPER				lpClip;			// DirectDraw Clipper Object
	DDSURFACEDESC2					ddsd;
	DDSCAPS2						ddscaps;
};

//==========================================================================

#endif


