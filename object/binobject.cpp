#include <precomp_header.h>

#include <win32/win32.h>
#include <object/geometry.h>
#include <object/material.h>
#include <object/mesh.h>
#include <object/binobject.h>

//==========================================================================

class ObjectCache
{
public:
	ObjectCache( void );
	ObjectCache( TBinObject* );
	~ObjectCache( void );

	void		AddObject( TBinObject* );
	TBinObject* GetObject( const TString& fname ) const;

private:
	ObjectCache*		next;
	TBinObject*			object;

public:
	static ObjectCache first;
};


ObjectCache ObjectCache::first;

ObjectCache::ObjectCache( void )
	: next(NULL),
	  object(NULL)
{
};


ObjectCache::ObjectCache( TBinObject* o )
	: next(NULL),
	  object( o )
{
};


ObjectCache::~ObjectCache( void )
{
	if ( object!=NULL )
		delete object;
	object = NULL;

	ObjectCache* list = next;
	while ( list!=NULL )
	{
		ObjectCache* temp = list;
		list = list->next;
		temp->next = NULL;
		delete temp;
	}
	next = NULL;
};


TBinObject* ObjectCache::GetObject( const TString& fname ) const
{
	ObjectCache* list = next;
	while ( list!=NULL )
	{
		if ( stricmp(fname.c_str(),list->object->Name().c_str())==0 )
			return list->object;
		list = list->next;
	}
	return NULL;
};

	
void ObjectCache::AddObject( TBinObject* o )
{
	if ( next==NULL )
	{
		next = new ObjectCache( o );
	}
	else
	{
		ObjectCache* list = next;
		bool found = false;
		while ( list->next!=NULL && !found )
		{
			if ( stricmp(o->Name().c_str(),list->object->Name().c_str())==0 )
			{
				found = true;
			}
			else
				list = list->next;
		}
		if ( !found )
			list->next = new ObjectCache( o );
	}
};

//==========================================================================

TMeshList::TMeshList( void )
	: next(NULL)
{
};


TMeshList::TMeshList( const TMesh& _mesh )
	: next(NULL)
{
	mesh = _mesh;
};

TMeshList::~TMeshList( void )
{
	TMeshList* list = next;
	while ( list!=NULL )
	{
		TMeshList* temp = list;
		list = list->next;
		temp->next = NULL;
		delete temp;
	};
    next = NULL;
};


TMeshList::TMeshList( const TMeshList& ml )
	: next(NULL)
{
	operator=(ml);
};


const TMeshList& TMeshList::operator=( const TMeshList& mlist )
{
	const TMeshList* ml = &mlist;
	TMeshList* curr = this;
	while ( ml!=NULL )
	{
		curr->mesh = ml->mesh;
		if ( curr->next==NULL && ml->next!=NULL )
			curr->next = new TMeshList();
		curr = curr->next;
		ml = ml->next;
	}
	return *this;
};

//==========================================================================

TMaterialList::TMaterialList( void )
	: next(NULL)
{
};


TMaterialList::TMaterialList( const TMaterial& _material )
	: next(NULL)
{
	material = _material;
};


TMaterialList::~TMaterialList( void )
{
	TMaterialList* list = next;
	while ( list!=NULL )
	{
		TMaterialList* temp = list;
		list = list->next;
		temp->next = NULL;
		delete temp;
	};
    next = NULL;
};


TMaterialList::TMaterialList( const TMaterialList& ml )
	: next(NULL)
{
	operator=(ml);
};


const TMaterialList& TMaterialList::operator=( const TMaterialList& mlist )
{
	const TMaterialList* ml = &mlist;
	TMaterialList* curr = this;
	while ( ml!=NULL )
	{
		curr->material = ml->material;
		if ( curr->next==NULL && ml->next!=NULL )
			curr->next = new TMaterialList();
		curr = curr->next;
		ml = ml->next;
	}
	return *this;
};

//==========================================================================

TBinObject::TBinObject( void )
	: materialList(NULL),
	  objectSize(0),
	  materialSize(0),
	  loaded(false),
      meshList(NULL)
{
	signature = "PDVX0002";
	minX = 0;
	minY = 0;
	minZ = 0;
	maxX = 0;
	maxY = 0;
	maxZ = 0;
	centerX = 0;
	centerY = 0;
	centerZ = 0;
	xSize = 0;
	ySize = 0;
	zSize = 0;
};



TBinObject::~TBinObject( void )
{
	DeleteAll();
};


bool TBinObject::Loaded( void ) const
{
	return loaded;
};


void TBinObject::DeleteAll( void )
{
    if ( materialList!=NULL )
		delete materialList;
	materialList = NULL;

    if ( meshList!=NULL )
		delete meshList;
	meshList = NULL;
};


void TBinObject::Resize( void )
{
	// go through all the meshes and find the min, max (x,y,z) to define
	// the box area of this object
    TMeshList* ml = meshList;
    while ( ml!=NULL )
    {
		TMesh& m = ml->mesh;
		if ( m.MinX()<minX )
			minX = m.MinX();
		if ( m.MinY()<minY )
			minY = m.MinY();
		if ( m.MinZ()<minZ )
			minZ = m.MinZ();
				
		if ( m.MaxX()>maxX )
			maxX = m.MaxX();
		if ( m.MaxY()>maxY )
			maxY = m.MaxY();
		if ( m.MaxZ()>maxZ )
			maxZ = m.MaxZ();

		ml = ml->next;
	}
	centerX = (minX + maxX) * 0.5f;
	centerY = (minY + maxY) * 0.5f;
	centerZ = (minZ + maxZ) * 0.5f;

	xSize = maxX - centerX;
	ySize = maxY - centerY;
	zSize = maxZ - centerZ;
}


TMaterialList* TBinObject::MaterialList( void )
{
	return materialList;
};


void TBinObject::MaterialList( TMaterialList* _materialList )
{
	materialList = _materialList;
};


TMeshList* TBinObject::MeshList( void )
{
	return meshList;
};



void TBinObject::MeshList( TMeshList* _meshList )
{
	meshList = _meshList;
};


long TBinObject::ObjectSize( void ) const
{
	return objectSize;
};


long TBinObject::MaterialSize( void ) const
{
	return materialSize;
};


bool TBinObject::SaveBinary( const TString& fname )
{
	TPersist file(fileWrite);
	if ( !file.FileOpen( fname ) )
		return false;

	bool success = SaveBinary( file );
	file.FileClose();
	return success;
};



bool TBinObject::SaveBinary( TPersist& file )
{
    // write signature to file
	file.FileWrite( signature.c_str(), signature.length()+1 );

    int num = NumMaterials();
	file.FileWrite( &num, sizeof(int) );
    num = NumMeshes();
	file.FileWrite( &num, sizeof(int) );

    TMaterialList* ml2 = materialList;
    while ( ml2!=NULL )
    {
    	ml2->material.SaveBinary(file);
    	ml2 = ml2->next;
    }

    TMeshList* ml1 = meshList;
    while ( ml1!=NULL )
    {
    	ml1->mesh.SaveBinary(file,this);
    	ml1 = ml1->next;
    }
    return true;
};


TBinObject* TBinObject::GetObject( const TString& fname, 
								   TString& errStr, 
								   const TString& pathname,
								   const TString& texturePath )
{
	// does it already exist in cache?
	// if so - just copy the cache item and return
	TString openName;
	ConstructPath( openName, pathname, fname );
	TBinObject* obj = ObjectCache::first.GetObject(openName);
	if ( obj!=NULL )
	{
		return obj;
	}
	else
	{
		TBinObject* obj = new TBinObject();
		PostCond( obj!=NULL );
		if ( !obj->LoadBinary( fname, errStr, pathname, texturePath ) )
		{
			delete obj;
			return NULL;
		}
		obj->Name( openName );
		ObjectCache::first.AddObject( obj );
		return obj;
	}
};


bool TBinObject::LoadBinary( const TString& fname, 
							 TString& errStr, 
							 const TString& pathname,
							 const TString& texturePath )
{
	TString openName;

	// texture path
	TString materialPath = texturePath;
	if ( materialPath.length()==0 )
		materialPath = pathname;

	ConstructPath( openName, pathname, fname );
	TPersist file(fileRead);
	if ( !file.FileOpen( openName ) )
	{
		errStr = file.ErrorString();
		return false;
	}

	name = openName;
	return LoadBinary( file, errStr, pathname, texturePath );
};



bool TBinObject::LoadBinary( TPersist& file, TString& errStr, 
							 const TString& pathname,
							 const TString& texturePath )
{
	int i;
	TString openName = name;

	// texture path
	TString materialPath = texturePath;
	if ( materialPath.length()==0 )
		materialPath = pathname;

    // read signature to file
	TString fileSignature;
	file.ReadString( fileSignature );
    if ( fileSignature!=signature )
    {
    	errStr = "Error incorrect file type";
		file.FileClose();
    	return false;
    }

    int numMaterials, numMeshes;
	file.FileRead( &numMaterials, sizeof(int) );
	file.FileRead( &numMeshes, sizeof(int) );

    for ( i=0; i<numMaterials; i++ )
    {
    	TMaterial mat;
		TString errorStr;
    	if ( !mat.LoadBinary( file, materialPath, errorStr ) )
	    {
	    	errStr = "Error loading material in object \"";
			errStr = errStr + openName;
			errStr = errStr + "\" (";
			errStr = errStr + errorStr;
			errStr = errStr + ")";
			file.FileClose();
	    	return false;
	    }
        AddMaterial( mat );
    }

    for ( i=0; i<numMeshes; i++ )
    {
    	TMesh mesh;
    	if ( !mesh.LoadBinary( file, this ) )
	    {
	    	errStr = "Error loading mesh in object \"";
			errStr = errStr + openName;
			errStr = errStr + "\"";
	    	return false;
	    }
        AddMesh( mesh );
    }

	objectSize = 0;
	TMaterialList* temp = materialList;
	while (temp!=NULL )
	{
		objectSize += sizeof(TMaterialList);
		objectSize += temp->material.ObjectSize();
		temp = temp->next;
	}
	materialSize = objectSize;

	TMeshList* ml = meshList;
	objectSize += sizeof(TMeshList);
	while (ml!=NULL )
	{
		objectSize += ml->mesh.ObjectSize();
		ml = ml->next;
	}
	Resize();
	loaded = true;
	return true;
};



void TBinObject::Information( bool detailed ) const
{

	// convert ObjectSize to MB, KB, or bytes
	char buf1[256];
	if ( ObjectSize() < 1024 )
		sprintf( buf1, "%d Bytes", ObjectSize() );
	else if ( ObjectSize() < (1024*1024) )
		sprintf( buf1, "%2.2f KB", double(ObjectSize()) / 1024.0 );
	else
		sprintf( buf1, "%2.2f MB", double(ObjectSize()) / 1048576.0 );

	char buf2[256];
	if ( MaterialSize() < 1024 )
		sprintf( buf2, "%d Bytes", MaterialSize() );
	else if ( MaterialSize() < (1024*1024) )
		sprintf( buf2, "%2.2f KB", double(MaterialSize()) / 1024.0 );
	else
		sprintf( buf2, "%2.2f MB", double(MaterialSize()) / 1048576.0 );

	WriteString( "file type: object\n");
    WriteString( "        material count    : %d\n", NumMaterials() );
    WriteString( "        mesh count        : %d\n", NumMeshes() );
    WriteString( "        total object size : %s\n", buf1 );
    WriteString( "        texture size      : %s\n\n", buf2 );

	if ( detailed )
	{
	    TMeshList* ml1 = meshList;
	    while ( ml1!=NULL )
	    {
    		ml1->mesh.Print();
    		ml1 = ml1->next;
		}

		TMaterialList* ml2 = materialList;
		while ( ml2!=NULL )
		{
	    	ml2->material.Print();
    		ml2 = ml2->next;
		}
	}
};



void TBinObject::AddMaterial( const TMaterial& mat )
{
	TMaterialList* temp = new TMaterialList( mat );
    TMaterialList* ml = materialList;
    if ( ml==NULL )
    	materialList = temp;
    else
    {
	    while ( ml->next!=NULL )
	    	ml = ml->next;
        ml->next = temp;
    }
};



TMaterial* TBinObject::GetMaterial( size_t index ) const
{
	size_t cntr = 0;
    TMaterialList* ml = materialList;
    while ( ml!=NULL )
    {
    	if ( cntr==index )
        	return &ml->material;
    	ml = ml->next;
        cntr++;
    }
    return NULL;
}



size_t TBinObject::GetMaterialIndex( TMaterial* mat ) const
{
    TMaterialList* ml = materialList;
	size_t cntr = 0;
    while ( ml!=NULL )
    {
    	if ( ml->material.MaterialName()==mat->MaterialName() )
        	return cntr;
    	ml = ml->next;
        cntr++;
    }
    return 0;
}



TMaterial* TBinObject::GetMaterial( const TString& name ) const
{
    TMaterialList* ml = materialList;
    while ( ml!=NULL )
    {
    	if ( ml->material.MaterialName()==name )
        	return &ml->material;
    	ml = ml->next;
    }
    return NULL;
};



size_t TBinObject::NumMaterials( void ) const
{
	size_t cntr = 0;
    TMaterialList* ml = materialList;
    while ( ml!=NULL )
    {
    	cntr++;
    	ml = ml->next;
    }
    return cntr;
};



void TBinObject::AddMesh( const TMesh& mesh )
{
	TMeshList* temp = new TMeshList( mesh );
    TMeshList* ml = meshList;

    if ( ml==NULL )
    	meshList = temp;
    else
    {
	    while ( ml->next!=NULL )
	    	ml = ml->next;
        ml->next = temp;
    }
};



TMesh* TBinObject::GetMesh( const TString& name ) const
{
    TMeshList* ml = meshList;
    while ( ml!=NULL )
    {
    	if ( ml->mesh.Name()==name )
        	return &ml->mesh;
    	ml = ml->next;
    }
    return NULL;
};



TMesh* TBinObject::GetMesh( size_t index ) const
{
	size_t cntr = 0;
    TMeshList* ml = meshList;
    while ( ml!=NULL )
    {
    	if ( cntr==index )
        	return &ml->mesh;
    	ml = ml->next;
        cntr++;
    }
    return NULL;
}



size_t TBinObject::NumMeshes( void ) const
{
	size_t cntr = 0;
    TMeshList* ml = meshList;
    while ( ml!=NULL )
    {
    	cntr++;
    	ml = ml->next;
    }
    return cntr;
};



void TBinObject::CullMeshesBackToOne( void )
{
	if ( meshList!=NULL )
	{
		if ( meshList->next!=NULL )
			delete meshList->next;
		meshList->next = NULL;
	}
};



void TBinObject::Draw( void ) const
{
    TMeshList* ml = meshList;
	while ( ml!=NULL )
	{
    	ml->mesh.Draw();
   		ml = ml->next;
	}
}


void TBinObject::Draw( const float* matrix ) const
{
    TMeshList* ml = meshList;
	while ( ml!=NULL )
	{
    	ml->mesh.Draw(matrix);
   		ml = ml->next;
	}
}


void TBinObject::Draw( float scale, const float* matrix ) const
{
    TMeshList* ml = meshList;
	while ( ml!=NULL )
	{
    	ml->mesh.Draw(scale,matrix);
   		ml = ml->next;
	}
}


void TBinObject::Draw( float scale ) const
{
    TMeshList* ml = meshList;
	while ( ml!=NULL )
	{
    	ml->mesh.Draw(scale);
   		ml = ml->next;
	}
}


void TBinObject::DrawNoMaterials( float scale ) const
{
    TMeshList* ml = meshList;
	while ( ml!=NULL )
	{
    	ml->mesh.DrawNoMaterials(scale);
   		ml = ml->next;
	}
}


void TBinObject::MatrixMult( const float* matrix )
{
    TMeshList* ml = meshList;
	while ( ml!=NULL )
	{
    	ml->mesh.MatrixMult(matrix);
   		ml = ml->next;
	}
};


void TBinObject::Draw( bool drawTransp ) const
{
    TMeshList* ml = meshList;
    while ( ml!=NULL )
    {
    	ml->mesh.Draw(drawTransp);
    	ml = ml->next;
    }
}


void TBinObject::AssimilateMaterials( const TBinObject& obj )
{
	// take over all its materials (the ones I don't have yet)
	for ( size_t i=0; i<obj.NumMaterials(); i++ )
	{
		const TMaterial* mat = obj.GetMaterial(i);
		if ( GetMaterial(mat->MaterialName() )==NULL )
			AddMaterial( *mat );
	};
};


void TBinObject::AssimilateMesh( TMesh* thisMesh, TMesh* mesh )
{
	size_t j, cntr;
	int offset;

	PreCond( mesh!=NULL );
	PreCond( thisMesh!=NULL );

	// copy the old vertices
	size_t numVerts = mesh->NumVertices() + thisMesh->NumVertices();
	size_t oldVSize = thisMesh->NumVertices()*3;
	if ( numVerts>0 )
	{
		float* nv = new float[numVerts*3];
		for ( j=0; j<oldVSize; j++ )
			nv[j] = thisMesh->Vertices()[j];
		cntr = 0;
		for ( j=oldVSize; j<(numVerts*3); j++ )
			nv[j] = mesh->Vertices()[cntr++];
		thisMesh->numVertices = numVerts;
		if ( thisMesh->vertices!=NULL )
			delete thisMesh->vertices;
		thisMesh->vertices = nv;
	}

	// copy the old surfaces
	size_t numSurfs = mesh->NumFaces() + thisMesh->NumFaces();
	if ( numSurfs>0 )
	{
		size_t* ns = new size_t[numSurfs*3];
		size_t oldSSize = thisMesh->NumFaces()*3;
		for ( j=0; j<oldSSize; j++ )
			ns[j] = thisMesh->Surfaces()[j];
		cntr = 0;
		offset = oldVSize / 3;
		for ( j=oldSSize; j<(numSurfs*3); j++ )
			ns[j] = mesh->Surfaces()[cntr++] + offset;
		thisMesh->numFaces = numSurfs;
		if ( thisMesh->surfaces!=NULL )
			delete thisMesh->surfaces;
		thisMesh->surfaces = ns;
	}

	// copy the old normals
	size_t numNorms = mesh->NumNormals() + thisMesh->NumNormals();
	if ( numNorms>0 )
	{
		float* nn = new float[numNorms*3];
		size_t oldNSize = thisMesh->NumNormals()*3;
		for ( j=0; j<oldNSize; j++ )
			nn[j] = thisMesh->Normals()[j];
		cntr = 0;
		for ( j=oldNSize; j<(numNorms*3); j++ )
			nn[j] = mesh->Normals()[cntr++];
		thisMesh->numNormals = numNorms;
		if ( thisMesh->normals!=NULL )
			delete thisMesh->normals;
		thisMesh->normals = nn;
	}

	// copy the old normal surfaces
	size_t numNormSurfs = mesh->NumNormalFaces() + thisMesh->NumNormalFaces();
	if ( numNormSurfs>0 )
	{
		size_t* nns = new size_t[numNormSurfs*3];
		size_t oldNSSize = thisMesh->NumNormalFaces()*3;
		for ( j=0; j<oldNSSize; j++ )
			nns[j] = thisMesh->NormalFaces()[j];
		cntr = 0;
		offset = oldNSSize / 3;
		for ( j=oldNSSize; j<(numNormSurfs*3); j++ )
			nns[j] = mesh->NormalFaces()[cntr++] + offset;
		thisMesh->numNormalFaces = numNormSurfs;
		if ( thisMesh->normalFaces!=NULL )
			delete thisMesh->normalFaces;
		thisMesh->normalFaces = nns;
	}

	// copy the old uvs
	size_t numUVS = mesh->NumTextureCoords() + thisMesh->NumTextureCoords();
	if ( numUVS>0 )
	{
		float* nuv = new float[numUVS*2];
		size_t oldUVSize = thisMesh->NumTextureCoords()*2;
		for ( j=0; j<oldUVSize; j++ )
			nuv[j] = thisMesh->TextureCoords()[j];
		cntr = 0;
		for ( j=oldUVSize; j<(numUVS*2); j++ )
			nuv[j] = mesh->TextureCoords()[cntr++];
		thisMesh->numTextureCoords = numUVS;
		if ( thisMesh->textureCoords!=NULL )
			delete thisMesh->textureCoords;
		thisMesh->textureCoords = nuv;
	}

	// copy the old materials
	size_t numMats = mesh->NumMaterials() + thisMesh->NumMaterials();
	size_t oldMatSize = thisMesh->NumMaterials();
	if ( numMats>0 )
	{
		TMaterial** mm = new TMaterial*[numMats];
		for ( j=0; j<oldMatSize; j++ )
			mm[j] = thisMesh->materials[j];
		cntr = 0;
		for ( j=oldMatSize; j<(numMats); j++ )
			mm[j] = mesh->materials[cntr++];
		thisMesh->numMaterials= numMats;
		if ( thisMesh->materials!=NULL )
			delete thisMesh->materials;
		thisMesh->materials = mm;
	}

	// copy the old material surfaces
	size_t numMatFaces = mesh->NumMaterialFaces() + thisMesh->NumMaterialFaces();
	if ( numMatFaces>0 )
	{
		size_t* nmf = new size_t[numMatFaces];
		size_t oldFSize = thisMesh->NumMaterialFaces();
		for ( j=0; j<oldFSize; j++ )
			nmf[j] = thisMesh->MaterialFaces()[j];
		cntr = 0;
		for ( j=oldFSize; j<(numMatFaces); j++ )
			nmf[j] = mesh->MaterialFaces()[cntr++] + oldMatSize;
		thisMesh->numMaterialFaces = numMatFaces;
		if ( thisMesh->materialFaces!=NULL )
			delete thisMesh->materialFaces;
		thisMesh->materialFaces = nmf;
	}
};


void TBinObject::MergeMeshesIntoOne( void )
{
	if ( meshList!=NULL )
	{
		TMeshList* list = meshList->next;
		while ( list!=NULL )
		{
			AssimilateMesh( &meshList->mesh, &list->mesh );
	        list = list->next;
	    }
	}
	delete meshList->next;
	meshList->next = NULL;
};


void TBinObject::PauseAnimations( bool tf )
{
    TMaterialList* ml = materialList;
    while ( ml!=NULL )
	{
		ml->material.PauseAnimation( tf );
    	ml = ml->next;
	}
};


TBinObject::TBinObject( const TBinObject& _binObject )
	: materialList(NULL),
	  objectSize(0),
	  materialSize(0),
      meshList(NULL)
{
	signature = "PDVX0001";
	operator=(_binObject);
};


void TBinObject::CenterAxes( bool cx, bool cy, bool cz )
{
	// go through all the meshes and find the min, max (x,y,z) to define
	// the box area of this object
	float minX, minY, minZ, maxX, maxY, maxZ;

    TMeshList* ml = meshList;
	bool firstTime = true;
    while ( ml!=NULL )
    {
		int numVertices = ml->mesh.NumVertices();
		float *vertices = ml->mesh.Vertices();
		for ( int i=0; i<numVertices; i++ )
		{
			float x = vertices[i*3+0];
			float y = vertices[i*3+1];
			float z = vertices[i*3+2];
			if ( firstTime )
			{
				firstTime = false;
				minX = x;
				minY = y;
				minZ = z;
				maxX = x;
				maxY = y;
				maxZ = z;
			}
			else
			{
				if ( x<minX )
					minX = x;
				if ( y<minY )
					minY = y;
				if ( z<minZ )
					minZ = z;
				
				if ( x>maxX )
					maxX = x;
				if ( y>maxY )
					maxY = y;
				if ( z>maxZ )
					maxZ = z;
			}
		}
   		ml = ml->next;
	}
	float centerX = (minX + maxX) / 2.0f;
	float centerY = (minY + maxY) / 2.0f;
	float centerZ = (minZ + maxZ) / 2.0f;

    ml = meshList;
    while ( ml!=NULL )
    {
		int numVertices = ml->mesh.NumVertices();
		float *vertices = ml->mesh.Vertices();
		for ( int i=0; i<numVertices; i++ )
		{
			float x = vertices[i*3+0];
			float y = vertices[i*3+1];
			float z = vertices[i*3+2];

			if ( cx )
				vertices[i*3+0] = x - centerX;
			if ( cy )
				vertices[i*3+1] = y - centerY;
			if ( cz )
				vertices[i*3+2] = z - centerZ;
		}
   		ml = ml->next;
	}
	Resize();
};


void TBinObject::Normalise( void )
{
	GetBounds( minX, minY, minZ, maxX, maxY, maxZ );
	Resize( minX, minY, minZ, maxX, maxY,maxZ );
};


void TBinObject::GetBounds( float& minX, float& minY, float& minZ,
						    float& maxX, float& maxY, float& maxZ )
{
	// go through all the meshes and find the min, max (x,y,z) to define
	// the box area of this object
    TMeshList* ml = meshList;
	bool firstTime = true;
    while ( ml!=NULL )
    {
		int numVertices = ml->mesh.NumVertices();
		float *vertices = ml->mesh.Vertices();
		for ( int i=0; i<numVertices; i++ )
		{
			float x = vertices[i*3+0];
			float y = vertices[i*3+1];
			float z = vertices[i*3+2];
			if ( firstTime )
			{
				firstTime = false;
				minX = x;
				minY = y;
				minZ = z;
				maxX = x;
				maxY = y;
				maxZ = z;
			}
			else
			{
				if ( x<minX )
					minX = x;
				if ( y<minY )
					minY = y;
				if ( z<minZ )
					minZ = z;
				
				if ( x>maxX )
					maxX = x;
				if ( y>maxY )
					maxY = y;
				if ( z>maxZ )
					maxZ = z;
			}
		}
   		ml = ml->next;
	}
};

void TBinObject::Resize( float minX, float minY, float minZ,
						 float maxX, float maxY, float maxZ )
{
	// get largest number of all
	float sizeX = float(fabs(minX));
	if ( float(fabs(maxX))>sizeX )
		sizeX = float(fabs(maxX));
	float sizeY = float(fabs(minY));
	if ( float(fabs(maxY))>sizeY )
		sizeY = float(fabs(maxY));
	float sizeZ = float(fabs(minZ));
	if ( float(fabs(maxZ))>sizeZ )
		sizeZ = float(fabs(maxZ));

	float divider = sizeX;
	if ( sizeY > divider )
		divider = sizeY;
	if ( sizeZ > divider )
		divider = sizeZ;

	minX = minX / divider;
	minY = minY / divider;
	minZ = minZ / divider;
	maxX = maxX / divider;
	maxY = maxY / divider;
	maxZ = maxZ / divider;

	centerX = (minX + maxX) / 2.0f;
	centerY = (minY + maxY) / 2.0f;
	centerZ = (minZ + maxZ) / 2.0f;

	sizeX = maxX - centerX;
	sizeY = maxY - centerY;
	sizeZ = maxZ - centerZ;

    TMeshList* ml = meshList;
    while ( ml!=NULL )
    {
		int numVertices = ml->mesh.NumVertices();
		float *vertices = ml->mesh.Vertices();
		for ( int i=0; i<numVertices; i++ )
		{
			float x = vertices[i*3+0];
			float y = vertices[i*3+1];
			float z = vertices[i*3+2];

			vertices[i*3+0] = x / divider;
			vertices[i*3+1] = y / divider;
			vertices[i*3+2] = z / divider;
		}

		ml->mesh.minX = minX / divider;
		ml->mesh.minY = minY / divider;
		ml->mesh.minZ = minZ / divider;
		ml->mesh.maxX = maxX / divider;
		ml->mesh.maxY = maxY / divider;
		ml->mesh.maxZ = maxZ / divider;

		ml->mesh.centerX = (minX + maxX) / 2.0f;
		ml->mesh.centerY = (minY + maxY) / 2.0f;
		ml->mesh.centerZ = (minZ + maxZ) / 2.0f;

		ml->mesh.sizeX = maxX - centerX;
		ml->mesh.sizeY = maxY - centerY;
		ml->mesh.sizeZ = maxZ - centerZ;

		ml = ml->next;
	}
}

const TBinObject& TBinObject::operator = ( const TBinObject& obj )
{
	DeleteAll();

	objectSize = obj.objectSize;
	materialSize = obj.materialSize;

	meshList = new TMeshList();
	materialList = new TMaterialList();

	*meshList = *obj.meshList;
	*materialList = *obj.materialList;

	name = obj.name;
	loaded = obj.loaded;

	minX = obj.minX;
	minY = obj.minY;
	minZ = obj.minZ;
	maxX = obj.maxX;
	maxY = obj.maxY;
	maxZ = obj.maxZ;
	centerX = obj.centerX;
	centerY = obj.centerY;
	centerZ = obj.centerZ;
	xSize = obj.xSize;
	ySize = obj.ySize;
	zSize = obj.zSize;

	return *this;
};


void TBinObject::CreateNewNormals( void )
{
    TMeshList* ml = meshList;
    while ( ml!=NULL )
    {
		ml->mesh.CreateNewNormals();
   		ml = ml->next;
	}
};


void TBinObject::ReverseWindings( void )
{
    TMeshList* ml = meshList;
    while ( ml!=NULL )
    {
		ml->mesh.ReverseWindings();
   		ml = ml->next;
	}
};


size_t TBinObject::NumSurfaces( void ) const
{
	size_t count = 0;

    TMeshList* ml = meshList;
    while ( ml!=NULL )
    {
		count += ml->mesh.NumFaces();
   		ml = ml->next;
	}
	return count;
};


void TBinObject::Name( const TString& _name )
{
	name = _name;
};


const TString& TBinObject::Name( void ) const
{
	return name;
};


float TBinObject::MinX( void ) const
{
	return minX;
}


float TBinObject::MinY( void ) const
{
	return minY;
}


float TBinObject::MinZ( void ) const
{
	return minZ;
}


float TBinObject::MaxX( void ) const
{
	return maxX;
}


float TBinObject::MaxY( void ) const
{
	return maxY;
}


float TBinObject::MaxZ( void ) const
{
	return maxZ;
}


void TBinObject::MinX( float t )
{
	minX = t;
};


void TBinObject::MinY( float t )
{
	minY = t;
};


void TBinObject::MinZ( float t )
{
	minZ = t;
};


void TBinObject::MaxX( float t )
{
	maxX = t;
};


void TBinObject::MaxY( float t )
{
	maxY = t;
};


void TBinObject::MaxZ( float t )
{
	maxZ = t;
};


void TBinObject::CenterX( float t )
{
	centerX = t;
};


void TBinObject::CenterY( float t )
{
	centerY = t;
};


void TBinObject::CenterZ( float t )
{
	centerZ = t;
};


float TBinObject::CenterX( void ) const
{
	return centerX;
};


float TBinObject::CenterY( void ) const
{
	return centerY;
};


float TBinObject::CenterZ( void ) const
{
	return centerZ;
};


float TBinObject::SizeX( void ) const
{
	return xSize;
};


float TBinObject::SizeY( void ) const
{
	return ySize;
};


float TBinObject::SizeZ( void ) const
{
	return zSize;
};


void TBinObject::SizeX( float sx )
{
	xSize = sx;
};


void TBinObject::SizeY( float sy )
{
	ySize = sy;
};


void TBinObject::SizeZ( float sz)
{
	zSize = sz;
};


void TBinObject::AdjustAxes( bool minx0, bool miny0, bool minz0, 
							 bool maxx0, bool maxy0, bool maxz0 )
{
	// go through all the meshes and find the min, max (x,y,z) to define
	// the box area of this object
	float minX, minY, minZ, maxX, maxY, maxZ;

    TMeshList* ml = meshList;
	bool firstTime = true;
    while ( ml!=NULL )
    {
		int numVertices = ml->mesh.NumVertices();
		float *vertices = ml->mesh.Vertices();
		for ( int i=0; i<numVertices; i++ )
		{
			float x = vertices[i*3+0];
			float y = vertices[i*3+1];
			float z = vertices[i*3+2];
			if ( firstTime )
			{
				firstTime = false;
				minX = x;
				minY = y;
				minZ = z;
				maxX = x;
				maxY = y;
				maxZ = z;
			}
			else
			{
				if ( x<minX )
					minX = x;
				if ( y<minY )
					minY = y;
				if ( z<minZ )
					minZ = z;
				
				if ( x>maxX )
					maxX = x;
				if ( y>maxY )
					maxY = y;
				if ( z>maxZ )
					maxZ = z;
			}
		}
   		ml = ml->next;
	}

    ml = meshList;
    while ( ml!=NULL )
    {
		ml->mesh.AdjustAxes( minX, maxX, minY,maxY, minZ,maxZ,
							 minx0, miny0, minz0, maxx0, maxy0, maxz0 );
   		ml = ml->next;
	}
};


void TBinObject::CalculateBoundaries( void )
{
    TMeshList* ml = meshList;
    while ( ml!=NULL )
    {
		ml->mesh.CalculateBoundaries();
   		ml = ml->next;
	};
	Resize();
};


void TBinObject::ResizeMesh( float bx1, float by1, float bz1,
							 float bx2, float by2, float bz2 )
{
    TMeshList* ml = meshList;
    while ( ml!=NULL )
    {
		ml->mesh.CalculateBoundaries();
   		ml = ml->next;
	};

	Resize();

    ml = meshList;
    while ( ml!=NULL )
    {
		ml->mesh.ResizeMesh( minX, minY, minZ, maxX, maxY, maxZ,
							 bx1, by1, bz1, bx2, by2, bz2 );
   		ml = ml->next;
	}
};


bool TBinObject::GetLORTriangle( float xp, float yp, float zp, float size, float& y )
{
	float dist = -10000;
	bool found = false;
    TMeshList* ml = meshList;
    while ( ml!=NULL )
    {
		float tempy;
		if ( ml->mesh.GetLORTriangle(xp,yp,zp,size,tempy) )
		{
			found = true;
			if ( tempy > dist )
			{
				dist = tempy;
				y = tempy;
			}
		}
   		ml = ml->next;
	};
	return found;
};


bool TBinObject::GetLORTriangle( const float* matrix,
								 float xp, float yp, float zp, 
								 float size, float& y )
{
	float dist = -10000;
	bool found = false;
    TMeshList* ml = meshList;
    while ( ml!=NULL )
    {
		float tempy;
		if ( ml->mesh.GetLORTriangle(matrix,xp,yp,zp,size,tempy) )
		{
			found = true;
			if ( tempy > dist )
			{
				dist = tempy;
				y = tempy;
			}
		}
   		ml = ml->next;
	};
	return found;
};


bool TBinObject::Collision( float l1x, float l1y, float l1z,
						    float l2x, float l2y, float l2z ) const
{
    TMeshList* ml = meshList;
    while ( ml!=NULL )
    {
		if ( ml->mesh.Collision( l1x, l1y, l1z, l2x, l2y, l2z ) )
			return true;
   		ml = ml->next;
	};
	return false;
}


bool TBinObject::Collision( const float* matrix,
						    float l1x, float l1y, float l1z,
						    float l2x, float l2y, float l2z ) const
{
    TMeshList* ml = meshList;
    while ( ml!=NULL )
    {
		if ( ml->mesh.Collision( matrix, l1x, l1y, l1z, l2x, l2y, l2z ) )
			return true;
   		ml = ml->next;
	};
	return false;
}


bool TBinObject::Collision( TMatrix& matrix,
						    float l1x, float l1y, float l1z,
						    float l2x, float l2y, float l2z )
{
    TMeshList* ml = meshList;
    while ( ml!=NULL )
    {
		if ( ml->mesh.Collision( matrix, l1x, l1y, l1z, l2x, l2y, l2z ) )
			return true;
   		ml = ml->next;
	};
	return false;
}


bool TBinObject::Collision( const float* matrix,
						    float l1x, float l1z,
						    float l2x, float l2z ) const
{
    TMeshList* ml = meshList;
    while ( ml!=NULL )
    {
		if ( ml->mesh.Collision( matrix, l1x, l1z, l2x, l2z ) )
			return true;
   		ml = ml->next;
	};
	return false;
}


void TBinObject::FlipUV( void )
{
    TMeshList* ml = meshList;
    while ( ml!=NULL )
    {
		ml->mesh.FlipUV();
   		ml = ml->next;
	};
};


void TBinObject::ManualTransform( void )
{
    TMeshList* ml = meshList;
    while ( ml!=NULL )
    {
		TString name = ml->mesh.name;
		name = name.lcase();
		if ( name!="landscapemesh" )
			ml->mesh.ManualTransform();
   		ml = ml->next;
	};
};


size_t TBinObject::NumVertices( void )
{
	size_t vc = 0;
    TMeshList* ml = meshList;
    while ( ml!=NULL )
    {
		vc += ml->mesh.NumVertices();
   		ml = ml->next;
	};
	return vc;
};


size_t TBinObject::NumFaces( void )
{
	size_t fc = 0;
    TMeshList* ml = meshList;
    while ( ml!=NULL )
    {
		fc += ml->mesh.NumFaces();
   		ml = ml->next;
	};
	return fc;
};


void TBinObject::HalfMesh( void )
{
    TMeshList* ml = meshList;
    while ( ml!=NULL )
    {
		ml->mesh.HalfMesh();
   		ml = ml->next;
	};
};


void TBinObject::Scale( float scale )
{
	// go through all the meshes and find the min, max (x,y,z) to define
	// the box area of this object
	float minX, minY, minZ, maxX, maxY, maxZ;

    TMeshList* ml = meshList;
	bool firstTime = true;
    while ( ml!=NULL )
    {
		int numVertices = ml->mesh.NumVertices();
		float *vertices = ml->mesh.Vertices();
		for ( int i=0; i<numVertices; i++ )
		{
			float x = vertices[i*3+0];
			float y = vertices[i*3+1];
			float z = vertices[i*3+2];
			if ( firstTime )
			{
				firstTime = false;
				minX = x;
				minY = y;
				minZ = z;
				maxX = x;
				maxY = y;
				maxZ = z;
			}
			else
			{
				if ( x<minX )
					minX = x;
				if ( y<minY )
					minY = y;
				if ( z<minZ )
					minZ = z;
				
				if ( x>maxX )
					maxX = x;
				if ( y>maxY )
					maxY = y;
				if ( z>maxZ )
					maxZ = z;
			}
		}
   		ml = ml->next;
	}

	float centerX = (minX + maxX) / 2.0f;
	float centerY = (minY + maxY) / 2.0f;
	float centerZ = (minZ + maxZ) / 2.0f;

    ml = meshList;
    while ( ml!=NULL )
    {
		int numVertices = ml->mesh.NumVertices();
		float *vertices = ml->mesh.Vertices();
		for ( int i=0; i<numVertices; i++ )
		{
			float x = vertices[i*3+0];
			float y = vertices[i*3+1];
			float z = vertices[i*3+2];

			x = (x-centerX) * scale + centerX;
			y = (y-centerY) * scale + centerY;
			z = (z-centerZ) * scale + centerZ;

			vertices[i*3+0] = x;
			vertices[i*3+1] = y;
			vertices[i*3+2] = z;
		}
   		ml = ml->next;
	}
};


bool TBinObject::SaveText( TString fname, TString& errStr )
{
	TString temp;

	temp = "xof 0302txt 0064\n\n";
	temp = temp + "Header {\n";
	temp = temp + "1; // Major version\n";
	temp = temp + "0; // Minor version\n";
	temp = temp + "1; // Flags\n";
	temp = temp + "}\n\n";

	temp = temp + "Material xof_default {\n";
	temp = temp + "0.400000;0.400000;0.400000;1.000000;;\n";
	temp = temp + "32.000000;\n";
	temp = temp + "0.700000;0.700000;0.700000;;\n";
	temp = temp + "0.000000;0.000000;0.000000;;\n";
	temp = temp + "}\n\n";


	TPersist file(fileWrite);
	if ( !file.FileOpen( fname ) )
	{
		errStr = "Error opening file " + fname + "\n";
		return false;
	}

	file.FileWrite( temp.c_str(), temp.length() );

    TMaterialList* ml = materialList;
    while ( ml!=NULL )
    {
		if ( !ml->material.SaveText(file,errStr) )
			return false;
   		ml = ml->next;
	};

    TMeshList* ml1 = meshList;
    while ( ml1!=NULL )
    {
		if ( !ml1->mesh.SaveText(file,this) )
			return false;
   		ml1 = ml1->next;
	};
	return true;
};

void TBinObject::MultiplyUV( float um, float vm )
{
    TMeshList* ml = meshList;
    while ( ml!=NULL )
    {
		ml->mesh.MultiplyUV(um,vm);
   		ml = ml->next;
	}
};

void TBinObject::DrawExplosion( size_t frame, size_t numFrames ) const
{
	float scale = (float(frame) / float(numFrames));

	glPushMatrix();

	glScalef( scale, scale, scale );

    TMeshList* ml = meshList;
	while ( ml!=NULL )
	{
   		ml->mesh.DrawExplosion( scale );
   		ml = ml->next;
	}

	glPopMatrix();
};

void TBinObject::Center( float cx, float cy, float cz )
{
    TMeshList* ml = meshList;
	while ( ml!=NULL )
	{
		ml->mesh.Center( cx,cy,cz );
   		ml = ml->next;
	}
};

bool TBinObject::Reload( TString& errStr )
{
    TMaterialList* ml = materialList;
	while ( ml!=NULL )
	{
		if ( !ml->material.Reload( errStr ) )
		{
			return false;
		}
   		ml = ml->next;
	}

	// and reset the child meshes' materials
    TMeshList* ml1 = meshList;
	while ( ml1!=NULL )
	{
		if ( !ml1->mesh.ReloadBumpmap(errStr) )
		{
			return false;
		}
		ml1->mesh.ResetMaterials( this );
   		ml1 = ml1->next;
	}

	return true;
};

bool TBinObject::SaveAsShadow( const TString& filename, TString& errStr )
{
	size_t numMeshes = 0;
	TMeshList* ml1 = meshList;
	while ( ml1!=NULL )
	{
		ml1 = ml1->next;
		numMeshes++;
	};

	if ( numMeshes!=1 )
	{
		errStr = "Incorrect number of meshes - must only have one mesh";
		return false;
	}

	TMesh* m = GetMesh( 0 );
	if ( m->NumVertices()==0 || m->NumFaces()==0 )
	{
		errStr = "Binobject mesh has no vertices or surfaces";
		return false;
	}

	TPersist file(fileWrite);
	if ( !file.FileOpen( filename ) )
	{
		errStr = file.ErrorString();
		return false;
	}
	else
	{
		TString sig = "PDV01SHD";
		file.FileWrite( sig.c_str(), sig.length()+1 );
		float angle = 0;
		float scale = 1;
		file.FileWrite( &angle, sizeof(float) );
		file.FileWrite( &scale, sizeof(float) );

		// convert vertices of existing mesh to 2D vertices
		size_t numVertices = m->NumVertices();
		float* vertices = new float[numVertices*2];
		float* v = m->Vertices();
		for ( size_t i=0; i<numVertices; i++ )
		{
			vertices[i*2+0] = v[i*3+0];
			vertices[i*2+1] = v[i*3+2];
		}

		file.FileWrite( &numVertices, sizeof(size_t) );
		file.FileWrite( vertices, sizeof(float) * numVertices * 2 );

		// cleanup
		delete[] vertices;

		size_t numSurfaces = m->NumFaces();
		file.FileWrite( &numSurfaces, sizeof(size_t) );
		file.FileWrite( m->Surfaces(), sizeof(size_t) * numSurfaces * 3 );

		file.FileClose();
	}
	return true;
};

//==========================================================================
