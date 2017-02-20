#include <precomp_header.h>

#include <win32/win32.h>
#include <object/explosive.h>
#include <object/camera.h>
#include <tank/tankapp.h>

//==========================================================================

const float degToRad = 0.01745329252033f;

//==========================================================================

TExplosive::TExplosive( void )
	: explosive( NULL ),
	  soundSystem( NULL ),
	  app( NULL )
{
	x = 0;
	y = 0;
	z = 0;

	scale = 1;
	inUse = false;

	explosionType = etMaterial;
	explosionSound = 0;
};

const TExplosive& TExplosive::operator=( const TExplosive& ex )
{
	scale = ex.scale;

	explosionSound = ex.explosionSound;
	soundSystem = ex.soundSystem;
	app = ex.app;

	x = ex.x;
	y = ex.y;
	z = ex.z;

	explosive = ex.explosive;
	explosionType = ex.explosionType;
	inUse = ex.inUse;

	return *this;
};

TExplosive::TExplosive( const TExplosive& ex )
	: explosive( NULL ),
	  soundSystem( NULL )
{
	operator=(ex);
};

TExplosive::~TExplosive( void )
{
	explosive = NULL;
};

void TExplosive::SetXYZ( float _x, float _y, float _z )
{
	x = _x;
	y = _y;
	z = _z;
};

void TExplosive::SetXYZ( const TPoint& point )
{
	x = point.x;
	y = point.y;
	z = point.z;
};

void TExplosive::GetXYZ( float& _x, float& _y, float& _z )
{
	_x = x;
	_y = y;
	_z = z;
};

void TExplosive::Draw( void ) const
{
	if ( inUse )
	if ( explosive!=NULL )
	{
		glPushMatrix();
	
			glTranslatef( x,y,z );
			glScalef( scale,scale,scale );

			explosive->Draw();
	
		glPopMatrix();
	}
};

float TExplosive::Scale( void ) const
{
	return scale;
};

void TExplosive::Scale( float _scale )
{
	scale = _scale;
};

void TExplosive::StartTimer( size_t time )
{
	inUse = true;
	explosionTime = ::GetTickCount() + time;
};

void TExplosive::StartTimer( void )
{
	inUse = true;
};

bool TExplosive::Ready( void ) const
{
	return !inUse;
};

void TExplosive::InUse( bool _inUse )
{
	inUse = _inUse;
};

void TExplosive::Logic( void )
{
	if ( inUse )
	if ( ::GetTickCount() > explosionTime )
	{
		inUse = false;
		if ( app!=NULL )
			app->ExplodeAt( TPoint(x,y,z), explosionType, 30 );
	}
};

void TExplosive::CalculateDamage( const TPoint& point, float damage )
{
	if ( inUse )
		CalculateDamage( point.x,point.y,point.z, damage );
};

void TExplosive::CalculateDamage( float expx, float expy, float expz, 
								  float damage )
{
	if ( inUse )
	{
		float dist = (expx-x)*(expx-x) + (expy-y)*(expy-y) + (expz-z)*(expz-z);
		dist = dist * 0.5f;

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
			inUse = false;
			if ( app!=NULL )
				app->ExplodeAt( TPoint(x,y,z), explosionType, 30 );
		}
	}
};

void TExplosive::Explosion( TBinObject* _explosive, float _scale )
{
	explosive = _explosive;
	scale = _scale;
};

size_t TExplosive::ExplosionType( void ) const
{
	return explosionType;
};

void TExplosive::ExplosionType( size_t _explosionType )
{
	explosionType = _explosionType;
};

void TExplosive::SetApp( TApp* _app )
{
	app = _app;
}

size_t TExplosive::netSet( size_t myId, byte* buf ) const
{
	PreCond( app!=NULL );
	PreCond( buf!=NULL );

	buf[0] = id_explosive;

	buf[1] = myId;

	float* f = (float*)&buf[2];
	f[0] = x;
	f[1] = y;
	f[2] = z;

	size_t* d = (size_t*)&buf[2+sizeof(float)*3];
	d[0] = explosionTime - ::GetTickCount();

	return netSize();
};

size_t TExplosive::netSize( void ) const
{
	return (2 + sizeof(float)*3 + sizeof(size_t));
};

size_t TExplosive::netGet( const byte* buf )
{
	PreCond( app!=NULL );
	PreCond( buf!=NULL );
	PreCond( buf[0]==id_explosive );

	float* f = (float*)&buf[2];
	x = f[0];
	y = f[1];
	z = f[2];

	size_t* d = (size_t*)&buf[2+sizeof(float)*3];
	explosionTime = d[0] + ::GetTickCount();

	return netSize();
}

//==========================================================================
