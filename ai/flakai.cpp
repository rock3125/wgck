#include <precomp_header.h>

#include <win32/win32.h>

#include <tank/tankapp.h>
#include <object/geometry.h>
#include <ai/flakai.h>

#include <botCompiler/botgrammar.h>

#if defined(_EDITOR) || defined(_PARSER) || defined(_VIEWER)
#define _DONTUSE
#endif

//==========================================================================

const float Deg2Rad = 0.017453293f;
const float Rad2Deg = 57.29577951f;

//==========================================================================

AIFlak::AIFlak( void )
	: flak(NULL)
{
	myId = (aiId++ % kAIFrequency) + 1;

	sendOverNetwork = true;

	keys = 0;
	currentState = none;
	aiType = flakBot;
	aiTeam = 0;
	aiCounter = 0;
	fireNow = false;
};


AIFlak::~AIFlak( void )
{
	flak = NULL;
};


AIFlak::AIFlak( const AIFlak& a )
{
	operator=(a);
};

const AIFlak& AIFlak::operator=( const AIFlak& a )
{
	flak = a.flak;
	keys = a.keys;

	currentState = a.currentState;

	myId = a.myId;

	aiType = a.aiType;
	aiTeam = a.aiTeam;
	aiCounter = a.aiCounter;
	fireNow = a.fireNow;
	sendOverNetwork = a.sendOverNetwork;

	return *this;
};

const AIFlak& AIFlak::operator=( const TLandscapeObject& obj )
{
	aiType = obj.AIType();
	aiTeam = obj.AITeam();
	sendOverNetwork = true;

	return *this;
};

bool AIFlak::SendOverNetwork( void ) const
{
	return sendOverNetwork;
};

void AIFlak::SendOverNetwork( bool _sendOverNetwork )
{
	sendOverNetwork = _sendOverNetwork;
};

void AIFlak::SetVehicle( TFlak& _flak )
{
	flak = &_flak;
	flak->IsaAI( true );
};

TFlak& AIFlak::Flak( void ) const
{
	PreCond( flak!=NULL );
	return *flak;
};

bool AIFlak::HitFriendly( TApp* app, TLandscape& landObj, TParams& params )
{
#if !defined(_DONTUSE)
	size_t i;
	AITank* ais = app->AITanks();

	TPoint start, dirn;
	size_t gunRange;
	flak->GetGunDetails( start, dirn, gunRange );

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
				if ( aif[i].AITeam()==aiTeam && &flak2!=flak )
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


void AIFlak::Logic( TApp* app, TLandscape& landObj, TLandscapeAI& land, 
				    TParams& params )
{
	PreCond( flak!=NULL );

	if ( flak->Strength()>0 )
	{
		aiCounter++;

		switch ( aiType )
		{
		case flakBot:
			{
				if ( (logicCounter%myId)==0 )
					FlakBot(app,landObj,land,params);
				break;
			}
		}

		if ( fireNow )
		{
			keys |= aFire;
			fireNow = false;
		}
		else
		{
			keys &= (~aFire);
		}
		flak->Move(keys,false,0,0);
	};
};


void AIFlak::Draw( bool transp )
{
	PreCond( flak!=NULL );
	if ( flak->Strength()>0 )
	{
		flak->Draw(transp);
	}
};


float AIFlak::Distance( float px, float py, float pz )
{
	float dist = (flak->X()-px)*(flak->X()-px) + 
				 (flak->Y()-py)*(flak->Y()-py) +
				 (flak->Z()-pz)*(flak->Z()-pz);
	return dist;
};


float AIFlak::Random( float r )
{
	float _r = float(rand()) / float(RAND_MAX);
	return _r*r;
};


void AIFlak::FireAt( float x, float y, float z )
{
	fireNow = true;
};

float AIFlak::X( void ) const
{
	PreCond( flak!=NULL );
	return flak->X();
};

float AIFlak::Y( void ) const
{
	PreCond( flak!=NULL );
	return flak->Y();
};

float AIFlak::Z( void ) const
{
	PreCond( flak!=NULL );
	return flak->Z();
};

size_t AIFlak::AIType( void ) const
{
	return aiType;
};

void AIFlak::AIType( size_t _aiType )
{
	aiType = _aiType;
};

size_t AIFlak::AITeam( void ) const
{
	return aiTeam;
};

void AIFlak::AITeam( size_t _aiTeam )
{
	aiTeam = _aiTeam;
};

bool AIFlak::TurnTurret( float px, float py, float pz )
{
	float yangle = -flak->Yangle() * Deg2Rad;
	float v1fcos = -(float)cos(yangle);
	float v1fsin = (float)sin(yangle);

	float tx = flak->X() - v1fsin;
	float tz = flak->Z() - v1fcos;

	yangle = (-flak->Yangle()-10) * Deg2Rad;
	v1fcos = -(float)cos(yangle);
	v1fsin = (float)sin(yangle);
	float tx1 = flak->X() - v1fsin;
	float tz1 = flak->Z() - v1fcos;

	yangle = (-flak->Yangle()+10) * Deg2Rad;
	v1fcos = -(float)cos(yangle);
	v1fsin = (float)sin(yangle);
	float tx2 = flak->X() - v1fsin;
	float tz2 = flak->Z() - v1fcos;

	float d1 = (tx-px)*(tx-px) + (tz-pz)*(tz-pz);
	float d2 = (tx1-px)*(tx1-px) + (tz1-pz)*(tz1-pz);
	float d3 = (tx2-px)*(tx2-px) + (tz2-pz)*(tz2-pz);

	bool ready = true;

	keys = (keys&~15);
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

	// aim the barrel
	float dist = float(sqrt(d1));
	float dh = flak->Y() - py;
	float barrelAngle;
	if ( dh==0 || dist==0 )
	{
		barrelAngle = 0;
	}
	else
	{
		barrelAngle = float(atan(dh/dist)) * Rad2Deg;
	}
	flak->BarrelAngle( barrelAngle );

	return ready;
};

bool AIFlak::InsideVehicle( const TPoint& point ) const
{
	PreCond( flak!=NULL );
	return flak->InsideVehicle(point);
};

//==========================================================================

void AIFlak::FlakBot( TApp* app, TLandscape& landObj, TLandscapeAI& land,
					  TParams& params )
{
	float aiDist = 6000;

#if !defined(_DONTUSE)
	switch ( currentState )
	{
	case s_none:
		{
			currentState = s_guard;
			break;
		};
	case s_guard:
		{
			// see if any enemy has come too close, and switch to attack if so
			AIFlak* ais = app->AIFlaks();
			size_t i;
			bool switchModes = false;
			for ( i=0; i<params.numFlak && !switchModes; i++ )
			{
				TFlak& flak = params.flak[i];
				if ( flak.IsaAI() && flak.Strength()>0 )
				{
					size_t aiId = flak.AIID();
					if ( ais[aiId].AITeam()!=aiTeam )
					{
						float dist = Distance( flak.X(), flak.Y(), flak.Z() );
						if ( dist < aiDist )
						{
							switchModes = true;
						}
					}
				}
			}

			AITank* ais2 = app->AITanks();
			for ( i=0; i<params.numTanks && !switchModes; i++ )
			{
				TTank& tank = params.tanks[i];
				if ( tank.IsaAI() && tank.Strength()>30 )
				{
					size_t aiId = tank.AIID();
					if ( ais2[aiId].AITeam()!=aiTeam )
					{
						float dist = Distance( tank.X(), tank.Y(), tank.Z() );
						if ( dist < aiDist )
						{
							switchModes = true;
						}
					}
				}
			}

			AIPlane* ais3 = app->AIPlanes();
			for ( i=0; i<params.numPlanes && !switchModes; i++ )
			{
				TTank& tank = params.tanks[i];
				if ( tank.IsaAI() && tank.Strength()>30 )
				{
					size_t aiId = tank.AIID();
					if ( ais2[aiId].AITeam()!=aiTeam )
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

			if ( switchModes && flak->Strength()>0 )
			{
				currentState = s_attack;
			}
			break;
		};
	case s_attack:
		{
			if ( flak->Strength()==0 || flak->Ammo()==0 )
			{
				currentState = s_guard;
			}
			else
			{
				float minDist = aiDist;
				bool foundTarget = false;
				float tx,ty,tz;
				size_t i;

				// pick closest enemy AI tank
				AITank* ais = app->AITanks();
				for ( i=0; i<params.numTanks; i++ )
				{
					TTank& tank = params.tanks[i];
					if ( tank.IsaAI() && tank.Strength()>30 )
					{
						size_t aiId = tank.AIID();
						if ( ais[aiId].AITeam()!=aiTeam )
						{
							float dist = Distance( tank.X(), tank.Y(), tank.Z() );
							if ( dist < minDist )
							{
								tx = tank.X();
								ty = tank.Y();
								tz = tank.Z();
								minDist = dist;
								foundTarget = true;
							}
						}
					}
				}

				AIPlane* ais2 = app->AIPlanes();
				for ( i=0; i<params.numPlanes; i++ )
				{
					TPlane& plane = params.planes[i];
					if ( plane.IsaAI() && plane.Strength()>0 )
					{
						size_t aiId = plane.AIID();
						if ( ais2[aiId].AITeam()!=aiTeam )
						{
							float dist = Distance( plane.X(), plane.Y(), plane.Z() );
							if ( dist < minDist )
							{
								tx = plane.X();
								ty = plane.Y();
								tz = plane.Z();
								minDist = dist;
								foundTarget = true;
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
						if ( dist < minDist )
						{
							tx = dp.X();
							ty = dp.Y();
							tz = dp.Z();
							minDist = dist;
							foundTarget = true;
						}
					}
				}

				if ( foundTarget )
				{
					TurnTurret( tx, ty, tz );

					if ( (aiCounter%(myId*2+1))==0 )
					if ( !HitFriendly(app,landObj,params) )
					{
						FireAt( tx, ty, tz );
					}
				}
			}
			break;
		}
	}
#endif
}

