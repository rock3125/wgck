#include <precomp_header.h>

#include <wingdi.h>

#include <CreateFont/fontDialog.h>
#include <CreateFont/resource.h>
#include <systemDialogs/dialogAssist.h>

//==========================================================================
//
// Window callback procedure
//
LRESULT CALLBACK FontDialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
FontDialog* FontDialog::ptr = NULL;

//==========================================================================

FontDialog::FontDialog( HINSTANCE _hInstance )
	: hInstance( _hInstance )
{
	FontDialog::ptr = this;
	currentFontSize = 0;
};


FontDialog::~FontDialog( void )
{
	FontDialog::ptr = NULL;
};


bool FontDialog::Execute( void )
{
	bool success = false;
	if ( ::DialogBox( hInstance, MAKEINTRESOURCE(IDD_FONTDIALOG), 
					  NULL, (DLGPROC)FontDialogProc )==IDOK )
	{
		// set new language id in registry
		success = true;
	}
	return success;
}

LOGFONT FontDialog::Font( void ) const
{
	return currentFont;
};

int FontDialog::Size( void ) const
{
	return currentFontSize;
};

//==========================================================================

size_t fontCounter = 0;

int __stdcall EnumFontFamExProc(
								const LOGFONT *lplf,    // logical-font data
								const TEXTMETRIC *lpntme,  // physical-font data
								DWORD FontType,           // type of font
								LPARAM lParam             // application-defined data
							  )
{
	FontDialog* _ptr = (FontDialog*)lParam;
	if ( lplf!=NULL )
	{
		LOGFONT lf = *lplf;
		if ( fontCounter < kMaxFonts )
		{
			_ptr->fonts[fontCounter++] = lf;
		}
	}
	return 1;
};


TString DescribeFont( LOGFONT& lf )
{
	TString str;
	str = lf.lfFaceName;
	str = str + " (";

	switch ( lf.lfWeight )
	{
	case 100:
		str = str + "thin";
		break;
	case 200:
		str = str + "ultra light";
		break;
	case 300:
		str = str + "light";
		break;
	case 400:
		str = str + "normal";
		break;
	case 500:
		str = str + "medium";
		break;
	case 600:
		str = str + "semibold";
		break;
	case 700:
		str = str + "bold";
		break;
	case 800:
		str = str + "extrabold";
		break;
	case 900:
		str = str + "black";
		break;
	}

	if ( lf.lfItalic==TRUE )
		str = str + ",italic";
	if ( lf.lfUnderline==TRUE )
		str = str + ",underline";

	switch ( lf.lfCharSet )
	{
	case ANSI_CHARSET:
		str = str + ",ansi";
		break;
	case BALTIC_CHARSET:
		str = str + ",baltic";
		break;
	case CHINESEBIG5_CHARSET:
		str = str + ",chinese";
		break;
	case DEFAULT_CHARSET:
		str = str + ",default";
		break;
	case EASTEUROPE_CHARSET:
		str = str + ",eastern";
		break;
	case GB2312_CHARSET:
		str = str + ",gb2312";
		break;
	case GREEK_CHARSET:
		str = str + ",greek";
		break;
	case HANGUL_CHARSET:
		str = str + ",hangul";
		break;
	case MAC_CHARSET:
		str = str + ",mac";
		break;
	case OEM_CHARSET:
		str = str + ",oem";
		break;
	case RUSSIAN_CHARSET:
		str = str + ",russian";
		break;
	case SHIFTJIS_CHARSET:
		str = str + ",shiftjis";
		break;
	case SYMBOL_CHARSET:
		str = str + ",symbol";
		break;
	case TURKISH_CHARSET:
		str = str + ",turkish";
		break;
	case HEBREW_CHARSET:
		str = str + ",hebrew";
		break;
	case ARABIC_CHARSET:
		str = str + ",arabic";
		break;
	};

	str = str + ")";

	return str;
};

//==========================================================================

LRESULT CALLBACK FontDialogProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch (message)
	{
		
		case WM_INITDIALOG:
			{
				FontDialog* _ptr = FontDialog::ptr;

				_ptr->bCreateFont = GetDlgItem( hwnd, IDOK );
				_ptr->bExit  = GetDlgItem( hwnd, IDCANCEL );
				
				_ptr->ddFont  = GetDlgItem( hwnd, IDC_FONTS );
				_ptr->ddSize  = GetDlgItem( hwnd, IDC_FONTSIZE );

				// add sizes to drop down box
				size_t i;
				for ( i=6; i<16; i++ )
				{
					char buf[25];
					sprintf( buf, "%d", i );
					::ComboBoxAddString( _ptr->ddSize, buf );
				}
				::SetControlHeight( _ptr->ddSize, 200 );
				::DropDownBoxSetSelection( _ptr->ddSize, 0 );

				// enumerate all fonts
				fontCounter = 0;
				LOGFONT lf;
				lf.lfCharSet = DEFAULT_CHARSET;
				lf.lfFaceName[0] = 0;
				lf.lfPitchAndFamily = 0;
				HDC dc = ::GetWindowDC( ::GetDesktopWindow() );
				::EnumFontFamiliesEx( dc, &lf, EnumFontFamExProc, (LPARAM)_ptr, 0 );

				for ( i=0; i<fontCounter; i++ )
				{
					::ComboBoxAddString( _ptr->ddFont, DescribeFont( _ptr->fonts[i] ).c_str() );
				}
				::SetControlHeight( _ptr->ddFont, 200 );
				::DropDownBoxSetSelection( _ptr->ddFont, 0 );
				

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
						FontDialog* _ptr = FontDialog::ptr;

						_ptr->currentFontSize = ::DropDownBoxGetSelection( _ptr->ddSize );
						_ptr->currentFontSize += 6;
						_ptr->currentFont = _ptr->fonts[::DropDownBoxGetSelection( _ptr->ddFont )];

						::EndDialog(hwnd, IDOK);
						return 0;
					}
					if ( LOWORD(wParam)==IDCANCEL )
					{
						::EndDialog(hwnd, IDCANCEL);
						return 0;
					}
				}
				break;
			}
	}
	return 0;
}

//==========================================================================

