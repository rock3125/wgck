#ifndef __AI_FLAKAI_H_
#define __AI_FLAKAI_H_

//==========================================================================

#include <object/flak.h>
#include <ai/landscapeai.h>
#include <ai/tankai.h>
#include <ai/planeai.h>

#include <win32/params.h>

//==========================================================================

class TApp;

//==========================================================================

class AIFlak
{
public:
	AIFlak( void );
	~AIFlak( void );

	AIFlak( const AIFlak& );
	const AIFlak& operator=( const AIFlak& );
	const AIFlak& operator=( const TLandscapeObject& obj );

	void SetVehicle( TFlak& _flak );
	TFlak& Flak( void ) const;

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

	// point inside this vehicle?
	bool InsideVehicle( const TPoint& point ) const;

	bool	SendOverNetwork( void ) const;
	void	SendOverNetwork( bool _sendOverNetwork );

private:
	bool  TurnTurret( float px, float py, float pz );
	void  FireAt( float x, float y, float z );
	float Distance( float px, float py, float pz );
	float Random( float r );

	void FlakBot( TApp* app, TLandscape& landObj, TLandscapeAI& land,
				  TParams& params );
private:
	TFlak*			flak;
	size_t			keys;

	size_t			aiType;
	size_t			aiTeam;
	size_t			aiCounter;
	size_t			currentState;

	bool			fireNow;
	bool			sendOverNetwork;

	// only one can fire per round
	size_t			myId;
};

//==========================================================================

#endif
