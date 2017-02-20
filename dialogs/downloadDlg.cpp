#include <precomp_header.h>

#include <windows.h>
#include <direct.h>
#include <process.h>
#include <shellapi.h>

#include <win32/events.h>

#include <systemDialogs/dialogAssist.h>
#include <systemDialogs/enterRego.h>
#include <dialogs/downloadDlg.h>

#include <win32/resource.h>


//==========================================================================
DownloadDialog::DownloadDialog( TEvent* _app, HINSTANCE _hInstance, HWND _parent )
	: hInstance( _hInstance ),
	  parent( _parent ),
	  aboutPage( NULL ),
	  app( _app )
{
	initialised = false;
	if ( !GetRegistryKey( "Software\\PDV\\Performance", "Language", languageId ) )
	{
		languageId = 0;
	}
};


DownloadDialog::~DownloadDialog( void )
{
	if ( aboutPage!=NULL )
		delete aboutPage;
	aboutPage = NULL;
};


bool DownloadDialog::Execute( void )
{
	if ( aboutPage!=NULL )
		delete aboutPage;
	aboutPage = NULL;

	float left = (app->Width()*0.35f);
	float top = (app->Height()*0.95f);

	okButton = new TGUI( app, guiButton, left - 25,
						 top - 40, 150, 25, -3, STR(strReturnToGame) );
	downloadButton = new TGUI( app, guiButton, left + 135,
								top - 40, 100, 25, -3, STR(strDownload) );
	enterRegButton = new TGUI( app, guiButton, left + 245,
								top - 40, 150, 25, -3, STR(strEnterRego) );
	AddItem( aboutPage, okButton );
	AddItem( aboutPage, downloadButton );
	AddItem( aboutPage, enterRegButton );
	okButton->ButtonKey( VK_ESCAPE );
	
	initialised = true;
	TGUI* prevPage = app->CurrentPage();
	app->CurrentPage( aboutPage );

	MSG msg;
	size_t fpsTimer = ::GetTickCount();
	bool closing = false;
	while ( !closing )
	{
		::ProcessMessages( app, msg, fpsTimer );
		if ( okButton->Clicked() )
		{
			app->CurrentPage( prevPage );
			return false;
		}
		if ( downloadButton->Clicked() )
		{
			app->CurrentPage( prevPage );
			return true;
		}
		if ( enterRegButton->Clicked() )
		{
			EnterRego dlg(app);
			dlg.Execute();
		}

		if ( msg.message==WM_QUIT || msg.message==WM_CLOSE )
		{
			closing = true;
			app->CurrentPage( prevPage );
			::PostQuitMessage( 0 );
		}
	}
	return false;
}

//==========================================================================

