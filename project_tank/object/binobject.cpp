#include <precomp_header.h>

#include <win32/win32.h>
#include <object/material.h>
#include <object/mesh.h>
#include <object/binobject.h>

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
//	if ( next!=NULL )
//		delete next;
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
		if ( curr->next==NULL )
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
//	if ( next!=NULL )
//		delete next;
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
		if ( curr->next==NULL )
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
      meshList(NULL)
{
	signature = "PDVX0001";
};



TBinObject::~TBinObject( void )
{
	DeleteAll();
};



void TBinObject::DeleteAll( void )
{
    while ( materialList!=NULL )
    {
        TMaterialList* temp = materialList;
        materialList = materialList->next;
        delete temp;
    }

    while ( meshList!=NULL )
    {
        TMeshList* temp = meshList;
        meshList = meshList->next;
        delete temp;
    }
};


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
    int index = 0;
    while ( signature[index]!=0 )
    {
		file.FileWrite( &signature[index], 1 );
	   	index++;
    }

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


bool TBinObject::LoadBinary( const TString& fname, TString& errStr, 
							 const TString& pathname,
							 const TString& texturePath )
{
	int i;
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

    // read signature to file
    int index = 0;
    bool correct = true;
    while ( signature[index]!=0 && correct )
    {
    	char ch;
		file.FileRead( &ch, 1 );
        if ( ch!=signature[index] )
        	correct = false;
    	index++;
    }
    if ( !correct )
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
	    	errStr = "Error loading material in object \"" + openName + "\" (" + errorStr + ")";
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
	    	errStr = "Error loading mesh in object \"" + openName + "\"";
			file.FileClose();
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
	return true;
};



bool TBinObject::LoadBinary( TPersist& file, TString& errStr, 
							 const TString& pathname,
							 const TString& texturePath )
{
	int i;
	char openName[256];

	// texture path
	TString materialPath = texturePath;
	if ( materialPath.length()==0 )
		materialPath = pathname;

    // read signature to file
    int index = 0;
    bool correct = true;
    while ( signature[index]!=0 && correct )
    {
    	char ch;
		file.FileRead( &ch, 1 );
        if ( ch!=signature[index] )
        	correct = false;
    	index++;
    }
    if ( !correct )
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

	ConsolePrint( "object:");
    ConsolePrint( "        materials   : %d", NumMaterials() );
    ConsolePrint( "        meshes      : %d", NumMeshes() );
    ConsolePrint( "        object size : %s", buf1 );
    ConsolePrint( "        materials   : %s\n", buf2 );

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
	size_t j;
	
	PreCond( mesh!=NULL );
	PreCond( thisMesh!=NULL );

	// copy the old vertices
	size_t numVerts = mesh->NumVertices() + thisMesh->NumVertices();
	float* nv = new float[numVerts*3];
	size_t oldVSize = thisMesh->NumVertices()*3;
	for ( j=0; j<oldVSize; j++ )
		nv[j] = thisMesh->Vertices()[j];
	size_t cntr = 0;
	for ( j=oldVSize; j<(numVerts*3); j++ )
		nv[j] = mesh->Vertices()[cntr++];
	thisMesh->numVertices = numVerts;
	delete thisMesh->vertices;
	thisMesh->vertices = nv;

	// copy the old surfaces
	size_t numSurfs = mesh->NumFaces() + thisMesh->NumFaces();
	size_t* ns = new size_t[numSurfs*3];
	size_t oldSSize = thisMesh->NumFaces()*3;
	for ( j=0; j<oldSSize; j++ )
		ns[j] = thisMesh->Surfaces()[j];
	cntr = 0;
	int offset = oldVSize / 3;
	for ( j=oldSSize; j<(numSurfs*3); j++ )
		ns[j] = mesh->Surfaces()[cntr++] + offset;
	thisMesh->numFaces = numSurfs;
	delete thisMesh->surfaces;
	thisMesh->surfaces = ns;

	// copy the old normals
	size_t numNorms = mesh->NumNormals() + thisMesh->NumNormals();
	float* nn = new float[numNorms*3];
	size_t oldNSize = thisMesh->NumNormals()*3;
	for ( j=0; j<oldNSize; j++ )
		nn[j] = thisMesh->Normals()[j];
	cntr = 0;
	for ( j=oldNSize; j<(numNorms*3); j++ )
		nn[j] = mesh->Normals()[cntr++];
	thisMesh->numNormals = numNorms;
	delete thisMesh->normals;
	thisMesh->normals = nn;

	// copy the old normal surfaces
	size_t numNormSurfs = mesh->NumNormalFaces() + thisMesh->NumNormalFaces();
	size_t* nns = new size_t[numNormSurfs*3];
	size_t oldNSSize = thisMesh->NumNormalFaces()*3;
	for ( j=0; j<oldNSSize; j++ )
		nns[j] = thisMesh->NormalFaces()[j];
	cntr = 0;
	offset = oldNSize / 3;
	for ( j=oldNSSize; j<(numNormSurfs*3); j++ )
		nns[j] = mesh->NormalFaces()[cntr++] + offset;
	thisMesh->numNormalFaces = numNormSurfs;
	delete thisMesh->normalFaces;
	thisMesh->normalFaces = nns;

	// copy the old uvs
	size_t numUVS = mesh->NumTextureCoords() + thisMesh->NumTextureCoords();
	float* nuv = new float[numUVS*2];
	size_t oldUVSize = thisMesh->NumTextureCoords()*2;
	for ( j=0; j<oldUVSize; j++ )
		nuv[j] = thisMesh->TextureCoords()[j];
	cntr = 0;
	for ( j=oldUVSize; j<(numUVS*2); j++ )
		nuv[j] = mesh->TextureCoords()[cntr++];
	thisMesh->numTextureCoords = numUVS;
	delete thisMesh->textureCoords;
	thisMesh->textureCoords = nuv;

	// copy the old materials
	size_t numMats = mesh->NumMaterials() + thisMesh->NumMaterials();
	TMaterial** mm = new TMaterial*[numMats];
	size_t oldMatSize = thisMesh->NumMaterials();
	for ( j=0; j<oldMatSize; j++ )
		mm[j] = thisMesh->materials[j];
	cntr = 0;
	for ( j=oldMatSize; j<(numMats); j++ )
		mm[j] = mesh->materials[cntr++];
	thisMesh->numMaterials= numMats;
	delete thisMesh->materials;
	thisMesh->materials = mm;

	// copy the old material surfaces
	size_t numMatFaces = mesh->NumMaterialFaces() + thisMesh->NumMaterialFaces();
	size_t* nmf = new size_t[numMatFaces];
	size_t oldFSize = thisMesh->NumMaterialFaces();
	for ( j=0; j<oldFSize; j++ )
		nmf[j] = thisMesh->MaterialFaces()[j];
	cntr = 0;
	for ( j=oldFSize; j<(numMatFaces); j++ )
		nmf[j] = mesh->MaterialFaces()[cntr++] + oldMatSize;
	thisMesh->numMaterialFaces = numMatFaces;
	delete thisMesh->materialFaces;
	thisMesh->materialFaces = nmf;
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
};


void TBinObject::Normalise( void )
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

	// get largest number of all
	minX = float(fabs(minX));
	minY = float(fabs(minY));
	minZ = float(fabs(minZ));
	maxX = float(fabs(maxX));
	maxY = float(fabs(maxY));
	maxZ = float(fabs(maxZ));

	float sizeX = minX;
	if ( maxX>sizeX )
		sizeX = maxX;
	float sizeY = minY;
	if ( maxY>sizeY )
		sizeY = maxY;
	float sizeZ = minZ;
	if ( maxZ>sizeZ )
		sizeZ = maxZ;

	float divider = sizeX;
	if ( sizeY > divider )
		divider = sizeY;
	if ( sizeZ > divider )
		divider = sizeZ;

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

			vertices[i*3+0] = x / divider;
			vertices[i*3+1] = y / divider;
			vertices[i*3+2] = z / divider;
		}
   		ml = ml->next;
	}
};


const TBinObject& TBinObject::operator = ( const TBinObject& obj )
{
	DeleteAll();

	objectSize = obj.objectSize;
	materialSize = obj.materialSize;

	*meshList = *obj.meshList;
	*materialList = *obj.materialList;
	
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

//==========================================================================
