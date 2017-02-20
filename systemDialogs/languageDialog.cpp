#include <precomp_header.h>

#include <win32/events.h>

#include <systemDialogs/dialogAssist.h>
#include <systemDialogs/languageDialog.h>

#include <win32/resource.h>

//==========================================================================
//
// Window callback procedure
//
LRESULT CALLBACK LanguageDialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LanguageDialog* LanguageDialog::ptr = NULL;

//==========================================================================

LanguageDialog::LanguageDialog( HINSTANCE _hInstance )
	: hInstance( _hInstance )
{
	LanguageDialog::ptr = this;
	languageId = 0;
};


LanguageDialog::~LanguageDialog( void )
{
	LanguageDialog::ptr = NULL;
};


bool LanguageDialog::Execute( void )
{
	bmpEnglish = ::LoadBitmap( hInstance, MAKEINTRESOURCE( IDB_ENGLISH ) );
	bmpGerman  = ::LoadBitmap( hInstance, MAKEINTRESOURCE( IDB_GERMAN ) );
	bmpDutch = ::LoadBitmap( hInstance, MAKEINTRESOURCE( IDB_DUTCH ) );

	bool success = false;
	if ( ::DialogBox( hInstance, MAKEINTRESOURCE(IDD_LANGUAGESELECTION), 
					  NULL, (DLGPROC)LanguageDialogProc )==IDOK )
	{
		// set new language id in registry
		SetRegistryKey( "Software\\PDV\\Performance", "Language", languageId );
		success = true;
	}

	if ( bmpEnglish!=NULL )
		::DeleteObject( bmpEnglish );
	if ( bmpGerman!=NULL )
		::DeleteObject( bmpGerman );
	if ( bmpDutch!=NULL )
		::DeleteObject( bmpDutch );

	return success;
}

size_t LanguageDialog::LanguageId( void ) const
{
	return languageId;
};

//==========================================================================

LRESULT CALLBACK LanguageDialogProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch (message)
	{
		
		case WM_INITDIALOG:
			{
				LanguageDialog* _ptr = LanguageDialog::ptr;

				_ptr->bEnglish = GetDlgItem( hwnd, IDC_ENGLISH );
				_ptr->bGerman  = GetDlgItem( hwnd, IDC_GERMAN );
				_ptr->bDutch  = GetDlgItem( hwnd, IDC_DUTCH );

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

		case WM_CTLCOLORBTN:
			{
				HDC buttonDC = HDC(wParam);
				HWND hButton = HWND(lParam);

				LanguageDialog* _ptr = LanguageDialog::ptr;
				if ( _ptr->bEnglish==hButton )
				if ( _ptr->bmpEnglish!=NULL )
				{
					HDC bitmapDC = ::CreateCompatibleDC( buttonDC );
					::SelectObject( bitmapDC, _ptr->bmpEnglish );

					RECT r;
					POINT p;
					::GetWindowRect( hButton, &r );
					p.x = (r.right-r.left) / 2 - 64;
					p.y = (r.bottom-r.top) / 2 - 64;
					::BitBlt( buttonDC, p.x,p.y, 128,128, bitmapDC,0,0, SRCCOPY );
					::DeleteDC( bitmapDC );
				}

				if ( _ptr->bGerman==hButton )
				if ( _ptr->bmpGerman!=NULL )
				{
					HDC bitmapDC = ::CreateCompatibleDC( buttonDC );
					::SelectObject( bitmapDC, _ptr->bmpGerman );

					RECT r;
					POINT p;
					::GetWindowRect( hButton, &r );
					p.x = (r.right-r.left) / 2 - 64;
					p.y = (r.bottom-r.top) / 2 - 64;
					::BitBlt( buttonDC, p.x,p.y, 128,128, bitmapDC,0,0, SRCCOPY );
					::DeleteDC( bitmapDC );
				}

				if ( _ptr->bDutch==hButton )
				if ( _ptr->bmpDutch!=NULL )
				{
					HDC bitmapDC = ::CreateCompatibleDC( buttonDC );
					::SelectObject( bitmapDC, _ptr->bmpDutch );

					RECT r;
					POINT p;
					::GetWindowRect( hButton, &r );
					p.x = (r.right-r.left) / 2 - 64;
					p.y = (r.bottom-r.top) / 2 - 64;
					::BitBlt( buttonDC, p.x,p.y, 128,128, bitmapDC,0,0, SRCCOPY );
					::DeleteDC( bitmapDC );
				}
				break;
			}

		case WM_PAINT:
			{
				break;
			};

		case WM_COMMAND:
			{
				if ( HIWORD(wParam)==BN_CLICKED )
				{
					if ( LOWORD(wParam)==IDC_ENGLISH )
					{
						LanguageDialog* _ptr = LanguageDialog::ptr;
						_ptr->languageId = 0;
						::EndDialog(hwnd, IDOK);
						return 0;
					}
					if ( LOWORD(wParam)==IDC_GERMAN )
					{
						LanguageDialog* _ptr = LanguageDialog::ptr;
						_ptr->languageId = 1;
						::EndDialog(hwnd, IDOK);
						return 0;
					}
					if ( LOWORD(wParam)==IDC_DUTCH )
					{
						LanguageDialog* _ptr = LanguageDialog::ptr;
						_ptr->languageId = 2;
						::EndDialog(hwnd, IDOK);
						return 0;
					}
				}
				break;
			}
	}
	return 0;
}

//==========================================================================

