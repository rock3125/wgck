#include <precomp_header.h>

#include <win32/win32.h>

#include <tank/tankapp.h>
#include <object/geometry.h>
#include <ai/artilleryai.h>

#include <botCompiler/botgrammar.h>

#if defined(_EDITOR) || defined(_PARSER) || defined(_VIEWER)
#define _DONTUSE
#endif

//==========================================================================

const float Deg2Rad = 0.017453293f;
const float Rad2Deg = 57.29577951f;

//==========================================================================

AIArtillery::AIArtillery( void )
	: artillery(NULL)
{
	myId = (aiId++ % kAIFrequency) + 1;

	sendOverNetwork = true;

	keys = 0;
	currentNode = -1;
	numNodes = 0;
	movetoX = movetoY = movetoZ = 0;
	initX = initY = initZ = 0;
	finishedMove = true;
	currentState = none;
	aiType = guardBot;
	aiTeam = 0;
	aiCounter = 0;
	fireNow = false;
};


AIArtillery::~AIArtillery( void )
{
	artillery = NULL;
};


AIArtillery::AIArtillery( const AIArtillery& a )
{
	operator=(a);
};

const AIArtillery& AIArtillery::operator=( const AIArtillery& a )
{
	artillery = a.artillery;
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
	fireNow = a.fireNow;

	return *this;
};

bool AIArtillery::SendOverNetwork( void ) const
{
	return sendOverNetwork;
};

void AIArtillery::SendOverNetwork( bool _sendOverNetwork )
{
	sendOverNetwork = _sendOverNetwork;
};

const AIArtillery& AIArtillery::operator=( const TLandscapeObject& obj )
{
	sendOverNetwork = true; // reset

	aiType = obj.AIType();
	aiTeam = obj.AITeam();

	return *this;
};

void AIArtillery::SetVehicle( TArtillery& _artillery )
{
	artillery = &_artillery;
	artillery->IsaAI( true );
};

TArtillery& AIArtillery::Artillery( void ) const
{
	PreCond( artillery!=NULL );
	return *artillery;
};

bool AIArtillery::HitFriendly( TApp* app, TLandscape& landObj, TParams& params )
{
#if !defined(_DONTUSE)
	size_t i;

	TPoint start, dirn;
	size_t gunRange;
	artillery->GetGunDetails( start, dirn, gunRange );

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
			TPlane& plane = params.planes[i];
			if ( plane.IsaAI() && plane.Strength()>0 )
			{
				if ( aip[i].AITeam()==aiTeam )
				{
					if ( plane.InsideVehicle(p1) )
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
				if ( aia[i].AITeam()==aiTeam && &artillery2!=artillery )
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


void AIArtillery::Logic( TApp* app, TLandscape& landObj, TLandscapeAI& land, 
						 TParams& params )
{
	PreCond( artillery!=NULL );

	if ( artillery->Strength()>0 )
	{
		aiCounter++;

		switch ( aiType )
		{
		case artilleryBot:
			{
				if ( (logicCounter%myId)==0 )
					ArtilleryBot(app,landObj,land,params);
				break;
			}
		}

		artillery->Move(keys,false,0,0);
		if ( artillery->LegalVehicleMove( &landObj, params ) )
			artillery->CommitMove();
		else
			artillery->AbortMove();
	};
};


void AIArtillery::Draw( bool transp )
{
	PreCond( artillery!=NULL );
	if ( artillery->Strength()>0 )
	{
		artillery->Draw(transp);
	}
};


bool AIArtillery::TurnVehicle( float px, float py, float pz )
{
	float yangle = -artillery->Yangle() * Deg2Rad;
	float v1fcos = -(float)cos(yangle);
	float v1fsin = (float)sin(yangle);

	float tx = artillery->x - v1fsin;
	float tz = artillery->z - v1fcos;


	yangle = (-artillery->Yangle()-10) * Deg2Rad;
	v1fcos = -(float)cos(yangle);
	v1fsin = (float)sin(yangle);
	float tx1 = artillery->x - v1fsin;
	float tz1 = artillery->z - v1fcos;

	yangle = (-artillery->Yangle()+10) * Deg2Rad;
	v1fcos = -(float)cos(yangle);
	v1fsin = (float)sin(yangle);
	float tx2 = artillery->x - v1fsin;
	float tz2 = artillery->z - v1fcos;

	float d1 = (tx-px)*(tx-px) + (tz-pz)*(tz-pz);
	float d2 = (tx1-px)*(tx1-px) + (tz1-pz)*(tz1-pz);
	float d3 = (tx2-px)*(tx2-px) + (tz2-pz)*(tz2-pz);

	bool ready = true;

	keys = (keys & ~15);
	if ( d3 < d1 )
	{
		keys = keys | aRight;
		ready = false;
	}
	else if ( d2 < d1 )
	{
		keys = keys | aLeft;
		ready = false;
	}

	// aim the barrel
	float dist = float(sqrt(d1));
	float dh = artillery->y - py;
	float barrelAngle;
	if ( dh==0 || dist==0 )
	{
		barrelAngle = 0;
	}
	else
	{
		barrelAngle = float(atan(dh/dist)) * Rad2Deg;
	}
	artillery->BarrelAngle( barrelAngle );

	return ready;
};


float AIArtillery::Distance( float px, float py, float pz )
{
	float dist = (artillery->x-px)*(artillery->x-px) + 
				 (artillery->y-py)*(artillery->y-py) +
				 (artillery->z-pz)*(artillery->z-pz);
	return dist;
};


float AIArtillery::Random( float r )
{
	float _r = float(rand()) / float(RAND_MAX);
	return _r*r;
};


void AIArtillery::FireAt( float x, float y, float z )
{
	{
		fireNow = true;
	}
};


void AIArtillery::ResetMoves( void )
{
	finishedMove = true;
};

bool AIArtillery::MoveTo( TLandscapeAI& land, float x, float y, float z )
{
	if ( fireNow )
	{
		keys |= aFire;
		fireNow = false;
	}
	else
	{
		keys &= (~aFire);
	}

	if ( land.NumLocations()==0 ) // finished if nothing to do
	{
		currentNode = -1;
		movetoX = x;
		movetoY = y;
		movetoZ = z;
		if ( TurnVehicle( movetoX, movetoY, movetoZ ) )
		{
			if ( fabs(artillery->x-movetoX) >= 4 &&
				 fabs(artillery->z-movetoZ) >= 4 )
			{
				keys = keys | aUp;
			}
			else
			{
				keys = (keys & ~15);
				return true;
			}
		}
		return false;
	}

	if ( finishedMove )
	{
		finishedMove = false;
		numNodes = 0;
		if ( !land.EvaluatePath( artillery->x,artillery->y,artillery->z, 
								 x,y,z, numNodes, path ) )
		{
			currentNode = -1;
			movetoX = x;
			movetoY = y;
			movetoZ = z;
		}
		else
		{
			currentNode = 1;
			movetoX = x;
			movetoY = y;
			movetoZ = z;
		}
	}
	else
	{
		if ( currentNode==-1 )
		{
			if ( TurnVehicle( movetoX, movetoY, movetoZ ) )
			{
				if ( fabs(artillery->x-movetoX) >= 4 &&
					 fabs(artillery->z-movetoZ) >= 4 )
				{
					keys = keys | aUp;
				}
			}
			return false;
		}
		else if ( currentNode>int(numNodes) )
		{
			keys = (keys & ~15);
			return true;
		}

		TConnections conn = land.GetNode( path[currentNode] );
		keys = (keys & ~15);
		if ( currentNode!=int(numNodes) )
		{
			if ( TurnVehicle( conn.x, conn.y, conn.z ) )
			{
				keys = keys | aUp;
			}
			if ( fabs(artillery->x-conn.x) < 2 &&
				 fabs(artillery->z-conn.z) < 2 )
			{
				currentNode++;
			}
		}
		if ( currentNode>=int(numNodes) )
		{
			currentNode = numNodes;
			if ( TurnVehicle( movetoX, movetoY, movetoZ ) )
			{
				if ( fabs(artillery->x-movetoX) >= 2 &&
					 fabs(artillery->z-movetoZ) >= 2 )
				{
					keys = keys | aUp;
				}
			}
		}
	}
	return false;
};

float AIArtillery::X( void ) const
{
	PreCond( artillery!=NULL );
	return artillery->X();
};

float AIArtillery::Y( void ) const
{
	PreCond( artillery!=NULL );
	return artillery->Y();
};

float AIArtillery::Z( void ) const
{
	PreCond( artillery!=NULL );
	return artillery->Z();
};

size_t AIArtillery::AIType( void ) const
{
	return aiType;
};

void AIArtillery::AIType( size_t _aiType )
{
	aiType = _aiType;
};

size_t AIArtillery::AITeam( void ) const
{
	return aiTeam;
};

void AIArtillery::AITeam( size_t _aiTeam )
{
	aiTeam = _aiTeam;
};

bool AIArtillery::InsideVehicle( const TPoint& point ) const
{
	PreCond( artillery!=NULL );
	return artillery->InsideVehicle(point);
};

//==========================================================================

void AIArtillery::ArtilleryBot( TApp* app, TLandscape& landObj, TLandscapeAI& land,
								TParams& params )
{
	float aiDist = 6000;

#if !defined(_DONTUSE)
	switch ( currentState )
	{
	case s_none:
		{
			initX = artillery->X();
			initY = artillery->Y();
			initZ = artillery->Z();
			currentState = s_guard;
			break;
		};
	case s_guard:
		{
			MoveTo( land, initX, initY, initZ );

			// see if any enemy has come too close, and switch to attack if so
			AITank* ait = app->AITanks();
			size_t i;
			bool switchModes = false;
			for ( i=0; i<params.numTanks && !switchModes; i++ )
			{
				TTank& tank2 = params.tanks[i];
				if ( tank2.IsaAI() && tank2.Strength()>30 )
				{
					size_t aiId = tank2.AIID();
					if ( ait[aiId].AITeam()!=aiTeam )
					{
						float dist = Distance( tank2.X(), tank2.Y(), tank2.Z() );
						if ( dist < aiDist )
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
				if ( aiTeam!=dp.Team() && !switchModes ) // not friendly to single player
				{
					float dist = Distance( dp.X(), dp.Y(), dp.Z() );
					if ( dist < aiDist )
					{
						switchModes = true;
					}
				}
			}

			if ( switchModes && Distance( initX, initY, initZ ) < aiDist)
			{
				currentState = s_attack;
			}
			break;
		};
	case s_attack:
		{
			if ( Distance( initX, initY, initZ ) > aiDist )
			{
				currentState = s_guard;
				ResetMoves();
				MoveTo( land, initX, initY, initZ );
			}
			else
			{
				// pick closest enemy AI artillery
				AITank* ait = app->AITanks();
				float minDist = aiDist;
				size_t targetIndex = params.numTanks+1;
				size_t i;
				for ( i=0; i<params.numTanks; i++ )
				{
					TTank& tank2 = params.tanks[i];
					if ( tank2.IsaAI() && tank2.Strength()>30 )
					{
						size_t aiId = tank2.AIID();
						if ( ait[aiId].AITeam()!=aiTeam )
						{
							float dist = Distance( tank2.X(), tank2.Y(), tank2.Z() );
							if ( dist < minDist )
							{
								minDist = dist;
								targetIndex = i+1;
							}
						}
					}
				}

				float px,py,pz;
				for ( i=0; i<params.numPlayers; i++ )
				if ( params.inUse[i] )
				{
					TCharacter& dp = params.players[i];
					if ( aiTeam!=dp.Team() ) // not friendly to single player
					{
						float dist = Distance( dp.X(), dp.Y(), dp.Z() );
						if ( dist < minDist )
						{
							px = dp.X();
							py = dp.Y();
							pz = dp.Z();
							minDist = dist;
							targetIndex = 0;
						}
					}
				}

				if ( targetIndex==0 )
				{
					TurnVehicle( px,py,pz );

					// every 50 moves - re-evaluate direction
					if ( (aiCounter%((myId+1)*5))==0 )
					{
						ResetMoves();
					}
					MoveTo( land, px,py,pz );

					if ( Distance( px,py,pz ) < aiDist )
					{
						if ( (aiCounter%(myId*2+1))==0 )
						if ( !HitFriendly(app,landObj,params) )
						{
							FireAt( px,py,pz );
						}
					}
				}
				else if ( targetIndex<=params.numTanks )
				{
					TTank& tank2 = params.tanks[targetIndex-1];

					TurnVehicle( tank2.X(), tank2.Y(), tank2.Z() );

					if ( (aiCounter%((myId+1)*5))==0 )
					{
						ResetMoves();
					}
					MoveTo( land, tank2.X(), tank2.Y(), tank2.Z() );

					if ( Distance( tank2.X(), tank2.Y(), tank2.Z() ) < aiDist )
					{
						if ( (aiCounter%(myId*2+1))==0 )
						if ( !HitFriendly(app,landObj,params) )
						{
							FireAt( tank2.X(), tank2.Y(), tank2.Z() );
						}
					}
				} 
				else
				{
					currentState = s_guard;
					ResetMoves();
					MoveTo( land, initX, initY, initZ );
				}
			}
			break;
		};
	}
#endif
}

