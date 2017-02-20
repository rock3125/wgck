#include <precomp_header.h>

#include <win32/win32.h>

#include <tank/tankapp.h>
#include <object/geometry.h>
#include <ai/tankai.h>

#include <botCompiler/botgrammar.h>

#if defined(_EDITOR) || defined(_PARSER) || defined(_VIEWER)
#define _DONTUSE
#endif

//==========================================================================

const float Deg2Rad = 0.017453293f;
const float Rad2Deg = 57.29577951f;

//==========================================================================

AITank::AITank( void )
	: tank(NULL),
	  botScript(NULL)
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
	aiType = guardBot;
	aiTeam = 0;
	aiCounter = 0;
	fireNow = false;
};


AITank::~AITank( void )
{
	tank = NULL;
	if ( botScript!=NULL )
		delete botScript;
	botScript = NULL;
};


AITank::AITank( const AITank& a )
{
	operator=(a);
};

const AITank& AITank::operator=( const AITank& a )
{
	tank = a.tank;
	keys = a.keys;

	botScript = a.botScript;

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

const AITank& AITank::operator=( const TLandscapeObject& obj )
{
	aiType = obj.AIType();
	aiTeam = obj.AITeam();
	sendOverNetwork = true;

	return *this;
};

bool AITank::SendOverNetwork( void ) const
{
	return sendOverNetwork;
};

void AITank::SendOverNetwork( bool _sendOverNetwork )
{
	sendOverNetwork = _sendOverNetwork;
};

void AITank::SetVehicle( TTank& _tank )
{
	tank = &_tank;
	tank->IsaAI( true );
};


TTank& AITank::Tank( void ) const
{
	PreCond( tank!=NULL );
	return *tank;
};

bool AITank::LoadAICore( const TString& fname, TString& errStr )
{
	if ( botScript!=NULL )
		delete botScript;
	botScript = new BotParser();
	return botScript->LoadBot( fname, errStr );
};


bool AITank::HitFriendly( TApp* app, TLandscape& landObj, TParams& params )
{
#if !defined(_DONTUSE)
	size_t i;
	AITank* ais = app->AITanks();

	TPoint start, dirn;
	size_t gunRange;
	tank->GetGunDetails( start, dirn, gunRange );

	TPoint p1 = start;
	for ( size_t j=0; j<gunRange; j++ )
	{
		AITank* ait = app->AITanks();
		for ( i=0; i<params.numTanks; i++ )
		{
			TTank& tank2 = params.tanks[i];
			if ( tank2.IsaAI() && tank2.Strength()>0 )
			{
				if ( ait[i].AITeam()==aiTeam && &tank2!=tank )
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


void AITank::Logic( TApp* app, TLandscape& landObj, TLandscapeAI& land, 
				    TParams& params )
{
	PreCond( tank!=NULL );

	if ( tank->Strength()>0 )
	{
		aiCounter++;

		switch ( aiType )
		{
		case guardBot:
			{
				if ( (logicCounter%myId)==0 )
					GuardBot(app,landObj,land,params);
				break;
			}
		case attackBot:
			{
				if ( (logicCounter%myId)==0 )
					AttackBot(app,landObj,land,params);
				break;
			}
		case patrolBot:
			{
				if ( (logicCounter%myId)==0 )
					PatrolBot(app,landObj,land,params);
				break;
			}
		}

		tank->Move(landObj,keys,false,0,0,NULL);
		// numTanks=0 => don't bother with collisions for AIs
		if ( tank->LegalVehicleMove( &landObj, params ) )
			tank->CommitMove();
		else
			tank->AbortMove();
	};
};


void AITank::Draw( bool transp )
{
	PreCond( tank!=NULL );
	if ( tank->Strength()>0 )
	{
		tank->Draw(transp);
	}
};


bool AITank::TurnVehicle( float px, float pz )
{
	float yangle = tank->Yangle() * Deg2Rad;

	float v1fcos = (float)cos(-yangle);
	float v1fsin = -(float)sin(-yangle);

	float tx = tank->X() + v1fsin;
	float tz = tank->Z() + v1fcos;

	yangle = (tank->Yangle()-10) * Deg2Rad;
	v1fcos = (float)cos(-yangle);
	v1fsin = -(float)sin(-yangle);
	float tx1 = tank->X() + v1fsin;
	float tz1 = tank->Z() + v1fcos;

	yangle = (tank->Yangle()+10) * Deg2Rad;
	v1fcos = (float)cos(-yangle);
	v1fsin = -(float)sin(-yangle);
	float tx2 = tank->X() + v1fsin;
	float tz2 = tank->Z() + v1fcos;

	float d1 = (tx-px)*(tx-px) + (tz-pz)*(tz-pz);
	float d2 = (tx1-px)*(tx1-px) + (tz1-pz)*(tz1-pz);
	float d3 = (tx2-px)*(tx2-px) + (tz2-pz)*(tz2-pz);

	bool ready = true;

	keys = (keys & ~15);
	if ( d2 < d1 )
	{
		keys = keys | aRight;
		ready = false;
	}
	else if ( d3 < d1 )
	{
		keys = keys | aLeft;
		ready = false;
	}
	return ready;
};


bool AITank::TurnVehicleTurret( float px, float py, float pz )
{
	if ( tank->TurretLess() )
		return true;

	float delta = 1;

	float turretAngle = tank->Yangle() + tank->TurretAngle();
	float yangle = turretAngle * Deg2Rad;
	float v1fcos = (float)cos(-yangle);
	float v1fsin = -(float)sin(-yangle);

	float tx = tank->X() + v1fsin;
	float tz = tank->Z() + v1fcos;

	yangle = (turretAngle-delta) * Deg2Rad;
	v1fcos = (float)cos(-yangle);
	v1fsin = -(float)sin(-yangle);
	float tx1 = tank->X() + v1fsin;
	float tz1 = tank->Z() + v1fcos;

	yangle = (turretAngle+delta) * Deg2Rad;
	v1fcos = (float)cos(-yangle);
	v1fsin = -(float)sin(-yangle);
	float tx2 = tank->X() + v1fsin;
	float tz2 = tank->Z() + v1fcos;

	float d1 = (tx-px)*(tx-px) + (tz-pz)*(tz-pz);
	float d2 = (tx1-px)*(tx1-px) + (tz1-pz)*(tz1-pz);
	float d3 = (tx2-px)*(tx2-px) + (tz2-pz)*(tz2-pz);

	bool ready = true;

	keys = (keys & ~15);
	if ( d2 < d1 )
	{
		tank->TurretAngle( tank->TurretAngle() - delta );
		ready = false;
	}
	else if ( d3 < d1 )
	{
		tank->TurretAngle( tank->TurretAngle() + delta );
		ready = false;
	}

	// aim the barrel
	float dist = float(sqrt(d1));
	float dh = tank->Y() - py;
	float barrelAngle;
	if ( dh==0 || dist==0 )
	{
		barrelAngle = 0;
	}
	else
	{
		barrelAngle = float(atan(dh/dist)) * Rad2Deg;
	}
	tank->BarrelAngle( barrelAngle );

	return ready;
};


float AITank::Distance( float px, float py, float pz )
{
	float dist = (tank->X()-px)*(tank->X()-px) + 
				 (tank->Y()-py)*(tank->Y()-py) +
				 (tank->Z()-pz)*(tank->Z()-pz);
	return dist;
};


float AITank::Random( float r )
{
	float _r = float(rand()) / float(RAND_MAX);
	return _r*r;
};


void AITank::FireAt( float x, float y, float z )
{
//	if ( Random(100) > 80 )
	{
		fireNow = true;
	}
};


void AITank::ResetMoves( void )
{
	finishedMove = true;
};

bool AITank::MoveTo( TLandscapeAI& land, float x, float y, float z )
{
	if ( fireNow )
	{
		keys |= aFire;
		fireNow = false;
	}

	if ( land.NumLocations()==0 ) // finished if nothing to do
	{
		currentNode = -1;
		movetoX = x;
		movetoY = y;
		movetoZ = z;
		if ( TurnVehicle( movetoX, movetoZ ) )
		{
			if ( fabs(tank->X()-movetoX) >= 4 &&
				 fabs(tank->Z()-movetoZ) >= 4 )
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
		if ( !land.EvaluatePath( tank->X(),tank->Y(),tank->Z(), 
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
			if ( TurnVehicle( movetoX, movetoZ ) )
			{
				if ( fabs(tank->X()-movetoX) >= 4 &&
					 fabs(tank->Z()-movetoZ) >= 4 )
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
			if ( TurnVehicle( conn.x, conn.z ) )
			{
				keys = keys | aUp;
			}
			if ( fabs(tank->X()-conn.x) < 2 &&
				 fabs(tank->Z()-conn.z) < 2 )
			{
				currentNode++;
			}
		}
		if ( currentNode>=int(numNodes) )
		{
			currentNode = numNodes;
			if ( TurnVehicle( movetoX, movetoZ ) )
			{
				if ( fabs(tank->X()-movetoX) >= 2 &&
					 fabs(tank->Z()-movetoZ) >= 2 )
				{
					keys = keys | aUp;
				}
			}
		}
	}
	return false;
};

bool AITank::MovePatrol( TLandscapeAI& land )
{
	if ( fireNow )
	{
		keys |= aFire;
		fireNow = false;
	}

	if ( land.NumPatrolLocations()==0 ) // finished if nothing to do
	{
		currentNode = -1;
		return false;
	}

	if ( finishedMove )
	{
		finishedMove = false;
		numNodes = 0;
		if ( !land.EvaluatePatrolPath( tank->X(),tank->Y(),tank->Z(), 
									   numNodes, path ) )
		{
			currentNode = -1;
		}
		else
		{
			currentNode = 1;
		}
	}
	else
	{
		if ( currentNode==-1 )
		{
			return false;
		}
		else if ( currentNode>=int(numNodes) )
		{
			currentNode = 1;
		}

		TConnections conn = land.GetPatrolNode( path[currentNode] );
		keys = (keys & ~15);
		if ( currentNode!=int(numNodes) )
		{
			if ( TurnVehicle( conn.x, conn.z ) )
			{
				keys = keys | aUp;
			}
			if ( fabs(tank->X()-conn.x) < 2 &&
				 fabs(tank->Z()-conn.z) < 2 )
			{
				currentNode++;
			}
		}
		if ( currentNode>=int(numNodes) )
		{
			currentNode = numNodes;
			if ( TurnVehicle( movetoX, movetoZ ) )
			{
				if ( fabs(tank->X()-movetoX) >= 2 &&
					 fabs(tank->Z()-movetoZ) >= 2 )
				{
					keys = keys | aUp;
				}
			}
		}
	}
	return false;
};

float AITank::X( void ) const
{
	PreCond( tank!=NULL );
	return tank->X();
};

float AITank::Y( void ) const
{
	PreCond( tank!=NULL );
	return tank->Y();
};

float AITank::Z( void ) const
{
	PreCond( tank!=NULL );
	return tank->Z();
};

size_t AITank::AIType( void ) const
{
	return aiType;
};

void AITank::AIType( size_t _aiType )
{
	aiType = _aiType;
};

size_t AITank::AITeam( void ) const
{
	return aiTeam;
};

void AITank::AITeam( size_t _aiTeam )
{
	aiTeam = _aiTeam;
};

bool AITank::InsideVehicle( const TPoint& point ) const
{
	PreCond( tank!=NULL );
	return tank->InsideVehicle(point);
};

//==========================================================================

void AITank::GuardBot( TApp* app, TLandscape& landObj, TLandscapeAI& land,
					   TParams& params )
{
	float aiDist = 4000;

#if !defined(_DONTUSE)
	switch ( currentState )
	{
	case s_none:
		{
			initX = tank->X();
			initY = tank->Y();
			initZ = tank->Z();
			currentState = s_guard;
			break;
		};
	case s_guard:
		{
			MoveTo( land, initX, initY, initZ );

			// see if any enemy has come too close, and switch to attack if so
			AITank* ais = app->AITanks();
			size_t i;
			bool switchModes = false;
			for ( i=0; i<params.numTanks && !switchModes; i++ )
			{
				TTank& tank = params.tanks[i];
				if ( tank.IsaAI() && tank.Strength()>0 )
				{
					size_t aiId = tank.AIID();
					if ( ais[aiId].AITeam()!=aiTeam )
					{
						float dist = Distance( tank.X(), tank.Y(), tank.Z() );
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
				if ( aiTeam!=dp.Team() ) // not friendly to single player
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
				// pick closest enemy AI tank
				AITank* ais = app->AITanks();
				float minDist = aiDist;
				size_t targetIndex = params.numTanks+1;
				size_t i;
				for ( i=0; i<params.numTanks; i++ )
				{
					TTank& tank = params.tanks[i];
					if ( tank.IsaAI() && tank.Strength()>0 )
					{
						size_t aiId = tank.AIID();
						if ( ais[aiId].AITeam()!=aiTeam )
						{
							float dist = Distance( tank.X(), tank.Y(), tank.Z() );
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
					TurnVehicleTurret( px,py,pz );
					TurnVehicle( px, pz );

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
					TTank& tank = params.tanks[targetIndex-1];

					TurnVehicleTurret( tank.X(), tank.Y(), tank.Z() );
					TurnVehicle( tank.X(), tank.Z() );

					if ( (aiCounter%((myId+1)*5))==0 )
					{
						ResetMoves();
					}
					MoveTo( land, tank.X(), tank.Y(), tank.Z() );

					if ( Distance( tank.X(), tank.Y(), tank.Z() ) < aiDist )
					{
						if ( (aiCounter%(myId*2+1))==0 )
						if ( !HitFriendly(app,landObj,params) )
						{
							FireAt( tank.X(), tank.Y(), tank.Z() );
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




void AITank::AttackBot( TApp* app, TLandscape& landObj, TLandscapeAI& land,
					    TParams& params )
{
	float aiDist = 25000;

#if !defined(_DONTUSE)
	switch ( currentState )
	{
	case s_none:
		{
			initX = tank->X();
			initY = tank->Y();
			initZ = tank->Z();
			currentState = s_guard;
			break;
		};
	case s_guard:
		{
			MoveTo( land, initX, initY, initZ );

			// see if any enemy has come too close, and switch to attack if so
			AITank* ais = app->AITanks();
			size_t i;
			bool switchModes = false;
			for ( i=0; i<params.numTanks && !switchModes; i++ )
			{
				TTank& tank = params.tanks[i];
				if ( tank.IsaAI() && tank.Strength()>0 )
				{
					size_t aiId = tank.AIID();
					if ( ais[aiId].AITeam()!=aiTeam )
					{
						float dist = Distance( tank.X(), tank.Y(), tank.Z() );
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
				if ( aiTeam!=dp.Team() ) // not friendly to single player
				{
					float dist = Distance( dp.X(), dp.Y(), dp.Z() );
					if ( dist < aiDist )
					{
						switchModes = true;
					}
				}
			}

			if ( switchModes && tank->Strength()>40 && tank->Fuel()>20 )
			{
				currentState = s_attack;
			}
			break;
		};
	case s_attack:
		{
			if ( tank->Strength()<40 || tank->Fuel()<20 )
			{
				currentState = s_guard;
				ResetMoves();
				MoveTo( land, initX, initY, initZ );
			}
			else
			{
				// pick closest enemy AI tank
				AITank* ais = app->AITanks();
				float minDist = aiDist;
				size_t targetIndex = params.numTanks+1;
				size_t i;
				for ( i=0; i<params.numTanks; i++ )
				{
					TTank& tank = params.tanks[i];
					if ( tank.IsaAI() && tank.Strength()>0 )
					{
						size_t aiId = tank.AIID();
						if ( ais[aiId].AITeam()!=aiTeam )
						{
							float dist = Distance( tank.X(), tank.Y(), tank.Z() );
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
					TurnVehicleTurret( px,py,pz );
					TurnVehicle( px, pz );

					// every 50 moves - re-evaluate direction
					if ( (aiCounter%((myId+1)*2))==0 )
					{
						ResetMoves();
					}
					MoveTo( land, px,py,pz );

					if ( Distance( px,py,pz ) < aiDist * 0.075f )
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
					TTank& tank = params.tanks[targetIndex-1];

					TurnVehicleTurret( tank.X(), tank.Y(), tank.Z() );
					TurnVehicle( tank.X(), tank.Z() );

					// every 50 moves - re-evaluate direction
					if ( (aiCounter%((myId+1)*5))==0 )
					{
						ResetMoves();
					}
					MoveTo( land, tank.X(), tank.Y(), tank.Z() );

					if ( Distance( tank.X(), tank.Y(), tank.Z() ) < aiDist * 0.075f )
					{
						if ( (aiCounter%(myId*2+1))==0 )
						if ( !HitFriendly(app,landObj,params) )
						{
							FireAt( tank.X(), tank.Y(), tank.Z() );
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


void AITank::PatrolBot( TApp* app, TLandscape& landObj, TLandscapeAI& land,
					    TParams& params )
{
	float aiDist = 2000;

#if !defined(_DONTUSE)
	switch ( currentState )
	{
	case s_none:
		{
			initX = tank->X();
			initY = tank->Y();
			initZ = tank->Z();
			currentState = s_patrol;
			break;
		};
	case s_patrol:
		{
			MovePatrol( land );

			// see if any enemy has come too close, and switch to attack if so
			AITank* ais = app->AITanks();
			size_t i;
			bool switchModes = false;
			for ( i=0; i<params.numTanks && !switchModes; i++ )
			{
				TTank& tank = params.tanks[i];
				if ( tank.IsaAI() && tank.Strength()>0 )
				{
					size_t aiId = tank.AIID();
					if ( ais[aiId].AITeam()!=aiTeam )
					{
						float dist = Distance( tank.X(), tank.Y(), tank.Z() );
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
				if ( aiTeam!=dp.Team() ) // not friendly to single player
				{
					float dist = Distance( dp.X(), dp.Y(), dp.Z() );
					if ( dist < aiDist )
					{
						switchModes = true;
					}
				}
			}

			if ( switchModes && tank->Strength()>40 && tank->Fuel()>20 )
			{
				currentState = s_attack;
			}
			break;
		};
	case s_attack:
		{
			if ( tank->Strength()<40 || tank->Fuel()<20 )
			{
				currentState = s_patrol;
				ResetMoves();
				MovePatrol( land );
			}
			else
			{
				// pick closest enemy AI tank
				AITank* ais = app->AITanks();
				float minDist = aiDist;
				size_t targetIndex = params.numTanks+1;
				size_t i;
				for ( i=0; i<params.numTanks; i++ )
				{
					TTank& tank = params.tanks[i];
					if ( tank.IsaAI() && tank.Strength()>0 )
					{
						size_t aiId = tank.AIID();
						if ( ais[aiId].AITeam()!=aiTeam )
						{
							float dist = Distance( tank.X(), tank.Y(), tank.Z() );
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
					TurnVehicleTurret( px,py,pz );
					TurnVehicle( px, pz );

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
							FireAt( px,py,pz);
						}
					}
				}
				else if ( targetIndex<=params.numTanks )
				{
					TTank& tank = params.tanks[targetIndex-1];

					TurnVehicleTurret( tank.X(), tank.Y(), tank.Z() );
					TurnVehicle( tank.X(), tank.Z() );

					// every 50 moves - re-evaluate direction
					if ( (aiCounter%((myId+1)*5))==0 )
					{
						ResetMoves();
					}
					MoveTo( land, tank.X(), tank.Y(), tank.Z() );

					if ( Distance( tank.X(), tank.Y(), tank.Z() ) < aiDist )
					{
						if ( (aiCounter%(myId*2+1))==0 )
						if ( !HitFriendly(app,landObj,params) )
						{
							FireAt( tank.X(), tank.Y(), tank.Z() );
						}
					}
				} 
				else
				{
					currentState = s_patrol;
					ResetMoves();
					MoveTo( land, initX, initY, initZ );
				}
			}
			break;
		};
	}
#endif
}

