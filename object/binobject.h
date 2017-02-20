#ifndef __DXPARSER_BINOBJECT_H_
#define __DXPARSER_BINOBJECT_H_

//==========================================================================

#include <object/material.h>
#include <object/mesh.h>
#include <object/deffile.h>

//==========================================================================

class TMaterialList;
class TMeshList;
class TMapHeightInfo;
class TDefFile;
class TLandscape;

//==========================================================================

class _EXPORT TBinObject
{
public:
	TBinObject( void );
    ~TBinObject( void );

	TBinObject( const TBinObject& );
	const TBinObject& operator = ( const TBinObject& );

    // load & save binary routines for fast loading & saving
    virtual bool SaveBinary( const TString& fname );
	virtual bool SaveBinary( TPersist& file );
	virtual bool SaveAsShadow( const TString& filename, TString& errStr );

	// force materials to reload after resoltion reset
	virtual bool Reload( TString& errStr );

	bool SaveText( TString fname, TString& errStr );

	static TBinObject* GetObject( const TString& fname, 
								  TString& errStr, 
								  const TString& pathname,
								  const TString& texturePath );

    virtual bool LoadBinary( const TString& fname, 
							 TString& errStr, 
							 const TString& pathname,
							 const TString& texturePath );

	virtual bool LoadBinary( TPersist& file, TString& errStr, 
							 const TString& pathname,
							 const TString& texturePath );

    // display info about this object
    void		Information( bool detailed=false ) const;
	void		PauseAnimations( bool tf );

	// collision detection with this object - does the motion
	// from l1 to l2 cause a collision with this object?
	bool Collision( float l1x, float l1y, float l1z,
					float l2x, float l2y, float l2z ) const;
	bool Collision( const float* matrix,
					float l1x, float l1y, float l1z,
					float l2x, float l2y, float l2z ) const;
	bool Collision( TMatrix& matrix,
					float l1x, float l1y, float l1z,
					float l2x, float l2y, float l2z );
	bool Collision( const float* matrix,
					float l1x, float l1z,
					float l2x, float l2z ) const;

	// draw the complete object
	virtual void Draw( void ) const;
	virtual void Draw( float scale, const float* matrix ) const;
	virtual void Draw( const float* matrix ) const;
	virtual void Draw( bool drawTransp ) const;
	virtual void Draw( float scale ) const;
	void DrawNoMaterials( float scale ) const;

	// draw an explosion animation
	virtual void DrawExplosion( size_t frame, size_t numFrames ) const;

	void		Resize( void );

	// center all meshes around certain axis (optional)
	void CenterAxes( bool cx, bool cy, bool cz );
	void AdjustAxes( bool minx0, bool miny0, bool minz0, 
					 bool maxx0, bool maxy0, bool maxz0 );

	// statistically calculate normals
	void Normalise( void );

	// set mesh and submeshes to fit within this boundary box
	void CalculateBoundaries( void );
	void ResizeMesh( float bx1, float by1, float bz1,
					 float bx2, float by2, float bz2 );

	// resize the object physically by reducing all vertices located spacially
	// within threshold, return how many vertices were affected
	void HalfMesh( void );
	size_t NumVertices( void );
	size_t NumFaces( void );

	void FlipUV( void );
	void Scale( float scale ); // scale mesh by multiplying its vertices by scale

	// center object around a point
	void Center( float cx, float cy, float cz );

    // material & mesh interfaces
    void 		AddMaterial( const TMaterial& mat );
	TMaterial* 	GetMaterial( size_t index ) const;
    TMaterial*	GetMaterial( const TString& name ) const;
	size_t		GetMaterialIndex( TMaterial* mat ) const;
    size_t		NumMaterials( void) const;

    void		AddMesh( const TMesh& mesh );
    TMesh*		GetMesh( const TString& name ) const;
    TMesh*		GetMesh( size_t index) const;
    size_t		NumMeshes( void) const;

	TMaterialList*	MaterialList( void );
	void			MaterialList( TMaterialList* );

	TMeshList*		MeshList( void );
	void			MeshList( TMeshList* );

	long		ObjectSize( void ) const;
	long		MaterialSize( void ) const;

	// merge another obj with this one
	void AssimilateMaterials( const TBinObject& obj );
	void AssimilateMesh( TMesh* destinationMesh, TMesh* mesh );
	void CullMeshesBackToOne( void );
	void MergeMeshesIntoOne( void );

	void CreateNewNormals( void ); // create new normals based on std. algorithms
	void ReverseWindings( void ); // reverse surface windings
	void ManualTransform( void ); // manually define transform

	size_t NumSurfaces( void ) const; // return a count of total number of surfaces

	const TString&	Name( void ) const;
	void			Name( const TString& _name );

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

	void SizeX( float );
	void SizeY( float );
	void SizeZ( float );

	void MinX( float );
	void MinY( float );
	void MinZ( float );

	void MaxX( float );
	void MaxY( float );
	void MaxZ( float );

	void CenterX( float );
	void CenterY( float );
	void CenterZ( float );

	bool Loaded( void ) const;

	bool GetLORTriangle( float xp, float yp, float zp, float size, float& y );
	bool GetLORTriangle( const float* matrix,
						 float xp, float yp, float zp, 
						 float size, float& y );


	// multiply the vertices of the meshes by a transformation matrix
	void MatrixMult( const float* matrix );

	// multiply UV by these values
	void MultiplyUV( float um, float vm );

	// resize a mesh given these parameters
	void Resize( float minX, float minY, float minZ,
				 float maxX, float maxY, float maxZ );

	// get bounds (calculated on the fly)
	void GetBounds( float& minX, float& minY, float& minZ,
					float& maxX, float& maxY, float& maxZ );
private:
	void DeleteAll( void ); // delete meshes and materials

	// create binary mesh given an RGB with width & height
	static TBinObject* CreateBinaryMesh( byte* rgb, size_t w, size_t h,
										 size_t seaHeight, 
										 const TString& strip,
										 const TString& material,
										 TString& errStr );

private:
	friend class TLandscape;
	friend class TAnimation;

	TString			name;

	TMaterialList*	materialList;
    TMeshList*		meshList;

	// linked lists of materials and meshes
    TString			signature;
	long			objectSize;
	long			materialSize;
	bool			loaded;

	float minX, minY, minZ;
	float maxX, maxY, maxZ;
	float centerX, centerY, centerZ;
	float xSize, ySize, zSize;
};

//==========================================================================

class _EXPORT TMeshList
{
public:
	TMeshList( void );
	TMeshList( const TMesh& _mesh );
    ~TMeshList( void );

	TMeshList( const TMeshList& );
	const TMeshList& operator=( const TMeshList& );

    TMesh		mesh;
    TMeshList*	next;
};

//==========================================================================

class _EXPORT TMaterialList
{
public:
	TMaterialList( void );
	TMaterialList( const TMaterial& _material );
    ~TMaterialList( void );

	TMaterialList( const TMaterialList& );
	const TMaterialList& operator=( const TMaterialList& );

    TMaterial		material;
    TMaterialList*	next;
};

//==========================================================================
/*
#define kMaxHeightEntries	50
#define kMaxMaterialEntries	20

class TMapHeightInfo
{
public:
	TMapHeightInfo( void );
	~TMapHeightInfo( void );

	size_t Count( void ) const;
	void AddHeight( float height, size_t numMaterialIndexes, size_t* materialIndexes );
	void GetHeight( size_t index, float& height, size_t& numMat, size_t* materialIndexes );

private:
	size_t	count;		// num entries

	float	heights[kMaxHeightEntries];
	size_t	numMaterialIndexes[kMaxHeightEntries];
	size_t	materialIndexes[kMaxHeightEntries][kMaxMaterialEntries];
};

///==========================================================================///
*/
#endif

