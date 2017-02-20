#include <precomp_header.h>

#include <win32/win32.h>
#include <object/material.h>
#include <object/mesh.h>
#include <object/binobject.h>
#include <object/geometry.h>

//==========================================================================

TMesh::TMesh( void )
	: vertices(NULL),
	  surfaces(NULL),
	  materialFaces(NULL),
	  materials(NULL),
	  normals(NULL),
	  normalFaces(NULL),
	  textureCoords(NULL),
	  objectSize(0)
{
	numVertices			= 0;
    numFaces			= 0;

    numMaterials		= 0;
    numMaterialFaces	= 0;

    numNormals			= 0;
    numNormalFaces		= 0;

    numTextureCoords	= 0;
};


TMesh::TMesh( const TMesh& mesh )
	: vertices(NULL),
	  surfaces(NULL),
	  materialFaces(NULL),
	  materials(NULL),
	  normals(NULL),
	  normalFaces(NULL),
	  textureCoords(NULL),
	  objectSize(0)
{
	numVertices			= 0;
    numFaces			= 0;

    numMaterials		= 0;
    numMaterialFaces	= 0;

    numNormals			= 0;
    numNormalFaces		= 0;

    numTextureCoords	= 0;

	operator=(mesh);
};


const TMesh& TMesh::operator=( const TMesh& mesh )
{
	name = mesh.name;

	NumVertices( mesh.NumVertices() );
	Vertices( mesh.Vertices() );
	NumFaces( mesh.NumFaces() );
	Surfaces( mesh.Surfaces() );

	NumNormals( mesh.NumNormals() );
	Normals( mesh.Normals() );
	NumNormalFaces( mesh.NumNormalFaces() );
	NormalFaces( mesh.NormalFaces() );

	NumMaterials( mesh.NumMaterials() );
	NumMaterialFaces( mesh.NumMaterialFaces() );
	MaterialFaces( mesh.MaterialFaces() );

	for ( size_t i=0; i<numMaterials; i++ )
		Materials( i, mesh.Materials(i) );

	NumTextureCoords( mesh.NumTextureCoords() );
	TextureCoords( mesh.TextureCoords() );

	objectSize = mesh.objectSize;

	maxX = mesh.maxX;
	maxY = mesh.maxY;
	maxZ = mesh.maxZ;

	minX = mesh.minX;
	minY = mesh.minY;
	minZ = mesh.minZ;

	centerX = mesh.centerX;
	centerY = mesh.centerY;
	centerZ = mesh.centerZ;

	sizeX = mesh.sizeX;
	sizeY = mesh.sizeY;
	sizeZ = mesh.sizeZ;

	return *this;
};


TMesh::~TMesh( void )
{
	DeleteAll();
};


void TMesh::DeleteAll( void )
{
	if ( vertices!=NULL )
    	delete vertices;
    vertices = NULL;

	if ( surfaces!=NULL )
    	delete surfaces;
    surfaces = NULL;

	if ( materialFaces!=NULL )
    	delete materialFaces;
    materialFaces = NULL;

	if ( materials!=NULL )
    	delete []materials;
    materials = NULL;

	if ( normals!=NULL )
    	delete normals;
    normals = NULL;

	if ( normalFaces!=NULL )
    	delete normalFaces;
    normalFaces = NULL;

	if ( textureCoords!=NULL )
    	delete textureCoords;
    textureCoords = NULL;
}


void TMesh::GetBoundaries( float& minx, float& miny, float& minz,
						   float& maxx, float& maxy, float& maxz ) const
{
	minx = minX;
	miny = minY;
	minz = minZ;

	maxx = maxX;
	maxy = maxY;
	maxz = maxZ;
};


void TMesh::CalculateBoundaries( void )
{
	// go through all the meshes and find the min, max (x,y,z) to define
	// the box area of this object
	int numVertices = NumVertices();
	float *vertices = Vertices();
	bool firstTime = true;
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
	centerX = (minX + maxX) / 2.0f;
	centerY = (minY + maxY) / 2.0f;
	centerZ = (minZ + maxZ) / 2.0f;

	sizeX = maxX - centerX;
	sizeY = maxY - centerY;
	sizeZ = maxZ - centerZ;
}


long TMesh::ObjectSize( void ) const
{
	return objectSize;
};



void TMesh::RemoveDuplicates( void )
{
	// kill duplicates from the vertices & surfaces
	for ( size_t i=0; i<numVertices; i++ )
	for ( size_t j=i+1; j<numVertices; j++ )
	{
		if ( vertices[i*3+0]==vertices[j*3+0] &&
			 vertices[i*3+1]==vertices[j*3+1] &&
			 vertices[i*3+2]==vertices[j*3+2] )
		{
			// eliminate J from all surface & replace with i
			for ( size_t k=0; k<numFaces*3; k++ )
				if ( surfaces[k]==j )
					surfaces[k]=i;
		}
	};
}



bool TMesh::SaveBinary( TPersist& file, const TBinObject* obj ) const
{
	size_t i;

	file.FileWrite( name.c_str(), name.length()+1 );

    // vertices and their faces
    file.FileWrite( &numVertices, sizeof(size_t) );
	if ( numVertices>0 )
	    file.FileWrite( vertices, numVertices*3*sizeof(float) );
    file.FileWrite( &numFaces, sizeof(size_t) );
	if ( numFaces>0 )
		file.FileWrite( surfaces, numFaces*3*sizeof(size_t) );

    file.FileWrite( &numMaterialFaces, sizeof(size_t) );
	if ( numMaterialFaces>0 )
	    file.FileWrite( materialFaces, numMaterialFaces*sizeof(size_t) );

    // normals and their faces
    file.FileWrite( &numNormals, sizeof(size_t) );
	if ( numNormals>0 )
		file.FileWrite( normals, numNormals*3*sizeof(float) );
    file.FileWrite( &numNormalFaces, sizeof(size_t) );
	if ( numNormalFaces>0 )
		file.FileWrite( normalFaces, numNormalFaces*3*sizeof(size_t) );

    // texture coords
    file.FileWrite( &numTextureCoords, sizeof(size_t) );
	if ( numTextureCoords>0 )
	    file.FileWrite( textureCoords, numTextureCoords*2*sizeof(float) );

    // materials
    file.FileWrite( &numMaterials, sizeof(size_t) );
	if ( numMaterials>0 )
	{
		PreCond( obj!=NULL );
		for ( i=0; i<numMaterials; i++ )
		{
	    	TMaterial* mat = Materials( i );
			PostCond( mat!=NULL );
			size_t materialIndex = obj->GetMaterialIndex( mat );
			file.FileWrite( &materialIndex, sizeof(size_t) );
		}
	}
	return true;
};



bool TMesh::LoadBinary( TPersist& file, const TBinObject* obj )
{
	objectSize = 0;
	DeleteAll();

	TString buf;
	char ch;
    size_t index = 0;
    do
    {
    	file.FileRead( &ch, 1 );
        buf = buf + TString(ch);
		index++;
    }
    while ( ch!=0 && index<255 );
    Name( buf );
	objectSize += index;

    // vertices and their faces
    file.FileRead( &numVertices, sizeof(size_t) );
    NumVertices( numVertices );
	if ( numVertices>0 )
		file.FileRead( vertices, numVertices*3*sizeof(float) );
	objectSize += (numVertices*3*sizeof(float));

    file.FileRead( &numFaces, sizeof(size_t) );
	NumFaces( numFaces );
	if ( numFaces>0 )
		file.FileRead( surfaces, numFaces*3*sizeof(size_t) );
	objectSize += (numFaces*3*sizeof(size_t));

    file.FileRead( &numMaterialFaces, sizeof(size_t) );
    NumMaterialFaces( numMaterialFaces );
	if ( numMaterialFaces>0 )
	    file.FileRead( materialFaces, numMaterialFaces*sizeof(size_t) );
	objectSize += (numMaterialFaces*sizeof(size_t));

    // normals and their faces
    file.FileRead( &numNormals, sizeof(size_t) );
    NumNormals( numNormals );
	if ( numNormals>0 )
		file.FileRead( normals, numNormals*3*sizeof(float) );
	objectSize += (numNormals*3*sizeof(float));

    file.FileRead( &numNormalFaces, sizeof(size_t) );
    NumNormalFaces( numNormalFaces );
	if ( numNormalFaces>0 )
	    file.FileRead( normalFaces, numNormalFaces*3*sizeof(size_t) );
	objectSize += (numNormalFaces*3*sizeof(size_t));

    // texture coords
    file.FileRead( &numTextureCoords, sizeof(size_t) );
    NumTextureCoords( numTextureCoords );
	if ( numTextureCoords>0 )
		file.FileRead( textureCoords, numTextureCoords*2*sizeof(float) );
	objectSize += (numTextureCoords*2*sizeof(float));

    // materials
    file.FileRead( &numMaterials, sizeof(size_t) );
    NumMaterials( numMaterials );
	if ( numMaterials>0 )
	{
		PreCond( obj!=NULL );
	    for ( size_t i=0; i<numMaterials; i++ )
	    {
			size_t materialIndex;
			file.FileRead( &materialIndex, sizeof(size_t) );
	
    		TMaterial* mat = obj->GetMaterial( materialIndex );
			if ( mat==NULL )
	        	return false;

			objectSize += index;
			objectSize += sizeof(TMaterial);
			Materials( i, mat );
		}
	}

	// calculate object bounds
	CalculateBoundaries();

	return true;
};



void TMesh::Print( void ) const
{
	ConsolePrint( "mesh: %s", Name() );
    ConsolePrint( "      vertices        : %d", NumVertices() );
    ConsolePrint( "      surfaces        : %d", NumFaces() );
    ConsolePrint( "      materials       : %d", NumMaterials() );
    ConsolePrint( "      normals         : %d", NumNormals() );
    ConsolePrint( "      texture coords  : %d", NumTextureCoords() );
    ConsolePrint( "      object size     : %ld bytes", ObjectSize() );
};



const TString& TMesh::Name( void ) const
{
	return name;
};



void TMesh::Name( const TString& _name )
{
    name = _name;
};



size_t TMesh::NumVertices( void ) const
{
	return numVertices;
};



void TMesh::NumVertices( size_t val )
{
	numVertices = val;
    if ( vertices!=NULL )
    	delete vertices;
	if ( val>0 )
		vertices = new float[val*3];
	else
		vertices = NULL;
};



void TMesh::CullNumVertices( size_t newNumVertices )
{
	if ( newNumVertices==numVertices || newNumVertices==0 )
		return;

	PreCond( newNumVertices<numVertices );

/*
	float* newVertices = new float[newNumVertices*3];
	PostCond( newVertices!=NULL );

	for ( size_t i=0; i<(newNumVertices*3); i++ )
		newVertices[i] = vertices[i];
	if ( vertices!=NULL )
		delete []vertices;
	vertices = newVertices;
*/
	numVertices = newNumVertices;
};



void TMesh::CullUVs( size_t newNumTextureCoords )
{
	if ( newNumTextureCoords==numTextureCoords || newNumTextureCoords==0 )
		return;

	PreCond( newNumTextureCoords<numTextureCoords );
	numTextureCoords = newNumTextureCoords;
};



float* TMesh::Vertices( void ) const
{
	return vertices;
};



void TMesh::Vertices( float* ptr )
{
	if ( ptr!=NULL )
	{
		PreCond( vertices!=NULL );
		for ( size_t i=0; i<numVertices*3; i++ )
			vertices[i] = ptr[i];
	}
};



size_t TMesh::NumFaces( void ) const
{
	return numFaces;
};



void TMesh::NumFaces( size_t val )
{
	numFaces = val;
    if ( surfaces!=NULL )
    	delete surfaces;
	if ( val>0 )
		surfaces = new size_t[val*3];
	else
		surfaces = NULL;
};



size_t* TMesh::Surfaces( void ) const
{
	return surfaces;
};



void TMesh::Surfaces( size_t* ptr )
{
	if ( ptr!=NULL )
	{
		PreCond( surfaces!=NULL );
		for ( size_t i=0; i<numFaces*3; i++ )
			surfaces[i] = ptr[i];
	}
};



size_t TMesh::NumMaterials( void ) const
{
	return numMaterials;
};



void TMesh::NumMaterials( size_t val )
{
	numMaterials = val;

    if ( materials!=NULL )
    	delete []materials;
	if ( val>0 )
	{
		materials = new TMaterial*[val];
		for ( size_t i=0; i<val; i++ )
	    	materials[i] = NULL;
	}
	else
		materials = NULL;
};



size_t TMesh::NumMaterialFaces( void ) const
{
	return numMaterialFaces;
};



void TMesh::NumMaterialFaces( size_t val )
{
	numMaterialFaces = val;
    if ( materialFaces!=NULL )
    	delete materialFaces;
	if ( val>0 )
		materialFaces = new size_t[val];
	else
		materialFaces = NULL;
};



size_t* TMesh::MaterialFaces( void ) const
{
	return materialFaces;
};



void TMesh::MaterialFaces( size_t* ptr )
{
	if ( ptr!=NULL )
	{
		PreCond( materialFaces!=NULL );
		for ( size_t i=0; i<numMaterialFaces; i++ )
			materialFaces[i] = ptr[i];
	}
};



TMaterial* TMesh::Materials( size_t index ) const
{
	PreCond( index<numMaterials );
	return materials[index];
};



void TMesh::Materials( size_t index, TMaterial* ptr )
{
	PreCond( index<numMaterials );
	materials[index] = ptr;
};



size_t TMesh::NumNormals( void ) const
{
	return numNormals;
};



void TMesh::NumNormals( size_t val )
{
	numNormals = val;
    if ( normals!=NULL )
    	delete normals;
	if ( val>0 )
		normals = new float[val*3];
	else
		normals = NULL;
};



void TMesh::CullNumNormals( size_t newNumNormals )
{
	if ( newNumNormals==numNormals || newNumNormals==0 )
		return;

	PreCond( newNumNormals<numNormals );
/*
	float* newNormals = new float[newNumNormals*3];
	PostCond( newNormals!=NULL );

	for ( size_t i=0; i<(newNumNormals*3); i++ )
		newNormals[i] = normals[i];

	if ( normals!=NULL )
		delete []normals;
	normals = newNormals;
*/
	numNormals = newNumNormals;
};



float* TMesh::Normals( void ) const
{
	return normals;
};



void TMesh::Normals( float* ptr )
{
	if ( ptr!=NULL )
	{
		PreCond( normals!=NULL );
		for ( size_t i=0; i<numNormals*3; i++ )
			normals[i] = ptr[i];
	}
};



size_t TMesh::NumNormalFaces( void ) const
{
	return numNormalFaces;
};



void TMesh::NumNormalFaces( size_t val )
{
	numNormalFaces = val;
    if ( normalFaces!=NULL )
    	delete normalFaces;
	if ( val>0 )
		normalFaces = new size_t[val*3];
	else
		normalFaces = NULL;
};



size_t* TMesh::NormalFaces( void ) const
{
	return normalFaces;
};



void TMesh::NormalFaces( size_t* ptr )
{
	if ( ptr!=NULL )
	{
		PreCond( normalFaces!=NULL );
		for ( size_t i=0; i<numNormalFaces*3; i++ )
			normalFaces[i] = ptr[i];
	}
};



size_t TMesh::NumTextureCoords( void ) const
{
	return numTextureCoords;
};



void TMesh::NumTextureCoords( size_t val )
{
	numTextureCoords = val;
    if ( textureCoords!=NULL )
    	delete textureCoords;
	if ( val>0 )
		textureCoords = new float[val*2];
	else
		textureCoords = NULL;
};



float* TMesh::TextureCoords( void ) const
{
	return textureCoords;
};



void TMesh::TextureCoords( float* ptr )
{
	if ( ptr!=NULL )
	{
		PreCond( textureCoords!=NULL );
		for ( size_t i=0; i<numTextureCoords*2; i++ )
			textureCoords[i] = ptr[i];
	}
};



void TMesh::Draw( void ) const
{
	size_t  previousMaterialSurface = -1;
    bool usingTexture = false;
    bool usingNormals = false;

    if ( numNormals>0 )
    	usingNormals = true & useNormals;
        
	glEnable(GL_COLOR_MATERIAL);

	// for each surface in the mesh model
	glBegin(GL_TRIANGLES);
	for ( size_t j=0; j<numFaces; j++ )
	{
    	size_t normal;
        
        // get surface material details
        if ( numMaterialFaces>0 )
        {
			size_t matIndex = j;
			if ( matIndex>=numMaterialFaces )
				matIndex = 0;

        	size_t materialSurface = materialFaces[matIndex];
            if ( materialSurface!=previousMaterialSurface )
            {
            	glEnd();
            	previousMaterialSurface = materialSurface;
                TMaterial* mat = materials[ materialSurface ];
                if ( mat!=NULL )
                {
//                    glMaterialfv(GL_FRONT,GL_SPECULAR, mat->Power() );
//                    glMaterialfv(GL_FRONT,GL_SHININESS, mat->Specularity() );
//                    glMaterialfv(GL_FRONT,GL_EMISSION, mat->Emission() );

                    if ( mat->Texture().Used() )
                    {
	                    glColor3f( 1,1,1 );
                    	mat->Texture().StartTexture();
                        usingTexture = true;
                    }
                    else if ( usingTexture )
                    {
                    	TTexture::EndTexture();
                    	usingTexture = false;
                    }

                    if ( !usingTexture )
					{
						glColor3fv( mat->ColourRGB() );
					}
                }
				glBegin(GL_TRIANGLES);
            }
        }

		size_t surface = surfaces[ j*3 ];
		if ( usingTexture )
			glTexCoord2fv( &textureCoords[surface*2] );
        if ( usingNormals )
        {
        	normal  = normalFaces[ j*3 ];
			glNormal3fv( &normals[normal*3] );
        }
		glVertex3fv( &vertices[surface*3] );

		surface = surfaces[ j*3+1 ];
		if ( usingTexture )
			glTexCoord2fv( &textureCoords[surface*2] );
        if ( usingNormals )
        {
        	normal  = normalFaces[ j*3+1 ];
			glNormal3fv( &normals[normal*3] );
        }
		glVertex3fv( &vertices[surface*3] );

		surface = surfaces[ j*3+2 ];
		if ( usingTexture )
			glTexCoord2fv( &textureCoords[surface*2] );
        if ( usingNormals )
        {
        	normal  = normalFaces[ j*3+2 ];
			glNormal3fv( &normals[normal*3] );
        }
		glVertex3fv( &vertices[surface*3] );
	}
	glEnd();

    // reset settings
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_COLOR_MATERIAL);
    glColor3f(1,1,1);
}



void TMesh::Draw( size_t numFaces, size_t* surfaces, size_t* mats )
{
	size_t  previousMaterialSurface = -1;
    bool usingTexture = false;
    bool usingNormals = false;

    if ( numNormals>0 )
    	usingNormals = true & useNormals;
        
	glEnable(GL_COLOR_MATERIAL);

	// for each surface in the mesh model
	glBegin(GL_TRIANGLES);
	for ( size_t j=0; j<numFaces; j++ )
	{
    	size_t normal;
        
        // get surface material details
        {
        	size_t materialSurface = mats[j];
            if ( materialSurface!=previousMaterialSurface )
            {
            	glEnd();
            	previousMaterialSurface = materialSurface;
                TMaterial* mat = materials[ materialSurface ];
                if ( mat!=NULL )
                {
//                    glMaterialfv(GL_FRONT,GL_SPECULAR, mat->Power() );
//                    glMaterialfv(GL_FRONT,GL_SHININESS, mat->Specularity() );
//                    glMaterialfv(GL_FRONT,GL_EMISSION, mat->Emission() );

                    if ( mat->Texture().Used() )
                    {
	                    glColor3f( 1,1,1 );
                    	mat->Texture().StartTexture();
                        usingTexture = true;
                    }
                    else if ( usingTexture )
                    {
                    	TTexture::EndTexture();
                    	usingTexture = false;
                    }

                    if ( !usingTexture )
					{
						glColor3fv( mat->ColourRGB() );
					}
                }
				glBegin(GL_TRIANGLES);
            }
        }

		size_t surface = surfaces[ j*3 ];

		if ( usingTexture )
			glTexCoord2fv( &textureCoords[surface*2] );
        if ( usingNormals )
        {
        	normal  = normalFaces[ j*3 ];
			glNormal3fv( &normals[normal*3] );
        }
		glVertex3fv( &vertices[surface*3] );

		surface = surfaces[ j*3+1 ];
		if ( usingTexture )
			glTexCoord2fv( &textureCoords[surface*2] );
        if ( usingNormals )
        {
        	normal  = normalFaces[ j*3+1 ];
			glNormal3fv( &normals[normal*3] );
        }
		glVertex3fv( &vertices[surface*3] );

		surface = surfaces[ j*3+2 ];
		if ( usingTexture )
			glTexCoord2fv( &textureCoords[surface*2] );
        if ( usingNormals )
        {
        	normal  = normalFaces[ j*3+2 ];
			glNormal3fv( &normals[normal*3] );
        }
		glVertex3fv( &vertices[surface*3] );
	}
	glEnd();

    // reset settings
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_COLOR_MATERIAL);
    glColor3f(1,1,1);
}



void TMesh::MergeFloats( size_t& orgSize, float*& forg, 
						 size_t newSize, float *fnew,
						 size_t elemSize )
{
	size_t i,start,end;

	size_t total = orgSize + newSize;
	float* newArray = new float[total*elemSize];
	PostCond( newArray!=NULL );

	end = orgSize*elemSize;
	for ( i=0; i<end; i++ )
		newArray[i] = forg[i];

	start = orgSize*elemSize;
	end = total*elemSize;
	for ( i=start; i<end; i++ )
		newArray[i] = fnew[i-start];

	// switch arrays
	delete forg;
	forg = newArray;
	orgSize = total;
}



void TMesh::MergeSizeT( size_t& orgSize, size_t*& forg, 
						size_t newSize, size_t* fnew,
						size_t elemSize, size_t offset )
{
	size_t i,start,end;

	size_t total = orgSize + newSize;
	size_t* newArray = new size_t[total*elemSize];
	PostCond( newArray!=NULL );

	end = orgSize*elemSize;
	for ( i=0; i<end; i++ )
		newArray[i] = forg[i];

	start = orgSize*elemSize;
	end = total*elemSize;
	for ( i=start; i<end; i++ )
		newArray[i] = offset + fnew[i-start];

	// switch arrays
	delete forg;
	forg = newArray;
	orgSize = total;
}



void TMesh::MergeMesh( TMesh* mesh )
{
	size_t i;

	PreCond( mesh!=NULL );
	PreCond( numTextureCoords==numVertices );
	PreCond( mesh->NumTextureCoords()==mesh->NumVertices() );

	size_t oldVertices = numVertices;
	MergeFloats( numVertices, vertices,
				 mesh->NumVertices(),mesh->Vertices(),3 );
	MergeSizeT( numFaces, surfaces, mesh->NumFaces(),
				mesh->Surfaces(), 3, oldVertices );

	size_t oldNormals = numNormals;
	MergeFloats( numNormals, normals, mesh->NumNormals(),
				 mesh->Normals(), 3 );
	MergeSizeT( numNormalFaces, normalFaces, mesh->NumNormalFaces(),
				mesh->NormalFaces(), 3, oldNormals);

	MergeFloats( numTextureCoords, textureCoords,
				 mesh->NumTextureCoords(), mesh->TextureCoords(), 2 );
	
	// copy materials from mesh into my mesh - just a straight append
	// no size optimisations here
	size_t newNumMaterials = mesh->NumMaterials()+numMaterials;
	size_t oldNumMaterials = numMaterials;
    TMaterial** newMaterials = new TMaterial*[newNumMaterials];
    for ( i=0; i<numMaterials; i++ )
    	newMaterials[i] = materials[i];
	for ( i=numMaterials; i<newNumMaterials; i++ )
		newMaterials[i] = mesh->Materials(i-numMaterials);
	delete materials;
	materials = newMaterials;
	numMaterials = newNumMaterials;

	// get material faces & re-align them
	MergeSizeT( numMaterialFaces, materialFaces, mesh->NumMaterialFaces(),
				mesh->MaterialFaces(), 1, oldNumMaterials );
};


float TMesh::MinX( void ) const
{
	return minX;
}


float TMesh::MinY( void ) const
{
	return minY;
}


float TMesh::MinZ( void ) const
{
	return minZ;
}


float TMesh::MaxX( void ) const
{
	return maxX;
}


float TMesh::MaxY( void ) const
{
	return maxY;
}


float TMesh::MaxZ( void ) const
{
	return maxZ;
}


float TMesh::CenterX( void ) const
{
	return centerX;
}


float TMesh::CenterY( void ) const
{
	return centerY;
}


float TMesh::CenterZ( void ) const
{
	return centerZ;
}


float TMesh::SizeX( void ) const
{
	return sizeX;
}


float TMesh::SizeY( void ) const
{
	return sizeY;
}


float TMesh::SizeZ( void ) const
{
	return sizeZ;
}


void TMesh::CreateNewNormals( void )
{
	size_t i;

	// for each surface, calculate a normal
	NumNormalFaces( numFaces );
	NumNormals( numVertices );
	for ( i=0; i<numFaces; i++ )
	{
		size_t t1 = surfaces[i*3+0];
		size_t t2 = surfaces[i*3+1];
		size_t t3 = surfaces[i*3+2];

		float nx,ny,nz;

		Normal( nx,ny,nz,
				vertices[t1*3+0],vertices[t1*3+1],vertices[t1*3+2],
				vertices[t2*3+0],vertices[t2*3+1],vertices[t2*3+2],
				vertices[t3*3+0],vertices[t3*3+1],vertices[t3*3+2] );

		normals[t1*3+0] = nx;
		normals[t1*3+1] = ny;
		normals[t1*3+2] = nz;

		normals[t2*3+0] = nx;
		normals[t2*3+1] = ny;
		normals[t2*3+2] = nz;

		normals[t3*3+0] = nx;
		normals[t3*3+1] = ny;
		normals[t3*3+2] = nz;

		normalFaces[i*3+0] = t1;
		normalFaces[i*3+1] = t2;
		normalFaces[i*3+2] = t3;
	}

	// average the normals for each vertex in its three surfaces
	ResetPercentage();
	for ( i=0; i<numVertices; i++ )
	{
		DisplayPercentage( i, numVertices );

		// find 3 surfaces this vertex is used in and average it
		size_t t1 = -1;
		size_t t2 = -1;
		size_t t3 = -1;

		size_t j = 0;
		bool found = false;
		while ( j<numFaces && !found )
		{
			if ( normalFaces[j*3+0]==i )
			{
				found = true;
				t1 = normalFaces[j*3+0];
			}
			if ( normalFaces[j*3+1]==i && !found )
			{
				found = true;
				t1 = normalFaces[j*3+1];
			}
			if ( normalFaces[j*3+2]==i && !found )
			{
				found = true;
				t1 = normalFaces[j*3+2];
			}
			j++;
		}

		j = 0;
		found = false;
		while ( j<numFaces && !found )
		{
			if ( normalFaces[j*3+0]==i && t1!=i )
			{
				found = true;
				t2 = normalFaces[j*3+0];
			}
			if ( normalFaces[j*3+1]==i && !found && t1!=i )
			{
				found = true;
				t2 = normalFaces[j*3+1];
			}
			if ( normalFaces[j*3+2]==i && !found && t1!=i )
			{
				found = true;
				t2 = normalFaces[j*3+2];
			}
			j++;
		}

		j = 0;
		found = false;
		while ( j<numFaces && !found )
		{
			if ( normalFaces[j*3+0]==i && t1!=i && t2!=i )
			{
				found = true;
				t3 = normalFaces[j*3+0];
			}
			if ( normalFaces[j*3+1]==i && !found && t1!=i && t2!=i )
			{
				found = true;
				t3 = normalFaces[j*3+1];
			}
			if ( normalFaces[j*3+2]==i && !found && t1!=i && t2!=i )
			{
				found = true;
				t3 = normalFaces[j*3+2];
			}
			j++;
		}

		float nx,ny,nz;
		float count = 1.0f;
		PreCond( t1!=-1 );
		nx = normals[t1*3+0];
		ny = normals[t1*3+1];
		nz = normals[t1*3+2];

		if ( t2!=-1 )
		{
			count = 0.5f;
			nx += normals[t2*3+0];
			ny += normals[t2*3+1];
			nz += normals[t2*3+2];
		}

		if ( t3!=-1 )
		{
			if ( count==0.5f )
				count = 0.33f;
			else
				count = 0.5f;

			nx += normals[t3*3+0];
			ny += normals[t3*3+1];
			nz += normals[t3*3+2];
		}

		nx = nx * count;
		ny = ny * count;
		nz = nz * count;

		normals[t1*3+0] = nx;
		normals[t1*3+1] = ny;
		normals[t1*3+2] = nz;

		if ( t2!=-1 )
		{
			normals[t2*3+0] = nx;
			normals[t2*3+1] = ny;
			normals[t2*3+2] = nz;
		}

		if ( t3!=-1 )
		{
			normals[t3*3+0] = nx;
			normals[t3*3+1] = ny;
			normals[t3*3+2] = nz;
		}
	}
	FinishPercentage();
};

//==========================================================================


