#include <precomp_header.h>

#include <windows.h>

#include <dialogs/dialogAssist.h>
#include <dialogs/customiseVehicle.h>

#include <win32/events.h>
#include <Win32/resource.h>

//==========================================================================

#ifndef _USEGUI
LRESULT CALLBACK CustomiseVehicleDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
CustomiseVehicle* CustomiseVehicle::ptr = NULL;
#endif

//==========================================================================

CustomiseVehicle::CustomiseVehicle( TEvent* _app, HINSTANCE _hInstance, HWND _parent )
	: hInstance( _hInstance ),
	  parent( _parent ),
	  app( _app ),
	  customisePage(NULL)
{
#ifndef _USEGUI
	CustomiseVehicle::ptr = this;
#endif
	initialised = false;
};

CustomiseVehicle::~CustomiseVehicle( void )
{
	if ( customisePage!=NULL )
		delete customisePage;
	customisePage = NULL;
};


bool CustomiseVehicle::Initialised( void ) const
{
	return initialised;
};


void CustomiseVehicle::Initialised( bool i )
{
	initialised = i;
};


bool CustomiseVehicle::Execute( size_t maxPoints )
{
#ifdef _USEGUI
	PreCond( app!=NULL );

	if ( customisePage!=NULL )
		delete customisePage;
	customisePage = NULL;

	AddItem( customisePage, new TGUI( app, guiWindow, 0, 0, 330, 220, -2, "Customise Vehicle" ) );

	AddItem( customisePage, new TGUI( app, guiLabel, 28,25,61,10, -2, "Amount of ammo", 255,255,255 ) );
	AddItem( customisePage, new TGUI( app, guiLabel, 36,55,53,9, -2, "Ammo strength", 255,255,255 ) );
	AddItem( customisePage, new TGUI( app, guiLabel, 32,85,58,11, -2, "Maximum speed:", 255,255,255 ) );
	AddItem( customisePage, new TGUI( app, guiLabel, 12,115,77,11, -2, "Ammo maximum range", 255,255,255 ) );
	AddItem( customisePage, new TGUI( app, guiLabel, 170,155,80,14, -2, "Available points", 255,255,255 ) );

	info1Label = new TGUI( app, guiLabel, 260,25,32,14, -2, "", 255,255,255 );
	info2Label = new TGUI( app, guiLabel, 260,55,32,14, -2, "", 255,255,255 );
	info3Label = new TGUI( app, guiLabel, 260,85,32,14, -2, "", 255,255,255 );
	info4Label = new TGUI( app, guiLabel, 260,115,32,14, -2, "", 255,255,255 );

	info5Label = new TGUI( app, guiLabel, 260,155,32,14, -2, "", 255,255,255 );

	ammoSlider = new TGUI( app, guiSlider, 130,25,120,23, -2 );
	strengthSlider = new TGUI( app, guiSlider, 130,55,120,23, -2 );
	speedSlider = new TGUI( app, guiSlider, 130,85,120,23, -2 );
	rangeSlider = new TGUI( app, guiSlider, 130,115,120,23, -2 );

	submergeCheckBox = new TGUI( app, guiCheckBox, 12,155,130,14, -2, "Vehicle can submerge" );

	okButton = new TGUI( app, guiButton, 190,185,60,25, -2, "OK" );
	cancelButton = new TGUI( app, guiButton, 260,185,60,25, -2, "Cancel" );
	okButton->ButtonKey( 13 );
	cancelButton->ButtonKey( VK_ESCAPE );

	customisePage->SetLocation( 100, 100 );

	AddItem( customisePage, info1Label );
	AddItem( customisePage, info2Label );
	AddItem( customisePage, info3Label );
	AddItem( customisePage, info4Label );
	AddItem( customisePage, info5Label );

	AddItem( customisePage, ammoSlider );
	AddItem( customisePage, strengthSlider );
	AddItem( customisePage, speedSlider );
	AddItem( customisePage, rangeSlider );
	AddItem( customisePage, submergeCheckBox );
	AddItem( customisePage, okButton );
	AddItem( customisePage, cancelButton );

	ammoSlider->SliderSetRange( 5, 25 );
	strengthSlider->SliderSetRange( 25, 65 );
	speedSlider->SliderSetRange( 30, 100 );
	rangeSlider->SliderSetRange( 5, 10 );

	ammoSlider->SliderSetFrequency( 5 );
	strengthSlider->SliderSetFrequency( 10 );
	speedSlider->SliderSetFrequency( 10 );
	rangeSlider->SliderSetFrequency( 1 );

	// set values
	ammoSlider->SliderPos( amount );
	strengthSlider->SliderPos( strength );
	speedSlider->SliderPos( speed );
	rangeSlider->SliderPos( range );
	submergeCheckBox->Checked( submerge );

	initialised = true;
	max = maxPoints;
	
	TGUI* prevPage = app->CurrentPage();
	app->CurrentPage( customisePage );

	MSG msg;
	size_t fpsTimer = ::GetTickCount();
	bool closing = false;
	while ( !closing )
	{
		::ProcessMessages( app, msg, fpsTimer );
		if ( msg.message==WM_QUIT || msg.message==WM_CLOSE )
		{
			closing = true;
			app->CurrentPage( prevPage );
			::PostQuitMessage( 0 );
		}
		UpdateInfo();
		if ( okButton->Clicked() )
		{
			app->CurrentPage( prevPage );
			return true;
		}
		if ( cancelButton->Clicked() )
		{
			app->CurrentPage( prevPage );
			return false;
		}
	}
	return false;
#else
	max = maxPoints;
	if ( DialogBox( hInstance, 
                    MAKEINTRESOURCE(IDD_CUSTOMISEVEHICLE), 
                    parent, (DLGPROC)CustomiseVehicleDlgProc)==IDOK ) 
	{
		return true;
	}
	else
		return false;
#endif
}


void CustomiseVehicle::UpdateInfo( void )
{
	if ( !initialised )
		return;

#ifdef _USEGUI
	amount = ammoSlider->SliderPos();
	strength = strengthSlider->SliderPos();
	speed = speedSlider->SliderPos();
	range = rangeSlider->SliderPos();
	submerge = submergeCheckBox->Checked();
#else
	SliderGetPos( hSpeed, speed );
	SliderGetPos( hRange, range );
	SliderGetPos( hStrength, strength );
	SliderGetPos( hAmount, amount );
	submerge = ButtonGetCheck( hSubmerge );
#endif

	long sub = 0;
	if ( submerge )
		sub = 200;

	long points = long(max) - long(speed*6 + range*20 + strength*3 + amount*6 + sub);

#ifdef _USEGUI
	info1Label->Text( Int2Str(amount) );
	info2Label->Text( Int2Str(strength) );
	info3Label->Text( Int2Str(speed) );
	info4Label->Text( Int2Str(range) );
	info5Label->Text( Int2Str(points) );

	if ( points<0 )
	{
		if ( okButton->Enabled() )
		{
			okButton->EnableControl( false );
		}
	}
	else
	{
		if ( !okButton->Enabled() )
		{
			okButton->EnableControl( true );
		}
	}
#else
	EditPrintSingle( hInfo2, Int2Str(amount).c_str() );
	EditPrintSingle( hInfo3, Int2Str(strength).c_str() );
	EditPrintSingle( hInfo4, Int2Str(speed).c_str() );
	EditPrintSingle( hInfo5, Int2Str(range).c_str() );
	EditPrintSingle( hPoints, Int2Str(points).c_str() );

	if ( points<0 )
	{
		EnableControl( hOk, false );
	}
	else
	{
		EnableControl( hOk, true );
	}
#endif
};


#ifndef _USEGUI
LRESULT CALLBACK CustomiseVehicleDlgProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch (message)
	{
		
		case WM_INITDIALOG:
			{
				CustomiseVehicle::ptr->hPoints = GetDlgItem( hwnd, IDC_POINTS );
				CustomiseVehicle::ptr->hSubmerge = GetDlgItem( hwnd, IDC_SUBMERGE );
				CustomiseVehicle::ptr->hSpeed = GetDlgItem( hwnd, IDC_MAXSPEED );
				CustomiseVehicle::ptr->hRange = GetDlgItem( hwnd, IDC_MAXRANGE );
				CustomiseVehicle::ptr->hStrength = GetDlgItem( hwnd, IDC_AMMOSTRENGTH );
				CustomiseVehicle::ptr->hAmount = GetDlgItem( hwnd, IDC_AMMOCOUNT );
				CustomiseVehicle::ptr->hOk = GetDlgItem( hwnd, IDOK );

				CustomiseVehicle::ptr->hInfo1 = GetDlgItem( hwnd, IDC_I1 );
				CustomiseVehicle::ptr->hInfo2 = GetDlgItem( hwnd, IDC_I2 );
				CustomiseVehicle::ptr->hInfo3 = GetDlgItem( hwnd, IDC_I3 );
				CustomiseVehicle::ptr->hInfo4 = GetDlgItem( hwnd, IDC_I4 );
				CustomiseVehicle::ptr->hInfo5 = GetDlgItem( hwnd, IDC_I5 );

				CustomiseVehicle::ptr->Initialised( true );

				SliderSetRange( CustomiseVehicle::ptr->hAmount, 5, 25 );
				SliderSetRange( CustomiseVehicle::ptr->hStrength, 25, 65 );
				SliderSetRange( CustomiseVehicle::ptr->hSpeed, 30, 100 );
				SliderSetRange( CustomiseVehicle::ptr->hRange, 5, 10 );

				SliderSetFrequency( CustomiseVehicle::ptr->hAmount, 5 );
				SliderSetFrequency( CustomiseVehicle::ptr->hStrength, 10 );
				SliderSetFrequency( CustomiseVehicle::ptr->hSpeed, 10 );
				SliderSetFrequency( CustomiseVehicle::ptr->hRange, 1 );

				SliderSetPos( CustomiseVehicle::ptr->hAmount, CustomiseVehicle::ptr->amount );
				SliderSetPos( CustomiseVehicle::ptr->hStrength, CustomiseVehicle::ptr->strength );
				SliderSetPos( CustomiseVehicle::ptr->hSpeed, CustomiseVehicle::ptr->speed );
				SliderSetPos( CustomiseVehicle::ptr->hRange, CustomiseVehicle::ptr->range );
				ButtonSetCheck( CustomiseVehicle::ptr->hSubmerge, CustomiseVehicle::ptr->submerge );

				CustomiseVehicle::ptr->UpdateInfo();

				SetTimer( hwnd, 1, 100, NULL );

				return 0;
			}

		case WM_TIMER:
			{
				CustomiseVehicle::ptr->UpdateInfo();
				::ValidateRgn( hwnd, NULL );
				break;
			}

		case WM_CLOSE:
			{
				KillTimer(hwnd,1);
				::EndDialog(hwnd,IDCANCEL);
			}
			break;

		case WM_COMMAND:
			{
				if ( HIWORD(wParam)==BN_CLICKED )
				{
					if ( LOWORD(wParam)==IDCANCEL )
					{
						KillTimer(hwnd,1);
						EndDialog(hwnd, IDCANCEL);
						return 0;
					}
					else if ( LOWORD(wParam)==IDOK )
					{
						KillTimer(hwnd,1);
						EndDialog(hwnd, IDOK);
						return 0;
					}
				}
				break;
			}

		case WM_SYSCOMMAND:
			{
				if (wParam==IDCANCEL)
				{
					KillTimer(hwnd,1);
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
				return 0;
			}
	}
	return 0;
}
#endif

/*
	// Customise vehicle dialog
	AddItem( customisePage, new TGUI( this, guiWindow, 0, 0, 330, 220, -3, "Customise Vehicle" ) );

	AddItem( customisePage, new TGUI( this, guiLabel, 28,25,61,10, -3, "Amount of ammo", 255,255,255 ) );
	AddItem( customisePage, new TGUI( this, guiLabel, 36,55,53,9, -3, "Ammo strength", 255,255,255 ) );
	AddItem( customisePage, new TGUI( this, guiLabel, 32,85,58,11, -3, "Maximum speed:", 255,255,255 ) );
	AddItem( customisePage, new TGUI( this, guiLabel, 12,115,77,11, -3, "Ammo maximum range", 255,255,255 ) );
	AddItem( customisePage, new TGUI( this, guiLabel, 170,155,80,14, -3, "Available points", 255,255,255 ) );

	info1Label = new TGUI( this, guiLabel, 260,25,32,14, -3, "l1", 255,255,255 );
	info2Label = new TGUI( this, guiLabel, 260,55,32,14, -3, "l2", 255,255,255 );
	info3Label = new TGUI( this, guiLabel, 260,85,32,14, -3, "l3", 255,255,255 );
	info4Label = new TGUI( this, guiLabel, 260,115,32,14, -3, "l4", 255,255,255 );

	info5Label = new TGUI( this, guiLabel, 260,155,32,14, -3, "l5", 255,255,255 );

	ammoSlider = new TGUI( this, guiSlider, 130,25,120,23, -3 );
	strengthSlider = new TGUI( this, guiSlider, 130,55,120,23, -3 );
	speedSlider = new TGUI( this, guiSlider, 130,85,120,23, -3 );
	rangeSlider = new TGUI( this, guiSlider, 130,115,120,23, -3 );

	submergeCheckBox = new TGUI( this, guiCheckBox, 12,155,130,14, -3, "Vehicle can submerge" );

	okButton = new TGUI( this, guiButton, 190,185,60,25, -3, "OK" );
	cancelButton = new TGUI( this, guiButton, 260,185,60,25, -3, "Cancel" );

	customisePage->SetLocation( 100, 100 );
*/
