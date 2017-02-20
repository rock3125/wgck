#ifndef __MYWIN32_H__
#define __MYWIN32_H__

//==========================================================================

class TEvent;

//==========================================================================
//
//	function fwd. declarations
//
LRESULT CALLBACK	WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
void				ParseCommandLine( const TString& lpCmdLine, TString& fname, 
									  TString& texturePath );
bool				SetupWindow( void );
void				SetMessage( const TString& msg );
void				SetMenuItemTick( UINT menuItem );
void				ClearMenuItemTick( UINT menuItem );
bool				GetRegistrySettings( void );

bool InitialiseStartup( const TString& cmdLine, const TString& texturePath,
					    HINSTANCE hInst, TEvent*& app, TString& appName, 
						TString& appTitle, TString& errStr );

//
//	event system
//
bool	DoCreate( HWND hWnd );
void	DoSize( HWND hWnd, WPARAM wParam );
void	DoActivate( HWND hWnd, WPARAM wParam );
void	DoPaint( HWND hWnd );
void	DoKeyDown( HWND hWnd, WPARAM wParam );
void	DoKeyUp( HWND hWnd, WPARAM wParam );
void	DoSysKeyUp( HWND hWnd, WPARAM wParam );
void	DoKeyPress( HWND hWnd, WPARAM wParam );
void	DoDestroy( HWND hWnd );
void	DoTimer( HWND hWnd, WPARAM wParam );
void	DoMenu( HWND hWnd, WPARAM wParam );
void	DoMouseMove( HWND hWnd, WPARAM wParam, LPARAM lParam );
void	DoMouseButtonDown( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
void	DoMouseButtonUp( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

//==========================================================================
//
//	Console feedback system
//

void ConsoleCreate( HWND parent );
void ConsoleHide( void );
void ConsoleShow( void );
void ConsoleClear( void );
void ConsolePrint( char *text, ... );

//==========================================================================

extern bool useMask;
extern bool	forceDepth16;
extern bool	useHint1;
extern bool	useHint2;
extern bool	useFlush;
extern bool	useShadeModel;
extern bool	useColourMaterial;
extern bool	useLighting;
extern bool useNormals;

extern size_t kMinRenderDepth;
extern size_t kMaxRenderDepth;
extern size_t renderDepth;

//==========================================================================

#endif


