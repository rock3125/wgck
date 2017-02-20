#include <precomp_header.h>

#include <dialogs/dialogAssist.h>
#include <expansionInstaller/WelcomeDlg.h>
#include <expansionInstaller/resource.h>

//==========================================================================
//
// Window callback procedure
//
LRESULT CALLBACK WelcomeDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
WelcomeDlg* WelcomeDlg::ptr = NULL;

#define _ptr WelcomeDlg::ptr

//==========================================================================

WelcomeDlg::WelcomeDlg( HINSTANCE _hInstance )
	: hInstance( _hInstance )
{
	WelcomeDlg::ptr = this;
};


WelcomeDlg::~WelcomeDlg( void )
{
	WelcomeDlg::ptr = NULL;
};


bool WelcomeDlg::Execute( void )
{
	bool success = false;
	if ( ::DialogBox( hInstance, MAKEINTRESOURCE(IDD_WELCOME), 
					  NULL, (DLGPROC)WelcomeDlgProc )==IDOK )
	{
		success = true;
	}
	return success;
}

//==========================================================================

LRESULT CALLBACK WelcomeDlgProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch (message)
	{
		
		case WM_INITDIALOG:
			{
				HWND hmsg = ::GetDlgItem( hwnd, IDC_MSG );
				TString msg;

				msg = "Welcome to the War Games Inc. package installer\n";
				msg = msg + "only registered users can install these packages\n\n";
				msg = msg + "If you are a registered user, make sure you have\n";
				msg = msg + "already installed War Games Inc. before installing\n";
				msg = msg + "this package.\n\n";
				msg = msg + "Click OK to continue";

				::SetWindowText( hmsg, msg.c_str() );

				RECT r1,r2;
				HWND desktop = ::GetDesktopWindow();
				::GetWindowRect( desktop, &r1 );
				::GetWindowRect( hwnd, &r2 );
				int left = ((r1.right - r1.left) / 2) - ((r2.right - r2.left) / 2);
				int top = ((r1.bottom - r1.top) / 2) - ((r2.bottom - r2.top) / 2);

				::SetWindowPos( hwnd, HWND_TOPMOST, left, top, 0,0, SWP_NOSIZE | SWP_SHOWWINDOW );

				return 0;
			}

		case WM_CLOSE:
			{
				::EndDialog(hwnd,IDCANCEL);
			}

		case WM_COMMAND:
			{
				DWORD test = IDOK;
				if ( LOWORD(wParam)==IDOK )
				{
					::EndDialog(hwnd,IDOK);
				}
				if ( LOWORD(wParam)==IDCANCEL )
				{
					::EndDialog(hwnd,IDCANCEL);
				}
				break;
			}
	}
	return 0;
}

//==========================================================================

