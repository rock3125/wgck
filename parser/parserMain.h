#ifndef _PARSER_PARSERMAIN_H_
#define _PARSER_PARSERMAIN_H_

//==========================================================================
//
#include <win32/events.h>

#define kMaxStrings 80

//==========================================================================

class TApp : public TEvent
{
public:
	TApp( HINSTANCE );
	~TApp( void );

	void Draw( void );
	void Font( HFONT _font );
	void Logic( void );

	void WriteString( const char* fmt, ... );
	void ParserWrite( const TString& str );
	bool DoBatch( const TString& fname );

	void DoTransform( float* matrix,
					  float tx, float ty, float tz,
					  float rx, float ry, float rz,
					  float sx, float sy, float sz );

	void KeyDown( size_t kd );
	void KeyUp( size_t keyUp );
	void KeyPress( size_t key );
	void Help( void );

	void MouseDown( int button, int x, int y);
	void MouseDown( int button, int x, int y, int dx, int dy );
	void MouseUp( int button, int x, int y );
	void MouseUp( int button, int x, int y, int dx, int dy );
	void MouseMove( int x, int y );
	void MouseMove( int x, int y, int dx, int dy );

protected:
	bool ProcessInput( const TString& line );
	void ClearObjects( void );
	bool LoadObject( const TString& fname, const TString& path, const TString& texturePath,
					 TString& errStr );
	bool OpenObject( TString& fname );
	bool OpenXObject( TString& fname );
	bool Open3DSObject( TString& fname );
	bool SaveObject( TString& _fname );

	void AddToHistory( const TString& str );

	void CloseTransformWindow( void );
	void SetupTransformWindow( void );
	void TransformLogic( void );

private:
	HBRUSH	blackBrush;
	HFONT	font;

	size_t	cursorFlash;
	TString	str;
	TString	lines[kMaxStrings];
	TString	whoStr;
	size_t	numStrings;
	size_t	fontHeight;
	size_t	multiplier;

	TString history[kMaxStrings];
	size_t	historyIndex;
	size_t	numHistory;

	float	x;
	float	y;

	// object location
	float	posx;
	float	posy;
	float	posz;
	float	yangle;
	float	xangle;
	float	deltaXAngle;
	float	deltaYAngle;
	size_t	currentObject;
	size_t	keys;
	bool	showBoundingBox;
	size_t	startX, startY;
	bool	leftDown;
	bool	paper;

	TBinObject*			binObj;
	TCompoundObject*	compObj;
	TLandscape*			landObj;
	TAnimation*			animObj;

	TString				binfilename;

	TGUI*				transform;

	TGUI*				resetButton;
	TGUI*				applyButton;
	TGUI*				saveButton;
	TGUI*				closeButton;
	TGUI*				flipuvButton;

	TGUI*				editTX;
	TGUI*				editTY;
	TGUI*				editTZ;

	TGUI*				editRX;
	TGUI*				editRY;
	TGUI*				editRZ;

	TGUI*				editSX;
	TGUI*				editSY;
	TGUI*				editSZ;
};


//==========================================================================

#endif

