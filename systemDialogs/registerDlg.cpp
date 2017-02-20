#include <precomp_header.h>

#include <windows.h>
#include <shellapi.h>

#include <systemDialogs/dialogAssist.h>
#include <systemDialogs/registerDlg.h>
#include <systemDialogs/enterCodeDlg.h>

#include <win32/resource.h>

//==========================================================================
//
// Window callback procedure
//
LRESULT CALLBACK RegisterDialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
RegisterDialog* RegisterDialog::ptr = NULL;

//==========================================================================

RegisterDialog::RegisterDialog( HINSTANCE _hInstance, HWND _parent )
	: hInstance( _hInstance ),
	  parent( _parent )
{
	RegisterDialog::ptr = this;
	initialised = false;
};


RegisterDialog::~RegisterDialog( void )
{
	RegisterDialog::ptr = NULL;
};


void RegisterDialog::Execute( void )
{
	if ( DialogBox( hInstance, 
                    MAKEINTRESOURCE(IDD_REGISTER), 
                    parent, (DLGPROC)RegisterDialogProc)==IDOK) 
	{
	};
}

//==========================================================================

LRESULT CALLBACK RegisterDialogProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch (message)
	{
		
		case WM_INITDIALOG:
			{
				RegisterDialog::ptr->initialised = true;
				char errStr[256];
				if ( !RegisterDialog::ptr->registerJpeg.Load( RegisterDialog::ptr->hInstance,
														MAKEINTRESOURCE(IDR_REGISTER),
														errStr ) )
				{
				}
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
						EndDialog(hwnd, IDOK);
						return 0;
					}
					if ( LOWORD(wParam)==IDC_ENTERREGISTRATION )
					{
						EnterCodeDialog dlg( RegisterDialog::ptr->hInstance, hwnd );
						dlg.Execute();
						return 0;
					}
					if ( LOWORD(wParam)==IDC_PURCHASE )
					{
						const char* url = "https://www.regnow.com/softsell/nph-softsell.cgi?item=6083-1";
						ShellExecute( hwnd, "open", url, "", "", SW_SHOW );
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

		case WM_PAINT:
			{
				{
					RECT myRect;
					RECT rect;
					PAINTSTRUCT ps;

					TJpeg* registerJpeg = &RegisterDialog::ptr->registerJpeg;

					HDC hdc = ::BeginPaint(hwnd,&ps);

					GetClientRect( hwnd, &myRect );

					rect.left   = (myRect.right-myRect.left)/2-registerJpeg->Width()/2;
					rect.top    = (myRect.bottom-myRect.top)/2-registerJpeg->Height()/2;
					rect.right  = rect.left + registerJpeg->Width();
					rect.bottom = rect.top + registerJpeg->Height();

					rect.top    = myRect.top + 5;
					rect.bottom = rect.top + registerJpeg->Height() + 5;

					registerJpeg->Draw(hdc,rect);
					::EndPaint(hwnd,&ps);
				}
				return 0;
			}

		case WM_SYSKEYUP:
			{
				if ( wParam=='C' || wParam=='c' )
				{
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
