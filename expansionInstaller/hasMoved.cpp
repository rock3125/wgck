#include <windows.h>
#include <shellapi.h>

int APIENTRY WinMain( HINSTANCE hInst,
			   		  HINSTANCE hPrevInstance,
			   		  LPSTR lpCmdLine,
			   		  int nCmdShow )
{
	char msg[1024];
	strcpy( msg, "Sorry\n\nWar Games Inc. has moved from this location\n\n" );
	strcat( msg, "Please click yes to download it from its new location at\n" );
	strcat( msg, "http://www.peter.co.nz/\n\n" );
	strcat( msg, "thanks!" );
	if ( ::MessageBox( NULL, msg, "Oops", MB_YESNO | MB_ICONINFORMATION )==IDYES )
	{
		char* url = "http://www.peter.co.nz/tank/installer.exe";
		::ShellExecute( NULL, "open", url, "", "", SW_SHOW );
	}
	return 0;
};

