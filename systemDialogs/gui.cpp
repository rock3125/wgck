#include <precomp_header.h>

#include <win32/events.h>
#include <systemDialogs/gui.h>
#include <object/character.h>

//==========================================================================

TGUI::TGUI( TEvent* _app, size_t type, float _x, float _y, 
		    float _w, float _h, float _depth,
			const TString& _text,
			byte _r, byte _g, byte _b )
	: next(NULL),
	  app( _app ),
	  texture( NULL ),
	  compoundObject( NULL ),
	  character( NULL )
{
	lm = 1.0f; // light multiplier for dimmer menus & alpha values

	xa = ya = za = 0;

	maxText = 10;
	fontSize = 14;
	scale = 1;

	dragging = false;
	dragx = 0;
	dragy = 0;

	xpos = 0;
	ypos = 0;
	zpos = 0;

	selectable = true;
	if ( type==guiWindow || type==guiLabel )
		selectable = false;

	numItems = 0;

	itemType = type;
	x = _x;
	y = _y;
	w = _w;
	h = _h;
	r = _r;
	g = _g;
	b = _b;
	depth = _depth;
	text = _text;
	state = csEnabled;
	xp = 0;
	yp = 0;
	selectedItem = -1;
	cntr = 0;

	numDivisions = 10;
	numMarkers = 10;
	sliderPos = 0;
	sliderMin = 0;
	sliderMax = 10;

	buttonkey = 0;

	checked = false;

	animationCounter = 0;
};


TGUI::TGUI( TEvent* _app, size_t type, float _x, float _y, 
			float _w, float _h, float _depth,
			TTexture* _texture, float _u0, float _v0, float _u1, float _v1 )
	: next(NULL),
	  app( _app ),
	  texture( _texture ),
	  compoundObject( NULL ),
	  character( NULL )
{
	u0 = _u0;
	v0 = _v0;
	u1 = _u1;
	v1 = _v1;

	xa = ya = za = 0;

	fontSize = 14;
	scale = 1;

	dragging = false;
	dragx = 0;
	dragy = 0;

	xpos = 0;
	ypos = 0;
	zpos = 0;

	selectable = false;
	numItems = 0;

	itemType = type;
	x = _x;
	y = _y;
	w = _w;
	h = _h;
	r = 255;
	g = 255;
	b = 255;
	depth = _depth;

	state = csEnabled;
	xp = 0;
	yp = 0;
	selectedItem = -1;
	cntr = 0;

	numDivisions = 10;
	numMarkers = 10;
	sliderPos = 0;

	checked = false;
	buttonkey = 0;

	animationCounter = 0;
};


TGUI::TGUI( TEvent* _app, size_t type, float _x, float _y, float _z,
			TCompoundObject* _compoundObject )
	: next(NULL),
	  app( _app ),
	  texture( NULL ),
	  compoundObject( _compoundObject ),
	  character( NULL )
{
	u0 = 0;
	v0 = 0;
	u1 = 0;
	v1 = 0;

	xa = ya = za = 0;

	fontSize = 14;
	scale = 1;

	dragging = false;
	dragx = 0;
	dragy = 0;

	xpos = 0;
	ypos = 0;
	zpos = 0;

	selectable = false;
	numItems = 0;

	itemType = type;
	x = _x;
	y = _y;
	w = 5;
	h = 5;
	r = 255;
	g = 255;
	b = 255;
	depth = _z;

	state = csEnabled;
	xp = 0;
	yp = 0;
	selectedItem = -1;
	cntr = 0;

	numDivisions = 10;
	numMarkers = 10;
	sliderPos = 0;

	checked = false;
	buttonkey = 0;

	animationCounter = 0;
};


TGUI::TGUI( TEvent* _app, size_t type, float _x, float _y, float _z,
			TCharacter* _character )
	: next(NULL),
	  app( _app ),
	  texture( NULL ),
	  compoundObject( NULL ),
	  character( _character )
{
	u0 = 0;
	v0 = 0;
	u1 = 0;
	v1 = 0;

	xa = ya = za = 0;

	fontSize = 14;
	scale = 1;

	dragging = false;
	dragx = 0;
	dragy = 0;

	xpos = 0;
	ypos = 0;
	zpos = 0;

	selectable = false;
	numItems = 0;

	itemType = type;
	x = _x;
	y = _y;
	w = 5;
	h = 5;
	r = 255;
	g = 255;
	b = 255;
	depth = _z;

	state = csEnabled;
	xp = 0;
	yp = 0;
	selectedItem = -1;
	cntr = 0;

	numDivisions = 10;
	numMarkers = 10;
	sliderPos = 0;

	checked = false;
	buttonkey = 0;

	animationCounter = 0;
};


TGUI::~TGUI( void )
{
	// delete all others too
	TGUI* temp = next;
	next = NULL;
	delete temp;
	texture = NULL;
	character = NULL;
};


void TGUI::Translate( float _x, float _y, float _z )
{
	x = _x;
	y = _y;
	depth = _z;
};


void TGUI::CenterMenu( void )
{
	xpos = float(app->vwidth) * 0.5f - (w * 0.5f);
	ypos = (float(app->vheight) * 0.5f - (h * 0.5f)) + 20;
};


void TGUI::Rotate( float x, float y, float z )
{
	xa = x;
	ya = y;
	za = z;
};


void TGUI::SetObject( TCompoundObject* obj )
{
	compoundObject = obj;
};


TCompoundObject* TGUI::GetObject( void )
{
	return compoundObject;
};


void TGUI::SetModel( TCharacter* obj )
{
	character = obj;
};


TCharacter* TGUI::GetModel( void )
{
	return character;
};


void TGUI::Scale( float _scale )
{
	scale = _scale;
};


void TGUI::MaxText( size_t _maxText )
{
	maxText = _maxText;
};


void TGUI::ButtonKey( size_t key )
{
	buttonkey = key;
};


void TGUI::AddDropDownItem( const TString& item )
{
	PreCond( numItems<kMaxDropDown );
	dropDownList[numItems] = item;
	numItems++;
};


void TGUI::AddListBoxItem( const TString& item )
{
	AddDropDownItem( item );
};


void TGUI::ClearItems( void )
{
	numItems = 0;
};


void TGUI::SelectedItem( int index )
{
	selectedItem = index;
	if ( selectedItem>=0 )
		text = dropDownList[selectedItem];
};


int TGUI::SelectedItem( void ) const
{
	return selectedItem;
};


void TGUI::Text( const TString& _text )
{
	text = _text;
};


const TString& TGUI::Text( void ) const
{
	return text;
};


bool TGUI::Checked( void ) const
{
	return checked;
};


void TGUI::Checked( bool _checked )
{
	checked = _checked;
};


bool TGUI::Clicked( void )
{
	bool c = ( state==csClicked );
	if ( c )
		state = csSelected;
	return c;
};


void TGUI::EnableControl( bool enable )
{
	if ( enable )
		state = csEnabled;
	else
		state = csDisabled;
};


bool TGUI::Enabled( void ) const
{
	return ( state>=csEnabled );
};


TGUI* TGUI::GetControl( size_t index )
{
	TGUI* list = this;
	if ( index==0 )
		return this;
	else
	{
		while ( list!=NULL && index>0 )
		{
			list = list->next;
			index--;
		}
		if ( index>0 )
			return NULL;
		return list;
	}
};


void TGUI::SliderSetRange( size_t min, size_t max )
{
	PreCond( min < max );
	numDivisions = max - min;
	sliderMin = min;
	sliderMax = max;
};


void TGUI::SliderSetFrequency( size_t freq )
{
	PreCond( freq>0 );
	numMarkers = numDivisions / freq;
	PostCond( numMarkers>0 );
}


size_t TGUI::SliderPos( void ) const
{
	return sliderMin + sliderPos;
};


void TGUI::SliderPos( size_t pos )
{
	PreCond( pos>=sliderMin );
	sliderPos = pos - sliderMin;
};


void TGUI::SetLocation( float x, float y )
{
	xpos = x;
	ypos = y;
};


TGUI* TGUI::GetSelected( void )
{
	TGUI* list = this;
	while ( list!=NULL )
	{
		if ( list->state>=csSelected )
			return list;
		list = list->next;
	}
	return NULL;
};


void TGUI::Draw( bool dim ) 
{
	app->SetupGL2dNormal( app->vdepth, app->vwidth, app->vheight );

	glEnable( GL_NORMALIZE );
	glNormal3f( 0,0,1 );
	glLoadIdentity();
	glTranslatef( xpos, ypos, 0 );

	if ( itemType==guiWindow && animationCounter<5 )
	{
		float scale = (animationCounter+1)*0.15f;

		float x = (w*0.5f) - w*0.5f*scale;
		float y = (h*0.5f) - h*0.5f*scale;
		glTranslatef( x,y,0 );

		glScalef( scale, scale, scale );
		animationCounter++;
	}
	else if ( itemType==guiWindow )
	{
		scale = 1;
		glScalef( scale, scale, scale );
	}

	TGUI* list = this;
	while ( list!=NULL )
	{
		list->xpos = xpos;
		list->ypos = ypos;
		if ( itemType==guiWindow )
			list->animationCounter = animationCounter;
		list->DrawSingle(dim);
		list = list->next;
	}
	
	glDisable( GL_NORMALIZE );
};


void TGUI::Write( float x, float y, float z, const TString& text )
{
	app->WriteReverse( x,y,z,text );
};


void TGUI::MouseDown( int _x, int _y ) 
{
	TGUI* list = this;

	xp = float(_x) - xpos;
	yp = float(_y) - ypos;

	while ( list!=NULL )
	{
		if ( list->selectable )
			list->MouseDownSingle(xp,yp);
		else if ( list->itemType==guiWindow )
		{
			if ( xp>=list->x && xp<=(list->x+list->w) )
			if ( yp>=list->y && yp<=(list->y+fontSize) )
			{
				dragging = true;
				dragx = xp;
				dragy = yp;
			}
		}
		list = list->next;
	}
};


void TGUI::MouseMove( int _x, int _y ) 
{
	TGUI* list = this;

	xp = float(_x) - xpos;
	yp = float(_y) - ypos;

	if ( dragging )
	{
		float dx = xp-dragx;
		float dy = yp-dragy;
		xpos += dx;
		ypos += dy;
	}
	else
	{

		bool found = false;
		bool lbactive = false;
		while ( list!=NULL )
		{
			if ( list->itemType==guiDropDown )
			if ( list->state == csClicked )
			{
				lbactive = true;
			}
			if ( list->selectable )
			if ( xp>=list->x && xp<=(list->x+list->w) )
			if ( yp>=list->y && yp<=(list->y+list->h) )
			{
				found = true;
			}
			list = list->next;
		};

		list = this;
		while ( list!=NULL )
		{
			if ( list->selectable )
			{
				if ( list->state==csSelected && found )
				{
					if ( xp>=list->x && xp<=(list->x+list->w) )
					{
						if ( yp>=list->y && yp<=(list->y+list->h) )
							list->state = csSelected;
						else
							list->state = csEnabled;
					}
					else
						list->state = csEnabled;
				}
				if ( !lbactive || list->state==csClicked )
					list->MouseMoveSingle(xp,yp);
			}
			list = list->next;
		}
	}
};


void TGUI::MouseUp( int _x, int _y ) 
{
	dragging = false;

	xp = float(_x) - xpos;
	yp = float(_y) - ypos;

	TGUI* list = this;
	while ( list!=NULL )
	{
		if ( list->selectable )
			list->MouseUpSingle(xp,yp);
		list = list->next;
	}
};


void TGUI::KeyPress( size_t kp, bool ctrlDown )
{
	TGUI* list = this;
	while ( list!=NULL )
	{
		if ( list->selectable )
			list->KeyPressSingle(kp,ctrlDown);
		list = list->next;
	}
};


void TGUI::KeyDown( size_t kp )
{
	TGUI* list = this;
	while ( list!=NULL )
	{
		if ( kp!=VK_TAB )
			list->KeyDownSingle(kp);
		list = list->next;
	}
};


void TGUI::KeyUp( size_t kp )
{
	TGUI* list = this;

	// if this is UP or DOWN, make
	// sure we're not inside a dropdown
	// use the moved flag to switch off inter contorl movement
	bool moved = false;
	while ( list!=NULL )
	{
		if ( list->itemType==guiDropDown && list->state==csClicked )
			moved = true;
		list = list->next;
	}

	list = this;
	while ( list!=NULL )
	{
		// get next
		if ( kp==VK_TAB && !moved )
		if ( list->state > csEnabled )
		{
			moved = true;
			TGUI* l = list;
			l->state = csEnabled;
			l = GetNext( this, list );
			l->state = csSelected;
		};
		// get previous
		if ( kp==VK_UP && !moved )
		if ( list->state > csEnabled )
		{
			moved = true;
			TGUI* l = list;
			l->state = csEnabled;
			l = GetPrev( this, list );
			l->state = csSelected;
		}
		// get next
		if ( kp==VK_DOWN && !moved )
		if ( list->state > csEnabled )
		{
			moved = true;
			TGUI* l = list;
			l->state = csEnabled;
			l = GetNext( this, list );
			l->state = csSelected;
		}

		// send key down to individual controls
		if ( kp!=VK_TAB && list->selectable )
			list->KeyUpSingle(kp);
		list = list->next;
	}

	// nothing moved? nothing selected!  =>  select first item
	if ( (kp==VK_TAB || kp==VK_UP || kp==VK_DOWN) && !moved )
	{
		TGUI* l = GetNext( this, this );
		l->state = csSelected;
	};
};


TGUI* TGUI::GetNext( TGUI* start, TGUI* list )
{
	if ( list->next==NULL )
	{
		TGUI* tl = start;
		while ( tl!=NULL && (!tl->selectable || tl->state==csDisabled) ) tl = tl->next;
		PostCond( tl!=NULL );
		return tl;
	}
	else
	{
		TGUI* tl = list->next;
		while ( tl!=NULL && (!tl->selectable || tl->state==csDisabled) ) tl = tl->next;
		if ( tl==NULL )
		{
			tl = start;
			while ( tl!=NULL && (!tl->selectable || tl->state==csDisabled) ) tl = tl->next;
		}
		PostCond( tl!=NULL );
		return tl;
	}
};


TGUI* TGUI::GetPrev( TGUI* start, TGUI* list )
{
	// find it before we get there
	TGUI* tl = start;
	TGUI* before = NULL;
	while ( tl!=NULL && tl!=list )
	{
		if ( tl->selectable && tl->state!=csDisabled )
			before = tl;
		tl = tl->next;
	}
	if ( before!=NULL )
		return before;

	// otherwise, there is none-selectable before - so go to the last
	// selectable in the list
	tl = list->next;
	TGUI* after = NULL;
	while ( tl!=NULL )
	{
		if ( tl->selectable && tl->state!=csDisabled )
			after = tl;
		tl = tl->next;
	}
	PostCond( after!=NULL );
	return after;
};


void TGUI::DrawSingle( bool dim )
{
	if ( dim )
		lm = 0.3f;
	else
		lm = 1.0f;

	glEnable( GL_COLOR_MATERIAL );

	switch ( itemType )
	{
	case guiWindow:
		{
			MenuBox();
			break;
		}
	case guiLabel:
		{
			MenuLabel();
			break;
		}
	case guiButton:
		{
			MenuButton();
			break;
		}
	case guiEdit:
		{
			MenuEdit();
			break;
		}
	case guiDropDown:
		{
			MenuDropDown();
			break;
		}
	case guiSlider:
		{
			MenuSlider();
			break;
		}
	case guiCheckBox:
		{
			MenuCheckBox();
			break;
		}
	case guiBitmap:
		{
			MenuBitmap();
			break;
		}
	case guiCompoundObject:
		{
			if ( animationCounter>=5 )
				MenuCompoundObject();
			break;
		}
	case guiListbox:
		{
			MenuListbox();
			break;
		}
	};
};


size_t TGUI::MouseDownSingle( float _x, float _y )
{
	xp = _x;
	yp = _y;

	if ( state==csDisabled )
		return state;

	state = csEnabled;
	if ( xp>=x && xp<=(x+w) )
	{
		if ( yp>=y && yp<=(y+h) )
		{
			state = csClicked;
		}
	}

	if ( itemType==guiCheckBox )
	if ( xp>=x && xp<=(x+w) )
	if ( yp>=y && yp<=(y+h) )
	{
		if ( checked )
			checked = false;
		else
			checked = true;
	};

	return state;
};


size_t TGUI::MouseMoveSingle( float _x, float _y )
{
	xp = _x;
	yp = _y;

	if ( state==csDisabled )
		return state;

	if ( itemType==guiDropDown && state==csClicked )
		return state;
	if ( state==csDisabled )
		return state;

	if ( xp>=x && xp<=(x+w) )
	{
		if ( yp>=y && yp<=(y+h) )
		{
			if ( state!=csClicked )
				state = csSelected;
		}
	}
	return state;
};


size_t TGUI::MouseUpSingle( float _x, float _y )
{
	xp = _x;
	yp = _y;

	if ( state==csDisabled )
		return state;

	state = csEnabled;
	if ( xp>=x && xp<=(x+w) )
	{
		if ( yp>=y && yp<=(y+h) )
		{
			state = csSelected;
		}
	}
	if ( itemType==guiDropDown )
	{
		if ( selectedItem>=0 )
			text = dropDownList[selectedItem];
	}
	else if ( itemType==guiSlider )
	{
		if ( xp>=x && xp<=(x+w) )
		if ( yp>=y && yp<=(y+h) )
		{
			float p = (xp-x) / w;
			sliderPos = size_t(p*numDivisions);
			if ( sliderPos>=numDivisions )
				sliderPos = numDivisions - 1;
		}
	}
	return state;
};


size_t TGUI::KeyDownSingle( size_t kp )
{
	if ( state==csDisabled )
		return state;

	if ( kp==buttonkey )
	{
		state = csClicked;
		return state;
	}

	if ( state==csSelected )
	{
		if ( kp==13 || kp==' ' )
		{
			state = csClicked;
		}
	}
	else
	{
		if ( state==csClicked )
		{
			if ( kp==13 || kp==' ' )
			{
				state = csSelected;
				if ( itemType==guiDropDown )
				if ( selectedItem>=0 )
					text = dropDownList[selectedItem];
			}
		}
	}
	return state;
};


size_t TGUI::KeyUpSingle( size_t kp )
{
	if ( state==csDisabled )
		return state;

	if ( itemType==guiDropDown && state==csClicked )
	{
		if ( kp==VK_UP )
		{
			if ( selectedItem>0 )
				selectedItem--;
			else
				selectedItem = numItems-1;
		}
		if ( kp==VK_DOWN )
		{
			selectedItem++;
			if ( selectedItem>=int(numItems) )
				selectedItem = 0;
		}
	}
	else if ( itemType==guiSlider && state==csSelected )
	{
		if ( kp==VK_LEFT )
		{
			if ( sliderPos>0 )
				sliderPos--;
			else
				sliderPos = numDivisions-1;
		}
		if ( kp==VK_RIGHT )
		{
			sliderPos++;
			if ( sliderPos>=numDivisions )
				sliderPos = 0;
		}
	}
	else if ( itemType==guiCheckBox && state>=csSelected )
	{
		if ( kp==13 || kp==32 )
		{
			if ( checked )
				checked = false;
			else
				checked = true;
		}
	}
	else
	{
		if ( state==csClicked )
		{
			if ( kp==13 || kp==' ' || kp==buttonkey )
			{
				state = csSelected;
			}
		}
	}
	return state;
};


size_t TGUI::KeyPressSingle( size_t kp, bool ctrlDown )
{
	if ( state==csDisabled )
		return state;

	if ( itemType==guiEdit && state>=csSelected )
	{
		if ( kp==8 )
		{
			if ( text.length()>1 )
				text = text.substr(0,text.length()-1);
			else
				text = "";
		}
		else if ( kp>=32 && kp<='z' && !ctrlDown )
		{
			if ( (maxText>0 && text.length()<maxText) ||
				 maxText==0 )
				text = text + TString( char(kp) );
		}
		else if ( kp==22 ) // paste from clipboard
		{
			if ( ::OpenClipboard( app->Window() )==TRUE )
			{
				HANDLE hcb = ::GetClipboardData(CF_TEXT);
				if ( hcb!=NULL )
				{
					char* str = (char*)::GlobalLock(hcb);
					if ( str!=NULL )
					{
						text = text + str;
						text = text.substr(0,maxText);
					}
					::GlobalUnlock( hcb );
				}
				::CloseClipboard();
			}
		}
	}
	return state;
};


void TGUI::MenuBox( void )
{
	// background box
	glEnable( GL_BLEND );
	glColor4ub( 0,0,0, 175 );

	glBegin( GL_QUADS );
		glVertex3f( x, y, depth );
		glVertex3f( x, y+h, depth );
		glVertex3f( x+w, y+h, depth );
		glVertex3f( x+w, y, depth );
	glEnd();
	glDisable( GL_BLEND );

	// background border around box
	float off = 5;
	float add = 0.02f;
	glColor3ub( 0,byte(20*lm),byte(100*lm) );
	glBegin( GL_LINES );
		glVertex3f( x+off, y+off, depth+add );
		glVertex3f( x+off, y+(h-off), depth+add );

		glVertex3f( x+off, y+(h-off), depth+add );
		glVertex3f( x+(w-off), y+(h-off), depth+add );

		glVertex3f( x+(w-off), y+(h-off), depth+add );
		glVertex3f( x+(w-off), y+off, depth+add );

		for ( size_t i=0; i<5; i++ )
		{
			glVertex3f( x+(w-off), y+off + float(i*2), depth+add );
			glVertex3f( x+off, y+off + float(i*2), depth+add );
		}
	glEnd();

	add = add + 0.02f;
	if ( text.length()>0 )
	{
		if ( state==csDisabled )
			glColor3ub( byte(50*lm),byte(50*lm),byte(50*lm) );
		else
			glColor3ub( byte(200*lm),byte(200*lm),byte(255*lm) );
		Write( x+off+4, y+2, depth+add, text );
	}
};


void TGUI::MenuLabel( void )
{
	float add = 0.01f;
	if ( state==csDisabled )
		glColor3ub( byte(50*lm),byte(50*lm),byte(50*lm) );
	else
		glColor3ub( byte(r*lm),byte(g*lm),byte(b*lm) );
	Write( x,y,depth+add, text );
};


void TGUI::MenuButton( void )
{
	float add = 0.02f;

	glEnable(GL_BLEND);
	if  ( state==csEnabled || state==csDisabled )
	{
		glColor4ub( 0,0,byte(100*lm),100 );
	}
	else if ( state==csSelected )
	{
		glColor4ub( 0,0,byte(180*lm),100 );
	}
	else if ( state==csClicked )
	{
		glColor4ub( byte(170*lm),byte(100*lm),byte(170*lm),100 );
	}

	float off = 5;
	glBegin( GL_POLYGON );
		glVertex3f( x+off, y, depth+add );
		glVertex3f( x, y+off, depth+add );
		glVertex3f( x, y+(h-off), depth+add );
		glVertex3f( x+off, y+h, depth+add );
		glVertex3f( x+(w-off), y+h, depth+add );
		glVertex3f( x+w, y+(h-off), depth+add );
		glVertex3f( x+w, y+off, depth+add );
		glVertex3f( x+(w-off), y, depth+add );
	glEnd();
	glDisable(GL_BLEND);

	add = add + 0.01f;
	float off2 = 2;
	float off3 = off;
	glColor3ub( 0,0,byte(40*lm) );
	glBegin( GL_LINES );
		glVertex3f( x+(w-off), y+off2, depth+add );
		glVertex3f( x+w-(off2), y+off3, depth+add );

		glVertex3f( x+w-(off2), y+off3, depth+add );
		glVertex3f( x+w-(off2), y+(h-off), depth+add );

		glVertex3f( x+w-(off2), y+(h-off), depth+add );
		glVertex3f( x+(w-off), y+h-(off2-1), depth+add );

		glVertex3f( x+(w-off), y+h-(off2-1), depth+add );
		glVertex3f( x+off, y+h-(off2-1), depth+add );

		glVertex3f( x+off, y+h-(off2-1), depth+add );
		glVertex3f( x+off2, y+(h-off), depth+add );

		glVertex3f( x+off2, y+(h-off), depth+add );
		glVertex3f( x+off2, y+off3, depth+add );

		glVertex3f( x+off2, y+off3, depth+add );
		glVertex3f( x+off, y+off2, depth+add );

		glVertex3f( x+off, y+off2, depth+add );
		glVertex3f( x+(w-off), y+off2, depth+add );
	glEnd();

	add = add + 0.01f;
	if ( state==csDisabled )
		glColor3ub( byte(50*lm),byte(50*lm),byte(50*lm) );
	else
		glColor3ub( byte(180*lm),byte(180*lm),byte(200*lm) );
	float wdth = float(text.length() / 2) * 5;
	Write( float(int(x + w*0.5f - wdth)), 
		   float(int(y + h*0.5f - fontSize*0.5f)),
		   depth+add*2, text );
};


void TGUI::MenuEdit( void )
{
	glEnable(GL_BLEND);
	if  ( state==csEnabled || state==csDisabled )
	{
		glColor4ub( byte(60*lm),byte(60*lm),byte(60*lm),100 );
	}
	else
	{
		glColor4ub( byte(120*lm),byte(120*lm),byte(120*lm),100 );
	}

	float add = 0.01f;
	glBegin( GL_QUADS );
		glVertex3f( x, y+h, depth+add );
		glVertex3f( x+w, y+h, depth+add );
		glVertex3f( x+w, y, depth+add );
		glVertex3f( x, y, depth+add );
	glEnd();
	glDisable(GL_BLEND);

	add = add + add;
	glColor3ub(0,0,0);
	glBegin( GL_LINES );
		glVertex3f( x, y, depth+add );
		glVertex3f( x+w, y, depth+add );

		glVertex3f( x+w, y, depth+add );
		glVertex3f( x+w, y+h, depth+add );

		glVertex3f( x+w, y+h, depth+add );
		glVertex3f( x, y+h, depth+add );

		glVertex3f( x, y+h, depth+add );
		glVertex3f( x, y, depth+add );
	glEnd();

	if ( state==csDisabled )
		glColor3ub( byte(50*lm),byte(50*lm),byte(50*lm) );
	else
		glColor3ub( byte(200*lm),byte(200*lm),byte(200*lm) );
	add = add * 1.5f;

	TString outText = text;
	cntr++;
	if ( cntr>10 )
	{
		cntr = 0;
	}
	if ( itemType==guiEdit && state==csSelected )
	{
		if ( cntr>5 )
		{
			outText = outText + "_";
		}
	}
	Write( x + 4, float(int(y + h*0.5f - fontSize*0.5f)), depth+add, outText );
};


void TGUI::MenuDropDown( void )
{
	MenuEdit();

	float add = 0.03f;
	float size = 10;
	glColor3ub(0,0,0);
	glBegin( GL_LINES );
		glVertex3f(x+w-size,y+h,depth+add);
		glVertex3f(x+w,y+h,depth+add);
		glVertex3f(x+w,y+h,depth+add);
		glVertex3f(x+w,y,depth+add);
		glVertex3f(x+w,y,depth+add);
		glVertex3f(x+w-size,y,depth+add);
	glEnd();

	glBegin( GL_TRIANGLES );
		glVertex3f(x+(w-size)+5,y+8,depth+add);
		glVertex3f(x+(w-size)+9,y+4,depth+add);
		glVertex3f(x+(w-size)+1,y+4,depth+add);
	glEnd();

	if ( state==csClicked )
	{
		// background box
		glEnable( GL_BLEND );
		glColor4ub( 20,20,20, 100 );

		add = add + 0.01f;
		float dropWidth = 140;
		float dropHeight = (float(numItems) * (fontSize+2)) + fontSize*0.5f;
		glBegin( GL_QUADS );
			glVertex3f( x+w+dropWidth, y, depth+add );
			glVertex3f( x+w, y, depth+add );
			glVertex3f( x+w, y+dropHeight, depth+add );
			glVertex3f( x+w+dropWidth, y+dropHeight, depth+add );
		glEnd();
		glDisable( GL_BLEND );

		add = add + 0.01f;
		glBegin( GL_LINES );
			glVertex3f( x+w+dropWidth, y, depth+add );
			glVertex3f( x+w, y, depth+add );
			glVertex3f( x+w, y, depth+add );
			glVertex3f( x+w, y+dropHeight, depth+add );
			glVertex3f( x+w, y+dropHeight, depth+add );
			glVertex3f( x+w+dropWidth, y+dropHeight, depth+add );
			glVertex3f( x+w+dropWidth, y+dropHeight, depth+add );
			glVertex3f( x+w+dropWidth, y, depth+add );
		glEnd();

		add = add + 0.01f;
		for ( size_t i=0; i<numItems; i++ )
		{
			float ypos = (y+(i*(fontSize+2))+2);
			glColor3ub( byte(110*lm),byte(110*lm),byte(110*lm) );
			if ( xp>=(x+w) && xp<=(x+w+dropWidth) )
			if ( yp>=ypos && yp<=(ypos+12) )
			{
				selectedItem = i;
			}
			if ( int(i)==selectedItem )
				glColor3ub( byte(200*lm),byte(200*lm),byte(200*lm) );

			Write( x+w+4, ypos, depth+add, dropDownList[i] );
		}
	}
};


void TGUI::MenuSlider( void )
{
	glDisable( GL_BLEND );

	float add = 0.01f;

	if ( state!=csEnabled )
		glColor3ub( byte(180*lm),byte(120*lm),byte(10*lm) );
	else
		glColor3ub( byte(120*lm),byte(80*lm),byte(10*lm) );

	float half = h * 0.5f;
	glLineWidth( 4 );
	glBegin( GL_LINES );
		glVertex3f( x,y+half,depth+add);
		glVertex3f( x+w,y+half,depth+add);
	glEnd();
	glLineWidth( 1 );

	if ( numMarkers>0 )
	{
		add = add + 0.01f;
		glColor3ub( 0,0,0 );
		glBegin( GL_LINES );
		float subDiv = w / float(numMarkers);
		for ( size_t i=0; i<=numMarkers; i++ )
		{
			glVertex3f( x+i*subDiv, y+half-4, depth+add );
			glVertex3f( x+i*subDiv, y+half+4, depth+add );
		};
		glEnd();
	};

	add = 0.03f;
	float subDiv = w / float(numDivisions);
	glColor3ub( byte(100*lm),byte(100*lm),byte(100*lm) );
	glBegin( GL_POLYGON );
		glVertex3f( x+(subDiv*sliderPos),   y+half-2, depth+add );
		glVertex3f( x+(subDiv*sliderPos)+2, y+half-4, depth+add );
		glVertex3f( x+(subDiv*sliderPos)+2, y+half-h*0.75f, depth+add );
		glVertex3f( x+(subDiv*sliderPos)-1, y+half-h*0.75f, depth+add );
		glVertex3f( x+(subDiv*sliderPos)-1, y+half-4, depth+add );
		glVertex3f( x+(subDiv*sliderPos),   y+half-2, depth+add );
	glEnd();
};


void TGUI::MenuCheckBox( void )
{
	float add = 0.01f;

	glEnable(GL_BLEND);
	if  ( state==csEnabled || state==csDisabled )
	{
		glColor4ub( byte(60*lm),byte(60*lm),byte(60*lm),100 );
	}
	else
	{
		glColor4ub( byte(120*lm),byte(120*lm),byte(120*lm),100 );
	}

	glBegin( GL_QUADS );
		glVertex3f( x, y, depth+add );
		glVertex3f( x, y+h, depth+add );
		glVertex3f( x+w, y+h, depth+add );
		glVertex3f( x+w, y, depth+add );
	glEnd();
	glDisable(GL_BLEND);

	add = add + 0.01f;
	glColor3ub( 0,0,0 );
	glBegin( GL_LINES );
		glVertex3f( x+2,y+2,depth+add );
		glVertex3f( x+h-2,y+2,depth+add );
		glVertex3f( x+h-2,y+2,depth+add );
		glVertex3f( x+h-2,y+h-2,depth+add );
		glVertex3f( x+h-2,y+h-2,depth+add );
		glVertex3f( x+2,y+h-2,depth+add );
		glVertex3f( x+2,y+h-2,depth+add );
		glVertex3f( x+2,y+2,depth+add );
	glEnd();

	if ( state==csDisabled )
		glColor3ub( byte(50*lm),byte(50*lm),byte(50*lm) );
	else
		glColor3ub( byte(255*lm),byte(255*lm),byte(255*lm) );
	if ( checked )
	{
		glBegin( GL_LINES );
			glVertex3f( x+3,y+3,depth+add );
			glVertex3f( x+h-3,y+h-3,depth+add );
			glVertex3f( x+h-3,y+3,depth+add );
			glVertex3f( x+3,y+h-3,depth+add );
		glEnd();
	}

	add = add + 0.01f;
	Write( x+h+2, y, depth+add, text );
};


void TGUI::MenuBitmap( void )
{
	float add = 0.01f;
	glDisable(GL_BLEND);
	glEnable( GL_COLOR_MATERIAL );

	if ( texture!=NULL )
	{
		glPushMatrix();
		glTranslatef( 0,0,depth+add );
		texture->DrawCCW( x,y,x+w,x+h, u0,v0, u1,v1 );
		glPopMatrix();
	}
};


void TGUI::MenuCompoundObject( void )
{
	glPushMatrix();
	app->SetupGL2d( app->vdepth, app->vwidth, app->vheight );
	glLoadIdentity();

	glDisable(GL_BLEND);
	glEnable( GL_COLOR_MATERIAL );

	if ( compoundObject!=NULL )
	{
		TBinObject** obj = compoundObject->Objects();
		TBinObject* t = obj[0];
//		glRotatef( xa, 1,0,0 );
//		glRotatef( za, 0,0,1 );
		glTranslatef( x+xpos, (app->Height()-ypos)-y, depth);
		glRotatef( ya, 0,1,0 );
		glScalef( scale,scale,scale );
		t->Draw((bool)false);
	}
	if ( character!=NULL )
	{
		glTranslatef( x+xpos, (app->Height()-ypos)-y, depth);
		glRotatef( ya, 0,1,0 );
		glScalef( scale,scale,scale );
		character->Draw();
	}
	app->SetupGL2dNormal( app->vdepth, app->vwidth, app->vheight );
	glPopMatrix();
};

void TGUI::MenuListbox( void )
{
	glEnable(GL_BLEND);
	if  ( state==csEnabled || state==csDisabled )
	{
		glColor4ub( byte(60*lm),byte(60*lm),byte(60*lm),100 );
	}
	else
	{
		glColor4ub( byte(120*lm),byte(120*lm),byte(120*lm),100 );
	}

	float add = 0.01f;
	glBegin( GL_QUADS );
		glVertex3f( x, y+h, depth+add );
		glVertex3f( x+w, y+h, depth+add );
		glVertex3f( x+w, y, depth+add );
		glVertex3f( x, y, depth+add );
	glEnd();
	glDisable(GL_BLEND);

	add = add + add;
	glColor3ub(0,0,0);
	glBegin( GL_LINES );
		glVertex3f( x, y, depth+add );
		glVertex3f( x+w, y, depth+add );

		glVertex3f( x+w, y, depth+add );
		glVertex3f( x+w, y+h, depth+add );

		glVertex3f( x+w, y+h, depth+add );
		glVertex3f( x, y+h, depth+add );

		glVertex3f( x, y+h, depth+add );
		glVertex3f( x, y, depth+add );
	glEnd();

	if ( state==csDisabled )
		glColor3ub( byte(50*lm),byte(50*lm),byte(50*lm) );
	else
		glColor3ub( byte(200*lm),byte(200*lm),byte(200*lm) );
	add = add * 1.5f;

	float dropWidth = w - 10;
	for ( size_t i=0; i<(numItems/2); i++ )
	{
		float ypos = (y+(i*2*(fontSize+2))+2);
		glColor3ub( byte(110*lm),byte(110*lm),byte(110*lm) );
		if ( xp>=(x) && xp<=(x+dropWidth) )
		if ( yp>=ypos && yp<=(ypos+12) )
		{
			selectedItem = i;
		}
		if ( int(i)==selectedItem )
		{
			glColor3ub( byte(200*lm),byte(200*lm),byte(200*lm) );
		}

		Write( x+4, ypos, depth+add, dropDownList[i*2+0] );
		Write( x+4, ypos+fontSize, depth+add, dropDownList[i*2+1] );
	}
};

//==========================================================================

