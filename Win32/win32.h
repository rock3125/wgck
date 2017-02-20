#ifndef __MYWIN32_H__
#define __MYWIN32_H__

//==========================================================================

void	_EXPORT PreInit( HINSTANCE hInst );	// (1) call first
void	_EXPORT SetApp( TEvent* _app );		// (1) create TApp & set app
bool	_EXPORT PostInit( void );			// (2) then call this
HRESULT _EXPORT StartGameSystem( void );	// (3) enter main loop till quit
void	_EXPORT ReleaseResources( void );	// (4) exit

TString	_EXPORT	RegisteredMessage( void );

//==========================================================================

enum vehicleType { 
					vtInvalid = 0,

					vtNone, // character

					vtTank,
					vtPlane,
					vtFlak,
					vtArtillery,
					vtMG42,
					vtV2,

					vtBuilding
				};

//==========================================================================

enum sounds		{ 
					sengine1 = 0, 
					sengine2, 
					scanon, 
					sgun, 
					swalk, 
					smachinegun, 
					spropellor,
					sjetengine, 
					sexplode, 
					smg42, 
					sdaytime, 
					snighttime, 
					sfog 
				};

//==========================================================================

enum actions	{ 
					aUp=1,				// up down left right movement
					aDown=2, 
					aLeft=4, 
					aRight=8, 
					aFire=16,			// fire
					aExplode=32,			// self destruct
					aJump=64,			// enter/leave vehicle & jump
				};

//==========================================================================

#define kLogicFPS		15

#define kMaxPlayers		16
#define kMaxTeams		4
#define kMaxModels		16
#define kMaxWeapons		8
#define kMaxTanks		16
#define kMaxLandscapes	128
#define kMaxObjects		256
#define kMaxGameObjects 256
#define kMaxTracks		64
#define kMaxDifferentVehicles	25
#define kNumExplosionAnimations	7

// every AI gets a turn every... kAIFrequency
#define kAIFrequency	2

//==========================================================================
//
//	function fwd. declarations
//

LRESULT CALLBACK	WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
bool				_EXPORT SetupWindow( TString& errStr );
void				_EXPORT SetMessage( const TString& msg );
void				_EXPORT SetMenuItemTick( UINT menuItem );
void				_EXPORT ClearMenuItemTick( UINT menuItem );
bool				_EXPORT GetRegistrySettings( void );
bool				_EXPORT CheckRego( void );
void				_EXPORT ResetScreenToStartupMode( void ); // reset to initial mode

// win32 inits at several levels and release of resources
void				_EXPORT PreInit( HINSTANCE hInst );
bool				_EXPORT PostInit( void );
void				_EXPORT ReleaseResources( void );

// msg system
void AddMessage( const char* msg );
void AddMessage( TString message );
void _EXPORT ProcessDisplayMessages( void );
void _EXPORT DisplayMessages( void );
void _EXPORT ReleaseMouse( void );

//
//	event system
//
bool	_EXPORT DoCreate( HWND hWnd );
void	_EXPORT DoExit( HWND hWnd );
void	_EXPORT DoSize( HWND hWnd, WPARAM wParam );
void	_EXPORT DoActivate( HWND hWnd, WPARAM wParam );
void	_EXPORT DoPaint( HWND hWnd );
void	_EXPORT DoKeyDown( HWND hWnd, WPARAM wParam );
void	_EXPORT DoKeyUp( HWND hWnd, WPARAM wParam );
void	_EXPORT DoSysKeyUp( HWND hWnd, WPARAM wParam );
void	_EXPORT DoKeyPress( HWND hWnd, WPARAM wParam );
void	_EXPORT DoDestroy( HWND hWnd );
void	_EXPORT DoTimer( HWND hWnd, WPARAM wParam );
void	_EXPORT DoMenu( HWND hWnd, WPARAM wParam );
void	_EXPORT DoMouseMove( HWND hWnd, WPARAM wParam, LPARAM lParam );
void	_EXPORT DoMouseButtonDown( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
void	_EXPORT DoMouseButtonUp( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

//==========================================================================

class _EXPORT TMessage
{
public:
	TMessage( void );
	~TMessage( void );

	void AddMessage( const TString& msg );
	void AddMessage( const char* msg );
	void ProcessDisplayMessages( void );
	void DisplayMessages( void ) const;

private:
	TString			msg[5];
	size_t			maxMsg;
	size_t			msgIndex;
	size_t			msgTimer;
};

//==========================================================================

extern HWND mainWindow;
extern TString _dir;
extern DEVMODE initialScreenMode; // initial screen mode
extern TString registeredMsg;

// show progress bar [0..100] only on starup screen
void _EXPORT SetProgress( size_t _progress );
void _EXPORT SetProgressMessage( const TString& msg );

#ifdef _MESA
extern GMDC* gm;
#endif

//==========================================================================

#endif


