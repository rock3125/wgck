#include <precomp_header.h>

#include <windows.h>
#include <direct.h>
#include <process.h>
#include <shellapi.h>

#include <win32/events.h>

#include <systemDialogs/dialogAssist.h>
#include <systemDialogs/aboutDlg.h>

#include <win32/resource.h>


//==========================================================================
AboutDialog::AboutDialog( TEvent* _app, HINSTANCE _hInstance, HWND _parent, TString _regName )
	: hInstance( _hInstance ),
	  parent( _parent ),
	  aboutPage( NULL ),
	  app( _app )
{
	regName = _regName;
	initialised = false;
	if ( !GetRegistryKey( "Software\\PDV\\Performance", "Language", languageId ) )
	{
		languageId = 0;
	}
};


AboutDialog::~AboutDialog( void )
{
	if ( aboutPage!=NULL )
		delete aboutPage;
	aboutPage = NULL;
};


void AboutDialog::Execute( void )
{
	if ( aboutPage!=NULL )
		delete aboutPage;
	aboutPage = NULL;

//	TString errStr;
//	TTexture* t = new TTexture();
//	PostCond( t->LoadBinary( "engineLogo.jpg", "data\\textures", errStr, false ) );

	// Customise vehicle dialog
//	AddItem( aboutPage, new TGUI( app, guiWindow, 0, 0, 286, 384+38, -3, "About" ) );
//	AddItem( aboutPage, new TGUI( app, guiBitmap,  10,18, 266, 384+18, -3, t, 0,0,1,1 ) );
	AddItem( aboutPage, new TGUI( app, guiLabel, app->Width() - 150, 30, 100, 14, -2.8f, regName, 204,204,204 ) );
	AddItem( aboutPage, new TGUI( app, guiLabel, 10, app->Height() - 20, 100, 14, -2.8f, app->GameVersionString(), 204,204,204 ) );

//	AddItem( aboutPage, new TGUI( app, guiLabel, 20, 370, 100, 14, -2.8f, "written by Peter de Vocht", 204,204,204 ) );
//	AddItem( aboutPage, new TGUI( app, guiLabel, 20, 385, 100, 14, -2.8f, "graphics Rob Takken", 204,204,204 ) );

	okButton = new TGUI( app, guiButton, (app->Width()*0.5f)-25,
						 app->Height() - 40, 50,25, -3, STR(strOK) );
	AddItem( aboutPage, okButton );
	okButton->ButtonKey( VK_ESCAPE );
	
//	aboutPage->SetLocation( 120, 50 );

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
			closing = true;
		}
		if ( msg.message==WM_QUIT || msg.message==WM_CLOSE )
		{
			closing = true;
			app->CurrentPage( prevPage );
			::PostQuitMessage( 0 );
		}
	}
}

//==========================================================================

