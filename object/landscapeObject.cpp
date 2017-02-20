#include <precomp_header.h>

#include <object/landscape.h>
#include <object/landscapeObject.h>
#include <object/geometry.h>

#include <tank/tankapp.h>

//==========================================================================

const size_t landscapeObjectVersion = 2;

//==========================================================================

TLandscapeObject::TLandscapeObject( const TString& _name,
								    size_t _id,
									float _tx, float _ty, float _tz,
									float _rx, float _ry, float _rz )
	: obj(NULL)
{
	PreCond( _id>0 );

	Name( _name );
	id = _id;

	tx = _tx;
	ty = _ty;
	tz = _tz;

	rx = _rx;
	ry = _ry;
	rz = _rz;

	scaleX = 1;
	scaleY = 1;
	scaleZ = 1;

	minx = miny = minz = 0;
	maxx = maxy = maxz = 0;

	b1x = b2x = b3x = b4x = 0;
	b5x = b6x = b7x = b8x = 0;
	b1y = b2y = b3y = b4y = 0;
	b5y = b6y = b7y = b8y = 0;
	b1z = b2z = b3z = b4z = 0;
	b5z = b6z = b7z = b8z = 0;

	maxIndex = 0;
	currentIndex = 0;
	destructable = true;
	isaAI = false;
	aiType = 0;
	aiTeam = 0;
	gameImportance = 0;
	strength = 100;
	strengthUpdated = false;
	points = 0;
	bombs = 5;
	shells = 25;
	armory = false;
	repairs = false;
	refuelStation = false;
	characterType = 0;

	for ( size_t i=0; i<16; i++ )
		matrix[i] = 0;
	matrix[0] = 1;
	matrix[5] = 1;
	matrix[10] = 1;
	matrix[15] = 1;
}


TLandscapeObject::TLandscapeObject( void )
	: obj(NULL)
{
	id = 0;

	tx = 0;
	ty = 0;
	tz = 0;

	rx = 0;
	ry = 0;
	rz = 0;

	scaleX = 1;
	scaleY = 1;
	scaleZ = 1;

	minx = miny = minz = 0;
	maxx = maxy = maxz = 0;

	b1x = b2x = b3x = b4x = 0;
	b5x = b6x = b7x = b8x = 0;
	b1y = b2y = b3y = b4y = 0;
	b5y = b6y = b7y = b8y = 0;
	b1z = b2z = b3z = b4z = 0;
	b5z = b6z = b7z = b8z = 0;

	currentIndex = 0;
	maxIndex = 0;
	destructable = true;
	isaAI = false;
	aiType = 0;
	aiTeam = 0;
	gameImportance = 0;
	strength = 100;
	strengthUpdated = false;
	points = 0;
	bombs = 5;
	shells = 20;
	armory = false;
	repairs = false;
	refuelStation = false;
	characterType = 0;

	for ( size_t i=0; i<16; i++ )
		matrix[i] = 0;
	matrix[0] = 1;
	matrix[5] = 1;
	matrix[10] = 1;
	matrix[15] = 1;
}


TLandscapeObject::TLandscapeObject( const TLandscapeObject& o )
	: obj(NULL)
{
	id = 0;

	tx = 0;
	ty = 0;
	tz = 0;

	rx = 0;
	ry = 0;
	rz = 0;

	scaleX = 1;
	scaleY = 1;
	scaleZ = 1;

	minx = miny = minz = 0;
	maxx = maxy = maxz = 0;

	b1x = b2x = b3x = b4x = 0;
	b5x = b6x = b7x = b8x = 0;
	b1y = b2y = b3y = b4y = 0;
	b5y = b6y = b7y = b8y = 0;
	b1z = b2z = b3z = b4z = 0;
	b5z = b6z = b7z = b8z = 0;

	currentIndex = 0;
	maxIndex = 0;
	destructable = true;
	gameImportance = 0;
	isaAI = false;
	aiType = 0;
	aiTeam = 0;
	gameImportance = 0;
	strength = 100;
	strengthUpdated = false;
	points = 0;
	bombs = 5;
	shells = 20;
	characterType = 0;

	for ( size_t i=0; i<16; i++ )
		matrix[i] = 0;
	matrix[0] = 1;
	matrix[5] = 1;
	matrix[10] = 1;
	matrix[15] = 1;

	operator=(o);
};


const TLandscapeObject& TLandscapeObject::operator=( const TLandscapeObject& o )
{
	name = o.name;

	id = o.id;
	obj = o.obj;

	tx = o.tx;
	ty = o.ty;
	tz = o.tz;

	rx = o.rx;
	ry = o.ry;
	rz = o.rz;

	scaleX = o.scaleX;
	scaleY = o.scaleY;
	scaleZ = o.scaleZ;

	currentIndex = o.currentIndex;
	maxIndex = o.maxIndex;

	armory = o.armory;
	refuelStation = o.refuelStation;
	repairs = o.repairs;

	minx = o.minx;
	miny = o.miny;
	minz = o.minz;
	maxx = o.maxx;
	maxy = o.maxy;
	maxz = o.maxz;

	b1x = o.b1x;
	b1y = o.b1y;
	b1z = o.b1z;
	b2x = o.b2x;
	b2y = o.b2y;
	b2z = o.b2z;
	b3x = o.b3x;
	b3y = o.b3y;
	b3z = o.b3z;
	b4x = o.b4x;
	b4y = o.b4y;
	b4z = o.b4z;
	b5x = o.b5x;
	b5y = o.b5y;
	b5z = o.b5z;
	b6x = o.b6x;
	b6y = o.b6y;
	b6z = o.b6z;
	b7x = o.b7x;
	b7y = o.b7y;
	b7z = o.b7z;
	b8x = o.b8x;
	b8y = o.b8y;
	b8z = o.b8z;

	destructable = o.destructable;
	isaAI = o.isaAI;
	aiType = o.aiType;
	aiTeam = o.aiTeam;
	gameImportance = o.gameImportance;
	strength = o.strength;
	strengthUpdated = o.strengthUpdated;
	points = o.points;
	bombs = o.bombs;
	shells = o.shells;
	characterType = o.characterType;

	for ( size_t i=0; i<16; i++ )
		matrix[i] = o.matrix[i];

	StrengthToIndex();

	return *this;
}


TLandscapeObject::~TLandscapeObject( void )
{
	obj = NULL;
	maxIndex = 0;
	currentIndex = 0;
}


void TLandscapeObject::StrengthToIndex( void )
{
	if ( maxIndex>0 )
	{
		size_t div = 100 / maxIndex;
		if ( div>0 )
		{
			size_t min = (strength / div);
			if ( min>(maxIndex-1) )
				currentIndex = 0;
			else
				currentIndex = (maxIndex-1) - min;
		}
	}
};


const TString& TLandscapeObject::Name( void ) const
{
	return name;
};


void TLandscapeObject::Name( const TString& _name )
{
	name = _name;
};


size_t TLandscapeObject::Id( void ) const
{
	return id;
};


void TLandscapeObject::Id( size_t _id )
{
	id = _id;
};


void TLandscapeObject::SetTranslation( float x, float y, float z )
{
	if ( tx!=x || ty!=y || tz!=z )
	{
		tx = x;
		ty = y;
		tz = z;
		GetTransformationMatrix();
	}
}


void TLandscapeObject::GetTranslation( float& x, float& y, float& z ) const
{
	x = tx;
	y = ty;
	z = tz;
}


void TLandscapeObject::SetRotation( float x, float y, float z )
{
	if ( rx!=x || ry!=y || rz!=z )
	{
		rx = x;
		ry = y;
		rz = z;
		GetTransformationMatrix();
	}
}


void TLandscapeObject::GetRotation( float& x, float& y, float& z ) const
{
	x = rx;
	y = ry;
	z = rz;
}


void TLandscapeObject::SetScale( float x, float y, float z )
{
	if ( scaleX!=x || scaleY!=y || scaleZ!=z )
	{
		scaleX = x;
		scaleY = y;
		scaleZ = z;
		GetTransformationMatrix();
	}
}


void TLandscapeObject::GetScale( float& x, float& y, float& z ) const
{
	x = scaleX;
	y = scaleY;
	z = scaleZ;
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

	GetTransformationMatrix();
}


bool TLandscapeObject::InsideObstacle( float x, float y, float z ) const
{
	if ( currentIndex>=maxIndex )
		return false;

	TPoint point(x,y,z);
	TPoint base(b1x,b1y,b1z);
	TPoint v1(b2x-b1x,b2y-b1y,b2z-b1z);
	TPoint v2(b3x-b1x,b3y-b1y,b3z-b1z);
	TPoint v3(b5x-b1x,b5y-b1y,b5z-b1z);
	return PointInsideBox( base, v1,v2,v3, point );
};

/*
bool TLandscapeObject::LineInsideObject( float x1, float z1, float x2, float z2 ) const
{
	if ( currentIndex>=maxIndex )
		return false;

	float min[3];
	float max[3];
	GetBounds(min,max);

	float t1[3], t2[3], t3[3], p1[3], p2[3];
	p1[0] = x1;
	p1[1] = 0;
	p1[2] = z1;

	p2[0] = x2;
	p2[1] = 0;
	p2[2] = z2;

	t1[0] = minx;
	t1[1] = 0;
	t1[2] = minz;

	t2[0] = maxx;
	t2[1] = 0;
	t2[2] = minz;

	t3[0] = maxx;
	t3[1] = 0;
	t3[2] = maxz;

	if ( LineIntersectsTriangle2( p1,p2, t1,t2,t3) )
	{
		return true;
	}

	t1[2] = maxz;
	t2[0] = minx;
	t3[2] = minz;
	if ( LineIntersectsTriangle2( p1,p2, t1,t2,t3 ) )
	{
		return true;
	}
	return false;
};
*/

bool TLandscapeObject::InsideObject( float x1, float z1 ) const
{
	if ( currentIndex>=maxIndex )
		return false;

	float minx,miny,minz;
	float maxx,maxy,maxz;
	GetBounds(minx,miny,minz,maxx,maxy,maxz);

	return ( minx<=x1 && x1<=maxx && minz<=z1 && z1<=maxz );
};


bool TLandscapeObject::InsideObject( float* x1, float* z1 ) const
{
	if ( currentIndex>=maxIndex )
		return false;

	float minx,miny,minz;
	float maxx,maxy,maxz;
	GetBounds(minx,miny,minz,maxx,maxy,maxz);

	return ( minx<=x1[0] && x1[0]<=maxx && minz<=z1[0] && z1[0]<=maxz &&
			 minx<=x1[1] && x1[1]<=maxx && minz<=z1[1] && z1[1]<=maxz &&
			 minx<=x1[2] && x1[2]<=maxx && minz<=z1[2] && z1[2]<=maxz &&
			 minx<=x1[3] && x1[3]<=maxx && minz<=z1[3] && z1[3]<=maxz );
};


bool TLandscapeObject::GetLORTriangle( float xp, float yp, float zp, 
									   float size, float& y )
{
	if ( currentIndex>=maxIndex )
		return false;

	TCompoundObject& object = *const_cast<TCompoundObject*>(obj);
	bool found = object.GetLORTriangle( currentIndex, matrix, 
										xp,yp,zp, size, y );
	return found;
};


bool TLandscapeObject::Collision( float l1x, float l1y, float l1z,
								  float l2x, float l2y, float l2z ) const
{
	if ( currentIndex>=maxIndex )
		return false;

	TCompoundObject& object = *const_cast<TCompoundObject*>(obj);
	return object.Collision( currentIndex, matrix,
							 l1x,l1y,l1z, 
							 l2x,l2y,l2z );
};


bool TLandscapeObject::Collision( float l1x, float l1z,
								  float l2x, float l2z ) const
{
	if ( currentIndex>=maxIndex )
		return false;

	TCompoundObject& object = *const_cast<TCompoundObject*>(obj);
	return object.Collision( currentIndex, matrix,
							 l1x,l1z, 
							 l2x,l2z );
};


void TLandscapeObject::DrawBoundingBox( float r, float g, float b )
{
	if ( currentIndex>=maxIndex )
		return;

	::DrawBoundingBox( b1x,b1y,b1z, b2x,b2y,b2z, b3x,b3y,b3z, b4x,b4y,b4z,
					   b5x,b5y,b5z, b6x,b6y,b6z, b7x,b7y,b7z, b8x,b8y,b8z, r,g,b );
};


void TLandscapeObject::SaveBinary( TPersist& file ) const
{
	PreCond( name!=NULL );

	file.FileWrite( &landscapeObjectVersion, sizeof(size_t) );

	file.FileWrite( name.c_str(),name.length()+1);

	file.FileWrite( &id, sizeof(size_t) );

	file.FileWrite( &tx, sizeof(float) );
	file.FileWrite( &ty, sizeof(float) );
	file.FileWrite( &tz, sizeof(float) );

	file.FileWrite( &rx, sizeof(float) );
	file.FileWrite( &ry, sizeof(float) );
	file.FileWrite( &rz, sizeof(float) );

	file.FileWrite( &scaleX, sizeof(float) );
	file.FileWrite( &scaleY, sizeof(float) );
	file.FileWrite( &scaleZ, sizeof(float) );

	file.FileWrite( &currentIndex, sizeof(size_t) );

	file.FileWrite( &destructable, sizeof(bool) );
	file.FileWrite( &isaAI, sizeof(bool) );
	file.FileWrite( &aiType, sizeof(size_t) );
	file.FileWrite( &aiTeam, sizeof(size_t) );
	file.FileWrite( &gameImportance, sizeof(size_t) );
	file.FileWrite( &strength, sizeof(size_t) );
	file.FileWrite( &points, sizeof(size_t) );
	file.FileWrite( &bombs, sizeof(size_t) );
	file.FileWrite( &shells, sizeof(size_t) );

	file.FileWrite( &armory, sizeof(bool) );
	file.FileWrite( &refuelStation, sizeof(bool) );
	file.FileWrite( &repairs, sizeof(bool) );

	file.FileWrite( &characterType, sizeof(size_t) );
};


bool TLandscapeObject::LoadBinary( TPersist& file, TString& errStr )
{
	// get version number
	size_t version;
	file.FileRead( &version, sizeof(size_t) );

	// get name string
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

	file.FileRead( &id, sizeof(size_t) );

	file.FileRead( &tx, sizeof(float) );
	file.FileRead( &ty, sizeof(float) );
	file.FileRead( &tz, sizeof(float) );

	file.FileRead( &rx, sizeof(float) );
	file.FileRead( &ry, sizeof(float) );
	file.FileRead( &rz, sizeof(float) );

	file.FileRead( &scaleX, sizeof(float) );
	file.FileRead( &scaleY, sizeof(float) );
	file.FileRead( &scaleZ, sizeof(float) );

	file.FileRead( &currentIndex, sizeof(size_t) );

	file.FileRead( &destructable, sizeof(bool) );
	file.FileRead( &isaAI, sizeof(bool) );
	file.FileRead( &aiType, sizeof(size_t) );
	file.FileRead( &aiTeam, sizeof(size_t) );
	file.FileRead( &gameImportance, sizeof(size_t) );
	file.FileRead( &strength, sizeof(size_t) );
	file.FileRead( &points, sizeof(size_t) );
	file.FileRead( &bombs, sizeof(size_t) );
	file.FileRead( &shells, sizeof(size_t) );

	file.FileRead( &armory, sizeof(bool) );
	file.FileRead( &refuelStation, sizeof(bool) );
	file.FileRead( &repairs, sizeof(bool) );

	if ( version>=2 )
	{
		file.FileRead( &characterType, sizeof(size_t) );
	}

	GetTransformationMatrix();

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


void TLandscapeObject::TX( float _tx )
{
	if ( tx!=_tx )
	{
		tx = _tx;
		GetTransformationMatrix();
	}
}


void TLandscapeObject::TY( float _ty )
{
	if ( ty!=_ty )
	{
		ty = _ty;
		GetTransformationMatrix();
	}
}


void TLandscapeObject::TZ( float _tz )
{
	if ( tz!=_tz )
	{
		tz = _tz;
		GetTransformationMatrix();
	}
}


const float& TLandscapeObject::TZ( void ) const
{
	return tz;
};


float TLandscapeObject::SizeX( void ) const
{
	if ( currentIndex>=maxIndex )
		return 0;

	return obj->SizeX(currentIndex);
};


float TLandscapeObject::SizeY( void ) const
{
	if ( currentIndex>=maxIndex )
		return 0;

	return obj->SizeY(currentIndex);
};


float TLandscapeObject::SizeZ( void ) const
{
	if ( currentIndex>=maxIndex )
		return 0;

	return obj->SizeZ(currentIndex);
};


void TLandscapeObject::Draw( void ) const
{
	if ( maxIndex>1 )
	{
		float ci = float(100-strength) / 100.0f;
		currentIndex = size_t(maxIndex * ci);
	}
	if ( currentIndex>=maxIndex || strength==0 ) 
		return;

	TCompoundObject& object = *const_cast<TCompoundObject*>(obj);
	glPushMatrix();
		glTranslatef( tx, ty, tz );
		glRotatef( rz, 0,0,1 );
		glRotatef( ry, 0,1,0 );
		glRotatef( rx, 1,0,0 );
		glScalef( scaleX, scaleY, scaleZ );
		object.Draw(currentIndex);
	glPopMatrix();
};


void TLandscapeObject::Draw( bool drawTransp ) const
{
	if ( currentIndex>=maxIndex ) 
		return;

	TCompoundObject& object = *const_cast<TCompoundObject*>(obj);
	glPushMatrix();
		glTranslatef( tx, ty, tz );
		glRotatef( rz, 0,0,1 );
		glRotatef( ry, 0,1,0 );
		glRotatef( rx, 1,0,0 );
		glScalef( scaleX, scaleY, scaleZ );
		object.Draw(currentIndex,drawTransp);
	glPopMatrix();
};


void TLandscapeObject::GetTransformationMatrix( void )
{
	glPushMatrix();
		glLoadIdentity();
		glTranslatef( tx, ty, tz );
		glRotatef( rz, 0,0,1 );
		glRotatef( ry, 0,1,0 );
		glRotatef( rx, 1,0,0 );
		glScalef( scaleX, scaleY, scaleZ );
		glGetFloatv( GL_MODELVIEW_MATRIX, matrix );
	glPopMatrix();

	// important - recalculate boundaries for the new matrix
	CalculateBounds();
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

	// recalculate new boundaries
	CalculateBounds();
};


void TLandscapeObject::CalculateBounds( void )
{
	if ( obj==NULL || currentIndex>=maxIndex )
		return;
	TCompoundObject& object = *const_cast<TCompoundObject*>(obj);

	// all are equal in bounds - so object's 0 bounds are fine
	object.CalculateBoundaries(0);
	object.GetBounds(0,minx,miny,minz,maxx,maxy,maxz);

	float adjust = 0.1f;
	float _minx,_miny,_minz,_maxx,_maxy,_maxz;
	float a,b,c;

	a = minx - adjust;
	b = miny - adjust;
	c = minz - adjust;
	MatrixMult( matrix, a,b,c );
	b1x = a;
	b1y = b;
	b1z = c;
	_minx = a;
	_miny = b;
	_minz = c;
	_maxx = a;
	_maxy = b;
	_maxz = c;

	a = maxx + adjust;
	b = miny - adjust;
	c = minz - adjust;
	MatrixMult( matrix, a,b,c );
	b2x = a;
	b2y = b;
	b2z = c;
	if ( a>_maxx )
		_maxx = a;
	if ( a<_minx )
		_minx = a;
	if ( b>_maxy )
		_maxy = b;
	if ( b<_miny )
		_miny = b;
	if ( c>_maxz )
		_maxz = c;
	if ( c<_minz )
		_minz = c;

	a = minx - adjust;
	b = maxy + adjust;
	c = minz - adjust;
	MatrixMult( matrix, a,b,c );
	b3x = a;
	b3y = b;
	b3z = c;
	if ( a>_maxx )
		_maxx = a;
	if ( a<_minx )
		_minx = a;
	if ( b>_maxy )
		_maxy = b;
	if ( b<_miny )
		_miny = b;
	if ( c>_maxz )
		_maxz = c;
	if ( c<_minz )
		_minz = c;

	a = maxx + adjust;
	b = maxy + adjust;
	c = minz - adjust;
	MatrixMult( matrix, a,b,c );
	b4x = a;
	b4y = b;
	b4z = c;
	if ( a>_maxx )
		_maxx = a;
	if ( a<_minx )
		_minx = a;
	if ( b>_maxy )
		_maxy = b;
	if ( b<_miny )
		_miny = b;
	if ( c>_maxz )
		_maxz = c;
	if ( c<_minz )
		_minz = c;

	a = minx - adjust;
	b = miny - adjust;
	c = maxz + adjust;
	MatrixMult( matrix, a,b,c );
	b5x = a;
	b5y = b;
	b5z = c;
	if ( a>_maxx )
		_maxx = a;
	if ( a<_minx )
		_minx = a;
	if ( b>_maxy )
		_maxy = b;
	if ( b<_miny )
		_miny = b;
	if ( c>_maxz )
		_maxz = c;
	if ( c<_minz )
		_minz = c;

	a = maxx + adjust;
	b = miny - adjust;
	c = maxz + adjust;
	MatrixMult( matrix, a,b,c );
	b6x = a;
	b6y = b;
	b6z = c;
	if ( a>_maxx )
		_maxx = a;
	if ( a<_minx )
		_minx = a;
	if ( b>_maxy )
		_maxy = b;
	if ( b<_miny )
		_miny = b;
	if ( c>_maxz )
		_maxz = c;
	if ( c<_minz )
		_minz = c;

	a = minx - adjust;
	b = maxy + adjust;
	c = maxz + adjust;
	MatrixMult( matrix, a,b,c );
	b7x = a;
	b7y = b;
	b7z = c;
	if ( a>_maxx )
		_maxx = a;
	if ( a<_minx )
		_minx = a;
	if ( b>_maxy )
		_maxy = b;
	if ( b<_miny )
		_miny = b;
	if ( c>_maxz )
		_maxz = c;
	if ( c<_minz )
		_minz = c;

	a = maxx + adjust;
	b = maxy + adjust;
	c = maxz + adjust;
	MatrixMult( matrix, a,b,c );
	b8x = a;
	b8y = b;
	b8z = c;
	if ( a>_maxx )
		_maxx = a;
	if ( a<_minx )
		_minx = a;
	if ( b>_maxy )
		_maxy = b;
	if ( b<_miny )
		_miny = b;
	if ( c>_maxz )
		_maxz = c;
	if ( c<_minz )
		_minz = c;

	minx = _minx;
	miny = _miny;
	minz = _minz;
	maxx = _maxx;
	maxy = _maxy;
	maxz = _maxz;
};


void TLandscapeObject::GetBounds( float& _minx, float& _miny, float& _minz,
								  float& _maxx, float& _maxy, float& _maxz ) const
{
	_minx = minx;
	_miny = miny;
	_minz = minz;

	_maxx = maxx;
	_maxy = maxy;
	_maxz = maxz;
}


void TLandscapeObject::GetBounds( float* min, float* max ) const
{
	min[0] = minx;
	min[1] = miny;
	min[2] = minz;

	max[0] = maxx;
	max[1] = maxy;
	max[2] = maxz;
}


void TLandscapeObject::GetGround( float& x1, float& z1, float& x2, float& z2,
								  float& x3, float& z3, float& x4, float& z4 ) const
{
	x1 = b1x;
	z1 = b1z;
	x2 = b2x;
	z2 = b2z;
	x3 = b5x;
	z3 = b5z;
	x4 = b6x;
	z4 = b6z;
};


void TLandscapeObject::GetCorners( float& x1, float& y1, float& z1, 
								   float& x2, float& y2, float& z2,
								   float& x3, float& y3, float& z3,
								   float& x4, float& y4, float& z4,
								   float& x5, float& y5, float& z5,
								   float& x6, float& y6, float& z6,
								   float& x7, float& y7, float& z7,
								   float& x8, float& y8, float& z8 ) const
{
	x1 = b1x;
	y1 = b1y;
	z1 = b1z;
	x2 = b2x;
	y2 = b2y;
	z2 = b2z;
	x3 = b3x;
	y3 = b3y;
	z3 = b3z;
	x4 = b4x;
	y4 = b4y;
	z4 = b4z;

	x5 = b5x;
	y5 = b5y;
	z5 = b5z;
	x6 = b6x;
	y6 = b6y;
	z6 = b6z;
	x7 = b7x;
	y7 = b7y;
	z7 = b7z;
	x8 = b8x;
	y8 = b8y;
	z8 = b8z;
};


bool TLandscapeObject::Destructable( void ) const
{
	return destructable;
};


void TLandscapeObject::Destructable( bool d )
{
	destructable = d;
};

bool TLandscapeObject::Armory( void ) const
{
	return armory;
};


void TLandscapeObject::Armory( bool d )
{
	armory = d;
};

bool TLandscapeObject::Repairs( void ) const
{
	return repairs;
};


void TLandscapeObject::Repairs( bool d )
{
	repairs = d;
};

bool TLandscapeObject::RefuelStation( void ) const
{
	return refuelStation;
};


void TLandscapeObject::RefuelStation( bool d )
{
	refuelStation = d;
};

bool TLandscapeObject::IsaAI( void ) const
{
	return isaAI;
};


void TLandscapeObject::IsaAI( bool d )
{
	isaAI = d;
};

bool TLandscapeObject::StrengthUpdated( void ) const
{
	return strengthUpdated;
};

void TLandscapeObject::StrengthUpdated( bool _strengthUpdated )
{
	strengthUpdated = _strengthUpdated;
};

size_t TLandscapeObject::AIType( void ) const
{
	return aiType;
};


void TLandscapeObject::AIType( size_t d )
{
	aiType = d;
};

size_t TLandscapeObject::AITeam( void ) const
{
	return aiTeam;
};


void TLandscapeObject::AITeam( size_t d )
{
	aiTeam = d;
};

size_t TLandscapeObject::GameImportance( void ) const
{
	return gameImportance;
};


void TLandscapeObject::GameImportance( size_t d )
{
	gameImportance = d;
};

size_t TLandscapeObject::Strength( void ) const
{
	return strength;
};

void TLandscapeObject::Strength( size_t d )
{
	if ( destructable )
	{
		if ( strength!=d )
		{
			strengthUpdated = true;
		}
		strength = d;
	}
};

size_t TLandscapeObject::Points( void ) const
{
	return points;
};

void TLandscapeObject::Points( size_t d )
{
	points = d;
};

size_t TLandscapeObject::Bombs( void ) const
{
	return bombs;
};

void TLandscapeObject::Bombs( size_t d )
{
	bombs = d;
};

size_t TLandscapeObject::V2Countdown( void ) const
{
	return bombs;
};

void TLandscapeObject::V2Countdown( size_t d )
{
	bombs = d;
};

size_t TLandscapeObject::Shells( void ) const
{
	return shells;
};

void TLandscapeObject::Shells( size_t d )
{
	shells = d;
};

size_t TLandscapeObject::CharacterType( void ) const
{
	return characterType;
};

void TLandscapeObject::CharacterType( size_t d )
{
	characterType = d;
};

float TLandscapeObject::RX( void ) const
{
	return rx;
};

float TLandscapeObject::RY( void ) const
{
	return ry;
};

float TLandscapeObject::RZ( void ) const
{
	return rz;
};

float TLandscapeObject::ScaleX( void ) const
{
	return scaleX;
};

float TLandscapeObject::ScaleY( void ) const
{
	return scaleY;
};

float TLandscapeObject::ScaleZ( void ) const
{
	return scaleZ;
};

void TLandscapeObject::ScaleX( float s )
{
	if ( scaleX!=s )
	{
		scaleX = s;
		GetTransformationMatrix();
	}
};

void TLandscapeObject::ScaleY( float s )
{
	if ( scaleY!=s )
	{
		scaleY = s;
		GetTransformationMatrix();
	}
};

void TLandscapeObject::ScaleZ( float s )
{
	if ( scaleZ!=s )
	{
		scaleZ = s;
		GetTransformationMatrix();
	}
};

void TLandscapeObject::RX( float r )
{
	if ( rx!=r )
	{
		rx = r;
		GetTransformationMatrix();
	}
};

void TLandscapeObject::RY( float r )
{
	if ( ry!=r )
	{
		ry = r;
		GetTransformationMatrix();
	}
};

void TLandscapeObject::RZ( float r )
{
	if ( rz!=r )
	{
		rz = r;
		GetTransformationMatrix();
	}
};

void TLandscapeObject::Refresh( void )
{
	GetTransformationMatrix();
};

bool TLandscapeObject::IsPlant( const TString& name )
{
	TString lname = name.lcase();
	if ( lname=="tree1.obj" ||
		 lname=="tree2.obj" ||
		 lname=="tree3.obj" ||
		 lname=="tree4.obj" ||
		 lname=="tree5.obj" ||
		 lname=="snowtree1.obj" ||
		 lname=="snowtree2.obj" ||
		 lname=="snowtree3.obj" ||
		 lname=="snowtree4.obj" ||
		 lname=="snowtree5.obj" )
	{
		return true;
	}
	return false;
};

bool TLandscapeObject::IsStructure( const TString& name )
{
	TString lname = name.lcase();
	if ( lname=="house1.obj" ||
		 lname=="house2.obj" ||
		 lname=="house3.obj" ||
		 lname=="house4.obj" ||
		 lname=="house5.obj" ||
		 lname=="house6.obj" ||
		 lname=="house7.obj" ||
		 lname=="house8.obj" ||
		 lname=="house9.obj" ||
		 lname=="kiosk.obj" ||
		 lname=="road1.obj" ||
		 lname=="newbridge.obj" ||
		 lname=="church.obj" ||
		 lname=="bank.obj" ||
		 lname=="bridge1.obj" ||
		 lname=="bridge2.obj" ||

		 lname=="r11.obj" ||
		 lname=="r12.obj" ||
		 lname=="r14.obj" ||
		 lname=="r18.obj" ||
		 lname=="r22.obj" ||
		 lname=="r24.obj" ||
		 lname=="r28.obj" ||
		 lname=="r44.obj" ||
		 lname=="r48.obj" ||
		 lname=="r88.obj" ||

		 lname=="bridge3.obj" )
	{
		return true;
	}
	return false;
};


TString TLandscapeObject::FlagNameToTeamName( const TString& name )
{
	TString lname = name.lcase();
	if ( lname=="redflag.obj" )
		return "Red team";
	if ( lname=="greenflag.obj" )
		return "Green team";
	if ( lname=="blueflag.obj" )
		return "Blue team";
	return "Yellow team";
};


bool TLandscapeObject::IsTeam( const TString& name )
{
	TString lname = name.lcase();
	if ( lname=="redflag.obj" ||
		 lname=="greenflag.obj" ||
		 lname=="blueflag.obj" ||
		 lname=="yellowflag.obj" )
	{
		return true;
	}
	return false;
}


bool TLandscapeObject::IsFlak( const TString& name )
{
	TString lname = name.lcase();
	if ( lname=="flak1.obj" ||
		 lname=="flak2.obj" )
	{
		return true;
	}
	return false;
}


bool TLandscapeObject::IsV2( const TString& name )
{
	TString lname = name.lcase();
	if ( lname=="v2.obj" )
	{
		return true;
	}
	return false;
}


bool TLandscapeObject::IsV2TargetMarker( const TString& name )
{
	TString lname = name.lcase();
	if ( lname=="blackflag.obj" )
	{
		return true;
	}
	return false;
}


bool TLandscapeObject::IsArtillery( const TString& name )
{
	TString lname = name.lcase();
	if ( lname=="gun1.obj" ||
		 lname=="gun2.obj" )
	{
		return true;
	}
	return false;
}


void TLandscapeObject::CalculateDamage( TApp* app, size_t damage )
{
	if ( damage < strength )
	{
		strengthUpdated = true;
		strength = strength - damage;
	}
	else
	{
		strengthUpdated = true;
		strength = 0;
	}
	Strength( strength );
	if ( strength==0 )
	{
		currentIndex = maxIndex;
#ifdef _USEGUI
		if ( app!=NULL )
		{
			app->ExplodeAt( TPoint(tx,ty,tz), etMaterial, 5 );
		}
#endif
	}
	else
	{
		StrengthToIndex();
	}
};


void TLandscapeObject::CalculateDamage( TApp* app, float expx, float expy, float expz, 
										float damage )
{
	if ( strength==0 || !destructable )
	{
		return;
	}

	float dist = (expx-tx)*(expx-tx) + (expy-ty)*(expy-ty) + (expz-tz)*(expz-tz);
	dist = dist * 0.1f;

	size_t sub;
	if ( dist==0 )
	{
		sub = size_t(damage);
	}
	else
	{
		float d = 1 / dist;
		d = d * damage;
		sub = size_t(d);
	}

	if ( sub>0 )
	{
		strengthUpdated = true;
		if ( sub < strength )
			strength -= sub;
		else
			strength = 0;
		Strength( strength );
		if ( strength==0 )
		{
			currentIndex = maxIndex;
#ifdef _USEGUI
			if ( app!=NULL )
			{
				app->ExplodeAt( TPoint(tx,ty,tz), etMaterial, 5 );
			}
#endif
		}
		else
		{
			StrengthToIndex();
		}
	}
};

//==========================================================================

TLandscapeObjectDraw::TLandscapeObjectDraw( TLandscapeObject& obj )
	: obj(&obj),
	  next(NULL)
{
};


TLandscapeObjectDraw::TLandscapeObjectDraw( void )
	: obj(NULL),
	  next(NULL)
{
}


TLandscapeObjectDraw::TLandscapeObjectDraw( const TLandscapeObjectDraw& o )
	: obj(NULL),
	  next(NULL)
{
	operator=(o);
};


const TLandscapeObjectDraw& TLandscapeObjectDraw::operator=( const TLandscapeObjectDraw& o )
{
	obj = o.obj;
	next = o.next;
	return *this;
};


TLandscapeObjectDraw::~TLandscapeObjectDraw( void )
{
	obj = NULL;
	delete next;
};


void TLandscapeObjectDraw::Draw( void ) const
{
	PreCond( obj!=NULL );
	if ( obj->Strength()>0 )
	{
		obj->Draw();
	}
};


void TLandscapeObjectDraw::Draw( bool drawTransparencies ) const
{
	PreCond( obj!=NULL );
	if ( obj->Strength()>0 )
	{
		obj->Draw(drawTransparencies);
	}
};


bool TLandscapeObjectDraw::GetLORTriangle( float xp, float yp, float zp, 
										   float size, float& y )
{
	return obj->GetLORTriangle(xp,yp,zp,size,y);
};


bool TLandscapeObjectDraw::Collision( float l1x, float l1y, float l1z,
									  float l2x, float l2y, float l2z ) const
{
	return obj->Collision(l1x,l1y,l1z, l2x,l2y,l2z );
}


bool TLandscapeObjectDraw::InsideObject( float x1, float y1, float z1 )
{
	if ( obj->Strength()>0 )
		return obj->InsideObstacle(x1,y1,z1);
	return false;
};


bool TLandscapeObjectDraw::InsideObject( float x1, float z1 )
{
	if ( obj->Strength()>0 )
		return obj->InsideObject(x1,z1);
	return false;
};


bool TLandscapeObjectDraw::InsideObject( float* x1, float* z1 )
{
	if ( obj->Strength()>0 )
		return obj->InsideObject(x1,z1);
	return false;
};


bool TLandscapeObjectDraw::InsideObject( float x1, float y1, float z1,
										 float x2, float y2, float z2 )
{
	if ( obj->Strength()>0 )
		return obj->InsideObstacle(x1,y1,z1) || obj->InsideObstacle(x2,y2,z2);
	return false;
};


void TLandscapeObjectDraw::DrawBoundingBox( float r, float g, float b )
{
	obj->DrawBoundingBox(r,g,b);
};


TLandscapeObjectDraw* TLandscapeObjectDraw::Next( void ) const
{
	return next;
};


void TLandscapeObjectDraw::Next( TLandscapeObjectDraw* _next )
{
	next = _next;
};


void TLandscapeObjectDraw::GetBounds( float& minx, float& miny, float& minz,
									  float& maxx, float& maxy, float& maxz )
{
	obj->GetBounds(minx,miny,minz,maxx,maxy,maxz);
};


const TString& TLandscapeObjectDraw::Name( void ) const
{
	return obj->Name();
};

//==========================================================================


