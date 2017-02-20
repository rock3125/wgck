#include <precomp_header.h>

#include <win32/win32.h>
#include <object/binobject.h>
#include <object/landscape.h>
#include <object/vehicle.h>
#include <object/geometry.h>

//==========================================================================

const float kPI = 3.1415927f;
const float degToRad = 0.01745329252033f;
const float radToDeg = 57.2957795112730f;

//==========================================================================

TVehicle::TVehicle( float _hoverHeight, float _submergeDepth )
	: hoverHeight( _hoverHeight ),
	  submergeDepth( _submergeDepth ),
	  currentIndex(0)
{
	mx = 0.0f;
	my = 0.0f;
	mz = 0.0f;

	initX = 0;
	initY = 0;
	initZ = 0;

	dirn = 0;
	diff = 0.0f;

	yangle = 0;
	xangle = 0;
	zangle = 0;

	x = 0;
	y = 0;
	z = 0;

	px1 = py1 = pz1 = 0;
	px2 = py2 = pz2 = 0;
	px3 = py3 = pz3 = 0;
	px4 = py4 = pz4 = 0;

	x1 = y1 = z1 = 0;
	x2 = y2 = z2 = 0;
	x3 = y3 = z3 = 0;
	x4 = y4 = z4 = 0;

	scaleX = 1;
	scaleY = 1;
	scaleZ = 1;

	for ( size_t i=0; i<16; i++ )
		matrix[i] = 0;
	matrix[0] = 1;
	matrix[5] = 1;
	matrix[10] = 1;
	matrix[15] = 1;
};


TVehicle::TVehicle( const TVehicle& v )
{
	operator=(v);
};


const TVehicle& TVehicle::operator = ( const TVehicle& v )
{
	// mid point and normal of vehicle
	mx = v.mx;
	my = v.my;
	mz = v.mz;

	xzRadius = v.xzRadius;
	xzAngle = v.xzAngle;

	dirn = v.dirn;
	diff = v.diff;

	x = v.x;
	y = v.y;
	z = v.z;

	xangle = v.xangle;
	yangle = v.yangle;
	zangle = v.zangle;

	return *this;
};

	
TVehicle::~TVehicle( void )
{
};


TCompoundObject& TVehicle::Object( void )
{
	return object;
};


float TVehicle::SubmergeDepth( void ) const
{
	return submergeDepth;
};


void TVehicle::SubmergeDepth( float smd ) 
{
	submergeDepth = smd;
};

bool TVehicle::LoadBinary( const TString& fname, TString& errStr, 
						   const TString& pathname,
						   const TString& texturePath )
{
	bool success = object.LoadBinary(fname,errStr,pathname,texturePath);
	if ( success )
	{
		SetupObject();
	};
	return success;
};


void TVehicle::SetupObject( void )
{
	float xSize = object.SizeX(currentIndex);
	float zSize = object.SizeZ(currentIndex);
	xzRadius = float(sqrt(xSize*xSize+zSize*zSize));
	if ( xSize!=0.0 )
		xzAngle = float(atan(fabs(xSize)/fabs(zSize))) * radToDeg;
	else
		xzAngle = 90.0f;
};


float TVehicle::GetXZRadius( void ) const
{
	return xzRadius;
};


void TVehicle::GetCornerPoints( void )
{
	float yangle;
	float offsetX, offsetZ;

    yangle = (Yangle()+xzAngle)*degToRad;
	offsetX = -float(sin(yangle))*xzRadius*0.8f*scaleX;
	offsetZ = float(cos(yangle))*xzRadius*0.8f*scaleZ;

	x1 = n_x + object.CenterX(currentIndex) + offsetX;
	z1 = n_z + object.CenterZ(currentIndex) + offsetZ;
	x3 = n_x + object.CenterX(currentIndex) - offsetX;
	z3 = n_z + object.CenterZ(currentIndex) - offsetZ;

    yangle = (Yangle()-xzAngle)*degToRad;
	offsetX = -float(sin(yangle))*xzRadius*0.8f*scaleX;
	offsetZ = float(cos(yangle))*xzRadius*0.8f*scaleZ;

	x2 = n_x + object.CenterX(currentIndex) + offsetX;
	z2 = n_z + object.CenterZ(currentIndex) + offsetZ;
	x4 = n_x + object.CenterX(currentIndex) - offsetX;
	z4 = n_z + object.CenterZ(currentIndex) - offsetZ;
};


// bit pattern in key
void TVehicle::Move( size_t key, float speed )
{
	// get new corners after new movement has been determined
	px1 = x1;
	py1 = y1;
	pz1 = z1;

	px2 = x2;
	py2 = y2;
	pz2 = z2;

	px3 = x3;
	py3 = y3;
	pz3 = z3;

	px4 = x4;
	py4 = y4;
	pz4 = z4;
	pa  = Yangle();

	px = x;
	py = y;
	pz = z;

	n_x = x;
	n_z = z;
	n_yangle = pa;

	// slow down on hills (gravity)
	float anglespeed = speed;
	if ( diff>0.25f && (key&aUp)>0 )
	{
		anglespeed = anglespeed / (diff*3);
	}
	if ( diff<-0.25f && (key&aDown)>0 )
	{
		anglespeed = anglespeed / (-diff*3);
	}

	if ( (key&aLeft)>0 )
	{
		n_yangle -= 5.0f;
	}
	if ( (key&aRight)>0 )
	{
		n_yangle += 5.0f;
	}
    float yangle1 = n_yangle*degToRad;
	float v1fcos = (float)cos(yangle1);
	float v1fsin = (float)sin(yangle1);
	dirn = key;
	if ( (key&aUp)>0 )
	{
		n_x -= v1fsin*anglespeed;
		n_z += v1fcos*anglespeed;
	}
	if ( (key&aDown)>0 )
	{
		n_x += v1fsin*anglespeed;
		n_z -= v1fcos*anglespeed;
	}

	GetCornerPoints();

	// get mid point
	n_mx = (x1 + x2 + x3 + x4) * 0.25f;
	n_my = (y1 + y2 + y3 + y4) * 0.25f + hoverHeight * scaleY;
	n_mz = (z1 + z2 + z3 + z4) * 0.25f;

	// for tank speed
	diff = y1 - y4;

	// pre-calculate x, y and z angles
	float dy = ((y1+y2)*0.5f-(y4+y3)*0.5f);
	n_za = -float(atan( dy / (object.SizeZ(currentIndex)*2) )) * radToDeg;
	dy = y1-y2;
	n_xa = -float(atan( dy / (object.SizeX(currentIndex)*2) )) * radToDeg;
	n_ya = -Yangle();
};


void TVehicle::CommitMove( void )
{
	x = n_x;
	z = n_z;
	yangle = n_yangle;

	mx = n_mx;
	my = n_my;
	mz = n_mz;

	xa = n_xa;
	ya = n_ya;
	za = n_za;

	UpdateVehicleMatrix();
};


void TVehicle::AbortMove( void )
{
	x = px;
	y = py;
	z = pz;
	yangle = pa;

	mx = n_mx;
	my = n_my;
	mz = n_mz;

	xa = n_xa;
	ya = n_ya;
	za = n_za;

	x1 = px1;
	y1 = py1;
	z1 = pz1;

	x2 = px2;
	y2 = py2;
	z2 = pz2;

	x3 = px3;
	y3 = py3;
	z3 = pz3;

	x4 = px4;
	y4 = py4;
	z4 = pz4;
};


void TVehicle::UpdateVehicleMatrix( void )
{
	glPushMatrix();

		glLoadIdentity();
		glTranslatef( mx,
					  my,
					  mz );

		glRotatef( ya, 0,1,0);
		glRotatef( xa, 0,0,1 );
		glRotatef( za, 1,0,0 );

		glScalef( scaleX, scaleY, scaleZ );
		glGetFloatv( GL_MODELVIEW_MATRIX, matrix );

	glPopMatrix();
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

		glScalef( scaleX, scaleY, scaleZ );

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


float TVehicle::X( void ) const
{
	return x;
};



float TVehicle::Y( void ) const
{
	return y;
};



float TVehicle::Z( void ) const
{
	return z;
};



void TVehicle::X( float _x )
{
	x = _x;
	n_x = _x;
	mx = _x;
};



void TVehicle::Y( float _y )
{
	y = _y;
	n_y = _y;
	y1 = _y;
	y2 = _y;
	y3 = _y;
	y4 = _y;
	my = _y;
};



void TVehicle::Z( float _z )
{
	z = _z;
	n_z = _z;
	mz = _z;
};



float TVehicle::Xangle( void ) const
{
	return xangle;
};


float TVehicle::Yangle( void ) const
{
	return yangle;
};



//float TVehicle::TrueYangle( void ) const
//{
	//return yangle;
//};



float TVehicle::Zangle( void ) const
{
	return zangle;
};



void TVehicle::Xangle( float _xangle )
{
	xangle = _xangle;
};



void TVehicle::Yangle( float _yangle )
{
	yangle = _yangle;
};


void TVehicle::Zangle( float _zangle )
{
	zangle = _zangle;
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


bool TVehicle::InsideVehicle( float _x, float _y, float _z ) const
{
	if ( _y>=(object.MinY(currentIndex)+Y()) && _y<=(object.MaxY(currentIndex)+Y()) )
	if ( PointInsideBox( _x,_z,  x1,z1, x2,z2, x3,z3, x4,z4 ) )
		return true;
	return false;
};


void TVehicle::SetInitialPosition( TLandscape& landObj, float x, float z )
{
	TMesh* landscapeMesh = landObj.LandscapeMesh();
	PostCond( landscapeMesh!=NULL );

	float maxy = landscapeMesh->MaxY();
	float size = landscapeMesh->SizeY() * 2;
	float ypos;
	bool found = landObj.LandscapeCollision( x, maxy, z, size, ypos );
	PostCond( found==true );

	X( x );
	Y( ypos );
	Z( z );

	initX = x;
	initY = ypos;
	initZ = z;
};


void TVehicle::ResetPosition( void )
{
	X( initX );
	Y( initY );
	Z( initZ );
};


bool TVehicle::Collision( TVehicle& otherVehicle ) const
{
	if ( PointInsideBox( otherVehicle.x1,otherVehicle.z1,  x1,z1,x2,z2,x3,z3,x4,z4 ) ||
		 PointInsideBox( otherVehicle.x2,otherVehicle.z2,  x1,z1,x2,z2,x3,z3,x4,z4 ) ||
		 PointInsideBox( otherVehicle.x3,otherVehicle.z3,  x1,z1,x2,z2,x3,z3,x4,z4 ) ||
		 PointInsideBox( otherVehicle.x4,otherVehicle.z4,  x1,z1,x2,z2,x3,z3,x4,z4 ) )
		return true;
	return false;
};


bool TVehicle::Collision( float x, float y, float z,
						  float x1, float y1, float z1 ) const
{
	return object.Collision( currentIndex, matrix, x,y,z, x1,y1,z1 );
};


void TVehicle::HoverHeight( float hh )
{
	hoverHeight = hh;
};

float TVehicle::HoverHeight( void ) const
{
	return hoverHeight;
};

//==========================================================================

