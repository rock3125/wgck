#ifndef __DXPARSER_MESH_H_
#define __DXPARSER_MESH_H_

///==========================================================================///

class TMaterial;
class TBinObject;
class TMapHeightInfo;

#include <object/graph.h>
#include <object/geometry.h>

//==========================================================================

class _EXPORT TMesh
{
public:
	TMesh( void );
    ~TMesh( void );

	TMesh( const TMesh& mesh );
	const TMesh& operator=( const TMesh& );

    // load & save binary routines for fast loading & saving
	bool		SaveBinary( TPersist& file, const TBinObject* obj ) const;
    bool		LoadBinary( TPersist& file, const TBinObject* obj );

	// save mesh as a directX ascii text file for export purposes
	bool		SaveText( TPersist& file, const TBinObject* obj ) const;

    // describe this mesh
	void Print( void ) const;

	// reload my materials after resizing
	bool		Reload( TString& errStr );
	bool		ReloadBumpmap( TString& errStr ); // called by Reload()

	void		ForceDraw( void ) const; // draw no matter ownerDraw status
	void 		Draw( void ) const;
	void 		Draw( bool drawTransparencies ) const;
	void		Draw( size_t numFaces, size_t* surfaces, size_t* mats );
	void		Draw( const float* matrix ) const;
	void		Draw( float scale, const float* matrix ) const;
	void		Draw( float scale ) const;
    void		DrawNoMaterials( float scale ) const;

	void		Draw( size_t numFaces, size_t* surfaces, size_t* mats, bool drawTransparencies );

	void		DrawMonoColoured( float r, float g, float b ) const;
	
	void		DrawExplosion( float scale ) const;

    const TString& Name( void ) const;
    void		Name( const TString& name );

	// bypass normal drawing system
	bool		OwnerDraw( void ) const;
	void		OwnerDraw( bool );

	// see if movement from l1 to l2 causes an intersection with any of my surfaces
	bool		Collision( float l1x, float l1y, float l1z,
						   float l2x, float l2y, float l2z ) const;
	bool		Collision( const float* matrix,
						   float l1x, float l1y, float l1z,
						   float l2x, float l2y, float l2z ) const;
	bool		Collision( TMatrix& matrix,
						   float l1x, float l1y, float l1z,
						   float l2x, float l2y, float l2z );
	bool		Collision( size_t numFaces, size_t* surfaces,
						   float l2x, float l2y, float l2z, 
						   float size, float& y ) const;
	bool		Collision( const float* matrix,
						   size_t numFaces, size_t* surfaces,
						   float l1x, float l1y, float l1z,
						   float size, float& y ) const;
	bool		Collision( const float* matrix,
						   float l1x, float l1z,
						   float l2x, float l2z ) const;

	size_t 		NumVertices( void ) const;
	void		NumVertices( size_t val );

    float*		Vertices( void ) const;
    void		Vertices( float* ptr );

    size_t		NumFaces( void ) const;
    void		NumFaces( size_t val );

    size_t*		Surfaces( void ) const;
    void		Surfaces( size_t* ptr );

    size_t		NumMaterials( void ) const;
    void		NumMaterials( size_t val );

    size_t		NumMaterialFaces( void ) const;
    void		NumMaterialFaces( size_t val );

    size_t*		MaterialFaces( void ) const;
    void		MaterialFaces( size_t* ptr );

    TMaterial*	Materials( size_t index ) const;
    void		Materials( size_t index, size_t _materialParentIndex, TMaterial* ptr );

    size_t		MaterialParentIndex( size_t index ) const;

    size_t		NumNormals( void ) const;
    void		NumNormals( size_t val );

    float*		Normals( void ) const;
    void		Normals( float* ptr );

    size_t		NumNormalFaces( void ) const;
    void		NumNormalFaces( size_t val );

    size_t*		NormalFaces( void ) const;
    void		NormalFaces( size_t* ptr );

    size_t		NumTextureCoords( void ) const;
    void		NumTextureCoords( size_t val );

    float*		TextureCoords( void ) const;
    void		TextureCoords( float* ptr );

	long		ObjectSize( void ) const;

	void		CullNumVertices( size_t newNumVertices );
	void		CullUVs( size_t newNumTextureCoords );
	void		CullNumNormals( size_t newNumNormals );

	void		RemoveDuplicates( void );
	void		MergeMesh( TMesh* mesh );
	void		ReverseWindings( void );
	void		ManualTransform( void );

	void		AdjustAxes( float minX, float maxX,
							float minY, float maxY,
							float minZ, float maxZ,
							bool minx0, bool miny0, bool minz0, 
							bool maxx0, bool maxy0, bool maxz0 );

	// center any of these axis inside this mesh
	void		CenterAxes( bool cx, bool cy, bool cz );

	// given a global size and a global resize - resize this particular mesh
	void		ResizeMesh( float _minX, float _minY, float _minZ,
							float _maxX, float _maxY, float _maxZ,
							float bx1, float by1, float bz1,
							float bx2, float by2, float bz2 );

	// resize the object physically by reducing all vertices located spacially
	// within threshold, return how many vertices were affected
	void		HalfMesh( void );

	// evaluate and return the mesh boundaries
	void		CalculateBoundaries( void );

	void		GetBoundaries( float& minx, float& miny, float& minz,
							   float& maxx, float& maxy, float& maxz ) const;

	float MinX( void ) const;
	float MinY( void ) const;
	float MinZ( void ) const;

	float MaxX( void ) const;
	float MaxY( void ) const;
	float MaxZ( void ) const;

	float CenterX( void ) const;
	float CenterY( void ) const;
	float CenterZ( void ) const;

	float SizeX( void ) const;
	float SizeY( void ) const;
	float SizeZ( void ) const;

	void CreateNewNormals( void ); // create new normals based on std. algorithms

	bool GetLORTriangle( float xp, float yp, float zp, float size, float& y );
	bool GetLORTriangle( const float* matrix,
						 float xp, float yp, float zp, 
						 float size, float& y );

	void FlipUV( void );

	bool IsColourMapped( void ) const;
	void IsColourMapped( bool cm );

	void CreateColourMap( const TTexture& colourMap, const TTexture& bumpMap );
	void SetColourMapSize( size_t _colourMapWidth, size_t _colourMapHeight );
	void ColourMapSetWaterLevel( size_t waterLevel, size_t w, size_t h );

	// multiply my vertices by a matrix
	void MatrixMult( const float* matrix );

	// multiply uv by values
	void MultiplyUV( float um, float vm );

	// center object around a point
	void Center( float cx, float cy, float cz );

	// reset material pointers according to a parent
	void ResetMaterials( TBinObject* obj );

	// get zero colour of colourmap for landscape zero slices
	void GetSeaSurfaceColour( float& r, float& g, float& b );

private:
	// draw colour mapped files
	void		DrawColourmapped( void ) const;
	void		DrawColourmapped( size_t numFaces, size_t* surfaces ) const;
	void		_Draw( void ) const;
	void 		_Draw( bool drawTransparencies ) const;

	void MergeFloats( size_t& orgSize, float*& forg, 
					  size_t newSize, float *fnew,
					  size_t elemSize );
	void MergeSizeT( size_t& orgSize, size_t*& forg, 
					 size_t newSize, size_t* fnew,
					 size_t elemSize, size_t offset );

	void DeleteAll( void );

	int		FindNeighbour( size_t orgSurface, size_t vert1, size_t vert2 );
	void	CreateSurfaceGraph( int* surfaceGraph );
	void	GetTriangles( size_t center, size_t t1, size_t t2, size_t t3,
						  size_t& t1s1, size_t& t1s2, size_t& t1s3,
						  size_t& t2s1, size_t& t2s2, size_t& t2s3 );
	float	Distance( size_t v1, size_t v2 );

private:
	friend class TBinObject;
	friend class TLandscapeObject;

	TString		name;

	bool		ownerDraw; // am I allowed to draw this mesh normally?

	size_t 		numVertices;
    float*		vertices;
    size_t		numFaces;
    size_t*		surfaces;

    size_t		numNormals;
    float*		normals;
    size_t		numNormalFaces;
    size_t*		normalFaces;

    size_t		numMaterials;
    size_t		numMaterialFaces;
    size_t*		materialFaces;
    TMaterial**	materials;
	size_t*		materialParentIndex;

    size_t		numTextureCoords;
    float*		textureCoords;

	long		objectSize;

	float		minX,minY,minZ;
	float		maxX,maxY,maxZ;
	float		centerX,centerY,centerZ;
	float		sizeX,sizeY,sizeZ;

	byte*		colourMap;
	size_t		colourMapWidth;
	size_t		colourMapHeight;

	bool		isColourMap;
	TTexture	bumpMap;
};

//==========================================================================

#endif

