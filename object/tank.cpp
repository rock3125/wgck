#include <precomp_header.h>

#include <win32/win32.h>
#include <common/sounds.h>

#include <object/binobject.h>
#include <object/tank.h>
#include <object/geometry.h>

#include <tank/tankapp.h>

#include <network/packet.h>

#if defined(_EDITOR) || defined(_PARSER) || defined(_VIEWER) || defined(_SPEEDTEST)
#define _DONTUSE
#endif

//==========================================================================

const float kPI = 3.1415927f;
const float degToRad = 0.01745329252033f;
const float radToDeg = 57.2957795112730f;

//==========================================================================

TTank::TTank( void )
	: recording(NULL)
{
	type = vtTank;

	distanceFromCamera = 0;
	isaDistanceObject = false;
	switchDistance = 900; // arbitrary

	submergeDepth = 0.4f;

	dirn = 0;
	diff = 0.0f;

	projectile = 0;

	dirnkey = 0;

	trackIndex = 0;
	numTracks = 0;

	initNumShells = 25;
	numShells = initNumShells;
	turretLess = false;
	occupied = true;

	actualSpeed = 0;
	trackIndex = 0;
	moving = false;

	turretX = 0;
	turretY = 0;
	turretZ = 0;

	engineChannel = 0;

	barrelX = 0;
	barrelY = 0;
	barrelZ = 0;

	gunRange = 10;
	kReloadTime = 3000;

	strength = 100;
	strengthUpdated = true;
	fuel = 100;

	fuelConsumption = 0.01f;

	barrelXadj = 0;
	barrelYadj = 0;
	barrelZadj = 0;

	engineSound = 0;
	gunSound = 0;
	gunSoundOn = false;
	engineSoundOn = false;

	reloadTime = 0;
	readyToFire = true;

	body[0] = NULL;
	body[1] = NULL;
	turret[0] = NULL;
	turret[1] = NULL;
	barrel[0] = NULL;
	barrel[1] = NULL;

	initialised = false;
	turretAngle = 0;
	barrelAngle = 0;
	shellRange = 10;

	fuelConsumption = 0.01f;
	moveCounter = 0;
	showTarget = false;
	speed = 0.5f;

	maxBarrelAngle = 5;
	minBarrelAngle = -5;

	barrelXadj = 0;
	barrelYadj = 0;
	barrelZadj = 0;
	gunRange = 40;

	prevEngineOn = false;
	prevFireOn = false;

	playback = false;
	record = false;
};

float TTank::SubmergeDepth( void ) const
{
	return submergeDepth;
};


void TTank::SubmergeDepth( float smd ) 
{
	submergeDepth = smd;
};

void TTank::operator=( TLandscapeObject& o )
{
	TVehicle::operator = (o);

	moving = false;
	strength = 100;
	strengthUpdated = true;
	readyToFire = true;
	strength = 100;
	fuel = 100;
	moveCounter = 0;
	showTarget = false;
	prevEngineOn = false;
	prevFireOn = false;
	trackIndex = 0;
	numTracks = 0;

	initNumShells = o.Shells();
	numShells = initNumShells;

	TString errStr;
	if ( !SetupTank( errStr ) )
	{
		WriteString( errStr.c_str() );
	}
	CommitMove();
};

void TTank::CommitMove( void )
{
	TVehicle::CommitMove();

	if ( moving )
	{
		numTracks++;

		tracks[trackIndex*6+0] = x3;
		tracks[trackIndex*6+1] = y3+0.1f;
		tracks[trackIndex*6+2] = z3;
		tracks[trackIndex*6+3] = x4;
		tracks[trackIndex*6+4] = y4+0.1f;
		tracks[trackIndex*6+5] = z4;

		trackIndex++;
		if ( trackIndex>=kMaxTracks )
			trackIndex = 0;
		if ( numTracks>kMaxTracks )
			numTracks = kMaxTracks;
	}
};


void TTank::operator=( TCharacter& dp )
{
	X( dp.X() );
	Y( dp.Y() );
	Z( dp.Z() );
	Xangle( dp.Xangle() );
	Yangle( dp.Yangle() );
	Zangle( dp.Zangle() );
	barrelAngle = dp.BarrelAngle();
	turretAngle = dp.TurretAngle();
	Team( dp.Team() );
};

void TTank::HoverHeight( float hh )
{
	hoverHeight = hh;
};

float TTank::HoverHeight( void ) const
{
	return hoverHeight;
};

void TTank::operator=( TVehicleSettings& vs )
{
	barrelXadj = vs.BarrelXadj();
	barrelYadj = vs.BarrelYadj();
	barrelZadj = vs.BarrelZadj();
	gunRange = vs.GunRange();
	engineSound = vs.EngineSound();
	gunSound = vs.GunSound();
	HoverHeight( vs.Hover() );
	maxBarrelAngle = vs.MaxBarrelAngle();
	minBarrelAngle = vs.MinBarrelAngle();
	speed = vs.Speed();
	fuelConsumption = vs.FuelConsumption();
	IconId( vs.IconId() );
	kReloadTime = vs.ReloadTime();
};

bool TTank::TurretLess( void ) const
{
	return turretLess;
};


TTank::~TTank( void )
{
	body[0] = NULL;
	body[1] = NULL;
	turret[0] = NULL;
	turret[1] = NULL;
	barrel[0] = NULL;
	barrel[1] = NULL;

	if ( recording!=NULL )
		delete recording;
	recording = NULL;

	initialised = false;
};


bool TTank::SetupTank( TString& errStr )
{
	size_t i;
	
	size_t numObjects = object.NumObjects();
	if ( numObjects>2 )
	{
		numObjects = 2;
	}
	if ( numObjects==2 )
	{
		isaDistanceObject = true;
	}

	for ( size_t j=0; j<numObjects; j++ )
	{
		body[j] = NULL;
		barrel[j] = NULL;
		turret[j] = NULL;

		// calculate global mesh properties
		object.CalculateBoundaries(j);

		bool foundBody = false;
		bool foundBarrel = false;
		bool foundTurret = false;
		for ( i=0; i<3; i++ )
		{
			TMesh* m = object.GetMesh(j,i);
			if ( m!=NULL )
			{
				PostCond( m->Name()!=NULL );
				if ( stricmp(m->Name().c_str(),"body")==0 )
				{
					foundBody = true;
					body[j] = m;
				}
				if ( stricmp(m->Name().c_str(),"barrel")==0 )
				{
					foundBarrel = true;
					barrel[j] = m;
				}
				if ( stricmp(m->Name().c_str(),"turret")==0 )
				{
					foundTurret = true;
					turret[j] = m;
				}
			}
		}
		if ( !foundBarrel )
		{
			errStr = "Tank object missing 'barrel'\n";
			return false;
		}
		if ( !foundBody )
		{
			errStr = "Tank object missing 'body'\n";
			return false;
		}
		turretLess = !foundTurret;

		// re-offset the turret to 0,0,0 relative to turretX,Y,Z
		float mix,miy,miz,max,may,maz;
		if ( turret[j]!=NULL && turret[j]->OwnerDraw() )
		{
			turret[j]->OwnerDraw( false );
			turret[j]->CalculateBoundaries();
			turret[j]->GetBoundaries( mix,miy,miz,max,may,maz );
			turretX = (mix+max) * 0.5f;
			turretY = (miy+may) * 0.5f;
			turretZ = (miz+maz) * 0.5f;

			float *vs = turret[j]->Vertices();
			size_t cnt = turret[j]->NumVertices();
			for ( i=0; i<cnt; i++ )
			{
				vs[i*3+0] = vs[i*3+0] - turretX;
				vs[i*3+1] = vs[i*3+1] - turretY;
				vs[i*3+2] = vs[i*3+2] - turretZ;
			}
		}

		if ( barrel[j]!=NULL && barrel[j]->OwnerDraw() )
		{
			barrel[j]->OwnerDraw( false );
			barrel[j]->CalculateBoundaries();
			barrel[j]->GetBoundaries( mix,miy,miz,max,may,maz );

			// re-offset the barrel to 0,0,0 relative to barrelX,Y,Z
			float* vs = barrel[j]->Vertices();
			size_t cnt = barrel[j]->NumVertices();
			for ( i=0; i<cnt; i++ )
			{
				vs[i*3+0] = vs[i*3+0] - (barrelX+turretX);
				vs[i*3+1] = vs[i*3+1] - (barrelY+turretY);
				vs[i*3+2] = vs[i*3+2] - (barrelZ+turretZ);
			}
		}
	}
	initialised = true;
	return true;
};


void TTank::SetupRecording( const TString& filename )
{
	if ( recording!=NULL )
		delete recording;
	recording = new TTankRecord();
	PostCond( recording!=NULL );

	record = true;
	playback = false;

	recordingFname = filename;
};


bool TTank::StopRecording( TString& errStr )
{
	PreCond( record==true );
	PreCond( recording!=NULL );
	record = false;
	playback = false;
	return recording->SaveBinary( recordingFname, errStr );
};


bool TTank::SetupPlayback( const TString& filename, TString& errStr )
{
	if ( recording!=NULL )
		delete recording;
	recording = new TTankRecord();
	PostCond( recording!=NULL );

	playback = true;
	record = false;
	return recording->LoadBinary( filename, errStr );
};


void TTank::StopPlayback( void )
{
	playback = false;
	record = false;
};


bool TTank::LegalVehicleMove( TLandscape* landObj, TParams& params )
{
	size_t i;

	if ( landObj==NULL || strength==0 )
		return false;

	bool canMove = true;

	if ( !isaAI )
	{
		// collide with any other vehicle
		for ( i=0; i<params.numPlanes; i++ )
		{
			if ( params.planes[i].Strength()>0 )
			{
				float dx = params.planes[i].X() - x;
				float dy = params.planes[i].Y() - y;
				float dz = params.planes[i].Z() - z;
				float d = dx*dx + dy*dy + dz*dz;
				float sz = (SizeX() + params.planes[i].SizeX());
				sz = sz * sz;
				if ( d < sz )
					return false;
			}
		}

		for ( i=0; i<params.numTanks; i++ )
		{
			if ( params.tanks[i].Strength()>0 )
			if ( &params.tanks[i]!=this )
			{
				float dx = params.tanks[i].X() - x;
				float dy = params.tanks[i].Y() - y;
				float dz = params.tanks[i].Z() - z;
				float d = dx*dx + dy*dy + dz*dz;
				float sz = (SizeX() + params.tanks[i].SizeX());
				sz = sz * sz;
				if ( d < sz )
					return false;
			}
		}

		for ( i=0; i<params.numArtillery; i++ )
		{
			if ( params.artillery[i].Strength()>0 )
			{
				float dx = params.artillery[i].X() - x;
				float dy = params.artillery[i].Y() - y;
				float dz = params.artillery[i].Z() - z;
				float d = dx*dx + dy*dy + dz*dz;
				float sz = (SizeX() + params.artillery[i].SizeX());
				sz = sz * sz;
				if ( d < sz )
					return false;
			}
		}

		for ( i=0; i<params.numFlak; i++ )
		{
			if ( params.flak[i].Strength()>0 )
			{
				float dx = params.flak[i].X() - x;
				float dy = params.flak[i].Y() - y;
				float dz = params.flak[i].Z() - z;
				float d = dx*dx + dy*dy + dz*dz;
				float sz = (SizeX() + params.flak[i].SizeX());
				sz = sz * sz;
				if ( d < sz )
					return false;
			}
		}
	}

	// see if this move is a legal one
	// calculate new positions on landscape
	canMove = landObj->GetLORTriangle( *this );

	// are we below allowed depth in the water?
	float wl = landObj->WaterLevel();
	float submergeDepth = SubmergeDepth();
	if ( (n_y1+submergeDepth)<wl || (n_y2+submergeDepth)<wl || 
		 (n_y3+submergeDepth)<wl || (n_y4+submergeDepth)<wl )
	{
		canMove = false;
	}

	// if whole tank suddenly under water => explode!
	if ( (n_y1+submergeDepth)<wl && (n_y2+submergeDepth)<wl &&
		 (n_y3+submergeDepth)<wl && (n_y4+submergeDepth)<wl )
	{
		Strength(0);
	}

	// did we pick up fuel or something?
	// did we collide with anything on the landscape
	if ( canMove )
	{
		size_t objId;
		if ( landObj->Collision( *this, objId ) )
		{
			canMove = false;
		}
	}
	return canMove;
};

void TTank::GetBarrelMatrix( void )
{
	glPushMatrix();

		glLoadIdentity();
		glTranslatef( x,
					  Y(),
 					  z );
		
		// do not change this order - all important!!!
		glRotatef( yangle, 0,1,0 );
		glRotatef( xangle, 1,0,0 );
		glRotatef( zangle, 0,0,1 );

		glScalef( scalex, scaley, scalez );
			
		if ( turret!=NULL )
		{
			glTranslatef( turretX, turretY, turretZ );
		}

		glRotatef( turretAngle, 0,1,0 );
		glRotatef( barrelAngle, 1,0,0 );
		glTranslatef( barrelX+barrelXadj, 
					  barrelY+barrelYadj, 
					  barrelZ+barrelZadj );

		float m[16];
		glGetFloatv( GL_MODELVIEW_MATRIX, m );
		matrix.Matrix(m);
		GetGunDetails();

	glPopMatrix();
};


void TTank::GetGunDetails( void )
{
	TPoint hp1;
	TPoint hp2(0,0,1);

	TPoint p1,p2;
	matrix.Mult( hp1, p1 );
	matrix.Mult( hp2, p2 );

	float dx = (p2.x - p1.x);
	float dy = (p2.y - p1.y);
	float dz = (p2.z - p1.z);
	dirn = TPoint( dx,dy,dz );

	float x = p1.x + dx;
	float y = p1.y + dy;
	float z = p1.z + dz;
	start = TPoint(x,y,z);
};


void TTank::GetGunDetails( TPoint& _start, TPoint& _dirn, size_t& _gunRange )
{
	GetGunDetails();
	_start = start;
	_dirn = dirn;
	_gunRange = gunRange;
};

void TTank::ShowTargeting( void )
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

bool TTank::Move( TLandscape& landObj, size_t& key, bool _showTarget, 
				  float dxa, float dya, TCamera* cam )
{
	if ( strength==0 )
		return false;

	long time = ::GetTickCount();

	// get new corners after new movement has been determined
	n_x = x;
	n_y = y;
	n_z = z;
	n_yangle = yangle;

	if ( cam!=NULL )
	{
		float dx = -cam->CameraX() - x;
		float dy = -cam->CameraY() - Y();
		float dz = -cam->CameraZ() - z;
		distanceFromCamera = dx*dx + dy*dy + dz*dz;
	}

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
		n_yangle += 5.0f;
	}
	else if ( (key&aRight)>0 )
	{
		n_yangle -= 5.0f;
	}

	dirnkey = key;

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

	n_x -= v1fsin*anglespeed*mult;
	n_z += v1fcos*anglespeed*mult;

	GetCornerPoints();

	// for tank speed
	diff = ((n_y1+n_y2)*0.5f - (n_y3+n_y4)*0.5f);

	// pre-calculate x, y and z angles
	float dy = -diff; //-((n_y1+n_y2)*0.5f-n_y);
	n_xangle = float(atan( dy / (object.SizeZ(currentIndex)*2) )) * radToDeg;
	dy = n_y1-n_y2;
	n_zangle = -float(atan( dy / (object.SizeX(currentIndex)*2) )) * radToDeg;

	// can we move
	moving = false;
	if ( fuel==0 )
	{
		key = key & (~15);
	}
	else
	{
		if ( (key&15) > 0 && fuel>0 )
		{
			moving = true;
			fuel = fuel - fuelConsumption;
		}
	}

	// firing-reload time
	if ( time > reloadTime )
	{
		readyToFire = true;
	}

	// sounds
	if ( (key&15)!=0 )
	{
		if ( engineSoundOn )
		{
			if ( soundSystem!=NULL )
			{
				soundSystem->SetLocation( engineChannel, TPoint(x,y,z), prevEnginePos );
			}
			prevEnginePos = TPoint(x,y,z);
		}
		if ( !engineSoundOn )
		{
			engineSoundOn = true;
			prevEnginePos = TPoint(x,y,z);
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

	if ( (key&15)>0 )
		moveCounter++;

	// set actual speed
	actualSpeed = 0;
	if ( (key&15)>0 )
	{
		actualSpeed = speed;
	}

	showTarget = _showTarget;
	if ( recording!=NULL )
	{
		if ( record )
		{
			recording->Add( this, key, showTarget, dxa, dya );
		}
		else if ( playback )
		{
			playback = recording->Get( this, key, showTarget, dxa, dya );
		}
	}

	if ( showTarget )
	{
		turretAngle += dya;
		if ( turretLess )
		{
			if ( turretAngle>5 )
				turretAngle = 5;
			if ( turretAngle<-5 )
				turretAngle = -5;
		}

		barrelAngle += dxa;
		if ( barrelAngle > maxBarrelAngle )
			barrelAngle = maxBarrelAngle;
		if ( barrelAngle<minBarrelAngle )
			barrelAngle = minBarrelAngle;
	}

	// engine stuff
	bool engineOn = ((key&15)!=0);
	PauseAnimations( !engineOn );

	// update internal matrix
	GetBarrelMatrix();

	// engine sounds
	if ( engineOn!=prevEngineOn )
	{
		prevEngineOn = engineOn;
	}

	// shell fire
	// fire?  => calculate target position
	bool fireOn = ((key&aFire)!=0) && (numShells>0) && readyToFire;

	if ( fireOn )
	{
		numShells--;
		prevFireOn = fireOn;
		key = key & ~aFire;

		readyToFire = false;
		reloadTime = time + kReloadTime;
		gunSoundOn = true;
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

		// gun recoil
		float yangle = Yangle() * degToRad;
		float v1fcos = -(float)cos(yangle);
		float v1fsin = (float)sin(yangle);
		X( X() + v1fsin*0.1f );
		Z( Z() + v1fcos*0.1f );
	}
	else if ( prevFireOn )
	{
		prevFireOn = false;

		// gun recoil
		float yangle = Yangle() * degToRad;
		float v1fcos = -(float)cos(yangle);
		float v1fsin = (float)sin(yangle);
		X( X() - v1fsin*0.1f );
		Z( Z() - v1fcos*0.1f );
	}
	
#ifndef _DONTUSE
	if ( strength < 60 )
	if ( Random() > (strength*0.01f) )
	{
		float v1fcos = -(float)cos(yangle);
		float v1fsin = (float)sin(yangle);
		app->AddSmoke( x,y,z, -v1fcos*0.1f,0.5f,-v1fsin*0.1f );
	}
#endif

	if ( gunSoundOn && readyToFire )
	{
		gunSoundOn = false;
	}

	return fireOn;
};

void TTank::Draw1stPerson( void )
{
	if ( !initialised )
		return;

	if ( strength>0 )
	{
		glEnable( GL_COLOR_MATERIAL );
		glColor3ub( 20,20,20 );
		glLineWidth(4);
		glBegin( GL_LINES );
		for ( size_t i=0; i<numTracks; i++ )
		{
			float x1 = tracks[i*6+0];
			float y1 = tracks[i*6+1];
			float z1 = tracks[i*6+2];

			float x2 = tracks[i*6+3];
			float y2 = tracks[i*6+4];
			float z2 = tracks[i*6+5];

			glVertex3f( x1+(x2-x1)*0.0f, y1+(y2-y1)*0.0f, z1+(z2-z1)*0.0f );
			glVertex3f( x1+(x2-x1)*0.1f, y1+(y2-y1)*0.1f, z1+(z2-z1)*0.1f );

			glVertex3f( x1+(x2-x1)*0.9f, y1+(y2-y1)*0.9f, z1+(z2-z1)*0.9f );
			glVertex3f( x1+(x2-x1)*1.0f, y1+(y2-y1)*1.0f, z1+(z2-z1)*1.0f );
		}
		glEnd();
		glLineWidth(1);

		if ( showTarget )
		{
			size_t range;
			TPoint start, dirn;
			GetGunDetails( start, dirn, range );
			if ( range>0 )
			{
				dirn = dirn * float(range);
				TPoint p = start + dirn;
				::DrawCrosshair( 3, p, yangle+turretAngle );
			}
		}
	}
};


void TTank::Draw( bool transp )
{
	if ( !initialised )
		return;

	if ( strength>0 )
	{
		glPushMatrix();

			glTranslatef( x,
						  Y(),
 						  z );
			
			// do not change this order - all important!!!
			glRotatef( yangle, 0,1,0 );
			glRotatef( xangle, 1,0,0 );
			glRotatef( zangle, 0,0,1 );

			glScalef( scalex, scaley, scalez );

			currentIndex = 0;
			if ( isaDistanceObject )
			if ( distanceFromCamera > switchDistance && object.NumObjects()>1 )
			{
				currentIndex = 1;
			}

			body[currentIndex]->Draw();
			if ( turret[currentIndex]!=NULL )
			{
				glTranslatef( turretX, turretY, turretZ );
			}
			glRotatef( turretAngle, 0,1,0 );
			if ( turret[currentIndex]!=NULL )
			{
				turret[currentIndex]->ForceDraw();
			}

			glRotatef( barrelAngle, 1,0,0 );
			glTranslatef( barrelX, barrelY, barrelZ );

			if ( barrel[currentIndex]!=NULL )
			{
				barrel[currentIndex]->ForceDraw();
			}

		glPopMatrix();

/*
		// draw corner points 4 testing
		float height = 1.7f;
		glColor3ub( 255,255,255 );
		glBegin( GL_LINES );
			glVertex3f( x1,y1+height,z1 );
			glVertex3f( x,y+height,z );
			glVertex3f( x2,y2+height,z2 );
			glVertex3f( x,y+height,z );

			glVertex3f( x1,y1+height,z1 );
			glVertex3f( x2,y2+height,z2 );
		glEnd();
*/

		glEnable( GL_COLOR_MATERIAL );
		glColor3ub( 20,20,20 );
		glLineWidth(4);
		glBegin( GL_LINES );
		for ( size_t i=0; i<numTracks; i++ )
		{
			float x1 = tracks[i*6+0];
			float y1 = tracks[i*6+1];
			float z1 = tracks[i*6+2];

			float x2 = tracks[i*6+3];
			float y2 = tracks[i*6+4];
			float z2 = tracks[i*6+5];

			glVertex3f( x1+(x2-x1)*0.0f, y1+(y2-y1)*0.0f, z1+(z2-z1)*0.0f );
			glVertex3f( x1+(x2-x1)*0.1f, y1+(y2-y1)*0.1f, z1+(z2-z1)*0.1f );

			glVertex3f( x1+(x2-x1)*0.9f, y1+(y2-y1)*0.9f, z1+(z2-z1)*0.9f );
			glVertex3f( x1+(x2-x1)*1.0f, y1+(y2-y1)*1.0f, z1+(z2-z1)*1.0f );
		}
		glEnd();
		glLineWidth(1);

		if ( showTarget )
			ShowTargeting();
	}
};


const float& TTank::TurretAngle( void ) const
{
	return turretAngle;
};


void TTank::TurretAngle( const float& a)
{
	turretAngle = a;
};


const float& TTank::BarrelAngle( void ) const
{
	return barrelAngle;
};


void TTank::BarrelAngle( const float& a)
{
	float angle = a;
	if ( angle <minBarrelAngle )
		angle = minBarrelAngle;
	if ( angle > maxBarrelAngle )
		angle = maxBarrelAngle;

	barrelAngle = angle;
};

bool TTank::InsideVehicle( const TPoint& point ) const
{
	return InsideVehicle( point.x, point.y, point.z );
};

bool TTank::InsideVehicle( float _x, float _y, float _z ) const
{
	float dist = (_x-x)*(_x-x) + (_y-y)*(_y-y) + (_z-z)*(_z-z);
	float s = scale*scale;
	return (dist<s);
};


bool TTank::Occupied( void ) const
{
	return occupied;
};

void TTank::Occupied( bool o )
{
	occupied = o;
};

void TTank::SetProjectile( size_t _projectile )
{
	projectile = _projectile;
}

size_t TTank::Ammo( void ) const
{
	return numShells;
};

size_t TTank::Strength( void ) const
{
	return strength;
};

void TTank::Strength( size_t _strength )
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
			app->ExplodeAt( TPoint(x,y,z), etMaterial, 30 );
		}
#endif
	}
};

float TTank::Fuel( void ) const
{
	return fuel;
};

void TTank::Fuel( float _fuel )
{
	fuel = _fuel;
};

void TTank::Explode( void ) 
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
};

void TTank::CalculateDamage( const TPoint& point, float damage )
{
	CalculateDamage( point.x,point.y,point.z, damage );
};


void TTank::CalculateDamage( float expx, float expy, float expz, 
							 float damage )
{
	if ( strength==0 )
	{
		return;
	}

	float dist = (expx-x)*(expx-x) + (expy-y)*(expy-y) + (expz-z)*(expz-z);
	dist = dist * 0.12f;
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

void TTank::Rearm( void )
{
	if ( numShells<initNumShells )
	{
		numShells++;
	}
};

void TTank::Refuel( void )
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

void TTank::Repair( void )
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

bool TTank::CanEnter( void ) const
{
	return (!isaAI && strength>0 );
};

void TTank::GetTurretBarrel( float& tx, float& ty, float& tz, 
							 float& bx, float& by, float& bz )
{
	tx = turretX;
	ty = turretY;
	tz = turretZ;

	bx = barrelX;
	by = barrelY;
	bz = barrelZ;
};

void TTank::SetTurretBarrel( float tx, float ty, float tz, 
							 float bx, float by, float bz )
{
	turretX = tx;
	turretY = ty;
	turretZ = tz;

	barrelX = bx;
	barrelY = by;
	barrelZ = bz;
};

void TTank::StopSounds( void )
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

float TTank::Speed( void ) const
{
	return actualSpeed;
};

float TTank::Y( void ) const
{
	return y + hoverHeight * scaley;
};

void TTank::Y( float _y )
{
	n_y = _y;
	y = _y;
};

void TTank::PauseAnimations( bool p )
{
	object.PauseAnimations( p );
};

size_t TTank::netSet( size_t myId, size_t vid, byte* data )
{
	PreCond( data!=NULL );

	data[0] = id_tankData;
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

size_t TTank::netSize( void ) const
{
	return (5 + 8*sizeof(float));
};

size_t TTank::netGet( const byte* data )
{
	PreCond( data!=NULL );
	PreCond( data[0]==id_tankData );

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

