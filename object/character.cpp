#include <precomp_header.h>

#include <win32/win32.h>
#include <object/landscape.h>
#include <object/character.h>
#include <tank/tankapp.h>

#if defined(_EDITOR) || defined(_PARSER) || defined(_VIEWER) || defined(_SPEEDTEST)
#define _DONTUSE
#endif

//==========================================================================

const float degToRad = 0.01745329252f;

//==========================================================================

TCharacter::TCharacter( void )
	: character(NULL),
	  parachute( NULL )
{
	type = vtNone;
	vehicleType = vtNone;

	s_angle = 0;
	s_scale = 0.25f;
	s_numSurfaces = 0;
	s_numVertices = 0;
	s_vertices = NULL;
	s_surfaces = NULL;

	running = false;
	ctrlDown = false;
	owner = false;
	showParachute = false;
	sendOverNetwork = true;

	projectileGraphic = 0;

	walkSound = 0;
	walkSoundOn = false;
	gun1Sound = 0;
	gun2Sound = 0;
	gun3Sound = 0;
	machinegunFire = false;
	machineGunChannel = 0;
	walkChannel = 0;

	jumping = false;
	jumpStarted = false;
	jumpCounter = 0;

	mg = true;
	bazooka = true;
	dynamite = true;
	usingMG = false;

	keys = 0;

	barrelAngle = 0;
	turretAngle = 0;

	gunXoff = 0;
	speed = 0.25f;
	characterSpeed = speed;

	backOffTime = 0;

	initExplosives = 5;
	initProjectiles = 5;

	submergeDepth = 0.6f;
	submerged = false;

	numWeapons = 0;
	selectedWeapon = 0;
	weapons = NULL;
};


TCharacter::~TCharacter( void )
{
	StopSounds();

	if ( character!=NULL && owner )
		delete character;
	character = NULL;
	weapons = NULL;
	parachute = NULL;

	ClearShadow();
};


void TCharacter::ClearShadow( void )
{
	s_angle = 0;
	s_scale = 0.25f;
	s_numSurfaces = 0;
	s_numVertices = 0;

	if ( s_vertices!=NULL )
	{
		delete[] s_vertices;
		s_vertices = NULL;
	}

	if ( s_surfaces!=NULL )
	{
		delete[] s_surfaces;
		s_surfaces = NULL;
	}
};

void TCharacter::GetBoundingBox( TPoint& _minPoint, TPoint& _maxPoint ) const
{
	if ( character!=NULL )
	{
		TMatrix mat;
		float m[16];
		character->GetOpenGLTransformation( m );
		mat.Matrix( m );
		mat.Mult( minPoint, _minPoint );
		mat.Mult( maxPoint, _maxPoint );
	}
};

void TCharacter::Reset( void )
{
	strength = 100;
	strengthUpdated = true;

	running = false;
	ctrlDown = false;
	jumpStarted = false;
	submerged = false;
	machinegunFire = false;
	showParachute = false;
	sendOverNetwork = true;
	jumpCounter = 0;
	keys = 0;
	if ( character!=NULL )
		character->SetSequence(idle);
	numRounds[0] = 20;
	numRounds[1] = 40;
	numRounds[2] = initProjectiles;
	numRounds[3] = initExplosives;
};


TCharacter::TCharacter( const TCharacter& c )
{
	operator=(c);
};


void TCharacter::SetWeapons( size_t _numWeapons, TBinObject** _weapons )
{
	PreCond( _numWeapons==4 );

	numWeapons = _numWeapons;
	weapons = _weapons;
	selectedWeapon = 0;
	numRounds[0] = 20;
	numRounds[1] = 40;
	numRounds[2] = initProjectiles;
	numRounds[3] = initExplosives;
};


const TCharacter& TCharacter::operator=( const TCharacter& c )
{
	TVehicle::operator = (c);

	ClearShadow();

	s_angle = c.s_angle;
	s_scale = c.s_scale;
	s_numSurfaces = c.s_numSurfaces;
	s_numVertices = c.s_numVertices;

	if ( s_numSurfaces > 0 )
	{
		size_t size = 2*sizeof(float)*s_numVertices;
		s_vertices = new float[size];
		for ( size_t i=0; i<s_numVertices*2; i++ )
		{
			s_vertices[i] = c.s_vertices[i];
		}
	}

	if ( s_numVertices > 0 )
	{
		size_t size = 3*sizeof(size_t)*s_numSurfaces;
		s_surfaces = new size_t[size];
		for ( size_t i=0; i<s_numSurfaces*3; i++ )
		{
			s_surfaces[i] = c.s_surfaces[i];
		}
	}

	if ( character!=NULL && owner )
		delete character;
	character = c.character;
	owner = false;

	originalFilename = originalFilename;

	vehicleType = c.VehicleType;
	vehicleId = c.vehicleId;

	speed = c.speed;
	usingMG = c.usingMG;
	sendOverNetwork = c.sendOverNetwork;

	initExplosives = c.initExplosives;
	initProjectiles = c.initProjectiles;

	parachute = c.parachute;
	showParachute = c.showParachute;

	backOffTime = c.backOffTime;
	walkSound = c.walkSound;
	gun1Sound = c.gun1Sound;
	gun2Sound = c.gun2Sound;
	gun3Sound = c.gun3Sound;
	machineGunChannel = c.machineGunChannel;
	walkChannel = c.walkChannel;

	mg = c.mg;
	bazooka = c.bazooka;
	dynamite = c.dynamite;

	projectileGraphic = c.projectileGraphic;

	handPoint1 = c.handPoint1;
	handPoint2 = c.handPoint2;

	machinegunFire = c.machinegunFire;
	walkSoundOn = c.walkSoundOn;

	name = c.character->Name();
	running = c.running;
	keys = c.keys;
	ctrlDown = c.ctrlDown;

	jumping = c.jumping;
	jumpCounter = c.jumpCounter;
	jumpStarted = c.jumpStarted;

	weapons = c.weapons;
	numWeapons = c.numWeapons;
	selectedWeapon = c.selectedWeapon;
	gunXoff = c.gunXoff;

	if ( numWeapons>0 )
	{
		for ( size_t i=0; i<numWeapons; i++ )
		{
			numRounds[i] = c.numRounds[i];
		};
	}

	jumpX = c.jumpX;
	jumpY = c.jumpY;
	jumpZ = c.jumpZ;

	barrelAngle = c.barrelAngle;
	turretAngle = c.turretAngle;

	submergeDepth = c.submergeDepth;
	waterLevel = c.waterLevel;
	submerged = c.submerged;

	minPoint = c.minPoint;
	maxPoint = c.maxPoint;

	return *this;
};


void TCharacter::operator=( const TLandscapeObject& obj )
{
	TVehicle::operator = (obj);

	sendOverNetwork = true; // reset

	initExplosives = float(obj.Bombs());
	numRounds[3] = initExplosives;

	initProjectiles = float(obj.Shells());
	numRounds[2] = initProjectiles;

	switch ( obj.CharacterType() )
	{
	case 0: // soldier
	{
		AvailableWeapons( true, false, true );
		Speed( 0.25f );
		break;
	}
	case 1: // engineer
	{
		AvailableWeapons( false, true, true );
		Speed( 0.20f );
		break;
	}
	case 2: // saboteur
	{
		AvailableWeapons( false, true, true );
		Speed( 0.3f );
		break;
	}
	case 3: // special forces
	{
		AvailableWeapons( true, true, true );
		Speed( 0.35f );
		break;
	}
	}
	characterSpeed = Speed();
};

bool TCharacter::ReloadCharacter( TString& errStr )
{
	PreCond( originalFilename.length() > 0 );
	return Load( originalFilename, errStr );
};

bool TCharacter::Load( const TString& fname, TString& errStr )
{
	if ( character!=NULL && owner )
		delete character;
	character = new MDL();
	owner = true;
	if ( !character->Load( fname, errStr ) )
	{
		errStr = "\nError loading character bin (" + fname + "): " + errStr + "\n";
		WriteString( errStr.c_str() );
		return false;
	};
	name = character->Name();
	originalFilename = fname;

	LoadShadow( "data\\person.shd", errStr );

	return true;
};


bool TCharacter::LegalCharacterMove( TLandscape& landObj, TParams& params, 
									 size_t& vehicleType, size_t& vehicleId )
{
	size_t i;
	bool canMove = true;

	// collide with any other vehicle
	for ( i=0; i<params.numPlanes; i++ )
	{
		if ( params.planes[i].Strength()>0 )
		{
			float dx = params.planes[i].X() - n_x;
			float dy = params.planes[i].Y() - n_y;
			float dz = params.planes[i].Z() - n_z;
			float d = dx*dx + dy*dy + dz*dz;
			float sz = params.planes[i].SizeX();
			sz = sz * sz;
			if ( d < sz )
			{
				vehicleType = vtPlane;
				vehicleId = i;
				return true;
			}
		}
	}

	for ( i=0; i<params.numTanks; i++ )
	{
		if ( params.tanks[i].Strength()>0 )
		{
			float dx = params.tanks[i].X() - n_x;
			float dy = params.tanks[i].Y() - n_y;
			float dz = params.tanks[i].Z() - n_z;
			float d = dx*dx + dy*dy + dz*dz;
			float sx = params.tanks[i].SizeX();
			float sy = params.tanks[i].SizeY();
			float sz = params.tanks[i].SizeZ();
			float s =  (sx*sx + sz*sz + sy*sy);
			if ( d < s )
			{
				vehicleType = vtTank;
				vehicleId = i;
				return true;
			}
		}
	}

	for ( i=0; i<params.numFlak; i++ )
	{
		if ( params.flak[i].Strength()>0 )
		{
			float dx = params.flak[i].X() - n_x;
			float dy = params.flak[i].Y() - n_y;
			float dz = params.flak[i].Z() - n_z;
			float d = dx*dx + dy*dy + dz*dz;
			float sx = params.flak[i].SizeX();
			float sy = params.flak[i].SizeY();
			float sz = params.flak[i].SizeZ();
			float s =  (sx*sx + sz*sz + sy*sy);
			if ( d < s )
			{
				vehicleType = vtFlak;
				vehicleId = i;
				return true;
			}
		}
	}

	for ( i=0; i<params.numArtillery; i++ )
	{
		if ( params.artillery[i].Strength()>0 )
		{
			float dx = params.artillery[i].X() - n_x;
			float dy = params.artillery[i].Y() - n_y;
			float dz = params.artillery[i].Z() - n_z;
			float d = dx*dx + dy*dy + dz*dz;
			float sx = params.artillery[i].SizeX();
			float sy = params.artillery[i].SizeY();
			float sz = params.artillery[i].SizeZ();
			float s =  (sx*sx + sz*sz + sy*sy);
			if ( d < s )
			{
				vehicleType = vtArtillery;
				vehicleId = i;
				return true;
			}
		}
	}

	for ( i=0; i<params.numMG42s; i++ )
	{
		if ( params.mg42[i].Strength()>0 )
		{
			float dx = params.mg42[i].X() - n_x;
			float dy = params.mg42[i].Y() - n_y;
			float dz = params.mg42[i].Z() - n_z;
			float d = dx*dx + dy*dy + dz*dz;
			float sx = params.mg42[i].SizeX();
			float sy = params.mg42[i].SizeY();
			float sz = params.mg42[i].SizeZ();
			float s =  (sx*sx + sz*sz + sy*sy);
			if ( d < s )
			{
				vehicleType = vtMG42;
				vehicleId = i;
				return true;
			}
		}
	}

	// see if this move is a legal one
	// calculate new positions on landscape
	if ( !jumping )
	{
		landObj.GetLORTriangle( *this );
	}

	// did we collide with anything on the landscape
	if ( canMove )
	if ( landObj.Collision(*this) )
	{
		canMove = false;
	}

	if ( !canMove && jumping )
	{
		jumping = false;
		landObj.GetLORTriangle( *this );
	}
	return canMove;
};


void TCharacter::Move( size_t& _keys, float _waterLevel,
					   bool _ctrlDown, float dxa, float dya )
{
	size_t time = ::GetTickCount();

	if ( character!=NULL )
	{
		float hp1[3], hp2[3];

		// get hand bones
		size_t cnt = character->NumBones();
/*
		if ( cnt==43 )
		{
			character->GetBone( 40, hp1 );
			character->GetBone( 41, hp2 );
		}
		else
		{
			character->GetBone( 37, hp1 );
			character->GetBone( 38, hp2 );
		}
*/
		character->GetBone( cnt-3, hp1 );
		character->GetBone( cnt-2, hp2 );

		handPoint1 = TPoint( hp1 );
		handPoint2 = TPoint( hp2 );
	}

	if ( strength==0 )
		return;

	if ( showParachute )
	{
		_keys &= (~(aUp|aDown|aJump));
	}
	keys = _keys;
	ctrlDown = _ctrlDown;
	n_x = x;
	n_y = y;
	n_z = z;
	fwd_x = x;
	fwd_z = z;
	n_yangle = yangle;

	if ( ctrlDown )
	{
		gunXoff += (dxa * 0.2f);
		if ( gunXoff<-4 )
		{
			gunXoff = -4;
		}
		if ( gunXoff>4 )
		{
			gunXoff = 4;
		}

		if ( dya<0 )
		{
			n_yangle -= dya;
		}
		if ( dya>0 )
		{
			n_yangle -= dya;
		}
	}

	waterLevel = _waterLevel;

    float ya = n_yangle*degToRad;
	float v1fcos = (float)cos(ya);
	float v1fsin = (float)sin(ya);

	float anglespeed = speed;
	if ( submerged || jumping ) // 2x slower under water
		anglespeed = anglespeed * 0.5f;

	// sounds
	if ( walkSoundOn )
	{
		TPoint loc = TPoint(x,y,z);
		if ( soundSystem!=NULL )
		{
			soundSystem->SetLocation( walkChannel, loc, prevWalkPos );
		}
		prevWalkPos = loc;
	}

	if ( character->GetSequence()==run )
	{
		if ( !walkSoundOn )
		{
			walkSoundOn = true;
			prevWalkPos = TPoint(x,y,z);
			if ( soundSystem!=NULL )
			{
				walkChannel = soundSystem->Play( walkSound, prevWalkPos );
			}
		}
	}
	else if ( walkSoundOn )
	{
		walkSoundOn = false;
		if ( soundSystem!=NULL )
		{
			soundSystem->Stop( walkChannel );
		}
		walkChannel = 0;
	}

	if ( (keys&aFire)==aFire )
	{
		if ( numRounds[selectedWeapon]>0 )
		{
			size_t range = 8;
			bool fireBazooka = false;
			if ( selectedWeapon==0 && time > backOffTime )
			{
				if ( soundSystem!=NULL )
				{
					soundSystem->Play( gun1Sound, TPoint(x,y,z) );
				}
				range = 4;
				numRounds[selectedWeapon] = numRounds[selectedWeapon] - 1;
				backOffTime = time + 2000;
			}
			else if ( selectedWeapon==1 && !machinegunFire )
			{
				if ( soundSystem!=NULL )
				{
					machineGunChannel = soundSystem->Play( gun2Sound, TPoint(x,y,z) );
				}
				machinegunFire = true;
			}
			else if ( selectedWeapon==2 && time > backOffTime )
			{
				fireBazooka = true;
				if ( soundSystem!=NULL )
				{
					soundSystem->Play( gun3Sound, TPoint(x,y,z) );
				}
				range = 12;
				numRounds[selectedWeapon] = numRounds[selectedWeapon] - 1;
				backOffTime = time + 4000;
			}
			else if ( selectedWeapon==3 && time > backOffTime )
			{
				backOffTime = time + 500;
#ifndef _DONTUSE
				if ( app!=NULL )
				{
					// drop explosive on a 5 second fuse
					app->DropExplosive( x,y+0.1f,z, 10000 );
				}
#endif
				numRounds[selectedWeapon] = numRounds[selectedWeapon] - 1;
			};

			if ( selectedWeapon==1 )
				numRounds[selectedWeapon] = numRounds[selectedWeapon] - 0.2f;
#ifndef _DONTUSE
			if ( app!=NULL )
			{
				TPoint start, dirn;
				size_t range;
				GetGunDetails( start, dirn, range );

				if ( selectedWeapon==2 )
				{
					if ( fireBazooka )
						app->FireProjectile( start, dirn, projectileGraphic, range, 20 );
				}
				else
				{
					app->Fire( start, dirn, range, 1 );
				}
			}
#endif
		}
	}
	else if ( machinegunFire )
	{
		machinegunFire = false;
		if ( soundSystem!=NULL )
		{
			soundSystem->Stop( machineGunChannel );
		}
	}

	if ( !jumping )
	{
		if ( (keys&aUp)>0 )
		{
			n_x -= v1fsin*anglespeed;
			n_z += v1fcos*anglespeed;
			fwd_x = n_x - v1fsin*anglespeed*2;
			fwd_z = n_z + v1fcos*anglespeed*2;
		}
		else if ( (keys&aDown)>0 )
		{
			n_x += v1fsin*anglespeed;
			n_z -= v1fcos*anglespeed;
			fwd_x = n_x + v1fsin*anglespeed*2;
			fwd_z = n_z - v1fcos*anglespeed*2;
		}
		if ( (keys&aLeft)>0 )
		{
			n_yangle -= 7.0f;
		}
		if ( (keys&aRight)>0 )
		{
			n_yangle += 7.0f;
		}
	}
	else
	{
		keys = keys & ~(aUp|aDown);
		float jc = float(jumpCounter);
		n_x = jumpX - v1fsin*anglespeed * jc;
		n_z = jumpZ + v1fcos*anglespeed * jc;
		n_y = jumpY + float(sin(jc*0.1f*3.141592f)) * 2;

		fwd_x = n_x - v1fsin*anglespeed*jc*2;
		fwd_z = n_z + v1fcos*anglespeed*jc*2;

		jumpCounter++;
		if ( jumpCounter>=10 )
			jumping = false;
	}

	if ( (keys&aJump)>0 && !jumping && !submerged )
	{
		jumping = true;
		jumpStarted = true;
		jumpCounter = 2;
		jumpX = n_x;
		jumpY = n_y;
		jumpZ = n_z;
	}
};


void TCharacter::Logic( void )
{
	if ( strength>0 )
  	if ( character!=NULL )
	{
		character->Logic();

		GetMatrix();

		float minx, miny, minz;
		float maxx, maxy, maxz;
		character->GetBoundingBox(minx,miny,minz, maxx,maxy,maxz );
		minPoint = TPoint(minx,miny,minz);
		maxPoint = TPoint(maxx,maxy,maxz);
	}
};

size_t TCharacter::Sequence( void ) const
{
	if ( character!=NULL )
	{
		return character->GetSequence();
	}
	return idle;
};

void TCharacter::Sequence( size_t _sequence )
{
	if ( character!=NULL )
	{
		character->SetSequence( _sequence );
	}
};

void TCharacter::SetSequence( bool changed )
{
	PreCond( character!=NULL );

	size_t idleSeq = idle;
	if ( (y+submergeDepth) < waterLevel )
	{
		if ( !submerged )
			changed = true;
		submerged = true;
		idleSeq = treadWater;
	}
	else
	{
		if ( submerged )
			changed = true;
		submerged = false;
	}

	// make me run when applicable
	if ( changed && !running )
	{
		running = true;
		if ( submerged )
			character->SetSequence(swim);
		else
			character->SetSequence(run);
	}
	else if ( changed && jumping )
	{
		running = false;
		jumpStarted = false;
		character->SetSequence(jump);
	}
	else if ( changed )
	{
		running = false;
		character->SetSequence(idleSeq);
	}
};


void TCharacter::CommitMove( void )
{
	PreCond( character!=NULL );

	showParachute = false;
	if ( !jumping && !submerged )
	{
		// falling?
		if ( (y-n_y) > 1 )
		{
			//character->SetSequence(treadWater); // falling sequence
			if ( (y-n_y) > 4 )
			{
				showParachute = true;
				y = y - 0.2f;
			}
			else
			{
				y = y - 1; // can only go down - no other movement possible
			}
		}
		else // normal
		{
			x = n_x;
			y = n_y;
			z = n_z;
		}
	}
	else // normal
	{
		x = n_x;
		y = n_y;
		z = n_z;
	}

	yangle = n_yangle;

	// max depth
	if ( (y+submergeDepth+0.1f) < waterLevel )
		y = waterLevel-(submergeDepth+0.1f);

	bool changed = ((keys&(aUp|aDown))>0 && !running) || ((keys&(aUp|aDown))==0 && running) ||
					(jumping && character->GetSequence()!=jump) ||
					(!jumping && character->GetSequence()==jump);

	SetSequence( changed );
};


void TCharacter::CommitAngles( void )
{
	PreCond( character!=NULL );

	yangle = n_yangle;

	// max depth
	if ( (y+submergeDepth+0.1f) < waterLevel )
		y = waterLevel-(submergeDepth+0.1f);

	bool changed = ((keys&(aUp|aDown))>0 && !running) || ((keys&(aUp|aDown))==0 && running) ||
					(jumping && character->GetSequence()!=jump) ||
					(!jumping && character->GetSequence()==jump);

	SetSequence( changed );
};


void TCharacter::Draw1stPerson( void )
{
	PreCond( character!=NULL );

	if ( strength>0 )
	{
		glPushMatrix();
			character->SetTransRot( x,y+0.46f,z,-90,-yangle-90,0);
			TBinObject* gun = NULL;
			size_t gunId = selectedWeapon;
			if ( gunId < numWeapons && numWeapons>0)
			{
				gun = weapons[gunId];
			}
		glPopMatrix();

		if ( gun!=NULL && ctrlDown )
		{
			size_t range;
			TPoint start, dirn;
			GetGunDetails( start, dirn, range );
			if ( range>0 )
			{
				dirn = dirn * float(range);
				TPoint p = start + dirn;
				::DrawCrosshair( 1, p, -yangle );
			}
		}
	}
};


void TCharacter::Draw( void )
{
	PreCond( character!=NULL );

	if ( strength>0 )
	{
		TBinObject* gun = NULL;
		glEnable( GL_COLOR_MATERIAL );
		glColor3ub( 255,255,255 );
		glPushMatrix();

			character->SetTransRot( x,y+0.46f,z,-90,-yangle-90,0);

			size_t gunId = selectedWeapon;
			if ( gunId < numWeapons && numWeapons>0)
				gun = weapons[gunId];

			// can't show explosive if we have none
			if ( (gunId==3 && numRounds[gunId]==0) || usingMG )
				gun = NULL;

			character->Draw( gun );

		glPopMatrix();

		// show parachute above?
		if ( parachute!=NULL && showParachute )
		{
			glPushMatrix();
				glTranslatef( x,y+3,z);
				glScalef( 2,2,2 );
				glRotatef( -yangle, 0,1,0 );
				parachute->Draw();
			glPopMatrix();
		}
/*
		// draw collision line for checking
		glPushMatrix();
			float height = 0.46f;
			glColor3ub( 255,255,255 );
			glEnable( GL_COLOR_MATERIAL );
			glBegin( GL_LINES );
			glVertex3f( x,y+height,z);
			glVertex3f( fwd_x,n_y+height,fwd_z );
			glEnd();
		glPopMatrix();
*/
		// show targetting?
		if ( gun!=NULL && ctrlDown )
		{
			ShowTargeting();
		}
	}
};


void TCharacter::GetMatrix( void )
{
	float m[16];
	character->GetOpenGLTransformation( m );
	matrix.Matrix(m);
};


void TCharacter::GetGunDetails( TPoint& start, TPoint& dirn, size_t& range )
{
	// plot trajectory course
	range = 0;
	switch ( selectedWeapon )
	{
		case 0: // magnum
			{
				range = 20;
				break;
			}
		case 1: // laser rifle
			{
				range = 25;
				break;
			}
		case 2:
			{
				range = 30;
				break;
			}
	}

	TPoint p1( matrix.Mult( handPoint1 ) );
	TPoint p2( matrix.Mult( handPoint2 ) );

	float dx = (p2.x - p1.x) * 10;
	float dy = (p2.y - p1.y) * 10 - gunXoff * 0.01f;
	float dz = (p2.z - p1.z) * 10;
	dirn = TPoint( dx,dy,dz );
	dirn = dirn * 7.5f;

	float x = p1.x + dx;
	float y = p1.y + dy;
	float z = p1.z + dz;
	start = TPoint(x,y,z);
};


void TCharacter::ShowTargeting( void )
{
	PreCond( character!=NULL );

	size_t range;
	TPoint start, dirn;
	GetGunDetails( start, dirn, range );

	glColor3ub(255,255,255);
	glEnable(GL_COLOR_MATERIAL );
	glBegin( GL_POINTS );
		while ( range>0 )
		{
			glVertex3f( start.x, start.y, start.z );
			start = start + dirn;
			range--;
		}
	glEnd();
};

float TCharacter::BarrelAngle( void ) const
{
	return barrelAngle;
};

void TCharacter::BarrelAngle( float _barrelAngle )
{
	barrelAngle = _barrelAngle;
};

float TCharacter::TurretAngle( void ) const
{
	return turretAngle;
};

void TCharacter::TurretAngle( float _turretAngle )
{
	turretAngle = _turretAngle;
};

float TCharacter::SubmergeDepth( void ) const
{
	return submergeDepth;
};


bool TCharacter::Submerged( void ) const
{
	return submerged;
};


void TCharacter::SelectedGun( size_t gunId )
{
	if ( gunId==1 && !mg )
		return;
	if ( gunId==2 && !bazooka )
		return;
	if ( gunId==3 && !dynamite )
		return;

	selectedWeapon = gunId;
};


size_t TCharacter::SelectedGun( void ) const
{
	return selectedWeapon;
};

float TCharacter::GunXoff( void ) const
{
	return gunXoff;
};

void TCharacter::JumpOutOfVehicle( TTank& tank )
{
	float scale = float(sqrt( tank.SizeX()*tank.SizeX() +
							  tank.SizeY()*tank.SizeY() +
							  tank.SizeZ()*tank.SizeZ() ) );

	float offsetX = -float(sin(tank.Yangle()*degToRad))*scale;
	float offsetZ = float(cos(tank.Yangle()*degToRad))*scale;
	x = tank.X() + offsetX;
	y = tank.Y() + tank.HoverHeight() + tank.SizeY();
	z = tank.Z() + offsetZ;
	yangle = tank.Yangle();
	jumping = true;
	jumpStarted = true;
	jumpCounter = 2;
	jumpX = x;
	jumpY = y;
	jumpZ = z;
	speed = characterSpeed;
};


void TCharacter::JumpOutOfVehicle( TPlane& plane )
{
	float scale = float(sqrt( plane.SizeX()*plane.SizeX() +
							  plane.SizeY()*plane.SizeY() +
							  plane.SizeZ()*plane.SizeZ() ) ) * 1.5f;

	float offsetX = -float(sin(plane.Yangle()*degToRad))*scale;
	float offsetZ = float(cos(plane.Yangle()*degToRad))*scale;
	x = plane.X() + offsetX;
	y = plane.Y();
	z = plane.Z() + offsetZ;
	yangle = plane.Yangle();
	jumping = true;
	jumpStarted = true;
	jumpCounter = 2;
	jumpX = x;
	jumpY = y;
	jumpZ = z;
	speed = characterSpeed;
};


void TCharacter::JumpOutOfVehicle( TFlak& flak )
{
	float scale = float(sqrt( flak.SizeX()*flak.SizeX() +
							  flak.SizeY()*flak.SizeY() +
							  flak.SizeZ()*flak.SizeZ() ) );

	float offsetX = -float(sin(flak.Yangle()*degToRad))*scale;
	float offsetZ = float(cos(flak.Yangle()*degToRad))*scale;
	x = flak.X() + offsetX;
	y = flak.Y();
	z = flak.Z() + offsetZ;
	yangle = flak.Yangle();
	jumping = true;
	jumpStarted = true;
	jumpCounter = 2;
	jumpX = x;
	jumpY = y;
	jumpZ = z;
	speed = characterSpeed;
};


void TCharacter::JumpOutOfVehicle( TArtillery& artillery )
{
	float scale = float(sqrt( artillery.SizeX()*artillery.SizeX() +
							  artillery.SizeY()*artillery.SizeY() +
							  artillery.SizeZ()*artillery.SizeZ() ) );

	float offsetX = -float(sin(artillery.Yangle()*degToRad))*scale;
	float offsetZ = float(cos(artillery.Yangle()*degToRad))*scale;
	x = artillery.X() + offsetX;
	y = artillery.Y();
	z = artillery.Z() + offsetZ;
	yangle = artillery.Yangle();
	jumping = true;
	jumpStarted = true;
	jumpCounter = 2;
	jumpX = x;
	jumpY = y;
	jumpZ = z;
	speed = characterSpeed;
};


void TCharacter::JumpOutOfVehicle( TMG42& mg42 )
{
	float scale = float(sqrt( mg42.SizeX()*mg42.SizeX() +
							  mg42.SizeY()*mg42.SizeY() +
							  mg42.SizeZ()*mg42.SizeZ() ) ) * 2;

	float angle = -mg42.Yangle();
	float offsetX = -float(sin(angle*degToRad))*scale;
	float offsetZ = float(cos(angle*degToRad))*scale;
	x = mg42.X() - offsetX;
	y = mg42.Y();
	z = mg42.Z() - offsetZ;
	yangle = -angle;
	jumping = true;
	jumpStarted = true;
	usingMG = false;
	jumpCounter = 2;
	jumpX = x;
	jumpY = y;
	jumpZ = z;
	speed = characterSpeed;
}


void TCharacter::Character( MDL* _character )
{
	character = _character;
	owner = false;
}


MDL* TCharacter::Character( void ) const
{
	return character;
};


bool TCharacter::Owner( void ) const
{
	return owner;
};


void TCharacter::Owner( bool _owner )
{
	owner = _owner;
};


bool TCharacter::InsideVehicle( const TPoint& point ) const
{
	return InsideVehicle( point.x, point.y, point.z );
};

bool TCharacter::InsideVehicle( float _x, float _y, float _z ) const
{
	float dist = (_x-x)*(_x-x) + (_y-y)*(_y-y) + (_z-z)*(_z-z);
	float s = 2;
	return (dist<s);
};


void TCharacter::JumpIntoVehicle( void )
{
	if ( walkSoundOn )
	{
		walkSoundOn = false;
		if ( soundSystem!=NULL )
		{
			soundSystem->Stop( walkChannel );
		}
	}
};

void TCharacter::SetSounds( size_t _walkSound, size_t _gun1Sound,
						    size_t _gun2Sound, size_t _gun3Sound,
							size_t _projectileGraphic,
							TBinObject* _parachute )
{
	walkSound = _walkSound;
	gun1Sound = _gun1Sound;
	gun2Sound = _gun2Sound;
	gun3Sound = _gun3Sound;
	projectileGraphic = _projectileGraphic;
	parachute = _parachute;
}

size_t TCharacter::Ammo( void ) const
{
	return size_t(numRounds[selectedWeapon]);
};

float TCharacter::Speed( void ) const
{
	return speed;
};

void TCharacter::Speed( float _speed )
{
	speed = _speed;
};

void TCharacter::AvailableWeapons( bool _mg, bool _bazooka, bool _dynamite )
{
	selectedWeapon = 0;
	mg = _mg;
	bazooka = _bazooka;
	dynamite = _dynamite;
};

void TCharacter::CalculateDamage( const TPoint& point, float damage )
{
	CalculateDamage( point.x,point.y,point.z, damage );
};

void TCharacter::CalculateDamage( float expx, float expy, float expz, 
								  float damage )
{
	float dist = (expx-x)*(expx-x) + (expy-y)*(expy-y) + (expz-z)*(expz-z);
	dist = dist * 0.04f;

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
	}
};

void TCharacter::Rearm( void )
{
	if ( numRounds[0] < 20 )
		numRounds[0]++;
	else if ( numRounds[1] < 40 )
		numRounds[1]++;
	else if ( numRounds[2] < initProjectiles )
		numRounds[2]++;
	else if ( numRounds[3] < initExplosives )
		numRounds[3]++;
};

void TCharacter::StopSounds( void )
{
	if ( walkSoundOn )
	if ( soundSystem!=NULL )
		soundSystem->Stop( walkChannel );
	if ( machinegunFire )
	if ( soundSystem!=NULL )
		soundSystem->Stop( machineGunChannel );
	walkSoundOn = false;
	machinegunFire = false;
};

void TCharacter::SetupForMG42( void )
{
	jumping = false;
	jumpStarted = false;
	character->SetSequence(idle);
	usingMG = true;
};

size_t TCharacter::VehicleType( void ) const
{
	return vehicleType;
};

void TCharacter::VehicleType( size_t _vehicleType )
{
	vehicleType = _vehicleType;
};

size_t TCharacter::VehicleId( void ) const
{
	return vehicleId;
};

void TCharacter::VehicleId( size_t _vehicleId )
{
	vehicleId = _vehicleId;
};

bool TCharacter::SendOverNetwork( void ) const
{
	return sendOverNetwork;
};

void TCharacter::SendOverNetwork( bool _sendOverNetwork )
{
	sendOverNetwork = _sendOverNetwork;
};

size_t TCharacter::netSet( size_t myId, size_t vid, byte* data )
{
	PreCond( data!=NULL );

	data[0] = id_characterData;
	data[1] = myId;
	data[2] = vid;
	data[3] = strength;
	data[4] = aiId;
	data[5] = Sequence();
	data[6] = selectedWeapon;

	float* f = (float*)&data[7];
	f[0] = x;
	f[1] = y;
	f[2] = z;
	f[3] = yangle;
	f[4] = GunXoff();

	return netSize();
};

size_t TCharacter::netSize( void ) const
{
	return (7 + 5*sizeof(float));
};

size_t TCharacter::netGet( const byte* data )
{
	PreCond( data!=NULL );
	PreCond( data[0]==id_characterData );

	if ( strength!=data[3] )
	{
		strengthUpdated = true;
	}

	strength = data[3];
	aiId = data[4];

	// only set if changed
	if ( Sequence()!=data[5] )
		Sequence( data[5] );

	selectedWeapon = data[6];

	float* f = (float*)&data[7];
	x = f[0];
	y = f[1];
	z = f[2];
	yangle = f[3];
	gunXoff = f[4];

	return netSize();
}

bool TCharacter::LoadShadow( const TString& fname,
							 TString& errStr )
{
	ClearShadow();

	TPersist file(fileRead);
	if ( !file.FileOpen( fname ) )
	{
		errStr = file.ErrorString();
		return false;
	}
	else
	{
		TString sig = "PDV01SHD";
		TString signature;

		file.ReadString( signature );

		if ( signature!=sig )
		{
			errStr = "Shadow file \"" + fname + "\" incorrect version (";
			errStr = errStr + signature + ")";
			file.FileClose();
			return false;
		}

		file.FileRead( &s_angle, sizeof(float) );
		file.FileRead( &s_scale, sizeof(float) );
		s_scale = 0.25f;

		file.FileRead( &s_numVertices, sizeof(size_t) );
		s_vertices = new float[s_numVertices*2];
		file.FileRead( s_vertices, sizeof(float) * s_numVertices * 2 );

		file.FileRead( &s_numSurfaces, sizeof(size_t) );
		s_surfaces = new size_t[s_numSurfaces*3];
		file.FileRead( s_surfaces, sizeof(size_t) * s_numSurfaces * 3 );

		file.FileClose();
	}
	return true;
};

void TCharacter::DrawShadow( size_t shadowLevel )
{
	if ( shadowLevel==0 || y < waterLevel )
	{
		return;
	};

	float height = 0.01f;

	TLandscape* landObj = app->LandObj();

	glPushMatrix();

		glEnable(GL_BLEND);
		glEnable(GL_COLOR_MATERIAL);
		glColor4ub( 0,0,0,150 );

		float m[16];
		glPushMatrix();
			glLoadIdentity();
			glTranslatef( x, 0, z );
			glRotatef( yangle + s_angle, 0,1,0 );
			glGetFloatv( GL_MODELVIEW_MATRIX, m );
		glPopMatrix();

		glBegin( GL_TRIANGLES );

		for ( size_t i=0; i<s_numSurfaces; i++ )
		{
			size_t s[3];
			s[0] = s_surfaces[i*3+0];
			s[1] = s_surfaces[i*3+1];
			s[2] = s_surfaces[i*3+2];

			float v1[3][3];
			float vt1[3][3];

			for ( size_t j=0; j<3; j++ )
			{

				vt1[j][0] = s_vertices[s[j]*2+0] * scale * s_scale;
				vt1[j][1] = Y() + 1;
				vt1[j][2] = s_vertices[s[j]*2+1] * scale * s_scale;

				MatrixMult( m, vt1[j], v1[j] );

				if ( shadowLevel==1 )
				{
					landObj->GetY( v1[j][0], v1[j][1], v1[j][2] );
				}
				else
				{
					landObj->GetBestY( v1[j][0], v1[j][1], v1[j][2] );
				}
				v1[j][1] += height;
			}

			glVertex3fv( v1[0] );
			glVertex3fv( v1[1] );
			glVertex3fv( v1[2] );

			glVertex3fv( v1[2] );
			glVertex3fv( v1[1] );
			glVertex3fv( v1[0] );
		}

		glEnd();

		glDisable(GL_BLEND);

	glPopMatrix();
};

//==========================================================================

