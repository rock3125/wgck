#include <precomp_header.h>

#include <win32/win32.h>

#include <tank/tankapp.h>
#include <object/geometry.h>
#include <ai/v2ai.h>

#if defined(_EDITOR) || defined(_PARSER) || defined(_VIEWER)
#define _DONTUSE
#endif

//==========================================================================

const float Deg2Rad = 0.017453293f;
const float Rad2Deg = 57.29577951f;

const float kRocketCeiling = 60;

//==========================================================================

V2::V2( void )
	: soundSystem( NULL )
{
	Init();
};

V2::V2( const V2& v2 )
{
	operator=(v2);
};

V2::~V2( void )
{
};

void V2::Init( void )
{
	rocket = NULL;

	strength = 100;
	strengthUpdated = true;
	launched = false;
	launchTime = 0;
	waitingForLaunch =false;
	countDown = 0;
	startHeight = 0;
	speed = 0;
	exploded = false;
	sendOverNetwork = true;

	gameImportance = 0;

	currentState = none;

	aiId = 0;

	x = 0;
	y = 0;
	z = 0;

	xangle = 0;
	yangle = 0;
	zangle = 0;

	targetx = 0;
	targety = 0;
	targetz = 0;

	scalex = 1;
	scaley = 1;
	scalez = 1;
	scale = 1;

	hover = 0;

	engineSound = 0;
	engineChannel = 0;
	engineSoundOn = false;
};

const V2& V2::operator=( const V2& v2 )
{
	rocket = v2.rocket;

	x = v2.x;
	y = v2.y;
	z = v2.z;

	aiId = v2.aiId;

	speed = v2.speed;
	soundSystem = v2.soundSystem;

	startHeight = v2.startHeight;
	currentState = v2.currentState;
	exploded = v2.exploded;
	sendOverNetwork = v2.sendOverNetwork;

	launched = v2.launched;
	waitingForLaunch = v2.waitingForLaunch;
	countDown = v2.countDown;
	launchTime = v2.launchTime;

	name = v2.name;
	engineSound = v2.engineSound;
	hover = v2.hover;

	xangle = v2.xangle;
	yangle = v2.yangle;
	zangle = v2.zangle;

	gameImportance = v2.gameImportance;

	targetx = v2.targetx;
	targety = v2.targety;
	targetz = v2.targetz;

	scalex = v2.scalex;
	scaley = v2.scaley;
	scalez = v2.scalez;
	scale = v2.scale;

	return *this;
};

size_t V2::AIID( void ) const
{
	return aiId;
};


void V2::AIID( size_t _aiId )
{
	aiId = _aiId;
};

bool V2::SendOverNetwork( void ) const
{
	return sendOverNetwork;
};

void V2::SendOverNetwork( bool _sendOverNetwork )
{
	sendOverNetwork = _sendOverNetwork;
};

void V2::SetTarget( float tx, float ty, float tz )
{
	targetx = tx;
	targety = ty;
	targetz = tz;
};

float V2::X( void ) const
{
	return x;
};

float V2::Y( void ) const
{
	return y;
};

float V2::Z( void ) const
{
	return z;
};

float V2::Xangle( void ) const
{
	return xangle;
};

float V2::Yangle( void ) const
{
	return yangle;
};

float V2::Zangle( void ) const
{
	return zangle;
};

void V2::StartTimer( void )
{
	launchTime = ::GetTickCount() + countDown * 1000;
	waitingForLaunch = true;
};

size_t V2::CountDown( void ) const
{
	size_t time = ::GetTickCount();

	if ( launchTime > time )
	{
		return size_t(launchTime-time) / 1000;
	}
	return 0;
};

void V2::Logic( TApp* app, TLandscape& landObj, TLandscapeAI& land, 
				TParams& params )
{
	PreCond( app!=NULL );

	if ( exploded )
		return;

	if ( soundSystem!=NULL )
	{
		if ( engineSoundOn )
		{
			TPoint loc = TPoint(x,y,z);
			soundSystem->SetLocation( engineChannel, loc, prevEnginePos );
			prevEnginePos = loc;
		}
	}

	if ( waitingForLaunch )
	{
		if ( ::GetTickCount() > launchTime )
		{
			launched = true;
			waitingForLaunch =false;
			launchTime = 0;
		}
	}
	else if ( launched )
	{
		if ( currentState==s_none )
		{
			currentState = s_takeoff;
			EngineOn();
		}
		V2AI(app,landObj,land,params);
	}

	// check collision
	float landy;
	landObj.GetY( x,landy,z );
	if ( y <= landy )
	{
		exploded = true;
	}
	if ( strength==0 )
	{
		exploded = true;
	}
	if ( exploded )
	{
		StopSounds();
		strength = 0;
		app->ExplodeAt( TPoint(x-2,y,z-2), etMaterial, 50 );
		app->ExplodeAt( TPoint(x-2,y,z+2), etMaterial, 50 );
		app->ExplodeAt( TPoint(x+2,y,z-2), etMaterial, 50 );
		app->ExplodeAt( TPoint(x+2,y,z+2), etMaterial, 50 );
		app->ExplodeAt( TPoint(x,y,z), etFire, 50 );
	}
};

void V2::Draw( bool transp )
{
	if ( strength>0 )
	{
		// obj can be considered to be at 0,0 at this stage
		glPushMatrix();

			glTranslatef( x,
						  y,
						  z );

			glRotatef( zangle, 0,0,1 );
			glRotatef( -yangle, 0,1,0 );
			glRotatef( xangle, 1,0,0 );

			glScalef( scalex, scaley, scalez );
			if ( rocket!=NULL )
			{
				rocket->Draw( 0, transp );
			}
		glPopMatrix();
	}
};

void V2::operator=( TVehicleSettings& vs )
{
	PreCond( vs.IsV2() );

	engineSound = vs.EngineSound();
	hover = vs.Hover();
};


const V2& V2::operator = ( const TLandscapeObject& o )
{
	PreCond( o.Object()!=NULL );
	rocket = const_cast<TCompoundObject*>(o.Object());

	strength = 100;
	strengthUpdated = true;

	xangle = o.RX();
	zangle = o.RZ();
	yangle = o.RY();

	scalex = o.ScaleX();
	scaley = o.ScaleY();
	scalez = o.ScaleZ();
	scale = scalex;
	if ( scaley > scale )
		scale = scaley;
	if ( scalez > scale )
		scale = scalez;

	x = o.TX();
	y = o.TY() + hover;
	z = o.TZ();

	startHeight = y;
	
	name = o.Name();
	gameImportance = o.GameImportance();
	countDown = o.V2Countdown();

	sendOverNetwork = true;

	StartTimer();

	return *this;
};

size_t V2::GameImportance( void ) const
{
	return gameImportance;
};

size_t V2::Strength( void ) const
{
	return strength;
};

void V2::Strength( size_t _strength )
{
	if ( strength!=_strength )
	{
		strengthUpdated = true;
	}
	strength = _strength;
	if ( strength==0 )
	{
		StopSounds();
	}
};

float V2::Distance( float px, float py, float pz )
{
	float dist = (x-px)*(x-px) + 
				 (y-py)*(y-py) +
				 (z-pz)*(z-pz);
	return dist;
};

void V2::SetSoundSystem( TSound* _soundSystem )
{
	soundSystem = _soundSystem;
}

bool V2::InsideVehicle( const TPoint& point ) const
{
	return InsideVehicle( point.x, point.y, point.z );
};

bool V2::InsideVehicle( float _x, float _y, float _z ) const
{
	float dist = (_x-X())*(_x-X()) + (_y-Y())*(_y-Y()) +
				 (_z-Z())*(_z-Z());
	return (dist < scale);
};

void V2::CalculateDamage( const TPoint& point, float damage )
{
	CalculateDamage( point.x,point.y,point.z, damage );
};

void V2::CalculateDamage( float expx, float expy, float expz, 
						  float damage )
{
	float dist = (expx-x)*(expx-x) + (expy-Y())*(expy-Y()) + (expz-z)*(expz-z);
	dist = dist * 0.05f;

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
	}
};

void V2::EngineOn( void )
{
	if ( soundSystem!=NULL )
	{
		if ( !engineSoundOn )
		{
			engineSoundOn = true;
			prevEnginePos = TPoint(x,y,z);
			engineChannel = soundSystem->Play( engineSound, prevEnginePos );
		}
	}
};

void V2::StopSounds( void )
{
	if ( engineSoundOn )
	{
		engineSoundOn = false;
		soundSystem->Stop( engineChannel );
	}
};

bool V2::TurnVehicle( float px, float pz )
{
	PreCond( rocket!=NULL );

	float yangle1 = yangle * Deg2Rad;
	float v1fcos = -(float)cos(yangle1);
	float v1fsin = (float)sin(yangle1);

	float tx = x - v1fsin;
	float tz = z - v1fcos;

	yangle1 = (yangle-10) * Deg2Rad;
	v1fcos = -(float)cos(yangle1);
	v1fsin = (float)sin(yangle1);
	float tx1 = x - v1fsin;
	float tz1 = z - v1fcos;

	yangle1 = (yangle+10) * Deg2Rad;
	v1fcos = -(float)cos(yangle1);
	v1fsin = (float)sin(yangle1);
	float tx2 = x - v1fsin;
	float tz2 = z - v1fcos;

	float d1 = (tx-px)*(tx-px) + (tz-pz)*(tz-pz);
	float d2 = (tx1-px)*(tx1-px) + (tz1-pz)*(tz1-pz);
	float d3 = (tx2-px)*(tx2-px) + (tz2-pz)*(tz2-pz);

	bool ready = true;
	if ( d2 < d1 )
	{
		yangle -= 1;
		ready = false;
	}
	else if ( d3 < d1 )
	{
		yangle += 1;
		ready = false;
	}
	return ready;
};

void V2::V2AI( TApp* app, TLandscape& landObj, TLandscapeAI& land, 
			   TParams& params )
{
	switch ( currentState )
	{
	case s_none:
		{
			speed = 0;
			break;
		}
	case s_takeoff:
		{
			// climb to kRocketCeiling
			if ( speed<=1 )
			{
				speed = speed + 0.01f;
			}
			y = y + speed;

			// level the rocket to 90 degrees
			if ( y >= (kRocketCeiling-20) )
			{
				xangle = ((y-(kRocketCeiling-20)) / (kRocketCeiling)) * 90;
			}

			// set yangle before I get all the way up
			TurnVehicle( targetx, targetz );

			app->AddSmoke( x,y,z, Random() - 0.5f, -0.01f, Random() - 0.5f );

			// at max height?
			if ( y >= (kRocketCeiling-5) )
			{
				currentState = s_findenemy;
				xangle = 90;
			}
			break;
		}
	case s_findenemy:
		{
			float yangle1 = yangle * Deg2Rad;
			float v1fcos = -(float)cos(yangle1);
			float v1fsin = (float)sin(yangle1);

			TurnVehicle( targetx, targetz );
			if ( Distance( targetx,targety,targetz ) > (kRocketCeiling*kRocketCeiling) )
			{
				if ( Distance( targetx,targety,targetz ) < (kRocketCeiling*kRocketCeiling*2) )
				{
					currentState = s_decend;
				}
				x -= v1fsin;
				z -= v1fcos;
			}
			else
			{
			}

			app->AddSmoke( x + v1fsin,y,z + v1fcos, 
						   v1fsin, -0.001f, v1fcos );

			break;
		}
	case s_decend:
		{
			TurnVehicle( targetx, targetz );

			float yangle1 = yangle * Deg2Rad;
			float v1fcos = -(float)cos(yangle1);
			float v1fsin = (float)sin(yangle1);

			x -= v1fsin;
			y -= 0.5f;
			z -= v1fcos;

			while ( xangle < 180 )
				xangle += 1;

			app->AddSmoke( x + v1fsin,y,z + v1fcos, 
						   v1fsin, -0.001f, v1fcos );

			break;
		};
	}
};

bool V2::StrengthUpdated( void ) const
{
	return strengthUpdated;
};

void V2::StrengthUpdated( bool _strengthUpdated )
{
	strengthUpdated = _strengthUpdated;
};

size_t V2::netSet( size_t myId, size_t vid, byte* data )
{
	PreCond( data!=NULL );

	data[0] = id_v2Data;
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

	// send the actual time left before launch will occur
	long* d = (long*)&data[5+6*sizeof(float)];
	long dt = launchTime - ::GetTickCount();
	if ( dt<=0 )
	{
		dt = 0;
	}
	d[0] = dt;

	return netSize();
};

size_t V2::netSize( void ) const
{
	return (5 + 6*sizeof(float) + sizeof(long));
};

size_t V2::netGet( const byte* data )
{
	PreCond( data!=NULL );
	PreCond( data[0]==id_v2Data );

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

	long time = ::GetTickCount();
	long* d = (long*)&data[5+6*sizeof(float)];
	launchTime = time + d[0];

	return netSize();
}

//==========================================================================

