#include <precomp_header.h>

#include <win32/win32.h>

#include <tank/tankapp.h>
#include <object/geometry.h>
#include <ai/mg42ai.h>

#include <botCompiler/botgrammar.h>

#if defined(_EDITOR) || defined(_PARSER) || defined(_VIEWER)
#define _DONTUSE
#endif

//==========================================================================

const float Deg2Rad = 0.017453293f;
const float Rad2Deg = 57.29577951f;

//==========================================================================

AIMG42::AIMG42( void )
	: mg42(NULL)
{
	myId = (aiId++ % kAIFrequency) + 1;

	keys = 0;
	currentState = none;
	aiType = mg42Bot;
	aiTeam = 0;
	aiCounter = 0;
	fireNow = false;
	sendOverNetwork = true;

	timeout = 0;
	fireFlipFlag = false;
};


AIMG42::~AIMG42( void )
{
	mg42 = NULL;
};


AIMG42::AIMG42( const AIMG42& a )
{
	operator=(a);
};

const AIMG42& AIMG42::operator=( const AIMG42& a )
{
	mg42 = a.mg42;
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

const AIMG42& AIMG42::operator=( const TLandscapeObject& obj )
{
	aiType = obj.AIType();
	aiTeam = obj.AITeam();
	sendOverNetwork = true;

	return *this;
};

bool AIMG42::SendOverNetwork( void ) const
{
	return sendOverNetwork;
};

void AIMG42::SendOverNetwork( bool _sendOverNetwork )
{
	sendOverNetwork = _sendOverNetwork;
};

void AIMG42::SetVehicle( TMG42& _mg42 )
{
	mg42 = &_mg42;
	mg42->IsaAI( true );
};

TMG42& AIMG42::MG42( void ) const
{
	PreCond( mg42!=NULL );
	return *mg42;
};

bool AIMG42::HitFriendly( TApp* app, TLandscape& landObj, TParams& params )
{
#if !defined(_DONTUSE)
	size_t i;
	AITank* ais = app->AITanks();

	TPoint start, dirn;
	size_t gunRange;
	mg42->GetGunDetails( start, dirn, gunRange );

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

		AIMG42* aif = app->AIMG42s();
		for ( i=0; i<params.numFlak; i++ )
		{
			TMG42& flak2 = params.mg42[i];
			if ( flak2.IsaAI() && flak2.Strength()>0 )
			{
				if ( aif[i].AITeam()==aiTeam && &flak2!=mg42 )
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


void AIMG42::Logic( TApp* app, TLandscape& landObj, TLandscapeAI& land, 
				    TParams& params )
{
	PreCond( mg42!=NULL );

	if ( mg42->Strength()>0 )
	{
		aiCounter++;

		switch ( aiType )
		{
		case mg42Bot:
			{
				if ( (logicCounter%myId)==0 )
					MG42Bot(app,landObj,land,params);
				break;
			}
		}

		if ( fireNow )
		{
			keys |= aFire;
		}
		else
		{
			keys &= (~aFire);
		}
		mg42->Move(keys,false,0,0);
	};
};


void AIMG42::Draw( bool transp )
{
	PreCond( mg42!=NULL );
	if ( mg42->Strength()>0 )
	{
		mg42->Draw(transp);
	}
};


float AIMG42::Distance( float px, float py, float pz )
{
	float dist = (mg42->X()-px)*(mg42->X()-px) + 
				 (mg42->Y()-py)*(mg42->Y()-py) +
				 (mg42->Z()-pz)*(mg42->Z()-pz);
	return dist;
};


float AIMG42::Random( float r )
{
	float _r = float(rand()) / float(RAND_MAX);
	return _r*r;
};


float AIMG42::X( void ) const
{
	PreCond( mg42!=NULL );
	return mg42->X();
};

float AIMG42::Y( void ) const
{
	PreCond( mg42!=NULL );
	return mg42->Y();
};

float AIMG42::Z( void ) const
{
	PreCond( mg42!=NULL );
	return mg42->Z();
};

size_t AIMG42::AIType( void ) const
{
	return aiType;
};

void AIMG42::AIType( size_t _aiType )
{
	aiType = _aiType;
};

size_t AIMG42::AITeam( void ) const
{
	return aiTeam;
};

void AIMG42::AITeam( size_t _aiTeam )
{
	aiTeam = _aiTeam;
};

bool AIMG42::TurnTurret( float px, float py, float pz )
{
	float yangle = -mg42->Yangle() * Deg2Rad;
	float v1fcos = -(float)cos(yangle);
	float v1fsin = (float)sin(yangle);

	float tx = mg42->X() - v1fsin;
	float tz = mg42->Z() - v1fcos;

	yangle = (-mg42->Yangle()-10) * Deg2Rad;
	v1fcos = -(float)cos(yangle);
	v1fsin = (float)sin(yangle);
	float tx1 = mg42->X() - v1fsin;
	float tz1 = mg42->Z() - v1fcos;

	yangle = (-mg42->Yangle()+10) * Deg2Rad;
	v1fcos = -(float)cos(yangle);
	v1fsin = (float)sin(yangle);
	float tx2 = mg42->X() - v1fsin;
	float tz2 = mg42->Z() - v1fcos;

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
	float barrelAngle = 0;
	mg42->BarrelAngle( barrelAngle );

	return ready;
};

bool AIMG42::InsideVehicle( const TPoint& point ) const
{
	PreCond( mg42!=NULL );
	return mg42->InsideVehicle(point);
};

//==========================================================================

void AIMG42::MG42Bot( TApp* app, TLandscape& landObj, TLandscapeAI& land,
					  TParams& params )
{
	float aiDist = 600;

#if !defined(_DONTUSE)
	size_t time = ::GetTickCount();

	switch ( currentState )
	{
	case s_none:
		{
			fireNow = false;
			currentState = s_guard;
			break;
		};
	case s_guard:
		{
			// see if any enemy has come too close, and switch to attack if so
			fireNow = false;
			AIMG42* ais = app->AIMG42s();
			size_t i;
			bool switchModes = false;
			for ( i=0; i<params.numFlak && !switchModes; i++ )
			{
				TMG42& mg42 = params.mg42[i];
				if ( mg42.IsaAI() && mg42.Strength()>0 )
				{
					size_t aiId = mg42.AIID();
					if ( ais[aiId].AITeam()!=aiTeam )
					{
						float dist = Distance( mg42.X(), mg42.Y(), mg42.Z() );
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

			if ( switchModes && mg42->Strength()>0 )
			{
				currentState = s_attack;
			}
			break;
		};
	case s_attack:
		{
			if ( mg42->Strength()==0 )
			{
				keys = 0;
				fireNow = false;
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

					fireNow = false;
					if ( time > timeout )
					{
						if ( fireFlipFlag )
							timeout = time + 1000; // 1 sec wait - no fire
						else
							timeout = time + 2000; // 2 secs fire

						fireFlipFlag = !fireFlipFlag;
					}

					if ( !HitFriendly(app,landObj,params) )
					{
						fireNow = fireFlipFlag;
					}
				}
				else
				{
					fireNow = false;
				}
			}
			break;
		}
	}
#endif
}

