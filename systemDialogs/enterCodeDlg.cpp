#include <precomp_header.h>

#include <windows.h>

#include <systemDialogs/dialogAssist.h>
#include <systemDialogs/enterCodeDlg.h>

#include <win32/resource.h>

//==========================================================================
//
// Window callback procedure
//
LRESULT CALLBACK EnterCodeDialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
EnterCodeDialog* EnterCodeDialog::ptr = NULL;

//==========================================================================

EnterCodeDialog::EnterCodeDialog( HINSTANCE _hInstance, HWND _parent )
	: hInstance( _hInstance ),
	  parent( _parent )
{
	EnterCodeDialog::ptr = this;
	initialised = false;
};


EnterCodeDialog::~EnterCodeDialog( void )
{
	EnterCodeDialog::ptr = NULL;
};


bool EnterCodeDialog::Execute( void )
{
	if ( DialogBox( hInstance, 
                    MAKEINTRESOURCE(IDD_ENTERREGO), 
                    parent, (DLGPROC)EnterCodeDialogProc)==IDOK) 
	{
		return true;
	};
	return false;
}

//==========================================================================

LRESULT CALLBACK EnterCodeDialogProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch (message)
	{
		
		case WM_INITDIALOG:
			{
				size_t languageId;
				if ( !GetRegistryKey( "Software\\PDV\\Performance", "Language", languageId ) )
				{
					languageId = 0;
				}

				EnterCodeDialog::ptr->initialised = true;
				EnterCodeDialog::ptr->hName = GetDlgItem( hwnd, IDC_REGONAME );
				EnterCodeDialog::ptr->hCode = GetDlgItem( hwnd, IDC_REGOCODE );
				EnterCodeDialog::ptr->hGenButton = GetDlgItem( hwnd, IDC_GENERATE );
				EnterCodeDialog::ptr->hUserName = GetDlgItem( hwnd, IDC_NAME );
				EnterCodeDialog::ptr->okButton = GetDlgItem( hwnd, IDOK );
				EditPrintSingle( EnterCodeDialog::ptr->hUserName, STR(strEnterName) );
				EnableControl( EnterCodeDialog::ptr->hGenButton, false );
				ShowControl( EnterCodeDialog::ptr->hGenButton, false );
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
						TString name, code;
						char buf[256];
						EditGetText( EnterCodeDialog::ptr->hName, buf, 256 );
						name = buf;
						EditGetText( EnterCodeDialog::ptr->hCode, buf, 256 );
						code = buf;

						size_t languageId;
						if ( !GetRegistryKey( "Software\\PDV\\Performance", "Language", languageId ) )
						{
							languageId = 0;
						}

						if ( name.length()==0 )
						{
							const char* msg = STR(strEnterName);
							::MessageBox( hwnd, msg, STR(strError), MB_OK | MB_ICONERROR );
						}
						else
						{
							if ( ::ValidCode( code ) )
							{
								::SetRegistryKey( "Software\\PDV\\Performance", "RegoCode", code );
								::SetRegistryKey( "Software\\PDV\\Performance", "RegoName", name );
								::MessageBox( hwnd, STR(strThanksForRegistering), STR(strThankYou), MB_OK | MB_ICONINFORMATION );
								::EndDialog(hwnd,IDOK);
							}
							else
							{
								::MessageBox( hwnd, STR(strIncorrectCode), STR(strError), MB_OK | MB_ICONERROR );
							}
						}
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

		case WM_SYSKEYUP:
			{
				if ( wParam=='C' || wParam=='c' )
				{
				}
			}
			break;

		default:
			{
				/*
				char buf[256];
				EditGetText( EnterCodeDialog::ptr->hCode, buf, 256 );
				TString code = buf;
				bool cv = ValidCode( code );
				if ( cv!=Enabled(EnterCodeDialog::ptr->okButton) )
				{
					EnableControl( EnterCodeDialog::ptr->okButton, cv );
				}
				*/
				return 0;
			}
	}
	return 0;
}

//==========================================================================
