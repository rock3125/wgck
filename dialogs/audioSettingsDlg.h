#ifndef _DIALOGS_AUDIOSETTINGS_H__
#define _DIALOGS_AUDIOSETTINGS_H__

//==========================================================================

#ifdef _USEGUI
#include <systemDialogs/gui.h>
#endif

class TEvent;

//==========================================================================

class _EXPORT AudioSettingsDialog
{
public:
#ifdef _USEGUI
	AudioSettingsDialog( TEvent*, HINSTANCE, HWND );
#else
	AudioSettingsDialog( HINSTANCE, HWND );
#endif
	~AudioSettingsDialog( void );

	void Execute( void );

	TEvent*		app;
	HINSTANCE	hInstance;
	HWND		parent;
	bool		initialised;

	size_t		playMusic;
	size_t		playEffects;

#ifdef _USEGUI
	TGUI*		audioSettingsPage;
	TGUI*		enableMusicCheckBox;
	TGUI*		enableEffectsCheckBox;
	TGUI*		okButton;
	TGUI*		cancelButton;
#else
	HWND		enableMusicCheckBox;
	HWND		enableEffectsCheckBox;
	static AudioSettingsDialog* ptr;
#endif
};

//==========================================================================

#endif
