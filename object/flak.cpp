#include <precomp_header.h>

#include <win32/win32.h>
#include <object/binobject.h>
#include <object/landscape.h>
#include <object/flak.h>
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

TFlak::TFlak( void )
{
	type = vtFlak;

	projectile = 0;

	engineSoundOn = false;
	engineSound = 0;
	engineChannel = 0;
	gunSound = 0;
	gunSoundOn = false;

	initNumProjectiles = 200;
	numRounds = initNumProjectiles;

	hover = 0;
	reloadTime = 0;
	gunRange = 30;

	showTarget = false;

	logicCounter = 0;

	barrelAngle = 0;
};


TFlak::TFlak( const TFlak& v )
{
	operator=(v);
};


const TFlak& TFlak::operator = ( const TFlak& v )
{
	TVehicle::operator = (v);

	numRounds = v.numRounds;
	initNumProjectiles = v.initNumProjectiles;

	engineSound = v.engineSound;
	gunSound = v.gunSound;
	engineSoundOn = v.engineSoundOn;
	engineChannel = v.engineChannel;
	gunSoundOn = v.gunSoundOn;

	gunRange = v.gunRange;
	hover = v.hover;
	projectile = v.projectile;

	gunId = v.gunId;
	showTarget = v.showTarget;

	logicCounter = v.logicCounter;

	barrelAngle = v.barrelAngle;

	return *this;
};


void TFlak::operator = ( const TLandscapeObject& o )
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


void TFlak::operator=( TCharacter& dp )
{
	Yangle( dp.Yangle() );
	barrelAngle = dp.BarrelAngle();
	team = dp.Team();
};


void TFlak::operator=( TVehicleSettings& vs )
{
	PreCond( vs.IsFlak() );

	gunRange   = vs.GunRange();
	hover = vs.Hover();
	engineSound = vs.EngineSound();
	gunSound = vs.GunSound();
	iconId = vs.IconId();
};


TFlak::~TFlak( void )
{
};


float TFlak::GetXZRadius( void ) const
{
	return xzRadius;
};


// bit pattern in key
void TFlak::Move( size_t& key, bool ctrlDown, float dxa, float dya )
{
	size_t time = ::GetTickCount();
	logicCounter++;

	if ( strength==0 )
	{
		return;
	}

	GetMatrix();

	if ( (key&aFire)==aFire )
	{
		if ( numRounds>0 && (time>reloadTime) )
		{
			reloadTime = time + kReloadTime;
			numRounds--;
			if ( soundSystem!=NULL )
			{
				soundSystem->Play( gunSound, TPoint(x,y,z) );
			}

				// fire gun - 10==damage, 3==speed - experimental
#ifndef _DONTUSE
			if ( app!=NULL )
			{
				app->FireProjectile( start, dirn, projectile, gunRange, 10 );
			}
#endif
		}
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
		yangle -= 1;
	}
	else if ( (key&aLeft)==aLeft )
	{
		yangle += 1;
	}

	if ( showTarget && yangle!=prev_yangle )
	{
		if ( !engineSoundOn )
		{
			engineSoundOn = true;
			if ( soundSystem!=NULL )
			{
				engineChannel = soundSystem->Play( engineSound, TPoint(x,y,z) );
			}
		}
	}
	else if ( engineSoundOn )
	{
		engineSoundOn = false;
		if ( soundSystem!=NULL )
		{
			soundSystem->Stop( engineChannel );
		}
	}
};


void TFlak::GetMatrix( void )
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


void TFlak::GetGunDetails( TPoint& _start, TPoint& _dirn, size_t& _gunRange )
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


void TFlak::GetGunDetails( void )
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


void TFlak::ShowTargeting( void )
{
	GetGunDetails();
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
};


void TFlak::Draw1stPerson( void )
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


void TFlak::Draw( bool transp )
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

		if ( showTarget || gunSoundOn )
			ShowTargeting();
	}
};


float TFlak::Y( void ) const
{
	return y;
};


void TFlak::Y( float _y )
{
	y = _y;
	n_y = _y;
};


void TFlak::PauseAnimations( bool p )
{
	object.PauseAnimations( p );
};


bool TFlak::InsideVehicle( const TPoint& point ) const
{
	return InsideVehicle( point.x, point.y, point.z );
};


bool TFlak::InsideVehicle( float _x, float _y, float _z ) const
{
	float dist = (_x-X())*(_x-X()) + (_y-Y())*(_y-Y()) +
				 (_z-Z())*(_z-Z());
	return (dist < scale);
};


bool TFlak::Collision( float x, float y, float z,
						float x1, float y1, float z1 )
{
	return object.Collision( currentIndex, matrix, x,y,z, x1,y1,z1 );
};


void TFlak::Bounds( float& x1, float& y1, float& z1,
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

float TFlak::BarrelAngle( void ) const
{
	return barrelAngle;
};

void TFlak::BarrelAngle( float ba )
{
	barrelAngle = ba;
};

size_t TFlak::Ammo( void ) const
{
	return numRounds;
};

void TFlak::SetProjectile( size_t _projectile )
{
	projectile = _projectile;
}

size_t TFlak::Strength( void ) const
{
	return strength;
};

void TFlak::Strength( size_t _strength )
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

void TFlak::Explode( void ) 
{
	if ( strength!=0 )
	{
		strengthUpdated = true;
	}
	strength = 0;
	if ( engineSoundOn )
	{
		engineSoundOn = false;
		if ( soundSystem!=NULL )
		{
			soundSystem->Stop( engineSound );
		}
	}
	if ( gunSoundOn )
	{
		gunSoundOn = false;
		if ( soundSystem!=NULL )
		{
			soundSystem->Stop( gunSound );
		}
	}
};

void TFlak::CalculateDamage( const TPoint& point, float damage )
{
	CalculateDamage( point.x,point.y,point.z, damage );
};

void TFlak::CalculateDamage( float expx, float expy, float expz, 
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

void TFlak::Rearm( void )
{
	if ( numRounds<initNumProjectiles )
	{
		numRounds++;
	}
};

void TFlak::Repair( void )
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

void TFlak::StopSounds( void )
{
	if ( engineSoundOn )
	{
		engineSoundOn = false;
		if ( soundSystem!=NULL )
		{
			soundSystem->Stop( engineChannel );
		}
	};
};

bool TFlak::CanEnter( void ) const
{
	return (!isaAI && strength>0 );
};

size_t TFlak::netSet( size_t myId, size_t vid, byte* data )
{
	PreCond( data!=NULL );

	data[0] = id_flakData;
	data[1] = myId;
	data[2] = vid;
	data[3] = strength;
	data[4] = aiId;

	float* f = (float*)&data[5];
	f[0] = yangle;
	f[1] = barrelAngle;

	return netSize();
};

size_t TFlak::netSize( void ) const
{
	return (5 + 2*sizeof(float));
};

size_t TFlak::netGet( const byte* data )
{
	PreCond( data!=NULL );
	PreCond( data[0]==id_flakData );

	if ( strength!=data[3] )
	{
		strengthUpdated = true;
	}

	strength = data[3];
	aiId = data[4];

	float* f = (float*)&data[5];
	yangle = f[0];
	barrelAngle = f[1];

	return netSize();
}

//==========================================================================

