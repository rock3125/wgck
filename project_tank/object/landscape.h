#ifndef __LANDSCAPE_H_
#define __LANDSCAPE_H_

//==========================================================================

#include <object/object.h>
#include <object/compoundObject.h>
#include <object/landscapeObject.h>

//==========================================================================

#define kBridgeSize		4
#define kMaxNumBridges	100

//==========================================================================

class TLandscapeObjectDraw;

class TLandscape : public TObject
{
public:
	TLandscape( void );
	~TLandscape( void );

    // load & save binary routines for fast loading & saving
    virtual bool SaveBinary( const TString& fname );
    virtual bool LoadBinary( const TString& fname, TString& errStr, 
							 const TString& pathname,
							 const TString& texturePath );

	virtual void Draw( float x, float y, float z, 
					   float yAngle, float xAngle,
					   float vx, float vz,
					   bool showBoundingBox = false );

	// locality of reference search (use this one for getting the actual landscape posn
	bool GetLORTriangle( float x, float &y, float z, int surface );
	void DrawTriangle( int surface );

	// divide a landscape mesh into paritions
	bool DivideMeshes( TObject& landscape, size_t xdiv, size_t ydiv, TString& errStr,
					   const TString& fileName, const TString& path, 
					   const TString& texturePath );

	void	RenderDepth( size_t _renderDepth );
	size_t	RenderDepth( void );

	// is it?
	bool	Initialised( void ) const;

	// on bridge?
	bool BridgeHeight( float x, float& y, float z );
	float WaterLevel( void ) const;

	bool InNoGoZone( float x1, float z1,
					 float x2, float z2,
					 float x3, float z3,
					 float x4, float z4 ) const;

	// add a new landscape object to the system
	void			AddLandscapeObject( TLandscapeObject* lnd );
	void			RemoveLandscapeObject( TLandscapeObjectDraw*& list, 
										   TLandscapeObjectDraw* remove );

	// checking for bullets and stuff
	bool			InsideObstacle( float x, float y, float z );

	TLandscapeObject* GetLandscapeObject( const TString& name ) const;

private:
	size_t GetVertex( float* vertices, float* vectorXYZ, size_t& count );
	void DeleteDivisions( void );
	
	// helper functions to CreateSurfaceGraph()
	void CreateSurfaceGraph( void );
	int  FindNeighbour( size_t orgSurface, size_t vert1, size_t vert2 );

	void RemoveDuplicates(  size_t numVertices, float* vertices,
							size_t numFaces, size_t* surfaces );

	// get landscape graph item closest to x,y,z or NULL if none
	void GetTriangleCenter( size_t surf, float& vx, float& vy,float& vz );
	bool GetExhaustiveSearchTriangle( float x, float& y, float z );
	bool InsideSurface( float x, float &y, float z, size_t surface );
	bool InsideMeshSurface( float x, float &y, float z,
							TMesh* mesh, size_t surface );

	void AddBridge( TMesh* mesh );

	bool GetLORTriangle( float x, float &y, float z, int surface, size_t rdepth );

	bool IsObjectName( const TString& str ) const;
	TCompoundObject* LoadLandscapeObject( TString& objname, 
										  TString& errStr,
										  const TString& path,
										  const TString& texturePath );

public:
	// lor return values
	size_t				lorReference;

private:
	size_t				divx;
	size_t				divy;
	size_t				visitedValue;
	size_t				renDepth;
	bool*				circle;
	float				divSizeX;
	float				divSizeY;
	bool				initialised;
	TString				signature;

	float				landscapeminX, landscapeminZ;

	TMesh**					divisions;
	TMesh*					landscapeMesh;
	TLandscapeObjectDraw**	noGoDivisions;

	TLandscapeObjectList	objectList;

	size_t*				surfaceDivisionCounts;
	size_t**			surfaceMaterialFaces;
	size_t**			surfaceDivisions;

	// for directed graph access
	int*				surfaceGraph;
	size_t				gnumSurfaces;
	size_t*				gsurfaces;
	size_t				gnumVertices;
	float*				gvertices;
	int					recursionCounter;

	// bridges
	size_t				numBridges;
	float*				bridges;
	TMesh**				bridgeMeshes;

	// sea stuff
	float				waterLevel;

	// cache for landscape processing routines
	TString*			objectNames;
	TCompoundObject*	objectCache;
	size_t				objectCount;
};


inline
bool TLandscape::Initialised( void ) const
{
	return initialised;
};

//==========================================================================

class TLandscapeObjectDraw
{
public:
	TLandscapeObjectDraw( TLandscapeObject& obj );
	TLandscapeObjectDraw( void );
	TLandscapeObjectDraw( const TLandscapeObjectDraw& );

	const TLandscapeObjectDraw& operator=( const TLandscapeObjectDraw& );

	~TLandscapeObjectDraw( void );

	void Draw( void ) const;

	TLandscapeObjectDraw*	Next( void ) const;
	void		Next( TLandscapeObjectDraw* _next );

	// (x,z) valid?
	bool InNoGoZone( float x1, float z1,
					 float x2, float z2,
					 float x3, float z3,
					 float x4, float z4 ) const;

	bool InsideObject( float x1, float y1, float z1 );

private:
	friend class TLandscape;

	TLandscapeObject* obj;
	TLandscapeObjectDraw* next;
};

//==========================================================================

#endif

