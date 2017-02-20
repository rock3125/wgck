#include <precomp_header.h>

#include <object/landscapeObject.h>
#include <object/geometry.h>

//==========================================================================

TLandscapeObject::TLandscapeObject( const TString& _name,
									float _tx, float _ty, float _tz,
									float _rx, float _ry, float _rz )
	: obj(NULL)
{
	Name( _name );

	tx = _tx;
	ty = _ty;
	tz = _tz;

	rx = _rx;
	ry = _ry;
	rz = _rz;

	maxIndex = 0;
	currentIndex = 0;
	armour = 0;
	damage = 0;
}


TLandscapeObject::TLandscapeObject( void )
	: obj(NULL)
{
	tx = 0;
	ty = 0;
	tz = 0;

	rx = 0;
	ry = 0;
	rz = 0;

	currentIndex = 0;
	armour = 0;
	damage = 0;
}


TLandscapeObject::~TLandscapeObject( void )
{
	obj = NULL;
	maxIndex = 0;
	currentIndex = 0;
}


const TString& TLandscapeObject::Name( void ) const
{
	return name;
};


void TLandscapeObject::Name( const TString& _name )
{
	name = _name;
};


void TLandscapeObject::SetTranslation( float x, float y, float z )
{
	tx = x;
	ty = y;
	tz = z;
}


void TLandscapeObject::GetTranslation( float& x, float& y, float& z ) const
{
	x = tx;
	y = ty;
	z = tz;
}


void TLandscapeObject::SetRotation( float x, float y, float z )
{
	rx = x;
	ry = y;
	rz = z;
}


void TLandscapeObject::GetRotation( float& x, float& y, float& z ) const
{
	x = rx;
	y = ry;
	z = rz;
}


const TCompoundObject* TLandscapeObject::Object( void ) const
{
	return obj;
}


void TLandscapeObject::Object( const TCompoundObject* _obj )
{
	PreCond( _obj!=NULL );
	PreCond( _obj->ValidObject() );
	PreCond( _obj->Objects()!=NULL );
	obj = _obj;
	maxIndex = _obj->NumObjects();
}


TLandscapeObject::TLandscapeObject( const TLandscapeObject& o )
{
	operator=(o);
};


const TLandscapeObject& TLandscapeObject::operator=( const TLandscapeObject& o )
{
	Name( o.name );

	PreCond( o.obj!=NULL );
	obj = o.obj;

	tx = o.tx;
	ty = o.ty;
	tz = o.tz;

	rx = o.rx;
	ry = o.ry;
	rz = o.rz;

	currentIndex = o.currentIndex;
	armour = o.armour;
	damage = o.damage;
	maxIndex = o.maxIndex;

	return *this;
}


bool TLandscapeObject::InsideObstacle( float x, float z ) const
{
	float minx,minz;
	float maxx,maxz;

	TCompoundObject& object = *const_cast<TCompoundObject*>(obj);

	minx = tx + obj->MinX(currentIndex);
	minz = tz + object.MinZ(currentIndex);

	maxx = tx + object.MaxX(currentIndex);
	maxz = tz + object.MaxZ(currentIndex);

	float y = 0;
	if ( PointInTriangle(x,y,z, minx,0,minz, minx,0,maxz, maxx,0,minz ) ||
		 PointInTriangle(x,y,z, maxx,0,maxz, minx,0,maxz, maxx,0,minz ) )
		 return true;
/*
	if ( x>=minx && x<=maxx )
	if ( z>=minz && z<=maxz )
		return true;
*/
	return false;
};


bool TLandscapeObject::InsideObstacle( float x, float y, float z )
{
	float minx,miny,minz;
	float maxx,maxy,maxz;

	TCompoundObject& object = *const_cast<TCompoundObject*>(obj);

	minx = tx + object.MinX(currentIndex);
	miny = ty + object.MinY(currentIndex);
	minz = tz + object.MinZ(currentIndex);

	maxx = tx + object.MaxX(currentIndex);
	maxy = ty + object.MaxY(currentIndex);
	maxz = tz + object.MaxZ(currentIndex);

	if ( x>=minx && x<=maxx )
	if ( y>=miny && y<=maxy )
	if ( z>=minz && z<=maxz )
	{
		return true;
	}
	return false;
};


void TLandscapeObject::SaveBinary( TPersist& file ) const
{
	PreCond( name!=NULL );
	file.FileWrite( name.c_str(),name.length()+1);

	file.FileWrite( &tx, sizeof(float) );
	file.FileWrite( &ty, sizeof(float) );
	file.FileWrite( &tz, sizeof(float) );

	file.FileWrite( &rx, sizeof(float) );
	file.FileWrite( &ry, sizeof(float) );
	file.FileWrite( &rz, sizeof(float) );

	file.FileWrite( &currentIndex, sizeof(size_t) );
	file.FileWrite( &armour, sizeof(float) );
	file.FileWrite( &damage, sizeof(float) );
};


bool TLandscapeObject::LoadBinary( TPersist& file, TString& errStr )
{
	TString _name;
	size_t i=0;
	char ch;
	do
	{
		file.FileRead( &ch, 1 );
		_name = _name + TString(ch);
		i++;
	}
	while ( ch!=0 && i<255 );

	if ( ch!=0 )
	{
		errStr = "Error: malformed LandscapeObject\n";
		return false;
	}

	Name( _name );

	file.FileRead( &tx, sizeof(float) );
	file.FileRead( &ty, sizeof(float) );
	file.FileRead( &tz, sizeof(float) );

	file.FileRead( &rx, sizeof(float) );
	file.FileRead( &ry, sizeof(float) );
	file.FileRead( &rz, sizeof(float) );

	file.FileRead( &currentIndex, sizeof(size_t) );
	file.FileRead( &armour, sizeof(float) );
	file.FileRead( &damage, sizeof(float) );

	return true;
};


const float& TLandscapeObject::TX( void ) const
{
	return tx;
};


const float& TLandscapeObject::TY( void ) const
{
	return ty;
};


const float& TLandscapeObject::TZ( void ) const
{
	return tz;
};


float TLandscapeObject::SizeX( void ) const
{
	return obj->SizeX(currentIndex)*4;
};


float TLandscapeObject::SizeY( void ) const
{
	return obj->SizeY(currentIndex)*4;
};


float TLandscapeObject::SizeZ( void ) const
{
	return obj->SizeZ(currentIndex)*4;
};


void TLandscapeObject::Draw( void ) const
{
	if ( currentIndex>=maxIndex ) 
		return;

	TCompoundObject& object = *const_cast<TCompoundObject*>(obj);

	// translate - align bottoms of objects
	float diff = object.MinY(0) - object.MinY(currentIndex);

	glPushMatrix();
	glTranslatef( tx, ty+diff, tz );
	glRotatef( 90.0f-ry, 0,1,0 );

	object.Draw(currentIndex);
	glPopMatrix();
};


size_t TLandscapeObject::CurrentIndex( void ) const
{
	return currentIndex;
};


size_t TLandscapeObject::MaxIndex( void ) const
{
	return maxIndex;
};


void TLandscapeObject::CurrentIndex( size_t _currentIndex )
{
	PreCond( obj!=NULL );
	currentIndex =  _currentIndex;
};

//==========================================================================

TLandscapeObjectList::TLandscapeObjectList( const TLandscapeObject& _obj )
	: obj( &_obj ),
	  next(NULL)
{
};


TLandscapeObjectList::TLandscapeObjectList( void )
	: obj( NULL ),
	  next(NULL)
{
};


TLandscapeObjectList::~TLandscapeObjectList( void )
{
	obj = NULL;
	next = NULL;
};


const TLandscapeObject& TLandscapeObjectList::Object( void ) const
{
	return *obj;
};


void TLandscapeObjectList::Object( const TLandscapeObject& _obj )
{
	obj = &_obj;
};


TLandscapeObjectList* TLandscapeObjectList::Next( void ) const
{
	return next;
};


void TLandscapeObjectList::Next( TLandscapeObjectList* _next )
{
	next = _next;
};


void TLandscapeObjectList::Append( const TLandscapeObject& _obj )
{
	TLandscapeObjectList* app = new TLandscapeObjectList( _obj );
	if ( next==NULL )
	{
		next = app;
	}
	else
	{
		TLandscapeObjectList* ll = next;
		while ( ll->next!=NULL )
			ll = ll->next;
		ll->next = app;
	}
};


void TLandscapeObjectList::Remove( const TLandscapeObject& _obj )
{
	TLandscapeObjectList* ll = next;
	TLandscapeObjectList* prev = NULL;
	while ( ll!=NULL && ll->obj!=&_obj )
	{
		prev = ll;
		ll = ll->next;
	}
	if ( ll!=NULL )
	{
		if ( ll->obj==&_obj )
		{
			if ( prev==NULL )
			{
				prev = ll;
				next = ll->next;
				delete ll;
			}
			else
			{
				prev->next = ll->next;
				delete ll;
			}
		}
	}
};


void TLandscapeObjectList::Clear( void )
{
	TLandscapeObjectList* ll = next;
	TLandscapeObjectList* prev;
	while ( ll!=NULL )
	{
		prev = ll;
		ll = ll->next;
		delete prev;
	};
	next = NULL;
};


TLandscapeObject* TLandscapeObjectList::GetObject( const TString& name ) const
{
	TLandscapeObjectList* ll = next;
	while ( ll!=NULL )
	{
		if ( ll->Object().Name()==name )
			return const_cast<TLandscapeObject*>(&ll->Object());
		ll = ll->next;
	};
	return NULL;
};

//==========================================================================
