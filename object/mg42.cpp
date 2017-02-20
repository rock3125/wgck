#include <precomp_header.h>

#include <win32/win32.h>
#include <object/binobject.h>
#include <object/landscape.h>
#include <object/mg42.h>
#include <object/geometry.h>
#include <network/packet.h>

#include <tank/tankapp.h>

#if defined(_EDITOR) || defined(_PARSER) || defined(_VIEWER) || defined(_SPEEDTEST)
#define _DONTUSE
#endif

//==========================================================================

const float kPI = 3.1415927f;
const float degToRad = 0.01745329252033f;
const float radToDeg = 57.2957795112730f;

const long kReloadTime = 500;

//==========================================================================

TMG42::TMG42( void )
	: soldier(NULL)
{
	type = vtMG42;

	characterOwner = false;

	mgSoundOn = false;
	gunSound = 0;
	mgChannel = 0;

	initNumProjectiles = 200;
	numRounds = initNumProjectiles;

	reloadTime = 0;
	gunRange = 30;

	showTarget = false;

	logicCounter = 0;
	barrelAngle = 0;
	counter = 0;
};


TMG42::TMG42( const TMG42& v )
{
	operator=(v);
};


const TMG42& TMG42::operator = ( const TMG42& v )
{
	TVehicle::operator = (v);

	characterOwner = false; // always

	numRounds = v.numRounds;
	initNumProjectiles = v.initNumProjectiles;
	soldier = v.soldier;

	gunSound = v.gunSound;
	mgSoundOn = v.mgSoundOn;
	mgChannel = v.mgChannel;

	gunRange = v.gunRange;

	gunId = v.gunId;
	showTarget = v.showTarget;

	logicCounter = v.logicCounter;
	barrelAngle = v.barrelAngle;

	return *this;
};


void TMG42::operator = ( const TLandscapeObject& o )
{
	TVehicle::operator = (o);

	initNumProjectiles = o.Shells();
	numRounds = initNumProjectiles;

	gunId = 0;
	showTarget = false;
	logicCounter = 0;

	barrelAngle = 0;

	size_t key = 0;
	Move(key,false,0,0);
};


void TMG42::operator=( TCharacter& dp )
{
	Yangle( dp.Yangle() );
	barrelAngle = dp.BarrelAngle();
	team = dp.Team();
};


void TMG42::operator=( TVehicleSettings& vs )
{
	PreCond( vs.IsMG42() );

	iconId = vs.IconId();
	gunRange   = vs.GunRange();
	gunSound = vs.GunSound();
};


TMG42::~TMG42( void )
{
	if ( characterOwner && soldier!=NULL )
	{
		delete soldier;
	}
	soldier = NULL;
};


float TMG42::GetXZRadius( void ) const
{
	return xzRadius;
};


// bit pattern in key
void TMG42::Move( size_t& key, bool ctrlDown, float dxa, float dya )
{
	size_t time = ::GetTickCount();
	logicCounter++;

	if ( strength==0 )
	{
		return;
	}

	if ( soldier!=NULL )
	{
		soldier->Logic();
	}

	GetMatrix();

	if ( (key&aFire)==aFire )
	{
		if ( numRounds>0 )
		{
			numRounds--;
			if ( !mgSoundOn )
			{
				mgSoundOn = true;
				if ( soundSystem!=NULL )
				{
					mgChannel = soundSystem->Play( gunSound, TPoint(x,y,z) );
				}
			}

				// fire gun - 10==damage, 3==speed - experimental
#ifndef _DONTUSE
			if ( app!=NULL )
			{
				app->Fire( start, dirn, gunRange, 2 );
			}
#endif
		}
		else if ( mgSoundOn )
		{
			if ( soundSystem!=NULL )
			{
				soundSystem->Stop( mgChannel );
			}
			mgSoundOn = false;
		}
	}
	else if ( mgSoundOn )
	{
		if ( soundSystem!=NULL )
		{
			soundSystem->Stop( mgChannel );
		}
		mgSoundOn = false;
	}

	showTarget = ctrlDown;
	float prev_yangle = yangle;
	if ( showTarget )
	{
		yangle += dya;
		if ( yangle>360 )
			yangle-=360;
		if ( yangle<0 )
			yangle+=360;

		barrelAngle += dxa;
		if ( barrelAngle>10 )
			barrelAngle = 10;
		if ( barrelAngle<-45 )
			barrelAngle = -45;
	}

	if ( (key&aRight)==aRight )
	{
		yangle -= 2.5f;
	}
	else if ( (key&aLeft)==aLeft )
	{
		yangle += 2.5f;
	}
};


void TMG42::GetMatrix( void )
{
	glPushMatrix();

		glLoadIdentity();
		glTranslatef( x,
					  Y(),
					  z );

		glRotatef( zangle, 0,0,1 );
		glRotatef( yangle, 0,1,0 );
		glRotatef( xangle, 1,0,0 );

		glScalef( scalex, scaley, scalez );

		glRotatef( barrelAngle, 1,0,0 );

		float m[16];
		glGetFloatv( GL_MODELVIEW_MATRIX, m );
		gunMatrix.Matrix(m);
		GetGunDetails();

	glPopMatrix();
};


void TMG42::GetGunDetails( TPoint& _start, TPoint& _dirn, size_t& _gunRange )
{
	TPoint hp1;
	TPoint hp2(0,0,1);

	TPoint p1,p2;
	gunMatrix.Mult( hp1, p1 );
	gunMatrix.Mult( hp2, p2 );

	float dx = (p2.x - p1.x);
	float dy = (p2.y - p1.y);
	float dz = (p2.z - p1.z);
	_dirn = TPoint( dx,dy,dz );

	float x = p1.x + dx;
	float y = p1.y + dy;
	float z = p1.z + dz;
	_start = TPoint(x,y,z);

	_gunRange = gunRange;
};


void TMG42::GetGunDetails( void )
{
	TPoint hp1;
	TPoint hp2(0,0,1);

	TPoint p1,p2;
	gunMatrix.Mult( hp1, p1 );
	gunMatrix.Mult( hp2, p2 );

	float dx = (p2.x - p1.x);
	float dy = (p2.y - p1.y);
	float dz = (p2.z - p1.z);
	dirn = TPoint( dx,dy,dz );

	float x = p1.x + dx;
	float y = p1.y + dy;
	float z = p1.z + dz;
	start = TPoint(x,y,z);
};


void TMG42::ShowTargeting( void )
{
	GetGunDetails();
	if ( mgSoundOn )
	{
		counter++;
		::ShowMachineGunFire( start, dirn, float(gunRange) * 1.5f, counter );
	}
	else
	{
		glColor3ub(255,255,255);
		glEnable(GL_COLOR_MATERIAL );
		glBegin( GL_POINTS );
			size_t range = gunRange;
			while ( range>0 )
			{
				glVertex3f( start.x,start.y,start.z );
				start = start + dirn;
				range--;
			}
		glEnd();
	}
};


void TMG42::Draw1stPerson( void )
{
	if ( strength>0 )
	{
		if ( showTarget )
		{
			size_t range;
			TPoint start, dirn;
			GetGunDetails( start, dirn, range );
			if ( range>0 )
			{
				dirn = dirn * float(range);
				TPoint p = start + dirn;
				::DrawCrosshair( 4, p, yangle );
			}
		}
	}
};


void TMG42::Draw( bool transp )
{
	if ( strength>0 )
	{
		// obj can be considered to be at 0,0 at this stage
		glPushMatrix();

			glTranslatef( x,
						  Y(),
						  z );

			glRotatef( zangle, 0,0,1 );
			glRotatef( yangle, 0,1,0 );
			glRotatef( xangle, 1,0,0 );

			glScalef( scalex, scaley, scalez );
			object.Draw(currentIndex,transp);

		glPopMatrix();

		if ( soldier!=NULL )
		{
			float x1 = -float(sin(-yangle*degToRad))*scale;
			float z1 = float(cos(-yangle*degToRad))*scale;
			soldier->X( x - x1 );
			soldier->Y( y - 0.3f );
			soldier->Z( z - z1 );
			soldier->Yangle( -yangle );
			soldier->Draw();
		}

		if ( showTarget || mgSoundOn )
			ShowTargeting();
	}
};

void TMG42::PauseAnimations( bool p )
{
	object.PauseAnimations( p );
};

bool TMG42::InsideVehicle( const TPoint& point ) const
{
	return InsideVehicle( point.x, point.y, point.z );
};


bool TMG42::InsideVehicle( float _x, float _y, float _z ) const
{
	float dist = (_x-X())*(_x-X()) + (_y-Y())*(_y-Y()) +
				 (_z-Z())*(_z-Z());
	return (dist < scale);
};


bool TMG42::Collision( float x, float y, float z,
						float x1, float y1, float z1 )
{
	return object.Collision( currentIndex, matrix, x,y,z, x1,y1,z1 );
};


void TMG42::Bounds( float& x1, float& y1, float& z1,
					float& x2, float& y2, float& z2,
					float& x3, float& y3, float& z3,
					float& x4, float& y4, float& z4 ) const
{
	float xsize = SizeX() * 0.5f;
	float zsize = SizeZ() * 0.5f;

	x1 = -xsize;
	y1 = 0;
	z1 = -zsize;

	x2 = -xsize;
	y2 = 0;
	z2 = zsize;

	x3 = xsize;
	y3 = 0;
	z3 = zsize;

	x4 = xsize;
	y4 = 0;
	z4 = -zsize;

//	MatrixMult( matrix, x1,y1,z1 );
//	MatrixMult( matrix, x2,y2,z2 );
//	MatrixMult( matrix, x3,y3,z3 );
//	MatrixMult( matrix, x4,y4,z4 );
};

float TMG42::BarrelAngle( void ) const
{
	return barrelAngle;
};

void TMG42::BarrelAngle( float ba )
{
	barrelAngle = ba;
};

size_t TMG42::Ammo( void ) const
{
	return numRounds;
};

size_t TMG42::Strength( void ) const
{
	return strength;
};

void TMG42::Strength( size_t _strength )
{
	if ( strength!=_strength )
	{
		strengthUpdated = true;
	}
	strength = _strength;
	if ( strength==0 )
	{
		StopSounds();
		Explode();
#ifndef _DONTUSE
		if ( app!=NULL )
		{
			app->ExplodeAt( TPoint(x,Y(),z), etMaterial, 30 );
		}
#endif
	}
};

void TMG42::Explode( void ) 
{
	if ( strength!=0 )
	{
		strengthUpdated = true;
	}
	strength = 0;
	if ( mgSoundOn )
	{
		mgSoundOn = false;
		if ( soundSystem!=NULL )
		{
			soundSystem->Stop( mgChannel );
		}
	}
};

void TMG42::CalculateDamage( const TPoint& point, float damage )
{
	CalculateDamage( point.x,point.y,point.z, damage );
};

void TMG42::CalculateDamage( float expx, float expy, float expz, 
							 float damage )
{
	float dist = (expx-x)*(expx-x) + (expy-Y())*(expy-Y()) + (expz-z)*(expz-z);
	dist = dist * 0.05f;
	dist = dist / scale;

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

		if ( strength==0 )
		{
#ifndef _DONTUSE
			if ( app!=NULL )
			{
				app->ExplodeAt( TPoint(x,y,z), etMaterial, 30 );
			}
#endif
		}
	}
};

void TMG42::Rearm( void )
{
	if ( numRounds<initNumProjectiles )
	{
		numRounds++;
	}
};

void TMG42::Repair( void )
{
	if ( strength<100 )
	{
		strength = strength + 5;
		strengthUpdated = true;
	}
	if ( strength>100 )
	{
		strength = 100;
	}
};

void TMG42::StopSounds( void )
{
	if ( mgSoundOn )
	{
		if ( soundSystem!=NULL )
		{
			soundSystem->Stop( mgChannel );
		}
		mgSoundOn = false;
	}
};

bool TMG42::CanEnter( void ) const
{
	return (!isaAI && strength>0 );
};

void TMG42::SetSoldier( TCharacter* _soldier )
{
	soldier = _soldier;
};

void TMG42::RemoveSoldier( void )
{
	soldier = NULL;
};

void TMG42::CharacterOwner( bool _characterOwner )
{
	characterOwner = _characterOwner;
};

size_t TMG42::netSet( size_t myId, size_t vid, byte* data )
{
	PreCond( data!=NULL );

	data[0] = id_mg42Data;
	data[1] = myId;
	data[2] = vid;
	data[3] = strength;
	data[4] = aiId;

	float* f = (float*)&data[5];
	f[0] = yangle;
	f[1] = barrelAngle;

	return netSize();
};

size_t TMG42::netSize( void ) const
{
	return (5 + 2*sizeof(float));
};

size_t TMG42::netGet( const byte* data )
{
	PreCond( data!=NULL );
	PreCond( data[0]==id_mg42Data );

	if ( strength!=data[3] )
	{
		strengthUpdated = true;
	};
	strength = data[3];
	aiId = data[4];

	float* f = (float*)&data[5];
	yangle = f[0];
	barrelAngle = f[1];

	return netSize();
}

//==========================================================================

