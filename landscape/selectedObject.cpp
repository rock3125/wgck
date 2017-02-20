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
#include <landscape/selectedObject.h>

//==========================================================================

LRESULT CALLBACK ObjectEditorProc( HWND hwnd, UINT message, 
								   WPARAM wParam, LPARAM lParam );

ObjectEditor* ObjectEditor::ptr = NULL;

#define _ptr ObjectEditor::ptr

//==========================================================================

ObjectEditor::ObjectEditor( TApp& _app, HINSTANCE _instance, HWND _parent,
						    TVehicleList& _vehicleList )
	: app(_app),
	  parent( _parent ),
	  instance( _instance ),
	  vehicleList( _vehicleList )
{
	ptr = this;

	currentEdit = -1;

	addRotEnabled = true;
	addScaleEnabled = true;
	translationChanged = true;

	tx = 0;
	ty = 0;
	tz = 0;

	rotX = 0;
	rotY = 0;
	rotZ = 0;

	scaleX = 10;
	scaleY = 10;
	scaleZ = 10;

	destructable = false;
	armory = false;
	repairs = false;
	refuelStation = false;
	isaAI = false;

	editControlsEnabled = true;

	currentRotation = -1;
	currentScale = -1;
	characterType = -1;
	numPresetRotations = 0;
	numPresetScales = 0;

	LoadBinary( "mapEditorSettings.bin" );

	dialog = CreateDialog( _instance, MAKEINTRESOURCE(IDD_SELECTEDOBJECT),
						   parent, (DLGPROC)ObjectEditorProc );

	modified = false;
};

ObjectEditor::~ObjectEditor( void )
{
	SaveBinary( "mapEditorSettings.bin" );
};

void ObjectEditor::LoadBinary( const TString& fname )
{
	TPersist file(fileRead);

	if ( file.FileOpen( fname ) )
	{
		size_t i;

		file.FileRead( &numPresetRotations, sizeof(size_t) );
		file.FileRead( &numPresetScales, sizeof(size_t) );

		PostCond( numPresetRotations<kMaxEntries && numPresetScales<kMaxEntries );

		for ( i=0; i<numPresetRotations; i++ )
		{
			TString temp;
			char ch;
			do
			{
				file.FileRead(&ch,1);
				if ( ch>0 )
					temp = temp + TString(ch);
			}
			while ( ch!=0 );
			presetRotationNames[i] = temp;
			file.FileRead( presetRotations[i], sizeof(float)*3 );
		}
		for ( i=0; i<numPresetScales; i++ )
		{
			TString temp;
			char ch;
			do
			{
				file.FileRead(&ch,1);
				if ( ch>0 )
					temp = temp + TString(ch);
			}
			while ( ch!=0 );
			presetScaleNames[i] = temp;
			file.FileRead( presetScales[i], sizeof(float)*3 );
		}
		file.FileClose();
	}
};


void ObjectEditor::SaveBinary( const TString& fname )
{
	TPersist file(fileWrite);

	if ( file.FileOpen( fname ) )
	{
		size_t i;

		file.FileWrite( &numPresetRotations, sizeof(size_t) );
		file.FileWrite( &numPresetScales, sizeof(size_t) );

		for ( i=0; i<numPresetRotations; i++ )
		{
			file.FileWrite( presetRotationNames[i].c_str(), presetRotationNames[i].length()+1 );
			file.FileWrite( presetRotations[i], sizeof(float)*3 );
		}
		for ( i=0; i<numPresetScales; i++ )
		{
			file.FileWrite( presetScaleNames[i].c_str(), presetScaleNames[i].length()+1 );
			file.FileWrite( presetScales[i], sizeof(float)*3 );
		}
		file.FileClose();
	}
};


void ObjectEditor::SetTranslation( float lx, float ly, float lz )
{
	tx = lx;
	ty = ly;
	tz = lz;
	translationChanged = true;
};


void ObjectEditor::SetRotation( float rx, float ry, float rz )
{
	rotX = rx;
	rotY = ry;
	rotZ = rz;

	SliderSetPos( sliderXRot, size_t(rx) );
	SliderSetPos( sliderYRot, size_t(ry) );
	SliderSetPos( sliderZRot, size_t(rz) );
	DropDownBoxSetSelection( ddSavedRotations, -1 );
	currentRotation = -1;
	translationChanged = true;
};


void ObjectEditor::SetScale( float sx, float sy, float sz )
{
	scaleX = sx;
	scaleY = sy;
	scaleZ = sz;

	SliderSetPos( sliderXScale, sx );
	SliderSetPos( sliderYScale, sy );
	SliderSetPos( sliderZScale, sz );
	DropDownBoxSetSelection( ddSavedScales, -1 );
	currentScale = -1;
	translationChanged = true;
};

void ObjectEditor::Timer( void )
{
	TDefFile& def = app.def;
	bool changed = false;
	bool changeDisplay = false;

	if ( app.currentObjectInUse )
	{
		// enable / disable buttons
		char buf[256];
		EditGetText( editRotationName, buf, 256 );
		if ( buf[0]==0 && addRotEnabled )
		{
			addRotEnabled = false;
			EnableControl( butAddRot, addRotEnabled );
		}
		else if ( buf[0]!=0 && !addRotEnabled )
		{
			addRotEnabled = true;
			EnableControl( butAddRot, addRotEnabled );
		}

		EditGetText( editScaleName, buf, 256 );
		if ( buf[0]==0 && addScaleEnabled )
		{
			addScaleEnabled = false;
			EnableControl( butAddScale, addScaleEnabled );
		}
		else if ( buf[0]!=0 && !addScaleEnabled )
		{
			addScaleEnabled = true;
			EnableControl( butAddScale, addScaleEnabled );
		}

		// scale selection changed?
		TLandscapeObject& co = app.currObj;
		int cr = DropDownBoxGetSelection( ddSavedRotations );
		if ( cr!=currentRotation && cr>=0 )
		{
			currentRotation = cr;
			SliderSetPos( sliderXRot, size_t(presetRotations[currentRotation][0]) );
			SliderSetPos( sliderYRot, size_t(presetRotations[currentRotation][1]) );
			SliderSetPos( sliderZRot, size_t(presetRotations[currentRotation][2]) );
		}

		cr = DropDownBoxGetSelection( ddCharacter );
		if ( cr!=characterType && cr>=0 )
		{
			characterType = cr;
			DropDownBoxSetSelection( ddCharacter, characterType );
			app.currObj.CharacterType(characterType);
		}

		int ait = DropDownBoxGetSelection( ddAIType );
		if ( size_t(ait)!=co.AIType() && ait>=0 )
		{
			modified = true;
			co.AIType( ait );
		}

		ait = DropDownBoxGetSelection( ddAITeam );
		if ( size_t(ait)!=co.AITeam() && ait>=0 )
		{
			modified = true;
			co.AITeam( ait );
		}

		int gip = DropDownBoxGetSelection( ddGameImportance );
		if ( size_t(gip)!=co.GameImportance() && gip>=0 )
		{
			modified = true;
			co.GameImportance( gip );
		}

		int cs = DropDownBoxGetSelection( ddSavedScales );
		if ( cs!=currentScale && cs>=0 )
		{
			currentScale = cs;
			SliderSetPos( sliderXScale, presetScales[currentScale][0] );
			SliderSetPos( sliderYScale, presetScales[currentScale][1] );
			SliderSetPos( sliderZScale, presetScales[currentScale][2] );
		}

		// check destructable  
		if ( destructable!=ButtonGetCheck( checkDestructable ) )
		{
			changed = true;
			destructable = !destructable;
 			co.Destructable( destructable );
			if ( !destructable && isaAI )
			{
				isaAI = false;
				co.IsaAI( isaAI );
			}
		}

		// check armory
		if ( armory!=ButtonGetCheck( checkArmory ) )
		{
			changed = true;
			armory = !armory;
 			co.Armory( armory );
		}

		// check refuelStation
		if ( refuelStation!=ButtonGetCheck( checkRefuelStation ) )
		{
			changed = true;
			refuelStation = !refuelStation;
 			co.RefuelStation( refuelStation );
		}

		// check armory
		if ( repairs!=ButtonGetCheck( checkRepairs ) )
		{
			changed = true;
			repairs = !repairs;
 			co.Repairs( repairs );
		}

		// check isa AI
		if ( isaAI!=ButtonGetCheck( checkIsaAI ) )
		{
			changed = true;
			isaAI = !isaAI;
 			co.IsaAI( isaAI );
		}

		int rx = SliderGetPos( sliderXRot );
		if ( rx!=int(co.RX()) )
		{
			app.rotX = float(rx);
			co.RX(float(rx));
			changeDisplay = true;
		}

		int ry = SliderGetPos( sliderYRot );
		if ( ry!=int(co.RY()) )
		{
			app.rotY = float(ry);
			co.RY(float(ry));
			changeDisplay = true;
		}

		int rz = SliderGetPos( sliderZRot );
		if ( rz!=int(co.RZ()) )
		{
			app.rotZ = float(rz);
			co.RZ(float(rz));
			changeDisplay = true;
		}

		float xs;
		SliderGetPos( sliderXScale, xs );
		if ( xs!=co.ScaleX() )
		{
			app.scaleX = xs;
			co.ScaleX(xs);
			SliderSetPos( sliderXScale, xs );
			changeDisplay = true;
		}

		float ys;
		SliderGetPos( sliderYScale, ys );
		if ( ys!=co.ScaleY() )
		{
			app.scaleY = ys;
			co.ScaleY(ys);
			changeDisplay = true;
		}

		float zs;
		SliderGetPos( sliderZScale, zs );
		if ( zs!=co.ScaleZ() )
		{
			app.scaleZ = zs;
			co.ScaleZ(zs);
			changeDisplay = true;
		}

		if ( changeDisplay || translationChanged )
		{
			translationChanged = false;
			modified = true;

			EditPrintSingle( editXPos, FloatToString(1,4,co.TX()).c_str() );
			EditPrintSingle( editYPos, FloatToString(1,4,co.TY()).c_str() );
			EditPrintSingle( editZPos, FloatToString(1,4,co.TZ()).c_str() );

			EditPrintSingle( editXRot, FloatToString(1,4,co.RX()).c_str() );
			EditPrintSingle( editYRot, FloatToString(1,4,co.RY()).c_str() );
			EditPrintSingle( editZRot, FloatToString(1,4,co.RZ()).c_str() );

			EditPrintSingle( editXScale, FloatToString(1,4,co.ScaleX()).c_str() );
			EditPrintSingle( editYScale, FloatToString(1,4,co.ScaleY()).c_str() );
			EditPrintSingle( editZScale, FloatToString(1,4,co.ScaleZ()).c_str() );

			SliderSetPos( sliderXRot, size_t(co.RX()) );
			SliderSetPos( sliderYRot, size_t(co.RY()) );
			SliderSetPos( sliderZRot, size_t(co.RZ()) );

			SliderSetPos( sliderXScale, co.ScaleX() );
			SliderSetPos( sliderYScale, co.ScaleY() );
			SliderSetPos( sliderZScale, co.ScaleZ() );
		}

		// enable/disable controls
		if ( !app.currentObjectInUse && editControlsEnabled )
		{
			editControlsEnabled = false;
			changed = true;
		}
		else if ( app.currentObjectInUse && !editControlsEnabled )
		{
			editControlsEnabled = true;
			changed = true;
		}
	}
	if ( changed )
	{
		modified = true;
		SelectionChanged( currentEdit, true );
	}
};

void ObjectEditor::SetPos( int x, int y )
{
	RECT r;
	::GetWindowRect( dialog, &r );
	int w = r.right - r.left;
	int h = r.bottom - r.top;
	::MoveWindow( dialog, x,y,w,h,TRUE );
};

void ObjectEditor::SelectionChanged( int _currentEdit, bool forceChange )
{
	// map object selection changed?
	if ( (_currentEdit!=currentEdit && _currentEdit>=0) || forceChange )
	{
		currentEdit = _currentEdit;

		SetTranslation( app.lx, app.ly, app.lz );
		SetRotation( app.rotX, app.rotY, app.rotZ );
		SetScale( app.scaleX, app.scaleY, app.scaleZ );

		TString name = app.currObj.Name();
		TVehicleSettings* vs = vehicleList.Find( name );

		bool isStructure = TLandscapeObject::IsStructure(name);
		bool isFlag = TLandscapeObject::IsTeam(name);
		bool isV2 = TLandscapeObject::IsV2(name);

		bool isArtillery = false;
		bool isTank = false;
		bool isPlane = false;
		if ( vs!=NULL )
		{
			if ( vs->IsPlane() )
			{
				isPlane = true;
			}
			if ( vs->IsTank() )
			{
				isTank = true;
			}
			if ( vs->IsArtillery() || vs->IsFlak() || vs->IsMG42() )
			{
				isArtillery = true;
			}
		}

		if ( isV2 )
		{
			::SetWindowText( bombLabel, "Countdown(sec):" );
		}
		else
		{
			::SetWindowText( bombLabel, "Bombs:" );
		}

		currentScale = -1;
		currentRotation = -1;
		DropDownBoxSetSelection( ddSavedRotations, currentRotation );
		DropDownBoxSetSelection( ddSavedScales, currentScale );

		destructable = app.currObj.Destructable();
		ButtonSetCheck( checkDestructable, destructable );
		EnableControl( checkDestructable, editControlsEnabled&&(!isFlag) );

		EnableControl( checkArmory, editControlsEnabled&&isStructure );
		EnableControl( checkRepairs, editControlsEnabled&&isStructure );
		EnableControl( checkRefuelStation, editControlsEnabled&&isStructure );
		EnableControl( ddCharacter, editControlsEnabled&&isFlag );
		EnableControl( ddGameImportance, editControlsEnabled&&(!isFlag) );

		if ( editControlsEnabled && isFlag )
		{
			characterType = app.currObj.CharacterType();
			DropDownBoxSetSelection( ddCharacter, characterType );
		}
		else
		{
			DropDownBoxSetSelection( ddCharacter, -1 );
		}

		isaAI = app.currObj.IsaAI();
		EnableControl( checkIsaAI, editControlsEnabled&&(!isStructure)&&(!isFlag) );
		EnableControl( ddAIType, editControlsEnabled&&isaAI&&(!isStructure)&&(!isFlag) );
		EnableControl( ddAITeam, editControlsEnabled&&isaAI&&(!isStructure)&&(!isFlag) );

		if ( editControlsEnabled && isaAI )
		{
			DropDownBoxSetSelection( ddAIType, app.currObj.AIType() );
			DropDownBoxSetSelection( ddAITeam, app.currObj.AITeam() );
		}
		else
		{
			DropDownBoxSetSelection( ddAIType, -1 );
			DropDownBoxSetSelection( ddAITeam, -1 );
		}

		EnableControl( editBombs, editControlsEnabled&&(!isStructure)&&(!isTank)&&(!isArtillery)||isFlag||isPlane||isV2 );
		EnableControl( editShells, editControlsEnabled&&(!isStructure)||isFlag||isArtillery||isTank );

		armory = app.currObj.Armory();
		ButtonSetCheck( checkArmory, armory );

		repairs = app.currObj.Repairs();
		ButtonSetCheck( checkRepairs, repairs );

		refuelStation = app.currObj.RefuelStation();
		ButtonSetCheck( checkRefuelStation, refuelStation );

		ButtonSetCheck( checkIsaAI, isaAI );

		EditPrintSingle( editStrength, Int2Str(app.currObj.Strength()).c_str() );
		EditPrintSingle( editPoints, Int2Str(app.currObj.Points()).c_str() );

		if ( isV2 )
		{
			EditPrintSingle( editBombs, Int2Str(app.currObj.V2Countdown()).c_str() );
		}
		else
		{
			EditPrintSingle( editBombs, Int2Str(app.currObj.Bombs()).c_str() );
		}

		EditPrintSingle( editShells, Int2Str(app.currObj.Shells()).c_str() );

		DropDownBoxSetSelection( ddGameImportance, app.currObj.GameImportance() );

		EditPrintSingle( editXPos, FloatToString(1,4,app.lx).c_str() );
		EditPrintSingle( editYPos, FloatToString(1,4,app.ly).c_str() );
		EditPrintSingle( editZPos, FloatToString(1,4,app.lz).c_str() );
		tx = app.lx;
		ty = app.ly;
		tz = app.lz;

		EditPrintSingle( editXScale, FloatToString( 1,4,app.scaleX ).c_str() );
		EditPrintSingle( editYScale, FloatToString( 1,4,app.scaleY ).c_str() );
		EditPrintSingle( editZScale, FloatToString( 1,4,app.scaleZ ).c_str() );
		scaleX = app.scaleX;
		scaleY = app.scaleX;
		scaleZ = app.scaleX;

		EditPrintSingle( editXRot, FloatToString( 1,4,app.rotX ).c_str() );
		EditPrintSingle( editYRot, FloatToString( 1,4,app.rotY ).c_str() );
		EditPrintSingle( editZRot, FloatToString( 1,4,app.rotZ ).c_str() );
		rotX = app.rotX;
		rotY = app.rotY;
		rotZ = app.rotZ;

		SliderSetPos( sliderXScale, app.scaleX );
		SliderSetPos( sliderYScale, app.scaleY );
		SliderSetPos( sliderZScale, app.scaleZ );

		SliderSetPos( sliderXRot, size_t(app.rotX) );
		SliderSetPos( sliderYRot, size_t(app.rotY) );
		SliderSetPos( sliderZRot, size_t(app.rotZ) );
	}
	else
	{
		currentEdit = _currentEdit;
	}
}

void ObjectEditor::CurrentRotation( int _rotation )
{
	currentRotation = _rotation;
	DropDownBoxSetSelection( ddSavedRotations, currentRotation );
		currentScale = -1;
		DropDownBoxSetSelection( ddSavedScales, currentScale );
};

void ObjectEditor::CurrentScale( int _scale )
{
	currentScale = _scale;
	DropDownBoxSetSelection( ddSavedScales, currentScale );
};

void ObjectEditor::SetCheckBoxes( bool _destruct, bool _ai )
{
	destructable = _destruct;
	isaAI = _ai;
};

HWND ObjectEditor::Window( void ) const
{
	return dialog;
};

bool ObjectEditor::Modified( void ) const
{
	return modified;
}

void ObjectEditor::Modified( bool _modified )
{
	modified = _modified;
}

//==========================================================================

LRESULT CALLBACK ObjectEditorProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
		{
			size_t i;
			RECT r;
			GetWindowRect(hwnd,&r);

			PreCond( _ptr!=NULL );

			_ptr->ddSavedRotations = GetDlgItem( hwnd, IDC_SAVEDROTATIONS );
			_ptr->ddSavedScales = GetDlgItem( hwnd, IDC_SAVEDSCALES );

			for ( i=0; i<_ptr->numPresetScales; i++ )
			{
				ComboBoxAddString( _ptr->ddSavedScales, _ptr->presetScaleNames[i].c_str() ); 
			}
			for ( i=0; i<_ptr->numPresetRotations; i++ )
			{
				ComboBoxAddString( _ptr->ddSavedRotations, _ptr->presetRotationNames[i].c_str() );
			}

			_ptr->editXPos = GetDlgItem( hwnd, IDC_XPOS );
			_ptr->editYPos = GetDlgItem( hwnd, IDC_YPOS );
			_ptr->editZPos = GetDlgItem( hwnd, IDC_ZPOS );

			_ptr->editXRot = GetDlgItem( hwnd, IDC_XROT );
			_ptr->editYRot = GetDlgItem( hwnd, IDC_YROT );
			_ptr->editZRot = GetDlgItem( hwnd, IDC_ZROT );

			_ptr->sliderXRot = GetDlgItem( hwnd, IDC_XROTSLIDER );
			_ptr->sliderYRot = GetDlgItem( hwnd, IDC_YROTSLIDER );
			_ptr->sliderZRot = GetDlgItem( hwnd, IDC_ZROTSLIDER );

			_ptr->editRotationName = GetDlgItem( hwnd, IDC_ROTATIONSETTINGNAME );

			_ptr->editXScale = GetDlgItem( hwnd, IDC_XSCALE );
			_ptr->editYScale = GetDlgItem( hwnd, IDC_YSCALE );
			_ptr->editZScale = GetDlgItem( hwnd, IDC_ZSCALE );

			_ptr->sliderXScale = GetDlgItem( hwnd, IDC_XSCALESLIDER );
			_ptr->sliderYScale = GetDlgItem( hwnd, IDC_YSCALESLIDER );
			_ptr->sliderZScale = GetDlgItem( hwnd, IDC_ZSCALESLIDER );

			_ptr->editScaleName = GetDlgItem( hwnd, IDC_SCALESETTINGNAME );

			_ptr->butAddRot = GetDlgItem( hwnd, IDC_ADDROTATIONSETTING );
			_ptr->butAddScale = GetDlgItem( hwnd, IDC_ADDSCALESETTING );
			_ptr->butYOnLand = GetDlgItem( hwnd, IDC_HUGH );

			_ptr->checkDestructable = GetDlgItem( hwnd, IDC_INDESTRUCTABLE );
			_ptr->checkArmory = GetDlgItem( hwnd, IDC_ARMORY );
			_ptr->checkRefuelStation = GetDlgItem( hwnd, IDC_REFUEL );
			_ptr->checkRepairs = GetDlgItem( hwnd, IDC_REPAIRS );
			_ptr->checkIsaAI = GetDlgItem( hwnd, IDC_ISA_AI );
			_ptr->ddAIType = GetDlgItem( hwnd, IDC_AITYPE );
			_ptr->ddAITeam = GetDlgItem( hwnd, IDC_AITEAM );
			_ptr->ddGameImportance = GetDlgItem( hwnd, IDC_GAMEIMPORTANCE );
			_ptr->editStrength = GetDlgItem( hwnd, IDC_STRENGTHEDIT );
			
			_ptr->editPoints = GetDlgItem( hwnd, IDC_POINTS );
			_ptr->editBombs = GetDlgItem( hwnd, IDC_BOMBS );
			_ptr->editShells = GetDlgItem( hwnd, IDC_SHELLS );

			_ptr->bombLabel = GetDlgItem( hwnd, IDC_LABEL2 );

			// character settings
			_ptr->ddCharacter = GetDlgItem( hwnd, IDC_CHARACTER2 );

			SetControlHeight( _ptr->ddAIType, 100 );
			ComboBoxAddString( _ptr->ddAIType, "Guard Tank" );
			ComboBoxAddString( _ptr->ddAIType, "Attack Tank" );
			ComboBoxAddString( _ptr->ddAIType, "Patrol Tank" );
			ComboBoxAddString( _ptr->ddAIType, "Bomber Plane" );
			ComboBoxAddString( _ptr->ddAIType, "Fighter Plane" );
			ComboBoxAddString( _ptr->ddAIType, "Flak" );
			ComboBoxAddString( _ptr->ddAIType, "Artillery" );
			ComboBoxAddString( _ptr->ddAIType, "MG42" );

			SetControlHeight( _ptr->ddAITeam, 100 );
			ComboBoxAddString( _ptr->ddAITeam, "red team" );
			ComboBoxAddString( _ptr->ddAITeam, "green team" );
			ComboBoxAddString( _ptr->ddAITeam, "blue team" );
			ComboBoxAddString( _ptr->ddAITeam, "yellow team" );

			SetControlHeight( _ptr->ddGameImportance, 100 );
			ComboBoxAddString( _ptr->ddGameImportance, "n/a" );
			ComboBoxAddString( _ptr->ddGameImportance, "Must be destroyed" );
			ComboBoxAddString( _ptr->ddGameImportance, "Must not be destroyed" );
			
			SetControlHeight( _ptr->ddCharacter, 100 );
			ComboBoxAddString( _ptr->ddCharacter, "Soldier" );
			ComboBoxAddString( _ptr->ddCharacter, "Engineer" );
			ComboBoxAddString( _ptr->ddCharacter, "Saboteur" );
			ComboBoxAddString( _ptr->ddCharacter, "Special Forces" );

			// add objects to drop down
			SetControlHeight( _ptr->ddSavedRotations, 100 );
			SetControlHeight( _ptr->ddSavedScales, 100 );

			// scrollbars
			SliderSetRange( _ptr->sliderXRot, 0, 359 );
			SliderSetRange( _ptr->sliderYRot, 0, 359 );
			SliderSetRange( _ptr->sliderZRot, 0, 359 );

			SliderSetRange( _ptr->sliderXScale, 1, 300 );
			SliderSetRange( _ptr->sliderYScale, 1, 300 );
			SliderSetRange( _ptr->sliderZScale, 1, 300 );

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

			if ( HIWORD(wParam)==EN_KILLFOCUS )
			{
				TLandscapeObject& co = _ptr->app.currObj;
				TApp& app = _ptr->app;

				switch ( LOWORD(wParam) )
				{
					case IDC_STRENGTHEDIT:
						{
							size_t strength = EditGetInt( _ptr->editStrength );
							if ( strength!=co.Strength() )
							{
								if ( strength>100 )
									strength = 100;

								co.Strength( strength );
								_ptr->Modified( true );
							}
							break;
						}
					case IDC_POINTS:
						{
							size_t points = EditGetInt( _ptr->editPoints );
							if ( points!=co.Points() )
							{
								co.Points( points );
								_ptr->Modified( true );
							}
							break;
						}
					case IDC_BOMBS:
						{
							size_t bombs = EditGetInt( _ptr->editBombs );
							if ( bombs!=co.Bombs() )
							{
								co.Bombs( bombs );
								co.V2Countdown( bombs );
								_ptr->Modified( true );
							}
							break;
						}
					case IDC_SHELLS:
						{
							size_t shells = EditGetInt( _ptr->editShells );
							if ( shells!=co.Shells() )
							{
								co.Shells( shells );
								_ptr->Modified( true );
							}
							break;
						}
					case IDC_XPOS:
						{
							float xp = EditGetFloat( _ptr->editXPos );
							if ( xp!=co.TX() )
							{
								app.lx = xp;
								co.TX(xp);
								_ptr->SetTranslation( app.lx,app.ly,app.lz );
								_ptr->Modified( true );
							}
							break;
						}
					case IDC_YPOS:
						{
							float yp = EditGetFloat( _ptr->editYPos );
							if ( yp!=co.TY() )
							{
								app.ly = yp;
								co.TY(yp);
								_ptr->SetTranslation( app.lx,app.ly,app.lz );
								_ptr->Modified( true );
							}
							break;
						}
					case IDC_ZPOS:
						{
							float zp = EditGetFloat( _ptr->editZPos );
							if ( zp!=co.TZ() )
							{
								app.lz = zp;
								co.TZ(zp);
								_ptr->SetTranslation( app.lx,app.ly,app.lz );
								_ptr->Modified( true );
							}
							break;
						}

					case IDC_XROT:
						{
							float xr = EditGetFloat( _ptr->editXRot );
							if ( xr!=co.RX() )
							{
								app.rotX = xr;
								co.RX(xr);
								_ptr->SetRotation( app.rotX, app.rotY, app.rotZ );
								_ptr->Modified( true );
							}
							break;
						}
					case IDC_YROT:
						{
							float yr = EditGetFloat( _ptr->editYRot );
							if ( yr!=co.RY() )
							{
								app.rotY = yr;
								co.RY(yr);
								_ptr->SetRotation( app.rotX, app.rotY, app.rotZ );
								_ptr->Modified( true );
							}
							break;
						}
					case IDC_ZROT:
						{
							float zr = EditGetFloat( _ptr->editZRot );
							if ( zr!=co.RZ() )
							{
								app.rotZ = zr;
								co.RZ(zr);
								_ptr->SetRotation( app.rotX, app.rotY, app.rotZ );
								_ptr->Modified( true );
							}
							break;
						}

					case IDC_XSCALE:
						{
							float xs = EditGetFloat( _ptr->editXScale );
							if ( xs!=co.ScaleX() )
							{
								app.scaleX = xs;
								co.ScaleX(xs);
								_ptr->SetScale( app.scaleX, app.scaleY, app.scaleZ );
								_ptr->Modified( true );
							}
							break;
						}
					case IDC_YSCALE:
						{
							float ys = EditGetFloat( _ptr->editYScale );
							if ( ys!=co.ScaleY() )
							{
								app.scaleY = ys;
								co.ScaleY(ys);
								_ptr->SetScale( app.scaleX, app.scaleY, app.scaleZ );
								_ptr->Modified( true );
							}
							break;
						}
					case IDC_ZSCALE:
						{
							float zs = EditGetFloat( _ptr->editZScale );
							if ( zs!=co.ScaleZ() )
							{
								app.scaleZ = zs;
								co.ScaleZ(zs);
								_ptr->SetScale( app.scaleX, app.scaleY, app.scaleZ );
								_ptr->Modified( true );
							}
							break;
						}
				}
			};

			if ( HIWORD(wParam)==BN_CLICKED )
			{
				if ( LOWORD(wParam)==IDC_ADDROTATIONSETTING )
				{
					char buf[256];
					EditGetText( _ptr->editRotationName, buf, 256 );
					TString newName = buf;
					if ( _ptr->numPresetRotations<255 )
					{
						int existingIndex = 0;
						bool found = false;
						for ( size_t i=0; i<_ptr->numPresetRotations && !found; i++ )
						{
							if ( _ptr->presetRotationNames[i]==newName )
							{
								found = true;
								existingIndex = i;
							}
						}

						if ( found )
						{
							if ( ::MessageBox( _ptr->parent, "A rotation preset with that name already exists\nOverwrite settings?", 
										  "Warning", MB_YESNO | MB_ICONWARNING )==IDYES )
							{
								float xr = EditGetFloat( _ptr->editXRot );
								float yr = EditGetFloat( _ptr->editYRot );
								float zr = EditGetFloat( _ptr->editZRot );
								_ptr->presetRotations[existingIndex][0] = xr;
								_ptr->presetRotations[existingIndex][1] = yr;
								_ptr->presetRotations[existingIndex][2] = zr;
							}
						}
						else
						{
							ComboBoxAddString( _ptr->ddSavedRotations, newName.c_str() );
							_ptr->presetRotationNames[_ptr->numPresetRotations] = newName;
							float xr = EditGetFloat( _ptr->editXRot );
							float yr = EditGetFloat( _ptr->editYRot );
							float zr = EditGetFloat( _ptr->editZRot );
							_ptr->presetRotations[_ptr->numPresetRotations][0] = xr;
							_ptr->presetRotations[_ptr->numPresetRotations][1] = yr;
							_ptr->presetRotations[_ptr->numPresetRotations][2] = zr;
							_ptr->numPresetRotations++;
						}
					}
				}
				if ( LOWORD(wParam)==IDC_HUGH )
				{
					float x,z,ys;
					x = EditGetFloat( _ptr->editXPos );
					z = EditGetFloat( _ptr->editZPos );
					ys = EditGetFloat( _ptr->editYScale );
					float sizeY = _ptr->app.currObj.SizeY() * ys;
					float y = _ptr->app.GetLandscapeOnlyY( x,z ) + sizeY;
					_ptr->app.ly = y;
					_ptr->app.currObj.TY(y);
					_ptr->SetTranslation( x,y,z );
					_ptr->Modified( true );
				}
				if ( LOWORD(wParam)==IDC_ADDSCALESETTING )
				{
					char buf[256];
					EditGetText( _ptr->editScaleName, buf, 256 );
					TString newName = buf;
					if ( _ptr->numPresetScales<255 )
					{
						int existingIndex = 0;
						bool found = false;
						for ( size_t i=0; i<_ptr->numPresetScales && !found; i++ )
						{
							if ( _ptr->presetScaleNames[i]==newName )
							{
								existingIndex = i;
								found = true;
							}
						}
						if ( found )
						{
							if ( ::MessageBox( _ptr->parent, "A scale preset with that name already exists\nOverwrite settings?", 
										  "Warning", MB_YESNO | MB_ICONWARNING )==IDYES )
							{
								float xs = EditGetFloat( _ptr->editXScale );
								float ys = EditGetFloat( _ptr->editYScale );
								float zs = EditGetFloat( _ptr->editZScale );
								_ptr->presetScales[existingIndex][0] = xs;
								_ptr->presetScales[existingIndex][1] = ys;
								_ptr->presetScales[existingIndex][2] = zs;
							}
						}
						else
						{
							ComboBoxAddString( _ptr->ddSavedScales, newName.c_str() );
							_ptr->presetScaleNames[_ptr->numPresetScales] = newName;
							float xs = EditGetFloat( _ptr->editXScale );
							float ys = EditGetFloat( _ptr->editYScale );
							float zs = EditGetFloat( _ptr->editZScale );
							_ptr->presetScales[_ptr->numPresetScales][0] = xs;
							_ptr->presetScales[_ptr->numPresetScales][1] = ys;
							_ptr->presetScales[_ptr->numPresetScales][2] = zs;
							_ptr->numPresetScales++;
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


