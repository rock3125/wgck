#ifndef __OBJECT_GRAPH_H__
#define __OBJECT_GRAPH_H__

//==========================================================================

class _EXPORT TGraph
{
public:
	TGraph( void );
	TGraph( size_t, size_t*, size_t, float* );
	~TGraph( void );

	// copy operators
	const TGraph& operator=( const TGraph& );
	TGraph( const TGraph& );

	// is this graph valid/usable?
	bool IsValidGraph( void ) const;

	// save a graph to an open file
	bool SaveBinary( TPersist& file );

	// load a graph from an open file
	bool LoadBinary( TPersist& file, TString& errStr ); 

	// get lor given an x and a y, returns true if possible
	bool GetLORTriangle( float x, float& y, float z, int surface, int& lorRef );

	// creates a graph given a valid constructed object
	void CreateSurfaceGraph( void );

private:
	void GetTriangleCenter( size_t surf, float& vx, float& vy, float& vz );
	bool InsideSurface( float x, float &y, float z, size_t surface );
	bool GetExhaustiveSearchTriangle( float x, float& y, float z );
	bool GetLORTriangle(  float x, float &y, float z, int surface, size_t rdepth );
	int  FindNeighbour( size_t orgSurface, size_t vert1, size_t vert2 );

private:
	int*	graph;
	size_t	gnumFaces;
	size_t	gnumVertices;
	size_t*	gsurfaces;
	float*	gvertices;

	int		lorReference;
	int		recursionCounter;
};

//==========================================================================

#endif
