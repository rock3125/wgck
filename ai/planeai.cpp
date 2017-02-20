#include <precomp_header.h>

#include <win32/win32.h>

#include <tank/tankapp.h>
#include <object/geometry.h>
#include <ai/planeai.h>

#include <botCompiler/botgrammar.h>

#if defined(_EDITOR) || defined(_PARSER) || defined(_VIEWER)
#define _DONTUSE
#endif

//==========================================================================

const float Deg2Rad = 0.017453293f;
const float Rad2Deg = 57.29577951f;

//==========================================================================

AIPlane::AIPlane( void )
	: plane(NULL)
{
	myId = (aiId++ % kAIFrequency) + 1;

	keys = 0;
	currentNode = -1;
	numNodes = 0;
	movetoX = movetoY = movetoZ = 0;
	initX = initY = initZ = 0;
	finishedMove = true;
	sendOverNetwork = true;
	currentState = none;
	aiType = fighterBot;
	aiTeam = 0;
	aiCounter = 0;
};


AIPlane::~AIPlane( void )
{
	plane = NULL;
};


AIPlane::AIPlane( const AIPlane& a )
{
	operator=(a);
};

const AIPlane& AIPlane::operator=( const AIPlane& a )
{
	plane = a.plane;
	keys = a.keys;

	for ( size_t i=0; i<1000; i++ )
	{
		path[i] = a.path[i];
	}
	numNodes = a.numNodes;
	currentNode = a.currentNode;
	movetoX = a.movetoX;
	movetoY = a.movetoY;
	movetoZ = a.movetoZ;
	finishedMove = a.finishedMove;
	currentState = a.currentState;
	sendOverNetwork = a.sendOverNetwork;

	myId = a.myId;

	initX = a.initX;
	initY = a.initY;
	initZ = a.initZ;

	aiType = a.aiType;
	aiTeam = a.aiTeam;
	aiCounter = a.aiCounter;

	return *this;
};

const AIPlane& AIPlane::operator=( const TLandscapeObject& obj )
{
	aiType = obj.AIType();
	aiTeam = obj.AITeam();
	sendOverNetwork = true;

	return *this;
};

bool AIPlane::SendOverNetwork( void ) const
{
	return sendOverNetwork;
};

void AIPlane::SendOverNetwork( bool _sendOverNetwork )
{
	sendOverNetwork = _sendOverNetwork;
};

// convert crazy plane zangle to proper
// plane angle (between -90 -> 90 )
float AIPlane::PlaneAngle( void ) const
{
	float zangle = plane->Zangle();
	if ( zangle>=0 && zangle<90 )
	{
		return -zangle;
	}
	else if ( zangle>=90 && zangle<180 )
	{
		return zangle-180;
	}
	else if ( zangle>270 )
	{
		return 360-zangle;
	}
	else
	{
		return zangle-180;
	}
};

void AIPlane::SetVehicle( TPlane& _plane )
{
	plane = &_plane;
	plane->IsaAI( true );
};

TPlane& AIPlane::Plane( void ) const
{
	PreCond( plane!=NULL );
	return *plane;
};

bool AIPlane::HitFriendly( TApp* app, TLandscape& landObj, TParams& params )
{
#if !defined(_DONTUSE)
	size_t i;
	AIPlane* ais = app->AIPlanes();

	TPoint start, dirn;
	size_t gunRange;
	plane->GetGunDetails( start, dirn, gunRange );

	TPoint p1 = start;
	for ( size_t j=0; j<gunRange; j++ )
	{
		AITank* ait = app->AITanks();
		for ( i=0; i<params.numTanks; i++ )
		{
			TTank& tank2 = params.tanks[i];
			if ( tank2.IsaAI() && tank2.Strength()>30 )
			{
				if ( ait[i].AITeam()==aiTeam )
				{
					if ( tank2.InsideVehicle(p1) )
						return true;
				}
			}
		}

		AIPlane* aip = app->AIPlanes();
		for ( i=0; i<params.numPlanes; i++ )
		{
			TPlane& plane2 = params.planes[i];
			if ( plane2.IsaAI() && plane2.Strength()>0 )
			{
				if ( aip[i].AITeam()==aiTeam && &plane2!=plane )
				{
					if ( plane2.InsideVehicle(p1) )
						return true;
				}
			}
		}

		AIFlak* aif = app->AIFlaks();
		for ( i=0; i<params.numFlak; i++ )
		{
			TFlak& flak2 = params.flak[i];
			if ( flak2.IsaAI() && flak2.Strength()>0 )
			{
				if ( aif[i].AITeam()==aiTeam )
				{
					if ( flak2.InsideVehicle(p1) )
						return true;
				}
			}
		}

		AIArtillery* aia = app->AIArtilleries();
		for ( i=0; i<params.numArtillery; i++ )
		{
			TArtillery& artillery2 = params.artillery[i];
			if ( artillery2.IsaAI() && artillery2.Strength()>0 )
			{
				if ( aia[i].AITeam()==aiTeam )
				{
					if ( artillery2.InsideVehicle(p1) )
						return true;
				}
			}
		}
		// can we hit ourselves like this? 
		// (did we fire into the ground in front of ourselves?)
		if ( j<5 )
		{
			float y;
			landObj.GetY( p1.x,y,p1.z);
			if ( y >= p1.y )
				return true;
		}
		p1 = p1 + dirn;
	}

#endif
	return false;
};


void AIPlane::Logic( TApp* app, TLandscape& landObj, TLandscapeAI& land, 
					 TParams& params )
{
	PreCond( plane!=NULL );

	if ( plane->Strength()>0 )
	{
		aiCounter++;

		switch ( aiType )
		{
		case fighterBot:
			{
				if ( (logicCounter%myId)==0 )
					FighterBot(app,landObj,land,params);
				break;
			}
		case bomberBot:
			{
				if ( (logicCounter%myId)==0 )
					BomberBot(app,landObj,land,params);
				break;
			}
		}

		plane->Move(keys,false,0,0,NULL);
		// numPlanes=0 => don't bother with collisions for AIs
		if ( plane->LegalVehicleMove( &landObj, params ) )
			plane->CommitMove();
		else
			plane->AbortMove();
	};
};


void AIPlane::Draw( bool transp )
{
	PreCond( plane!=NULL );
	if ( plane->Strength()>0 )
	{
		plane->Draw(transp);
	}
};


bool AIPlane::TurnVehicle( float px, float pz )
{
	float yangle = plane->Yangle() * Deg2Rad;
	float v1fcos = -(float)cos(yangle);
	float v1fsin = (float)sin(yangle);

	float tx = plane->X() - v1fsin;
	float tz = plane->Z() - v1fcos;

	yangle = (plane->Yangle()-10) * Deg2Rad;
	v1fcos = -(float)cos(yangle);
	v1fsin = (float)sin(yangle);
	float tx1 = plane->X() - v1fsin;
	float tz1 = plane->Z() - v1fcos;

	yangle = (plane->Yangle()+10) * Deg2Rad;
	v1fcos = -(float)cos(yangle);
	v1fsin = (float)sin(yangle);
	float tx2 = plane->X() - v1fsin;
	float tz2 = plane->Z() - v1fcos;

	float d1 = (tx-px)*(tx-px) + (tz-pz)*(tz-pz);
	float d2 = (tx1-px)*(tx1-px) + (tz1-pz)*(tz1-pz);
	float d3 = (tx2-px)*(tx2-px) + (tz2-pz)*(tz2-pz);

	bool ready = true;

	if ( d2 < d1 )
	{
		keys = keys | aLeft;
		ready = false;
	}
	else if ( d3 < d1 )
	{
		keys = keys | aRight;
		ready = false;
	}
	return ready;
};


float AIPlane::Distance( float px, float py, float pz )
{
	float dist = (plane->X()-px)*(plane->X()-px) + 
				 (plane->Y()-py)*(plane->Y()-py) +
				 (plane->Z()-pz)*(plane->Z()-pz);
	return dist;
};


float AIPlane::Random( float r )
{
	float _r = float(rand()) / float(RAND_MAX);
	return _r*r;
};


void AIPlane::ResetMoves( void )
{
	finishedMove = true;
};

bool AIPlane::TakeOff( TLandscapeAI& land )
{
	if ( !plane->flying )
	{
		keys = (keys&~15);
		keys |= aDown;
	}
	else
	{
		return true;
	}
	return false;
};

bool AIPlane::Climb( TLandscapeAI& land )
{
	keys = (keys&~15);

	// a. climb up 45 degree angle
	// 360..315 is 0 to 45 angle - 270 is 90 up
	float angle = PlaneAngle();
	if (  angle < 45 )
	{
		if ( plane->UpsideDown() )
		{
			keys |= aUp;
		}
		else
		{
			keys |= aDown;
		}
		return false;
	}
	// b. reach ceiling (wait till the plane levels)
	if ( angle < 15 )
	{
		return false;
	}
	return true;
};

bool AIPlane::BomberClimb( TLandscapeAI& land )
{
	keys = (keys&~15);

	// a. climb up 45 degree angle
	// 360..315 is 0 to 45 angle - 270 is 90 up
	if ( plane->Zangle() > 315 )
	{
		keys |= aDown;
		return false;
	}
	// b. reach ceiling (wait till the plane levels)
	return ( plane->Y() > (plane->WeaponsRange()-10) );
};

// level the plane
bool AIPlane::Level( void )
{
	if ( plane->Zangle() < 350 )
	{
		keys |= aUp;
		return false;
	}
	return true;
};

float AIPlane::X( void ) const
{
	PreCond( plane!=NULL );
	return plane->X();
};

float AIPlane::Y( void ) const
{
	PreCond( plane!=NULL );
	return plane->Y();
};

float AIPlane::Z( void ) const
{
	PreCond( plane!=NULL );
	return plane->Z();
};

size_t AIPlane::AIType( void ) const
{
	return aiType;
};

void AIPlane::AIType( size_t _aiType )
{
	aiType = _aiType;
};

size_t AIPlane::AITeam( void ) const
{
	return aiTeam;
};

void AIPlane::AITeam( size_t _aiTeam )
{
	aiTeam = _aiTeam;
};

bool AIPlane::InsideVehicle( const TPoint& point ) const
{
	PreCond( plane!=NULL );
	return plane->InsideVehicle(point);
};

void AIPlane::SelectGun( void )
{
	if ( plane->numMissiles>0 )
	{
		plane->gunId = 2;
	}
	else
	{
		plane->gunId = 0;
	}
};

void AIPlane::SelectBombs( void )
{
	plane->gunId = 1;
};


bool AIPlane::MoveTo( TLandscape& land, float closest, float px, float py, float pz )
{
	float yangle = -plane->Yangle() * Deg2Rad;
	float v1fcos = -(float)cos(yangle);
	float v1fsin = (float)sin(yangle);

	float tx = plane->X() - v1fsin;
	float tz = plane->Z() - v1fcos;

	yangle = (-plane->Yangle()-10) * Deg2Rad;
	v1fcos = -(float)cos(yangle);
	v1fsin = (float)sin(yangle);
	float tx1 = plane->X() - v1fsin;
	float tz1 = plane->Z() - v1fcos;

	yangle = (-plane->Yangle()+10) * Deg2Rad;
	v1fcos = -(float)cos(yangle);
	v1fsin = (float)sin(yangle);
	float tx2 = plane->X() - v1fsin;
	float tz2 = plane->Z() - v1fcos;

	float d1 = (tx-px)*(tx-px) + (tz-pz)*(tz-pz);
	float d2 = (tx1-px)*(tx1-px) + (tz1-pz)*(tz1-pz);
	float d3 = (tx2-px)*(tx2-px) + (tz2-pz)*(tz2-pz);

	bool ready = true;

	keys = 0;

	// too close? turn the other way
	// and pull up go into a loop
	bool fire = true;
	if ( closest < 250 )
	{
		fire = false;
		// veer to one side
		keys = keys | aRight;
	}
	else
	{
		if ( d2 < d1 )
		{
			keys = keys | aLeft;
		}
		else if ( d3 < d1 )
		{
			keys = keys | aRight;
		}
	}

	// reverse directions if we're up-side-down
	if ( plane->UpsideDown() )
	{
		fire = false;
		keys = aDown;
		if ( d2 < d1 )
		{
			keys = keys | aLeft;
		}
		else if ( d3 < d1 )
		{
			keys = keys | aRight;
		}
		return false;
	}

	// go up/down - go onto same height
	// but prevent a crash
	float landY;
	land.GetY( plane->X(), landY, plane->Z() );

	float pangle = PlaneAngle();
	float dh = plane->Y() - py;
	if ( dh < -0.25f && pangle < 60 )
	{
		keys = keys | aDown;
	}
	else if ( dh > 0.25f && ( plane->Y() > (landY+5) ) )
	{
		keys = keys | aUp;
	}

	// aim the barrel
	float dist = float(sqrt(d1));
	float barrelAngle;
	if ( dh==0 || dist==0 )
	{
		barrelAngle = 0;
	}
	else
	{
		barrelAngle = -float(atan(dh/dist)) * Rad2Deg;
	}
	plane->BarrelAngle( barrelAngle );

	if ( fire )
	if ( dist < 35 )
	if ( Random(100) > 75 )
	{
		keys = keys | aFire;
	}

	return ready;
};

bool AIPlane::LandAt( TLandscape& land, float px, float py, float pz )
{
	float yangle = -plane->Yangle() * Deg2Rad;
	float v1fcos = -(float)cos(yangle);
	float v1fsin = (float)sin(yangle);

	float tx = plane->X() - v1fsin;
	float tz = plane->Z() - v1fcos;

	yangle = (-plane->Yangle()-10) * Deg2Rad;
	v1fcos = -(float)cos(yangle);
	v1fsin = (float)sin(yangle);
	float tx1 = plane->X() - v1fsin;
	float tz1 = plane->Z() - v1fcos;

	yangle = (-plane->Yangle()+10) * Deg2Rad;
	v1fcos = -(float)cos(yangle);
	v1fsin = (float)sin(yangle);
	float tx2 = plane->X() - v1fsin;
	float tz2 = plane->Z() - v1fcos;

	float d1 = (tx-px)*(tx-px) + (tz-pz)*(tz-pz);
	float d2 = (tx1-px)*(tx1-px) + (tz1-pz)*(tz1-pz);
	float d3 = (tx2-px)*(tx2-px) + (tz2-pz)*(tz2-pz);

	bool ready = true;

	keys = 0;

	if ( d2 < d1 )
	{
		keys = keys | aLeft;
	}
	else if ( d3 < d1 )
	{
		keys = keys | aRight;
	}

	if ( plane->Zangle() < 350 )
	{
		keys |= aUp;
	}

	return !plane->flying;
};

bool AIPlane::StopEngine( void )
{
	keys = aUp;
	if ( plane->Speed() == 0 )
	{
		keys = 0;
		return true;
	}
	return false;
};

void AIPlane::MoveBomberTo( TLandscape& land, float px, float py, float pz )
{
	float yangle = -plane->Yangle() * Deg2Rad;
	float v1fcos = -(float)cos(yangle);
	float v1fsin = (float)sin(yangle);

	float tx = plane->X() - v1fsin;
	float tz = plane->Z() - v1fcos;

	yangle = (-plane->Yangle()-10) * Deg2Rad;
	v1fcos = -(float)cos(yangle);
	v1fsin = (float)sin(yangle);
	float tx1 = plane->X() - v1fsin;
	float tz1 = plane->Z() - v1fcos;

	yangle = (-plane->Yangle()+10) * Deg2Rad;
	v1fcos = -(float)cos(yangle);
	v1fsin = (float)sin(yangle);
	float tx2 = plane->X() - v1fsin;
	float tz2 = plane->Z() - v1fcos;

	float d1 = (tx-px)*(tx-px) + (tz-pz)*(tz-pz);
	float d2 = (tx1-px)*(tx1-px) + (tz1-pz)*(tz1-pz);
	float d3 = (tx2-px)*(tx2-px) + (tz2-pz)*(tz2-pz);

	keys = 0;

	if ( d2 < d1 )
	{
		keys = keys | aLeft;
	}
	else if ( d3 < d1 )
	{
		keys = keys | aRight;
	}

	plane->BarrelAngle( 45 ); // +45 makes 90 degrees - straight down
	if ( d1 < 25 )
	{
		keys = keys | aFire;
	}
};

//==========================================================================

void AIPlane::FighterBot( TApp* app, TLandscape& landObj, TLandscapeAI& land,
						  TParams& params )
{
	float takeOffDistance = 8000;
	float aiDist = 4000;

#if !defined(_DONTUSE)
	switch ( currentState )
	{
	case s_none:
		{
			initX = plane->X();
			initY = plane->Y();
			initZ = plane->Z();
			currentState = s_waitfortakeoff;
			break;
		};
	case s_waitfortakeoff:
		{
			SelectGun();

			// see if any enemy has come too close, and switch to attack if so
			AIPlane* ais = app->AIPlanes();
			size_t i;
			bool switchModes = false;
			for ( i=0; i<params.numPlanes && !switchModes; i++ )
			{
				TPlane& plane = params.planes[i];
				if ( plane.IsaAI() && plane.Strength()>0 )
				{
					size_t aiId = plane.AIID();
					if ( ais[aiId].AITeam()!=aiTeam )
					{
						float dist = Distance( plane.x, plane.Y(), plane.z );
						if ( dist < takeOffDistance )
						{
							switchModes = true;
						}
					}
				}
			}

			for ( i=0; i<params.numPlayers && !switchModes; i++ )
			if ( params.inUse[i] )
			{
				TCharacter& dp = params.players[i];
				if ( aiTeam!=dp.Team() ) // not friendly to single player
				{
					float dist = Distance( dp.X(), dp.Y(), dp.Z() );
					if ( dist < aiDist )
					{
						switchModes = true;
					}
				}
			}

			if ( switchModes && plane->Strength()>10 &&
				 plane->Fuel() > 10 && plane->Ammo() > 0 )
			{
				currentState = s_takeoff;
			}
			break;
		};
	case s_takeoff:
		{
			if ( TakeOff(land) )
			{
				currentState = s_climb;
			}
			break;
		};
	case s_climb:
		{
			if ( Climb(land) )
			{
				currentState = s_findenemy;
			}
			break;
		}
	case s_findenemy:
		{
			SelectGun();
			
			// see if any enemy has come too close, and switch to attack if so
			AIPlane* ais = app->AIPlanes();
			size_t i;
			float targetX, targetY, targetZ;
			bool found = false;
			float closest = aiDist * 10;
			for ( i=0; i<params.numPlanes; i++ )
			{
				TPlane& plane = params.planes[i];
				if ( plane.IsaAI() && plane.Strength()>0 )
				{
					size_t aiId = plane.AIID();
					if ( ais[aiId].AITeam()!=aiTeam )
					{
						float dist = Distance( plane.x, plane.Y(), plane.z );
						if ( dist < closest )
						{
							found = true;
							closest = dist;
							targetX = plane.x;
							targetY = plane.Y();
							targetZ = plane.z;
						}
					}
				}
			}

			for ( i=0; i<params.numPlayers; i++ )
			if ( params.inUse[i] )
			{
				TCharacter& dp = params.players[i];
				if ( aiTeam!=dp.Team() ) // not friendly to single player
				{
					float dist = Distance( dp.X(), dp.Y(), dp.Z() );
					if ( dist < closest )
					{
						targetX = dp.X();
						targetY = dp.Y();
						targetZ = dp.Z();
						closest = dist;
						found = true;
					}
				}
			}

			if ( found )
			{
				MoveTo( landObj, closest, targetX, targetY, targetZ );
			}
			else if ( plane->Fuel() <= 10 || plane->Ammo() == 0 || !found ||
					  plane->Strength() <= 10 )
			{
				MoveTo( landObj, aiDist, initX, initY, initZ );

				// go to land mode
				if ( Distance( initX, initY, initZ ) < 100 )
				{
					currentState = s_land;
				}
			}
			break;
		}

	case s_land:
		{
			if ( LandAt( landObj, initX, initY, initZ ) )
			{
				if ( StopEngine() )
				{
					currentState = s_waitfortakeoff;
				}
			}
			break;
		}
	}
#endif
}




void AIPlane::BomberBot( TApp* app, TLandscape& landObj, TLandscapeAI& land,
						 TParams& params )
{
	float takeOffDistance = 8000;
	float aiDist = 4000;

#if !defined(_DONTUSE)
	switch ( currentState )
	{
	case s_none:
		{
			initX = plane->X();
			initY = plane->Y();
			initZ = plane->Z();
			currentState = s_waitfortakeoff;
			break;
		};
	case s_waitfortakeoff:
		{
			SelectBombs();

			// see if any enemy has come too close, and switch to attack if so
			AITank* ait = app->AITanks();
			size_t i;
			bool switchModes = false;
			for ( i=0; i<params.numTanks && !switchModes; i++ )
			{
				TTank& tank = params.tanks[i];
				if ( tank.IsaAI() && tank.Strength()>0 )
				{
					size_t aiId = tank.AIID();
					if ( ait[aiId].AITeam()!=aiTeam )
					{
						float dist = Distance( tank.X(), tank.Y(), tank.Z() );
						if ( dist < takeOffDistance )
						{
							switchModes = true;
						}
					}
				}
			}

			AIFlak* aif = app->AIFlaks();
			for ( i=0; i<params.numFlak && !switchModes; i++ )
			{
				TFlak& flak = params.flak[i];
				if ( flak.IsaAI() && flak.Strength()>0 )
				{
					size_t aiId = flak.AIID();
					if ( aif[aiId].AITeam()!=aiTeam )
					{
						float dist = Distance( flak.X(), flak.Y(), flak.Z() );
						if ( dist < takeOffDistance )
						{
							switchModes = true;
						}
					}
				}
			}

			AIArtillery* aia = app->AIArtilleries();
			for ( i=0; i<params.numArtillery && !switchModes; i++ )
			{
				TArtillery& artillery = params.artillery[i];
				if ( artillery.IsaAI() && artillery.Strength()>0 )
				{
					size_t aiId = artillery.AIID();
					if ( aia[aiId].AITeam()!=aiTeam )
					{
						float dist = Distance( artillery.X(), artillery.Y(), artillery.Z() );
						if ( dist < takeOffDistance )
						{
							switchModes = true;
						}
					}
				}
			}

			for ( i=0; i<params.numPlayers && !switchModes; i++ )
			if ( params.inUse[i] )
			{
				TCharacter& dp = params.players[i];
				if ( aiTeam!=dp.Team() ) // not friendly to single player
				{
					float dist = Distance( dp.X(), dp.Y(), dp.Z() );
					if ( dist < aiDist )
					{
						switchModes = true;
					}
				}
			}

			if ( switchModes && plane->Strength()>10 &&
				 plane->Fuel() > 10 && plane->Ammo() > 0 )
			{
				currentState = s_takeoff;
			}
			break;
		};
	case s_takeoff:
		{
			if ( TakeOff(land) )
			{
				currentState = s_climb;
			}
			break;
		};
	case s_climb:
		{
			if ( BomberClimb(land) )
			{
				currentState = s_findenemy;
			}
			break;
		}
	case s_findenemy:
		{
			SelectBombs();
			
			// see if any enemy has come too close, and switch to attack if so
			AITank* ait = app->AITanks();
			float targetX, targetY, targetZ;
			float closest = aiDist * 10;
			size_t i;
			bool found = false;
			for ( i=0; i<params.numTanks; i++ )
			{
				TTank& tank = params.tanks[i];
				if ( tank.IsaAI() && tank.Strength()>0 )
				{
					size_t aiId = tank.AIID();
					if ( ait[aiId].AITeam()!=aiTeam )
					{
						float dist = Distance( tank.X(), tank.Y(), tank.Z() );
						if ( dist < closest )
						{
							closest = dist;
							found = true;
							targetX = tank.X();
							targetY = tank.Y();
							targetZ = tank.Z();
						}
					}
				}
			}

			AIFlak* aif = app->AIFlaks();
			for ( i=0; i<params.numFlak; i++ )
			{
				TFlak& flak = params.flak[i];
				if ( flak.IsaAI() && flak.Strength()>0 )
				{
					size_t aiId = flak.AIID();
					if ( aif[aiId].AITeam()!=aiTeam )
					{
						float dist = Distance( flak.X(), flak.Y(), flak.Z() );
						if ( dist < closest )
						{
							closest = dist;
							found = true;
							targetX = flak.X();
							targetY = flak.Y();
							targetZ = flak.Z();
						}
					}
				}
			}

			AIArtillery* aia = app->AIArtilleries();
			for ( i=0; i<params.numArtillery; i++ )
			{
				TArtillery& artillery = params.artillery[i];
				if ( artillery.IsaAI() && artillery.Strength()>0 )
				{
					size_t aiId = artillery.AIID();
					if ( aia[aiId].AITeam()!=aiTeam )
					{
						float dist = Distance( artillery.X(), artillery.Y(), artillery.Z() );
						if ( dist < closest )
						{
							closest = dist;
							found = true;
							targetX = artillery.X();
							targetY = artillery.Y();
							targetZ = artillery.Z();
						}
					}
				}
			}

			for ( i=0; i<params.numPlayers; i++ )
			if ( params.inUse[i] )
			{
				TCharacter& dp = params.players[i];
				if ( aiTeam!=dp.Team() ) // not friendly to single player
				{
					float dist = Distance( dp.X(), dp.Y(), dp.Z() );
					if ( dist < closest )
					{
						targetX = dp.X();
						targetY = dp.Y();
						targetZ = dp.Z();
						closest = dist;
						found = true;
					}
				}
			}

			if ( found )
			{
				MoveBomberTo( landObj, targetX, targetY, targetZ );
			}
			else if ( plane->Fuel() <= 10 || plane->Ammo() == 0 || !found ||
					  plane->Strength() <= 10 )
			{
				MoveTo( landObj, aiDist, initX, initY, initZ );

				// go to land mode
				if ( Distance( initX, initY, initZ ) < 100 )
				{
					currentState = s_land;
				}
			}
			break;
		}

	case s_land:
		{
			if ( LandAt( landObj, initX, initY, initZ ) )
			{
				if ( StopEngine() )
				{
					currentState = s_waitfortakeoff;
				}
			}
			break;
		}
	}
#endif
}


