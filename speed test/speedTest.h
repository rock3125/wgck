#ifndef _SPEEDTEST_SPEEDTEST_H_
#define _SPEEDTEST_SPEEDTEST_H_

//==========================================================================

#include <object/binobject.h>
#include <object/landscape.h>
#include <win32/events.h>

//==========================================================================

class TApp : public TEvent
{
public:
	TApp( HINSTANCE );
    ~TApp( void );

	bool StartupSystem( TString& errStr );
	void Destroy( void );

	void Render( void );
	void Logic( void );

	void MouseDown( int button, int x, int y);
	void MouseUp( int button, int, int );
	void MouseMove( int x, int y );
	void MouseDown( int button, int x, int y, int dx, int dy );
	void MouseUp( int button, int, int, int, int );
	void MouseMove( int x, int y, int, int );
	void KeyDown( int kd );
	void KeyUp( int ku );
    
	size_t PolygonsPerSecond( float fps ) const;

	bool GetRegistrySettings( void );

	TFont&	Arial( void );

	bool	Initialised( void ) const;
	void	Initialised( bool i );

	TCompoundObject* GetObjectByName( const TString& name );
	void AddTransport( TLandscapeObject );

public:
	bool		showBoundingBox;
    bool 		leftDown;
	bool		rightDown;

    float		yAngle;
    float		xAngle;
	float		deltaXAngle;
	float		deltaYAngle;

	size_t		numberOfPolygons;
	size_t		currentRenderDepth;
	int			cameraLocation;
	float		radius;

    float		lx;
    float		ly;
    float		lz;

	int			keys;

	bool		ctrlDown;

	TBinObject	spaceStation;
	TTexture*	marsTexture;
	TTexture	credits;

	float		offsetx,offsety;
	float		turnAngle1;

	bool		mp3playing;

	size_t		currentTime;

	bool		playMusic; // from registry

	// engine main vars

	size_t		kMinRenderDepth;
	size_t		kMaxRenderDepth;

	TString		versionString;

	TString		appName;
	TString		appTitle;
	TString		msgStr;

	TFont		arial;

	int			kFPS;

	bool		firstTimeFlag;
	bool		active;
	bool		consoleVisible;
	bool		showAbout;
	bool		refuel;

	bool		fullScreen;
	bool		minimised;
	bool		askOnce;
	bool		doSpeedTest; 
	bool		mouseCaptured;
	bool		firstTime;
	bool		fog;
	int			mouseButton;
	int			prevXPos;
	int			prevYPos;

	DWORD			startTime;
	DWORD			startTimerTime;
	int				frameCounter;
	int				fixedFrameCounter;
};

//==========================================================================

#endif
