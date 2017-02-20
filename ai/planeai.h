#ifndef __AI_PLANEAI_H_
#define __AI_PLANEAI_H_

//==========================================================================

#include <object/plane.h>
#include <ai/landscapeai.h>
#include <ai/tankai.h>

#include <win32/params.h>

//==========================================================================

class TApp;

//==========================================================================

class AIPlane
{
public:
	AIPlane( void );
	~AIPlane( void );

	AIPlane( const AIPlane& );
	const AIPlane& operator=( const AIPlane& );
	const AIPlane& operator=( const TLandscapeObject& obj );

	void SetVehicle( TPlane& _plane );
	TPlane& Plane( void ) const;

	void Logic( TApp* app, TLandscape& landObj, TLandscapeAI& land, TParams& params );
	void Draw( bool transp );

	bool HitFriendly( TApp* app, TLandscape& landObj, TParams& params );

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
	float Distance( float px, float py, float pz );
	float Random( float r );

	float PlaneAngle( void ) const;	// adjust propert plane angle

	// move to a location
	bool	MoveTo( TLandscape& land, float dist, float x, float y, float z );
	bool	TakeOff( TLandscapeAI& land );
	bool	Climb( TLandscapeAI& land );
	bool	BomberClimb( TLandscapeAI& land );
	void	SelectGun( void );
	void	ResetMoves( void );
	bool	StopEngine( void );
	bool	LandAt( TLandscape& land, float px, float py, float pz );
	void	SelectBombs( void );
	void	MoveBomberTo( TLandscape& land, float px, float py, float pz );
	bool	Level( void );

	void FighterBot( TApp* app, TLandscape& landObj, TLandscapeAI& land,
					 TParams& params );
	void BomberBot( TApp* app, TLandscape& landObj, TLandscapeAI& land,
					 TParams& params );
private:
	TPlane*			plane;
	size_t			keys;

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

	// only one can fire per round
	size_t			myId;
};

//==========================================================================

#endif
