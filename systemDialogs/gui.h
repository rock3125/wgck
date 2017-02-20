#ifndef _DIALOGS_GUI_H_
#define _DIALOGS_GUI_H_

//==========================================================================

#include <object/texture.h>
#include <object/compoundObject.h>

class TEvent;
class TCharacter;

#define kMaxDropDown 20

//==========================================================================

enum controlType { guiWindow=0, guiLabel, guiButton, guiEdit, guiDropDown, 
				   guiSlider, guiCheckBox, guiBitmap, guiCompoundObject,
				   guiListbox };

enum controlState { csDisabled=0, csEnabled, csSelected, csClicked };

class _EXPORT TGUI
{
public:
	TGUI( TEvent*, size_t type, float x, float y, 
		  float w, float h, float depth,
		  const TString& text="",
		  byte r=0, byte g=0, byte b=0 );

	TGUI( TEvent*, size_t type, float x, float y, 
		  float w, float h, float depth,
		  TTexture* texture, float u0, float v0, float u1, float v1 );

	TGUI( TEvent* _app, size_t type, float _x, float _y, float _z,
		  TCompoundObject* _compoundObject );

	TGUI( TEvent* _app, size_t type, float _x, float _y, float _z,
		  TCharacter* _character );

	~TGUI( void );

	// center the menu
	void		CenterMenu( void );

	void		Draw( bool dim );

	void		MouseDown( int x, int y );
	void		MouseMove( int x, int y );
	void		MouseUp( int x, int y );

	void		KeyPress( size_t kp, bool ctrlDown );
	void		KeyUp( size_t kp );
	void		KeyDown( size_t kp );

	TGUI*		GetSelected( void );

	// set x,y of window
	void		SetLocation( float x, float y );

	// add item to drop down
	void		AddDropDownItem( const TString& item );
	void		ClearItems( void );

	// add item to listbox
	void		AddListBoxItem( const TString& item );

	// get-set drop down selected item
	void		SelectedItem( int index );
	int			SelectedItem( void ) const;

	// get-set control text
	void			Text( const TString& text );
	const TString&	Text( void ) const;
	void			MaxText( size_t max );
	void			Edit( bool ); // can type text?

	// get-set control check-ed-ness (radio buttons)
	bool		Checked( void ) const;
	void		Checked( bool );

	// enable/disable a control
	void		EnableControl( bool enable );
	bool		Enabled( void ) const;

	// get control by index
	TGUI*		GetControl( size_t index );

	// compound object controls
	void		Translate( float x, float y, float z );
	void		Rotate( float x, float y, float z );
	void		Scale( float scale );

	// is a button clicked?
	bool		Clicked( void );

	// slider settings
	void		SliderSetRange( size_t min, size_t max );
	void		SliderSetFrequency( size_t freq );
	size_t		SliderPos( void ) const;
	void		SliderPos( size_t pos );

	// set one key that is equivalent to clicking it
	void		ButtonKey( size_t key );

	void				SetObject( TCompoundObject* obj );
	TCompoundObject*	GetObject( void );

	void				SetModel( TCharacter* obj );
	TCharacter*			GetModel( void );

private:
	void		DrawSingle( bool dim );

	size_t		MouseDownSingle( float x, float y );
	size_t		MouseMoveSingle( float x, float y );
	size_t		MouseUpSingle( float x, float y );

	size_t		KeyPressSingle( size_t kp, bool ctrlDown );
	size_t		KeyUpSingle( size_t kp );
	size_t		KeyDownSingle( size_t kp );

	void		MenuBox( void );
	void		MenuLabel( void );
	void		MenuButton( void );
	void		MenuEdit( void );
	void		MenuDropDown( void );
	void		MenuSlider( void );
	void		MenuCheckBox( void );
	void		MenuBitmap( void );
	void		MenuCompoundObject( void );
	void		MenuListbox( void );

	void		Write( float x, float y, float z, const TString& );

	TGUI*		GetNext( TGUI* start, TGUI* list );
	TGUI*		GetPrev( TGUI* start, TGUI* list );

public:
	float		xpos, ypos, zpos;
	float		xa, ya, za;
	float		scale;

	TEvent*		app;
	TTexture*	texture;
	TCompoundObject* compoundObject;
	TCharacter* character;
	size_t		itemType;
	float		x,y,w,h;
	float		depth;
	TString		text;
	size_t		state;
	byte		r,g,b;
	size_t		cntr;
	size_t		maxText;

	float		u0,v0,u1,v1;

	float		fontSize;

	bool		selectable;
	bool		checked;

	// has this menu been shown before?
	float		animationCounter;

	size_t		buttonkey;

	float		xp, yp; // mouse loc

	// light multiplier
	float		lm;

	// drop down list items
	size_t		numItems;
	TString		dropDownList[kMaxDropDown];
	int			selectedItem;

	// slider setup
	size_t		numDivisions;
	size_t		numMarkers;
	size_t		sliderPos;
	size_t		sliderMin;
	size_t		sliderMax;

	// window dragging
	bool		dragging;
	float		dragx, dragy;

	TGUI*		next;
};

//==========================================================================

#endif
