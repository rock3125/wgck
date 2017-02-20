#include <precomp_header.h>

#include <win32/win32.h>

#include <object/geometry.h>
#include <object/landscape.h>
#include <ai/landscapeai.h>

//==========================================================================

TLandscapeAI::TLandscapeAI( void )
	: locations(NULL),
	  paths(NULL),
	  patrolPaths(NULL),
	  airportPaths(NULL),
	  neighbours(NULL)
{
	numLocations = 0;
	numPatrolLocations = 0;
	numAirportLocations = 0;

	bufferSize = 128;
	width = 0;

	locations = new float[bufferSize*3];
	neighbours = new size_t[bufferSize*2];
	signature = "PDVAIMAP01";

	recurseCounter = 0;
}; 

TLandscapeAI::~TLandscapeAI( void )
{
	if ( locations!=NULL )
		delete []locations;
	locations = NULL;

	if ( neighbours!=NULL )
		delete []neighbours;
	neighbours = NULL;

	if ( paths!=NULL )
		delete []paths;
	paths = NULL;

	if ( patrolPaths!=NULL )
		delete []patrolPaths;
	patrolPaths = NULL;

	if ( airportPaths!=NULL )
		delete []airportPaths;
	airportPaths = NULL;
};


float TLandscapeAI::Distance( float x1, float y1, float z1,
							  float x2, float y2, float z2 )
{

	return (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) + (z2-z1)*(z2-z1);
};


float TLandscapeAI::Distance( size_t i, size_t j )
{
	PreCond( paths!=NULL );

	float x1, y1, z1;
	x1 = paths[i].x;
	y1 = paths[i].y;
	z1 = paths[i].z;

	float x2, y2, z2;
	x2 = paths[j].x;
	y2 = paths[j].y;
	z2 = paths[j].z;

	return float(sqrt(Distance(x1,y1,z1, x2,y2,z2)));
};


bool TLandscapeAI::CreateGraph( TString& errStr )
{
	size_t i,j;

	PreCond( numLocations>0 );
	PreCond( width>0 );

	paths = new TConnections[numLocations];

	for ( i=0; i<numLocations; i++ )
	{
		paths[i].x = locations[i*3+0];
		paths[i].y = locations[i*3+1];
		paths[i].z = locations[i*3+2];
	}

	delete []locations;
	locations = NULL;

	for ( i=0; i<numLocations; i++ )
	{
		for ( j=0; j<numLocations; j++ )
		if ( i!=j )
		{
			size_t dx = size_t(abs(neighbours[i*2+0]-neighbours[j*2+0]));
			size_t dz = size_t(abs(neighbours[i*2+1]-neighbours[j*2+1]));
			if ( dx<=1 && dz<=1 )
			if ( paths[i].numConnections<4 )
			{
				paths[i].neighbours[paths[i].numConnections] = j;
				paths[i].numConnections++;
			}
		}
		if ( paths[i].numConnections==0 )
		{
			errStr = "Error: Connection violation at element " + Int2Str(i);
			errStr = errStr + " of " + Int2Str(numLocations) + "\n";
			return false;
		}
	}
	return true;
};


size_t TLandscapeAI::GetClosestNode( float x, float y, float z )
{
	float dist = Distance(x,y,z,paths[0].x,paths[0].y,paths[0].z);
	size_t closest = numLocations;
	for ( size_t i=0; i<numLocations; i++ )
	{
		float d1 = Distance(x,y,z,paths[i].x,paths[i].y,paths[i].z);
		if ( d1 < dist && d1 < 4000 )
		{
			dist = d1;
			closest = i;
		}
	}
	return closest;
};


size_t TLandscapeAI::GetClosestPatrolNode( float x, float y, float z )
{
	size_t closest = 0;
	if ( patrolPaths!=NULL )
	{
		float dist = Distance(x,y,z,patrolPaths[0].x,patrolPaths[0].y,patrolPaths[0].z);
		closest = numLocations;
		for ( size_t i=0; i<numLocations; i++ )
		{
			float d1 = Distance(x,y,z,patrolPaths[i].x,patrolPaths[i].y,patrolPaths[i].z);
			if ( d1 < dist && d1 < 4000 )
			{
				dist = d1;
				closest = i;
			}
		}
	}
	return closest;
};


size_t TLandscapeAI::GetClosestAirportNode( float x, float y, float z )
{
	size_t closest = numAirportLocations;
	if ( airportPaths!=NULL )
	{
		float dist = 2000;
		for ( size_t i=0; i<numAirportLocations; i++ )
		{
			float d1 = Distance(x,y,z,airportPaths[i].x,airportPaths[i].y,airportPaths[i].z);
			if ( d1 < dist && d1 < 2000 )
			{
				dist = d1;
				closest = i;
			}
		}
	}
	return closest;
};


size_t TLandscapeAI::RecursePath( size_t n1, size_t n2, 
							      size_t numNodes, size_t* nodes,
								  size_t recurseCounter )
{
	size_t i,j;

	// goal reached - done!
	if ( n1==n2 )
	{
		if( numNodes==0 )
			return numNodes;
		return numNodes-1;
	}

	// already travelled this way?
	if ( paths[n1].recursionCounter==recurseCounter )
		return 0;
	paths[n1].recursionCounter = recurseCounter;

	nodes[numNodes] = n1;

	// evaluate all possible ways from this node to its neighbours and
	// choose the shorest one to the goal node
	float  dist[4];
	size_t neigh[4];

	size_t cntr = paths[n1].numConnections;
	for ( i=0; i<cntr; i++ )
	{
		size_t n = paths[n1].neighbours[i];
		dist[i] = Distance( paths[n].x,paths[n].y,paths[n].z,
							paths[n2].x,paths[n2].y,paths[n2].z );
		neigh[i] = n;
	}

	// sort them in terms of shorest to furthest
	for ( i=0; i<cntr; i++ )
	for ( j=i+1; j<cntr; j++ )
	if ( i!=j )
	{
		if ( dist[i] > dist[j] )
		{
			size_t n_temp = neigh[i];
			neigh[i] = neigh[j];
			neigh[j] = n_temp;

			float f_temp = dist[i];
			dist[i] = dist[j];
			dist[j] = f_temp;
		}
	}

	// traverse them in this order
	for ( i=0; i<cntr; i++ )
	{
		size_t cnt = RecursePath( neigh[i], n2, numNodes+1, nodes, recurseCounter );
		if ( cnt>0 )
			return cnt;
	}
	return 0;
};


size_t TLandscapeAI::PatrolPath( size_t n1,
								 size_t numNodes, size_t* nodes,
								 size_t recurseCounter )
{
	size_t n = n1;

	if ( patrolPaths[n].numConnections==0 )
		return 0;

	do
	{
		patrolPaths[n].recursionCounter = recurseCounter;
		nodes[numNodes++] = n;
		n = patrolPaths[n].neighbours[0];
	}
	while ( n!=n1 && patrolPaths[n].recursionCounter!=recurseCounter &&
			patrolPaths[n].numConnections>0 );

	return numNodes;
};


size_t TLandscapeAI::AirportPath( size_t n1,
								  size_t numNodes, size_t* nodes,
								  size_t recurseCounter )
{
	numNodes = 0;

	size_t n = n1;
	if ( airportPaths[n].numConnections==0 )
		return 0;

	do
	{
		airportPaths[n].recursionCounter = recurseCounter;
		nodes[numNodes++] = n;
		n = airportPaths[n].neighbours[0];
	}
	while ( airportPaths[n].recursionCounter!=recurseCounter &&
			airportPaths[n].numConnections>0 );

	return numNodes;
};


bool TLandscapeAI::EvaluatePath( float fx,float fy,float fz, 
								 float tx,float ty,float tz,
								 size_t& numNodes, size_t* nodes )
{
	size_t startNode = GetClosestNode( fx,fy,fz );
	if ( startNode==numLocations )
		return false;

	size_t goalNode  = GetClosestNode( tx,ty,tz );
	if ( goalNode==numLocations )
		return false;

	recurseCounter++;
	numNodes = RecursePath( startNode, goalNode, numNodes, nodes, recurseCounter );

	return true;
};


bool TLandscapeAI::EvaluatePatrolPath( float fx,float fy,float fz, 
									   size_t& numNodes, size_t* nodes )
{
	size_t startNode = GetClosestPatrolNode( fx,fy,fz );
	recurseCounter++;
	numNodes = PatrolPath( startNode, numNodes, nodes, recurseCounter );
	return true;
};


bool TLandscapeAI::EvaluateAirportPath( float fx,float fy,float fz, 
										size_t& numNodes, size_t* nodes )
{
	size_t startNode = GetClosestAirportNode( fx,fy,fz );
	if ( startNode==numAirportLocations )
		return false;

	recurseCounter++;
	numNodes = AirportPath( startNode, numNodes, nodes, recurseCounter );
	return (startNode!=numAirportLocations);
};


const TConnections& TLandscapeAI::GetNode( size_t index ) const
{
	PreCond( index < numLocations );
	return paths[index];
};


const TConnections& TLandscapeAI::GetPatrolNode( size_t index ) const
{
	PreCond( index < numPatrolLocations );
	return patrolPaths[index];
};


const TConnections& TLandscapeAI::GetAirportNode( size_t index ) const
{
	PreCond( index < numAirportLocations );
	return airportPaths[index];
};


size_t TLandscapeAI::NumLocations( void ) const
{
	return numLocations;
};


size_t TLandscapeAI::NumPatrolLocations( void ) const
{
	return numPatrolLocations;
};


size_t TLandscapeAI::NumAirportLocations( void ) const
{
	return numAirportLocations;
};


void TLandscapeAI::SetupPaths( TLandscape& landObj )
{
	if ( paths!=NULL )
		delete []paths;

	paths = NULL;
	numLocations = 0;

	const TDefFile* def = landObj.Def();
	if ( def!=NULL )
	{
		size_t i;

		numLocations = def->NumLocations();
		paths = new TConnections[numLocations];
		for ( i=0; i<numLocations; i++ )
		{
			paths[i] = def->Connection(i);
		}

		numPatrolLocations = def->NumPatrolLocations();

		if ( patrolPaths!=NULL )
			delete []patrolPaths;
		patrolPaths = new TConnections[numPatrolLocations];
		for ( i=0; i<numPatrolLocations; i++ )
		{
			patrolPaths[i] = def->PatrolConnection(i);
		}
	}
};

//==========================================================================

