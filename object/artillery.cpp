#include <precomp_header.h>

#include <win32/win32.h>
#include <object/binobject.h>
#include <object/landscape.h>
#include <object/artillery.h>
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

const long kReloadTime = 5000;

//==========================================================================

TArtillery::TArtillery( void )
	: soldier(NULL)
{
	type = vtArtillery;

	characterOwner = false;

	projectile = 0;
	fuel = 100;

	engineSoundOn = false;
	engineSound = 0;
	engineChannel = 0;
	gunSound = 0;

	initNumShells = 20;
	numShells = initNumShells;
	fuelConsumption = 0.01f;

	maxBarrelAngle = 10;
	minBarrelAngle = -30;

	hover = 0;
	reloadTime = 0;
	gunRange = 30;

	showTarget = false;

	speed = 0.25f;
	actualSpeed = 0;
	logicCounter = 0;

	turretAngle = 0;
	barrelAngle = 0;
};


TArtillery::TArtillery( const TArtillery& v )
{
	operator=(v);
};


const TArtillery& TArtillery::operator = ( const TArtillery& v )
{
	TVehicle::operator = (v);

	characterOwner = false; // always

	engineChannel = v.engineChannel;
	prevEnginePos = v.prevEnginePos;
	soldier = v.soldier;

	numShells = v.numShells;
	initNumShells = v.initNumShells;

	fuel = v.fuel;
	fuelConsumption = v.fuelConsumption;

	engineSound = v.engineSound;
	gunSound = v.gunSound;
	engineSoundOn = v.engineSoundOn;

	gunRange = v.gunRange;
	hover = v.hover;
	projectile = v.projectile;

	maxBarrelAngle = v.maxBarrelAngle;
	minBarrelAngle = v.minBarrelAngle;

	showTarget = v.showTarget;

	logicCounter = v.logicCounter;

	barrelAngle = v.barrelAngle;
	turretAngle = v.turretAngle;

	return *this;
};


void TArtillery::operator = ( const TLandscapeObject& o )
{
	TVehicle::operator = (o);

	fuel = 100;
	numShells = initNumShells;
	showTarget = false;
	logicCounter = 0;

	turretAngle = 0;
	barrelAngle = 0;

	y = o.TY() - (SizeY()+hover);
	n_y = y;
	
	initNumShells = o.Shells();
	numShells = initNumShells;

	// get initial matrix initialised
	UpdateArtilleryMatrix();
};


void TArtillery::operator=( TVehicleSettings& vs )
{
	PreCond( vs.IsArtillery() );

	gunRange   = vs.GunRange();
	hover = vs.Hover();
	engineSound = vs.EngineSound();
	gunSound = vs.GunSound();
	speed = vs.Speed();
	iconId = vs.IconId();
	fuelConsumption = vs.FuelConsumption();
	maxBarrelAngle = vs.MaxBarrelAngle();
	minBarrelAngle = vs.MinBarrelAngle();
};


TArtillery::~TArtillery( void )
{
	if ( characterOwner && soldier!=NULL )
	{
		delete soldier;
	}
	soldier = NULL;
};


void TArtillery::Move( size_t& key, bool ctrlDown, float dxa, float dya )
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

	// can we move
	if ( ((key&15) > 0) && fuel>0 )
	{
		fuel = fuel - fuelConsumption;
	}

	GetBarrelMatrix();

	// sounds
	if ( (key&15)>0 && fuel>0 )
	{
		if ( !engineSoundOn )
		{
			engineSoundOn = true;
			prevEnginePos = TPoint( x, Y(), z );
			if ( soundSystem!=NULL )
			{
				engineChannel = soundSystem->Play( engineSound, prevEnginePos );
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

	if ( engineSoundOn )
	{
		TPoint loc = TPoint(x,Y(),z);
		if ( soundSystem!=NULL )
		{
			soundSystem->SetLocation( engineChannel, loc, prevEnginePos );
		}
		prevEnginePos = loc;
	}

	if ( (key&aFire)==aFire && (time>reloadTime) )
	{
		reloadTime = time + kReloadTime;
		if ( numShells>0 )
		{
			numShells--;
			if ( soundSystem!=NULL )
			{
				soundSystem->Play( gunSound, TPoint(x,Y(),z) );
			}
			// fire gun - 10==damage, 3==speed - experimental
#ifndef _DONTUSE
			if ( app!=NULL )
			{
				app->FireProjectile( start, dirn, projectile, gunRange, 30 );
			}
#endif
		}
	}

	n_x = x;
	n_y = y;
	n_z = z;

	showTarget = ctrlDown;

	n_yangle = yangle;
	n_xangle = xangle;
	n_zangle = zangle;

	if ( showTarget )
	{
		turretAngle += dya;
		if ( turretAngle>5 )
			turretAngle = 5;
		if ( turretAngle<-5 )
			turretAngle = -5;

		barrelAngle += dxa;
		if ( barrelAngle > maxBarrelAngle )
			barrelAngle = maxBarrelAngle;
		if ( barrelAngle < minBarrelAngle )
			barrelAngle = minBarrelAngle;
	}

	if ( (key&aLeft)>0 )
	{
		n_yangle += 5.0f;
	}
	else if ( (key&aRight)>0 )
	{
		n_yangle -= 5.0f;
	}

	float mult = 0;
	if ( (key&aUp)>0 )
	{
		mult = 1;
	}
	else if ( (key&aDown)>0 )
	{
		mult = -1;
	}

	float yangle1 = n_yangle*degToRad;
	float v1fcos = (float)cos(-yangle1);
	float v1fsin = (float)sin(-yangle1);

	n_x -= v1fsin*speed*mult;
	n_z += v1fcos*speed*mult;

	actualSpeed = 0;
	if ( (key&15)>0 )
	{
		actualSpeed = speed;
	}
};


void TArtillery::GetBarrelMatrix( void )
{
	glPushMatrix();

		glLoadIdentity();
		glTranslatef( x,
					  Y(),
					  z );

		glRotatef( xangle, 0,0,1 );
		glRotatef( yangle, 0,1,0 );
		glRotatef( xangle, 1,0,0 );

		glScalef( scalex, scaley, scalez );

		glRotatef( turretAngle, 0,1,0 );
		glRotatef( barrelAngle, 1,0,0 );

		float m[16];
		glGetFloatv( GL_MODELVIEW_MATRIX, m );
		gunMatrix.Matrix(m);
		GetGunDetails();

	glPopMatrix();
};


void TArtillery::GetGunDetails( TPoint& _start, TPoint& _dirn, size_t& _gunRange )
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


void TArtillery::GetGunDetails( void )
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


void TArtillery::ShowTargeting( void )
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


void TArtillery::CommitMove( void )
{
	x = n_x;
	y = n_y;
	z = n_z;
	yangle = n_yangle;
	xangle = n_xangle;
	zangle = n_zangle;

	UpdateArtilleryMatrix();
};


void TArtillery::operator=( TCharacter& dp )
{
	X( dp.X() );
	Y( dp.Y() );
	Z( dp.Z() );
	Xangle( dp.Xangle() );
	Yangle( dp.Yangle() );
	Zangle( dp.Zangle() );
	turretAngle = dp.TurretAngle();
	barrelAngle = dp.BarrelAngle();
	team = dp.Team();
};


bool TArtillery::LegalVehicleMove( TLandscape* landObj, TParams& params )
{
	size_t i;

	if ( landObj==NULL )
		return false;
	if ( strength==0 )
		return false;

	// can't drive through any of these
	if ( !isaAI )
	{
		for ( i=0; i<params.numTanks; i++ )
		{
			if ( params.tanks[i].Strength()>0 )
			{
				float dx = params.tanks[i].X() - X();
				float dy = params.tanks[i].Y() - Y();
				float dz = params.tanks[i].Z() - Z();
				float d = dx*dx + dy*dy + dz*dz;
				float sz = (SizeX() + params.tanks[i].SizeX());
				sz = sz * sz;
				if ( d < sz )
				{
					return false;
				}
			}
		}

		for ( i=0; i<params.numPlanes; i++ )
		{
			if ( params.planes[i].Strength()>0 )
			{
				float dx = params.planes[i].X() - X();
				float dy = params.planes[i].Y() - Y();
				float dz = params.planes[i].Z() - Z();
				float d = dx*dx + dy*dy + dz*dz;
				float sz = (SizeX() + params.planes[i].SizeX());
				sz = sz * sz;
				if ( d < sz )
				{
					return false;
				}
			}
		}

		for ( i=0; i<params.numFlak; i++ )
		{
			if ( params.flak[i].Strength()>0 )
			{
				float dx = params.flak[i].X() - X();
				float dy = params.flak[i].Y() - Y();
				float dz = params.flak[i].Z() - Z();
				float d = dx*dx + dy*dy + dz*dz;
				float sz = (SizeX() + params.flak[i].SizeX());
				sz = sz * sz;
				if ( d < sz )
				{
					return false;
				}
			}
		}

		for ( i=0; i<params.numArtillery; i++ )
		{
			if ( params.artillery[i].Strength()>0 && &params.artillery[i]!=this )
			{
				float dx = params.artillery[i].X() - X();
				float dy = params.artillery[i].Y() - Y();
				float dz = params.artillery[i].Z() - Z();
				float d = dx*dx + dy*dy + dz*dz;
				float sz = (SizeX() + params.artillery[i].SizeX());
				sz = sz * sz;
				if ( d < sz )
				{
					return false;
				}
			}
		}
	}

	// see if this move is a legal one
	// calculate new positions on landscape
	bool canMove = landObj->GetLORTriangle( *this );
	if ( !canMove )
	{
		return false;
	}

	// crash into water?
	float wl = landObj->WaterLevel();

	float x1,y1,z1, x2,y2,z2, x3,y3,z3, x4,y4,z4;
	Bounds( x1,y1,z1, x2,y2,z2, x3,y3,z3, x4,y4,z4 );
	if ( y1<wl || y2<wl || y3<wl || y4<wl )
	{
		return false;
	}

	// crash into object?
	size_t objId;
	if ( (objId=landObj->Collision( *this ))>0 )
	{
		return false;
	}
	return true;
};


void TArtillery::UpdateArtilleryMatrix( void )
{
	glPushMatrix();

		glTranslatef( x,
					  Y(),
 					  z );

		glRotatef( zangle, 0,0,1 );
		glRotatef( yangle, 0,1,0 );
		glRotatef( xangle, 1,0,0 );

		glScalef( scalex, scaley, scalez );
		glGetFloatv( GL_MODELVIEW_MATRIX, matrix );

	glPopMatrix();
};


void TArtillery::Draw1stPerson( void )
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


void TArtillery::Draw( bool transp )
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

			glPushMatrix();
				glScalef( scalex, scaley, scalez );
				object.Draw(currentIndex,transp);
			glPopMatrix();

		glPopMatrix();

		if ( soldier!=NULL )
		{
			float x1 = -float(sin(-yangle*degToRad))*scale*0.5f;
			float z1 = float(cos(-yangle*degToRad))*scale*0.5f;
			soldier->X( x - x1 );
			soldier->Y( y );
			soldier->Z( z - z1 );
			soldier->Yangle( -yangle );
			soldier->Draw();
		}
/*
		// show collision box
		float x1,y1,z1, x2,y2,z2, x3,y3,z3, x4,y4,z4;
		Bounds( x1,y1,z1, x2,y2,z2, x3,y3,z3, x4,y4,z4 );

		float p1x,p1y,p1z, p2x,p2y,p2z;

		p1x = (x1+x2) * 0.5f;
		p1y = (y1+y2) * 0.5f;
		p1z = (z1+z2) * 0.5f;

		p2x = (x3+x4) * 0.5f;
		p2y = (y3+y4) * 0.5f;
		p2z = (z3+z4) * 0.5f;

		float p3x,p3y,p3z, p4x,p4y,p4z;

		p3x = (x1+x4) * 0.5f;
		p3y = (y1+y4) * 0.5f;
		p3z = (z1+z4) * 0.5f;

		p4x = (x2+x3) * 0.5f;
		p4y = (y2+y3) * 0.5f;
		p4z = (z2+z3) * 0.5f;
	
		glEnable( GL_COLOR_MATERIAL );
		glColor3ub(255,255,255);
		glBegin( GL_LINES );
			glVertex3f( p1x,p1y,p1z );
			glVertex3f( p2x,p2y,p2z );
			glVertex3f( p3x,p3y,p3z );
			glVertex3f( p4x,p4y,p4z );
		glEnd();
*/
		if ( showTarget )
			ShowTargeting();
	}
};


float TArtillery::Y( void ) const
{
	return y+hover*scaley;
};


float TArtillery::NY( void ) const
{
	return hover+n_y;
};

void TArtillery::Y( float _y )
{
	n_y = _y;
	y = _y;
}

void TArtillery::PauseAnimations( bool p )
{
	object.PauseAnimations( p );
};


bool TArtillery::InsideVehicle( const TPoint& point ) const
{
	return InsideVehicle( point.x, point.y, point.z );
};


bool TArtillery::InsideVehicle( float _x, float _y, float _z ) const
{
	float dist = (_x-X())*(_x-X()) + (_y-Y())*(_y-Y()) +
				 (_z-Z())*(_z-Z());
	return (dist < scale);
};


bool TArtillery::Collision( float x, float y, float z,
						float x1, float y1, float z1 ) const
{
	return object.Collision( currentIndex, matrix, x,y,z, x1,y1,z1 );
};


void TArtillery::Bounds( float& x1, float& y1, float& z1,
					 float& x2, float& y2, float& z2,
					 float& x3, float& y3, float& z3,
					 float& x4, float& y4, float& z4 ) const
{
	float sizex = SizeX() * 0.5f;
	float sizez = SizeZ() * 0.5f;

	x1 = -sizex;
	y1 = 0;
	z1 = -sizez;

	x2 = -sizex;
	y2 = 0;
	z2 = sizez;

	x3 = sizex;
	y3 = 0;
	z3 = sizez;

	x4 = sizex;
	y4 = 0;
	z4 = -sizez;

	MatrixMult( matrix, x1,y1,z1 );
	MatrixMult( matrix, x2,y2,z2 );
	MatrixMult( matrix, x3,y3,z3 );
	MatrixMult( matrix, x4,y4,z4 );
};

float TArtillery::BarrelAngle( void ) const
{
	return barrelAngle;
};

void TArtillery::BarrelAngle( float ba )
{
	barrelAngle = ba;
};

float TArtillery::TurretAngle( void ) const
{
	return turretAngle;
};

void TArtillery::TurretAngle( float ta )
{
	turretAngle = ta;
};

size_t TArtillery::Ammo( void ) const
{
	return numShells;
};

void TArtillery::SetProjectile( size_t _projectile )
{
	projectile = _projectile;
}

size_t TArtillery::Strength( void ) const
{
	return strength;
};

void TArtillery::Strength( size_t _strength )
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

float TArtillery::Fuel( void ) const
{
	return fuel;
};

void TArtillery::Fuel( float _fuel )
{
	fuel = _fuel;
};

void TArtillery::Explode( void ) 
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
			soundSystem->Stop( engineChannel );
		}
	}
};

void TArtillery::CalculateDamage( const TPoint& point, float damage )
{
	CalculateDamage( point.x,point.y,point.z, damage );
};

void TArtillery::CalculateDamage( float expx, float expy, float expz, 
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
				app->ExplodeAt( TPoint(x,Y(),z), etMaterial, 30 );
			}
#endif
		}
	}
};

void TArtillery::Rearm( void )
{
	if ( numShells<initNumShells )
	{
		numShells++;
	}
};

void TArtillery::Refuel( void )
{
	if ( fuel<100 )
	{
		fuel = fuel + 5;
	}
	if ( fuel>100 )
	{
		fuel = 100;
	}
};

void TArtillery::Repair( void )
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

void TArtillery::StopSounds( void )
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

bool TArtillery::CanEnter( void ) const
{
	return (!isaAI && strength>0 );
};

void TArtillery::SetSoldier( TCharacter* _soldier )
{
	soldier = _soldier;
};

void TArtillery::CharacterOwner( bool _characterOwner )
{
	characterOwner = _characterOwner;
};

void TArtillery::RemoveSoldier( void )
{
	soldier = NULL;
};

float TArtillery::Speed( void ) const
{
	return actualSpeed;
};

size_t TArtillery::netSet( size_t myId, size_t vid, byte* data )
{
	PreCond( data!=NULL );

	data[0] = id_artilleryData;
	data[1] = myId;
	data[2] = vid;
	data[3] = strength;
	data[4] = aiId;

	float* f = (float*)&data[5];
	f[0] = x;
	f[1] = y;
	f[2] = z;

	f[3] = xangle;
	f[4] = yangle;
	f[5] = zangle;

	f[6] = barrelAngle;
	f[7] = turretAngle;

	return netSize();
};

size_t TArtillery::netSize( void ) const
{
	return (5 + 8*sizeof(float));
};

size_t TArtillery::netGet( const byte* data )
{
	PreCond( data!=NULL );
	PreCond( data[0]==id_artilleryData );

	if ( strength!=data[3] )
	{
		strengthUpdated = true;
	}

	strength = data[3];
	aiId = data[4];

	float* f = (float*)&data[5];
	x = f[0];
	y = f[1];
	z = f[2];

	xangle = f[3];
	yangle = f[4];
	zangle = f[5];

	barrelAngle = f[6];
	turretAngle = f[7];

	return netSize();
}

//==========================================================================

