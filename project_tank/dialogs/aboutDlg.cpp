#include <precomp_header.h>

#include <windows.h>
#include <direct.h>
#include <process.h>
#include <shellapi.h>

#include <dialogs/dialogAssist.h>
#include <jpeglib/tjpeg.h>

#include <SetupWin32/resource.h>

HWND hAboutDlg = NULL;
TJpeg* aboutJpeg = NULL;

HWND url1 = NULL;
HWND url2 = NULL;
HFONT hFont = NULL;

//==========================================================================
//
// Window callback procedure
//
LRESULT CALLBACK AboutDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

//
// Create
//
void CreateAboutDlg( HINSTANCE hInstance, HWND parent, TJpeg* _aboutJpeg )
{
	aboutJpeg = _aboutJpeg;
	hAboutDlg = CreateDialog( hInstance, MAKEINTRESOURCE(IDD_ABOUT), 
							  parent, (DLGPROC)AboutDlgProc);
	ShowWindow(hAboutDlg, SW_SHOW);
}

//==========================================================================

LRESULT CALLBACK AboutDlgProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch (message)
	{
		
		case WM_INITDIALOG:
			{
				hFont = CreateFont( -11,0,0,0,FW_DONTCARE,0,1,0,0,0,0,
									ANTIALIASED_QUALITY, FF_DONTCARE,
									"Tahoma" );

				url1 = GetDlgItem( hwnd, IDC_URL1 );
				url2 = GetDlgItem( hwnd, IDC_URL2 );

				SetControlFont( url1, hFont, true );
				SetControlFont( url2, hFont, true );
				return 0;
			}

		case WM_DESTROY: 
			{
				PostQuitMessage(0); 
				return 0;
			}

		case WM_COMMAND:
			{
				if ( HIWORD(wParam)==BN_CLICKED )
				{
					if ( LOWORD(wParam)==IDC_URL1 )
					{
						ShellExecute( hwnd, "open", "http://www.peter.co.nz/",
									  "", "", SW_SHOW );
						break;
					}

					if ( LOWORD(wParam)==IDC_URL2 )
					{
						ShellExecute( hwnd, "open", "http://home.wanadoo.nl/scarab/stills.htm",
									  "", "", SW_SHOW );
						break;
					}

					if ( LOWORD(wParam)==IDOK )
					{
						EndDialog(hwnd, 0);
						return 0;
					}
				}
				break;
			}

		case WM_CLOSE:
			{
				::EndDialog(hwnd,0);
			}
			break;

		case WM_SYSCOMMAND:
			{
				if (wParam==IDCANCEL)
				{
					EndDialog(hwnd, 0);
					return 0;
				}
			}
			break;

		case WM_SYSKEYUP:
			{
				if ( wParam=='C' || wParam=='c' )
				{
				}
			}
			break;

		case WM_ERASEBKGND:
//		case WM_PAINT:
			{
				if ( aboutJpeg!=NULL )
				{
					RECT myRect;
					RECT rect;
					PAINTSTRUCT ps;

					HDC hdc = ::BeginPaint(hwnd,&ps);

					GetClientRect( hwnd, &myRect );

					rect.left   = (myRect.right-myRect.left)/2-aboutJpeg->Width()/2;
					rect.top    = (myRect.bottom-myRect.top)/2-aboutJpeg->Height()/2;
					rect.right  = rect.left + aboutJpeg->Width();
					rect.bottom = rect.top + aboutJpeg->Height();

//					rect.left   = myRect.left;
					rect.top    = myRect.top;
//					rect.right  = rect.left + aboutJpeg->Width();
					rect.bottom = rect.top + aboutJpeg->Height();

					aboutJpeg->Draw(hdc,rect);
//					::ValidateRect( hWnd, &rect );
					::EndPaint(hwnd,&ps);
				}
//				break;
				return 1;
			}

		default:
			{
				return 0;
			}
	}
	return 0;
}

