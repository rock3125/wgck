#ifndef _DIALOGS_ABOUT_H__
#define _DIALOGS_ABOUT_H__

//==========================================================================

#ifdef _USEGUI
#include <systemDialogs/gui.h>
#else
#include <jpeglib/tjpeg.h>
#endif

class TEvent;

//==========================================================================

class _EXPORT AboutDialog
{
public:
#ifdef _USEGUI
	AboutDialog( TEvent*, HINSTANCE, HWND, TString regName );
#else
	AboutDialog( HINSTANCE, HWND, TString regName );
#endif
	~AboutDialog( void );

	void Execute( void );

	TEvent*		app;
	HINSTANCE	hInstance;
	HWND		parent;
	bool		initialised;
	TString		regName;
	size_t		languageId;

#ifdef _USEGUI
	TGUI*		aboutPage;
	TGUI*		okButton;
#else
	TJpeg		aboutJpeg;
	static AboutDialog* ptr;
#endif
};

//==========================================================================

#endif
