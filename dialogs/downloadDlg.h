#ifndef _DIALOGS_DOWNLOAD_H__
#define _DIALOGS_DOWNLOAD_H__

//==========================================================================

#ifdef _USEGUI
#include <systemDialogs/gui.h>
#else
#include <jpeglib/tjpeg.h>
#endif

class TEvent;

//==========================================================================

class _EXPORT DownloadDialog
{
public:
#ifdef _USEGUI
	DownloadDialog( TEvent*, HINSTANCE, HWND );
#else
	DownloadDialog( HINSTANCE, HWND, TString regName );
#endif
	~DownloadDialog( void );

	bool Execute( void );

	TEvent*		app;
	HINSTANCE	hInstance;
	HWND		parent;
	bool		initialised;
	TString		regName;
	size_t		languageId;

#ifdef _USEGUI
	TGUI*		okButton;
	TGUI*		aboutPage;
	TGUI*		downloadButton;
	TGUI*		enterRegButton;
#else
	TJpeg		aboutJpeg;
	static DownloadDialog* ptr;
#endif
};

//==========================================================================

#endif
