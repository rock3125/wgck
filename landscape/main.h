#ifndef _VIEWER_MAIN_H_
#define _VIEWER_MAIN_H_

//==========================================================================

#include <Win32/events.h>
#include <Win32/win32.h>

#include <object/compoundObject.h>
#include <object/binobject.h>
#include <object/landscape.h>
#include <object/geometry.h>
#include <object/anim.h>

#include <object/deffile.h>
#include <landscape/mapEditorWindow.h>
#include <landscape/heightEditorWindow.h>
#include <landscape/BotEditorWindow.h>
#include <landscape/selectedObject.h>


//#undef kMaxObjects
//#define kMaxObjects		15


//==========================================================================

class TApp : public TEvent
{
public:
	TApp( HINSTANCE, const TString& );
    ~TApp( void );

	bool StartupSystem( TString& errStr );

	bool LoadObject( const TString& fname, TString& errStr );
	bool LoadObject( const TString& fname, TDefFile& def, TString& errStr );
	
	void SaveDefn( void );
	void SaveDefnAs( void );

	void Render( void );
	void Logic( void );

	void MouseDown( int button, int x, int y);
	void MouseDown( int button, int, int, int, int );
	void MouseDown( HWND, int button, int, int, int, int );
	void MouseUp( int button, int, int );
	void MouseUp( int button, int, int, int, int );
	void MouseUp( HWND, int button, int, int, int, int );
	void MouseMove( int x, int y );
	void MouseMove( int x, int y, int dx, int dy );
	void MouseMove( HWND, int x, int y, int dx, int dy );

	void KeyDown( size_t kd );
	void KeyUp( size_t ku );
	void KeyPress( size_t key );

	void DoMenu( int );

	bool Initialised( void );
	void Initialised( bool );

	void SetCmdLine( const char* );

	size_t		NumObjects( void ) const;
	TString		GetObjectName( size_t index ) const;

	float		GetLandscapeY( float x, float z );
	float		GetLandscapeOnlyY( float x, float z );

	// allow user to select a bin file
	bool		LoadBinAs( TString& binName );

	// do new given a .def for loading info from
	bool		NewIsland( const TString& _path, const TString& _texturePath, 
						   bool destroy, TString& errStr );

	// landscape load assist
	TCompoundObject*	GetObjectByName( const TString& name, size_t, size_t );
	size_t				GetObjectIndexByName( const TString& name ) const;

	// set currently displayed object given its name
	void				SetCurrentObject( const TString& name );
	TCompoundObject*	GetCurrentObject( TString& name ) const;

	// is obj clicked on at mx, my?
	bool ClickedObject( TLandscapeObject& obj, int mx, int my );

	// set map scale
	void		Scale( float _scale );

	bool ShowBotMarkers( void ) const;
	void ShowBotMarkers( bool );

	bool Modified( void ) const;

	void RegisteredMessage( const TString& _registeredMessage );

private:
	bool GetRegistrySettings( void );
	void GetObjects( void );

public:
    TString		fileName;
    TString		path;
	TString		storyPath;
	TString		texturePath;
    
private:
	friend LRESULT CALLBACK MapEditorProc(HWND, UINT, WPARAM, LPARAM);
	friend LRESULT CALLBACK ObjectEditorProc( HWND, UINT, WPARAM, LPARAM );
	friend class MapEditor;
	friend class BotEditor;
	friend class ObjectEditor;

	bool		showBoundingBox;
    bool 		leftDown;
	bool		initialised;
    int 		startX;
    int			startY;

    float		yAngle;
    float		xAngle;
	float		deltaXAngle;
	float		deltaYAngle;

    float		lx;
    float		ly;
    float		lz;

	int			keys;

	MapEditor*			editor;
	BotEditor*			botEditor;
	HeightEditor*		heightEdit;
	ObjectEditor*		objectEditor;
	bool				showBotMarkers;
	TString				registeredMessage;

	TBinObject*			obj;
	TLandscape			landObj;
	TAnimation			animationObj;
	size_t				currentLocation;

	TBinObject			heightMapObj;

	float				objectyangle;
	TCompoundObject*	objects[kMaxObjects];
	TString				objectNames[kMaxObjects];
	size_t				numObjects;

	size_t				currentObject;
	TString				currentObjectName;

	float				scaleX,scaleY,scaleZ;
	float				rotX, rotY, rotZ;

	TCamera				cam;

	TString				saveFname;
	TLandscapeObject	currObj;
	bool				currentObjectInUse;
	size_t				currentEdit;

	TDefFile			def;				// file that holds landscape additions

	float				scale;

	bool				mapLoaded;

	TVehicleList		vehicleList;		// list of vehicles
};

//==========================================================================

#endif
