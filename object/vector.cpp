#include <precomp_header.h>

#include <object/vector.h>

//==========================================================================

TVector::TVector( void )
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	xangle = 0.0f;
	yangle = 0.0f;
	zangle = 0.0f;
};


TVector::TVector( const TVector& v )
{
	operator=(v);
};


const TVector& TVector::operator=( const TVector& v )
{
	x = v.x;
    y = v.y;
    z = v.z;
    xangle = v.xangle;
    yangle = v.yangle;
    zangle = v.zangle;

	return *this;
};


TVector::~TVector( void )
{
}



float TVector::X( void ) const
{
	return x;
};



float TVector::Y( void ) const
{
	return y;
};



float TVector::Z( void ) const
{
	return z;
};



void TVector::X( float _x )
{
	x = _x;
};



void TVector::Y( float _y )
{
	y = _y;
};



void TVector::Z( float _z )
{
	z = _z;
};



float TVector::Xangle( void ) const
{
	return xangle;
};



float TVector::Yangle( void ) const
{
	return yangle;
};



float TVector::Zangle( void ) const
{
	return zangle;
};



void TVector::Xangle( float _xangle )
{
	xangle = _xangle;
};



void TVector::Yangle( float _yangle )
{
	yangle = _yangle;
};



void TVector::Zangle( float _zangle )
{
	zangle = _zangle;
};

//==========================================================================




