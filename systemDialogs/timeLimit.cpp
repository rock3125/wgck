#include <precomp_header.h>

#include <win32/events.h>

#include <systemDialogs/dialogAssist.h>
#include <systemDialogs/timeLimit.h>

#include <win32/resource.h>

//==========================================================================
//
// Window callback procedure
//
LRESULT CALLBACK TimeLimitDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
TimeLimitDialog* TimeLimitDialog::ptr = NULL;

//==========================================================================

TimeLimitDialog::TimeLimitDialog( HINSTANCE _hInstance, const TString& _msg )
	: hInstance( _hInstance ),
	  msg( _msg )
{
	showMessage = 1;
	TimeLimitDialog::ptr = this;
};


TimeLimitDialog::~TimeLimitDialog( void )
{
	TimeLimitDialog::ptr = NULL;
};


void TimeLimitDialog::Execute( void )
{
	if ( !GetRegistryKey( "Software\\PDV\\Performance", "ShowTimeLimitMessage", showMessage ) )
		showMessage = 1;

	if ( showMessage==1 )
	if ( ::DialogBox( hInstance, 
                    MAKEINTRESOURCE(IDD_TIMELIMIT), 
                    NULL, (DLGPROC)TimeLimitDlgProc)==IDOK) 
	{
		SetRegistryKey( "Software\\PDV\\Performance", "ShowTimeLimitMessage", 
						showMessage );
	};
}

//==========================================================================

LRESULT CALLBACK TimeLimitDlgProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch (message)
	{
		
		case WM_INITDIALOG:
			{
				TimeLimitDialog* _ptr = TimeLimitDialog::ptr;

				_ptr->enableShowAgain = GetDlgItem( hwnd, IDC_SHOWAGAIN );
				_ptr->labelMessage = GetDlgItem( hwnd, IDC_TLMESSAGE );

				ButtonSetCheck( _ptr->enableShowAgain, _ptr->showMessage==1 );
				EditPrintSingle( _ptr->labelMessage, _ptr->msg.c_str() );

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
				if ( HIWORD(wParam)==BN_CLICKED )
				{
					if ( LOWORD(wParam)==IDOK )
					{
						TimeLimitDialog* _ptr = TimeLimitDialog::ptr;

						if ( ButtonGetCheck( _ptr->enableShowAgain ) )
							_ptr->showMessage = 1;
						else
							_ptr->showMessage = 0;
						EndDialog(hwnd, IDOK);
						return 0;
					}
					if ( LOWORD(wParam)==IDCANCEL )
					{
						EndDialog(hwnd, IDCANCEL);
						return 0;
					}
				}
				break;
			}

		case WM_SYSCOMMAND:
			{
				if (wParam==IDCANCEL)
				{
					EndDialog(hwnd, IDCANCEL);
					return 0;
				}
			}
			break;

		default:
			{
				return 0;
			}
	}
	return 0;
}

//==========================================================================

