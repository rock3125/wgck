#include <precomp_header.h>

#include <Commdlg.h>
#include <shlobj.h>
#include <direct.h>
#include <Winbase.h>
#include <shellapi.h>

#include <io.h>

#include <dialogs/dialogassist.h>
#include <win32/resource.h>
#include <landscape/main.h>
#include <landscape/BotEditorWindow.h>

//==========================================================================

BotEditor* BotEditor::ptr = NULL;

#define _ptr BotEditor::ptr

//==========================================================================

BotEditor::BotEditor( TApp& _app, HINSTANCE _instance, HWND _parent )
	: app(_app),
	  parent( _parent ),
	  instance( _instance )
{
	ptr = this;

	changed = false;
	showMarkers = true;
	currentMarker = -1;
	currentc1 = 0;
	currentc2 = 0;
	currentc3 = 0;
	currentc4 = 0;

	selectedAI = 0;

	dialog = CreateDialog( _instance, MAKEINTRESOURCE(IDD_BOTDLG),
						   parent, (DLGPROC)BotEditorProc );
};


BotEditor::~BotEditor( void )
{
};


void BotEditor::SetAICheck( size_t aiCheck )
{
	selectedAI = aiCheck;
	currentMarker = -1;
	DropDownBoxSetSelection( ddMarkers, currentMarker );
	SetMapInfo( app.def );
};


size_t BotEditor::SelectedAI( void ) const
{
	return selectedAI;
}


void BotEditor::SetMapInfo( TDefFile& def )
{
	ComboBoxClear( ddMarkers );

	ComboBoxClear( ddConnection1 );
	ComboBoxClear( ddConnection2 );
	ComboBoxClear( ddConnection3 );
	ComboBoxClear( ddConnection4 );
	ComboBoxAddString( ddConnection1, "none" );
	ComboBoxAddString( ddConnection2, "none" );
	ComboBoxAddString( ddConnection3, "none" );
	ComboBoxAddString( ddConnection4, "none" );

	size_t count = 0;
	switch ( selectedAI )
	{
	case 0:
	{
		count = def.NumLocations();
		break;
	}
	case 1:
	{
		count = def.NumPatrolLocations();
		break;
	}
	case 2:
	{
		count = def.NumSequenceLocations();
		break;
	}
	}

	for ( size_t i=0; i<count; i++ )
	{
		TString name;
		
		if ( selectedAI!=2 )
		{
			name = "Marker " + Int2Str(i+1);
		}
		else
		{
			name = "Sequence " + Int2Str(i+1);
		}
		ComboBoxAddString( ddMarkers, name.c_str() );

		if ( selectedAI!=2 )
		{
			ComboBoxAddString( ddConnection1, name.c_str() );
		}
		if ( selectedAI==0 )
		{
			ComboBoxAddString( ddConnection2, name.c_str() );
			ComboBoxAddString( ddConnection3, name.c_str() );
			ComboBoxAddString( ddConnection4, name.c_str() );
		}
	}
};


void BotEditor::AddBotMarker( void )
{
	TDefFile& def = app.def;
	TString name;

	int prev = DropDownBoxGetSelection( ddMarkers );

	switch ( selectedAI )
	{
	case 0:
	{
		name = "Marker " + Int2Str(def.NumLocations()+1);
		float y = app.GetLandscapeY( app.lx,app.lz );
		app.ly = y;
		def.AddAILocation( prev, app.lx, y, app.lz );
		ComboBoxAddString( ddMarkers, name.c_str() );
		DropDownBoxSetSelection( ddMarkers, def.NumLocations()-1 );
		break;
	}
	case 1:
	{
		name = "Marker " + Int2Str(def.NumPatrolLocations()+1);
		float y = app.GetLandscapeY( app.lx,app.lz );
		app.ly = y;
		def.AddAIPatrolLocation( prev, app.lx, y, app.lz );
		ComboBoxAddString( ddMarkers, name.c_str() );
		DropDownBoxSetSelection( ddMarkers, def.NumPatrolLocations()-1 );
		break;
	}
	case 2:
	{
		name = "Sequence " + Int2Str(def.NumSequenceLocations()+1);
		float y = app.GetLandscapeY( app.lx,app.lz );
		app.ly = y;
		def.AddSequenceLocation( app.lx, y, app.lz );
		ComboBoxAddString( ddMarkers, name.c_str() );
		DropDownBoxSetSelection( ddMarkers, def.NumSequenceLocations()-1 );
		break;
	}
	}
	if ( selectedAI!=2 )
	{
		ComboBoxAddString( ddConnection1, name.c_str() );
	}
	if ( selectedAI==0 )
	{
		ComboBoxAddString( ddConnection2, name.c_str() );
		ComboBoxAddString( ddConnection3, name.c_str() );
		ComboBoxAddString( ddConnection4, name.c_str() );
	}

	changed = true;
};


void BotEditor::RemoveBotMarker( void )
{
	TDefFile& def = app.def;

	switch ( selectedAI )
	{
	case 0:
	{
		if ( currentMarker>=0 )
		{
			ComboBoxDeleteString( ddMarkers, currentMarker );
			ComboBoxDeleteString( ddConnection1, currentMarker );
			ComboBoxDeleteString( ddConnection2, currentMarker );
			ComboBoxDeleteString( ddConnection3, currentMarker );
			ComboBoxDeleteString( ddConnection4, currentMarker );

			def.RemoveAILocation( currentMarker );
			if ( currentMarker>0 && size_t(currentMarker)>=def.NumLocations() )
			{
				currentMarker--;
			}
			else if ( currentMarker==0 && def.NumLocations()==0 )
			{
				currentMarker = -1;
			}
			if ( currentMarker>=0 )
			{
				def.GetAILocation( currentMarker, app.lx,app.ly,app.lz );
				DropDownBoxSetSelection( ddMarkers, currentMarker );
			}
			changed = true;
		}
		break;
	}
	case 1:
	{
		if ( currentMarker>=0 )
		{
			ComboBoxDeleteString( ddMarkers, currentMarker );
			ComboBoxDeleteString( ddConnection1, currentMarker );
			ComboBoxDeleteString( ddConnection2, currentMarker );

			def.RemoveAIPatrolLocation( currentMarker );
			if ( currentMarker>0 && size_t(currentMarker)>=def.NumPatrolLocations() )
			{
				currentMarker--;
			}
			else if ( currentMarker==0 && def.NumPatrolLocations()==0 )
			{
				currentMarker = -1;
			}
			if ( currentMarker>=0 )
			{
				def.GetAIPatrolLocation( currentMarker, app.lx,app.ly,app.lz );
				DropDownBoxSetSelection( ddMarkers, currentMarker );
			}
			changed = true;
		}
		break;
	}
	case 2:
	{
		if ( currentMarker>=0 )
		{
			ComboBoxDeleteString( ddMarkers, currentMarker );

			def.RemoveSequenceLocation( currentMarker );
			if ( currentMarker>0 && size_t(currentMarker)>=def.NumSequenceLocations() )
			{
				currentMarker--;
			}
			else if ( currentMarker==0 && def.NumSequenceLocations()==0 )
			{
				currentMarker = -1;
			}
			if ( currentMarker>=0 )
			{
				def.GetSequenceLocation( currentMarker, app.lx,app.ly,app.lz );
				DropDownBoxSetSelection( ddMarkers, currentMarker );
			}
			changed = true;
		}
		break;
	}
	}
};


void BotEditor::Timer( void )
{
	TDefFile& def = app.def;

	bool ch = ButtonGetCheck( cbShowMarkers );
	if ( ch!=showMarkers )
	{
		showMarkers = ch;
		ButtonSetCheck( cbShowMarkers, showMarkers );
		app.ShowBotMarkers( showMarkers );
	}

	int marker = DropDownBoxGetSelection( ddMarkers );
	if ( marker!=currentMarker && marker>=0 )
	{
		TConnections conn;
		currentMarker = marker;

		switch ( selectedAI )
		{
		case 0:
		{
			conn = def.Connection(currentMarker);
			break;
		}
		case 1:
		{
			conn = def.PatrolConnection(currentMarker);
			break;
		}
		case 2:
		{
			conn = def.SequenceConnection(currentMarker);
			break;
		}
		}

		app.lx = conn.x;
		app.ly = conn.y;
		app.lz = conn.z;

		if ( selectedAI!=2 )
		{
			if ( conn.numConnections>0 )
				DropDownBoxSetSelection( ddConnection1, conn.neighbours[0]+1 );
			else
				DropDownBoxSetSelection( ddConnection1, 0 );

			if ( conn.numConnections>1 )
				DropDownBoxSetSelection( ddConnection2, conn.neighbours[1]+1 );
			else
				DropDownBoxSetSelection( ddConnection2, 0 );

			if ( conn.numConnections>2 )
				DropDownBoxSetSelection( ddConnection3, conn.neighbours[2]+1 );
			else
				DropDownBoxSetSelection( ddConnection3, 0 );

			if ( conn.numConnections>3 )
				DropDownBoxSetSelection( ddConnection4, conn.neighbours[3]+1 );
			else
				DropDownBoxSetSelection( ddConnection4, 0 );
		}
	}

	if ( marker>=0 )
	{
		TConnections conn;

		switch ( selectedAI )
		{
		case 0:
		{
			conn = def.Connection(currentMarker);
			break;
		}
		case 1:
		{
			conn = def.PatrolConnection(currentMarker);
			break;
		}
		case 2:
		{
			conn = def.SequenceConnection(currentMarker);
			break;
		}
		}

		// -1 because item 0 in the list is "none"
		int c1 = DropDownBoxGetSelection( ddConnection1 ) - 1;
		int c2 = DropDownBoxGetSelection( ddConnection2 ) - 1;
		int c3 = DropDownBoxGetSelection( ddConnection3 ) - 1;
		int c4 = DropDownBoxGetSelection( ddConnection4 ) - 1;

		if ( selectedAI!=2 )
		if ( c1!=currentc1 || c2!=currentc2 ||
			 c3!=currentc3 || c4!=currentc4 )
		{
			currentc1 = c1;
			currentc2 = c2;
			currentc3 = c3;
			currentc4 = c4;

			switch ( selectedAI )
			{
			case 0:
			{
				conn = def.Connection(marker);
				break;
			}
			case 1:
			{
				conn = def.PatrolConnection(marker);
				break;
			}
			}
 
			size_t cntr = 0;
			if ( c1>=0 )
				conn.neighbours[cntr++] = c1;
			if ( c2>=0 )
				conn.neighbours[cntr++] = c2;
			if ( c3>=0 )
				conn.neighbours[cntr++] = c3;
			if ( c4>=0 )
				conn.neighbours[cntr++] = c4;
	
			conn.numConnections = cntr;

			switch ( selectedAI )
			{
			case 0:
			{
				def.Connection(marker,conn);
				break;
			}
			case 1:
			{
				def.PatrolConnection(marker,conn);
				break;
			}
			}
			changed = true;
		}
	}
};


HWND BotEditor::Window( void ) const
{
	return dialog;
};

void BotEditor::SetPos( int x, int y )
{
	RECT r;
	::GetWindowRect( dialog, &r );
	int w = r.right - r.left;
	int h = r.bottom - r.top;
	::MoveWindow( dialog, x,y,w,h,TRUE );
};

bool BotEditor::Changed( void ) const
{
	return changed;
};

void BotEditor::Changed( bool _changed )
{
	changed = _changed;
};

//==========================================================================

LRESULT CALLBACK BotEditorProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		
		case WM_INITDIALOG:
			{
				// get all dialog items
				_ptr->ddMarkers = GetDlgItem( hwnd, IDC_MARKERS );

				_ptr->cbShowMarkers = GetDlgItem( hwnd, IDC_SHOW );

				_ptr->butAddMarker = GetDlgItem( hwnd, IDC_ADDLINEARMARKER );
				_ptr->butRemoveMarker = GetDlgItem( hwnd, IDC_REMOVELINEARMARKER );

				_ptr->ddConnection1 = GetDlgItem( hwnd, IDC_CONNECTION1 );
				_ptr->ddConnection2 = GetDlgItem( hwnd, IDC_CONNECTION2 );
				_ptr->ddConnection3 = GetDlgItem( hwnd, IDC_CONNECTION3 );
				_ptr->ddConnection4 = GetDlgItem( hwnd, IDC_CONNECTION4 );

				_ptr->radioAI1 = GetDlgItem( hwnd, IDC_AI1 );
				_ptr->radioAI2 = GetDlgItem( hwnd, IDC_AI2 );
				_ptr->radioAI3 = GetDlgItem( hwnd, IDC_AI3 );

				ButtonSetCheck( _ptr->radioAI1, true );

				RECT r;
				GetWindowRect(hwnd,&r);
				SetWindowPos( hwnd, NULL, r.left+int(_ptr->app.Width()), r.top, 
							  r.right-r.left, r.bottom-r.top, SWP_NOZORDER);

				// add objects to drop down
				SetControlHeight( _ptr->ddMarkers, 250 );
				SetControlHeight( _ptr->ddConnection1, 100 );
				SetControlHeight( _ptr->ddConnection2, 100 );
				SetControlHeight( _ptr->ddConnection3, 100 );
				SetControlHeight( _ptr->ddConnection4, 100 );

				ButtonSetCheck( _ptr->cbShowMarkers, _ptr->showMarkers );

				::SetTimer( hwnd, 1, 100, NULL );
				return 0;
			}

		case WM_TIMER:
			{
				if ( _ptr!=NULL )
					_ptr->Timer();
				return 0;
			}

		case WM_COMMAND:
			{
				if ( HIWORD(wParam)==BN_CLICKED )
				{
					if ( LOWORD(wParam)==IDC_ADDLINEARMARKER )
					{
						_ptr->AddBotMarker();
					}
					else if ( LOWORD(wParam)==IDC_REMOVELINEARMARKER )
					{
						_ptr->RemoveBotMarker();
					}
					else if ( LOWORD(wParam)==IDC_AI1 )
					{
						_ptr->SetAICheck( 0 );
					}
					else if ( LOWORD(wParam)==IDC_AI2 )
					{
						_ptr->SetAICheck( 1 );
					}
					else if ( LOWORD(wParam)==IDC_AI3 )
					{
						_ptr->SetAICheck( 2 );
					}
				}
				return 1;
			}

		default:
			{
				return 0;
			}
	}
	return 0;
};

//==========================================================================


