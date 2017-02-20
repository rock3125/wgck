#include <precomp_header.h>

#include <windows.h>
#include <direct.h>
#include <process.h>
#include <ddraw.h>

#include <systemDialogs/dialogAssist.h>

#include <Win32/resource.h>

HWND hEditMessage = NULL;
HWND hPerformanceDlg = NULL;

//==========================================================================

#ifdef _SPEEDTEST

#define kMaxResolutions 20

HWND hRes = NULL;
HWND hFullScreen = NULL;

size_t resolutionCounter = 0;
size_t currentRes = 0;
size_t currentFullScreen = 1;

int		rwidth[kMaxResolutions];
int		rheight[kMaxResolutions];
int		rbitDepth[kMaxResolutions];
int		rzDepth[kMaxResolutions];

void ClearResolutions( void );
HRESULT WINAPI EnumModesCallback3(
				LPDDSURFACEDESC2 lpsdesc,  
				LPVOID lpContext  );
#endif

//==========================================================================
//
// Window callback procedure
//
LRESULT CALLBACK PerformanceDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

//
// Create
//
#ifdef _SPEEDTEST
bool CreatePerformanceDlg( HINSTANCE hInstance, HWND parent )
{
	return DialogBox( hInstance, 
                      MAKEINTRESOURCE(IDD_PERFORMANCE), 
                      parent, (DLGPROC)PerformanceDlgProc)==IDOK; 
}
#else
void CreatePerformanceDlg( HINSTANCE hInstance, HWND parent )
{
	hPerformanceDlg = CreateDialog( hInstance, MAKEINTRESOURCE(IDD_PERFORMANCE), 
									parent, (DLGPROC)PerformanceDlgProc);
	ShowWindow(hPerformanceDlg, SW_SHOW);
}
#endif


#ifdef _SPEEDTEST

bool EnumResolutions( void )
{
	ClearResolutions();

	LPDIRECTDRAW7 lpDD;	// DirectDraw Object
	WriteString( "setting up DirectX 7\n" );
	int hRes=DirectDrawCreateEx(NULL,(LPVOID*)&lpDD,IID_IDirectDraw7,NULL);
	if ( hRes!=DD_OK) 
	{ 
		TString msg = "Error: Could not create DirectX7 object (" + Int2Str(hRes) + ")\n";
		WriteString( msg.c_str() );
		return false; 
	}

	bool success = lpDD->EnumDisplayModes( DDEDM_REFRESHRATES,
									NULL, NULL, 
									EnumModesCallback3 )==DD_OK;

	return success;
};


void ClearResolutions( void )
{
	resolutionCounter = 0;
};


void AddResolution( int width, int height, int bitDepth, int zDepth )
{
	bool found = false;
	size_t i = 0;
	while ( i<resolutionCounter && !found )
	{
		if ( width==rwidth[i] &&
			 height==rheight[i] &&
			 bitDepth==rbitDepth[i] &&
			 zDepth==rzDepth[i] )
			found = true;
		i++;
	}
	if ( !found && resolutionCounter < kMaxResolutions )
	{
		rwidth[resolutionCounter] = width;
		rheight[resolutionCounter] = height;
		rbitDepth[resolutionCounter] = bitDepth;
		rzDepth[resolutionCounter] = zDepth;

		resolutionCounter++;
	}
};

size_t NumResolutions( void )
{
	return resolutionCounter;
};

void GetResolution( size_t index, int& width, int& height, int& bitDepth, int& zDepth )
{
	PreCond( index < resolutionCounter );
	width = rwidth[index];
	height = rheight[index];
	bitDepth = rbitDepth[index];
	zDepth = rzDepth[index];
};


void GetRegistrySettings( void )
{
	// get values and set them in the dialog
	if ( !GetRegistryKey( "Software\\PDV\\Performance", "Resolution", currentRes ) )
		currentRes = 0;
	if ( !GetRegistryKey( "Software\\PDV\\Performance", "FullScreen", currentFullScreen ) )
		currentFullScreen = 1;
};

HRESULT WINAPI EnumModesCallback3(
				LPDDSURFACEDESC2 lpsdesc,  
				LPVOID lpContext  )
{
	int width = lpsdesc->dwWidth;
	int height = lpsdesc->dwHeight;
	int bitDepth = lpsdesc->ddpfPixelFormat.dwRGBBitCount;
	int zDepth = lpsdesc->ddpfPixelFormat.dwZBufferBitDepth;
	size_t fourCC = lpsdesc->ddpfPixelFormat.dwFourCC;
	if ( fourCC==0 ) // is it RGB?
	if ( width>=640 && height>=480 && 
		 bitDepth>8 && zDepth>=8 )
	{
		AddResolution( width, height, bitDepth, zDepth );
	}
	return DDENUMRET_OK;
};


void GetRating( TString& rating )
{
	bool success = false;
	size_t val;
	if ( GetRegistryKey( "Software\\PDV\\Performance", "TestCompleted", val ) )
	if ( val>0 )
		success = true;

	if ( success )
	{
		char buf2[256];
		if ( val < 1000 )
			sprintf( buf2, "%d", val );
		else if ( val < 1000000.0f )
			sprintf( buf2, "%2.2f thousand", double(val) / 1000.0 );
		else
			sprintf( buf2, "%2.2f million", double(val) / 1000000.0 );

		TString rat;
		if ( val<100000 )
			rat = "Diagnosis: Very bad - you won't be able to run this program\nwith your current video hardware";
		else if ( val<250000 )
			rat ="Diagnosis: Not good - the program won't perform great.\nPlease upgrade your video hardware before\ntrying this game again.";
		else if ( val<500000 )
			rat = "Diagnosis: Good - average performance";
		else if ( val<1000000 )
			rat = "Diagnosis: Excellent";
		else
			rat = "Diagnosis: Perfect";

		rating = "Finished performance measurements.  Your video card\n";
		rating = rating + "is capable of producing " + TString(buf2) + " polygons per second\n";
		rating = rating + rat;
	}
	else
		rating = "Could not get registry setting - program did not finish?\n";
};

#endif

//==========================================================================

LRESULT CALLBACK PerformanceDlgProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch (message)
	{
		
		case WM_INITDIALOG:
			{
				hEditMessage = GetDlgItem( hwnd, IDC_EDIT1 );
				hRes = GetDlgItem( hwnd, IDC_RESOLUTION );
				hFullScreen = GetDlgItem( hwnd, IDC_FULLSCREEN );

				EditPrint( hEditMessage, "The performance program will measure the suitability" );
				EditPrint( hEditMessage, "of your computer for running this software.  Make sure" );
				EditPrint( hEditMessage, "you have selected the appropriate video settings before" );
				EditPrint( hEditMessage, "executing this test.\n" );
				EditPrint( hEditMessage, "The performance test will take approximately 45 seconds" );
				EditPrint( hEditMessage, "to run. (you can stop the program at any time with the [ESC] key)" );

#ifdef _SPEEDTEST
				GetRegistrySettings();

				EnumResolutions();
				SetControlHeight( hRes, 100 );
				size_t cntr = NumResolutions();
				int w,h,bd,zd;
				for ( size_t i=0; i<cntr; i++ )
				{
					GetResolution( i, w,h,bd,zd );
					TString msg = Int2Str(w) + " x " + Int2Str(h);
					msg = msg + " x " + Int2Str(bd);
					ComboBoxAddString( hRes, msg.c_str() );
				}
				DropDownBoxSetSelection( hRes, currentRes );
				ButtonSetCheck( hFullScreen, currentFullScreen==1 );
#endif
				return 0;
			}

		case WM_CLOSE:
			{
				::EndDialog(hwnd,0);
			}
			break;

		case WM_DESTROY: 
			{
#ifndef _SPEEDTEST
				PostQuitMessage(0); 
#endif
				return 0;
			}

		case WM_COMMAND:
			{
				if ( HIWORD(wParam)==BN_CLICKED )
				{
					if ( LOWORD(wParam)==IDCANCEL )
					{
						EndDialog(hwnd, IDCANCEL );
						return 0;
					}
					else if ( LOWORD(wParam)==IDOK )
					{
#ifdef _SPEEDTEST
						currentRes = DropDownBoxGetSelection( hRes );
						if ( ButtonGetCheck( hFullScreen ) )
							currentFullScreen=1;
						else
							currentFullScreen=0;
						SetRegistryKey( "Software\\PDV\\Performance", "Resolution", currentRes );
						SetRegistryKey( "Software\\PDV\\Performance", "FullScreen", currentFullScreen );

						EndDialog(hwnd, IDOK );
						return 0;
#else
						char buf[256];
						char olddir[256];
						char dir[256];
				        getcwd(buf,255);
						strcpy( olddir, buf );

						strcat(buf,"\\system" );
						strcpy(dir,buf);
						strcat(buf,"\\performance.exe" );

						if ( !TPersist::FileExists( buf ) )
						{
							strcpy( buf, olddir );
							strcat(buf,"\\performance.exe" );
						}
						else
						{
							chdir( dir );
						}

						// clear setting
						SetRegistryKey( "Software\\PDV\\Performance", "TestCompleted", 0 );

						ShowWindow( hwnd, SW_HIDE );
						if ( spawnl( _P_WAIT, buf, "Performance.exe", NULL)!=0 )
						{
							chdir( olddir );
							ShowWindow( hwnd, SW_SHOW );
							MessageBox( hwnd, "An error occured executing the performance test\nPlease try modifying the video settings and try again",
										"Error", MB_OK | MB_ICONERROR );
						}
						else
						{
							chdir( olddir );
							ShowWindow( hwnd, SW_SHOW );
							EditClear( hEditMessage );

							size_t val;
							bool success = false;
							if ( GetRegistryKey( "Software\\PDV\\Performance", "TestCompleted", val ) )
							if ( val>0 )
								success = true;

							if ( success )
							{
								char buf2[256];
								if ( val < 1000 )
									sprintf( buf2, "%d", val );
								else if ( val < 1000000.0f )
									sprintf( buf2, "%2.2f thousand", double(val) / 1000.0 );
								else
									sprintf( buf2, "%2.2f million", double(val) / 1000000.0 );

								char rating[256];
								if ( val<100000 )
									strcpy( rating, "Diagnosis: Very bad - you won't be able to run this program\nwith your current video hardware" );
								else if ( val<250000 )
									strcpy( rating, "Diagnosis: Not good - the program won't perform great.\nPlease upgrade your video hardware before\ntrying this game again." );
								else if ( val<500000 )
									strcpy( rating, "Diagnosis: Good - average performance" );
								else if ( val<1000000 )
									strcpy( rating, "Diagnosis: Excellent" );
								else
									strcpy( rating, "Diagnosis: Perfect" );

								EditPrint( hEditMessage, "Finished performance measurements" );
								EditPrint( hEditMessage, "Your video card is capable of producing" );
								EditPrint( hEditMessage, "%s polygons per second\n", buf2 );
								EditPrint( hEditMessage, rating );
							}
							else
								EditPrint( hEditMessage, "Could not get registry setting - program did not finish?\n" );
						}
#endif
					}
				}
				break;
			}

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

		default:
			{
				return 0;
			}
	}
	return 0;
}

