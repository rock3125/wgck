#ifndef __AI_TANKAI_H_
#define __AI_TANKAI_H_

//==========================================================================

#include <object/tank.h>
#include <ai/landscapeai.h>
#include <win32/params.h>

//==========================================================================

class BotParser;
class TApp;

//==========================================================================

enum aiType {
		guardBot = 0,
		attackBot,
		patrolBot,
		bomberBot,
		fighterBot,
		flakBot,
		artilleryBot,
		mg42Bot
			};

enum states {
		s_none = 0,
		s_guard,
		s_attack,
		s_patrol,
		s_waitfortakeoff,
		s_takeoff,
		s_land,
		s_climb,
		s_findenemy,
		s_decend
			};

//==========================================================================

class AITank
{
public:
	AITank( void );
	~AITank( void );

	AITank( const AITank& );
	const AITank& operator=( const AITank& );
	const AITank& operator=( const TLandscapeObject& obj );

	void SetVehicle( TTank& _tank );
	TTank& Tank( void ) const;

	void Logic( TApp* app, TLandscape& landObj, TLandscapeAI& land, TParams& params );
	void Draw( bool transp );

	bool HitFriendly( TApp* app, TLandscape& landObj, TParams& params );

	// load bot ai byte code
	bool LoadAICore( const TString& fname, TString& errStr );

	float X( void ) const;
	float Y( void ) const;
	float Z( void ) const;

	size_t	AIType( void ) const;
	void	AIType( size_t );

	size_t	AITeam( void ) const;
	void	AITeam( size_t );

	bool SendOverNetwork( void ) const;
	void SendOverNetwork( bool _sendOverNetwork );

	// point inside this vehicle?
	bool InsideVehicle( const TPoint& point ) const;

private:
	bool  TurnVehicle( float x, float z );
	bool  TurnVehicleTurret( float x, float y, float z );
	void  FireAt( float x, float y, float z );
	float Distance( float px, float py, float pz );
	float Random( float r );

	// move to a location
	bool	MoveTo( TLandscapeAI& land, float x, float y, float z );
	bool	MovePatrol( TLandscapeAI& land );
	void	ResetMoves( void );

	void GuardBot( TApp* app, TLandscape& landObj, TLandscapeAI& land,
				   TParams& params );
	void AttackBot( TApp* app, TLandscape& landObj, TLandscapeAI& land,
				    TParams& params );
	void PatrolBot( TApp* app, TLandscape& landObj, TLandscapeAI& land,
				    TParams& params );
private:
	TTank*			tank;
	size_t			keys;

	BotParser*		botScript;

	// move to parameters & tracking
	size_t			path[1000];
	size_t			numNodes;
	int				currentNode;
	float			movetoX, movetoY, movetoZ;
	float			initX, initY, initZ;
	bool			finishedMove;
	bool			sendOverNetwork;

	size_t			aiType;
	size_t			aiTeam;
	size_t			aiCounter;
	size_t			currentState;

	bool			fireNow;

	// only one can fire per round
	size_t			myId;
};

//==========================================================================

#endif
