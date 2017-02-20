#ifndef _DIALOGS_SinglePlayerDialog_H_
#define _DIALOGS_SinglePlayerDialog_H_

//==========================================================================

#ifdef _USEGUI
#include <object/compoundobject.h>
#else
#include <jpeglib/tjpeg.h>
#endif

#include <dialogs/HostDialog.h>
#include <object/character.h>

class TGameParameters;
class TGUI;
class TApp;
class TStories;

//==========================================================================

class _EXPORT SinglePlayerDialog
{
public:
	SinglePlayerDialog( TApp*, HINSTANCE, HWND, 
						TStories* _stories );
	~SinglePlayerDialog( void );

	bool Execute( TGameParameters& hp );

	bool Initialised( void ) const;
	void Initialised( bool i );

private:
	void UpdateHostButtons( void );
	void GetRegistrySettings( void );
	void DrawVehicle( void );

private:
#ifndef _USEGUI
	friend LRESULT CALLBACK SinglePlayerDlgProc( HWND, UINT, WPARAM, LPARAM );
	static SinglePlayerDialog* ptr;
#endif

	TString hostPlayerName;
	int selectedCharacter;
	int selectedMap;
	int prevSelectedMap;
	size_t languageId;

	TApp*		app;
	HINSTANCE	hInstance;
	HWND		parent;
	HWND		hwnd; // me

	size_t			numModels;
	TCharacter		characters[kMaxModels];
	TString			modelNames[kMaxModels];

	TStories*		stories;

	bool closing;
	bool initialised;

	TGUI*	hostPage;
	TGUI*	nameEdit;
	TGUI*	characterDropDown;
	TGUI*	mapDropDown;
	TGUI*	hostButton;
	TGUI*	cancelButton;
	TGUI*	charGraphic;

	float yAngle;
	float singlePlayerDialogDepth;
};

//==========================================================================

#endif
