#include <precomp_header.h>

#include <dos.h>
#include <direct.h>
#include <shellapi.h>

#include <common/compatability.h>

#include <expansionInstaller/WelcomeDlg.h>
#include <expansionInstaller/resource.h>

//==========================================================================
//
// Entry point of all Windows programs
//
TString	_dir;

int APIENTRY WinMain( HINSTANCE hInst,
			   		  HINSTANCE hPrevInstance,
			   		  LPSTR lpCmdLine,
			   		  int nCmdShow )
{
	TString name, email;

	WelcomeDlg dlg(hInst);
	if ( dlg.Execute() )
	{
		if ( !IsRegistered( name, email ) )
		{
			TString msg = "Sorry\n\n";
			msg = msg + "You are not a registered user (could not find a valid registration code)\n";
			msg = msg + "You can register online at http://www.excessionsoft.com";
			::MessageBox( NULL, msg.c_str(), "Error", MB_OK | MB_ICONERROR );
		}
		else
		{
			// first - get the resource memory pointer
			HRSRC  resHandle = ::FindResource(hInst, MAKEINTRESOURCE(IDR_PAYLOAD1), "PAYLOAD" );
			if ( resHandle==0 )
			{
				TString msg = "FATAL\n\n";
				msg = msg + "payload resource not found\n";
				::MessageBox( NULL, msg.c_str(), "Error", MB_OK | MB_ICONERROR );
				return 0;
			};

			size_t size = ::SizeofResource(hInst, resHandle);
			if ( size==0 )
			{
				TString msg = "FATAL\n\n";
				msg = msg + "payload size==0\n";
				::MessageBox( NULL, msg.c_str(), "Error", MB_OK | MB_ICONERROR );
				return 0;
			};

			HANDLE handle = resHandle ? ::LoadResource(hInst, resHandle) : 0;
			if ( handle==NULL )
			{
				TString msg = "FATAL\n\n";
				msg = msg + "can't load resource\n";
				::MessageBox( NULL, msg.c_str(), "Error", MB_OK | MB_ICONERROR );
				return 0;
			}

			unsigned char* ptr = (unsigned char*) ::LockResource(handle);
			if ( ptr==NULL )
			{
				TString msg = "FATAL\n\n";
				msg = msg + "can't lock resource\n";
				::MessageBox( NULL, msg.c_str(), "Error", MB_OK | MB_ICONERROR );
			}
			else
			{
				char path[256];
				if ( ::GetTempPath( 255, path )==0 )
				{
					TString msg = "FATAL\n\n";
					msg = msg + "can't find temporary file path\n";
					::MessageBox( NULL, msg.c_str(), "Error", MB_OK | MB_ICONERROR );
					return 0;
				}
				char tempname[512];
				if ( ::GetTempFileName( path, "pld", 0, tempname )==0 )
				{
					TString msg = "FATAL\n\n";
					msg = msg + "can't create temporary file name\n";
					::MessageBox( NULL, msg.c_str(), "Error", MB_OK | MB_ICONERROR );
					return 0;
				}

				// change .tmp to .exe
				size_t index = strlen(tempname) - 4;
				if ( tempname[index]=='.' )
				{
					tempname[index+1] = 'e';
					tempname[index+2] = 'x';
					tempname[index+3] = 'e';
				}
				else
				{
					TString msg = "FATAL\n\n";
					msg = msg + "malformed temporary file name\n";
					::MessageBox( NULL, msg.c_str(), "Error", MB_OK | MB_ICONERROR );
					return 0;
				}

				// open this file and pump the payload into it
				FILE* fh = fopen( tempname, "wb" );
				if ( fh==NULL )
				{
					TString msg = "FATAL\n\n";
					msg = msg + "could not create payload output file\n";
					::MessageBox( NULL, msg.c_str(), "Error", MB_OK | MB_ICONERROR );
					return 0;
				}
				else
				{
					// decrypt payload
					const char* code = "*;U2DQ4#C\"|)THF9$Y0V>~3^\\(GIN5=EA,Z!R.1-`WMKL/8@?OJ<P%&+B:6XS7 '";
					size_t codeLength = strlen(code);

					for ( size_t i=0; i<size; i++ )
					{
						ptr[i] = ptr[i] ^ (unsigned char)code[i%codeLength];
					}

					// write payload
					if ( fwrite( (char*)ptr, 1, size, fh ) != size )
					{
						fclose(fh);
						TString msg = "FATAL\n\n";
						msg = msg + "could not write payload to output file\n";
						::MessageBox( NULL, msg.c_str(), "Error", MB_OK | MB_ICONERROR );
						return 0;
					}
					fclose(fh);

					// and execute the created file - start the installer
					::ShellExecute( NULL, "open", tempname, "", "", SW_SHOW );
				}
			}

			UnlockResource(ptr);
			::FreeResource(handle);
		}
	}

	return 0;
}

//==========================================================================

