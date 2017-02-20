#include <precomp_header.h>

#include <win32/win32.h>
#include <object/binobject.h>
#include <object/landscape.h>
#include <object/plane.h>
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

const long kReloadTime = 1000;

//==========================================================================

TPlane::TPlane( void )
{
	type = vtPlane;

	distanceFromCamera = 0;
	isaDistanceObject = 0;
	switchDistance = 900; // arbitrary

	prop = NULL;
	tail = NULL;

	projectile = 0;
	fuel = 100;

	propX = propY = propZ = 0;
	tailX = tailY = tailZ = 0;

	engineSoundOn = false;
	engineSound = 0;
	engineChannel = 0;
	gunSound = 0;
	gunSoundOn = false;

	initNumMissiles = 0;
	initNumBombs = 5;

	numRounds = 200;
	numBombs = initNumBombs;
	numMissiles = initNumMissiles;

	fuelConsumption = 0.01f;

	hover = 0;
	reloadTime = 0;
	gunRange = 10;

	propAngle = 0;
	tailAngle = 0;

	gunId = 0;
	showTarget = false;

	ceiling = 50;

	angleAdjust = 0;
	n_angleAdjust = 0;

	flyingAngle = 5;
	flyingOffset = 0.25f;
	restAngle = 24;

	flying = false;
	flyingY = 0;
	n_flyingY = 0;

	speed = 0;
	logicCounter = 0;
	maxSpeed = 1;
	takeoffSpeed = maxSpeed * 0.75f;
	response = 0.01f;

	yoff = 0;

	turretAngle = 0;
	barrelAngle = 0;

	for ( size_t i=0; i<16; i++ )
	{
		matrix[i] = 0;
	}
	matrix[0] = 1;
	matrix[5] = 1;
	matrix[10] = 1;
	matrix[15] = 1;

	counter = 0;
};


TPlane::TPlane( const TPlane& v )
{
	operator=(v);
};


const TPlane& TPlane::operator = ( const TPlane& v )
{
	TVehicle::operator = (v);

	distanceFromCamera = v.distanceFromCamera;
	isaDistanceObject = v.isaDistanceObject;
	switchDistance = v.switchDistance;

	engineChannel = v.engineChannel;
	prevEnginePos = v.prevEnginePos;

	numRounds = v.numRounds;
	numBombs = v.numBombs;
	numMissiles = v.numMissiles;
	initNumMissiles = v.initNumMissiles;
	initNumBombs = v.initNumBombs;
	fuel = v.fuel;
	fuelConsumption = v.fuelConsumption;

	tailX = v.tailX;
	tailY = v.tailY;
	tailZ = v.tailZ;

	propX = v.propX;
	propY = v.propY;
	propZ = v.propZ;

	engineSound = v.engineSound;
	gunSound = v.gunSound;
	engineSoundOn = v.engineSoundOn;
	gunSoundOn = v.gunSoundOn;

	gunRange = v.gunRange;
	hover = v.hover;
	projectile = v.projectile;

	gunId = v.gunId;
	showTarget = v.showTarget;

	logicCounter = v.logicCounter;

	flyingAngle = v.flyingAngle;
	flyingOffset = v.flyingOffset;
	restAngle = v.restAngle;
	flying = v.flying;
	speed = v.speed;
	maxSpeed = v.maxSpeed;
	takeoffSpeed = v.takeoffSpeed;
	yoff = v.yoff;

	flyingY = v.flyingY;
	n_flyingY = v.n_flyingY;

	tail = v.tail;
	prop = v.prop;

	propAngle = v.propAngle;
	tailAngle = v.tailAngle;
	response = v.response;

	barrelAngle = v.barrelAngle;
	turretAngle = v.turretAngle;
	ceiling = v.ceiling;

	return *this;
};


void TPlane::operator = ( const TLandscapeObject& o )
{
	TVehicle::operator = (o);

	fuel = 100;
	numRounds = 200;
	gunId = 0;
	showTarget = false;
	flying = false;
	flyingY = 0;
	n_flyingY = 0;
	speed = 0;
	logicCounter = 0;
	yoff = 0;

	turretAngle = 0;
	barrelAngle = 0;

	angleAdjust = 0;
	n_angleAdjust = angleAdjust;

	y = o.TY() + hover + SizeY() * 0.5f;
	n_y = y;
	
	SetupObject();

	initNumBombs = o.Bombs();
	numBombs = initNumBombs;
	initNumMissiles = o.Shells();
	numMissiles = initNumMissiles;

	flyingAngle = 5;
	flyingOffset = 0.25f;
	flying = false;
	speed = 0;
	yoff = 0;

	size_t key = 0;

	Move(key,false,0,0,NULL);
	CommitMove();
};


void TPlane::operator=( TCharacter& dp )
{
	X( dp.X() );
	Z( dp.Z() );
	Y( dp.Y() );
	Xangle( dp.Xangle() );
	Yangle( dp.Yangle() );
	Zangle( dp.Zangle() );
	speed = dp.Speed();
	turretAngle = dp.TurretAngle();
	barrelAngle = dp.BarrelAngle();
	team = dp.Team();
};


void TPlane::operator=( TVehicleSettings& vs )
{
	PreCond( vs.IsPlane() );

	IconId( vs.IconId() );
	restAngle  = vs.RestAngle();
	gunRange   = vs.GunRange();
	response   = vs.Response();
	takeoffSpeed = vs.TakeoffSpeed();
	ceiling = vs.Ceiling();
	hover = vs.Hover();
	engineSound = vs.EngineSound();
	gunSound = vs.GunSound();
	fuelConsumption = vs.FuelConsumption();
};


TPlane::~TPlane( void )
{
};


bool TPlane::UpsideDown( void ) const
{
	if ( ( zangle >= 0 && zangle < 90 ) ||
		 ( zangle > 270  ) )
	{
		return false;
	}
	return true;
};


void TPlane::SetupObject( void )
{
	TVehicle::SetupObject();

	isaDistanceObject = (object.NumObjects()==2);

	// find prop and tail if possible in meshList for object 0
	size_t j = 0;
	for ( size_t i=0; i<object.NumMeshes(j); i++ )
	{
		TMesh* m = object.GetMesh(j,i);
		if ( m!=NULL )
		{
			float minx,miny,minz, maxx,maxy,maxz;

			if ( strncmp( m->Name().c_str(), "rudder", 6 )==0 ||
				 strncmp( m->Name().c_str(), "tail", 4 )==0 )
			{
				tail = m;
				if ( m->OwnerDraw() )
				{
					m->OwnerDraw( false );
					m->CalculateBoundaries();
					m->GetBoundaries( minx,miny,minz, maxx,maxy,maxz );
					tailX = (minx+maxx) * 0.5f * scalex;
					tailY = (miny+maxy) * 0.5f * scaley;
					tailZ = (minz+maxz) * 0.5f * scalez;
					m->CenterAxes( true,true,true );
				}
			}
			else if ( strncmp( m->Name().c_str(), "prop", 4 )==0 )
			{
				prop = m;
				if ( m->OwnerDraw() )
				{
					m->OwnerDraw( false );
					m->CalculateBoundaries();
					m->GetBoundaries( minx,miny,minz, maxx,maxy,maxz );
					propX = (minx+maxx) * 0.5f * scalex;
					propY = (miny+maxy) * 0.5f * scaley;
					propZ = (minz+maxz) * 0.5f * scalez;
					m->CenterAxes( true,true,true );
				}
			}
		}
	}
};


float TPlane::GetXZRadius( void ) const
{
	return xzRadius;
};


// bit pattern in key
void TPlane::Move( size_t& key, bool ctrlDown, float dxa, float dya, TCamera* cam )
{
	size_t time = ::GetTickCount();
	logicCounter++;

	if ( strength==0 )
	{
		speed = 0;
		return;
	}

	if ( cam!=NULL )
	{
		float dx = -cam->CameraX() - x;
		float dy = -cam->CameraY() - Y();
		float dz = -cam->CameraZ() - z;
		distanceFromCamera = dx*dx + dy*dy + dz*dz;
	}

	// can we move
	if ( (((key&15) > 0) || flying) && fuel>0 )
	{
		fuel = fuel - fuelConsumption;
	}

	GetBarrelMatrix();

	if ( (flying || speed!=0) && fuel>0 )
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
		if ( fuel>5 )
		{
			TPoint loc = TPoint(x,Y(),z + (1-speed)*10);
			if ( soundSystem!=NULL )
			{
				soundSystem->SetLocation( engineChannel, loc, prevEnginePos );
			}
			prevEnginePos = loc;
		}
		else // not enough fuel - start engine sputter
		{
			float cnt = float(logicCounter % 20);
			float per = float(cos(cnt*0.1*3.141592f));
			TPoint loc = TPoint(x,Y(),z + (10*per));
			if ( soundSystem!=NULL )
			{
				soundSystem->SetLocation( engineChannel, loc, prevEnginePos );
			}
			prevEnginePos = loc;
		}
	}

	if ( (key&aFire)==aFire && gunId==0 )
	{
		if ( numRounds>0 )
		{
			numRounds--;
			if ( !gunSoundOn )
			{
				gunSoundOn = true;
				prevMachineGunPos = TPoint(x,Y(),z);
				if ( soundSystem!=NULL )
				{
					machineGunChannel = soundSystem->Play( gunSound, prevMachineGunPos );
				}
			}
				// fire gun - 10==damage, 3==speed - experimental
#ifndef _DONTUSE
			if ( app!=NULL )
			{
				app->Fire( start, dirn, size_t(float(gunRange)*1.5f), 1 );
			}
#endif
		}
	}
	else if ( gunSoundOn )
	{
		gunSoundOn = false;
		if ( soundSystem!=NULL )
		{
			soundSystem->Stop( machineGunChannel );
		}
	}

	if ( gunSoundOn )
	{
		TPoint loc = TPoint(x,Y(),z);
		if ( soundSystem!=NULL )
		{
			soundSystem->SetLocation( machineGunChannel, loc, prevMachineGunPos );
		}
		prevMachineGunPos = loc;
	};

	if ( (key&aFire)==aFire && gunId==1 && (time>reloadTime) )
	{
		reloadTime = time + kReloadTime;
		if ( numBombs>0 && dirn.y<0 )
		{
			numBombs--;
			// fire gun - 10==damage, 3==speed - experimental
#ifndef _DONTUSE
			if ( app!=NULL )
			{
				app->FireProjectile( start, dirn, projectile, gunRange*2, 20 );
			}
#endif
		}
	}

	if ( (key&aFire)==aFire && gunId==2 && (time>reloadTime) )
	{
		reloadTime = time + kReloadTime/2;
		if ( numMissiles>0 )
		{
			numMissiles--;
#ifndef _DONTUSE
			if ( app!=NULL )
			{
				app->FireProjectile( start, dirn, projectile, gunRange, 20 );
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
	n_flyingY = flyingY;

	if ( showTarget )
	{
		turretAngle += dya;
		if ( turretAngle>5 )
			turretAngle = 5;
		if ( turretAngle<-5 )
			turretAngle = -5;

		barrelAngle += dxa;
		if ( barrelAngle>5 )
			barrelAngle = 5;
		if ( barrelAngle<-5 )
			barrelAngle = -5;
	}

	// land?
	if ( flying )
	{
		// on a reasonable angle?
		if ( (key&aDown)==0 )
		if ( float(fabs(flyingY-y)) < 0.5f )
		if ( n_zangle>-35 && n_zangle<35 )
		if ( n_xangle>=-2 && n_xangle<=2 )
		{
			n_zangle = 0;
			flying = false;
		}
	}

	// stick down -> go up/accel
	if ( (key&aDown)>0 )
	{
		if ( flying )
		{
			n_zangle -= (response*200);
			if ( n_zangle<0 )
				n_zangle += 360;
		}
		else
		{
			if ( speed<maxSpeed )
				speed += response;
		}
	}

	// stick fwd -> go down, or slow down
	if ( (key&aUp)>0 )
	{
		if ( flying )
		{
			n_zangle += (response*200);
			if ( n_zangle>360 )
				n_zangle -= 360;
		}
		else
		{
			if ( speed>0 )
				speed -= response;
			if ( speed<0 )
				speed = 0;
		}
	}

	float speedModifier = 1;
	float heightModifier = 0;
	if ( flying )
	{
		speedModifier = float(cos(n_zangle*degToRad));
		heightModifier = float(-sin(n_zangle*degToRad));
	}

	float ratio = 1;
	if ( !flying && speed<takeoffSpeed )
	{
		ratio = speed/takeoffSpeed;
	}
	else if ( !flying )
	{
		n_flyingY = y;
		flying = true;
	}
	else if ( flying )
	{
		n_flyingY = n_flyingY + 0.5f * heightModifier;
	}

	tailAngle = 0;
	if ( (key&aLeft)>0 )
	{
		n_yangle += 5.0f*ratio;
		tailAngle = -30;
		if ( flying )
		{
			n_xangle = n_xangle + 2;
			if ( n_xangle > 20 )
				n_xangle = 20;
		}
	}
	else if ( (key&aRight)>0 )
	{
		n_yangle -= 5.0f*ratio;
		tailAngle = 30;
		if ( flying )
		{
			n_xangle = n_xangle - 2;
			if ( n_xangle < -20 )
				n_xangle = -20;
		}
	}
	else
	{
		if ( flying )
		if ( n_xangle != 0 )
		{
			n_xangle = n_xangle * 0.8f;
			if ( n_xangle>-2 && n_xangle<2 )
				n_xangle = 0;
		}
	}

	float yangle1 = n_yangle*degToRad;
	float v1fcos = (float)cos(-yangle1);
	float v1fsin = -(float)sin(-yangle1);

	// always moving if has speed
	n_z += v1fcos*speed*speedModifier;
	n_x += v1fsin*speed*speedModifier;

#ifndef _DONTUSE
	// show smoke if hit
	if ( strength < 60 )
	if ( speed>0 )
	if ( Random() > (strength*0.01f) )
	if ( app!=NULL )
	{
		app->AddSmoke( x,Y(),z, -v1fcos*speed,0.01f,-v1fsin*speed );
	}
#endif

	float propSpeed = speed;
	if ( propSpeed > 0.4f )
		propSpeed = 0.4f;

	if ( fuel>0 )
	{
		if ( flying )
		{
			propAngle += 131;
		}
		else
		{
			propAngle += (propSpeed*131);
		}
	}
	if ( propAngle>360 )
	{
		propAngle -= 360;
	}

	if ( fuel<=0 )
	{
		float adj = (response*210);
		if ( UpsideDown() )
			n_zangle = n_zangle - adj;
		else
			n_zangle = n_zangle + adj;
	}

	if ( !flying )
	{
		n_angleAdjust = restAngle + (flyingAngle-restAngle)*ratio;
		yoff = flyingOffset*ratio;
	}
	else
	{
		n_angleAdjust = flyingAngle;
		yoff = flyingOffset;

		// reiced ceiling?
		if ( n_flyingY >= (ceiling*0.9f) )
		{
			if ( n_zangle>=270 )
			{
				n_zangle = 270 + ((n_zangle-270) * (1.05f+response*2));
				if ( n_zangle > 355 )
					n_zangle = 0;
			}
			else if ( n_zangle>180 )
			{
				n_zangle = 180 + ((n_zangle-180) * (0.95f-response*2));
				if ( n_zangle < 185 )
					n_zangle = 180;
			}
			if ( n_flyingY > ceiling )
				n_flyingY = ceiling;
		}
	}
};

void TPlane::GetBarrelMatrix( void )
{
	glPushMatrix();

		glLoadIdentity();
		glTranslatef( x,
					  Y(),
					  z );


		glRotatef( yangle, 0,1,0 );
		if ( gunId==0 || gunId==2 )
		{
			glRotatef( zangle-angleAdjust, 1,0,0 );
		}
		else
		{
			glRotatef( zangle-angleAdjust+45, 1,0,0 );
		}
		glRotatef( -xangle, 0,0,1 );

		glScalef( scalex, scaley, scalez );

		glRotatef( turretAngle, 0,1,0 );
		glRotatef( barrelAngle, 1,0,0 );

		float m[16];
		glGetFloatv( GL_MODELVIEW_MATRIX, m );
		gunMatrix.Matrix(m);
		GetGunDetails();

	glPopMatrix();
};


void TPlane::GetGunDetails( TPoint& _start, TPoint& _dirn, size_t& _gunRange )
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


void TPlane::GetGunDetails( void )
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


void TPlane::ShowTargeting( void )
{
	GetGunDetails();
	if ( gunSoundOn )
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


void TPlane::CommitMove( void )
{
	TVehicle::CommitMove();

	angleAdjust = n_angleAdjust;
	flyingY = n_flyingY;
	UpdatePlaneMatrix();
};


bool TPlane::LegalVehicleMove( TLandscape* landObj, TParams& params )
{
	size_t i;

	if ( landObj==NULL )
		return false;
	if ( strength==0 )
		return false;

	// collide with any other vehicle
	for ( i=0; i<params.numPlanes; i++ )
	{
		if ( params.planes[i].Strength()>0 )
		if ( &params.planes[i]!=this )
		{
			float dx = params.planes[i].X() - X();
			float dy = params.planes[i].Y() - Y();
			float dz = params.planes[i].Z() - Z();
			float d = dx*dx + dy*dy + dz*dz;
			float sz = (SizeX() + params.planes[i].SizeX()) * 0.5f;
			sz = sz * sz;
			if ( d < sz )
			{
				Strength( 0 );
#ifndef _DONTUSE
				if ( app!=NULL )
				{
					app->ExplodeAt( TPoint(x,Y(),z), etMaterial, 20 );
				}
#endif
				return false;
			}
		}
	}

	for ( i=0; i<params.numTanks; i++ )
	{
		if ( params.tanks[i].Strength()>0 )
		{
			float dx = params.tanks[i].X() - X();
			float dy = params.tanks[i].Y() - Y();
			float dz = params.tanks[i].Z() - Z();
			float d = dx*dx + dy*dy + dz*dz;
			float sz = (SizeX() + params.tanks[i].SizeX()) * 0.5f;
			sz = sz * sz;
			if ( d < sz )
			{
				Strength( 0 );
#ifndef _DONTUSE
				if ( app!=NULL )
				{
					app->ExplodeAt( TPoint(x,Y(),z), etMaterial, 20 );
				}
#endif
				return false;
			}
		}
	}

	// see if this move is a legal one
	// calculate new positions on landscape
	bool canMove = landObj->GetLORTriangle( *this );
	if ( !canMove )
	{
		Strength( 0 );
#ifndef _DONTUSE
		if ( app!=NULL )
		{
			app->ExplodeAt( TPoint(x,Y(),z), etMaterial, 20 );
		}
#endif
		return false;
	}

	// crash into water?
	float wl = landObj->WaterLevel();

	float x1,y1,z1, x2,y2,z2, x3,y3,z3, x4,y4,z4;
	Bounds( x1,y1,z1, x2,y2,z2, x3,y3,z3, x4,y4,z4 );

	if ( y1<wl || y2<wl || y3<wl || y4<wl )
	{
		Strength( 0 );
#ifndef _DONTUSE
		if ( app!=NULL )
		{
			app->ExplodeAt( TPoint(x,Y(),z), etMaterial, 20 );
		}
#endif
		return false;
	}

	// crash into landscape?
	float ly1;
	landObj->GetY(n_x,ly1,n_z);
	if ( NY() < ly1 )
	{
		Strength( 0 );
#ifndef _DONTUSE
		if ( app!=NULL )
		{
			app->ExplodeAt( TPoint(x,Y(),z), etMaterial, 20 );
		}
#endif
		return false;
	}

	// crash into object?
	size_t objId;
	if ( (objId=landObj->Collision( *this ))>0 )
	{
		Strength(0);
#ifndef _DONTUSE
		if ( app!=NULL )
		{
			app->ExplodeAt( TPoint(x,Y(),z), etMaterial, 20 );
		}
#endif
		return false;
	}
	return true;
};


void TPlane::UpdatePlaneMatrix( void )
{
	glPushMatrix();

		glTranslatef( x,
					  Y(),
 					  z );

		glRotatef( yangle, 0,1,0 );
		glRotatef( zangle-angleAdjust, 1,0,0 );
		glRotatef( -xangle, 0,0,1 );

		glScalef( scalez, scaley, scalez );
		glGetFloatv( GL_MODELVIEW_MATRIX, matrix );

	glPopMatrix();
};


void TPlane::Draw1stPerson( void )
{
	if ( strength>0 )
	{
		if ( gunSoundOn )
		{
			glEnable(GL_COLOR_MATERIAL );

			float range1 = float(gunRange) * Random();
			float range2 = float(gunRange) * Random();

			TPoint p1 = TPoint( start.x+dirn.x*range1,
								start.y+dirn.y*range1,
								start.z+dirn.z*range1 );
			TPoint p2 = TPoint( start.x+dirn.x*range2,
								start.y+dirn.y*range2,
								start.z+dirn.z*range2 );

			glColor3ub( 0,0,0 );
			glBegin( GL_LINES );
				glVertex3f( p1.x,p1.y,p1.z );
				glVertex3f( p2.x,p2.y,p2.z );
			glEnd();
		}

		if ( showTarget )
		{
			size_t range;
			TPoint start, dirn;
			GetGunDetails( start, dirn, range );

			// bomb view?
			if ( gunId==1 )
			{
				if ( range>0 )
				{
					dirn = dirn * (float(range) * 0.33f);
					TPoint p = start + dirn;
					::DrawCrosshair( 3, p, yangle );
					p = p + dirn;
					::DrawCrosshair( 3, p, yangle );
					p = p + dirn;
					::DrawCrosshair( 3, p, yangle );
				}
			}
			else
			{
				if ( range>0 )
				{
					dirn = dirn * float(range);
					TPoint p = start + dirn;
					::DrawCrosshair( 3, p, yangle );
				}
			}
		}
	}
};


void TPlane::Draw( bool transp )
{
	if ( strength>0 )
	{
		// obj can be considered to be at 0,0 at this stage
		glPushMatrix();

			glTranslatef( x,
						  Y(),
						  z );

			glRotatef( yangle, 0,1,0 );
			glRotatef( zangle-angleAdjust, 1,0,0 );
			glRotatef( -xangle, 0,0,1 );

			glPushMatrix();
				glScalef( scalex, scaley, scalez );

				currentIndex = 0;
				if ( isaDistanceObject )
				if ( distanceFromCamera > switchDistance )
				{
					currentIndex = 1;
				}
				object.Draw(currentIndex,transp);

			glPopMatrix();

			if ( prop!=NULL )
			{
				glPushMatrix();
					glTranslatef( propX, propY, propZ );
					glRotatef( propAngle, 0,0,1 );
					glScalef( scalex, scaley, scalez );
					prop->ForceDraw();
				glPopMatrix();
			}
			if ( tail!=NULL )
			{
				glTranslatef( tailX, tailY, tailZ );
				glRotatef( tailAngle, 0,1,0 );
				glScalef( scalex, scaley, scalez );
				tail->ForceDraw();
			}

		glPopMatrix();

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
		if ( showTarget || gunSoundOn )
			ShowTargeting();
	}
};


float TPlane::Y( void ) const
{
	if ( flying )
		return flyingY+yoff+SizeY()*0.5f;
	else
		return hover+y+yoff+SizeY()*0.5f;
};


float TPlane::NY( void ) const
{
	if ( flying )
		return n_flyingY+yoff+SizeY()*0.5f;
	else
		return hover+n_y+yoff+SizeY()*0.5f;
};


void TPlane::Y( float _y )
{
	y = _y;
	n_y = _y;
};


void TPlane::PauseAnimations( bool p )
{
	object.PauseAnimations( p );
};


bool TPlane::InsideVehicle( const TPoint& point ) const
{
	return InsideVehicle( point.x, point.y, point.z );
};


bool TPlane::InsideVehicle( float _x, float _y, float _z ) const
{
	float dist = (_x-X())*(_x-X()) + (_y-Y())*(_y-Y()) +
				 (_z-Z())*(_z-Z());
	return (dist < scale);
};


bool TPlane::Collision( float x, float y, float z,
						float x1, float y1, float z1 ) const
{
	return object.Collision( currentIndex, matrix, x,y,z, x1,y1,z1 );
};


void TPlane::SelectedGun( size_t _gunId )
{
	gunId = _gunId;
};


size_t TPlane::SelectedGun( void ) const
{
	return gunId;
};


float TPlane::Speed( void ) const
{
	return speed;
};


void TPlane::Bounds( float& x1, float& y1, float& z1,
					 float& x2, float& y2, float& z2,
					 float& x3, float& y3, float& z3,
					 float& x4, float& y4, float& z4 ) const
{
	float sx = SizeX() * 0.5f;
	float sz = SizeZ() * 0.5f;

	x1 = -sx;
	y1 = 0;
	z1 = -sz;

	x2 = -sx;
	y2 = 0;
	z2 = sz;

	x3 = sx;
	y3 = 0;
	z3 = sz;

	x4 = sx;
	y4 = 0;
	z4 = -sz;

	MatrixMult( matrix, x1,y1,z1 );
	MatrixMult( matrix, x2,y2,z2 );
	MatrixMult( matrix, x3,y3,z3 );
	MatrixMult( matrix, x4,y4,z4 );
};

float TPlane::BarrelAngle( void ) const
{
	return barrelAngle;
};

void TPlane::BarrelAngle( float ba )
{
	barrelAngle = ba;
};

float TPlane::TurretAngle( void ) const
{
	return turretAngle;
};

void TPlane::TurretAngle( float ta )
{
	turretAngle = ta;
};

size_t TPlane::Ammo( void ) const
{
	if ( gunId==0 )
	{
		return numRounds;
	}
	else if ( gunId==1 )
	{
		return numBombs;
	}
	else
	{
		return numMissiles;
	}
};

void TPlane::SetProjectile( size_t _projectile )
{
	projectile = _projectile;
}

size_t TPlane::Strength( void ) const
{
	return strength;
};

void TPlane::Strength( size_t _strength )
{
	if ( strength!=_strength )
	{
		strengthUpdated = true;
	};

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

float TPlane::Fuel( void ) const
{
	return fuel;
};

void TPlane::Fuel( float _fuel )
{
	fuel = _fuel;
};

void TPlane::Explode( void ) 
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

void TPlane::CalculateDamage( const TPoint& point, float damage )
{
	CalculateDamage( point.x,point.y,point.z, damage );
};

void TPlane::CalculateDamage( float expx, float expy, float expz, 
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
		Strength( strength );
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

void TPlane::Rearm( void )
{
	if ( numRounds<200 )
	{
		numRounds += 10;
	}
	if ( numRounds>200 )
	{
		numRounds = 200;
	}
	if ( numBombs<initNumBombs )
	{
		numBombs++;
	}
	if ( numMissiles<initNumMissiles )
	{
		numMissiles++;
	}
};

void TPlane::Refuel( void )
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

void TPlane::Repair( void )
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

void TPlane::GetTailProp( float& tx, float& ty, float& tz, 
						  float& px, float& py, float& pz )
{
	tx = tailX;
	ty = tailY;
	tz = tailZ;

	px = propX;
	py = propY;
	pz = propZ;
};

void TPlane::SetTailProp( float tx, float ty, float tz, 
						  float px, float py, float pz )
{
	tailX = tx;
	tailY = ty;
	tailZ = tz;

	propX = px;
	propY = py;
	propZ = pz;
};

void TPlane::StopSounds( void )
{
	if ( engineSoundOn )
	{
		engineSoundOn = false;
		if ( soundSystem!=NULL )
		{
			soundSystem->Stop( engineChannel );
		}
	}
	if ( gunSoundOn )
	{
		gunSoundOn = false;
		if ( soundSystem!=NULL )
		{
			soundSystem->Stop( machineGunChannel );
		}
	}
};

bool TPlane::CanEnter( void ) const
{
	return (!isaAI && strength>0 );
};

float TPlane::WeaponsRange( void ) const
{
	return float(gunRange);
};

float TPlane::AngleAdjust( void ) const
{
	return angleAdjust;
};

void TPlane::AngleAdjust( float _angleAdjust )
{
	angleAdjust = _angleAdjust;
};

float TPlane::FlyingY( void ) const
{
	return flyingY;
};

void TPlane::FlyingY( float _flyingY )
{
	flyingY = _flyingY;
};

float TPlane::TailAngle( void ) const
{
	return tailAngle;
};

void TPlane::TailAngle( float _tailAngle )
{
	tailAngle = _tailAngle;
};

bool TPlane::Flying( void ) const
{
	return flying;
};

void TPlane::Flying( bool _flying )
{
	flying = _flying;
};

size_t TPlane::netSet( size_t myId, size_t vid, byte* data )
{
	PreCond( data!=NULL );

	data[0] = id_planeData;
	data[1] = myId;
	data[2] = vid;
	data[3] = strength;
	data[4] = flying?1:0;
	data[5] = aiId;

	float* f = (float*)&data[6];
	f[0] = speed;
	f[1] = tailAngle;
	f[2] = flyingY;
	f[3] = angleAdjust;

	f[4] = xangle;
	f[5] = yangle;
	f[6] = zangle;

	f[7] = x;
	f[8] = y;
	f[9] = z;

	return netSize();
};

size_t TPlane::netSize( void ) const
{
	return (6 + 10*sizeof(float));
};

size_t TPlane::netGet( const byte* data )
{
	PreCond( data!=NULL );
	PreCond( data[0]==id_planeData );

	if ( strength != data[3] )
	{
		strengthUpdated = true;
	}

	strength = data[3];
	flying = data[4]==1;
	aiId = data[5];

	float* f = (float*)&data[6];
	speed = f[0];
	tailAngle = f[1];
	flyingY = f[2];
	angleAdjust = f[3];

	xangle = f[4];
	yangle = f[5];
	zangle = f[6];

	x = f[7];
	y = f[8];
	z = f[9];

	n_x = x;
	n_y = y;
	n_z = z;
	n_yangle = yangle;
	n_xangle = xangle;
	n_zangle = zangle;
	n_angleAdjust = angleAdjust;
	n_flyingY = flyingY;

	return netSize();
}

//==========================================================================

