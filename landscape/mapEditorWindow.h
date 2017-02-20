#ifndef _LANDSCAPE_MAPEDITORWINDOW_H_
#define _LANDSCAPE_MAPEDITORWINDOW_H_

//==========================================================================

#include <object/soundTrack.h>

class ObjectEditor;
class HeightEditor;

//==========================================================================

class MapEditor
{
public:
	MapEditor( TApp& _app, HINSTANCE _instance, HWND _parent,
			   ObjectEditor* _objEdit, HeightEditor* _heightEditor,
			   TVehicleList& _vehicleList );
	~MapEditor( void );

	HWND Window( void ) const;

	void SetMapInfo( TDefFile& def );
	void AddObject( size_t type );
	void AddObject( float x, float y, float z, float scale, float yoffset );
	void DeleteObject( bool checkV2Marker = true );

	void Timer( void );

	void WaterLevel( size_t _water );
	float WaterLevel( void ) const;

	void SetCurrentEdit( int edit );
	void DoMenu( int cmd );

	// set window pos at x,y
	void SetPos( int x, int y );

	// modified
	bool Modified( void ) const;
	void Modified( bool _modified );

private:
	// start a new island
	void NewIsland( const TString& binName );
	void LoadDefn( void );

	bool LoadBitmap( TString& fname );

	void BeginCriticalSection( void );
	void EndCriticalSection( void );

private:
	friend LRESULT CALLBACK MapEditorProc(HWND, UINT, WPARAM, LPARAM);

	static MapEditor* ptr;

	TApp&			app;
	HWND			parent;
	HINSTANCE		instance;
	ObjectEditor*	objEdit;
	HeightEditor*	heightEditor;

	TVehicleList&	vehicleList;

	bool			modified;

	int		currentEdit;
	int		currentCreatePlant;
	int		currentCreateVehicle;
	int		currentCreatePlane;
	int		currentCreateStructure;
	int		currentCreateTeam;
	int		currentCreateMisc;

	TString	plants[256];
	TString	vehicles[256];
	TString	planes[256];
	TString	structures[256];
	TString	teams[256];
	TString	misc[256];

	bool	saveEnabled;
	bool	deleteEnabled;
	bool	editControlsEnabled;

	bool	insideCriticalSection;

	float	scale;
	size_t	water;

	TString				stripName[256];
	TString				stripDescription[256];
	size_t				numStrips;

	TString				bumpName[256];
	TString				bumpDescription[256];
	size_t				numBumps;

	HWND	dialog;

	HWND	ddPlants;
	HWND	ddVehicles;
	HWND	ddPlanes;
	HWND	ddTeams;
	HWND	ddStructures;
	HWND	ddMisc;

	HWND	ddMapItems;
	HWND	editMapName;
	HWND	editCreator;
	HWND	editBinFile;
	HWND	ddStrip;
	HWND	ddBump;

	HWND	ddTOD;
	HWND	ddSoundTrack;
	
	HWND	editRandomCount;
	HWND	editRandomScale;
	HWND	editRandomYOffset;

	HWND	butSave;
	HWND	butSaveAs;
	HWND	butDelete;

	HWND	sliderScale;
	HWND	sliderWater;

	SoundTrack	soundTracks;
};

//==========================================================================

#endif
