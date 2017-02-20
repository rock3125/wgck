#include <precomp_header.h>

#include <win32/win32.h>
#include <object/object.h>
#include <object/landscape.h>
#include <object/vehicle.h>
#include <object/geometry.h>

//==========================================================================

const float kPI = 3.1415927f;
const float degToRad = 0.01745329252033f;
const float radToDeg = 57.2957795112730f;

//==========================================================================

TVehicle::TVehicle( void )
	: location1(-1),
	  location2(-1),
	  location3(-1),
	  location4(-1),
	  currentIndex(0)
{
	mx = 0.0f;
	my = 0.0f;
	mz = 0.0f;

	dirn = 0;
	diff = 0.0f;
};


TVehicle::TVehicle( const TVehicle& v )
{
	operator=(v);
};


const TVehicle& TVehicle::operator = ( const TVehicle& v )
{
	location1 = v.location1;
	location2 = v.location2;
	location3 = v.location3;
	location4 = v.location4;

	// mid point and normal of vehicle
	mx = v.mx;
	my = v.my;
	mz = v.mz;

	// undo movement capability
	previousX = v.previousX;
	previousZ = v.previousZ;
	previousYangle = v.previousYangle;

	xzRadius = v.xzRadius;
	xzAngle = v.xzAngle;

	dirn = v.dirn;
	diff = v.diff;

	return *this;
};

	
TVehicle::~TVehicle( void )
{
};


int TVehicle::Location1( void ) const
{
	return location1;
};


void TVehicle::Location1( int _loc )
{
	location1 = _loc;
};


int TVehicle::Location2( void ) const
{
	return location2;
};



void TVehicle::Location2( int _loc )
{
	location2 = _loc;
};


int TVehicle::Location3( void ) const
{
	return location3;
};



void TVehicle::Location3( int _loc )
{
	location3 = _loc;
};


int TVehicle::Location4( void ) const
{
	return location4;
};



void TVehicle::Location4( int _loc )
{
	location4 = _loc;
};


bool TVehicle::LoadBinary( const TString& fname, TString& errStr, 
						   const TString& pathname,
						   const TString& texturePath )
{
	bool success = object.LoadBinary(fname,errStr,pathname,texturePath);
	if ( success )
	{
		float xSize = object.SizeX(currentIndex);
		float zSize = object.SizeZ(currentIndex);
		xzRadius = float(sqrt(xSize*xSize+zSize*zSize));
		if ( xSize!=0.0 )
			xzAngle = float(atan(fabs(zSize)/fabs(xSize))) * radToDeg;
		else
			xzAngle = 90.0f;
	};
	return success;
};


float TVehicle::GetXZRadius( void ) const
{
	return xzRadius;
};


void TVehicle::GetCornerPoints( float _yangle,
							    float &x1, float &z1,
							    float &x2, float &z2,
							    float &x3, float &z3,
							    float &x4, float &z4 ) const
{
	float yangle;
	float offsetX, offsetZ;

    yangle = (_yangle+xzAngle)*degToRad;
	offsetX = -float(sin(yangle))*xzRadius;
	offsetZ = float(cos(yangle))*xzRadius;

	x1 = X() + object.CenterX(currentIndex) + offsetX;
	z1 = Z() + object.CenterZ(currentIndex) + offsetZ;
	x3 = X() + object.CenterX(currentIndex) - offsetX;
	z3 = Z() + object.CenterZ(currentIndex) - offsetZ;

    yangle = (_yangle-xzAngle)*degToRad;
	offsetX = -float(sin(yangle))*xzRadius;
	offsetZ = float(cos(yangle))*xzRadius;

	x2 = X() + object.CenterX(currentIndex) + offsetX;
	z2 = Z() + object.CenterZ(currentIndex) + offsetZ;
	x4 = X() + object.CenterX(currentIndex) - offsetX;
	z4 = Z() + object.CenterZ(currentIndex) - offsetZ;
};


void TVehicle::GetCornerPoints( float lx, float lz,
							    float &x1, float &z1,
							    float &x2, float &z2,
							    float &x3, float &z3,
							    float &x4, float &z4 ) const
{
	float yangle;
	float offsetX, offsetZ;

    yangle = (object.Yangle(currentIndex)+xzAngle)*degToRad;
	offsetX = -float(sin(yangle))*xzRadius;
	offsetZ = float(cos(yangle))*xzRadius;

	x1 = X() + object.CenterX(currentIndex) + offsetX;
	z1 = Z() + object.CenterZ(currentIndex) + offsetZ;
	x3 = X() + object.CenterX(currentIndex) - offsetX;
	z3 = Z() + object.CenterZ(currentIndex) - offsetZ;

    yangle = (object.Yangle(currentIndex)-xzAngle)*degToRad;
	offsetX = -float(sin(yangle))*xzRadius;
	offsetZ = float(cos(yangle))*xzRadius;

	x2 = X() + object.CenterX(currentIndex) + offsetX;
	z2 = Z() + object.CenterZ(currentIndex) + offsetZ;
	x4 = X() + object.CenterX(currentIndex) - offsetX;
	z4 = Z() + object.CenterZ(currentIndex) - offsetZ;

	x1 += (-lx);
	x2 += (-lx);
	x3 += (-lx);
	x4 += (-lx);

	z1 += (-lz);
	z2 += (-lz);
	z3 += (-lz);
	z4 += (-lz);
};


void TVehicle::GetExhaustPoints( float lx, float lz,
							     float &x3, float &z3,
							     float &x4, float &z4 ) const
{
	float yangle;
	float offsetX, offsetZ;

    yangle = (object.Yangle(currentIndex)+xzAngle)*degToRad;
	offsetX = -float(sin(yangle))*xzRadius*0.8f;
	offsetZ = float(cos(yangle))*xzRadius*0.8f;

	x3 = X() + object.CenterX(currentIndex) - offsetX;
	z3 = Z() + object.CenterZ(currentIndex) - offsetZ;

    yangle = (object.Yangle(currentIndex)-xzAngle)*degToRad;
	offsetX = -float(sin(yangle))*xzRadius*0.8f;
	offsetZ = float(cos(yangle))*xzRadius*0.8f;

	x4 = X() + object.CenterX(currentIndex) - offsetX;
	z4 = Z() + object.CenterZ(currentIndex) - offsetZ;

	x3 += (-lx);
	x4 += (-lx);

	z3 += (-lz);
	z4 += (-lz);
};


// bit pattern in key: 1=fwd, 2=reverse, 4=left, 8=right
void TVehicle::Move( TLandscape& landObj, size_t key, float speed )
{
	previousX = X();
	previousZ = Z();
	previousYangle = object.Yangle(currentIndex);

	float anglespeed = speed;
	if ( diff>0.5f && (key&1)>0 )
	{
		anglespeed = anglespeed / (diff*2.0f);
	}
	if ( diff<-0.5f && (key&2)>0 )
	{
		anglespeed = anglespeed / (-diff*2.0f);
	}

    float yangle = object.Yangle(currentIndex)*degToRad;
	float v1fcos = (float)cos(yangle);
	float v1fsin = (float)sin(yangle);
	dirn = key;
	if ( key & 1 )
	{
		object.Z( Z() + v1fcos*anglespeed );
		object.X( X() - v1fsin*anglespeed );
	}
	if ( key & 2 )
	{
		object.Z( Z() - v1fcos*anglespeed );
		object.X( X() + v1fsin*anglespeed );
	}
	if ( key & 4 )
	{
		object.Yangle( object.Yangle(currentIndex) - 5.0f );
	}
	if ( key & 8 )
	{
		object.Yangle( object.Yangle(currentIndex) + 5.0f );
	}

	bool canMove = true;
	GetCornerPoints( 0,0, x1,z1, x2,z2, x3,z3, x4,z4 );
	GetExhaustPoints( 0,0, ex3,ez3, ex4,ez4 );

	landObj.GetLORTriangle( x1,y1,z1, location1 );
	location1 = landObj.lorReference;
	landObj.GetLORTriangle( x2,y2,z2, location2 );
	location2 = landObj.lorReference;
	landObj.GetLORTriangle( x3,y3,z3, location3 );
	location3 = landObj.lorReference;
	landObj.GetLORTriangle( x4,y4,z4, location4 );
	location4 = landObj.lorReference;

	if ( location1==-1 ||
		 location2==-1 ||
		 location3==-1 ||
		 location4==-1 )
		canMove = false;

	// on top of a bridge?
	landObj.BridgeHeight( x1,y1,z1 );
	landObj.BridgeHeight( x2,y2,z2 );
	landObj.BridgeHeight( x3,y3,z3 );
	landObj.BridgeHeight( x4,y4,z4 );

	float wl = landObj.WaterLevel();
	if ( y1<wl || y2<wl || y3<wl || y4<wl )
		canMove = false;

	if ( (dirn&15)!=0 )
	{
		float dx = v1fsin;
		float dy = v1fcos;
		if ( (dirn&1) > 0 )
		{
			dx = -dx;
			dy = -dy;
		}
	}

	if ( canMove )
	if ( landObj.InNoGoZone(x1,z1,x2,z2,x3,z3,x4,z4) )
		canMove = false;

	diff = 0;
	if ( !canMove )
	{
		object.X( previousX );
		object.Z( previousZ );
		object.Yangle( previousYangle );
	}

	// get mid point
	mx = (x1 + x2 + x3 + x4) * 0.25f;
	my = (y1 + y2 + y3 + y4) * 0.25f;
	mz = (z1 + z2 + z3 + z4) * 0.25f;

	// for tank speed
	diff = y1 - y4;

	// pre-calculate x, y and z angles
	float dy = ((y1+y2)*0.5f-(y4+y3)*0.5f);
	xa = -float(atan( dy / (object.SizeX(currentIndex)*2) )) * radToDeg;
	dy = y1-y2;
	za = float(atan( dy / (object.SizeZ(currentIndex)*2) )) * radToDeg;
	ya = -object.Yangle(currentIndex) + 90.0f;
};


void TVehicle::Draw( void ) const
{
	// obj can be considered to be at 0,0 at this stage
	glPushMatrix();

		glTranslatef( mx,
					  my,
 					  mz );

		glRotatef( ya, 0,1,0 );
		glRotatef( xa, 0,0,1 );
		glRotatef( za, 1,0,0 );

		object.Draw(currentIndex);

	glPopMatrix();
};


float TVehicle::MX( void ) const
{
	return mx;
};


float TVehicle::MY( void ) const
{
	return my;
};


float TVehicle::MZ( void ) const
{
	return mz;
};


void TVehicle::SetOrientationToVehicle( TVehicle& obj )
{
	X( obj.X() );
	Y( obj.Y() );
	Z( obj.Z() );
	Xangle( obj.Xangle() );
	Yangle( obj.Yangle() );
	Zangle( obj.Zangle() );

	xa = obj.xa;
	ya = obj.ya;
	za = obj.za;

	x1 = obj.x1;
	y1 = obj.y1;
	z1 = obj.z1;
	x2 = obj.x2;
	y2 = obj.y2;
	z2 = obj.z2;
	x3 = obj.x3;
	y3 = obj.y3;
	z3 = obj.z3;
	x4 = obj.x4;
	y4 = obj.y4;
	z4 = obj.z4;

	mx = obj.mx;
	my = obj.my;
	mz = obj.mz;
};


float TVehicle::X1( void ) const
{
	return x1;
};


float TVehicle::X2( void ) const
{
	return x2;
};


float TVehicle::X3( void ) const
{
	return x3;
};


float TVehicle::X4( void ) const
{
	return x4;
};


float TVehicle::Y1( void ) const
{
	return y1;
};


float TVehicle::Y2( void ) const
{
	return y2;
};


float TVehicle::Y3( void ) const
{
	return y3;
};


float TVehicle::Y4( void ) const
{
	return y4;
};


float TVehicle::Z1( void ) const
{
	return z1;
};


float TVehicle::Z2( void ) const
{
	return z2;
};


float TVehicle::Z3( void ) const
{
	return z3;
};


float TVehicle::Z4( void ) const
{
	return z4;
};


float TVehicle::EX3( void ) const
{
	return ex3;
};


float TVehicle::EX4( void ) const
{
	return ex4;
};


float TVehicle::EZ3( void ) const
{
	return ez3;
};


float TVehicle::EZ4( void ) const
{
	return ez4;
};


float TVehicle::X( void ) const
{
	return object.X(currentIndex);
};



float TVehicle::Y( void ) const
{
	return object.Y(currentIndex);
};



float TVehicle::Z( void ) const
{
	return object.Z(currentIndex);
};



void TVehicle::X( float _x )
{
	object.X( _x );
};



void TVehicle::Y( float _y )
{
	object.Y( _y );
};



void TVehicle::Z( float _z )
{
	object.Z( _z );
};



float TVehicle::Xangle( void ) const
{
	return object.Xangle(currentIndex);
};



float TVehicle::Yangle( void ) const
{
	return object.Yangle(currentIndex);
};



float TVehicle::Zangle( void ) const
{
	return object.Zangle(currentIndex);
};



void TVehicle::Xangle( float _xangle )
{
	object.Xangle(_xangle);
};



void TVehicle::Yangle( float _yangle )
{
	object.Yangle( _yangle );
};


void TVehicle::Zangle( float _zangle )
{
	object.Zangle( _zangle );
};


size_t TVehicle::CurrentIndex( void ) const
{
	return currentIndex;
};


void TVehicle::CurrentIndex( size_t c )
{
	currentIndex = c;
};


void TVehicle::PauseAnimations( bool p )
{
	object.PauseAnimations( p );
};


float TVehicle::SizeX( void ) const
{
	return object.SizeX(currentIndex);
};


float TVehicle::SizeY( void ) const
{
	return object.SizeY(currentIndex);
};


float TVehicle::SizeZ( void ) const
{
	return object.SizeZ(currentIndex);
};

//==========================================================================

