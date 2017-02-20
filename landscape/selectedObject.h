#ifndef _LANDSCAPE_SELECTEDOBJECT_H_
#define _LANDSCAPE_SELECTEDOBJECT_H_

//==========================================================================

#define kMaxEntries	256

//==========================================================================

class ObjectEditor
{
	ObjectEditor( const ObjectEditor& );
	const ObjectEditor& operator=( const ObjectEditor& );
public:
	ObjectEditor( TApp& _app, HINSTANCE _instance, HWND _parent,
				  TVehicleList& _vehicleList );
	~ObjectEditor( void );

	void SetTranslation( float lx, float ly, float lz );
	void SetRotation( float rx, float ry, float rz );
	void SetScale( float sx, float sy, float sz );
	void SelectionChanged( int _currentEdit, bool forceChange = false );

	void SetPos( int x, int y ); // set window pos

	// set scale & rotation drop down indexes
	void CurrentScale( int _scale );
	void CurrentRotation( int _rotation );

	// set new edit
	void SetCurrentEdit( void );

	// set destruct and AI check boxes
	void SetCheckBoxes( bool _destruct, bool _ai );

	// get offsets for map editor
	void GetOffset( float& x, float& z, float &yscale );

	// modified since last save?
	bool	Modified( void ) const;
	void	Modified( bool );

	// the dialog window 
	HWND Window( void ) const;

private:
	void LoadBinary( const TString& fname );
	void SaveBinary( const TString& fname );
	void Timer( void );

private:
	friend LRESULT CALLBACK ObjectEditorProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

	TApp&		app;
	HWND		parent;
	HINSTANCE	instance;
	TVehicleList& vehicleList;

	bool	modified; // since last set?

	HWND	dialog;

	HWND	ddSavedRotations;
	HWND	ddSavedScales;
	HWND	editXPos;
	HWND	editYPos;
	HWND	editZPos;
	HWND	editXRot;
	HWND	editYRot;
	HWND	editZRot;
	HWND	editStrength;
	HWND	editPoints;
	HWND	editShells;
	HWND	editBombs;
	HWND	sliderXRot;
	HWND	sliderYRot;
	HWND	sliderZRot;
	HWND	editRotationName;
	HWND	editXScale;
	HWND	editYScale;
	HWND	editZScale;
	HWND	sliderXScale;
	HWND	sliderYScale;
	HWND	sliderZScale;
	HWND	editScaleName;
	HWND	butAddRot;
	HWND	butAddScale;
	HWND	butYOnLand;
	HWND	checkDestructable;
	HWND	checkArmory;
	HWND	checkRepairs;
	HWND	checkRefuelStation;
	HWND	checkIsaAI;
	HWND	ddAIType;
	HWND	ddAITeam;
	HWND	ddGameImportance;
	HWND	ddCharacter;
	HWND	bombLabel;

	bool	addRotEnabled;
	bool	addScaleEnabled;
	bool	translationChanged;

	float	tx;
	float	ty;
	float	tz;

	float	rotX;
	float	rotY;
	float	rotZ;

	float	scaleX;
	float	scaleY;
	float	scaleZ;

	bool	destructable;
	bool	armory;
	bool	refuelStation;
	bool	isaAI;
	bool	repairs;

	bool	editControlsEnabled;

	int		currentEdit;

	int		currentRotation;
	TString	presetRotationNames[kMaxEntries];
	float	presetRotations[kMaxEntries][3];
	size_t	numPresetRotations;

	int		currentScale;
	TString	presetScaleNames[kMaxEntries];
	float	presetScales[kMaxEntries][3];
	size_t	numPresetScales;

	int		characterType;

	static ObjectEditor* ptr;
};

//==========================================================================

#endif
