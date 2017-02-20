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
#include <landscape/mapEditorWindow.h>
#include <landscape/heightEditorWindow.h>

//==========================================================================

MapEditor* MapEditor::ptr = NULL;

#define _ptr MapEditor::ptr

//==========================================================================

MapEditor::MapEditor( TApp& _app, HINSTANCE _instance, HWND _parent,
					  ObjectEditor* _objEdit, HeightEditor* _heightEditor,
					  TVehicleList& _vehicleList )
	: app(_app),
	  parent( _parent ),
	  instance( _instance ),
	  objEdit( _objEdit ),
	  heightEditor( _heightEditor ),
	  vehicleList( _vehicleList )
{
	ptr = this;

	insideCriticalSection = false;

	modified = false;

	currentEdit = -1;
	currentCreatePlant = -1;
	currentCreateVehicle = -1;
	currentCreatePlane = -1;
	currentCreateStructure = -1;
	currentCreateTeam = -1;
	currentCreateMisc = -1;

	saveEnabled = true;
	deleteEnabled = true;
	editControlsEnabled = false;

	scale = 100;
	water = 10;

	// load strip file
	TPersist file(fileRead);
	PreCond( file.FileOpen( "data\\textures\\strips.txt" ) );
	TString temp;
	numStrips = 0;
	while ( file.ReadLine(temp) && numStrips<256 )
	{
		stripDescription[numStrips] = temp.GetItem('=',0);
		stripName[numStrips] = temp.GetItem('=',1);
		if ( stripDescription[numStrips].length()>0 && 
			 stripName[numStrips].length()>0 )
			numStrips++;
	}
	file.FileClose();
	PostCond( numStrips>0 );

	// load bump file
	PreCond( file.FileOpen( "data\\textures\\bumps.txt" ) );
	numBumps = 0;
	while ( file.ReadLine(temp) && numBumps<256 )
	{
		bumpDescription[numBumps] = temp.GetItem('=',0);
		bumpName[numBumps] = temp.GetItem('=',1);
		if ( bumpDescription[numBumps].length()>0 && 
			 bumpName[numBumps].length()>0 )
			numBumps++;
	}
	PostCond( numBumps>0 );

	dialog = CreateDialog( _instance, MAKEINTRESOURCE(IDD_MAPEDITOR),
						   parent, (DLGPROC)MapEditorProc );
};


MapEditor::~MapEditor( void )
{
};


void MapEditor::BeginCriticalSection( void )
{
	insideCriticalSection = true;
};


void MapEditor::EndCriticalSection( void )
{
	insideCriticalSection = false;
};


void MapEditor::DoMenu( int cmd )
{
	TString mapName;
	TString creator;
	TString binName;
	TString strip;
	TString bump;
	TDefFile& def = app.def;

	char temp[256];
	EditGetText( _ptr->editMapName, temp, 256 );
	mapName = temp;
	EditGetText( _ptr->editCreator, temp, 256 );
	creator = temp;
	EditGetText( _ptr->editBinFile, temp, 256 );
	binName = temp;

	int stripItem = DropDownBoxGetSelection( _ptr->ddStrip );
	if ( stripItem>=0 )
		strip = stripName[stripItem];

	int bumpItem = DropDownBoxGetSelection( _ptr->ddBump );
	if ( bumpItem>=0 )
		bump = bumpName[bumpItem];

	if ( cmd==IDC_QUIT )
	{
		app.Finished(true);
	}
	if ( cmd==ID_HELP_MANUAL )
	{
		::ShellExecute( NULL, "open", "WorldBuilder.pdf", "", "", SW_SHOW );
	}
	if ( cmd==IDC_LOAD )
	{
		LoadDefn();
	}
	if ( cmd==IDC_NEW )
	{
		bool doit = true;
		if ( modified || objEdit->Modified() )
		if ( ::MessageBox( dialog, "Are you sure you want to start a new map?\nThis will discard any unsaved changes",
						   "Warning", MB_YESNO | MB_ICONWARNING )!=IDYES )
		{
			doit = false;
		}
		if ( doit )
		{
			TString binName;
			if ( app.LoadBinAs( binName ) )
			{
				NewIsland( binName );
			}
		}
	}
	if ( cmd==IDC_SAVE ||
		 cmd==IDC_SAVEAS )
	{
		if ( mapName.length()==0 ||
			 creator.length()==0 ||
			 binName.length()==0 ||
			 strip.length()==0 ||
			 bump.length()==0 )
		{
			TString msg;
			msg = "Error\n\nYou must enter a map name,\n";
			msg = msg + "the name of the creator of this map,\n";
			msg = msg + "the filename of a .bmp file,\n";
			msg = msg + "and select a strip\n";
			MessageBox( parent, msg.c_str(),
						"Error", MB_OK | MB_ICONERROR );
		}
		else
		{
			def.Creator( creator );
			def.BMPName( binName );
			def.BMPStrip( strip );
			def.Material( bump );
			def.IslandName( mapName );

			if ( cmd==IDC_SAVE )
				app.SaveDefn();
			else
				app.SaveDefnAs();
		}
	}
};


void MapEditor::NewIsland( const TString& binName )
{
	TString path, fname;
	SplitPath( binName, path, fname );
	EditPrintSingle( _ptr->editBinFile, fname.c_str() );
	EditPrintSingle( _ptr->editMapName, "Untitled" );
	EditPrintSingle( _ptr->editCreator, "Unknown" );

	DropDownBoxSetSelection( _ptr->ddStrip, 0 );
	DropDownBoxSetSelection( _ptr->ddBump, 0 );
	DropDownBoxSetSelection( _ptr->ddTOD, 0 );
	DropDownBoxSetSelection( _ptr->ddSoundTrack, 0 );

	// clear map items & app
	app.currentObjectInUse = false;
	TDefFile& def = app.def;
	ComboBoxDeleteStrings( ddMapItems );
	def.New( fname, "Untitled", "land1.bmp", "strip.bmp", "bump1.jpg", 10 );
	TString errStr;
	if ( !app.NewIsland(path,path+"\\textures",true,errStr) )
	{
		::MessageBox( parent, errStr.c_str(), "Error", MB_OK | MB_ICONERROR );
	}
};


void MapEditor::LoadDefn( void )
{
	TString path, fname;

	// clear map items & app
	app.currentObjectInUse = false;
	TDefFile& def = app.def;
	ComboBoxDeleteStrings( ddMapItems );

	TString defname;
	if ( def.LoadBinaryAs( parent, defname ) )
	{
		SplitPath( defname, path, fname );
		TString errStr;
		if ( def.LoadBinary( fname, path, errStr, false ) )
		{
			SetMapInfo( def );
//			if ( !app.NewIsland(path,path+"\\textures",true,errStr) )
//			{
//				::MessageBox( parent, errStr.c_str(), "Error", MB_OK | MB_ICONERROR );
//			}
			if ( !app.LoadObject( fname, def, errStr ) )
			{
				::MessageBox( parent, errStr.c_str(), "Error", MB_OK | MB_ICONERROR );
			}
		}
		else
		{
			::MessageBox( parent, errStr.c_str(), "Error", MB_OK | MB_ICONERROR );
		}

	};
};

void MapEditor::SetCurrentEdit( int edit )
{
	DropDownBoxSetSelection( ddMapItems, edit );
}

void MapEditor::WaterLevel( size_t _water )
{
	water = _water;
	SliderSetPos( sliderWater, _water );
};

float MapEditor::WaterLevel( void ) const
{
	return float(app.def.WaterLevel()) * 0.75f;
};

void MapEditor::Timer( void )
{
	if ( insideCriticalSection )
		return;

	TDefFile& def = app.def;

	bool changed = false;

	int sc = SliderGetPos( sliderScale );
	if ( sc!=int(scale) )
	{
		scale = float(sc);
		app.Scale( scale/100.0f );
	}

	int wl = SliderGetPos( sliderWater );
	if ( wl!=int(water) )
	{
		water = wl;
	}

	int tod = DropDownBoxGetSelection( ddTOD );
	if ( tod!=int(def.TimeOfDay()) && tod>=0 )
	{
		def.TimeOfDay( tod );
	}

	int soundTrack = DropDownBoxGetSelection( ddSoundTrack );
	if ( soundTrack!=int(def.SoundTrack()) && soundTrack>=0 )
	{
		def.SoundTrack( soundTrack );
	}

	bool enabled = true;
	char buf[256];
	EditGetText( editMapName, buf, 256 );
	if ( buf[0]==0 )
		enabled = false;
	EditGetText( editCreator, buf, 256 );
	if ( buf[0]==0 )
		enabled = false;
	EditGetText( editBinFile, buf, 256 );
	if ( buf[0]==0 )
		enabled = false;
	if ( DropDownBoxGetSelection( ddStrip )==-1 )
		enabled = false;
	if ( DropDownBoxGetSelection( ddBump )==-1 )
		enabled = false;
	if ( def.NumLandscapeObjects()==0 )
		enabled = false;
	if ( saveEnabled && !enabled )
	{
		saveEnabled = false;
		EnableControl( butSave, saveEnabled );
		EnableControl( butSaveAs, saveEnabled );
	}
	else if ( !saveEnabled && enabled )
	{
		saveEnabled = true;
		EnableControl( butSave, saveEnabled );
		EnableControl( butSaveAs, saveEnabled );
	}

	if ( deleteEnabled && !app.currentObjectInUse )
	{
		deleteEnabled = false;
		EnableControl( butDelete, deleteEnabled );
	}
	else if ( !deleteEnabled && app.currentObjectInUse )
	{
		deleteEnabled = true;
		EnableControl( butDelete, deleteEnabled );
	}

	
	// map object selection changed?
	TLandscapeObject& co = app.currObj;

	int ce = DropDownBoxGetSelection( ddMapItems );
	if ( ce!=currentEdit && ce>=0 )
	{
		currentEdit = ce;

		objEdit->CurrentScale( -1 );
		objEdit->CurrentRotation( -1 );

		if ( app.currentObjectInUse )
			def.LandscapeObjects(app.currentEdit, app.currObj);
		app.currentObjectInUse = true;
		app.currentEdit = currentEdit;
		app.currObj = def.LandscapeObjects(currentEdit);
		app.currObj.GetScale( app.scaleX,app.scaleY,app.scaleZ );
		app.currObj.GetRotation( app.rotX,app.rotY,app.rotZ );
		app.currObj.GetTranslation( app.lx, app.ly, app.lz );

		objEdit->SelectionChanged( currentEdit );
	}
	else if ( ce!=currentEdit && ce==-1 )
	{
		currentEdit = -1;
		DropDownBoxSetSelection( ddMapItems, -1 );
		app.currentObjectInUse = false;
	}

	// create object selection changed?
	int cc = DropDownBoxGetSelection( ddPlants );
	if ( cc!=currentCreatePlant )
	{
		currentCreatePlant = cc;
		if ( cc>=0 )
		{
			app.SetCurrentObject( plants[cc] );
		}
	}
	cc = DropDownBoxGetSelection( ddVehicles );
	if ( cc!=currentCreateVehicle )
	{
		currentCreateVehicle = cc;
		if ( cc>=0 )
		{
			app.SetCurrentObject( vehicles[cc] );
		}
	}
	cc = DropDownBoxGetSelection( ddPlanes );
	if ( cc!=currentCreatePlane )
	{
		currentCreatePlane = cc;
		if ( cc>=0 )
		{
			app.SetCurrentObject( planes[cc] );
		}
	}
	cc = DropDownBoxGetSelection( ddStructures );
	if ( cc!=currentCreateStructure )
	{
		currentCreateStructure = cc;
		if ( cc>=0 )
		{
			app.SetCurrentObject( structures[cc] );
		}
	}
	cc = DropDownBoxGetSelection( ddTeams );
	if ( cc!=currentCreateTeam )
	{
		currentCreateTeam = cc;
		if ( cc>=0 )
		{
			app.SetCurrentObject( teams[cc] );
		}
	}
	cc = DropDownBoxGetSelection( ddMisc );
	if ( cc!=currentCreateMisc )
	{
		currentCreateMisc = cc;
		if ( cc>=0 )
		{
			app.SetCurrentObject( misc[cc] );
		}
	}

	// enable/disable controls
	if ( !app.currentObjectInUse && editControlsEnabled )
	{
		DropDownBoxSetSelection( ddMapItems, -1 );
		editControlsEnabled = false;
		changed = true;
	}
	else if ( app.currentObjectInUse && !editControlsEnabled )
	{
		editControlsEnabled = true;
		changed = true;
	}
	if ( changed )
	{
		objEdit->SelectionChanged( currentEdit, true);
	}
};


void MapEditor::AddObject( float x, float y, float z, float scale, float yoffset )
{
	TDefFile& def = app.def;
	TLandscapeObject currObj;

	TString objName;
	TCompoundObject* obj = app.GetCurrentObject(objName);
	currObj.Object( obj );
	currObj.SetTranslation( x,y+yoffset,z );
	currObj.SetScale( scale, scale, scale );
	currObj.Name( objName );

	def.AddLandscapeObject( currObj );

	objEdit->SetCheckBoxes( true, false );

	TString name = currObj.Name();
	name = name + ":" + Int2Str(def.NumLandscapeObjects());
	ComboBoxAddString( ddMapItems, name.c_str() );
	DropDownBoxSetSelection( ddMapItems, def.NumLandscapeObjects()-1 );

	modified = true;
}


void MapEditor::AddObject( size_t type )
{
	TDefFile& def = app.def;

	TString objName;
	switch ( type )
	{
	case IDC_ADDPLANT:
		{
			if ( currentCreatePlant>=0 )
			{
				objName = plants[currentCreatePlant];
			}
			break;
		}
	case IDC_ADDVEHICLE:
		{
			if ( currentCreateVehicle>=0 )
			{
				objName = vehicles[currentCreateVehicle];
			}
			break;
		}
	case IDC_ADDPLANE:
		{
			if ( currentCreatePlane>=0 )
			{
				objName = planes[currentCreatePlane];
			}
			break;
		}
	case IDC_ADDSTRUCTURE:
		{
			if ( currentCreateStructure>=0 )
			{
				objName = structures[currentCreateStructure];
			}
			break;
		}
	case IDC_ADDMISC:
		{
			if ( currentCreateMisc>=0 )
			{
				objName = misc[currentCreateMisc];
			}
			break;
		}
	case IDC_ADDTEAM:
		{
			if ( currentCreateTeam>=0 )
			{
				objName = teams[currentCreateTeam];
			}
			break;
		}
	}

	TLandscapeObject& currObj = app.currObj;
	if ( app.currentObjectInUse )
	{
		def.LandscapeObjects(app.currentEdit, currObj);
		currObj.GetTranslation(app.lx,app.ly,app.lz); // get current object pos
	}

	if ( objName.length()>0 )
	{
		objEdit->SetCheckBoxes( true, false );

		app.scaleX = 1;
		app.scaleY = 1;
		app.scaleZ = 1;
		app.rotX = 0;
		app.rotY = 0;
		app.rotZ = 0;
		objEdit->SetScale( 1,1,1 );
		currObj.Object( app.GetObjectByName(objName,0,0) );
		currObj.Name( objName );
		currObj.SetTranslation( app.lx,app.ly,app.lz );
		app.currentEdit = def.NumLandscapeObjects();
		def.AddLandscapeObject( currObj );
		app.currentObjectInUse = true;

		TString name = currObj.Name();
		name = name + ":" + Int2Str(def.NumLandscapeObjects());
		ComboBoxAddString( ddMapItems, name.c_str() );
		DropDownBoxSetSelection( ddMapItems, def.NumLandscapeObjects()-1 );

		if ( TLandscapeObject::IsV2( objName ) )
		{
			TString bflag = "blackflag.obj";

			TCompoundObject* obj = app.GetObjectByName(bflag,0,0);
			PostCond( obj!=NULL );

			TLandscapeObject markerObj;
			markerObj.Object( obj );
			markerObj.SetTranslation( app.lx,app.ly,app.lz );
			markerObj.SetScale( 1,1,1 );

			TString dropDown = name + " target marker";
			markerObj.Name( bflag );

			def.AddLandscapeObject( markerObj );
			ComboBoxAddString( ddMapItems, dropDown.c_str() );
			DropDownBoxSetSelection( ddMapItems, def.NumLandscapeObjects()-1 );
		}

		modified = true;
	}
};


void MapEditor::DeleteObject( bool checkV2Marker )
{
	TDefFile& def = app.def;

	TLandscapeObject obj = def.LandscapeObjects(app.currentEdit);

	if ( checkV2Marker )
	{
		if ( obj.Name().contains("blackflag.obj") )
		{
			TString msg = "You can't delete a missile's target marker\n";
			msg = msg + "remove the missile it belongs to\n";
			msg = msg + "instead.";
			::MessageBox( dialog, msg.c_str(), "Error", MB_OK | MB_ICONERROR );
			return;
		}
	}

	// remove from combo box
	ComboBoxDeleteString( ddMapItems, app.currentEdit );

	for ( size_t i=app.currentEdit; i<(def.NumLandscapeObjects()-1); i++ )
	{
		def.LandscapeObjects(i, def.LandscapeObjects(i+1));
	}
	def.NumLandscapeObjects( def.NumLandscapeObjects()-1 );
	if ( def.NumLandscapeObjects()==0 )
	{
		app.currentObjectInUse = false;
	}
	else
	{
		BeginCriticalSection();

		if ( app.currentEdit<def.NumLandscapeObjects() )
		{
			app.currObj = def.LandscapeObjects(app.currentEdit);
			DropDownBoxSetSelection( ddMapItems, app.currentEdit );
		}
		else
		{
			app.currentEdit = def.NumLandscapeObjects()-1;
			app.currObj = def.LandscapeObjects(app.currentEdit);
			DropDownBoxSetSelection( ddMapItems, app.currentEdit );
		}
		app.currObj.GetScale( app.scaleX,app.scaleY,app.scaleZ );
		app.currObj.GetRotation( app.rotX,app.rotY,app.rotZ );
		currentEdit = -1;

		EndCriticalSection();
	}

	// if we just delete a V2 rocket - delete its marker too
	if ( obj.Name().contains("v2.obj") )
	{
		DeleteObject( false );
	}

	modified = true;
};


HWND MapEditor::Window( void ) const
{
	return dialog;
};


void MapEditor::SetMapInfo( TDefFile& def )
{
	size_t i;

	modified = false;
	objEdit->Modified( false );

	// add map items
	ComboBoxClear( _ptr->ddMapItems );
	for ( i=0; i<def.NumLandscapeObjects(); i++ )
	{
		TLandscapeObject lo = def.LandscapeObjects(i);
		TString name = lo.Name();
		name = name + ":" + Int2Str(i+1);
		ComboBoxAddString( _ptr->ddMapItems, name.c_str() );
	}

	EditPrintSingle( editMapName, def.IslandName().c_str() );
	EditPrintSingle( editBinFile, def.BMPName().c_str() );

	int index = -1;
	for ( i=0; i<numStrips; i++ )
	{
		if ( stripName[i]==def.BMPStrip() )
			index = i;
	}
	PostCond( index!=-1 );
	DropDownBoxSetSelection( ddStrip, index );

	index = -1;
	for ( i=0; i<numBumps; i++ )
	{
		if ( bumpName[i]==def.Material() )
			index = i;
	}
	PostCond( index!=-1 );
	DropDownBoxSetSelection( ddBump, index );

	DropDownBoxSetSelection( ddTOD, def.TimeOfDay() );
	DropDownBoxSetSelection( ddSoundTrack, def.SoundTrack() );

	EditPrintSingle( editCreator, def.Creator().c_str() );
	WaterLevel( def.WaterLevel() );
};


void MapEditor::SetPos( int x, int y )
{
	RECT r;
	::GetWindowRect( dialog, &r );
	int w = r.right - r.left;
	int h = r.bottom - r.top;
	::MoveWindow( dialog, x,y,w,h,TRUE );
};

bool MapEditor::LoadBitmap( TString& fname )
{
	OPENFILENAME ofn;
	memset( &ofn,0,sizeof(OPENFILENAME) );
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = parent;
	ofn.lpstrFilter = "128x128 Bitmap rgb b&w file\0*.bmp\0\0";
	char temp[256];
	temp[0] = 0;
	ofn.lpstrFile = temp;
	ofn.lpstrTitle = "Load map definition file";
	ofn.nMaxFileTitle = 256;
	ofn.nMaxFile = 256;
	ofn.Flags = OFN_EXPLORER;
	if ( GetOpenFileName( &ofn )==TRUE )
	{
		fname = ofn.lpstrFile;
		return true;
	};
	return false;
};

bool MapEditor::Modified( void ) const
{
	return (modified || objEdit->Modified());
}

void MapEditor::Modified( bool _modified )
{
	modified = _modified;
	objEdit->Modified( modified );
}

//==========================================================================

LRESULT CALLBACK MapEditorProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
		{
			RECT r;
			GetWindowRect(hwnd,&r);

			// get all dialog items
			_ptr->ddPlants = GetDlgItem( hwnd, IDC_PLANTS );
			_ptr->ddVehicles = GetDlgItem( hwnd, IDC_VEHICLES );
			_ptr->ddPlanes = GetDlgItem( hwnd, IDC_PLANES );
			_ptr->ddStructures = GetDlgItem( hwnd, IDC_STRUCTURES );
			_ptr->ddTeams = GetDlgItem( hwnd, IDC_TEAMS );
			_ptr->ddMisc = GetDlgItem( hwnd, IDC_MISC );

			// time of day
			_ptr->ddTOD = GetDlgItem( hwnd, IDC_TOD );
			_ptr->ddSoundTrack = GetDlgItem( hwnd, IDC_SOUNDTRACK );

			_ptr->editRandomCount = GetDlgItem( hwnd, IDC_RANDOMCOUNT );
			_ptr->editRandomScale = GetDlgItem( hwnd, IDC_RANDOMSCALE );
			_ptr->editRandomYOffset = GetDlgItem( hwnd, IDC_RANDOMYOFFSET );

			_ptr->ddMapItems = GetDlgItem( hwnd, IDC_MAPITEMS );
			_ptr->editMapName = GetDlgItem( hwnd, IDC_MAPNAME );
			_ptr->editCreator = GetDlgItem( hwnd, IDC_CREATOR );
			_ptr->editBinFile = GetDlgItem( hwnd, IDC_BINFILE );
			_ptr->ddStrip = GetDlgItem( hwnd, IDC_STRIPFILE );
			_ptr->ddBump = GetDlgItem( hwnd, IDC_BUMPS );

			_ptr->sliderScale = GetDlgItem( hwnd, IDC_SCALE );
			_ptr->sliderWater = GetDlgItem( hwnd, IDC_WATER );

			SetControlHeight( _ptr->ddPlants, 250 );
			SetControlHeight( _ptr->ddVehicles, 250 );
			SetControlHeight( _ptr->ddStructures, 250 );
			SetControlHeight( _ptr->ddTeams, 250 );
			SetControlHeight( _ptr->ddMisc, 250 );
			SetControlHeight( _ptr->ddPlanes, 250 );

			SetControlHeight( _ptr->ddTOD, 100 );
			SetControlHeight( _ptr->ddSoundTrack, 100 );

			SetControlHeight( _ptr->ddMapItems, 250 );
			SetControlHeight( _ptr->ddStrip, 100 );
			SetControlHeight( _ptr->ddBump, 100 );

			SliderSetRange( _ptr->sliderScale, 1, 100 );
			SliderSetFrequency( _ptr->sliderScale, 10 );
			SliderSetPos( _ptr->sliderScale, size_t(_ptr->scale) );

			SliderSetRange( _ptr->sliderWater, 1, 100 );
			SliderSetFrequency( _ptr->sliderWater, 10 );
			SliderSetPos( _ptr->sliderWater, size_t(_ptr->water) );

			ComboBoxAddString( _ptr->ddTOD, "Daytime" );
			ComboBoxAddString( _ptr->ddTOD, "Nighttime" );
			ComboBoxAddString( _ptr->ddTOD, "Fog" );

			ComboBoxAddString( _ptr->ddSoundTrack, "None" );

			size_t i;
			for ( i=0; i<_ptr->soundTracks.Count(); i++ )
			{
				TString filename, title;
				_ptr->soundTracks.Get( i, filename, title );
				ComboBoxAddString( _ptr->ddSoundTrack, title.c_str() );
			}

			TApp& app = _ptr->app;
			size_t pc, vc, planec, sc, tc, mc;
			pc = vc = planec = sc = tc = mc = 0;
			for ( i=0; i<app.NumObjects(); i++ )
			{
				TVehicleSettings* vehicle = NULL;
				TString name = app.GetObjectName(i);
				if ( TLandscapeObject::IsPlant(name) )
				{
					ComboBoxAddString( _ptr->ddPlants, name.c_str() );
					_ptr->plants[pc++] = name;
				}
				else if ( TLandscapeObject::IsStructure(name) )
				{
					ComboBoxAddString( _ptr->ddStructures, name.c_str() );
					_ptr->structures[sc++] = name;
				}
				else if ( TLandscapeObject::IsTeam(name) )
				{
					TString properName = TLandscapeObject::FlagNameToTeamName(name);
					ComboBoxAddString( _ptr->ddTeams, properName.c_str() );
					_ptr->teams[tc++] = name;
				}
				else if ( (vehicle=_ptr->vehicleList.Find(name))!=NULL )
				{
					if ( vehicle->IsPlane() )
					{
						ComboBoxAddString( _ptr->ddPlanes, name.c_str() );
						_ptr->planes[planec++] = name;
					}
					else if ( vehicle->IsTank() )
					{
						ComboBoxAddString( _ptr->ddVehicles, name.c_str() );
						_ptr->vehicles[vc++] = name;
					}
					else 
					{
						ComboBoxAddString( _ptr->ddVehicles, name.c_str() );
						_ptr->vehicles[vc++] = name;
					}
				}
				else if ( !TLandscapeObject::IsV2TargetMarker(name) )
				{
					ComboBoxAddString( _ptr->ddMisc, name.c_str() );
					_ptr->misc[mc++] = name;
				}
			}

			DropDownBoxSetSelection( _ptr->ddPlants, 0 );
			DropDownBoxSetSelection( _ptr->ddVehicles, 0 );
			DropDownBoxSetSelection( _ptr->ddPlanes, 0 );
			DropDownBoxSetSelection( _ptr->ddTeams, 0 );
			DropDownBoxSetSelection( _ptr->ddMisc, 0 );
			DropDownBoxSetSelection( _ptr->ddStructures, 0 );

			for ( i=0; i<_ptr->numStrips; i++ )
			{
				ComboBoxAddString( _ptr->ddStrip, _ptr->stripDescription[i].c_str() );
			}
			for ( i=0; i<_ptr->numBumps; i++ )
			{
				ComboBoxAddString( _ptr->ddBump, _ptr->bumpDescription[i].c_str() );
			}

			_ptr->butSave = GetDlgItem( hwnd, IDC_SAVE );
			_ptr->butSaveAs = GetDlgItem( hwnd, IDC_SAVEAS );
			_ptr->butDelete = GetDlgItem( hwnd, IDC_DELETE );

			::SetTimer( hwnd, 1, 100, NULL );
			return 0;
		}

		case WM_TIMER:
		{
			if ( _ptr!=NULL )
				_ptr->Timer();

			return 0;
		}

		case WM_KEYDOWN:
		{
			TApp& app = _ptr->app;
			app.KeyDown( int(wParam) );
			break;
		}

		case WM_KEYUP:
		{
			TApp& app = _ptr->app;
			app.KeyUp( int(wParam) );
			break;
		}

		case WM_CHAR:
		{
			TApp& app = _ptr->app;
			app.KeyPress( int(wParam) );
			break;
		}

		case WM_COMMAND:
		{
			TApp& app = _ptr->app;
			TDefFile& def = app.def;

			if ( HIWORD(wParam)==BN_CLICKED )
			{
				TString mapName;
				TString creator;
				TString binName;
				TString strip;
				TString bump;
				TString errStr;
				int water;

				char temp[256];
				EditGetText( _ptr->editMapName, temp, 256 );
				mapName = temp;
				EditGetText( _ptr->editCreator, temp, 256 );
				creator = temp;
				EditGetText( _ptr->editBinFile, temp, 256 );
				binName = temp;
				int index = DropDownBoxGetSelection( _ptr->ddStrip );
				if ( index<0 )
					break;
				strip = _ptr->stripName[index];
				index = DropDownBoxGetSelection( _ptr->ddBump );
				if ( index<0 )
					break;
				bump = _ptr->bumpName[index];
				water = SliderGetPos( _ptr->sliderWater );

				if ( LOWORD(wParam)==IDC_REBUILDMAP )
				{
					def.IslandName( mapName );
					def.Creator( creator );
					def.BMPName( binName );
					def.BMPStrip( strip );
					def.Material( bump );
					def.WaterLevel( water );

					if ( !app.NewIsland( "data", "data\\textures", false, errStr ) )
					{
						::MessageBox( _ptr->parent, errStr.c_str(), "Error", MB_OK | MB_ICONERROR );
					}
					_ptr->modified = true;
				}
				if ( LOWORD(wParam)==IDC_ADDPLANT ||
					 LOWORD(wParam)==IDC_ADDVEHICLE ||
					 LOWORD(wParam)==IDC_ADDPLANE ||
					 LOWORD(wParam)==IDC_ADDSTRUCTURE ||
					 LOWORD(wParam)==IDC_ADDTEAM ||
					 LOWORD(wParam)==IDC_ADDMISC )
				{
					_ptr->AddObject(wParam);
					_ptr->modified = true;
				}
				if ( LOWORD(wParam)==IDC_LOADBITMAP )
				{
					TString fname;
					if ( _ptr->LoadBitmap( fname ) )
					{
						size_t index = fname.length();
						while ( index>0 && fname[index]!='\\' ) index--;
						if ( fname[index]=='\\' )
						{
							fname = fname.substr(index+1);
						}
						EditPrintSingle( _ptr->editBinFile, fname.c_str() );
					}
					_ptr->modified = true;
				}
				if ( LOWORD(wParam)==IDC_DELETE )
				{
					_ptr->modified = true;
					_ptr->DeleteObject();
				}
				if ( LOWORD(wParam)==IDC_RANDOMDISPERSE )
				{
					_ptr->modified = true;
					char buf[256];
					EditGetText( _ptr->editRandomCount, buf, 255 );
					int count = atoi(buf);
					EditGetText( _ptr->editRandomScale, buf, 255 );
					float scale = float(atof(buf));
					EditGetText( _ptr->editRandomYOffset, buf, 255 );
					float yoffset = float(atof(buf));

					TString objName;
					TCompoundObject* obj = _ptr->app.GetCurrentObject(objName);
					if ( count<=0 )
					{
						::MessageBox( hwnd, "Number of items must be greater than 0",
									  "Error", MB_OK | MB_ICONERROR );
					}
					else if ( scale<=0 )
					{
						::MessageBox( hwnd, "Scale must be greater than 0",
									  "Error", MB_OK | MB_ICONERROR );
					}
					else if ( obj==NULL )
					{
						::MessageBox( hwnd, "You must select an object to disperse",
									  "Error", MB_OK | MB_ICONERROR );
					}
					else
					{
						float x1,z1,x2,z2;
						_ptr->heightEditor->GetBoundingRect( x1,z1,x2,z2 );
						for ( size_t i=0; i<size_t(count); i++ )
						{
							bool ok = false;
							do
							{
								float x = x1 + (Random()*(x2-x1));
								float z = z1 + (Random()*(z2-z1));
								float sizeY = _ptr->app.currObj.SizeY();
								float y = _ptr->app.GetLandscapeY( x,z ) + sizeY;
//								if ( y>_ptr->WaterLevel() )
								{
									ok = true;
									_ptr->AddObject( x,y,z, scale, yoffset );
								}
							}
							while ( !ok );
						}
					}
				}
			}
			break;
		}

		default:
		{
			return 0; //(DefWindowProc(hwnd, message, wParam, lParam));
		}
	}
	return 0;
};

//==========================================================================


