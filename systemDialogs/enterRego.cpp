#include <precomp_header.h>

#include <windows.h>

#include <systemDialogs/dialogAssist.h>
#include <systemDialogs/enterRego.h>
#include <common/compatability.h>

#include <tank/tankapp.h>

#include <win32/resource.h>

//==========================================================================
// the long night has come

EnterRego::EnterRego( TEvent* _app )
	: app( _app ),
	  regoPage( NULL )
{
	PreCond( app!=NULL );
	enterRegoDepth = -3;
	languageId = 0;
};


EnterRego::~EnterRego( void )
{
	if ( regoPage!=NULL )
		delete regoPage;
	regoPage = NULL;
};


bool EnterRego::Execute( void )
{
	if ( regoPage!=NULL )
		delete regoPage;
	regoPage = NULL;

	AddItem( regoPage, new TGUI( app, guiWindow, 0, 0, 453, 110, enterRegoDepth, STR(strRegistration) ) );
	AddItem( regoPage, new TGUI( app, guiLabel, 16,20,27,14, enterRegoDepth, STR(strName), 255,255,255 ) );
	AddItem( regoPage, new TGUI( app, guiLabel, 16,40,27,14, enterRegoDepth, STR(strCode), 255,255,255 ) );
	registerButton = new TGUI( app, guiButton, 335,70,50,25, enterRegoDepth, STR(strOK) );
	cancelButton = new TGUI( app, guiButton, 390,70,50,25, enterRegoDepth, STR(strCancel) );

	nameEdit = new TGUI( app, guiEdit, 60,20,131,14, enterRegoDepth );
	codeEdit = new TGUI( app, guiEdit, 60,40,331,14, enterRegoDepth );
	codeEdit->MaxText( 50 );

	registerButton->ButtonKey( 13 );
	cancelButton->ButtonKey( VK_ESCAPE );

	AddItem( regoPage, nameEdit );
	AddItem( regoPage, codeEdit );
	AddItem( regoPage, registerButton );
	AddItem( regoPage, cancelButton );

	regoPage->SetLocation( 50, 50 );

	TGUI* prevPage = app->CurrentPage();
	app->CurrentPage( regoPage );

	MSG msg;
	size_t fpsTimer = ::GetTickCount();
	size_t checkTimer = ::GetTickCount();
	bool closing = false;
	float rot = 0;
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
		if ( time >= checkTimer )
		{
			checkTimer = time + 20;
			CheckButtons();
		}

		if ( registerButton->Clicked() )
		{
			// check rego code & set it
			SetRegistryKey( "Software\\PDV\\Performance", "RegoName", nameEdit->Text() );
			SetRegistryKey( "Software\\PDV\\Performance", "RegoCode", codeEdit->Text() );
			if ( !CheckRego() )
			{
				TGUI* tempPage = app->CurrentPage();
				app->CurrentPage( prevPage );
				::Message( app, STR(strIncorrectCode), STR(strError), MB_OK );
				app->CurrentPage( tempPage );
			}
			else
			{
				TGUI* tempPage = app->CurrentPage();
				app->CurrentPage( prevPage );
				::Message( app, STR(strThanksForRegistering), STR(strThankYou), MB_OK );
				app->CurrentPage( tempPage );
				closing = true;
			}
		}
		if ( cancelButton->Clicked() )
		{
			app->CurrentPage( prevPage );
			return false;
		}
	}
	app->CurrentPage( prevPage );

	return true;
};


void EnterRego::CheckButtons( void )
{
	bool en = nameEdit->Text().length() > 0 && codeEdit->Text().length() > 0;
	if ( registerButton->Enabled()!=en )
	{
		registerButton->EnableControl( en );
	}
};

void EnterRego::GetRegistrySettings( void )
{
	if ( !GetRegistryKey( "Software\\PDV\\Performance", "Language", languageId ) )
	{
		languageId = 0;
	}
};

//==========================================================================

