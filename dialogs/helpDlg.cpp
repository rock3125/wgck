#include <precomp_header.h>

#include <windows.h>
#include <shellapi.h>

#include <tank/tankapp.h>

#include <systemDialogs/dialogAssist.h>
#include <systemDialogs/tipsDlg.h>
#include <dialogs/helpDlg.h>

#include <win32/resource.h>

//==========================================================================

HelpDialog::HelpDialog( TApp* _app )
	: app( _app ),
	  helpPage( NULL )
{
	PreCond( app!=NULL );

	languageId = 0;
	helpDialogDepth = -88;
};

HelpDialog::~HelpDialog( void )
{
	if ( helpPage!=NULL )
		delete helpPage;
	helpPage = NULL;
};


bool HelpDialog::Execute( void )
{
	if ( helpPage!=NULL )
		delete helpPage;
	helpPage = NULL;

	GetRegistrySettings();

	AddItem( helpPage, new TGUI( app, guiWindow, 0, 0, 165, 160, helpDialogDepth, STR(strHelp) ) );
	manualButton = new TGUI( app, guiButton, 12,20,130,25, helpDialogDepth, STR(strManual), 255,255,255 );
	tutorialButton = new TGUI( app, guiButton, 12,50,130,25, helpDialogDepth, STR(strShowTips), 255,255,255 );
	onlineButton = new TGUI( app, guiButton, 12,80,130,25, helpDialogDepth, STR(strEmailHelp), 255,255,255 );
	cancelButton = new TGUI( app, guiButton, 12,120,130,25, helpDialogDepth, STR(strCancel) );


	AddItem( helpPage, manualButton );
	if ( tutorialButton!=NULL )
	{
		AddItem( helpPage, tutorialButton );
	}
	AddItem( helpPage, onlineButton );
	AddItem( helpPage, cancelButton );

	cancelButton->ButtonKey( VK_ESCAPE );

	helpPage->SetLocation( 200, 130 );

	TGUI* prevPage = app->CurrentPage();
	app->CurrentPage( helpPage );

	MSG msg;
	size_t fpsTimer = ::GetTickCount();
	closing = false;
	while ( !closing )
	{
		size_t time = ::GetTickCount();
		::ProcessMessages( app, msg, fpsTimer );
		if ( msg.message==WM_QUIT || msg.message==WM_CLOSE )
		{
			closing = true;
			app->CurrentPage( prevPage );
			::PostQuitMessage( 0 );
		}
		if ( cancelButton->Clicked() )
		{
			app->CurrentPage( prevPage );
			return false;
		}
		if ( onlineButton->Clicked() )
		{
			app->CurrentPage( prevPage );
			if ( app->ForceWindowed() )
			{
				app->ReloadAsRequired();
			}
			const char* url = "mailto:peter@peter.co.nz?subject=War Games Inc Query&body=Hi Peter,";
			::ShellExecute( app->Window(), "open", url, "", "", SW_SHOW );
			return true;
		}
		if ( manualButton->Clicked() )
		{
			app->CurrentPage( prevPage );
			if ( app->ForceWindowed() )
			{
				app->ReloadAsRequired();
			}
			const char* url = "wargamesinc.pdf";
			::ShellExecute( app->Window(), "open", url, "", "", SW_SHOW );
			return true;
		}
		if ( tutorialButton!=NULL )
		if ( tutorialButton->Clicked() )
		{
			TipsDialog dlg(app);
			dlg.Execute(app->Tips(),true);
			app->CurrentPage( prevPage );
			return true;
		}
	}
	app->CurrentPage( prevPage );
	return true;
}


void HelpDialog::GetRegistrySettings( void )
{
	if ( !GetRegistryKey( "Software\\PDV\\Performance", "Language", languageId ) )
	{
		languageId = 0;
	}
};

//==========================================================================

