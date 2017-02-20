#include <precomp_header.h>

#include <windows.h>
#include <shellapi.h>

#include <tank/tankapp.h>

#include <systemDialogs/dialogAssist.h>
#include <systemDialogs/tipsDlg.h>

#include <win32/resource.h>

//==========================================================================

TipsDialog::TipsDialog( TApp* _app )
	: app( _app ),
	  tipPage( NULL )
{
	PreCond( app!=NULL );

	showTips = true;
	languageId = 0;
	currentIndex = 0;
	tipDialogDepth = -78;
};

TipsDialog::~TipsDialog( void )
{
	if ( tipPage!=NULL )
		delete tipPage;
	tipPage = NULL;
};


bool TipsDialog::Execute( TTips& tips, bool forceShow )
{
	GetRegistrySettings();

	if ( tips.NumTips()==0 || !showTips || languageId!=0 )
	{
		if ( !forceShow )
			return false;
	}

	if ( tipPage!=NULL )
		delete tipPage;
	tipPage = NULL;

	AddItem( tipPage, new TGUI( app, guiWindow, 0, 0, 400, 150, tipDialogDepth, STR(strTips) ) );

	previousButton = new TGUI( app, guiButton, 16, 115, 35, 25, tipDialogDepth, "<<" );
	nextButton = new TGUI( app, guiButton, 56, 115, 35, 25, tipDialogDepth, ">>" );

	closeButton = new TGUI( app, guiButton, 300, 115, 80, 25, tipDialogDepth, STR(strClose) );
	closeButton->ButtonKey( VK_ESCAPE );

	l1 = new TGUI( app, guiLabel, 16, 20, 320, 16, tipDialogDepth, "", 255,255,255 );
	l2 = new TGUI( app, guiLabel, 16, 40, 320, 16, tipDialogDepth, "", 255,255,255 );
	l3 = new TGUI( app, guiLabel, 16, 60, 320, 16, tipDialogDepth, "", 255,255,255 );

	info = new TGUI( app, guiLabel, 110, 118, 40, 16, tipDialogDepth, "1/1", 255,255,255 );

	showTipsCheck = new TGUI( app, guiCheckBox, 150, 118, 140, 16, tipDialogDepth, STR(strShowTipsAtStartup), 255,255,255 );

	AddItem( tipPage, l1 );
	AddItem( tipPage, l2 );
	AddItem( tipPage, l3 );
	AddItem( tipPage, info );

	SetText( tips );

	AddItem( tipPage, previousButton );
	AddItem( tipPage, nextButton );
	AddItem( tipPage, showTipsCheck );
	AddItem( tipPage, closeButton );

	tipPage->SetLocation( 200, 130 );

	showTipsCheck->Checked( showTips );

	TGUI* prevPage = app->CurrentPage();
	app->CurrentPage( tipPage );

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
		if ( closeButton->Clicked() )
		{
			app->CurrentPage( prevPage );

			showTips = 	showTipsCheck->Checked();
			SetRegistryKey( "Software\\PDV\\Performance", "ShowTips", showTips?1:0 );

			return false;
		}
		if ( nextButton->Clicked() && currentIndex < (tips.NumTips()-1) )
		{
			currentIndex++;
			SetText( tips );
		}
		if ( previousButton->Clicked() && currentIndex > 0 )
		{
			currentIndex--;
			SetText( tips );
		}
	}

	app->CurrentPage( prevPage );
	return true;
}


void TipsDialog::SetText( TTips& tips )
{
	l1->Text( "" );
	l2->Text( "" );
	l3->Text( "" );

	size_t nl = tips.GetTipSize( currentIndex );
	if ( nl > 0 )
	{
		l1->Text( tips.GetTip( currentIndex, 0 ) );
	}
	if ( nl > 1 )
	{
		l2->Text( tips.GetTip( currentIndex, 1 ) );
	}
	if ( nl > 2 )
	{
		l3->Text( tips.GetTip( currentIndex, 2 ) );
	}

	TString str;
	str = Int2Str( currentIndex+1 ) + "/" + Int2Str(tips.NumTips());
	info->Text( str );
};

void TipsDialog::GetRegistrySettings( void )
{
	if ( !GetRegistryKey( "Software\\PDV\\Performance", "Language", languageId ) )
	{
		languageId = 0;
	}
	size_t temp = 1;
	if ( !GetRegistryKey( "Software\\PDV\\Performance", "ShowTips", temp ) )
	{
		showTips = true;
	}
	else
	{
		showTips = (temp==1);
	}
};

//==========================================================================

