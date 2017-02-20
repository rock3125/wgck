#ifndef __MAIN_WIN32_H__
#define __MAIN_WIN32_H__

//==========================================================================

#include <common/sounds.h>

#define kMaxResolutions 20

#include <systemDialogs/gui.h>
#include <object/smoke.h>
#include <object/character.h>
#include <object/projectile.h>
#include <object/explosive.h>
#include <win32/tips.h>

#ifdef _USEGUI
#include <win32/story.h>
#endif

class TJpeg;
class TPNG;

//==========================================================================
//
//	TEvent is a base class to the actual TApp that is created
//	in win32.  All its behaviour you're interested in should be 
//	overwritten by TApp and implemented according to taste.
//	DO NOT modify TEvent itself!
//

class _EXPORT TEvent
{
public:
	TEvent( HINSTANCE hInstance, bool fog=true );
    virtual ~TEvent( void );

    // Viewport setup
	virtual void 	SetupGL3d( float depth, float width, float height ); // perspective
	virtual void 	SetupGL2d( float depth, float width, float height ); // orthographic
	virtual void	SetupGL2dNormal( float depth, float width, float height );

	// Get resolution enum information
	size_t		NumResolutions( void ) const;
	bool		ChangeResolution( TString& errStr );
	void		GetResolution( size_t index, size_t& width, size_t& height,
							   size_t& bitDepth, size_t& zDepth, size_t& freq );

	// interface for join dialog
	virtual void	ClientNetworkLogic( void );
	virtual void	SetupGuest( const TString& _playerName, size_t port, const TString& _hostName );

	virtual bool	ClientReceivedWelcome( void ) const;
	virtual bool	ClientReceivedStart( void ) const;
	virtual bool	ClientError( void ) const;
	virtual void	ClientError( bool );
	virtual TString ClientErrorStr( void ) const;

    // Event system
	virtual bool	StartupSystem( TString& errStr );

	virtual TLandscape* LandObj( void );

	// call this every-time you switch resolutions
	bool	ResetResolution( HMENU menu, TString& errStr );
	HWND	SetupWindow( HMENU menu, TString& errStr );
	bool	LoadFont( TString& errStr );

	// windows management routines for switching modes etc.
	bool	DeactivateWindow( HWND hWnd, TString& errStr );

	// set engine lighting using a 24 hour clock
	void	AdjustLightingByTime( size_t hours24, size_t mins );

	// read resolution and screen info from registry
	virtual bool GetRegistrySettings( void );

	// force the app to be windowed at its current res
	bool	ForceWindowed( void );

	bool	Initialised( void ) const;
	void	Initialised( bool i );

	void	CurrentPage( TGUI* page );
	TGUI*	CurrentPage( void ) const;

	// firing system management
	// an object Fires, after which the projectile explodes
	// at a location.  The system always needs to draw
	// projectiles and explosions for the game itself
	virtual TProjectile* FireProjectile( TPoint& start, TPoint& dirn,
										 size_t graphicName, size_t range, 
										 float damage );
	virtual TProjectile* FireProjectile( const byte* buf );

	virtual void ExplodeAt( const TPoint& point, size_t explosionType, 
							float damage );

	virtual void Fire( TPoint& start, TPoint& dirn, size_t range, size_t damage );

	// drop explosive by player
	virtual void DropExplosive( float x, float y, float z, size_t timer );
	virtual TExplosive* DropExplosive( const byte* buf );

	// add a means of transportation
	virtual bool AddTransport( TLandscapeObject& v2, TLandscapeObject& target );
	virtual bool AddTransport( TLandscapeObject& obj );

	// return compound object from all loaded objects by searching by name
	// used by landscape to create a map with objects on it
	virtual TCompoundObject* GetObjectByName( const TString& name, size_t objectIndex=0, 
											  size_t numObjects=0 );

	virtual void CalculateExplosionDamage( const TPoint& point, float damageFactor );
	virtual void AddGameObject( TLandscapeObject* obj );

	// network assistants for transmitting projectile information
	virtual TBinObject* ProjectileNameToBin( size_t graphicName ) const;
	virtual size_t		ProjectileBinToName( TBinObject* graphic ) const;

	// reload menu or game objects - depends on status
	virtual void	ReloadAsRequired( void );

	virtual void	ResetGameObjects( void );

	virtual byte	Map( void ) const;

	// get id for this game in game (single/multi-player id)
	virtual size_t	MyId( void ) const;

	// game type { 0==everyone for themselves, 1 = team play }
	virtual byte	GameType( void ) const;

#ifdef _USEGUI
	// given a map and stories - load a def into *def
	virtual bool	GetStoryDef( size_t map, TString& errStr, TDefFile& def );
	// what map are we using?
	virtual TMapPlayerInfo	LandscapeMapDef( void );
#endif

    virtual void	Destroy( void );

	virtual void 	KeyDown( size_t );
	virtual void 	KeyUp( size_t );
	virtual void 	KeyPress( size_t );
	virtual void 	MouseDown( int, int, int );
	virtual void 	MouseDown( int, int, int, int, int );
	virtual void 	MouseUp( int, int, int );
	virtual void 	MouseUp( int, int, int, int, int );
	virtual void 	MouseMove( int, int );
	virtual void	MouseMove( int, int, int, int );
	virtual void	MouseMove( HWND hwnd, int x, int y, int dx, int dy );
	virtual void 	MouseUp( HWND hwnd, int button, int x, int y, int dx, int dy );
	virtual void	MouseDown( HWND hwnd, int button, int x, int y, int dx, int dy );
	virtual void	DoMenu( int );

    virtual void 	Render(void); // overwrite & put your openGL drawing code in here
	virtual void 	Draw(void); // do not overwrite unless you're know what your doing
    							// draw calls render if you need to overwrite something
								// overwrite render instead

	virtual void	Logic(void); // process key logic

	virtual void 	SetupFog( bool _fog ); // setup fog

	virtual bool	Modified( void ) const;
	virtual void	Modified( bool _modified );

	float			Width( void ) const;
	float			Height( void ) const;

	bool			ShowMenu( void ) const; // is any menu showing
	void			ShowMenu( bool _showMenu );

	bool			MainMenu( void ) const; // main menu showing?

	bool			ShowStory( void ) const;
	void			ShowStory( bool );

    // Get/set main window handle
    HWND	Window( void ) const;
    void	Window( HWND );

	// expired
	void	Expired( bool _expired );
	bool	Expired( void ) const;

	// force end of program?
	bool	Finished( void ) const;
	void	Finished( bool _finished );

	size_t PolygonsPerSecond( float fps ) const;

	float	Depth( void ) const;
	size_t	BitDepth( void ) const;

	bool	FullScreen( void ) const;
	void	FullScreen( bool f );

	bool	Minimised( void ) const;
	void	Minimised( bool );

	bool	LimitFPS( void ) const;
	void	LimitFPS( bool );

	HDC		HDc( void ) const;
	void	HDc( HDC );

	HGLRC	HGLRc( void ) const;
	void	HGLRc( HGLRC );

	const TString& ApplicationName( void ) const;
	const TString& ApplicationTitle( void ) const;
	const TString& VersionString( void ) const;

	// set rego string
	void	RegisteredMessage( const TString& _registeredMsg );

	size_t LogicFPS( void ) const;

	size_t	RenderDepth( void ) const;
	void	RenderDepth( size_t );

	size_t	MinRenderDepth( void ) const;
	void	MinRenderDepth( size_t );

	size_t	MaxRenderDepth( void ) const;
	void	MaxRenderDepth( size_t );

	void	SetMessage( const TString& msg );
	void	WriteMessage( void );

	void	SetCountDown( const TString& msg );
	void	WriteCountDown( void );

	void	Width( float w );
	void	Height( float h );

	HINSTANCE Instance( void ) const;

	Font&	Arial( void );

	void	Write( float x, float y, float z, const TString& msg );
	void	Write( float x, float y, float z, const char* msg );

	void	WriteReverse( float x, float y, float z, const TString& msg );
	void	WriteReverse( float x, float y, float z, const char* msg );

	size_t	TextWidth( const TString& text );

	virtual void SetCmdLine( const char* );

	bool	CtrlDown( void ) const;
	void	CtrlDown( bool );

	bool	ShiftDown( void ) const;
	void	ShiftDown( bool );

	bool	Active( void ) const;
	void	Active( bool );

	size_t	ShadowLevel( void ) const;
	void	ShadowLevel( size_t _shadowLevel );

	HWND	Dialog1( void );
	void	Dialog1( HWND );

	HWND	Dialog2( void );
	void	Dialog2( HWND );

	void	AntiAlias( bool _antiAlias );
	bool	AntiAlias( void ) const;

	byte	EffectsVolume( void ) const;
	byte	MusicVolume( void ) const;

	// select a different language
	size_t	LanguageId( void ) const;
	void	LanguageId( size_t _languageId );

	// software acceleration?
	bool IsSoftware( void ) const;

	// first time run?  (checked through "Language" registry entry only)
	bool NewInstallation( void );

	TString GameVersionString( void ) const;

    bool	Setup( HWND _hwnd, float depth, int widht, int height, TString& errStr );

	// add smoke
	virtual void AddSmoke( float x, float y, float z, 
						   float dx, float dy, float dz );

	// get all .mdl files for reference
	virtual bool GetModels( size_t& numModels, TCharacter* models, 
							TString* modelNames, TString& errStr );

	virtual TString GetCharacterFilename( size_t characterId );

	// return # maps & maps .def files (if maps!=NULL)
	virtual void GetMultiPlayerMaps( TDefFile* maps, size_t& numMaps );

	// access tips
	TTips&	Tips( void );

	virtual void SaveDefn( void );

private:
	void 	InitGL( float depth, int w, int h );
	void 	SetupLight( bool );
	int		SetDCPixelFormat( HDC hDC, int bitDepth, int zDepth, TString& errStr );
	bool	SwitchFullScreen( HWND hWnd, TString& errStr );

	bool	isExtensionSupported( const char* extensions, const char *extension );
	int		ChoosePixelFormatEx( HDC hdc );

	// go through all resolutions and mark them
	bool	EnumResolutions( void );

	// menu/screen setup
	void RenderMenu( bool showAllPages = true );
	void AddItem( TGUI*& page, TGUI* item );
	void CenterMenu( void );

	virtual void SetupMenus( void );

	void ClearResolutions( void );

private:
	friend class TApp;
	friend class TGUI;

	// resolution change variables
	size_t	resolutionCounter;
	DEVMODE	modes[kMaxResolutions];

	// menu pages - up to a max of 3 deep
	TGUI*			currentPage;
	TGUI*			prevPage1;
	TGUI*			prevPage2;

	float			mainMenuDepth;

	bool			showMenu;
	bool			showStory;
	bool			antiAlias;
	bool			newInstallation;
	size_t			shadowLevel;			// use shadows? (ie. has stencilbuffer?)

	TString			commandLine;
	TString			gameVersionString;
	TString			registeredMsg;
	Font*			arial;

	bool			ctrlDown;
	bool			shiftDown;

    HWND			hWindow;		// window Handle
	HWND			dialog1;		// windows first dialog
	HWND			dialog2;		// window second dialog
	bool			fog;			// fog y/n
    bool			initialised;	// system properly initialised

	float			vdepth;			// viewport dimensions
	float			vwidth;			// 0..width, 0..height
	float			vheight;		// 1..depth (z axis)
	size_t			vfreq;			// display frequency
	size_t			vbitdepth;		// bit depth of viewport
	size_t			vzdepth;			// z depth of viewport

	bool			finished;		// end program?
	float			lightingSetting;	// lighting 1=midday, 0=midnight

	size_t			renderDepth;		// render depth var
	bool			limitFPS;

	bool			fullScreen;
	bool			minimised;
	bool			isSoftware;
	bool			expired;
	bool			active;

	bool			playMusic;			// play music & sound effects?
	bool			playEffects;
	size_t			effectsVolume;
	size_t			musicVolume;
	size_t			languageId;

	TString			versionString;
	TString			appName;
	TString			appTitle;
	TString			msgStr;
	TString			cdStr;

	size_t			kMinRenderDepth;
	size_t			kMaxRenderDepth;
	size_t			currentRenderDepth;

	HINSTANCE		hInstance;
	HDC				hdc;
	HGLRC			hglrc;

	// story system
#ifdef _USEGUI
	TStories		stories;			// story system
#endif

	TSmoke			smoke;				// smoke generator

	byte			map;				// game map #

	TTips			tips;				// tip system

	size_t			myId;				// local machine's game play id

	byte			gameType;			// type of game we're playing

	bool			prepareClientServer; // true when setting up client/server

	#ifdef _DXSOUND_
    TSound*		soundSystem;
	#endif

#ifdef _USEGUI
	// main menu stuff
	TGUI*	mainMenu;
	TGUI*	videoSettingsButton;
	TGUI*	singlePlayerButton;
	TGUI*	hostButton;
	TGUI*	joinButton;
	TGUI*	controlsButton;
	TGUI*	aboutButton;
	TGUI*	downloadButton;
	TGUI*	exitButton;
	TGUI*	helpButton;
#endif
};

//==========================================================================

#endif


