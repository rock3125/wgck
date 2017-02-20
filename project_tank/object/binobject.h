#ifndef __DXPARSER_BINOBJECT_H_
#define __DXPARSER_BINOBJECT_H_

//==========================================================================

#include <object/material.h>
#include <object/mesh.h>

//==========================================================================

class TMaterialList;
class TMeshList;

//==========================================================================

class TBinObject
{
public:
	TBinObject( void );
    ~TBinObject( void );

	TBinObject( const TBinObject& );
	const TBinObject& operator = ( const TBinObject& );

    // load & save binary routines for fast loading & saving
    virtual bool SaveBinary( const TString& fname );
	virtual bool SaveBinary( TPersist& file );
    virtual bool LoadBinary( const TString& fname, TString& errStr, 
							 const TString& pathname,
							 const TString& texturePath );
	virtual bool LoadBinary( TPersist& file, TString& errStr, 
							 const TString& pathname,
							 const TString& texturePath );

    // display info about this object
    void		Information( bool detailed=false ) const;
	void		PauseAnimations( bool tf );

	// draw the complete object
	virtual void Draw( void ) const;

	// center all meshes around certain axis (optional)
	void CenterAxes( bool cx, bool cy, bool cz );
	void Normalise( void );

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

	size_t NumSurfaces( void ) const; // return a count of total number of surfaces

public:
	TMaterialList*	materialList;
    TMeshList*		meshList;

private:
	void DeleteAll( void ); // delete meshes and materials

private:
	friend class TLandscape;

	// linked lists of materials and meshes
    TString			signature;
	long			objectSize;
	long			materialSize;
};

//==========================================================================

class TMeshList
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

class TMaterialList
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

#endif

