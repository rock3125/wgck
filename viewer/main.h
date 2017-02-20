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

//==========================================================================

class TApp : public TEvent
{
public:
	TApp( HINSTANCE );
    ~TApp( void );

	bool StartupSystem( TString& errStr );

	bool LoadObject( const TString& fname, TString& errStr );
	bool OpenNewObject( void );

	void Render( void );
	void Logic( void );

	void MouseDown( int button, int x, int y);
	void MouseDown( int button, int, int, int, int );
	void MouseUp( int button, int, int );
	void MouseUp( int button, int, int, int, int );
	void MouseMove( int x, int y );
	void MouseMove( int x, int y, int dx, int dy );
	void KeyDown( int kd );
	void KeyUp( int ku );

	void SetCmdLine( const char* );

	TCompoundObject* GetObjectByName( const TString&, size_t, size_t);
	bool AddTransport( TLandscapeObject& obj );

private:
	bool GetRegistrySettings( void );

public:
    TString		fileName;
    TString		path;
	TString		texturePath;
    
private:
	bool		showBoundingBox;
    bool 		leftDown;
	bool		paper;
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

	size_t		currentObject;
	bool		landscape;
	bool		animation;
	bool		object;
	bool		cobject;

	TCompoundObject		cobj;
	TBinObject			obj;
	TLandscape			landObj;
	TAnimation			animationObj;
	size_t				currentLocation;

	TBinObject			heightMapObj;
};

//==========================================================================

#endif
