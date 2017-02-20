#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include <common/string.h>
#include <win32/resource.h>
#include <dialogs/dialogAssist.h>

TString _dir;

//==========================================================================

class GenRegDialog
{
public:
	GenRegDialog( HINSTANCE );
	~GenRegDialog( void );

	bool Execute( void );

	HINSTANCE	hInstance;
	bool		initialised;

	HWND		hName;
	HWND		hCode;
	HWND		hGenButton;
	HWND		hUserName;

	static GenRegDialog* ptr;
};

//==========================================================================
//
// Window callback procedure
//
LRESULT CALLBACK GenRegDialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
GenRegDialog* GenRegDialog::ptr = NULL;

//==========================================================================

int APIENTRY WinMain( HINSTANCE hInst,
			   		  HINSTANCE hPrevInstance,
			   		  LPSTR lpCmdLine,
			   		  int nCmdShow )
{
	GenRegDialog dlg(hInst);
	dlg.Execute();

	return 0;
};

//==========================================================================

GenRegDialog::GenRegDialog( HINSTANCE _hInstance )
	: hInstance( _hInstance )
{
	GenRegDialog::ptr = this;
	initialised = false;
};


GenRegDialog::~GenRegDialog( void )
{
	GenRegDialog::ptr = NULL;
};


bool GenRegDialog::Execute( void )
{
	if ( DialogBox( hInstance, 
                    MAKEINTRESOURCE(IDD_ENTERREGO), 
                    NULL, (DLGPROC)GenRegDialogProc)==IDOK) 
	{
		return true;
	};
	return false;
}

void GenerateSerial( const char* _email, char* code )
{
	char email[256];
	strcpy( email, _email );
	strupr( email );

	size_t code1Len = strlen( email );
	if ( code1Len > 35 )
	{
		email[35] = 0;
	}

	char storeData[256];
	strcpy( storeData, email );

	size_t total = 0;
	size_t len = strlen(storeData);
	code[0] = 0;
	size_t codeIndex = 0;
	size_t i;

	for ( i=0; i<len; i++ )
	{
		char letter = storeData[i];
		char ascii = letter;
		bool valid = false;
		if ( ascii >= 65 )
		{
			if ( ascii <= 91 )
			{
				valid = true;
			}
		}
		if ( ascii >= 48 )
		{
			if ( ascii <= 58 )
			{
				valid = true;
			}
		}
		if ( ascii == 46 )
		{
			valid = true;
		}
		if ( ascii == 64 )
		{
			valid = true;
		}
		if ( valid )
		{
			total = total + ascii;
			code[codeIndex++] = ascii;
		}
	}
	code[codeIndex] = 0;

	// encode the id string - this is the actual coding part
	// encode 0..9 A..Z string into "secret code"
	// . = 0x2e = 46
	// @ = 0x40 = 64

	const char* abc = "9BHJ0E-N45X2I76CVQ1TP+DKZOAG3WMSYFR8LU";
	code1Len = strlen(code);
	int perm = 3;
	char code2[256];
	code2[0] = 0;
	size_t code2Index = 0;
	for ( i=0; i < code1Len; i++ )
	{
		char letter = code[i];
		char ascii = letter;
		if ( ascii >= 65 )
		{
			ascii = ascii - 65;
		}
		if ( ascii >= 48 )
		{
			if ( ascii <= 58 )
			{
				ascii = (ascii - 48) + 26;
			}
		}
		if ( ascii == 46 )
		{
			ascii = (ascii - 46) + 36;
		}
		if ( ascii == 64 )
		{
			ascii = (ascii - 64) + 37;
		}
		ascii = ascii + perm;
		if ( perm == 3 )
		{
			perm = -3;
		}
		else
		{
			perm = 3;
		}
		if ( ascii < 0 )
		{
			ascii = ascii + 38;
		}
		if ( ascii > 37 )
		{
			ascii = ascii - 38;
		}
		code2[code2Index++] = abc[ascii];
	}
	code2[code2Index] = 0;

	// prefix code with hex length
	char prefix[256];
	sprintf( prefix, "%02X", code1Len );

	// postfix code is hex checksum
	char postfix[256];
	sprintf( postfix, "%04X", total );

	// finale code is all together
	strcpy( code, prefix );
	strcat( code, code2 );
	strcat( code, postfix );
};

//==========================================================================

LRESULT CALLBACK GenRegDialogProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch (message)
	{
		
		case WM_INITDIALOG:
			{
				GenRegDialog::ptr->hName = GetDlgItem( hwnd, IDC_REGONAME );
				GenRegDialog::ptr->hCode = GetDlgItem( hwnd, IDC_REGOCODE );
				GenRegDialog::ptr->hGenButton = GetDlgItem( hwnd, IDC_GENERATE );

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
					if ( LOWORD(wParam)==IDC_GENERATE )
					{
						char buf[256];
						EditGetText( GenRegDialog::ptr->hName, buf, 256 );
						TString name = buf;
						char code[256];
						GenerateSerial( name.c_str(), code );
						EditPrintSingle( GenRegDialog::ptr->hCode, code );
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
