#include <precomp_header.h>

#include <common/compatability.h>

#include <botCompiler/botgrammar.h>

// ??? temp
#define kMaxPlayers 16

const size_t kMaxPlayerIds = 16;

//==========================================================================
//
// define bot state engine
// has following features:
//
// (1) single pass logic tree for decision processes
// decide location to head for determined by logic (location)
// decide target to fire at (target)
// decide turn direction (function of location and target)
//

// tree element structures

// statements:
// tsetState(newState)
// tprimaryTarget(loc)
// tsecondaryTarget(loc)
// tturnTowards(loc)
// tturnTurretTowards(loc)
// tmoveTo(loc)
// tfireAt(loc)
// tif(expr,body,else)
// tforeachi(body)
// 
// where each statement can be followed by a next
//

// locations:
// all simple types of enum {tme,tinitPos,tprimaryTarget,tsecondaryTarget,
//							 tnull,tdestination,tnearestFuel,tnearestAmmo,
//							 tnearestHealth,tvar_i};

// expression operators:
// tnot(exp)
// tlessthan(exp1,exp2)
// tgreaterthan(exp1,exp2)
// tgreaterequalthan(exp1,exp2)
// tlessequalthan(exp1,exp2)
// tequal(exp1,exp2)
// tnotequal(exp1,exp2)
// tor(exp1,exp2)
// tand(exp1,exp2)
// tplus(exp1,exp2)
// tminus(exp1,exp2)
// ttimes(exp1,exp2)
// tdivide(exp1,exp2)

// expressions:
// vfloat
// tammo
// tfuel
// tshields
// tdistance(loc1,loc2)
// trandom(exp)

class BotInterpretor
{
public:
	BotInterpretor( void );
	BotInterpretor( BotParser* _parser );
	~BotInterpretor( void );

	BotInterpretor( const BotInterpretor& );
	const BotInterpretor& operator=( const BotInterpretor& );

	void		Logic( void );

	BotTree*	MoveTo( void ) const;
	void		MoveTo( BotTree* _moveTo );

	BotTree*	PrimaryTarget( void ) const;
	void		PrimaryTarget( BotTree* _target );

	BotTree*	SecondaryTarget( void ) const;
	void		SecondaryTarget( BotTree* _target );

	BotTree*	TurnTowards( void ) const;
	void		TurnTowards( BotTree* _turnTowards );

	BotTree*	TurnTurretTowards( void ) const;
	void		TurnTurretTowards( BotTree* _turnTurretTowards );

	BotTree*	FireAt( void ) const;
	void		FireAt( BotTree* _fireAt );

	size_t		Shields( void ) const;
	void		Shields( size_t _shields );

	size_t		Fuel( void ) const;
	void		Fuel( size_t _fuel );

	size_t		Ammo( void ) const;
	void		Ammo( size_t _ammo );

	size_t		Health( void ) const;
	void		Health( size_t _health );

private:
	void ProcessStatement( BotTree* statement,
						   bool& moveToProcessed,
						   bool& primaryTargetProcessed,
						   bool& secondaryTargetProcessed,
						   bool& turnTowardsProcessed,
						   bool& turnTurretTowardsProcessed,
						   bool& fireAtProcessed );

	bool EvaluateBoolExpression( BotTree* expr );
	float EvaluateExpression( BotTree* expr );

	// helper functions

	// convert a tvar_i into a tplayerId if necessairy
	BotTree* Translate( BotTree* item );

	float Random( float num );
	float Distance( BotTree* loc1, BotTree* loc2 );
	void  LocationToXYZ( BotTree* loc, float& x, float& y, float& z );

private:
	BotParser*	parser;

	BotTree*	currentState;
	BotTree*	moveTo;
	BotTree*	primaryTarget;
	BotTree*	secondaryTarget;
	BotTree*	turnTowards;
	BotTree*	turnTurretTowards;
	BotTree*	fireAt;

	BotTree		playerIds[kMaxPlayerIds];
	size_t		playerIdCounter;

	bool		currentStateChanged;
	bool		moveToChanged;
	bool		primaryTargetChanged;
	bool		secondaryTargetChanged;
	bool		turnTowardsChanged;
	bool		turnTurretTowardsChanged;
	bool		fireAtChanged;

	// vehicle internals
	size_t		shields;
	size_t		fuel;
	size_t		health;
	size_t		ammo;

	float		initPosX;
	float		initPosY;
	float		initPosZ;

	float		x,y,z;

	bool		insideForEach;
	size_t		forEachCounter;
};

BotInterpretor::BotInterpretor( void )
	: parser(NULL)
{
	currentState = NULL;

	moveTo = NULL;
	primaryTarget = NULL;
	secondaryTarget = NULL;
	turnTowards = NULL;
	turnTurretTowards = NULL;
	fireAt = NULL;

	currentStateChanged = false;
	moveToChanged = false;
	primaryTargetChanged = false;
	secondaryTargetChanged = false;
	turnTowardsChanged = false;
	turnTurretTowardsChanged = false;
	fireAtChanged = false;

	shields = 0;
	fuel = 0;
	health = 0;
	ammo = 0;

	initPosX = 0;
	initPosY = 0;
	initPosZ = 0;

	x = 0;
	y = 0;
	z = 0;

	insideForEach = false;
	forEachCounter = 0;

	playerIdCounter = 0;
};

BotInterpretor::BotInterpretor( BotParser* _parser )
	: parser( _parser )
{
	currentState = parser->InitialState();

	moveTo = NULL;
	primaryTarget = NULL;
	secondaryTarget = NULL;
	turnTowards = NULL;
	turnTurretTowards = NULL;
	fireAt = NULL;

	currentStateChanged = false;
	moveToChanged = false;
	primaryTargetChanged = false;
	secondaryTargetChanged = false;
	turnTowardsChanged = false;
	turnTurretTowardsChanged = false;
	fireAtChanged = false;

	shields = 0;
	fuel = 0;
	health = 0;
	ammo = 0;

	initPosX = 0;
	initPosY = 0;
	initPosZ = 0;

	x = 0;
	y = 0;
	z = 0;

	insideForEach = false;
	forEachCounter = 0;

	playerIdCounter = 0;
};

BotInterpretor::~BotInterpretor( void )
{
	parser = NULL;

	currentState = NULL;

	moveTo = NULL;
	primaryTarget = NULL;
	secondaryTarget = NULL;
	turnTowards = NULL;
	turnTurretTowards = NULL;
	fireAt = NULL;
};

BotInterpretor::BotInterpretor( const BotInterpretor& bi )
	: parser(NULL)
{
	operator=(bi);
};

const BotInterpretor& BotInterpretor::operator=( const BotInterpretor& bi )
{
	parser = bi.parser;

	currentState = bi.currentState;

	moveTo = bi.moveTo;
	primaryTarget = bi.primaryTarget;
	secondaryTarget = bi.secondaryTarget;
	turnTowards = bi.turnTowards;
	turnTurretTowards = bi.turnTurretTowards;
	fireAt = bi.fireAt;

	currentStateChanged = bi.currentStateChanged;
	moveToChanged = bi.moveToChanged;
	primaryTargetChanged = bi.primaryTargetChanged;
	secondaryTargetChanged = bi.secondaryTargetChanged;
	turnTowardsChanged = bi.turnTowardsChanged;
	turnTurretTowardsChanged = bi.turnTurretTowardsChanged;
	fireAtChanged = bi.fireAtChanged;

	shields = bi.shields;
	fuel = bi.fuel;
	health = bi.health;
	ammo = bi.ammo;

	initPosX = bi.initPosX;
	initPosY = bi.initPosY;
	initPosZ = bi.initPosZ;

	x = bi.x;
	y = bi.y;
	z = bi.z;

	insideForEach = bi.insideForEach;
	forEachCounter = bi.forEachCounter;

	playerIdCounter = bi.playerIdCounter;
	for ( size_t i=0; i<kMaxPlayerIds; i++ )
		playerIds[i] = bi.playerIds[i];

	return *this;
};

BotTree* BotInterpretor::MoveTo( void ) const
{
	return moveTo;
};

void BotInterpretor::MoveTo( BotTree* _moveTo )
{
	moveTo = _moveTo;
};

BotTree* BotInterpretor::PrimaryTarget( void ) const
{
	return primaryTarget;
};

void BotInterpretor::PrimaryTarget( BotTree* _target )
{
	primaryTarget = _target;
};

BotTree* BotInterpretor::SecondaryTarget( void ) const
{
	return secondaryTarget;
};

void BotInterpretor::SecondaryTarget( BotTree* _target )
{
	secondaryTarget = _target;
};

BotTree* BotInterpretor::TurnTowards( void ) const
{
	return turnTowards;
};

void BotInterpretor::TurnTowards( BotTree* _turnTowards )
{
	turnTowards = _turnTowards;
};

BotTree* BotInterpretor::TurnTurretTowards( void ) const
{
	return turnTurretTowards;
};

void BotInterpretor::TurnTurretTowards( BotTree* _turnTurretTowards )
{
	turnTurretTowards = _turnTurretTowards;
};

BotTree* BotInterpretor::FireAt( void ) const
{
	return fireAt;
};

void BotInterpretor::FireAt( BotTree* _fireAt )
{
	fireAt = _fireAt;
};

size_t BotInterpretor::Shields( void ) const
{
	return shields;
};

void BotInterpretor::Shields( size_t _shields )
{
	shields = _shields;
};

size_t BotInterpretor::Fuel( void ) const
{
	return fuel;
};

void BotInterpretor::Fuel( size_t _fuel )
{
	fuel = _fuel;
};

size_t BotInterpretor::Ammo( void ) const
{
	return ammo;
};

void BotInterpretor::Ammo( size_t _ammo )
{
	ammo = _ammo;
};

size_t BotInterpretor::Health( void ) const
{
	return health;
};

void BotInterpretor::Health( size_t _health )
{
	health = _health;
};

void BotInterpretor::Logic( void )
{
	PreCond( parser!=NULL );
	PreCond( currentState!=NULL );

	// keep track of changes
	BotTree*	t_currentState = currentState;
	BotTree*	t_moveTo = moveTo;
	BotTree*	t_primaryTarget = primaryTarget;
	BotTree*	t_secondaryTarget = secondaryTarget;
	BotTree*	t_turnTowards = turnTowards;
	BotTree*	t_turnTurretTowards = turnTurretTowards;
	BotTree*	t_fireAt = fireAt;

	bool		moveToProcessed = false;
	bool		primaryTargetProcessed = false;
	bool		secondaryTargetProcessed = false;
	bool		turnTowardsProcessed = false;
	bool		turnTurretTowardsProcessed = false;
	bool		fireAtProcessed = false;

	// retrieve state and process all statements
	BotTree* statement = currentState;
	while ( statement!=NULL )
	{
		ProcessStatement( statement,
						  moveToProcessed,
						  primaryTargetProcessed,
						  secondaryTargetProcessed,
						  turnTowardsProcessed,
						  turnTurretTowardsProcessed,
						  fireAtProcessed );

		statement = statement->Next();
	}

	if ( !moveToProcessed )
	{
		moveTo = NULL;
	}
	moveToChanged = (moveTo!=t_moveTo);

	if ( !primaryTargetProcessed )
	{
		primaryTarget = NULL;
	}
	primaryTargetChanged = (primaryTarget!=t_primaryTarget);

	if ( !secondaryTargetProcessed )
	{
		secondaryTarget = NULL;
	}
	secondaryTargetChanged = (secondaryTarget!=t_secondaryTarget);

	if ( !turnTowardsProcessed )
	{
		turnTowards = NULL;
	}
	turnTowardsChanged = (turnTowards!=t_turnTowards);

	if ( !turnTurretTowardsProcessed )
	{
		turnTurretTowards = NULL;
	}
	turnTurretTowardsChanged = (turnTurretTowards!=t_turnTurretTowards);

	if ( !fireAtProcessed )
	{
		fireAt = NULL;
	}
	fireAtChanged = (fireAt!=t_fireAt);
};


BotTree* BotInterpretor::Translate( BotTree* item )
{
	PreCond( item!=NULL );

	if ( item->Tag()==tvar_i )
	{
		PreCond( insideForEach );
		BotTree* pid = &playerIds[playerIdCounter++];
		if ( playerIdCounter>kMaxPlayerIds )
			playerIdCounter = 0;
		pid->Value( float(forEachCounter) );
		return pid;
	}
	return item;
};


void BotInterpretor::ProcessStatement( BotTree* statement,
									   bool& moveToProcessed,
									   bool& primaryTargetProcessed,
									   bool& secondaryTargetProcessed,
									   bool& turnTowardsProcessed,
									   bool& turnTurretTowardsProcessed,
									   bool& fireAtProcessed )
{
	PreCond( statement!=NULL );

	switch ( statement->Tag() )
	{
		case tsetState:
		{
			currentState = parser->GetState( statement->Name() );
			break;
		}
		case tprimaryTarget:
		{
			primaryTarget = Translate(statement->Left());
			primaryTargetProcessed = true;
			break;
		}
		case tsecondaryTarget:
		{
			secondaryTarget = Translate(statement->Left());
			secondaryTargetProcessed = true;
			break;
		}
		case tturnTowards:
		{
			turnTowards = Translate(statement->Left());
			turnTowardsProcessed = true;
			break;
		}
		case tturnTurretTowards:
		{
			turnTurretTowards = Translate(statement->Left());
			turnTurretTowardsProcessed = true;
			break;
		}
		case tmoveTo:
		{
			moveTo = Translate(statement->Left());
			moveToProcessed = true;
			break;
		}
		case tfireAt:
		{
			fireAt = Translate(statement->Left());
			fireAtProcessed = true;
			break;
		}
		case tif:
		{
			// tif(expr,body,else)
			if ( EvaluateBoolExpression(statement->Left()) )
			{
				ProcessStatement( statement->Right(),
								  moveToProcessed,
								  primaryTargetProcessed,
								  secondaryTargetProcessed,
								  turnTowardsProcessed,
								  turnTurretTowardsProcessed,
								  fireAtProcessed );
			}
			else if ( statement->ElsePart()!=NULL )
			{
				ProcessStatement( statement->ElsePart(),
								  moveToProcessed,
								  primaryTargetProcessed,
								  secondaryTargetProcessed,
								  turnTowardsProcessed,
								  turnTurretTowardsProcessed,
								  fireAtProcessed );
			}
			break;
		}
		case tforeachi:
		{
			PreCond( !insideForEach );
			insideForEach = true;
			for ( forEachCounter=0; forEachCounter<kMaxPlayers; forEachCounter++ )
			{
				ProcessStatement( statement->Left(),
								  moveToProcessed,
								  primaryTargetProcessed,
								  secondaryTargetProcessed,
								  turnTowardsProcessed,
								  turnTurretTowardsProcessed,
								  fireAtProcessed );
			}
			break;
		}
		default:
		{
			PostCond( "Error - unknown state"==NULL );
			break;
		}
	}
};


bool BotInterpretor::EvaluateBoolExpression( BotTree* expr )
{
	PreCond( expr!=NULL );

	switch ( expr->Tag() )
	{
	case tnot:
	{
		return (!EvaluateBoolExpression( expr->Left()));
		break;
	}
	case tlessthan:
	{
		return (EvaluateExpression(expr->Left()) < EvaluateExpression(expr->Right()));
		break;
	}
	case tgreaterthan:
	{
		return (EvaluateExpression(expr->Left()) > EvaluateExpression(expr->Right()));
		break;
	}
	case tgreaterequalthan:
	{
		return (EvaluateExpression(expr->Left()) >= EvaluateExpression(expr->Right()));
		break;
	}
	case tlessequalthan:
	{
		return (EvaluateExpression(expr->Left()) <= EvaluateExpression(expr->Right()));
		break;
	}
	case tequal:
	{
		return (EvaluateExpression(expr->Left()) == EvaluateExpression(expr->Right()));
		break;
	}
	case tnotequal:
	{
		return (EvaluateExpression(expr->Left()) != EvaluateExpression(expr->Right()));
		break;
	}
	case tor:
	{
		return (EvaluateBoolExpression(expr->Left()) || EvaluateBoolExpression(expr->Right()));
		break;
	}
	case tand:
	{
		return (EvaluateBoolExpression(expr->Left()) && EvaluateBoolExpression(expr->Right()));
		break;
	}
	default:
	{
		PostCond( "Error - unknown bool expression"==NULL );
		break;
	}
	}
	return true;
};


float BotInterpretor::EvaluateExpression( BotTree* expr )
{
	PreCond( expr!=NULL );

	switch ( expr->Tag() )
	{
	case vfloat:
	{
		return expr->Value();
	}
	case tammo:
	{
		return float(ammo);
	}
	case tshields:
	{
		return float(shields);
	}
	case tfuel:
	{
		return float(fuel);
	}
	case thealth:
	{
		return float(health);
	}
	case tdistance:
	{
		return Distance(expr->Left(),expr->Right());
	}
	case trandom:
	{
		return Random(EvaluateExpression(expr->Left()));
	}
	default:
	{
		PreCond( "Invalid expression tag - can't evaluate"==NULL );
	}
	}
	return 0;
}


float BotInterpretor::Random( float num )
{
	// ??? not yet implemented
	return 0;
};


float BotInterpretor::Distance( BotTree* loc1, BotTree* loc2 )
{
	PreCond( loc1!=NULL );
	PreCond( loc2!=NULL );

	float x1,y1,z1;
	float x2,y2,z2;

	LocationToXYZ( loc1, x1,y1,z1 );
	LocationToXYZ( loc2, x2,y2,z2 );

	return float(sqrt( (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) + (z1-z2)*(z1-z2) ));
};


void BotInterpretor::LocationToXYZ( BotTree* loc, float& _x, float& _y, float& _z )
{
	PreCond( loc!=NULL );

	switch ( loc->Tag() )
	{
	case tme:
	{
		_x = x;
		_y = y;
		_z = z;
		break;
	};
	case tinitPos:
	{
		_x = initPosX;
		_y = initPosY;
		_z = initPosZ;
		break;
	}
	case tprimaryTarget:
	{
		PreCond( primaryTarget!=NULL );
		LocationToXYZ( primaryTarget, _x,_y,_z );
		break;
	}
	case tsecondaryTarget:
	{
		PreCond( secondaryTarget!=NULL );
		LocationToXYZ( secondaryTarget, _x,_y,_z );
		break;
	}
	case tplayerId: // ??? not yet implemented
	{
		size_t playerId = size_t(loc->Value());
		//app->GetPlayerPosition( playerId, _x,_y,_z );
		_x = 0;
		_y = 0;
		_z = 0;
		break;
	}
	case tnearestFuel: // ??? not yet implemented
	{
		//app->GetNearestFuel( x,y,z, _x,_y,_z );
		_x = 0;
		_y = 0;
		_z = 0;
		break;
	}
	case tnearestAmmo: // ??? not yet implemented
	{
		//app->GetNearestAmmo( x,y,z, _x,_y,_z );
		_x = 0;
		_y = 0;
		_z = 0;
		break;
	}
	case tnearestHealth: // ??? not yet implemented
	{
		//app->GetNearestHealth( x,y,z, _x,_y,_z );
		_x = 0;
		_y = 0;
		_z = 0;
		break;
	}
	case tvar_i: // ??? not yet implemented
	{
		PreCond( insideForEach );
		//app->GetPlayerPosition( forEachCounter, _x,_y,_z );
		_x = 0;
		_y = 0;
		_z = 0;
		break;
	}
	default:
	{
		PreCond( "Invalid tag in location"==NULL );
	}
	}
};

//==========================================================================

