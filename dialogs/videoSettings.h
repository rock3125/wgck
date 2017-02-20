#ifndef _DIALOGS_VIDEOSETTINGS_H__
#define _DIALOGS_VIDEOSETTINGS_H__

//==========================================================================

#include <systemDialogs/gui.h>
#include <common/sounds.h>

#define kMaxResolutions 20

class TEvent;

//==========================================================================

class _EXPORT VideoSettingsDialog
{
public:
#ifdef _USEGUI
	VideoSettingsDialog( TEvent*, HINSTANCE, HWND, TSound* );
#else
	VideoSettingsDialog( HINSTANCE, HWND );
#endif
	~VideoSettingsDialog( void );

	int Execute( void );

	// have the settings changed in such a way that items need
	// to be reloaded (reset res etc.)
	bool NeedsReload( void ) const;
	bool LanguageChanged( size_t& newLanguageId ) const;

private:
	void UpdateButtons( void );
	void GetRegistrySettings( void );

private:
	HINSTANCE	hInstance;
	HWND		parent;
	bool		initialised;

	// any of these change - reset res & reload objects
	size_t		currentRes;
	size_t		languageId;
	size_t		prevRes; 
	size_t		currentFullScreen;
	size_t		prevFullScreen;
//	size_t		limitFps;
	size_t		shadowLevel;
	size_t		prevLimitFps;
	size_t		prevLanguage;

	size_t		antiAlias;
	size_t		playMusicSetting;
	size_t		playSoundSetting;
	size_t		effectsVolume;
	size_t		musicVolume;
	size_t		renderDepth;

	TSound*		soundSystem;

	size_t	resolutionCounter;
	int		rwidth[kMaxResolutions];
	int		rheight[kMaxResolutions];
	int		rbitDepth[kMaxResolutions];
	int		rzDepth[kMaxResolutions];

#ifdef _USEGUI
	TEvent*	app;

	TGUI*	videoSettingsPage;
	TGUI*	okButton;
	TGUI*	cancelButton;
	TGUI*	resolutionDropDown;
	TGUI*	languageDropDown;
	TGUI*	runFullscreenCheckBox;
//	TGUI*	limitFpsCheckBox;
	TGUI*	shadowDropDown;
	TGUI*	playMusic;
	TGUI*	playSounds;
	TGUI*	musicVolumeSlider;
	TGUI*	soundVolumeSlider;
	TGUI*	renderDepthSlider;
	TGUI*	antiAliasCheckBox;

#else
	friend LRESULT CALLBACK VideoDlgProc( HWND, UINT, WPARAM, LPARAM );
	friend HRESULT WINAPI EnumModesCallback2( LPDDSURFACEDESC2, LPVOID );

	static VideoSettingsDialog* ptr;

	HWND	hRes;
	HWND	hFullScreen;
//	HWND	hLimitFPS;
#endif
};

//==========================================================================

#endif
