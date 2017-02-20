#ifndef __OBJECT_LANDSCAPE_H_
#define __OBJECT_LANDSCAPE_H_

//==========================================================================

#include <object/binobject.h>
#include <object/compoundObject.h>
#include <object/landscapeObject.h>
#include <object/camera.h>
#include <object/tank.h>
#include <object/plane.h>
#include <object/artillery.h>
#include <object/character.h>
#include <object/graph.h>
#include <object/deffile.h>

//==========================================================================

class TApp;
class TPlane;

//==========================================================================

class _EXPORT TLandscape : public TBinObject
{
public:
	TLandscape( void );
	~TLandscape( void );

	// completely clear everything loaded & allocated - reset to zero landscape
	void Clear( void );

	// re-use this map - reload all landscape object to their initial positions
	void ResetLandscapeObjects( void );

	// reload textures of landscape object materials
	bool Reload( TString& errStr );

	// get a status of all the objects on the map for transmitting to clients
	// when they join, so they see a consistent map
	size_t	GetLandscapeObjectStatus( size_t arraySize, byte* array );
	void	SetLandscapeObjectStatus( size_t arraySize, byte* array );

	// update landscape object strengths
	size_t	GetLandscapeObjectUpdateStrengths( byte* data );
	void	UpdateLandscapeObjectStrength( size_t builderId, size_t strength );


    // load & save binary routines for fast loading & saving
	// app is used for accessing initial render depth info
    virtual bool SaveBinary( const TString& fname, TDefFile& def );

	virtual bool LoadBinary( const TString& fname, TString& errStr, 
							 const TString& texturePath,
							 TApp* app );
    virtual bool LoadBinary( const TString& fname, TString& errStr, 
							 const TString& pathname,
							 const TString& texturePath,
							 TApp* app );

	// given a def file and an app, setup the landscape objects on the landscape
	bool SetupLandscapeObjects( TDefFile& def, TApp* app, TString& errStr );

	// draw a landscape from an X,Z position and a given set of angles with transparencies
	virtual void Draw( class TCamera& cam, float vx, float vz, bool drawTransparencies );
	virtual void Draw( float camX, float camY, float camZ, 
					   float camYangle, float camXangle,
					   float vx, float vz, bool drawTransparencies );

	virtual void DrawNoObj( bool drawTransparencies );

	bool SetFuelAmmo( size_t _numFuel, size_t _numAmmo, TString& errStr );

	// divide a landscape mesh into paritions (create landscape)
	bool DivideMeshes( TBinObject& landscape, TBinObject& smallMap, 
					   size_t xdiv, size_t ydiv, TString& errStr,
					   const TString& fileName, const TString& path, 
					   const TString& texturePath );

	// render depth set - adjust drawing radius
	void	RenderDepth( size_t _renderDepth );
	size_t	RenderDepth( void );

	// is this landscape Object initialised? (valid)
	bool	Initialised( void ) const;
	void	Initialised( bool );

	// how high is the water in this landscape
	float WaterLevel( void ) const;

	// checking for bullets and stuff
	size_t			InsideObstacle( float x, float y, float z );


//	size_t			InsideObstacle( TTank& v ); // fuel and ammo checking
	size_t			ObjCollision( TTank& v ) const;
	bool			GetLORTriangle( TTank& tank );

	void			UseFuel( size_t objId );
	void			UseAmmo( size_t objId );
	bool			IsAmmo( size_t objId );
	bool			IsFuel( size_t objId );

	// get the an object on the landscape by name or id
	TLandscapeObject* GetLandscapeObject( const TString& name ) const;
	TLandscapeObject* GetLandscapeObject( size_t id ) const;

	// collision detection, can we move from l1 to l2 without hitting
	// any object?
//	bool Collision( TTank& v );
	bool Collision( TTank& v, size_t& objId );
	bool Collision( TCharacter& ch );
	bool Collision( TArtillery& v );
	bool Collision( float l1x, float l1y, float l1z,
				    float l2x, float l2y, float l2z );
	bool Collision( float l1x, float l1y, float l1z,
				    float l2x, float l2y, float l2z, size_t& objId );
	bool Collision( TPlane& v );

	size_t	ObjCollision( TPlane& v ) const;
	bool	ObjCollision( float l1x, float l1y, float l1z,
						  float l2x, float l2y, float l2z, size_t& objId ) const;
	bool	ObjCollision( const TPoint& p1, const TPoint& p2, size_t& objId ) const;

	// surface collision mapping - returns objId>0 if hit valid object
	bool GetLORTriangle( TPlane& v );
	bool GetLORTriangle( TCharacter& ch );
	bool GetLORTriangle( TArtillery& v );
	bool GetLORTriangle( float x, float& y, float z, bool character = false );
	bool GetLORTriangle( float* xs, float* ys, float* zs );

	// put the landscape object identified by objectId into its next
	// graphic damage state
	void DamageLandscapeObject( size_t objectId );

	// slice size (X,Y) * renderDepth = view depth
	float	DivSizeX( void ) const;
	float	DivSizeY( void ) const;

	// returns the mesh that is the landscape
	// used for setting initial positions and moving the tank across landscape
	// also used for getting SizeX and SizeZ for map coordinates
	TMesh*	LandscapeMesh( void ) const;

	// set/get def
	TDefFile* Def( void ) const;
	void Def( const TDefFile& _def );

	// get/set landscape map player info - used for creating landscapes
	TMapPlayerInfo*		PlayerInfo( void ) const;
	void				PlayerInfo( TMapPlayerInfo* pi );

	// given an id, team, return the player's initial position
	TLandscapeObject GetPlayerPosition( size_t team, size_t playerId );
	TLandscapeObject GetPlayerPositionNoTeams( size_t playerId, size_t& team );
	TLandscapeObject GetSinglePlayer( size_t& team );

	// given an island filename (complete with path), return errStr or
	// the name of this island
	bool GetIslandDefn( const TString& fname, TDefFile& defn, TString& errStr );

	// return small map object
	TBinObject* SmallMap( void ) const;

	float DivX( void ) const;
	float DivZ( void ) const;

	// radius size in pixels
	float PixelRadius( void ) const;

	// get a y given an (x,z) from the landscapeMesh
	void GetY( float x, float& y, float z );
	void GetBestY( float x, float& y, float z );

	// for landscape objects
	void CalculateExplosionDamage( TApp* app, size_t id, size_t damageFactor );
	void CalculateExplosionDamage( TApp* app, const TPoint& point, float damageFactor );
	void CalculateExplosionDamage( TApp* app, float expx, float expy, float expz,
								   float damageFactor );

	// return a list and its size of all landscape objects that either
	// refuel, rearm, or repair.  if list==NULL return size only
	size_t GetResourceObjects( TLandscapeObject** list );

	// set sea surface colour
	void SetSeaSurfaceColour( float r, float g, float b );

private:
	friend class TBinObject;
	friend class TTank;
	friend class TPlane;
	friend class TArtillery;

	friend TLandscape* CreateLandscapeObject( const TDefFile& def, TString& errStr );

	bool LandscapeCollision( float l1x, float l1y, float l1z, 
							 float size, float& ly ) const;

	bool GetLORTriangle( float xp, float& yp, float zp, float maxy, float size );

	// add a new landscape object to the system
	void			AddLandscapeObject( TLandscapeObject* lnd );
	void			ClearLandscapeObjects( void );

	size_t GetVertex( float* vertices, float* vectorXYZ, size_t& count );
	void DeleteDivisions( void );
	
	bool IsObjectName( const TString& str ) const;

	// helper function
	void _AddLandscapeObject( size_t x, size_t y, TLandscapeObject* lnd );

private:
	size_t				divx;
	size_t				divz;
	size_t				visitedValue;
	size_t				renDepth;
	bool*				circle;
	float				divSizeX;
	float				divSizeY;
	bool				initialised;
	TString				signature;

	bool				renewableAmmo;
	bool				renewableFuel;

	float				landscapeminX, landscapeminZ;

	TBinObject*				smallMap;
	TMesh*					zeroSeaSlice; // for drawing blank areas outside the map
	TMesh*					zeroSurfaceSlice;

	float					seaSurfaceR;
	float					seaSurfaceG;
	float					seaSurfaceB;

	bool					owner; // own binobject?

	TMesh**					divisions;
	TMesh*					landscapeMesh;
	TLandscapeObjectDraw**	landscapeObjectDivisions;

	size_t*				surfaceDivisionCounts;
	size_t**			surfaceMaterialFaces;
	size_t**			surfaceDivisions;

	// sea stuff
	float				waterLevel;

	// cache for landscape processing routines
	TLandscapeObject**	landscapeObjectLookup;
	size_t				landscapeObjectCount;
	size_t				landscapeObjectCounter; // drawing recursion protection
	size_t*				landscapeObjectDrawn;	// drawing counters, both setup by LoadBinary

	// graph stuff
	size_t				gnumSurfaces;
	size_t*				gsurfaces;
	size_t				gnumVertices;
	float*				gvertices;

	// player position & team info
	TMapPlayerInfo		playerInfo;
	TDefFile*			def;

	// ammo and fuel items
	size_t		numAmmo;
	size_t		numFuel;

	// zero slice offset
	float		zeroMinz;
	float		zeroMinx;

	// radius size in pixels for single slice
	float		divRadiusSize;
};

//==========================================================================

#endif

