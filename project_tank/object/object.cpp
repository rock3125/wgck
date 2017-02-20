#include <precomp_header.h>

#include <win32/win32.h>
#include <object/object.h>
#include <object/mesh.h>

//==========================================================================

const float kPI = 3.1415927f;
const float degToRad = 0.01745329252033f;
const float radToDeg = 57.2957795112730f;

//==========================================================================

TObject::TObject( void )
	: loaded(false)
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;

	minX = 0.0f;
	minY = 0.0f;
	minZ = 0.0f;

	maxX = 0.0f;
	maxY = 0.0f;
	maxZ = 0.0f;

	centerX = 0.0f;
	centerY = 0.0f;
	centerZ = 0.0f;

	xSize = 0.0f;
	ySize = 0.0f;
	zSize = 0.0f;
};


TObject::TObject( const TObject& obj )
	: loaded(false)
{
	operator=(obj);
};


const TObject& TObject::operator=( const TObject& obj )
{
	loaded = obj.loaded;
	x = obj.x;
	y = obj.y;
	z = obj.z;
	
	minX = obj.minX;
	minY = obj.minY;
	minZ = obj.minZ;
	
	maxX = obj.maxX;
	maxY = obj.maxY;
	maxZ = obj.maxZ;
	
	centerX = obj.centerX;
	centerY = obj.centerY;
	centerZ = obj.centerZ;
	
	xSize = obj.xSize;
	ySize = obj.ySize;
	zSize = obj.zSize;

	return *this;
};

	
TObject::~TObject( void )
{
};


void TObject::SetLocationToObjectLocation( TObject& obj )
{
	x = obj.x;
	y = obj.y;
	z = obj.z;

	Yangle( obj.Yangle() );
	Xangle( obj.Xangle() );
	Zangle( obj.Zangle() );
};


void TObject::Resize( void )
{
	// go through all the meshes and find the min, max (x,y,z) to define
	// the box area of this object
    TMeshList* ml = meshList;
    while ( ml!=NULL )
    {
		TMesh& m = ml->mesh;
		if ( m.MinX()<minX )
			minX = m.MinX();
		if ( m.MinY()<minY )
			minY = m.MinY();
		if ( m.MinZ()<minZ )
			minZ = m.MinZ();
				
		if ( m.MaxX()>maxX )
			maxX = m.MaxX();
		if ( m.MaxY()>maxY )
			maxY = m.MaxY();
		if ( m.MaxZ()>maxZ )
			maxZ = m.MaxZ();

		ml = ml->next;
	}
	centerX = (minX + maxX) / 2.0f;
	centerY = (minY + maxY) / 2.0f;
	centerZ = (minZ + maxZ) / 2.0f;

	xSize = maxX - centerX;
	ySize = maxY - centerY;
	zSize = maxZ - centerZ;
}


bool TObject::LoadBinary( const TString& fname, TString& errStr, 
						  const TString& pathname,
						  const TString& texturePath )
{
	WriteString( "File: %s\\%s\n", pathname.c_str(), fname.c_str() );
	bool success = TBinObject::LoadBinary(fname,errStr,pathname,texturePath);
	if ( success )
		Resize();
	loaded = success;
	return success;
};


bool TObject::LoadBinary( TPersist& file, TString& errStr, 
						  const TString& pathname,
						  const TString& texturePath )
{
	bool success = TBinObject::LoadBinary(file,errStr,pathname,texturePath);
	if ( success )
		Resize();
	loaded = success;
	return success;
};


void TObject::GetBoudingBox( float& _minX, float& _minY, float& _minZ,
							 float& _maxX, float& _maxY, float& _maxZ ) const
{
	_minX = minX;
	_minY = minY;
	_minZ = minZ;

	_maxX = maxX;
	_maxY = maxY;
	_maxZ = maxZ;
};


void TObject::Draw( bool showBoundingBox ) const
{
	TBinObject::Draw();

	if ( showBoundingBox )
		DrawBoundingBox();
};


void TObject::DrawBoundingBox( void ) const
{
	glColor3f(1,1,1);
	glBegin( GL_LINES );

		glVertex3f( minX, minY, minZ );
		glVertex3f( maxX, minY, minZ );

		glVertex3f( maxX, minY, minZ );
		glVertex3f( maxX, maxY, minZ );

		glVertex3f( maxX, maxY, minZ );
		glVertex3f( minX, maxY, minZ );

		glVertex3f( minX, maxY, minZ );
		glVertex3f( minX, minY, minZ );

		glVertex3f( minX, minY, maxZ );
		glVertex3f( maxX, minY, maxZ );

		glVertex3f( maxX, minY, maxZ );
		glVertex3f( maxX, maxY, maxZ );

		glVertex3f( maxX, maxY, maxZ );
		glVertex3f( minX, maxY, maxZ );

		glVertex3f( minX, maxY, maxZ );
		glVertex3f( minX, minY, maxZ );

		glVertex3f( minX, minY, minZ );
		glVertex3f( minX, minY, maxZ );

		glVertex3f( maxX, minY, minZ );
		glVertex3f( maxX, minY, maxZ );

		glVertex3f( maxX, maxY, minZ );
		glVertex3f( maxX, maxY, maxZ );

		glVertex3f( minX, maxY, minZ );
		glVertex3f( minX, maxY, maxZ );

	glEnd();
};


void TObject::GetCenter( float& cx, float& cy, float &cz ) const
{
	cx = centerX;
	cy = centerY;
	cz = centerZ;
};


float TObject::MinX( void ) const
{
	return minX;
}


float TObject::MinY( void ) const
{
	return minY;
}


float TObject::MinZ( void ) const
{
	return minZ;
}


float TObject::MaxX( void ) const
{
	return maxX;
}


float TObject::MaxY( void ) const
{
	return maxY;
}


float TObject::MaxZ( void ) const
{
	return maxZ;
}


bool TObject::Loaded( void ) const
{
	return loaded;
};


void TObject::MinX( float t )
{
	minX = t;
};


void TObject::MinY( float t )
{
	minY = t;
};


void TObject::MinZ( float t )
{
	minZ = t;
};


void TObject::MaxX( float t )
{
	maxX = t;
};


void TObject::MaxY( float t )
{
	maxY = t;
};


void TObject::MaxZ( float t )
{
	maxZ = t;
};


void TObject::CenterX( float t )
{
	centerX = t;
};


void TObject::CenterY( float t )
{
	centerY = t;
};


void TObject::CenterZ( float t )
{
	centerZ = t;
};


float TObject::CenterX( void ) const
{
	return centerX;
};


float TObject::CenterY( void ) const
{
	return centerY;
};


float TObject::CenterZ( void ) const
{
	return centerZ;
};


float TObject::X( void ) const
{
	return x;
}


float TObject::Y( void ) const
{
	return y;
}


float TObject::Z( void ) const
{
	return z;
}


void TObject::X( float _x )
{
	x = _x;
}


void TObject::Y( float _y )
{
	y = _y;
}


void TObject::Z( float _z )
{
	z = _z;
}


float TObject::SizeX( void ) const
{
	return xSize;
};


float TObject::SizeY( void ) const
{
	return ySize;
};


float TObject::SizeZ( void ) const
{
	return zSize;
};


void TObject::SizeX( float sx )
{
	xSize = sx;
};


void TObject::SizeY( float sy )
{
	ySize = sy;
};


void TObject::SizeZ( float sz)
{
	zSize = sz;
};

//==========================================================================

