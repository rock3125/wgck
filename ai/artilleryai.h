#ifndef __AI_ARTILLERYAI_H_
#define __AI_ARTILLERYAI_H_

//==========================================================================

#include <ai/landscapeai.h>
#include <ai/planeai.h>
#include <ai/flakai.h>
#include <ai/tankai.h>

#include <object/artillery.h>
#include <win32/params.h>

//==========================================================================

class TApp;

//==========================================================================

class AIArtillery
{
public:
	AIArtillery( void );
	~AIArtillery( void );

	AIArtillery( const AIArtillery& );
	const AIArtillery& operator=( const AIArtillery& );
	const AIArtillery& operator=( const TLandscapeObject& obj );

	void SetVehicle( TArtillery& _artillery );
	TArtillery& Artillery( void ) const;

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
	bool  TurnVehicle( float x, float y, float z );
	void  FireAt( float x, float y, float z );
	float Distance( float px, float py, float pz );
	float Random( float r );

	// move to a location
	bool	MoveTo( TLandscapeAI& land, float x, float y, float z );
	void	ResetMoves( void );

	void ArtilleryBot( TApp* app, TLandscape& landObj, TLandscapeAI& land,
					   TParams& params );
private:
	TArtillery*		artillery;
	size_t			keys;

	// move to parameters & tracking
	size_t			path[1000];
	size_t			numNodes;
	int				currentNode;
	float			movetoX, movetoY, movetoZ;
	float			initX, initY, initZ;
	bool			finishedMove;

	size_t			aiType;
	size_t			aiTeam;
	size_t			aiCounter;
	size_t			currentState;

	bool			sendOverNetwork;

	bool			fireNow;

	// only one can fire per round
	size_t			myId;
};

//==========================================================================

#endif
