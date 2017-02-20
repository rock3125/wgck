#ifndef __DXPARSER_MESH_H_
#define __DXPARSER_MESH_H_

///==========================================================================///

class TMaterial;
class TBinObject;

//==========================================================================

class TMesh
{
public:
	TMesh( void );
    ~TMesh( void );

	TMesh( const TMesh& mesh );
	const TMesh& operator=( const TMesh& );

    // load & save binary routines for fast loading & saving
	bool		SaveBinary( TPersist& file, const TBinObject* obj ) const;
    bool		LoadBinary( TPersist& file, const TBinObject* obj );

    // describe this mesh
	void Print( void ) const;

	void 		Draw( void ) const;
	void		Draw( size_t numFaces, size_t* surfaces, size_t* mats );
    
    const TString& Name( void ) const;
    void		Name( const TString& name );
    
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
    void		Materials( size_t index, TMaterial* ptr );

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

private:
	void MergeFloats( size_t& orgSize, float*& forg, 
					  size_t newSize, float *fnew,
					  size_t elemSize );
	void MergeSizeT( size_t& orgSize, size_t*& forg, 
					 size_t newSize, size_t* fnew,
					 size_t elemSize, size_t offset );

	void CalculateBoundaries( void );

	void DeleteAll( void );

private:
	friend class TBinObject;
	friend class TLandscapeObject;

	TString		name;

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

    size_t		numTextureCoords;
    float*		textureCoords;

	long		objectSize;

	float		minX,minY,minZ;
	float		maxX,maxY,maxZ;
	float		centerX,centerY,centerZ;
	float		sizeX,sizeY,sizeZ;
};

//==========================================================================

#endif

