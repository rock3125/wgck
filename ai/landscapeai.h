#ifndef __AI_LANDSCAPEAI_H_
#define __AI_LANDSCAPEAI_H_

//==========================================================================

#include <object/texture.h>
#include <object/deffile.h>

//==========================================================================

class TLandscape;

//==========================================================================
// make sure AIs don't chew all available CPU
// each has a unique Id and all use time-slicing
// accordingly
extern size_t aiId;
extern size_t logicCounter;

//==========================================================================

class TLandscapeAI
{
public:
	TLandscapeAI( void );
	~TLandscapeAI( void );

	// from -> to path
	bool EvaluatePath( float fx,float fy,float fz, 
					   float tx,float ty,float tz,
					   size_t& numNodes, size_t* nodes );

	bool EvaluatePatrolPath( float fx,float fy,float fz, 
							 size_t& numNodes, size_t* nodes );

	bool EvaluateAirportPath( float fx,float fy,float fz, 
							  size_t& numNodes, size_t* nodes );

	const TConnections& GetNode( size_t index ) const;
	const TConnections& GetPatrolNode( size_t index ) const;
	const TConnections& GetAirportNode( size_t index ) const;

	size_t NumLocations( void ) const;
	size_t NumPatrolLocations( void ) const;
	size_t NumAirportLocations( void ) const;

	// setup paths from a landscape object
	void SetupPaths( TLandscape& landObj );

private:
	float Distance( float x1, float y1, float z1,
					float x2, float y2, float z2 );
	float Distance( size_t i, size_t j );

	bool  CreateGraph( TString& errStr );

	// get index of node closest to (x,y,z)
	size_t GetClosestNode( float x, float y, float z );
	size_t GetClosestPatrolNode( float x, float y, float z );
	size_t GetClosestAirportNode( float x, float y, float z );

	size_t RecursePath( size_t n1, size_t n2, 
						size_t count, size_t* nodes,
						size_t recurseCounter );

	size_t PatrolPath( size_t n1,
					   size_t numNodes, size_t* nodes,
					   size_t recurseCounter );

	size_t AirportPath( size_t n1,
						size_t numNodes, size_t* nodes,
						size_t recurseCounter );

private:
	TString signature;

	size_t	numLocations;
	size_t	numPatrolLocations;
	size_t	numAirportLocations;

	size_t	bufferSize;
	float*	locations;

	size_t*	neighbours;

	float	width;

	TConnections*	paths;
	TConnections*	airportPaths;
	TConnections*	patrolPaths;

	size_t	recurseCounter;
};

//==========================================================================

#endif
