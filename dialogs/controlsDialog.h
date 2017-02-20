#ifndef _DIALOGS_CONTROLS_H__
#define _DIALOGS_CONTROLS_H__

//==========================================================================

#include <systemDialogs/gui.h>

class TEvent;

//==========================================================================

class _EXPORT UserKeys
{
public:
	UserKeys( void );
	UserKeys( const UserKeys& );
	~UserKeys( void );

	const UserKeys& operator=( const UserKeys& );

	bool Defined( size_t key );

	void LoadFromRegistry( void );
	void SaveToRegistry( void );

	size_t	left;
	size_t	right;
	size_t	up;
	size_t	down;
	size_t	jump;
	size_t	map;
	size_t	destruct;
	size_t	scores;
	size_t	leftMouse;
	size_t	rightMouse;
	bool	reverseMouse;
};

//==========================================================================

class _EXPORT ControlsDialog
{
public:
	ControlsDialog( TEvent*, HINSTANCE, HWND );
	~ControlsDialog( void );

	void Execute( UserKeys& keys );

	TEvent*		app;
	HINSTANCE	hInstance;
	HWND		parent;
	bool		initialised;
	size_t		languageId;

#ifdef _USEGUI
	TGUI*	controlsPage;
	TGUI*	okButton;

	TGUI*	editLeft;
	TGUI*	editRight;
	TGUI*	editUp;
	TGUI*	editDown;
	TGUI*	editJump;
	TGUI*	editLeftMouse;
	TGUI*	editRightMouse;
	TGUI*	editScores;
	TGUI*	editMap;
	TGUI*	reverseMouse;

	TGUI*	buttonCustomise1;
	TGUI*	buttonCustomise2;
	TGUI*	buttonCustomise3;
	TGUI*	buttonCustomise4;
	TGUI*	buttonCustomise5;
	TGUI*	buttonCustomise6;
	TGUI*	buttonCustomise7;
	TGUI*	buttonCustomise8;
	TGUI*	buttonCustomise11;

#else
	static ControlsDialog* ptr;
#endif
};

//==========================================================================

#endif
