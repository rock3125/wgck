#include <precomp_header.h>

#include <windows.h>
#include <direct.h>
#include <process.h>

#include <dialogs/dialogAssist.h>

#include <SetupWin32/resource.h>

HWND hEditMessage = NULL;
HWND hPerformanceDlg = NULL;
HWND hSound = NULL;

size_t currMusic;

//==========================================================================
//
// Window callback procedure
//
LRESULT CALLBACK PerformanceDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

//
// Create
//
void CreatePerformanceDlg( HINSTANCE hInstance, HWND parent )
{
	hPerformanceDlg = CreateDialog( hInstance, MAKEINTRESOURCE(IDD_PERFORMANCE), 
									parent, (DLGPROC)PerformanceDlgProc);
	ShowWindow(hPerformanceDlg, SW_SHOW);
}


//==========================================================================

LRESULT CALLBACK PerformanceDlgProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch (message)
	{
		
		case WM_INITDIALOG:
			{
				hEditMessage = GetDlgItem( hwnd, IDC_EDIT1 );
				hSound = GetDlgItem( hwnd, IDC_SOUND );

				size_t currMusic;
				if ( !GetRegistryKey( "Software\\PDV\\Performance", "PlayMusic", currMusic ) )
					currMusic = 1;

				ButtonSetCheck( hSound, (currMusic==1) );

				EditPrint( hEditMessage, "The performance program will measure the suitability" );
				EditPrint( hEditMessage, "of your computer for running this software.  Make sure" );
				EditPrint( hEditMessage, "you have selected the appropriate video settings from" );
				EditPrint( hEditMessage, "the menu before executing this test.\n" );
				EditPrint( hEditMessage, "The performance test will take approximately 45 seconds" );
				EditPrint( hEditMessage, "to run. (you can stop the program at any time with ALT+F4)" );

				return 0;
			}

		case WM_CLOSE:
			{
				::EndDialog(hwnd,0);
			}
			break;

		case WM_DESTROY: 
			{
				PostQuitMessage(0); 
				return 0;
			}

		case WM_COMMAND:
			{
				if ( HIWORD(wParam)==BN_CLICKED )
				{
					if ( LOWORD(wParam)==IDCANCEL )
					{
						EndDialog(hwnd, 0);
						return 0;
					}
					else if ( LOWORD(wParam)==IDOK )
					{
						if ( ButtonGetCheck( hSound ) )
							currMusic = 1;
						else
							currMusic = 0;

						if ( !SetRegistryKey( "Software\\PDV\\Performance", "PlayMusic", currMusic ) )
						{
							MessageBox( hwnd, "Error setting registry values?", "Error", MB_OK|MB_ICONERROR );
						}

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

