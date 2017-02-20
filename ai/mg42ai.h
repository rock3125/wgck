#ifndef __AI_MG42AI_H_
#define __AI_MG42AI_H_

//==========================================================================

#include <object/mg42.h>
#include <ai/landscapeai.h>
#include <ai/tankai.h>
#include <ai/planeai.h>

#include <win32/params.h>

//==========================================================================

class TApp;

//==========================================================================

class AIMG42
{
public:
	AIMG42( void );
	~AIMG42( void );

	AIMG42( const AIMG42& );
	const AIMG42& operator=( const AIMG42& );
	const AIMG42& operator=( const TLandscapeObject& obj );

	void SetVehicle( TMG42& _mg42 );
	TMG42& MG42( void ) const;

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

	bool	SendOverNetwork( void ) const;
	void	SendOverNetwork( bool _sendOverNetwork );

	// point inside this vehicle?
	bool InsideVehicle( const TPoint& point ) const;

private:
	bool  TurnTurret( float px, float py, float pz );
	float Distance( float px, float py, float pz );
	float Random( float r );

	void MG42Bot( TApp* app, TLandscape& landObj, TLandscapeAI& land,
				  TParams& params );
private:
	TMG42*			mg42;
	size_t			keys;

	size_t			aiType;
	size_t			aiTeam;
	size_t			aiCounter;
	size_t			currentState;

	size_t			timeout;
	bool			fireFlipFlag;

	bool			fireNow;
	bool			sendOverNetwork;

	// only one can fire per round
	size_t			myId;
};

//==========================================================================

#endif
