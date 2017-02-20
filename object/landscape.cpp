#include <precomp_header.h>

#include <win32/win32.h>
#include <object/binobject.h>
#include <object/material.h>
#include <object/landscape.h>
#include <object/mesh.h>
#include <object/geometry.h>

#include <tank/tankapp.h>


#define _ALTERNATESEA


//==========================================================================

const float degToRad = 0.01745329252f;

//==========================================================================

#if defined(_EDITOR) || defined(_PARSER) || defined(_VIEWER) || defined(_SPEEDTEST)
#define _DONTUSE
#endif

//==========================================================================

TLandscape::TLandscape( void )
	: divisions(NULL),
	  landscapeObjectDivisions(NULL),
	  divx(0),
	  divz(0),
	  divRadiusSize(0),
	  divSizeX(0.0f),
	  divSizeY(0.0f),
	  gsurfaces(NULL),
	  gvertices(NULL),
	  gnumSurfaces(0),
	  gnumVertices(0),
	  visitedValue(0),
	  surfaceDivisionCounts(NULL),
	  surfaceDivisions(NULL),
	  surfaceMaterialFaces(NULL),
	  circle(NULL),
	  landscapeObjectLookup(NULL),
	  landscapeObjectDrawn(NULL),
	  def(NULL),
	  initialised(false),
	  smallMap(NULL),
	  zeroSeaSlice(NULL),
	  zeroSurfaceSlice(NULL),

	  landscapeMesh(NULL)
{
	numAmmo = 0;
	numFuel = 0;

	signature = "PDVLND21";
	renDepth = 0;
	waterLevel = 0.0f;

	owner = true;

	landscapeminX = 0;
	landscapeminZ = 0;
	landscapeObjectCount = 0;

	zeroMinz = zeroMinx = 0;

	renewableAmmo = false;
	renewableFuel = false;

	seaSurfaceR = 0.285f;
	seaSurfaceG = 0.306f;
	seaSurfaceB = 0.306f;
};


TLandscape::~TLandscape( void )
{
	Clear();
};


TDefFile* TLandscape::Def( void ) const
{
	return def;
};


void TLandscape::Def( const TDefFile& _def )
{
	if ( def!=NULL )
		delete def;
	def = new TDefFile( _def );
};


void TLandscape::Clear( void )
{
	if ( def!=NULL )
		delete def;
	def = NULL;

	if ( circle!=NULL )
		delete circle;
	circle = NULL;

	initialised = false;
	DeleteDivisions();

	divx = 0;
	divz = 0;
	divSizeX = 0.0f;
	divSizeY = 0.0f;
	gsurfaces = NULL;
	gvertices = NULL;
	gnumSurfaces = 0;
	gnumVertices = 0;
	visitedValue = 0;
	landscapeObjectCount = 0;
	surfaceDivisionCounts = NULL;
	surfaceDivisions = NULL;
	surfaceMaterialFaces = NULL;

	if ( owner && smallMap!=NULL )
		delete smallMap;
	smallMap = NULL;

	if ( zeroSeaSlice!=NULL )
		delete zeroSeaSlice;
	zeroSeaSlice = NULL;

	if ( zeroSurfaceSlice!=NULL )
		delete zeroSurfaceSlice;
	zeroSurfaceSlice = NULL;

	if ( landscapeMesh!=NULL )
		delete landscapeMesh;
	landscapeMesh = NULL;
};


void TLandscape::ResetLandscapeObjects( void )
{
	// remove all existing from the map
	ClearLandscapeObjects();

	for ( size_t i=0; i<landscapeObjectCount; i++ )
	{
		TLandscapeObject* lo = landscapeObjectLookup[i];
		// reset my index
		lo->CurrentIndex(0);

		// re-insert into the landscape
		AddLandscapeObject( lo );
	}
};


size_t TLandscape::GetLandscapeObjectStatus( size_t arraySize, byte* data )
{
	PreCond( arraySize>landscapeObjectCount );

	for ( size_t i=0; i<landscapeObjectCount; i++ )
	{
		TLandscapeObject* lo = landscapeObjectLookup[i];
		data[i] = byte(lo->Strength());
	}
	return landscapeObjectCount;
};


size_t TLandscape::GetLandscapeObjectUpdateStrengths( byte* data )
{
#if !defined(_DONTUSE)
	DataBuildingStrengthUpdate up;

	size_t cntr = 0;
	bool finished = false;

	for ( size_t i=0; i<landscapeObjectCount && !finished; i++ )
	{
		TLandscapeObject* lo = landscapeObjectLookup[i];
		if ( lo->StrengthUpdated() )
		{
			finished = !up.Add( lo->Id(), lo->Strength() );
			if ( !finished )
			{
				cntr++;
			}
		}
	}
	if ( cntr>0 )
	{
		size_t s = up.Set(data);
		return s;
	}
#endif
	return 0;
};


// return a list and its size of all landscape objects that either
// refuel, rearm, or repair
size_t TLandscape::GetResourceObjects( TLandscapeObject** list )
{
	size_t cntr = 0;
	for ( size_t i=0; i<landscapeObjectCount; i++ )
	{
		TLandscapeObject* lo = landscapeObjectLookup[i];
		if ( lo->RefuelStation() || lo->Armory() || lo->Repairs() )
		{
			if ( list!=NULL )
			{
				list[cntr] = lo;
			}
			cntr++;
		}
	}
	return cntr;
};


void TLandscape::SetLandscapeObjectStatus( size_t arraySize, byte* data )
{
	PreCond( arraySize==landscapeObjectCount );

	for ( size_t i=0; i<landscapeObjectCount; i++ )
	{
		TLandscapeObject* lo = landscapeObjectLookup[i];
		lo->Strength( size_t(data[i]) );
	}
};

void TLandscape::UpdateLandscapeObjectStrength( size_t builderId, size_t strength )
{
	PreCond( builderId < landscapeObjectCount );
	TLandscapeObject* lo = landscapeObjectLookup[builderId];
	lo->Strength( strength );
};

void TLandscape::DeleteDivisions( void )
{
	if ( divisions!=NULL )
	{
		for ( size_t x=0; x<divx; x++ )
		for ( size_t y=0; y<divz; y++ )
			if ( divisions[y*divx+x]!=NULL )
			{
				delete divisions[y*divx+x];
				divisions[y*divx+x] = NULL;
			}
		delete []divisions;
		divisions = NULL;
	}

	if ( landscapeObjectDivisions!=NULL )
	{
		ClearLandscapeObjects();
		delete []landscapeObjectDivisions;
		landscapeObjectDivisions = NULL;
	}

	if ( surfaceDivisionCounts!=NULL )
		delete surfaceDivisionCounts;
	surfaceDivisionCounts = NULL;

	if ( surfaceDivisions!=NULL )
	{
		for ( size_t x=0; x<divx; x++ )
		for ( size_t y=0; y<divz; y++ )
			if ( surfaceDivisions[y*divx+x]!=NULL )
			{
				delete []surfaceDivisions[y*divx+x];
				surfaceDivisions[y*divx+x] = NULL;
			}
		delete []surfaceDivisions;
		surfaceDivisions = NULL;
	}

	if ( surfaceMaterialFaces!=NULL )
	{
		for ( size_t x=0; x<divx; x++ )
		for ( size_t y=0; y<divz; y++ )
			if ( surfaceMaterialFaces[y*divx+x]!=NULL )
			{
				delete surfaceMaterialFaces[y*divx+x];
				surfaceMaterialFaces[y*divx+x] = NULL;
			}
		delete []surfaceMaterialFaces;
		surfaceMaterialFaces = NULL;
	}

	gsurfaces = NULL;
	gnumSurfaces = 0;
	gvertices = NULL;
	gnumVertices = 0;

	if ( landscapeObjectLookup!=NULL )
	{
		for ( size_t i=0; i<(landscapeObjectCount+numAmmo+numFuel); i++ )
			delete landscapeObjectLookup[i];
		delete []landscapeObjectLookup;
	}
	landscapeObjectCount = 0;
	landscapeObjectLookup = NULL;

	if ( landscapeObjectDrawn!=NULL )
		delete landscapeObjectDrawn;
	landscapeObjectDrawn = NULL;
}









bool TLandscape::GetLORTriangle( TTank& v )
{
	float size = landscapeMesh->SizeY() * 2;

	float height = 1.5f;
/*	
	float ys[4];
	float xs[4];
	float zs[4];

	xs[0] = v.n_x1;
	xs[1] = v.n_x2;
	xs[2] = v.n_x3;
	xs[3] = v.n_x4;

	ys[0] = v.y1 + height;
	ys[1] = v.y2 + height;
	ys[2] = v.y3 + height;
	ys[3] = v.y4 + height;

	zs[0] = v.n_z1;
	zs[1] = v.n_z2;
	zs[2] = v.n_z3;
	zs[3] = v.n_z4;

	GetLORTriangle( xs,ys,zs );
*/
	GetLORTriangle( v.n_x1, v.n_y1, v.n_z1, v.n_y1+height, size );
	GetLORTriangle( v.n_x2, v.n_y2, v.n_z2, v.n_y2+height, size );
	GetLORTriangle( v.n_x3, v.n_y3, v.n_z3, v.n_y3+height, size );
	GetLORTriangle( v.n_x4, v.n_y4, v.n_z4, v.n_y4+height, size );

/*
	v.n_y1 = ys[0];
	v.n_y2 = ys[1];
	v.n_y3 = ys[2];
	v.n_y4 = ys[3];

	v.n_y = (ys[0]+ys[1]+ys[2]+ys[3]) * 0.25f;
*/
	v.n_y = (v.n_y1+v.n_y2+v.n_y3+v.n_y4) * 0.25f;

	return true;
};

bool TLandscape::Collision( TTank& v, size_t& objId )
{
	float height = 0.3f;
	float d = (v.x1-v.n_x1)*(v.x1-v.n_x1) + (v.y1-v.n_y1)*(v.y1-v.n_y1) + 
			  (v.z1-v.n_z1)*(v.z1-v.n_z1);
	if ( d>0 )
	if ( Collision( v.n_x1,v.n_y1+height,v.n_z1, v.n_x2,v.n_y2+height,v.n_z2, objId ) ||
		 Collision( v.n_x2,v.n_y2+height,v.n_z2, v.n_x3,v.n_y3+height,v.n_z3, objId ) ||
		 Collision( v.n_x3,v.n_y3+height,v.n_z3, v.n_x4,v.n_y4+height,v.n_z4, objId ) ||
		 Collision( v.n_x4,v.n_y4+height,v.n_z4, v.n_x1,v.n_y1+height,v.n_z1, objId ) )
		return true;
	return false;
};

















bool TLandscape::GetLORTriangle( TPlane& v )
{
	float size = landscapeMesh->SizeY() * 4;
	float height = 2.5f;
	return GetLORTriangle( v.n_x, v.n_y, v.n_z, v.y+height, size );
/*
	float size = landscapeMesh->SizeY() * 4;
	float height = 2.5f;

	GetLORTriangle( v.n_x1, v.n_y1, v.n_z1, v.n_y1+height, size );
	GetLORTriangle( v.n_x2, v.n_y2, v.n_z2, v.n_y2+height, size );
	GetLORTriangle( v.n_x3, v.n_y3, v.n_z3, v.n_y3+height, size );
	GetLORTriangle( v.n_x4, v.n_y4, v.n_z4, v.n_y4+height, size );

	v.n_y = (v.n_y1+v.n_y2+v.n_y3+v.n_y4) * 0.25f;

	return true;
*/
};


bool TLandscape::GetLORTriangle( TArtillery& v )
{
	float size = landscapeMesh->SizeY() * 4;
	float height = 2.5f;
	return GetLORTriangle( v.n_x, v.n_y, v.n_z, v.y+height, size );
};


bool TLandscape::GetLORTriangle( TCharacter& ch )
{
	return GetLORTriangle( ch.n_x, ch.n_y, ch.n_z, !ch.jumping );
};


bool TLandscape::GetLORTriangle( float xp, float& yp, float zp, bool character )
{
	float maxy;
	if ( character )
		maxy = yp+1;
	else
		maxy = landscapeMesh->MaxY();

	float xpos = (xp-landscapeminX) / divSizeX;
	float zpos = (zp-landscapeminZ) / divSizeY;

	if ( xpos<0 || zpos<0 || size_t(xpos)>=divx || size_t(zpos)>=divz )
	{
		yp = waterLevel - 2;
		return true;
	}

	float size = landscapeMesh->SizeY() * 2;
	float dist = -10000;
	bool found = LandscapeCollision( xp, maxy, zp, size, dist );
	if ( found )
	{
		yp = dist;
	}

	for ( int xo=-1; xo<=1; xo++ )
	for ( int zo=-1; zo<=1; zo++ )
	{
		int x = int(xpos) + xo;
		int z = int(zpos) + zo;
		if ( x>=0 && z>=0 && x<int(divx) && z<int(divz) )
		if ( landscapeObjectDivisions[z*divx+x]!=NULL )
		{
			TLandscapeObjectDraw* ngd = landscapeObjectDivisions[z*divx+x];
			while ( ngd!=NULL )
			{
				if ( ngd->InsideObject(xp,zp) )
				{
					float tempy;
					if ( ngd->GetLORTriangle(xp,maxy,zp,-size, tempy ) )
					{
						if ( tempy > dist )
						{
							found = true;
							yp = tempy;
							dist = tempy;
						}
					}
				}
				ngd = ngd->Next();
			}
		}
	}
	if ( !found )
	{
		yp = waterLevel - 2;
	}
	return true;
};


bool TLandscape::GetLORTriangle( float xp, float& yp, float zp, float maxy, float size )
{
	float xpos = (xp-landscapeminX) / divSizeX;
	float zpos = (zp-landscapeminZ) / divSizeY;

	if ( xpos<0 || zpos<0 || size_t(xpos)>=divx || size_t(zpos)>=divz )
	{
		yp = waterLevel;
		return true;
	}

	float dist = -10000;
	bool found = LandscapeCollision( xp, maxy, zp, size, dist );
	if ( found )
	{
		yp = dist;
	}

	for ( int xo=-1; xo<=1; xo++ )
	for ( int zo=-1; zo<=1; zo++ )
	{
		int x = int(xpos) + xo;
		int z = int(zpos) + zo;
		if ( x>=0 && z>=0 && x<int(divx) && z<int(divz) )
		if ( landscapeObjectDivisions[z*divx+x]!=NULL )
		{
			TLandscapeObjectDraw* ngd = landscapeObjectDivisions[z*divx+x];
			while ( ngd!=NULL )
			{
				if ( ngd->InsideObject(xp,zp) )
				{
					float tempy;
					if ( ngd->GetLORTriangle(xp,maxy,zp,-size, tempy ) )
					{
						if ( tempy > dist )
						{
							found = true;
							yp = tempy;
							dist = tempy;
						}
					}
				}
				ngd = ngd->Next();
			}
		}
	}
	if ( !found )
	{
		yp = waterLevel;
	}
	return true;
};


bool TLandscape::Collision( TPlane& v )
{
	float height = 0.5f;
	float d = (v.x-v.n_x)*(v.x-v.n_x) + (v.y-v.n_y)*(v.y-v.n_y) + (v.z-v.n_z)*(v.z-v.n_z);
	if ( d>0 )
	{
		// get plane's bounds
		float x1,y1,z1, x2,y2,z2, x3,y3,z3, x4,y4,z4;
		v.Bounds( x1,y1,z1, x2,y2,z2, x3,y3,z3, x4,y4,z4 );

		float p1x,p1y,p1z, p2x,p2y,p2z;

		p1x = (x1+x2) * 0.5f;
		p1y = (y1+y2) * 0.5f;
		p1z = (z1+z2) * 0.5f;

		p2x = (x3+x4) * 0.5f;
		p2y = (y3+y4) * 0.5f;
		p2z = (z3+z4) * 0.5f;

		float p3x,p3y,p3z, p4x,p4y,p4z;

		p3x = (x1+x4) * 0.5f;
		p3y = (y1+y4) * 0.5f;
		p3z = (z1+z4) * 0.5f;

		p4x = (x2+x3) * 0.5f;
		p4y = (y2+y3) * 0.5f;
		p4z = (z2+z3) * 0.5f;

		if ( Collision( p1x,p1y+height,p1z, p2x,p2y+height,p2z ) ||
			 Collision( p3x,p3y+height,p3z, p4x,p4y+height,p4z ) )
			 return true;
	}
	return false;
};


bool TLandscape::Collision( TArtillery& v )
{
	float height = 0.5f;
	float d = (v.x-v.n_x)*(v.x-v.n_x) + (v.y-v.n_y)*(v.y-v.n_y) + (v.z-v.n_z)*(v.z-v.n_z);
	if ( d>0 )
	{
		// get plane's bounds
		float x1,y1,z1, x2,y2,z2, x3,y3,z3, x4,y4,z4;
		v.Bounds( x1,y1,z1, x2,y2,z2, x3,y3,z3, x4,y4,z4 );

		float p1x,p1y,p1z, p2x,p2y,p2z;

		p1x = (x1+x2) * 0.5f;
		p1y = (y1+y2) * 0.5f;
		p1z = (z1+z2) * 0.5f;

		p2x = (x3+x4) * 0.5f;
		p2y = (y3+y4) * 0.5f;
		p2z = (z3+z4) * 0.5f;

		float p3x,p3y,p3z, p4x,p4y,p4z;

		p3x = (x1+x4) * 0.5f;
		p3y = (y1+y4) * 0.5f;
		p3z = (z1+z4) * 0.5f;

		p4x = (x2+x3) * 0.5f;
		p4y = (y2+y3) * 0.5f;
		p4z = (z2+z3) * 0.5f;

		if ( Collision( p1x,p1y+height,p1z, p2x,p2y+height,p2z ) ||
			 Collision( p3x,p3y+height,p3z, p4x,p4y+height,p4z ) )
			 return true;
	}
	return false;
};

bool TLandscape::Collision( TCharacter& ch )
{
	float height = 0.46f;
	size_t objId;
	if ( ch.x!=ch.fwd_x || ch.y!=ch.n_y || ch.z!=ch.fwd_z )
	if ( Collision( ch.x,ch.y+height,ch.z, ch.fwd_x,ch.n_y+height,ch.fwd_z, objId ) )
		return true;
	return false;
};


bool TLandscape::Collision( float l1x, float l1y, float l1z,
						    float l2x, float l2y, float l2z )
{
	float xpos = (l1x-landscapeminX) / divSizeX;
	float zpos = (l1z-landscapeminZ) / divSizeY;

	landscapeObjectCounter++;

	for ( int xo=-1; xo<=1; xo++ )
	for ( int zo=-1; zo<=1; zo++ )
	{
		int x = int(xpos) + xo;
		int z = int(zpos) + zo;
		if ( x>=0 && z>=0 && x<int(divx) && z<int(divz) )
		if ( landscapeObjectDivisions[z*divx+x]!=NULL )
		{
			TLandscapeObjectDraw* ngd = landscapeObjectDivisions[z*divx+x];
			while ( ngd!=NULL )
			{
				if ( ngd->InsideObject(l1x,l1z) ||
					 ngd->InsideObject(l2x,l2z) )
				{
					size_t id = ngd->obj->Id() - 1;
					if ( landscapeObjectDrawn[id]!=landscapeObjectCounter )
					{
						landscapeObjectDrawn[id] = landscapeObjectCounter;
						if ( ngd->Collision(l1x,l1y,l1z, l2x,l2y,l2z) )
						{
							return true;
						}
					}
				}
				ngd = ngd->Next();
			}
		}
	}
	return false;
};


bool TLandscape::Collision( float l1x, float l1y, float l1z,
						    float l2x, float l2y, float l2z, size_t& objId )
{
	objId = 0;

	float xpos = (l1x-landscapeminX) / divSizeX;
	float zpos = (l1z-landscapeminZ) / divSizeY;

	landscapeObjectCounter++;

	for ( int xo=-1; xo<=1; xo++ )
	for ( int zo=-1; zo<=1; zo++ )
	{
		int x = int(xpos) + xo;
		int z = int(zpos) + zo;
		if ( x>=0 && z>=0 && x<int(divx) && z<int(divz) )
		if ( landscapeObjectDivisions[z*divx+x]!=NULL )
		{
			TLandscapeObjectDraw* ngd = landscapeObjectDivisions[z*divx+x];
			while ( ngd!=NULL )
			{
				if ( ngd->InsideObject(l1x,l1z) ||
					 ngd->InsideObject(l2x,l2z) )
				{
					size_t id = ngd->obj->Id() - 1;
					if ( landscapeObjectDrawn[id]!=landscapeObjectCounter )
					{
						if ( ngd->Collision(l1x,l1y,l1z, l2x,l2y,l2z) )
						{
							objId = ngd->obj->Id();
							return true;
						}
					}
				}
				ngd = ngd->Next();
			}
		}
	}
	return false;
};


bool TLandscape::ObjCollision( const TPoint& p1, const TPoint& p2, size_t& objId ) const
{
	return ObjCollision( p1.x,p1.y,p1.z, p2.x,p2.y,p2.z, objId );
};


bool TLandscape::ObjCollision( float l1x, float l1y, float l1z,
							   float l2x, float l2y, float l2z, size_t& objId ) const
{
	objId = 0;

	float xpos = (l1x-landscapeminX) / divSizeX;
	float zpos = (l1z-landscapeminZ) / divSizeY;

	for ( int xo=-1; xo<=1; xo++ )
	for ( int zo=-1; zo<=1; zo++ )
	{
		int x = int(xpos) + xo;
		int z = int(zpos) + zo;
		if ( x>=0 && z>=0 && x<int(divx) && z<int(divz) )
		if ( landscapeObjectDivisions[z*divx+x]!=NULL )
		{
			TLandscapeObjectDraw* ngd = landscapeObjectDivisions[z*divx+x];
			while ( ngd!=NULL )
			{
				if ( ngd->InsideObject( l1x,l1y,l1z, l2x,l2y,l2z ) )
				if ( ngd->Collision( l1x,l1y,l1z, l2x,l2y,l2z ) )
				{
					objId = ngd->obj->Id();
					return true;
				}
				ngd = ngd->Next();
			}
		}
	}
	return false;
};


bool TLandscape::LandscapeCollision( float l1x, float l1y, float l1z, 
									 float size, float& ly ) const
{
	float xpos = (l1x-landscapeminX) / divSizeX;
	float zpos = (l1z-landscapeminZ) / divSizeY;

	// make sure to include neighbouring sections due to cut-offs
	// and find the highest y for now (ok for landscapes)
	float dist = -10000;
	bool found = false;
	for ( int xo=-1; xo<=1; xo++ )
	for ( int zo=-1; zo<=1; zo++ )
	{
		int x = int(xpos) + xo;
		int z = int(zpos) + zo;
		if ( x>=0 && z>=0 && x<int(divx) && z<int(divz) )
		if ( surfaceDivisions[z*divx+x]!=NULL )
		{
			size_t numFaces = surfaceDivisionCounts[z*divx+x];
			size_t* surfaces = surfaceDivisions[z*divx+x];

			// dist below was -size... didn't work - was 30 at max height
			float tempy;
			if ( landscapeMesh->Collision( numFaces, surfaces, l1x,l1y,l1z, dist, tempy ) )
			{
				found = true;
				if ( tempy > dist )
				{
					dist = tempy;
					ly = tempy;
				}
			}
		}
	}
	return found;
};


bool TLandscape::SaveBinary( const TString& fname, TDefFile& def )
{
	size_t x,y;
	TPersist file(fileWrite);

	if ( !initialised )
		return false;

	if ( !file.FileOpen( fname ) )
		return false;

    // write signature to file
	file.FileWrite( signature.c_str(), signature.length()+1 );

	// write defn of landscape for fast reference
	def.SaveBinary( file );

    int num = NumMaterials();
    file.FileWrite( &num, sizeof(int) );
	num = divx;
    file.FileWrite( &num, sizeof(int) );
	num = divz;
    file.FileWrite( &num, sizeof(int) );

	file.FileWrite(&landscapeminX,sizeof(float));
	file.FileWrite(&landscapeminZ,sizeof(float));
	file.FileWrite(&divSizeX,sizeof(float));
	file.FileWrite(&divSizeY,sizeof(float));

	float temp;
	temp = MinX();
	file.FileWrite(&temp,sizeof(float));
	temp = MinY();
	file.FileWrite(&temp,sizeof(float));
	temp = MinZ();
	file.FileWrite(&temp,sizeof(float));
	temp = MaxX();
	file.FileWrite(&temp,sizeof(float));
	temp = MaxY();
	file.FileWrite(&temp,sizeof(float));
	temp = MaxZ();
	file.FileWrite(&temp,sizeof(float));
	temp = CenterX();
	file.FileWrite(&temp,sizeof(float));
	temp = CenterY();
	file.FileWrite(&temp,sizeof(float));
	temp = CenterZ();
	file.FileWrite(&temp,sizeof(float));

    TMaterialList* ml2 = materialList;
    while ( ml2!=NULL )
    {
    	ml2->material.SaveBinary(file);
    	ml2 = ml2->next;
    }

	size_t valid;
	for ( y=0; y<divz; y++ )
	for ( x=0; x<divx; x++ )
    {
		if ( divisions[y*divx+x]==NULL )
		{
			valid = 0;
			file.FileWrite( &valid, sizeof(size_t) );
		}
		else
		{
			valid = 1;
			file.FileWrite( &valid, sizeof(size_t) );
    		divisions[y*divx+x]->SaveBinary(file,this);
		}
    }

	// save landscapeMesh
	landscapeMesh->SaveBinary( file, this );

	PostCond( smallMap!=NULL );
	smallMap->SaveBinary( file );

	// save landscape surface divisions
	for ( y=0; y<divz; y++ )
	for ( x=0; x<divx; x++ )
    {
		file.FileWrite( &surfaceDivisionCounts[y*divx+x], sizeof(size_t) );
		if ( surfaceDivisionCounts[y*divx+x]>0 )
		{
			file.FileWrite( surfaceDivisions[y*divx+x], surfaceDivisionCounts[y*divx+x]*3*sizeof(size_t) );
			size_t temp = surfaceDivisionCounts[y*divx+x];
			if ( surfaceMaterialFaces[y*divx+x]==NULL )
				temp = 0;
			file.FileWrite( &temp, sizeof(size_t) );
			if ( temp>0 )
				file.FileWrite( surfaceMaterialFaces[y*divx+x], temp*sizeof(size_t) );
		}
    }

	// sea
	file.FileWrite( &waterLevel, sizeof(float) );
   	file.FileClose();
    
	return true;
};


bool TLandscape::GetIslandDefn( const TString& fname, 
							    TDefFile& defn,
							    TString& errStr )
{
	TPersist file(fileRead);
	if ( !file.FileOpen( fname ) )
    {
    	errStr = "Error opening file \"" + fname;
		errStr = errStr + "\"";
    	return false;
    }

    // check signature
	char ch;
	bool ok;
	TString sig;
	do
	{
		ok = file.FileRead(&ch,1);
		if ( ok && ch!=0 )
			sig = sig + TString(ch);
	}
	while ( ch!=0 && ok );

    if ( sig!=signature || !ok )
    {
    	errStr = "Error incorrect file type or version \"" + fname;
		errStr = errStr + "\" (?" + sig + ")";
    	return false;
    }

	// read island defn
	if ( !defn.LoadBinary( file, errStr ) )
	{
		file.FileClose();
		return false;
	}
	file.FileClose();
/*
	TPersist file2(fileWrite);
	file2.FileOpen( "exp.def" );
	defn.SaveBinary( file2 );
	file2.FileClose();
*/
	return true;
};


bool TLandscape::SetupLandscapeObjects( TDefFile& def, TApp* app, TString& errStr )
{
	size_t i;

	if ( app!=NULL )
	{
		// allocate cache for landscape objects
		landscapeObjectCount = def.NumLandscapeObjects();
		landscapeObjectLookup = new TLandscapeObject*[landscapeObjectCount];
		landscapeObjectDrawn = new size_t[landscapeObjectCount];
		landscapeObjectCounter = 0;
		size_t cntr = 0;
#if !defined(_DONTUSE)
		app->ResetGameObjects();
#endif
		for ( i=0; i<landscapeObjectCount; i++ )
		{
			TLandscapeObject obj = def.LandscapeObjects(i);

			// is it a tank?
			bool transport = false;
#if !defined(_DONTUSE)

			TCompoundObject* cobj = app->GetObjectByName( obj.Name().lcase(), i, landscapeObjectCount );
			if ( cobj==NULL )
			{
				errStr = "Error: landscape object \"" + obj.Name() + "\" not found";
				return false;
			}
			PostCond( cobj->Objects()!=NULL );
			obj.Object( cobj );
			if ( app!=NULL )
			{
				if ( TLandscapeObject::IsV2(obj.Name()) )
				{
					TLandscapeObject obj2 = def.LandscapeObjects(i+1);
					PreCond( TLandscapeObject::IsV2TargetMarker( obj2.Name() ) );
					transport = app->AddTransport( obj, obj2 );
				}
				else
				{
					transport = app->AddTransport( obj );
				}
			}
#endif

			if ( !obj.Name().contains( "flag.o" ) && !transport )
			{
				landscapeObjectDrawn[cntr] = 0;

				TLandscapeObject* lo = new TLandscapeObject();
				*lo = obj;
				landscapeObjectLookup[cntr] = lo;

#if !defined(_DONTUSE)
				TCompoundObject* cobj = app->GetObjectByName( lo->Name().lcase() );
				if ( cobj==NULL )
				{
					errStr = "Error: landscape object \"" + lo->Name() + "\" not found";
					return false;
				}
				PostCond( cobj->Objects()!=NULL );

				lo->Object( cobj );

				if ( lo->GameImportance()>0 )
				{
					app->AddGameObject( lo );
				}
#endif
				lo->Id( cntr+1 );

				// add it to the system for drawing
				AddLandscapeObject( lo );
				cntr++;
			}
		}
		landscapeObjectCount = cntr;
	}

	// setup the lot
	ResetLandscapeObjects();
	return true;
};


bool TLandscape::LoadBinary( const TString& fname, TString& errStr, 
							 const TString& texturePath,
							 TApp* app )
{
	TString path, fileName;
	SplitPath( fname, path, fileName );
	return LoadBinary( fileName, errStr, path, texturePath, app );
}


bool TLandscape::LoadBinary( const TString& fname, TString& errStr, 
							 const TString& pathname,
							 const TString& texturePath,
							 TApp* app )
{
	size_t i,x,y;
	TString openName;

	// texture path
	initialised = false;
	TString materialPath = texturePath;
	if ( materialPath.length()==0 )
		materialPath = pathname;

	ConstructPath( openName, pathname, fname );

	TPersist file(fileRead);
	if ( !file.FileOpen( openName ) )
    {
    	errStr = "Error opening file \"" + openName;
		errStr = errStr + "\"";
    	return false;
    }

    // check signature
	char ch;
	bool ok;
	TString sig;
	do
	{
		ok = file.FileRead(&ch,1);
		if ( ok && ch!=0 )
			sig = sig + TString(ch);
	}
	while ( ch!=0 && ok );

    if ( sig!=signature || !ok )
    {
    	errStr = "Error incorrect file type or version \"" + openName;
		errStr = errStr + "\" (?" + sig + ")";
    	return false;
    }


	// def file - first data-block after header id
	if ( def!=NULL )
		delete def;

	def = new TDefFile();
	if ( !def->LoadBinary( file, errStr ) )
		return false;

	// and then the rest...
    int numMaterials;
    file.FileRead( &numMaterials, sizeof(int) );
    file.FileRead( &divx, sizeof(int) );
    file.FileRead( &divz, sizeof(int) );

	if ( app!=NULL )
	{
		// calculate min and max render depths
		float depth = float(divx + divz) * 0.5f;
		app->MinRenderDepth( size_t( depth / 8.0f ) );
		if ( app->MinRenderDepth()<6 )
			app->MinRenderDepth( 6 );
		app->MaxRenderDepth( size_t( depth / 4.0f ) );
		if ( app->MaxRenderDepth()<10 )
			app->MaxRenderDepth( 10 );
		app->RenderDepth( app->MinRenderDepth() );
		RenderDepth( app->RenderDepth() );
	}
    
	file.FileRead(&landscapeminX,sizeof(float));
	file.FileRead(&landscapeminZ,sizeof(float));
	file.FileRead(&divSizeX,sizeof(float));
	file.FileRead(&divSizeY,sizeof(float));

	divRadiusSize = float(sqrt(divSizeX*divSizeX+divSizeY*divSizeY));

	float temp;
	file.FileRead(&temp,sizeof(float));
	MinX(temp);
	file.FileRead(&temp,sizeof(float));
	MinY(temp);
	file.FileRead(&temp,sizeof(float));
	MinZ(temp);
	file.FileRead(&temp,sizeof(float));
	MaxX(temp);
	file.FileRead(&temp,sizeof(float));
	MaxY(temp);
	file.FileRead(&temp,sizeof(float));
	MaxZ(temp);
	file.FileRead(&temp,sizeof(float));
	CenterX(temp);
	file.FileRead(&temp,sizeof(float));
	CenterY(temp);
	file.FileRead(&temp,sizeof(float));
	CenterZ(temp);

	materialSize = 0;
	for ( i=0; i<size_t(numMaterials); i++ )
    {
    	TMaterial mat;
		TString errorStr;
    	if ( !mat.LoadBinary( file, materialPath, errorStr ) )
	    {
	    	errStr = "Error loading material in object \"" + openName;
			errStr = errStr + "\" (";
			errStr = errStr + errorStr;
	    	return false;
	    }
        AddMaterial( mat );
		materialSize += mat.ObjectSize();
    }

	if ( divisions!=NULL )
		DeleteDivisions();
	divisions = new TMesh*[divx*divz];
	PostCond( divisions!=NULL );
	landscapeObjectDivisions = new TLandscapeObjectDraw*[divx*divz];
	PostCond( landscapeObjectDivisions!=NULL );

	size_t valid;
	objectSize = 0;
	for ( y=0; y<divz; y++ )
	for ( x=0; x<divx; x++ )
    {
		file.FileRead(&valid,sizeof(size_t));
		if ( valid==1 )
		{
    		TMesh* mesh = new TMesh();
			PostCond( mesh!=NULL );
	    	if ( !mesh->LoadBinary( file, this ) )
		    {
				errStr = "Error loading mesh in object \"" + openName;
				errStr = errStr + "\n";
	    		return false;
		    }
			objectSize += sizeof(TMeshList);
			objectSize += mesh->ObjectSize();
			divisions[y*divx+x] = mesh;
		}
		else
			divisions[y*divx+x] = NULL;
    }

	for ( y=0; y<divz; y++ )
	for ( x=0; x<divx; x++ )
    {
		landscapeObjectDivisions[y*divx+x] = NULL;
    }

	// load landscape mesh
	if ( !landscapeMesh->LoadBinary( file, this ) )
	{
		errStr = "Error loading landscapeMesh in object \"" + openName;
		errStr = errStr + "\n";
		return false;
	}
	objectSize += sizeof(TMeshList);
	objectSize += landscapeMesh->ObjectSize();

	if ( owner && smallMap!=NULL )
		delete smallMap;
	smallMap = new TBinObject();
	PostCond( smallMap!=NULL );
	if ( !smallMap->LoadBinary( file, errStr, pathname, texturePath ) )
	{
		errStr = "Error loading smallMap in object \"" + openName;
		errStr = errStr + "\n";
		return false;
	}
	objectSize += sizeof(TMeshList);
	objectSize += smallMap->ObjectSize();
	owner = true;

	gvertices = landscapeMesh->Vertices();
	gnumVertices = landscapeMesh->NumVertices();
	gsurfaces = landscapeMesh->Surfaces();
	gnumSurfaces = landscapeMesh->NumFaces();

	// load landscape surface divisions
	surfaceDivisionCounts = new size_t[divx*divz];
	PostCond( surfaceDivisionCounts!=NULL );
	surfaceDivisions = new size_t*[divx*divz];
	PostCond( surfaceDivisions!=NULL );
	surfaceMaterialFaces = new size_t*[divx*divz];
	PostCond( surfaceMaterialFaces!=NULL );

	for ( y=0; y<divz; y++ )
	for ( x=0; x<divx; x++ )
    {
		file.FileRead( &surfaceDivisionCounts[y*divx+x], sizeof(size_t) );
		if ( surfaceDivisionCounts[y*divx+x]>0 )
		{
			size_t size = surfaceDivisionCounts[y*divx+x];
			size_t* ptr = new size_t[size*3];
			PostCond( ptr!=NULL );
			file.FileRead( ptr, size*3*sizeof(size_t) );
			surfaceDivisions[y*divx+x] = ptr;

			file.FileRead( &size, sizeof(size_t) );
			if ( size>0 )
			{
				ptr = new size_t[size];
				PostCond( ptr!=NULL );
				file.FileRead( ptr, size*sizeof(size_t) );
				surfaceMaterialFaces[y*divx+x] = ptr;
			}
			else
				surfaceMaterialFaces[y*divx+x] = NULL;
		}
		else
		{
			surfaceDivisions[y*divx+x] = NULL;
			surfaceMaterialFaces[y*divx+x] = NULL;
		}
    }

	// get the center of slice(0) for translating the piece
	// into off-world positions
	PostCond( surfaceDivisionCounts[0]>0 );
	float* v = landscapeMesh->Vertices();
	size_t s = surfaceDivisions[0][0];
	zeroMinx = v[s*3+0];
	zeroMinz = v[s*3+2];
	for ( i=1; i<surfaceDivisionCounts[0]; i++ )
	{
		size_t s = surfaceDivisions[0][i];
		if ( v[s*3+0] < zeroMinx )
			zeroMinx = v[s*3+0];
		if ( v[s*3+2] < zeroMinz )
			zeroMinz = v[s*3+2];
	}

	// sea
	file.FileRead( &waterLevel, sizeof(float) ); // !!different from def->WaterLevel()

	// colour mapped?
	if ( landscapeMesh->IsColourMapped() )
	{
		landscapeMesh->ColourMapSetWaterLevel( def->WaterLevel(), 128, 128 );
	}

	// player info & teams and stuff
	playerInfo = def->GetPlayerInfo();

	file.FileClose();

	//
	//	Setup all the landscape objects on the map
	//
	if ( !SetupLandscapeObjects( *def, app, errStr ) )
		return false;
	
	// finish object sizing
	// tally up object size
	objectSize += sizeof(float)*3*gnumVertices + sizeof(size_t)*3*gnumSurfaces;
	objectSize += sizeof(int) * gnumSurfaces * 3;
	TMaterialList* temp1 = materialList;
	while (temp1!=NULL )
	{
		objectSize += sizeof(TMaterialList);
		objectSize += temp1->material.ObjectSize();
		temp1 = temp1->next;
	}

	initialised = true;
	return true;
};

void TLandscape::CalculateExplosionDamage( TApp* app, const TPoint& point, float damageFactor )
{
	CalculateExplosionDamage( app, point.x,point.y,point.z, damageFactor );
};

void TLandscape::CalculateExplosionDamage( TApp* app, float expx, float expy, float expz,
										   float damageFactor )
{
	for ( size_t i=0; i<landscapeObjectCount; i++ )
	{
		TLandscapeObject* lo = landscapeObjectLookup[i];
		lo->CalculateDamage( app, expx, expy, expz, damageFactor );
	}
};


void TLandscape::CalculateExplosionDamage( TApp* app, size_t id, size_t damageFactor )
{
	TLandscapeObject* lo = landscapeObjectLookup[id];
	lo->CalculateDamage( app, damageFactor );
};


bool TLandscape::SetFuelAmmo( size_t _numFuel, size_t _numAmmo, TString& errStr )
{
	return true;
};


bool TLandscape::IsObjectName( const TString& str ) const
{
	size_t index = 0;
	while ( str[index]!=0 && str[index]!='.' ) index++;
	if ( str[index]=='.' )
	if ( strnicmp( &str[index], ".obj", 4 )==0 )
		return true;
	return false;
};


bool TLandscape::DivideMeshes( TBinObject& landscape, 
							   TBinObject& _smallMap, 
							   size_t xdiv, 
							   size_t ydiv, 
							   TString& errStr, 
							   const TString& fileName,
							   const TString& path, 
							   const TString& texturePath )
{
	size_t i;
	errStr = "";

	landscape.CalculateBoundaries();

	smallMap = &_smallMap;
	owner = false;

	// set centers and min maxs
	MinX( landscape.MinX() );
	MinY( landscape.MinY() );
	MinZ( landscape.MinZ() );
	MaxX( landscape.MaxX() );
	MaxY( landscape.MaxY() );
	MaxZ( landscape.MaxZ() );
	CenterX( landscape.CenterX() );
	CenterY( landscape.CenterY() );
	CenterZ( landscape.CenterZ() );

    divx = xdiv;
	divz = ydiv;

	PreCond( divx>1 && divz>1 );

	DeleteDivisions();
	divisions = new TMesh*[divx*divz];
	surfaceDivisionCounts = new size_t[divx*divz];
	surfaceDivisions = new size_t*[divx*divz];
	surfaceMaterialFaces = new size_t*[divx*divz];
	landscapeObjectDivisions = new TLandscapeObjectDraw*[divx*divz];
	PostCond( divisions!=NULL );
	PostCond( surfaceDivisions!=NULL );
	PostCond( surfaceDivisionCounts!=NULL );
	PostCond( landscapeObjectDivisions!=NULL );
	for ( i=0; i<(divx*divz); i++ )
	{
		surfaceDivisionCounts[i] = 0;
		divisions[i] = NULL;
		surfaceDivisions[i] = NULL;
		surfaceMaterialFaces[i] = NULL;
		landscapeObjectDivisions[i] = NULL;
	}


	WriteString( "\n" );

	landscapeminX = landscape.MinX();
	landscapeminZ = landscape.MinZ();

	//
	// process land and sea meshes
	//
	size_t index = 0;
	i = 0;
	bool found = false;
	while ( i<landscape.NumMeshes() )
	{
		TMesh* mesh = landscape.GetMesh(i);
		if ( mesh->Name()!=NULL )
		{
			if ( stricmp( mesh->Name().c_str(), "landscapemesh" )==0 ||
				 stricmp( mesh->Name().c_str(), "landmesh" )==0 )
			{
				index = i;
				found = true;

				// set water level to min level possible
				float minx,miny,minz,maxx,maxy,maxz;
				mesh->GetBoundaries( minx,miny,minz, maxx,maxy,maxz );
				waterLevel = miny;
			}
		}
		i++;
	};

	if ( !found )
	{
		errStr = "Could not find any meshes labelled \"landscapeMesh\".\nCan't process this file.";
		return false;
	};

	// for each mesh
	for ( size_t cntr=0; cntr<landscape.NumMeshes(); cntr++ )
	{
		TMesh* mland = landscape.GetMesh(cntr);
		PostCond( mland!=NULL );
		PostCond( mland->Name()!=NULL );

		WriteString( "processing mesh \"%s\" (%d of %d)\n",
					 mland->Name().c_str(), cntr+1, landscape.NumMeshes() );

		if ( !mland->IsColourMapped() )
		if ( mland->NumNormalFaces()==0 )
		{
			TString temp = "Warning: " + mland->Name() + " must have normals to function creating them for you\n";
			WriteString( temp.c_str() );
			mland->CreateNewNormals();
		}

		// get current mesh info into some locals
		size_t	numVertices = mland->NumVertices();
		float*	vertices = mland->Vertices();
		size_t	numNormals = mland->NumNormals();
		float*	normals = mland->Normals();
		float*  textureCoords = mland->TextureCoords();
		size_t	numTextureCoords = mland->NumTextureCoords();
		size_t	numNormalFaces = mland->NumNormalFaces();
		size_t	numSurfaces = mland->NumFaces();
		size_t*	surfaces = mland->Surfaces();
		size_t*	normalFaces = mland->NormalFaces();
		size_t	numMaterialFaces = mland->NumMaterialFaces();
		size_t*	materialFaces = mland->MaterialFaces();

		// must be true for this to work properly
		if ( !mland->IsColourMapped() )
		if ( numSurfaces!=mland->NumNormalFaces() )
		{
			errStr = "MESH error: \"" + mland->Name();
			errStr = errStr + "\" numSurfaces!=numNormalFaces()\n";
			return false;
		};

		// make sure this never happens
		if ( numMaterialFaces!=numSurfaces )
		{
			PreCond( materialFaces!=NULL );

			size_t m;
			size_t* newMaterialFaces = new size_t[numSurfaces];
			for ( m=0; m<numMaterialFaces; m++ )
				newMaterialFaces[m] = materialFaces[m];
			for ( m=numMaterialFaces; m<numSurfaces; m++ )
				newMaterialFaces[m] = materialFaces[0];
			numMaterialFaces = numSurfaces;
			materialFaces = newMaterialFaces;
		};

		bool* processed = NULL;
		PreCond( numSurfaces>0 );

		processed = new bool[numSurfaces];
		PostCond( processed!=NULL );

		for ( i=0; i<numSurfaces; i++ )
			processed[i] = false;

		//
		// create a directed graph of the landscapeMesh object
		// copy surfaces and vertices for internal use
		//
		bool doAdd = true;
		if ( cntr==index )
		{
			WriteString( "Processing landscapeMesh - copying mesh\n" );
			gnumSurfaces = numSurfaces;
			gnumVertices = numVertices;
			gsurfaces = new size_t[gnumSurfaces*3];
			gvertices = new float[gnumVertices*3];
			PostCond( gsurfaces!=NULL );
			PostCond( gvertices!=NULL );
			for ( i=0; i<gnumSurfaces*3; i++ )
				gsurfaces[i] = surfaces[i];
			for ( i=0; i<gnumVertices*3; i++ )
				gvertices[i] = vertices[i];

			WriteString( "Processing landscapeMesh\n" );
			//
			// create a new Mesh at (x,y) with newSurfaceCount surfaces
			//
			landscapeMesh->NumVertices( numVertices );
			landscapeMesh->NumFaces( numSurfaces );
			landscapeMesh->NumTextureCoords( numTextureCoords );
			landscapeMesh->NumMaterialFaces( numMaterialFaces );
			landscapeMesh->NumNormals( numNormals );
			landscapeMesh->NumNormalFaces( numNormalFaces );

			landscapeMesh->Vertices( vertices );
			landscapeMesh->Surfaces( surfaces );
			landscapeMesh->TextureCoords( textureCoords );
			landscapeMesh->MaterialFaces( materialFaces );
			landscapeMesh->Normals( normals );
			landscapeMesh->NormalFaces( normalFaces );

			// copy material references
			size_t nms = mland->NumMaterials();
			landscapeMesh->NumMaterials( nms );
			for ( i=0; i<nms; i++ )
				landscapeMesh->Materials( i, i, mland->Materials(i) );
		}
		else if ( strnicmp( mland->Name().c_str(), "seamesh", 7 )==0 )
		{
			WriteString( "Processing sea object\n" );
			float minx,miny,minz,maxx,maxy,maxz;
			mland->GetBoundaries( minx,miny,minz, maxx,maxy,maxz );
			waterLevel = maxy;
		}

		if ( doAdd )
		{
			// division sizes
			divSizeX = ((landscape.MaxX() - landscape.MinX())+1.0f) / float(xdiv);
			divSizeY = ((landscape.MaxZ() - landscape.MinZ())+1.0f) / float(ydiv);

			// go through xdiv x ydiv stages and create new meshes for each
			WriteString( "Subdividing mesh\n" );
			size_t totalSize = xdiv*ydiv;
			ResetPercentage();

			for ( size_t x=0; x<xdiv; x++ )
			for ( size_t y=0; y<ydiv; y++ )
			{
				DisplayPercentage( x*ydiv+y, totalSize );

				// work out what the limits of this sub-division are
				float startx = landscape.MinX() + divSizeX * float(x);
				float starty = landscape.MinZ() + divSizeY * float(y);

				// count how many surfaces fall inside this section
				size_t newSurfaceCount = 0;
				for ( size_t i=0; i<numSurfaces; i++ )
				if ( !processed[i] )
				{
					bool found = false;
					for ( size_t j=0; j<3; j++ )
					{
						size_t surf = surfaces[i*3+j];
						float px = vertices[surf*3+0];
						float py = vertices[surf*3+1];
						float pz = vertices[surf*3+2];
						if ( px>=startx && px<=(startx+divSizeX) &&
							pz>=starty && pz<=(starty+divSizeY) && !found )
						{
							newSurfaceCount++;
							found = true;
						}
					}
				}


				//
				// non landscape meshes are merged
				//
				TMesh* mesh = NULL;
				if ( cntr!=index && newSurfaceCount>0 )
				{
					//
					// create a new Mesh at (x,y) with newSurfaceCount surfaces
					//
					mesh = new TMesh();
					PostCond( mesh!=NULL );
	
					mesh->NumVertices( newSurfaceCount*3 );
					mesh->NumFaces( newSurfaceCount );
					if ( !mland->IsColourMapped() )
					{
						mesh->NumTextureCoords( newSurfaceCount*3 );
						mesh->NumMaterialFaces( newSurfaceCount );
						mesh->NumNormals( newSurfaceCount*3 );
						mesh->NumNormalFaces( newSurfaceCount );
					}

					// copy material references
					size_t nms = mland->NumMaterials();
					mesh->NumMaterials( nms );
					for ( i=0; i<nms; i++ )
						mesh->Materials( i, i, mland->Materials(i) );
	
					float*	newVertices = mesh->Vertices();
					float*	newNormals = mesh->Normals();
					float*	newTextureCoords = mesh->TextureCoords();
					size_t*	newFaces = mesh->Surfaces();
					size_t*	newNormalFaces = mesh->NormalFaces();
					size_t*	newMaterialFaces = mesh->MaterialFaces();

					// copy the new faces to the new mesh
					newSurfaceCount = 0;
					size_t newVertexCount = 0;
					size_t newNormalCount = 0;
					for ( i=0; i<numSurfaces; i++ )
					if ( !processed[i] )
					{
						bool found = false;
						for ( size_t j=0; j<3; j++ )
						{
							size_t surf = surfaces[i*3+j];
							float px = vertices[surf*3+0];
							float py = vertices[surf*3+1];
							float pz = vertices[surf*3+2];
							if ( px>=startx && px<=(startx+divSizeX) &&
								pz>=starty && pz<=(starty+divSizeY) && !found )
							{
								found = true;
							}
						}

						if ( found )
						{
							processed[i] = true;
							size_t surf = surfaces[i*3+0];
							size_t vertexIndex = GetVertex( newVertices, &vertices[surf*3], newVertexCount );
							newFaces[newSurfaceCount*3+0] = vertexIndex;
							if ( !mland->IsColourMapped() )
							{
								newTextureCoords[vertexIndex*2+0] = textureCoords[surf*2+0];
								newTextureCoords[vertexIndex*2+1] = textureCoords[surf*2+1];
							}
	
							surf = surfaces[i*3+1];
							vertexIndex = GetVertex( newVertices, &vertices[surf*3], newVertexCount );
							newFaces[newSurfaceCount*3+1] = vertexIndex;
							if ( !mland->IsColourMapped() )
							{
								newTextureCoords[vertexIndex*2+0] = textureCoords[surf*2+0];
								newTextureCoords[vertexIndex*2+1] = textureCoords[surf*2+1];
							}

							surf = surfaces[i*3+2];
							vertexIndex = GetVertex( newVertices, &vertices[surf*3], newVertexCount );
							newFaces[newSurfaceCount*3+2] = vertexIndex;
							if ( !mland->IsColourMapped() )
							{
								newTextureCoords[vertexIndex*2+0] = textureCoords[surf*2+0];
								newTextureCoords[vertexIndex*2+1] = textureCoords[surf*2+1];
							}
				
							// copy material faces
							if ( !mland->IsColourMapped() )
							{
								newMaterialFaces[newSurfaceCount] = materialFaces[i];
			
								// process normals
								surf = normalFaces[i*3+0];
								size_t normalIndex = GetVertex( newNormals, &normals[surf*3], newNormalCount );
								newNormalFaces[newSurfaceCount*3+0] = normalIndex;
	
								surf = normalFaces[i*3+1];
								normalIndex = GetVertex( newNormals, &normals[surf*3], newNormalCount );
								newNormalFaces[newSurfaceCount*3+1] = normalIndex;
						
								surf = normalFaces[i*3+2];
								normalIndex = GetVertex( newNormals, &normals[surf*3], newNormalCount );
								newNormalFaces[newSurfaceCount*3+2] = normalIndex;
							}
							newSurfaceCount++;
						}
					}
				}
				else if ( newSurfaceCount>0 )
				{
					//
					//	Surface division otherwise
					//
					surfaceDivisionCounts[y*divx+x] = newSurfaceCount;
					size_t* ptr = new size_t[newSurfaceCount*3];
					surfaceDivisions[y*divx+x] = ptr;
					PostCond( surfaceDivisions[y*divx+x]!=NULL );

					if ( !mland->IsColourMapped() )
					{
						ptr = new size_t[newSurfaceCount];
						surfaceMaterialFaces[y*divx+x] = ptr;
						PostCond( surfaceMaterialFaces[y*divx+x]!=NULL );
					}

					size_t cntr = 0;
					size_t matCntr = 0;
					size_t* newSurfaces = surfaceDivisions[y*divx+x];

					size_t* matFaces = NULL;
					if ( !mland->IsColourMapped() )
					{
						matFaces = surfaceMaterialFaces[y*divx+x];
					}

					for ( i=0; i<numSurfaces; i++ )
					if ( !processed[i] )
					{
						bool found = false;
						size_t j;
						for ( j=0; j<3; j++ )
						{
							size_t surf = surfaces[i*3+j];
							float px = vertices[surf*3+0];
							float py = vertices[surf*3+1];
							float pz = vertices[surf*3+2];
							if ( px>=startx && px<=(startx+divSizeX) &&
								pz>=starty && pz<=(starty+divSizeY) && !found )
							{
								processed[i] = true;
								found = true;
							}
						}
						if ( found )
						{
							for ( j=0; j<3; j++ )
								newSurfaces[cntr++] = surfaces[i*3+j];
							if ( !mland->IsColourMapped() )
							{
								matFaces[matCntr++] = materialFaces[i];
							}
						}
					}
				}

				if ( mesh!=NULL )
				{
					TString buf;
					buf = FormatString( "%02d_%02d", x, y );
					mesh->Name( buf );
	
					if ( divisions[y*divx+x]==NULL )
						divisions[y*divx+x] = mesh;
					else if ( divisions[y*divx+x]!=NULL && mesh!=NULL )
						divisions[y*divx+x]->MergeMesh( mesh );
				}

			} // end of for x, for y

			FinishPercentage();
			if ( processed!=NULL )
				delete processed;

		} // end of if ( doAdd

	} // end of for cntr=firstMesh to lastMesh

	initialised = true;
	return true;
};


size_t TLandscape::GetVertex( float* vertices, float* vectorXYZ, size_t& count )
{
	vertices[count*3+0] = vectorXYZ[0];
	vertices[count*3+1] = vectorXYZ[1];
	vertices[count*3+2] = vectorXYZ[2];
	size_t foundIndex = count++;

	return size_t(foundIndex);
};


void TLandscape::Draw( class TCamera& cam, float vx, float vz, bool drawTransparencies )
{
	// this will fix transparency problems
	if ( drawTransparencies )
	{
		glDepthMask(GL_FALSE);
	}

	Draw( cam.CameraX(), cam.CameraY(), cam.CameraZ(), cam.YAngle(), cam.XAngle(), 
		  vx, vz, drawTransparencies );

	glDepthMask(GL_TRUE);
};


void TLandscape::SetSeaSurfaceColour( float r, float g, float b )
{
	seaSurfaceR = r;
	seaSurfaceG = g;
	seaSurfaceB = b;
};


void TLandscape::Draw( float camX, float camY, float camZ, 
					   float camYangle, float camXangle,
					   float vx, float vz, bool drawTransparencies )
{
	if ( initialised )
	{
		landscapeObjectCounter++;

		PreCond( circle!=NULL );

		int cX = int((-camX-MinX()) / divSizeX);
		int cZ = int((-camZ-MinZ()) / divSizeY);

		// adjust angle to true viewer angle
		float yAngle = -camYangle + 180.0f;

		// reduce the first factor of sliceSize to get smaller view port sizes
		float sliceSize = 90.0f - (camXangle / 2.0f); // tilt determines view optimisation
		float minAngle = yAngle - sliceSize;
		float maxAngle = yAngle + sliceSize;

		// calculate the three points (plus (0,0,0)) which make
		// up the two viewing triangles relative to me and only
		// draw squares within these triangles
		float yangleRad1 = yAngle * degToRad;
		float yangleRad2 = minAngle * degToRad;
		float yangleRad3 = maxAngle * degToRad;

		float p1x = float(sin(yangleRad1)) * float(renDepth*2);
		float p1z = float(cos(yangleRad1)) * float(renDepth*2);
		float p2x = float(sin(yangleRad2)) * float(renDepth*2);
		float p2z = float(cos(yangleRad2)) * float(renDepth*2);
		float p3x = float(sin(yangleRad3)) * float(renDepth*2);
		float p3z = float(cos(yangleRad3)) * float(renDepth*2);

		int rd = int(renDepth);
		int x1, y1;

		for ( x1=-rd; x1<=rd; x1++ )	// only draw up to renderDepth radius
		for ( y1=-rd; y1<=rd; y1++ )
		if ( circle[(y1+rd)*2*rd+(x1+rd)] ) // only draw inside the circle
		{
			// only draw view angles inside the viewing frustrum
			bool doDraw = false;

			// [-3..3,y,-3..3] get drawn regardless
			if ( x1>-3 && x1<3 && y1>-3 && y1<3 )
				doDraw = true;

			float v1[3], v2[3], v3[3];
			v1[0] = 0;
			v1[1] = 0;
			v1[2] = 0;

			v2[0] = p1x;
			v2[1] = 0;
			v2[2] = p1z;

			// inside the rectangles?
			if ( !doDraw )
			{
				float dummyY;
				v3[0] = p2x;
				v3[1] = 0;
				v3[2] = p2z;
				doDraw = PointInTriangle( float(x1), dummyY, float(y1),
										  v1,v2,v3 );
			}
			if ( !doDraw )
			{
				float dummyY;
				v3[0] = p3x;
				v3[1] = 0;
				v3[2] = p3z;
				doDraw = PointInTriangle( float(x1), dummyY, float(y1),
										  v1,v2,v3 );
			}

			// can we draw this item?
			if ( doDraw )
			{
				int newX = (cX+x1);
				int newY = (cZ+y1);

				if ( newX>=0 && newX<int(divx) && newY>=0 && newY<int(divz) )
				{
					if ( !drawTransparencies )
					if ( surfaceDivisions[newY*divx+newX]!=NULL )
						landscapeMesh->Draw( surfaceDivisionCounts[newY*divx+newX],
											 surfaceDivisions[newY*divx+newX],
											 surfaceMaterialFaces[newY*divx+newX],
											 drawTransparencies );

					if ( divisions!=NULL )
					if ( divisions[newY*divx+newX]!=NULL )
						divisions[newY*divx+newX]->Draw(drawTransparencies);

					if ( !drawTransparencies )
					{
						// draw objects in an even smaller radius
						if ( x1>(-rd+1) && x1<(rd-1) )
						if ( y1>(-rd+1) && y1<(rd-1) )
						if ( landscapeObjectDivisions[newY*divx+newX]!=NULL )
						{
							TLandscapeObjectDraw* ngd = landscapeObjectDivisions[newY*divx+newX];
							while ( ngd!=NULL )
							{

								//ngd->DrawBoundingBox( 255,255,255 );

								size_t id = ngd->obj->Id() - 1;
								if ( landscapeObjectDrawn[id]!=landscapeObjectCounter )
								{
									landscapeObjectDrawn[id] = landscapeObjectCounter;

									if ( ngd->obj->CurrentIndex() < ngd->obj->MaxIndex() )
									{
										//if ( !drawTransparencies )
										//	ngd->obj->DrawBoundingBox(255,255,255);
										ngd->Draw(drawTransparencies);
									}
								}
								ngd = ngd->Next();
							} // end of while
						} // end of if not NULL
					} // end if if (!drawTransparencies)

				} 
				else // draw outside bounds - just draw the zeroSeaSlice
				{
#ifndef _ALTERNATESEA
					glPushMatrix();

					float x = float(newX*divSizeX+landscapeminX);
					float z = float((newY+1)*divSizeY-493.2f); //landscapeminZ);
					glTranslatef( x+500, 0, z-500);

					if ( zeroSeaSlice!=NULL && drawTransparencies )
					{
						zeroSeaSlice->Draw(drawTransparencies);

					}
					if ( zeroSurfaceSlice!=NULL && !drawTransparencies )
					{
						// 97,104,78  x  0.75,0.75,1 = 0.285, 0.306, 0.306
						zeroSurfaceSlice->DrawMonoColoured( seaSurfaceR, seaSurfaceG, seaSurfaceB );
					}
					glPopMatrix();
#endif
				}
			}
		}

#ifdef _ALTERNATESEA
		glPushMatrix();

		if ( zeroSeaSlice!=NULL && drawTransparencies )
		{
			zeroSeaSlice->Draw(drawTransparencies);

		}
		if ( zeroSurfaceSlice!=NULL && !drawTransparencies )
		{
			zeroSurfaceSlice->DrawMonoColoured( seaSurfaceR, seaSurfaceG, seaSurfaceB );
		}

		glPopMatrix();
#endif

		// transparencies are special - draw the last
		if ( drawTransparencies )
		for ( x1=-rd; x1<=rd; x1++ )	// only draw up to renderDepth radius
		for ( y1=-rd; y1<=rd; y1++ )
		if ( circle[(y1+rd)*2*rd+(x1+rd)] ) // only draw inside the circle
		{
			// only draw view angles inside the viewing frustrum
			bool doDraw = false;

			// [-3..3,y,-3..3] get drawn regardless
			if ( x1>-3 && x1<3 && y1>-3 && y1<3 )
				doDraw = true;

			float v1[3], v2[3], v3[3];
			v1[0] = 0;
			v1[1] = 0;
			v1[2] = 0;

			v2[0] = p1x;
			v2[1] = 0;
			v2[2] = p1z;

			// inside the rectangles?
			if ( !doDraw )
			{
				float dummyY;
				v3[0] = p2x;
				v3[1] = 0;
				v3[2] = p2z;
				doDraw = PointInTriangle( float(x1), dummyY, float(y1),
										  v1,v2,v3 );
			}
			if ( !doDraw )
			{
				float dummyY;
				v3[0] = p3x;
				v3[1] = 0;
				v3[2] = p3z;
				doDraw = PointInTriangle( float(x1), dummyY, float(y1),
										  v1,v2,v3 );
			}

			// can we draw this item?
			if ( doDraw )
			{
				int newX = (cX+x1);
				int newY = (cZ+y1);

				if ( newX>=0 && newX<int(divx) && newY>=0 && newY<int(divz) )
				{
					// draw objects in an even smaller radius
					if ( x1>(-rd+1) && x1<(rd-1) )
					if ( y1>(-rd+1) && y1<(rd-1) )
					if ( landscapeObjectDivisions[newY*divx+newX]!=NULL )
					{
						TLandscapeObjectDraw* ngd = landscapeObjectDivisions[newY*divx+newX];
						while ( ngd!=NULL )
						{

							//ngd->DrawBoundingBox( 255,255,255 );

							size_t id = ngd->obj->Id() - 1;
							if ( landscapeObjectDrawn[id]!=landscapeObjectCounter )
							{
								landscapeObjectDrawn[id] = landscapeObjectCounter;

								if ( ngd->obj->CurrentIndex() < ngd->obj->MaxIndex() )
								{
									//if ( !drawTransparencies )
									//	ngd->obj->DrawBoundingBox(255,255,255);
									ngd->Draw(drawTransparencies);
								}
							}
							ngd = ngd->Next();
						} // end of while
					} // end of if not NULL
				}
			}
		}
	}
};



void TLandscape::DrawNoObj( bool drawTransparencies )
{
	if ( initialised )
	{
		for ( size_t newY=0; newY<divz; newY++ )
		for ( size_t newX=0; newX<divx; newX++ )
		{
			if ( surfaceDivisions[newY*divx+newX]!=NULL )
				landscapeMesh->Draw( surfaceDivisionCounts[newY*divx+newX],
									 surfaceDivisions[newY*divx+newX],
									 surfaceMaterialFaces[newY*divx+newX],
									 drawTransparencies );

			if ( divisions[newY*divx+newX]!=NULL )
				divisions[newY*divx+newX]->Draw(drawTransparencies);
		}
	}
};


void TLandscape::RenderDepth( size_t _renderDepth )
{
	if ( circle!=NULL )
		delete circle;
	circle = new bool[_renderDepth*_renderDepth*4];
	renDepth = _renderDepth;
	int rd = int(_renderDepth);
	for ( int x=-rd; x<rd; x++ )
	for ( int y=-rd; y<rd; y++ )
	{
		float dx = float(x);
		float dy = float(y);
		float dist = float(sqrt(x*x+y*y));
		if ( dist <= float(rd) )
			circle[(y+rd)*rd*2 + (x+rd)] = true;
		else
			circle[(y+rd)*rd*2 + (x+rd)] = false;
	}
};


size_t TLandscape::RenderDepth( void )
{
	return renDepth;
};


float TLandscape::WaterLevel( void ) const
{
	return waterLevel;
};


void TLandscape::ClearLandscapeObjects( void )
{
	for ( size_t x=0; x<divx; x++ )
	for ( size_t y=0; y<divz; y++ )
		if ( landscapeObjectDivisions[y*divx+x]!=NULL )
		{
			TLandscapeObjectDraw* first = landscapeObjectDivisions[y*divx+x];
			while ( first!=NULL )
			{
				TLandscapeObjectDraw* temp = first;
				first = first->next;
				temp->next = NULL;
				delete temp;
			}
			landscapeObjectDivisions[y*divx+x] = NULL;
		}
};


void TLandscape::AddLandscapeObject( TLandscapeObject* lnd )
{
	lnd->Refresh(); // make sure internal matrix is up to date

	float minx,miny,minz,maxx,maxy,maxz;
	lnd->GetBounds( minx,miny,minz,maxx,maxy,maxz );

	size_t xpos1 = size_t((minx-landscapeminX) / divSizeX);
	size_t zpos1 = size_t((minz-landscapeminZ) / divSizeY);
	size_t xpos2 = size_t((maxx-landscapeminX) / divSizeX);
	size_t zpos2 = size_t((maxz-landscapeminZ) / divSizeY);
/*
	if ( xpos1==xpos2 && zpos1==zpos2 )
	{
		_AddLandscapeObject( xpos1,zpos1, lnd );
	}
	else
	{
		_AddLandscapeObject( xpos1,zpos1, lnd );
		_AddLandscapeObject( xpos2,zpos2, lnd );
	};
	if ( xpos1>0 )
		xpos1--;
	if ( zpos1>0 )
		zpos1--;
	if ( xpos2<(divx-1) )
		xpos2++;
	if ( zpos2<(divz-1) )
		zpos2++;
*/
	// go through all landscape sections, and insert this landscapeObject
	// into every section it crosses
	for ( size_t z=zpos1; z<=zpos2; z++ )
	for ( size_t x=xpos1; x<=xpos2; x++ )
	{
/*
		// each landscape square has four points
		float x1,z1,x2,z2;
		x1 = landscapeminX + x*divSizeX;
		z1 = landscapeminZ + z*divSizeY;
		x2 = landscapeminX + (x+1)*divSizeX;
		z2 = landscapeminZ + (z+1)*divSizeY;

		// do any of these lines intersect the boundary rectangle of the object?
		if ( lnd->LineInsideObject( x1,z1,x2,z1 ) ||
			 lnd->LineInsideObject( x2,z1,x2,z2 ) ||
			 lnd->LineInsideObject( x2,z2,x1,z2 ) ||
			 lnd->LineInsideObject( x1,z2,x1,z1 ) )
		{
			_AddLandscapeObject( x,z, lnd );
		}
*/
		_AddLandscapeObject( x,z, lnd );
	}
};


void TLandscape::_AddLandscapeObject( size_t x, size_t z, TLandscapeObject* lnd )
{
	PreCond( lnd->Object()->Objects()!=NULL );
	if ( landscapeObjectDivisions[z*divx+x]==NULL )
	{
		landscapeObjectDivisions[z*divx+x] = new TLandscapeObjectDraw(*lnd);
	}
	else
	{
		TLandscapeObjectDraw* ngdll = landscapeObjectDivisions[z*divx+x];
		bool found = false;
		while ( ngdll->next!=NULL && !found )
		{
			if ( ngdll->obj->Id()==lnd->Id() )
				found = true;
			ngdll = ngdll->next;
		}
		if ( ngdll->obj->Id()==lnd->Id() )
			found = true;
		if ( !found )
			ngdll->next = new TLandscapeObjectDraw(*lnd);
	}
};


void TLandscape::UseFuel( size_t objId )
{
};


void TLandscape::UseAmmo( size_t objId )
{
};


bool TLandscape::IsFuel( size_t objId )
{
	return false;
};


bool TLandscape::IsAmmo( size_t objId )
{
	return false;
};

/*
size_t TLandscape::InsideObstacle( float xp, float yp, float zp )
{
	float xpos = (xp-landscapeminX) / divSizeX;
	float zpos = (zp-landscapeminZ) / divSizeY;

	size_t x = size_t(xpos);
	size_t z = size_t(zpos);

	if ( landscapeObjectDivisions[z*divx+x]!=NULL )
	{
		TLandscapeObjectDraw* ngd = landscapeObjectDivisions[z*divx+x];
		while ( ngd!=NULL )
		{
			if ( ngd->InsideObject(xp,yp,zp) )
			{
				return ngd->obj->Id();
			}
			ngd = ngd->Next();
		};
	}
	return 0;
};
*/

TLandscapeObject* TLandscape::GetLandscapeObject( const TString& name ) const
{
	TString nm = name.lcase();

	PreCond( landscapeObjectLookup!=NULL );
	for ( size_t i=0; i<landscapeObjectCount; i++ )
		if ( nm==landscapeObjectLookup[i]->Name().lcase() )
			return landscapeObjectLookup[i];
	return NULL;
};


TLandscapeObject* TLandscape::GetLandscapeObject( size_t id ) const
{
	PreCond( id>0 );
	PreCond( landscapeObjectLookup!=NULL );
	return landscapeObjectLookup[id-1];
};


void TLandscape::DamageLandscapeObject( size_t id )
{
	PreCond( id>0 );
	PreCond( landscapeObjectLookup!=NULL );
	TLandscapeObject* lo = landscapeObjectLookup[id-1];

	if ( lo->Destructable() )
	if ( lo->CurrentIndex() < lo->MaxIndex() )
		lo->CurrentIndex( lo->CurrentIndex() + 1 );
};


void TLandscape::Initialised( bool i )
{
	initialised = i;
};

bool TLandscape::Initialised( void ) const
{
	return initialised;
};

float TLandscape::DivSizeX( void ) const
{
	return divSizeX;
};


float TLandscape::DivSizeY( void ) const
{
	return divSizeY;
};


TMesh* TLandscape::LandscapeMesh( void ) const
{
	return landscapeMesh;
};


TMapPlayerInfo* TLandscape::PlayerInfo( void ) const
{
	return const_cast<TMapPlayerInfo*>(&playerInfo);
};


void TLandscape::PlayerInfo( TMapPlayerInfo* pi )
{
	PreCond( pi!=NULL );
	playerInfo = *pi;
};

TBinObject* TLandscape::SmallMap( void ) const
{
	return smallMap;
};

float TLandscape::DivX( void ) const
{
	return float(divx);
};

float TLandscape::DivZ( void ) const
{
	return float(divz);
};

float TLandscape::PixelRadius( void ) const
{
	return (float(renDepth)*divRadiusSize);
};

TLandscapeObject TLandscape::GetPlayerPosition( size_t team, size_t playerId )
{
	PreCond( playerInfo.numTeams > 0 );
	PreCond( team < kMaxTeams );
	PreCond( playerId < playerInfo.teamCount[team] );

	return playerInfo.teams[team][playerId];
};


TLandscapeObject TLandscape::GetPlayerPositionNoTeams( size_t playerId, size_t& team )
{
	PreCond( playerInfo.numTeams > 0 );

	// find first team/player combo that matches this index
	size_t cnt = 0;
	for ( size_t i=0; i<kMaxTeams; i++ )
	{
		if ( playerInfo.teamCount[i] > 0 )
		if ( (playerInfo.teamCount[i]+cnt) > playerId )
		{
			team = i;
			return playerInfo.teams[i][playerId-cnt];
		}
		cnt += playerInfo.teamCount[i];
	}
	PostCond( "Couldn't get player position"==NULL );
	return playerInfo.teams[0][0];
};


TLandscapeObject TLandscape::GetSinglePlayer( size_t& team )
{
	PreCond( playerInfo.numTeams>0 );

	for ( size_t i=0; i<kMaxTeams; i++ )
	{
		if ( playerInfo.teamCount[i]>0 )
		{
			team = i;
			return playerInfo.teams[i][0];
		}
	}
	PostCond( "Error in playerInfo object"==NULL );
	return playerInfo.teams[0][0];
};


void TLandscape::GetY( float l1x, float& y, float l1z )
{
	float maxy = landscapeMesh->MaxY() * 4;
	y = waterLevel;

	float xpos = (l1x-landscapeminX) / divSizeX;
	float ypos;
	float zpos = (l1z-landscapeminZ) / divSizeY;

	bool found = false;
	float dist = -10000;
	for ( int xo = -1; xo<=1 ; xo++ )
	for ( int yo = -1; yo<=1 ; yo++ )
	{
		int x = int(xpos) + xo;
		int z = int(zpos) + yo;
		if ( x>=0 && z>=0 && x<int(divx) && z<int(divz) )
		if ( surfaceDivisions[z*divx+x]!=NULL )
		{
			size_t numFaces = surfaceDivisionCounts[z*divx+x];
			size_t* surfaces = surfaceDivisions[z*divx+x];

			if ( landscapeMesh->Collision( numFaces, surfaces, l1x,maxy,l1z, dist, ypos ) )
			{
				if ( ypos > waterLevel )
					y = ypos;
			}
		}
	}
};

// expensive version of GetY()
void TLandscape::GetBestY( float l1x, float& y, float l1z )
{
	float maxy = landscapeMesh->MaxY() * 4;
	y = waterLevel;

	float xpos = (l1x-landscapeminX) / divSizeX;
	float ypos;
	float zpos = (l1z-landscapeminZ) / divSizeY;

	bool found = false;
	float dist = -10000;
	for ( int xo = -1; xo<=1 ; xo++ )
	for ( int yo = -1; yo<=1 ; yo++ )
	{
	int x = int(xpos) + xo;
	int z = int(zpos) + yo;
	if ( x>=0 && z>=0 && x<int(divx) && z<int(divz) )
	if ( surfaceDivisions[z*divx+x]!=NULL )
	{
		size_t numFaces = surfaceDivisionCounts[z*divx+x];
		size_t* surfaces = surfaceDivisions[z*divx+x];

		if ( landscapeMesh->Collision( numFaces, surfaces, l1x,maxy,l1z, dist, ypos ) )
		{
			if ( ypos > waterLevel )
				y = ypos;
		}

		if ( xo==0 && yo==0 )
		if ( landscapeObjectDivisions[z*divx+x]!=NULL )
		{
			TLandscapeObjectDraw* ngd = landscapeObjectDivisions[z*divx+x];
			while ( ngd!=NULL )
			{
				if ( ngd->InsideObject(l1x,l1z) )
				{
					if ( ngd->GetLORTriangle( l1x,maxy,l1z,dist, ypos ) )
					{
						if ( ypos > y )
						{
							y = ypos;
						}
					}
				}
				ngd = ngd->Next();
			}
		}
	}
	}
};

bool TLandscape::Reload( TString& errStr )
{
	if ( !TBinObject::Reload(errStr) )
	{
		return false;
	}
	if ( landscapeMesh!=NULL )
	{
		if ( !landscapeMesh->Reload(errStr) )
		{
			return false;
		}
	}
	if ( smallMap!=NULL )
	{
		if ( !smallMap->Reload(errStr) )
		{
			return false;
		}
	}
	return true;
};

//==========================================================================

