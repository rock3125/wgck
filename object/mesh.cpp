#include <precomp_header.h>

#include <win32/win32.h>
#include <object/material.h>
#include <object/mesh.h>
#include <object/binobject.h>
#include <object/geometry.h>

#define _USEMATERIALPROPERTIES

//==========================================================================

TMesh::TMesh( void )
	: vertices(NULL),
	  surfaces(NULL),
	  materialFaces(NULL),
	  materials(NULL),
	  materialParentIndex(NULL),
	  normals(NULL),
	  normalFaces(NULL),
	  textureCoords(NULL),
	  colourMap(NULL),
	  objectSize(0)
{
	isColourMap			= false;
	ownerDraw			= true;

	numVertices			= 0;
    numFaces			= 0;

    numMaterials		= 0;
    numMaterialFaces	= 0;

    numNormals			= 0;
    numNormalFaces		= 0;

    numTextureCoords	= 0;

	colourMapWidth		= 0;
	colourMapHeight		= 0;

	maxX				= 0;
	maxY				= 0;
	maxZ				= 0;

	minX				= 0;
	minY				= 0;
	minZ				= 0;

	centerX				= 0;
	centerY				= 0;
	centerZ				= 0;

	sizeX				= 0;
	sizeY				= 0;
	sizeZ				= 0;
};


TMesh::TMesh( const TMesh& mesh )
	: vertices(NULL),
	  surfaces(NULL),
	  materialFaces(NULL),
	  materials(NULL),
	  materialParentIndex(NULL),
	  normals(NULL),
	  normalFaces(NULL),
	  textureCoords(NULL),
	  colourMap(NULL),
	  objectSize(0)
{
	isColourMap			= false;
	ownerDraw			= true;

	numVertices			= 0;
    numFaces			= 0;

    numMaterials		= 0;
    numMaterialFaces	= 0;

    numNormals			= 0;
    numNormalFaces		= 0;

    numTextureCoords	= 0;

	colourMapWidth		= 0;
	colourMapHeight		= 0;

	maxX				= 0;
	maxY				= 0;
	maxZ				= 0;

	minX				= 0;
	minY				= 0;
	minZ				= 0;

	centerX				= 0;
	centerY				= 0;
	centerZ				= 0;

	sizeX				= 0;
	sizeY				= 0;
	sizeZ				= 0;

	operator=(mesh);
};


const TMesh& TMesh::operator=( const TMesh& mesh )
{
	name = mesh.name;
	ownerDraw = mesh.ownerDraw;

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
		Materials( i, mesh.MaterialParentIndex(i), mesh.Materials(i) );

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

	isColourMap = mesh.isColourMap;

	colourMapWidth = mesh.colourMapWidth;
	colourMapHeight = mesh.colourMapHeight;

	if ( colourMap!=NULL )
		delete []colourMap;
	colourMap = NULL;
	if ( mesh.colourMap!=NULL )
	{
		size_t s = colourMapWidth*colourMapHeight*3;
		colourMap = new byte[s];
		PostCond( colourMap!=NULL );
		for ( i=0; i<s; i++ )
			colourMap[i] = mesh.colourMap[i];
	}

	bumpMap = mesh.bumpMap;

	return *this;
};


TMesh::~TMesh( void )
{
	DeleteAll();
};


void TMesh::DeleteAll( void )
{
	if ( vertices!=NULL )
    	delete []vertices;
    vertices = NULL;

	if ( surfaces!=NULL )
    	delete []surfaces;
    surfaces = NULL;

	if ( materialFaces!=NULL )
    	delete []materialFaces;
    materialFaces = NULL;

	if ( materials!=NULL )
    	delete []materials;
    materials = NULL;

	if ( materialParentIndex!=NULL )
    	delete []materialParentIndex;
    materialParentIndex = NULL;

	if ( normals!=NULL )
    	delete []normals;
    normals = NULL;

	if ( normalFaces!=NULL )
    	delete []normalFaces;
    normalFaces = NULL;

	if ( textureCoords!=NULL )
    	delete []textureCoords;
    textureCoords = NULL;

	if ( colourMap!=NULL )
		delete []colourMap;
	colourMap = NULL;
}


bool TMesh::GetLORTriangle( float xp, float yp, float zp, float size, float& y )
{
	return Collision( numFaces, surfaces, xp, yp, zp, size, y );
};


bool TMesh::GetLORTriangle( const float* matrix, 
						    float xp, float yp, float zp, 
							float size, float& y )
{
	return Collision( matrix, numFaces, surfaces, xp, yp, zp, size, y );
};


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
	if ( isColourMap )
		numVertices--;
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
	centerX = (minX + maxX) * 0.5f;
	centerY = (minY + maxY) * 0.5f;
	centerZ = (minZ + maxZ) * 0.5f;

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


bool TMesh::SaveText( TPersist& file, const TBinObject* obj ) const
{
	size_t i;

	//Mesh NoName10 {
	//	3;		// 3 vertices
	//	0.078558;0.387928;-0.077368;,

	TString temp;
	temp = "Mesh " + name + " {\n";
	file.FileWrite( temp.c_str(), temp.length() );

    // vertices and their faces
	temp = "\t" + Int2Str(numVertices) + "; // number of vertices\n";
	file.FileWrite( temp.c_str(), temp.length() );

	if ( numVertices>0 )
	{
		for ( i=0; i<numVertices; i++ )
		{
			temp = "\t" + FloatToString(1,6,vertices[i*3+0]) + ";";
			temp = temp + FloatToString(1,6,vertices[i*3+1]) + ";";
			temp = temp + FloatToString(1,6,vertices[i*3+2]);
			if ( (i+1)==numVertices )
				temp = temp + ";;\n\n";
			else
				temp = temp + ";,\n";
			file.FileWrite( temp.c_str(), temp.length() );
		}
	}

	//	2;		// 2 faces
	//	3;0,1,2;,
	temp = "\t" + Int2Str(numFaces) + "; // number of triangles\n";
	file.FileWrite( temp.c_str(), temp.length() );
	if ( numFaces>0 )
	{
		for ( i=0; i<numFaces; i++ )
		{
			temp = "\t3;" + Int2Str(surfaces[i*3+0]) + ",";
			temp = temp + Int2Str(surfaces[i*3+1]) + ",";
			temp = temp + Int2Str(surfaces[i*3+2]);
			if ( (i+1)==numFaces )
				temp = temp + ";;\n\n";
			else
				temp = temp + ";,\n";
			file.FileWrite( temp.c_str(), temp.length() );
		}
	}

	//	MeshMaterialList {
	//		1;1;0;;
	//		{material}
	//	}

	if ( numMaterials>0 )
	{
		temp = "\tMeshMaterialList {\n";
		file.FileWrite( temp.c_str(), temp.length() );

		size_t i;
		bool* materialUsed = new bool[numMaterials];
		size_t* materialIndex = new size_t[numMaterials];
		for ( i=0; i<numMaterials; i++ )
		{
			materialUsed[i] = false;
		}
		for ( i=0; i<numMaterialFaces; i++ )
		{
			materialUsed[materialFaces[i]] = true;
		}
		size_t actualMaterialsUsed = 0;
		size_t index = 0;
		for ( i=0; i<numMaterials; i++ )
		{
			materialIndex[i] = index;
			if ( materialUsed[i] )
			{
				actualMaterialsUsed++;
				index++;
			}
		}

		if ( numMaterialFaces==1 || actualMaterialsUsed==1 )
		{
			temp = "\t\t1;1;0;;\n";
			file.FileWrite( temp.c_str(), temp.length() );
		}
		else
		{
			temp = "\t\t" + Int2Str(actualMaterialsUsed) + ";";
			temp = temp + Int2Str(numMaterialFaces) + ";";
			temp = temp + "\n";
			file.FileWrite( temp.c_str(), temp.length() );
			for ( i=0; i<numMaterialFaces; i++ )
			{
				temp = "\t\t" + Int2Str(materialIndex[materialFaces[i]]);
				if ( (i+1)==numMaterialFaces )
					temp = temp + ";;\n";
				else
					temp = temp + ",\n";
				file.FileWrite( temp.c_str(), temp.length() );
			}
		}
		for ( i=0; i<numMaterials; i++ )
		{
			if ( materialUsed[i] )
			{
				TMaterial* mat = Materials( i );
				TString matName = mat->MaterialName();
				temp = "\t\t{" + matName + "}\n";
				file.FileWrite( temp.c_str(), temp.length() );
			}
		}

		delete []materialUsed;
		delete []materialIndex;

		temp = "\t} // End of MeshMaterialList\n\n";
		file.FileWrite( temp.c_str(), temp.length() );
	}

	if ( numNormals>0 )
	{
		//	MeshNormals {
		//		132; // 132 normals
		//		-0.986652;0.004670;-0.162778;,
		temp = "\tMeshNormals {\n";
		file.FileWrite( temp.c_str(), temp.length() );

		// vertices and their faces
		temp = "\t\t" + Int2Str(numNormals) + "; // number of normals\n";
		file.FileWrite( temp.c_str(), temp.length() );

		for ( i=0; i<numNormals; i++ )
		{
			temp = "\t\t" + FloatToString(1,6,normals[i*3+0]) + ";";
			temp = temp + FloatToString(1,6,normals[i*3+1]) + ";";
			temp = temp + FloatToString(1,6,normals[i*3+2]);
			if ( (i+1)==numNormals )
				temp = temp + ";;\n\n";
			else
				temp = temp + ";,\n";
			file.FileWrite( temp.c_str(), temp.length() );
		}
	}

	//	2;		// 2 faces
	//	3;0,1,2;,
	if ( numNormalFaces>0 )
	{
		temp = "\t" + Int2Str(numNormalFaces) + "; // number of normal faces\n";
		file.FileWrite( temp.c_str(), temp.length() );

		for ( i=0; i<numNormalFaces; i++ )
		{
			temp = "\t3;" + Int2Str(normalFaces[i*3+0]) + ",";
			temp = temp + Int2Str(normalFaces[i*3+1]) + ",";
			temp = temp + Int2Str(normalFaces[i*3+2]);
			if ( (i+1)==numNormalFaces )
				temp = temp + ";;\n\n";
			else
				temp = temp + ";,\n";
			file.FileWrite( temp.c_str(), temp.length() );
		}

		temp = "\t} // End of mesh normals\n\n";
		file.FileWrite( temp.c_str(), temp.length() );
	}


	//	MeshTextureCoords {
	//		606; // 606 texture coords
	//		0.000000;0.000000;,
	temp = "\tMeshTextureCoords {\n";
	file.FileWrite( temp.c_str(), temp.length() );

    // texture coordinates
	temp = "\t\t" + Int2Str(numTextureCoords) + "; // number of texture coordinates\n";
	file.FileWrite( temp.c_str(), temp.length() );

	if ( numTextureCoords>0 )
	{
		for ( i=0; i<numTextureCoords; i++ )
		{
			temp = "\t\t" + FloatToString(1,6,textureCoords[i*2+0]) + ";";
			temp = temp + FloatToString(1,6,textureCoords[i*2+1]) + ";";
			if ( (i+1)==numTextureCoords )
				temp = temp + ";\n\n";
			else
				temp = temp + ",\n";
			file.FileWrite( temp.c_str(), temp.length() );
		}
	}

	temp = "\t} // end of texture coordinates\n} // End of mesh\n\n";
	file.FileWrite( temp.c_str(), temp.length() );

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

	// is this a colourmapped item?
	if ( numVertices>0 )
	{
		if ( vertices[(numVertices-1)*3+0]==3.141592f )
		{
			isColourMap = true;
		}
	}

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
			Materials( i, materialIndex, mat );
		}
	}

	// calculate object bounds
	CalculateBoundaries();

	// is this a colourmapped mesh?
	if ( isColourMap )
	{
		colourMapWidth = size_t(vertices[(numVertices-1)*3+1]);
		colourMapHeight = size_t(vertices[(numVertices-1)*3+2]);

		PreCond( numMaterials==2 );

		TTexture bumpMap = Materials(0)->Texture();
		TTexture colrMap = Materials(1)->Texture();

		CreateColourMap( colrMap, bumpMap );
	}

	return true;
};



void TMesh::Print( void ) const
{
	WriteString( "mesh: %s", Name() );
    WriteString( "      vertices        : %d", NumVertices() );
    WriteString( "      surfaces        : %d", NumFaces() );
    WriteString( "      materials       : %d", NumMaterials() );
    WriteString( "      normals         : %d", NumNormals() );
    WriteString( "      texture coords  : %d", NumTextureCoords() );
    WriteString( "      object size     : %ld bytes", ObjectSize() );
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
    	delete []vertices;
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
    	delete []surfaces;
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
    if ( materialParentIndex!=NULL )
    	delete []materialParentIndex;
	if ( val>0 )
	{
		materials = new TMaterial*[val];
		materialParentIndex = new size_t[val];
		for ( size_t i=0; i<val; i++ )
		{
	    	materials[i] = NULL;
			materialParentIndex[i] = 0;
		}
	}
	else
	{
		materials = NULL;
		materialParentIndex = NULL;
	}
};



size_t TMesh::NumMaterialFaces( void ) const
{
	return numMaterialFaces;
};



void TMesh::NumMaterialFaces( size_t val )
{
	numMaterialFaces = val;
    if ( materialFaces!=NULL )
    	delete []materialFaces;
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


size_t TMesh::MaterialParentIndex( size_t index ) const
{
	PreCond( index<numMaterials );
	return materialParentIndex[index];
};


void TMesh::Materials( size_t index, size_t parentIndex, TMaterial* ptr )
{
	PreCond( index<numMaterials );
	materials[index] = ptr;
	materialParentIndex[index] = parentIndex;
};


size_t TMesh::NumNormals( void ) const
{
	return numNormals;
};


void TMesh::NumNormals( size_t val )
{
	numNormals = val;
    if ( normals!=NULL )
    	delete []normals;
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
    	delete []normalFaces;
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
    	delete []textureCoords;
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


void TMesh::ForceDraw( void ) const
{
	_Draw();
}


void TMesh::Draw( void ) const
{
	if ( ownerDraw )
		_Draw();
};


void TMesh::_Draw( void ) const
{
	if ( isColourMap )
	{
		DrawColourmapped();
		return;
	}

	size_t  previousMaterialSurface = -1;
    bool usingTexture = false;
    bool usingNormals = false;

    if ( numNormals>0 )
    	usingNormals = true;
        
	glEnable(GL_COLOR_MATERIAL);
	glColor4f(1,1,1,1);

	// setup lookup table for openGL
//	glVertexPointer(3,GL_FLOAT,0,vertices);
//	glEnableClientState(GL_VERTEX_ARRAY);

	// for each surface in the mesh model
	//glPolygonMode( GL_FRONT, GL_LINE );
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
#ifdef _USEMATERIALPROPERTIES
                    glMaterialfv(GL_FRONT,GL_SHININESS, mat->Shininess() );
                    glMaterialfv(GL_FRONT,GL_SPECULAR, mat->Specularity() );
                    glMaterialfv(GL_FRONT,GL_EMISSION, mat->Emission() );
#endif
                    if ( mat->Texture().Used() )
                    {
						const float* rgba = mat->ColourRGBA();
						if ( rgba[3]>0.0f && rgba[3]<1.0f )
							glEnable( GL_BLEND );
	                    glColor4f( 1,1,1,rgba[3] );
//						glColor4fv( rgba );
						if ( textureCoords!=NULL )
						{
	                        usingTexture = true;
		                	mat->Texture().StartTexture();
						}
						else
	                        usingTexture = false;
                    }
                    else if ( usingTexture )
                    {
						glDisable( GL_BLEND );
                    	mat->Texture().EndTexture();
                    	usingTexture = false;
                    }

                    if ( !usingTexture )
					{
						glDisable( GL_BLEND );
						glColor3fv( mat->ColourRGB() );
					}
                }
				glBegin(GL_TRIANGLES);
            }
        }

		size_t surface = surfaces[j*3+2];
		size_t tsurf = surface;
		if ( usingTexture )
			glTexCoord2fv( &textureCoords[tsurf*2] );
        if ( usingNormals )
        {
        	normal  = normalFaces[j*3+2];
			glNormal3fv( &normals[normal*3] );
        }
		glVertex3fv( &vertices[surface*3] );
		//glArrayElement( surface );

		surface = surfaces[j*3+1];
		tsurf = surface;
		if ( usingTexture )
			glTexCoord2fv( &textureCoords[tsurf*2] );
        if ( usingNormals )
        {
        	normal  = normalFaces[j*3+1];
			glNormal3fv( &normals[normal*3] );
        }
		glVertex3fv( &vertices[surface*3] );
		//glArrayElement( surface );

		surface = surfaces[j*3+0];
		tsurf = surface;
		if ( usingTexture )
			glTexCoord2fv( &textureCoords[tsurf*2] );
        if ( usingNormals )
        {
        	normal  = normalFaces[j*3+0];
			glNormal3fv( &normals[normal*3] );
        }
		glVertex3fv( &vertices[surface*3] );
		//glArrayElement( surface );
	}
	glEnd();

//	glDisableClientState(GL_VERTEX_ARRAY);

    // reset settings
#ifdef _USEMATERIALPROPERTIES
	float zero[] = {0,0,0,1};
    glMateriali(GL_FRONT,GL_SHININESS, 0 );
    glMaterialfv(GL_FRONT,GL_SPECULAR, zero );
    glMaterialfv(GL_FRONT,GL_EMISSION, zero );
#endif
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_COLOR_MATERIAL);
    glColor3f(1,1,1);
}


void TMesh::DrawExplosion( float scale ) const
{
	size_t  previousMaterialSurface = -1;
    bool usingTexture = false;
    bool usingNormals = false;

    if ( numNormals>0 )
    	usingNormals = true;
        
	glEnable(GL_COLOR_MATERIAL);
	glColor3f(1,1,1);

	// setup lookup table for openGL
	glVertexPointer(3,GL_FLOAT,0,vertices);
	glEnableClientState(GL_VERTEX_ARRAY);

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
                    if ( mat->Texture().Used() )
                    {
						const float* rgba = mat->ColourRGBA();
						if ( scale < 0.7f )
						{
							glEnable( GL_BLEND );
							glColor4f( 0,0,0, scale );
						}
						else
						{
							glDisable( GL_BLEND );
							glColor3f( 0,0,0 );
						}

						if ( textureCoords!=NULL )
						{
	                        usingTexture = true;
		                	mat->Texture().StartTexture();
						}
						else
	                        usingTexture = false;
                    }
                    else if ( usingTexture )
                    {
						glDisable( GL_BLEND );
                    	mat->Texture().EndTexture();
                    	usingTexture = false;
                    }
                }
				glBegin(GL_TRIANGLES);
            }
        }

		size_t surface = surfaces[j*3+2];
		size_t tsurf = surface;
		if ( usingTexture )
			glTexCoord2fv( &textureCoords[tsurf*2] );
        if ( usingNormals )
        {
        	normal  = normalFaces[j*3+2];
			glNormal3fv( &normals[normal*3] );
        }
		//glVertex3fv( &vertices[surface*3] );
		glArrayElement( surface );

		surface = surfaces[j*3+1];
		tsurf = surface;
		if ( usingTexture )
			glTexCoord2fv( &textureCoords[tsurf*2] );
        if ( usingNormals )
        {
        	normal  = normalFaces[j*3+1];
			glNormal3fv( &normals[normal*3] );
        }
		//glVertex3fv( &vertices[surface*3] );
		glArrayElement( surface );

		surface = surfaces[j*3+0];
		tsurf = surface;
		if ( usingTexture )
			glTexCoord2fv( &textureCoords[tsurf*2] );
        if ( usingNormals )
        {
        	normal  = normalFaces[j*3+0];
			glNormal3fv( &normals[normal*3] );
        }
		//glVertex3fv( &vertices[surface*3] );
		glArrayElement( surface );
	}
	glEnd();

	// setup lookup table for openGL
	glDisableClientState(GL_VERTEX_ARRAY);

    // reset settings
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_COLOR_MATERIAL);
    glColor3f(1,1,1);
}


void TMesh::Draw( const float* matrix ) const
{
	if ( isColourMap )
	{
		DrawColourmapped();
		return;
	}

	size_t  previousMaterialSurface = -1;
    bool usingTexture = false;
    bool usingNormals = false;

    if ( numNormals>0 )
    	usingNormals = true;
        
	glEnable(GL_COLOR_MATERIAL);
	glColor3f(1,1,1);

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
#ifdef _USEMATERIALPROPERTIES
                    glMaterialfv(GL_FRONT,GL_SHININESS, mat->Shininess() );
                    glMaterialfv(GL_FRONT,GL_SPECULAR, mat->Specularity() );
                    glMaterialfv(GL_FRONT,GL_EMISSION, mat->Emission() );
#endif
                    if ( mat->Texture().Used() )
                    {
						const float* rgba = mat->ColourRGBA();
						if ( rgba[3]>0.0f && rgba[3]<1.0f )
							glEnable( GL_BLEND );
	                    glColor4f( 1,1,1,rgba[3] );
//						glColor4fv( rgba );
						if ( textureCoords!=NULL )
						{
	                        usingTexture = true;
		                	mat->Texture().StartTexture();
						}
						else
	                        usingTexture = false;
                    }
                    else if ( usingTexture )
                    {
						glDisable( GL_BLEND );
                    	mat->Texture().EndTexture();
                    	usingTexture = false;
                    }

                    if ( !usingTexture )
					{
						glDisable( GL_BLEND );
						glColor3fv( mat->ColourRGB() );
					}
                }
				glBegin(GL_TRIANGLES);
            }
        }

		size_t surface = surfaces[j*3+2];
		size_t tsurf = surface;
		if ( usingTexture )
			glTexCoord2fv( &textureCoords[tsurf*2] );
        if ( usingNormals )
        {
        	normal  = normalFaces[j*3+2];
			glNormal3fv( &normals[normal*3] );
        }
		float v[3];
		::MatrixMult( matrix, &vertices[surface*3], v ); 
		glVertex3fv( v );

		surface = surfaces[j*3+1];
		tsurf = surface;
		if ( usingTexture )
			glTexCoord2fv( &textureCoords[tsurf*2] );
        if ( usingNormals )
        {
        	normal  = normalFaces[j*3+1];
			glNormal3fv( &normals[normal*3] );
        }
		::MatrixMult( matrix, &vertices[surface*3], v ); 
		glVertex3fv( v );

		surface = surfaces[j*3+0];
		tsurf = surface;
		if ( usingTexture )
			glTexCoord2fv( &textureCoords[tsurf*2] );
        if ( usingNormals )
        {
        	normal  = normalFaces[j*3+0];
			glNormal3fv( &normals[normal*3] );
        }
		::MatrixMult( matrix, &vertices[surface*3], v ); 
		glVertex3fv( v );
	}
	glEnd();

    // reset settings
#ifdef _USEMATERIALPROPERTIES
	float zero[] = {0,0,0,1};
    glMateriali(GL_FRONT,GL_SHININESS, 0 );
    glMaterialfv(GL_FRONT,GL_SPECULAR, zero );
    glMaterialfv(GL_FRONT,GL_EMISSION, zero );
#endif
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_COLOR_MATERIAL);
    glColor3f(1,1,1);
}


void TMesh::Draw( float scale, const float* matrix ) const
{
	if ( isColourMap )
	{
		DrawColourmapped();
		return;
	}

	size_t  previousMaterialSurface = -1;
    bool usingTexture = false;
    bool usingNormals = false;

    if ( numNormals>0 )
    	usingNormals = true;
        
	glEnable(GL_COLOR_MATERIAL);
	glColor3f(1,1,1);

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
#ifdef _USEMATERIALPROPERTIES
                    glMaterialfv(GL_FRONT,GL_SHININESS, mat->Shininess() );
                    glMaterialfv(GL_FRONT,GL_SPECULAR, mat->Specularity() );
                    glMaterialfv(GL_FRONT,GL_EMISSION, mat->Emission() );
#endif
                    if ( mat->Texture().Used() )
                    {
						const float* rgba = mat->ColourRGBA();
						if ( rgba[3]>0.0f && rgba[3]<1.0f )
							glEnable( GL_BLEND );
	                    glColor4f( 1,1,1,rgba[3] );
//						glColor4fv( rgba );
						if ( textureCoords!=NULL )
						{
	                        usingTexture = true;
		                	mat->Texture().StartTexture();
						}
						else
	                        usingTexture = false;
                    }
                    else if ( usingTexture )
                    {
						glDisable( GL_BLEND );
                    	mat->Texture().EndTexture();
                    	usingTexture = false;
                    }

                    if ( !usingTexture )
					{
						glDisable( GL_BLEND );
						glColor3fv( mat->ColourRGB() );
					}
                }
				glBegin(GL_TRIANGLES);
            }
        }

		size_t surface = surfaces[j*3+2];
		size_t tsurf = surface;
		if ( usingTexture )
			glTexCoord2fv( &textureCoords[tsurf*2] );
        if ( usingNormals )
        {
        	normal  = normalFaces[j*3+2];
			glNormal3fv( &normals[normal*3] );
        }
		float v[3];
		::MatrixMult( matrix, &vertices[surface*3], scale, v ); 
		glVertex3fv( v );

		surface = surfaces[j*3+1];
		tsurf = surface;
		if ( usingTexture )
			glTexCoord2fv( &textureCoords[tsurf*2] );
        if ( usingNormals )
        {
        	normal  = normalFaces[j*3+1];
			glNormal3fv( &normals[normal*3] );
        }
		::MatrixMult( matrix, &vertices[surface*3], scale, v ); 
		glVertex3fv( v );

		surface = surfaces[j*3+0];
		tsurf = surface;
		if ( usingTexture )
			glTexCoord2fv( &textureCoords[tsurf*2] );
        if ( usingNormals )
        {
        	normal  = normalFaces[j*3+0];
			glNormal3fv( &normals[normal*3] );
        }
		::MatrixMult( matrix, &vertices[surface*3], scale, v ); 
		glVertex3fv( v );
	}
	glEnd();

    // reset settings
#ifdef _USEMATERIALPROPERTIES
	float zero[] = {0,0,0,1};
    glMateriali(GL_FRONT,GL_SHININESS, 0 );
    glMaterialfv(GL_FRONT,GL_SPECULAR, zero );
    glMaterialfv(GL_FRONT,GL_EMISSION, zero );
#endif
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_COLOR_MATERIAL);
    glColor3f(1,1,1);
}


void TMesh::Draw( float scale ) const
{
	if ( isColourMap )
	{
		DrawColourmapped();
		return;
	}

	size_t  previousMaterialSurface = -1;
    bool usingTexture = false;
    bool usingNormals = false;

    if ( numNormals>0 )
    	usingNormals = true;
        
	glEnable(GL_COLOR_MATERIAL);
	glColor3f(1,1,1);

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
#ifdef _USEMATERIALPROPERTIES
                glMaterialfv(GL_FRONT,GL_SHININESS, mat->Shininess() );
                glMaterialfv(GL_FRONT,GL_SPECULAR, mat->Specularity() );
                glMaterialfv(GL_FRONT,GL_EMISSION, mat->Emission() );
#endif
                if ( mat!=NULL )
                {
                    if ( mat->Texture().Used() )
                    {
						const float* rgba = mat->ColourRGBA();
						if ( rgba[3]>0.0f && rgba[3]<1.0f )
							glEnable( GL_BLEND );
	                    glColor4f( 1,1,1,rgba[3] );
//						glColor4fv( rgba );
						if ( textureCoords!=NULL )
						{
	                        usingTexture = true;
		                	mat->Texture().StartTexture();
						}
						else
	                        usingTexture = false;
                    }
                    else if ( usingTexture )
                    {
						glDisable( GL_BLEND );
                    	mat->Texture().EndTexture();
                    	usingTexture = false;
                    }

                    if ( !usingTexture )
					{
						glDisable( GL_BLEND );
						glColor3fv( mat->ColourRGB() );
					}
                }
				glBegin(GL_TRIANGLES);
            }
        }

		size_t surface = surfaces[j*3+2];
		size_t tsurf = surface;
		if ( usingTexture )
			glTexCoord2fv( &textureCoords[tsurf*2] );
        if ( usingNormals )
        {
        	normal  = normalFaces[j*3+2];
			glNormal3fv( &normals[normal*3] );
        }
		glVertex3f( vertices[surface*3+0]*scale, vertices[surface*3+1]*scale, vertices[surface*3+2]*scale );

		surface = surfaces[j*3+1];
		tsurf = surface;
		if ( usingTexture )
			glTexCoord2fv( &textureCoords[tsurf*2] );
        if ( usingNormals )
        {
        	normal  = normalFaces[j*3+1];
			glNormal3fv( &normals[normal*3] );
        }
		glVertex3f( vertices[surface*3+0]*scale, vertices[surface*3+1]*scale, vertices[surface*3+2]*scale );

		surface = surfaces[j*3+0];
		tsurf = surface;
		if ( usingTexture )
			glTexCoord2fv( &textureCoords[tsurf*2] );
        if ( usingNormals )
        {
        	normal  = normalFaces[j*3+0];
			glNormal3fv( &normals[normal*3] );
        }
		glVertex3f( vertices[surface*3+0]*scale, vertices[surface*3+1]*scale, vertices[surface*3+2]*scale );
	}
	glEnd();

    // reset settings
#ifdef _USEMATERIALPROPERTIES
	float zero[] = {0,0,0,1};
    glMateriali(GL_FRONT,GL_SHININESS, 0 );
    glMaterialfv(GL_FRONT,GL_SPECULAR, zero );
    glMaterialfv(GL_FRONT,GL_EMISSION, zero );
#endif
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_COLOR_MATERIAL);
    glColor3f(1,1,1);
}


void TMesh::DrawNoMaterials( float scale ) const
{
    bool usingNormals = numNormals>0;
        
	// for each surface in the mesh model
	glBegin(GL_TRIANGLES);
	for ( size_t j=0; j<numFaces; j++ )
	{
    	size_t normal;
        
		size_t surface = surfaces[j*3+2];
		size_t tsurf = surface;
        if ( usingNormals )
        {
        	normal  = normalFaces[j*3+2];
			glNormal3fv( &normals[normal*3] );
        }
		glVertex3f( vertices[surface*3+0]*scale, vertices[surface*3+1]*scale, vertices[surface*3+2]*scale );

		surface = surfaces[j*3+1];
		tsurf = surface;
        if ( usingNormals )
        {
        	normal  = normalFaces[j*3+1];
			glNormal3fv( &normals[normal*3] );
        }
		glVertex3f( vertices[surface*3+0]*scale, vertices[surface*3+1]*scale, vertices[surface*3+2]*scale );

		surface = surfaces[j*3+0];
		tsurf = surface;
        if ( usingNormals )
        {
        	normal  = normalFaces[j*3+0];
			glNormal3fv( &normals[normal*3] );
        }
		glVertex3f( vertices[surface*3+0]*scale, vertices[surface*3+1]*scale, vertices[surface*3+2]*scale );
	}
	glEnd();
}


void TMesh::Draw( bool drawTransparencies ) const
{
	if ( ownerDraw )
		_Draw(drawTransparencies);
};


void TMesh::_Draw( bool drawTransparencies ) const
{
	if ( isColourMap )
	{
		DrawColourmapped();
		return;
	}

	size_t  previousMaterialSurface = -1;
    bool usingTexture = false;
    bool usingNormals = false;

    if ( numNormals>0 )
    	usingNormals = true;
        
	glEnable(GL_COLOR_MATERIAL);
	glColor3f(1,1,1);

	// setup lookup table for openGL
	glVertexPointer(3,GL_FLOAT,0,vertices);
	glEnableClientState(GL_VERTEX_ARRAY);

	// for each surface in the mesh model
	glBegin(GL_TRIANGLES);
	bool doDraw = true;
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
				doDraw = true;
                if ( mat!=NULL )
                {
                    if ( mat->Texture().Used() )
                    {
						const float* rgba = mat->ColourRGBA();
						if ( rgba[3]>0.0f && rgba[3]<1.0f )
						{
							if ( !drawTransparencies )
								doDraw = false;

							glEnable( GL_BLEND );
						}
						else if ( drawTransparencies )
						{
							doDraw = false;
						}

						if ( mat->Texture().IsRGBA() )
						{
							doDraw = drawTransparencies;
						}

						glColor4f( 1,1,1,rgba[3] );
//						glColor4fv( rgba );
						if ( textureCoords!=NULL )
						{
	                        usingTexture = true;
		                	mat->Texture().StartTexture();
						}
						else
	                        usingTexture = false;
                    }
                    else if ( usingTexture )
                    {
						glDisable( GL_BLEND );
                    	mat->Texture().EndTexture();
                    	usingTexture = false;
                    }

                    if ( !usingTexture )
					{
						glDisable( GL_BLEND );
						const float* rgba = mat->ColourRGBA();
						if ( rgba[3]>0.0f && rgba[3]<1.0f )
						{
							if ( !drawTransparencies )
								doDraw = false;

							glEnable( GL_BLEND );
						}
						else if ( drawTransparencies )
						{
							doDraw = false;
						}
						glColor4fv( rgba );
					}
                }
				glBegin(GL_TRIANGLES);
            }
        }

		if ( doDraw )
		{
			size_t surface = surfaces[j*3+2];
			size_t tsurf = surface;
			if ( usingTexture )
				glTexCoord2fv( &textureCoords[tsurf*2+0] );
			if ( usingNormals )
		    {
		     	normal  = normalFaces[j*3+2];
				glNormal3fv( &normals[normal*3] );
		    }
			//glVertex3fv( &vertices[surface*3] );
			glArrayElement( surface );
	
			surface = surfaces[j*3+1];
			tsurf = surface;
			if ( usingTexture )
				glTexCoord2fv( &textureCoords[tsurf*2+0] );
	        if ( usingNormals )
	        {
	        	normal  = normalFaces[j*3+1];
				glNormal3fv( &normals[normal*3] );
	        }
			//glVertex3fv( &vertices[surface*3] );
			glArrayElement( surface );

			surface = surfaces[j*3+0];
			tsurf = surface;
			if ( usingTexture )
				glTexCoord2fv( &textureCoords[tsurf*2+0] );
	        if ( usingNormals )
	        {
	        	normal  = normalFaces[j*3+0];
				glNormal3fv( &normals[normal*3] );
	        }
			//glVertex3fv( &vertices[surface*3] );
			glArrayElement( surface );
		}
	}
	glEnd();

	glDisableClientState(GL_VERTEX_ARRAY);

    // reset settings
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_COLOR_MATERIAL);
    glColor3f(1,1,1);
}


void TMesh::DrawMonoColoured( float r, float g, float b ) const
{
	size_t  previousMaterialSurface = -1;
    bool usingTexture = false;
    bool usingNormals = false;

    if ( numNormals>0 )
    	usingNormals = true;
        
	glEnable(GL_COLOR_MATERIAL);
	glColor3f(1,1,1);

	// setup lookup table for openGL
	glVertexPointer(3,GL_FLOAT,0,vertices);
	glEnableClientState(GL_VERTEX_ARRAY);

	// for each surface in the mesh model
	glBegin(GL_TRIANGLES);
	const float* rgba = NULL;
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
                    if ( mat->Texture().Used() )
                    {
						rgba = mat->ColourRGBA();
						if ( rgba[3]>0.0f && rgba[3]<1.0f )
						{
							glEnable( GL_BLEND );
						}

						if ( textureCoords!=NULL )
						{
	                        usingTexture = true;
		                	mat->Texture().StartTexture();
						}
						else
	                        usingTexture = false;
                    }
                    else if ( usingTexture )
                    {
						glDisable( GL_BLEND );
                    	mat->Texture().EndTexture();
                    	usingTexture = false;
                    }

                    if ( !usingTexture )
					{
						glDisable( GL_BLEND );
						const float* rgba = mat->ColourRGBA();
						if ( rgba[3]>0.0f && rgba[3]<1.0f )
						{
							glEnable( GL_BLEND );
						}
						glColor4fv( rgba );
					}
                }
				glBegin(GL_TRIANGLES);
            }
        }

		{
			glColor3f( r,g,b );

			size_t surface = surfaces[j*3+2];
			size_t tsurf = surface;
			if ( usingTexture )
				glTexCoord2fv( &textureCoords[tsurf*2+0] );
			if ( usingNormals )
		    {
		     	normal  = normalFaces[j*3+2];
				glNormal3fv( &normals[normal*3] );
		    }
			//glVertex3fv( &vertices[surface*3] );
			glArrayElement( surface );
	
			surface = surfaces[j*3+1];
			tsurf = surface;
			if ( usingTexture )
				glTexCoord2fv( &textureCoords[tsurf*2+0] );
	        if ( usingNormals )
	        {
	        	normal  = normalFaces[j*3+1];
				glNormal3fv( &normals[normal*3] );
	        }
			//glVertex3fv( &vertices[surface*3] );
			glArrayElement( surface );

			surface = surfaces[j*3+0];
			tsurf = surface;
			if ( usingTexture )
				glTexCoord2fv( &textureCoords[tsurf*2+0] );
	        if ( usingNormals )
	        {
	        	normal  = normalFaces[j*3+0];
				glNormal3fv( &normals[normal*3] );
	        }
			//glVertex3fv( &vertices[surface*3] );
			glArrayElement( surface );
		}
	}
	glEnd();

	// setup lookup table for openGL
	glDisableClientState(GL_VERTEX_ARRAY);

    // reset settings
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_COLOR_MATERIAL);
    glColor3f(1,1,1);
}



void TMesh::Draw( size_t numFaces, size_t* surfaces, size_t* mats )
{
	if ( isColourMap )
	{
		DrawColourmapped( numFaces, surfaces );
		return;
	}

	size_t  previousMaterialSurface = -1;
    bool usingTexture = false;
    bool usingNormals = false;

    if ( numNormals>0 )
    	usingNormals = true;
        
	glEnable(GL_COLOR_MATERIAL);

	// setup lookup table for openGL
	glVertexPointer(3,GL_FLOAT,0,vertices);
	glEnableClientState(GL_VERTEX_ARRAY);

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
#ifdef _USEMATERIALPROPERTIES
                    glMaterialfv(GL_FRONT,GL_SHININESS, mat->Shininess() );
                    glMaterialfv(GL_FRONT,GL_SPECULAR, mat->Specularity() );
                    glMaterialfv(GL_FRONT,GL_EMISSION, mat->Emission() );
#endif
                    if ( mat->Texture().Used() )
                    {
						const float* rgba = mat->ColourRGBA();
						if ( rgba[3]>0.0f && rgba[3]<1.0f )
						{
							glEnable( GL_BLEND );
						}

						glColor4f( 1,1,1,rgba[3] );
//						glColor4fv( rgba );
						if ( textureCoords!=NULL )
						{
	                        usingTexture = true;
		                	mat->Texture().StartTexture();
						}
						else
	                        usingTexture = false;
                    }
                    else if ( usingTexture )
                    {
						glDisable( GL_BLEND );
                    	mat->Texture().EndTexture();
                    	usingTexture = false;
                    }

                    if ( !usingTexture )
					{
						glDisable( GL_BLEND );
						glColor3fv( mat->ColourRGB() );
					}
                }
				glBegin(GL_TRIANGLES);
            }
        }

		size_t surface = surfaces[ j*3+2 ];
		if ( usingTexture )
			glTexCoord2fv( &textureCoords[surface*2] );
        if ( usingNormals )
        {
        	normal  = normalFaces[ j*3+2 ];
			glNormal3fv( &normals[normal*3] );
        }
		//glVertex3fv( &vertices[surface*3] );
		glArrayElement( surface );

		surface = surfaces[ j*3+1 ];
		if ( usingTexture )
			glTexCoord2fv( &textureCoords[surface*2] );
        if ( usingNormals )
        {
        	normal  = normalFaces[ j*3+1 ];
			glNormal3fv( &normals[normal*3] );
        }
		//glVertex3fv( &vertices[surface*3] );
		glArrayElement( surface );

		surface = surfaces[ j*3+0 ];
		if ( usingTexture )
			glTexCoord2fv( &textureCoords[surface*2] );
        if ( usingNormals )
        {
        	normal  = normalFaces[ j*3+0 ];
			glNormal3fv( &normals[normal*3] );
        }
		//glVertex3fv( &vertices[surface*3] );
		glArrayElement( surface );
	}
	glEnd();

	// setup lookup table for openGL
	glDisableClientState(GL_VERTEX_ARRAY);

    // reset settings
#ifdef _USEMATERIALPROPERTIES
	float zero[] = {0,0,0,1};
    glMateriali(GL_FRONT,GL_SHININESS, 0 );
    glMaterialfv(GL_FRONT,GL_SPECULAR, zero );
    glMaterialfv(GL_FRONT,GL_EMISSION, zero );
#endif
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_COLOR_MATERIAL);
    glColor3f(1,1,1);
}


void TMesh::Draw( size_t numFaces, size_t* surfaces, size_t* mats, bool drawTransparencies )
{
	if ( isColourMap )
	{
		DrawColourmapped( numFaces, surfaces );
		return;
	}

	size_t  previousMaterialSurface = -1;
    bool usingTexture = false;
    bool usingNormals = false;

    if ( numNormals>0 )
    	usingNormals = true;
        
	glEnable(GL_COLOR_MATERIAL);

	// setup lookup table for openGL
	glVertexPointer(3,GL_FLOAT,0,vertices);
	glEnableClientState(GL_VERTEX_ARRAY);

	// for each surface in the mesh model
	glBegin(GL_TRIANGLES);
	bool doDraw = true;
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
					doDraw = true;
#ifdef _USEMATERIALPROPERTIES
                    glMaterialfv(GL_FRONT,GL_SHININESS, mat->Shininess() );
                    glMaterialfv(GL_FRONT,GL_SPECULAR, mat->Specularity() );
                    glMaterialfv(GL_FRONT,GL_EMISSION, mat->Emission() );
#endif
                    if ( mat->Texture().Used() )
                    {
						const float* rgba = mat->ColourRGBA();
						if ( rgba[3]>0.0f && rgba[3]<1.0f )
						{
							if ( !drawTransparencies )
								doDraw = false;

							glEnable( GL_BLEND );
						}
						else if ( drawTransparencies )
						{
							doDraw = false;
						}

						if ( mat->Texture().IsRGBA() )
						{
							doDraw = drawTransparencies;
						}

						glColor4f( 1,1,1,rgba[3] );
//						glColor4fv( rgba );
						if ( textureCoords!=NULL )
						{
	                        usingTexture = true;
		                	mat->Texture().StartTexture();
						}
						else
	                        usingTexture = false;
                    }
                    else if ( usingTexture )
                    {
						glDisable( GL_BLEND );
                    	mat->Texture().EndTexture();
                    	usingTexture = false;
                    }

                    if ( !usingTexture )
					{
						glDisable( GL_BLEND );
						glColor3fv( mat->ColourRGB() );
					}
                }
				glBegin(GL_TRIANGLES);
            }
        }

		if ( doDraw )
		{
			size_t surface = surfaces[ j*3+2 ];
			if ( usingTexture )
				glTexCoord2fv( &textureCoords[surface*2] );
	        if ( usingNormals )
	        {
	        	normal  = normalFaces[ j*3+2 ];
				glNormal3fv( &normals[normal*3] );
	        }
			//glVertex3fv( &vertices[surface*3] );
			glArrayElement( surface );

			surface = surfaces[ j*3+1 ];
			if ( usingTexture )
				glTexCoord2fv( &textureCoords[surface*2] );
	        if ( usingNormals )
	        {
	        	normal  = normalFaces[ j*3+1 ];
				glNormal3fv( &normals[normal*3] );
	        }
			//glVertex3fv( &vertices[surface*3] );
			glArrayElement( surface );

			surface = surfaces[ j*3+0 ];
			if ( usingTexture )
				glTexCoord2fv( &textureCoords[surface*2] );
	        if ( usingNormals )
	        {
	        	normal  = normalFaces[ j*3+0 ];
				glNormal3fv( &normals[normal*3] );
	        }
			//glVertex3fv( &vertices[surface*3] );
			glArrayElement( surface );
		}
	}
	glEnd();

	// setup lookup table for openGL
	glDisableClientState(GL_VERTEX_ARRAY);

    // reset settings
#ifdef _USEMATERIALPROPERTIES
	float zero[] = {0,0,0,1};
    glMateriali(GL_FRONT,GL_SHININESS, 0 );
    glMaterialfv(GL_FRONT,GL_SPECULAR, zero );
    glMaterialfv(GL_FRONT,GL_EMISSION, zero );
#endif
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_COLOR_MATERIAL);
    glColor3f(1,1,1);
}


void TMesh::MatrixMult( const float* matrix )
{
	for ( size_t i=0; i<numVertices; i++ )
	{
		float v[3];
		::MatrixMult( matrix, &vertices[i*3], v );
		vertices[i*3+0] = v[0];
		vertices[i*3+1] = v[1];
		vertices[i*3+2] = v[2];
	}
};


void TMesh::SetColourMapSize( size_t _colourMapWidth, size_t _colourMapHeight )
{
	isColourMap = true;
	colourMapWidth = _colourMapWidth;
	colourMapHeight = _colourMapHeight;
};


void TMesh::CreateColourMap( const TTexture& cMap, 
							 const TTexture& _bumpMap )
{
	isColourMap = true;
	CalculateBoundaries();
	bumpMap = _bumpMap;

	// sun's direction vector
//	float lx = 0;
//	float ly = 0.70710678f;
//	float lz = 0.70710678f;
	float lx = 0;
	float ly = 1;
	float lz = 0;

	size_t w = cMap.Width();
	size_t h = cMap.Height();
	PreCond( w>0 && h>0 );

	const byte* rgb = cMap.Rgb();
	PostCond( rgb!=NULL );

	if ( colourMap!=NULL )
		delete []colourMap;
	colourMap = new byte[colourMapWidth*colourMapHeight*3];
	PostCond( colourMap!=NULL );
	ZeroMemory( colourMap, colourMapWidth*colourMapHeight*3 );
	
	float sizeX = 1 / (maxX - minX);
	float sizeY = 1 / (maxY - minY);
	float sizeZ = 1 / (maxZ - minZ);

	for ( size_t j=0; j<(numVertices-1); j++ )
	{
		float height = ((vertices[j*3+1]-minY) * sizeY);
		height = height * (h-1);
		size_t index = size_t(h-1) - size_t(height); 

		size_t off = size_t(Random()*float(w));
		if ( off>0 )
			off--;
		index = index*w*3 + off * 3;
		byte r,g,b;
		r = rgb[index+0];
		g = rgb[index+1];
		b = rgb[index+2];
		
		float x = ((vertices[j*3+0]-minX) * sizeX) * colourMapWidth;
		float y = ((vertices[j*3+2]-minZ) * sizeZ) * colourMapHeight;
		size_t x1 = size_t(x);
		size_t y1 = size_t(y);

		if ( x1>=colourMapWidth )
			x1 = colourMapWidth-1;
		if ( y1>=colourMapHeight )
			y1 = colourMapHeight-1;

		// calculate normal
		float scale = 1;
		/*
		if ( (x1+1)<w && (y1+1)<h )
		{
			float xv1 = vertices[j*3+0];
			float yv1 = vertices[j*3+1];
			float zv1 = vertices[j*3+2];

			float xv2 = vertices[y1*colourMapWidth*3+(x1+1)*3+0];
			float yv2 = vertices[y1*colourMapWidth*3+(x1+1)*3+1];
			float zv2 = vertices[y1*colourMapWidth*3+(x1+1)*3+2];

			float xv3 = vertices[(y1+1)*colourMapWidth*3+x1*3+0];
			float yv3 = vertices[(y1+1)*colourMapWidth*3+x1*3+1];
			float zv3 = vertices[(y1+1)*colourMapWidth*3+x1*3+2];

			float nx,ny,nz;
			Normal( nx,ny,nz,  xv1,yv1,zv1,  xv2,yv2,zv2,  xv3,yv3,zv3 );
			float size = 1 / float(sqrt(nx*nx+ny*ny+nz*nz));
			nx = nx * size;
			ny = ny * size;
			nz = nz * size;

			float scale = nx*lx + ny*ly + nz*lz;
			scale = scale * 1.2f;
			if ( Random()<0.25f )
				scale = scale * 0.5f;
			if ( scale<0 )
				scale = 0;
			if ( scale>1 )
				scale = 1;
		}
		*/
		colourMap[y1*colourMapWidth*3+x1*3+0] = byte(float(r)*scale);
		colourMap[y1*colourMapWidth*3+x1*3+1] = byte(float(g)*scale);
		colourMap[y1*colourMapWidth*3+x1*3+2] = byte(float(b)*scale);
	}
};


void TMesh::ColourMapSetWaterLevel( size_t _waterLevel, size_t w, size_t h )
{
	// height according to auto-mesh generation
	float by1 = 0;
	float by2 = 75;
	if ( w==64 || h==64 )
		by2 = by2 * 0.5f;
	if ( w==256 || h==256 )
		by2 = by2 * 2;

	float waterLevel = float(_waterLevel) * 0.01f * (by2-by1) * 0.5f;
	
	if ( !isColourMap )
		return;
	if ( colourMap==NULL )
		return;

	float sizeX = 1 / (maxX - minX);
	float sizeY = 1 / (maxY - minY);
	float sizeZ = 1 / (maxZ - minZ);

	for ( size_t j=0; j<(numVertices-1); j++ )
	{
		float height = vertices[j*3+1];

		float x = ((vertices[j*3+0]-minX) * sizeX) * colourMapWidth;
		float y = ((vertices[j*3+2]-minZ) * sizeZ) * colourMapHeight;
		size_t x1 = size_t(x);
		size_t y1 = size_t(y);

		if ( x1>=colourMapWidth )
			x1 = colourMapWidth-1;
		if ( y1>=colourMapHeight )
			y1 = colourMapHeight-1;

		if ( height < waterLevel )
		{
			byte r = colourMap[y1*colourMapWidth*3+x1*3+0];
			byte g = colourMap[y1*colourMapWidth*3+x1*3+1];
			byte b = colourMap[y1*colourMapWidth*3+x1*3+2];

			colourMap[y1*colourMapWidth*3+x1*3+0] = byte(float(r)*0.75f);
			colourMap[y1*colourMapWidth*3+x1*3+1] = byte(float(g)*0.75f);
			colourMap[y1*colourMapWidth*3+x1*3+2] = byte(float(b));
		}
	}
};


void TMesh::GetSeaSurfaceColour( float& r, float& g, float& b )
{
	r = (float(colourMap[0]) / 255) * 0.75f;
	g = (float(colourMap[1]) / 255) * 0.75f;
	b = (float(colourMap[2]) / 255);
};


void TMesh::DrawColourmapped( void ) const
{
	PreCond( colourMap!=NULL );

	glNormal3f(0,0.70710678f,0.70710678f);
	glEnable(GL_COLOR_MATERIAL);

	size_t w = colourMapWidth;
	size_t h = colourMapHeight;
	PreCond( w>0 && h>0 );
	size_t w1 = w-1;
	size_t h1 = h-1;

	const byte* rgb = colourMap;

	if ( bumpMap.Used() )
		bumpMap.StartTexture();

	// setup lookup table for openGL
	glVertexPointer(3,GL_FLOAT,0,vertices);
	glEnableClientState(GL_VERTEX_ARRAY);

	glBegin(GL_TRIANGLES);

	float scale = 0.056f;

	float sizeX = 1 / (maxX - minX);
	float sizeZ = 1 / (maxZ - minZ);

	// for each surface in the mesh model
	for ( size_t j=0; j<numFaces; j++ )
	{
		float x,y;
		size_t x1,y1;

		size_t surface = surfaces[j*3+2];
		
		x = ((vertices[surface*3+0]-minX) * sizeX) * w1;
		y = ((vertices[surface*3+2]-minZ) * sizeZ) * h1;
		x1 = size_t(x)*3;
		y1 = size_t(y)*3;
		glColor3ubv( &rgb[y1*w+x1] );
		if ( bumpMap.Used() )
			glTexCoord2f( (vertices[surface*3+0]-minX)*scale, (vertices[surface*3+2]-minZ)*scale );
		//glVertex3fv( &vertices[surface*3] );
		glArrayElement( surface );

		surface = surfaces[j*3+1];
		x = ((vertices[surface*3+0]-minX) * sizeX) * w1;
		y = ((vertices[surface*3+2]-minZ) * sizeZ) * h1;
		x1 = size_t(x)*3;
		y1 = size_t(y)*3;
		glColor3ubv( &rgb[y1*w+x1] );
		if ( bumpMap.Used() )
			glTexCoord2f( (vertices[surface*3+0]-minX)*scale, (vertices[surface*3+2]-minZ)*scale );
		//glVertex3fv( &vertices[surface*3] );
		glArrayElement( surface );

		surface = surfaces[j*3+0];
		x = ((vertices[surface*3+0]-minX) * sizeX) * w1;
		y = ((vertices[surface*3+2]-minZ) * sizeZ) * h1;
		x1 = size_t(x)*3;
		y1 = size_t(y)*3;
		glColor3ubv( &rgb[y1*w+x1] );
		if ( bumpMap.Used() )
			glTexCoord2f( (vertices[surface*3+0]-minX)*scale, (vertices[surface*3+2]-minZ)*scale );
		//glVertex3fv( &vertices[surface*3] );
		glArrayElement( surface );
	}

	glEnd();

	glDisableClientState(GL_VERTEX_ARRAY);

	if ( bumpMap.Used() )
		bumpMap.EndTexture();

    // reset settings
    glColor3f(1,1,1);
}


void TMesh::DrawColourmapped( size_t numFaces, size_t* surfaces ) const
{
	PreCond( colourMap!=NULL );

	glNormal3f(0,0.70710678f,0.70710678f);
	glEnable(GL_COLOR_MATERIAL);

	size_t w = colourMapWidth;
	size_t h = colourMapHeight;
	PreCond( w>0 && h>0 );
	size_t w1 = w-1;
	size_t h1 = h-1;

	const byte* rgb = colourMap;

	if ( bumpMap.Used() )
		bumpMap.StartTexture();

	// setup lookup table for openGL
	glVertexPointer(3,GL_FLOAT,0,vertices);
	glEnableClientState(GL_VERTEX_ARRAY);

	glBegin(GL_TRIANGLES);

	float scale = 0.056f;

	float sizeX = 1 / (maxX - minX);
	float sizeZ = 1 / (maxZ - minZ);

	// for each surface in the mesh model
	for ( size_t j=0; j<numFaces; j++ )
	{
		float x,y;
		size_t x1,y1;

		size_t surface = surfaces[j*3+2];
		
		x = ((vertices[surface*3+0]-minX) * sizeX) * w1;
		y = ((vertices[surface*3+2]-minZ) * sizeZ) * h1;
		x1 = size_t(x)*3;
		y1 = size_t(y)*3;
		glColor3ubv( &rgb[y1*w+x1] );
		if ( bumpMap.Used() )
			glTexCoord2f( (vertices[surface*3+0]-minX)*scale, (vertices[surface*3+2]-minZ)*scale );
		//glVertex3fv( &vertices[surface*3] );
		glArrayElement( surface );

		surface = surfaces[j*3+1];
		x = ((vertices[surface*3+0]-minX) * sizeX) * w1;
		y = ((vertices[surface*3+2]-minZ) * sizeZ) * h1;
		x1 = size_t(x)*3;
		y1 = size_t(y)*3;
		glColor3ubv( &rgb[y1*w+x1] );
		if ( bumpMap.Used() )
			glTexCoord2f( (vertices[surface*3+0]-minX)*scale, (vertices[surface*3+2]-minZ)*scale );
		//glVertex3fv( &vertices[surface*3] );
		glArrayElement( surface );

		surface = surfaces[j*3+0];
		x = ((vertices[surface*3+0]-minX) * sizeX) * w1;
		y = ((vertices[surface*3+2]-minZ) * sizeZ) * h1;
		x1 = size_t(x)*3;
		y1 = size_t(y)*3;
		glColor3ubv( &rgb[y1*w+x1] );
		if ( bumpMap.Used() )
			glTexCoord2f( (vertices[surface*3+0]-minX)*scale, (vertices[surface*3+2]-minZ)*scale );
		//glVertex3fv( &vertices[surface*3] );
		glArrayElement( surface );
	}

	glEnd();

	glDisableClientState(GL_VERTEX_ARRAY);

	if ( bumpMap.Used() )
		bumpMap.EndTexture();

    // reset settings
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
	if ( !isColourMap )
	{
		PreCond( numTextureCoords==numVertices );
		PreCond( mesh->NumTextureCoords()==mesh->NumVertices() );
	}

	size_t oldVertices = numVertices;
	MergeFloats( numVertices, vertices,
				 mesh->NumVertices(),mesh->Vertices(),3 );
	MergeSizeT( numFaces, surfaces, mesh->NumFaces(),
				mesh->Surfaces(), 3, oldVertices );

	if ( !isColourMap )
	{
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
		delete []materials;
		materials = newMaterials;
		numMaterials = newNumMaterials;

		// get material faces & re-align them
		MergeSizeT( numMaterialFaces, materialFaces, mesh->NumMaterialFaces(),
					mesh->MaterialFaces(), 1, oldNumMaterials );
	}
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


void TMesh::ReverseWindings( void )
{
	for ( size_t i=0; i<numFaces; i++ )
	{
		size_t v1 = surfaces[i*3+0];
		size_t v2 = surfaces[i*3+1];
		size_t v3 = surfaces[i*3+2];

		surfaces[i*3+0] = v3;
		surfaces[i*3+1] = v2;
		surfaces[i*3+2] = v1;
	}
};


void TMesh::AdjustAxes( float minX, float maxX,
					    float minY, float maxY,
						float minZ, float maxZ,
					    bool minx0, bool miny0, bool minz0, 
						bool maxx0, bool maxy0, bool maxz0 )
{
	size_t i;
	bool* processed = new bool[numFaces*3];
	for ( i=0; i<(numFaces*3); i++ )
		processed[i] = false;

	for ( i=0; i<numFaces; i++ )
	for ( size_t j=0; j<3; j++ )
	{
		size_t v1 = surfaces[i*3+j];
		if ( !processed[v1] )
		{
			processed[v1] = true;

			float v1x = vertices[v1*3+0];
			float v1y = vertices[v1*3+1];
			float v1z = vertices[v1*3+2];

			if ( minx0 )
			{
				v1x -= minX;
			}
			else if ( maxx0 )
			{
				v1x -= maxX;
			}

			if ( miny0 )
			{
				v1y -= minY;
			}
			else if ( maxy0 )
			{
				v1y -= maxY;
			}

			if ( minz0 )
			{
				v1z -= minZ;
			}
			else if ( maxz0 )
			{
				v1z -= maxZ;
			}

			vertices[v1*3+0] = v1x;
			vertices[v1*3+1] = v1y;
			vertices[v1*3+2] = v1z;
		}
	}
}


void TMesh::CenterAxes( bool cx, bool cy, bool cz )
{
	// go through and find the min, max (x,y,z) to define
	// the box area of this object
	float minX, minY, minZ, maxX, maxY, maxZ;

	CalculateBoundaries();
	GetBoundaries( minX,minY,minZ, maxX,maxY,maxZ );

	float centerX = (minX + maxX) / 2.0f;
	float centerY = (minY + maxY) / 2.0f;
	float centerZ = (minZ + maxZ) / 2.0f;

	for ( size_t i=0; i<numVertices; i++ )
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

	CalculateBoundaries();
};


void TMesh::ResizeMesh( float _minX, float _minY, float _minZ,
					    float _maxX, float _maxY, float _maxZ,
					    float bx1, float by1, float bz1,
					    float bx2, float by2, float bz2 )
{
	size_t i;
	bool* processed = new bool[numFaces*3];
	for ( i=0; i<(numFaces*3); i++ )
		processed[i] = false;

	for ( i=0; i<numFaces; i++ )
	for ( size_t j=0; j<3; j++ )
	{
		size_t v1 = surfaces[i*3+j];
		if ( !processed[v1] )
		{
			processed[v1] = true;

			float v1x = vertices[v1*3+0];
			float v1y = vertices[v1*3+1];
			float v1z = vertices[v1*3+2];

			v1x = bx1 + (v1x-_minX)/(_maxX-_minX) * (bx2-bx1);
			v1y = by1 + (v1y-_minY)/(_maxY-_minY) * (by2-by1);
			v1z = bz1 + (v1z-_minZ)/(_maxZ-_minZ) * (bz2-bz1);

			vertices[v1*3+0] = v1x;
			vertices[v1*3+1] = v1y;
			vertices[v1*3+2] = v1z;
		}
	}
};


bool TMesh::Collision( float l1x, float l1y, float l1z,
					   float l2x, float l2y, float l2z ) const
{
	float ov1[3];
	float ov2[3];

	ov1[0] = l1x;
	ov1[1] = l1y;
	ov1[2] = l1z;

	ov2[0] = l2x;
	ov2[1] = l2y;
	ov2[2] = l2z;

	for ( size_t i=0; i<numFaces; i++ )
	{
		size_t v1s = surfaces[i*3+0];
		float* v1 = &vertices[v1s*3+0];

		size_t v2s = surfaces[i*3+1];
		float* v2 = &vertices[v2s*3+0];

		size_t v3s = surfaces[i*3+2];
		float* v3 = &vertices[v3s*3+0];

		if ( LineIntersectsTriangle2( ov1, ov2, v1, v2, v3 ) )
		{
			return true;
		}
	}
	return false;
}


bool TMesh::Collision( const float* matrix,
					   float l1x, float l1y, float l1z,
					   float l2x, float l2y, float l2z ) const
{
	float ov1[3];
	float ov2[3];

	ov1[0] = l1x;
	ov1[1] = l1y;
	ov1[2] = l1z;

	ov2[0] = l2x;
	ov2[1] = l2y;
	ov2[2] = l2z;

	float v1a[3];
	float v2a[3];
	float v3a[3];

	for ( size_t i=0; i<numFaces; i++ )
	{
		size_t v1s = surfaces[i*3+0];
		float* v1 = &vertices[v1s*3+0];

		size_t v2s = surfaces[i*3+1];
		float* v2 = &vertices[v2s*3+0];

		size_t v3s = surfaces[i*3+2];
		float* v3 = &vertices[v3s*3+0];

		::MatrixMult( matrix, v1, v1a );
		::MatrixMult( matrix, v2, v2a );
		::MatrixMult( matrix, v3, v3a );

		if ( LineIntersectsTriangle2( ov1, ov2, v1a, v2a, v3a ) )
		{
			return true;
		}
	}
	return false;
}


bool TMesh::Collision( TMatrix& _matrix,
					   float l1x, float l1y, float l1z,
					   float l2x, float l2y, float l2z )
{
	float ov1[3];
	float ov2[3];

	ov1[0] = l1x;
	ov1[1] = l1y;
	ov1[2] = l1z;

	ov2[0] = l2x;
	ov2[1] = l2y;
	ov2[2] = l2z;

	float v1a[3];
	float v2a[3];
	float v3a[3];

	float matrix[16];
	_matrix.Matrix( matrix );

	for ( size_t i=0; i<numFaces; i++ )
	{
		size_t v1s = surfaces[i*3+0];
		float* v1 = &vertices[v1s*3+0];

		size_t v2s = surfaces[i*3+1];
		float* v2 = &vertices[v2s*3+0];

		size_t v3s = surfaces[i*3+2];
		float* v3 = &vertices[v3s*3+0];

		::MatrixMult( matrix, v1, v1a );
		::MatrixMult( matrix, v2, v2a );
		::MatrixMult( matrix, v3, v3a );

		if ( LineIntersectsTriangle2( ov1, ov2, v1a, v2a, v3a ) )
		{
			return true;
		}
	}
	return false;
}


bool TMesh::Collision( const float* matrix,
					   float l1x, float l1z,
					   float l2x, float l2z ) const
{
	float ov1[3];
	float ov2[3];

	ov1[0] = l1x;
	ov1[1] = 0;
	ov1[2] = l1z;

	ov2[0] = l2x;
	ov2[1] = 0;
	ov2[2] = l2z;

	float v1a[3];
	float v2a[3];
	float v3a[3];

	for ( size_t i=0; i<numFaces; i++ )
	{
		size_t v1s = surfaces[i*3+0];
		float* v1 = &vertices[v1s*3+0];

		size_t v2s = surfaces[i*3+1];
		float* v2 = &vertices[v2s*3+0];

		size_t v3s = surfaces[i*3+2];
		float* v3 = &vertices[v3s*3+0];

		::MatrixMult( matrix, v1, v1a );
		::MatrixMult( matrix, v2, v2a );
		::MatrixMult( matrix, v3, v3a );

		if ( LineIntersectsTriangle2( ov1, ov2, v1a, v2a, v3a ) )
		{
			return true;
		}
	}
	return false;
}


bool TMesh::Collision( size_t numFaces, size_t* surfaces,
					   float l1x, float l1y, float l1z,
					   float size, float& y ) const
{
	bool found = false;
	float dist = -10000;
	for ( size_t i=0; i<numFaces; i++ )
	{
		size_t v1s = surfaces[i*3+0];
		float* v1 = &vertices[v1s*3+0];

		size_t v2s = surfaces[i*3+1];
		float* v2 = &vertices[v2s*3+0];

		size_t v3s = surfaces[i*3+2];
		float* v3 = &vertices[v3s*3+0];

		float tempy;
		if ( LineIntersectsTriangle( l1x,l1y,l1z, l1x, l1y+size, l1z, v1, v2, v3, tempy ) )
		{
			found = true;
			if ( tempy > dist )
			{
				dist = tempy;
				y = tempy;
			}
		}
	}
	return found;
}


bool TMesh::Collision( const float* matrix,
					   size_t numFaces, size_t* surfaces,
					   float l1x, float l1y, float l1z,
					   float size, float& y ) const
{
	float v1a[3];
	float v2a[3];
	float v3a[3];

	bool found = false;
	float dist = -10000;
	for ( size_t i=0; i<numFaces; i++ )
	{
		size_t v1s = surfaces[i*3+0];
		float* v1 = &vertices[v1s*3+0];

		size_t v2s = surfaces[i*3+1];
		float* v2 = &vertices[v2s*3+0];

		size_t v3s = surfaces[i*3+2];
		float* v3 = &vertices[v3s*3+0];

		::MatrixMult( matrix, v1, v1a );
		::MatrixMult( matrix, v2, v2a );
		::MatrixMult( matrix, v3, v3a );

		float tempy;
		if ( LineIntersectsTriangle( l1x,l1y,l1z, l1x,l1y+size,l1z, v1a,v2a,v3a, tempy ) )
		{
			found = true;
			if ( tempy > dist )
			{
				dist = tempy;
				y = tempy;
			}
		}
	}
	return found;
}


void TMesh::FlipUV( void )
{
	float* uvs = textureCoords;
	for ( size_t i=0; i<numTextureCoords; i++ )
	{
		uvs[i*2+1] = -uvs[i*2+1];
	}
};


void TMesh::ManualTransform( void )
{
	for ( size_t i=0; i<numVertices; i++ )
	{
		float v1x = vertices[i*3+0];
		float v1y = vertices[i*3+1] - 0.4f;
		float v1z = vertices[i*3+2];

		vertices[i*3+0] = v1x;
		vertices[i*3+1] = v1y;
		vertices[i*3+2] = v1z;
	}
};


//
//	Given two vertices, find another surface to orgSurface that has these vertices
//	or return -1 if not
//
int TMesh::FindNeighbour( size_t orgSurface, size_t vert1, size_t vert2 )
{
	size_t i = 0;
	while ( i<numFaces )
	{
		if ( i!=orgSurface )
		{
			size_t v1 = surfaces[i*3+0];
			size_t v2 = surfaces[i*3+1];
			size_t v3 = surfaces[i*3+2];
			if ( ( v1==vert1 || v2==vert1 || v3==vert1 ) &&
				 ( v1==vert2 || v2==vert2 || v3==vert2 ) )
				return i;
		}
		i++;
	};

	// if we couldn't find it the simple way - exhaustively search
	// the vertices to see if two points aren't connected after all
	float vx1 = vertices[vert1*3+0];
	float vy1 = vertices[vert1*3+1];
	float vz1 = vertices[vert1*3+2];

	float vx2 = vertices[vert2*3+0];
	float vy2 = vertices[vert2*3+1];
	float vz2 = vertices[vert2*3+2];

	i = 0;
	while ( i<numFaces )
	{
		if ( i!=orgSurface )
		{
			size_t s1 = surfaces[i*3+0];
			size_t s2 = surfaces[i*3+1];
			size_t s3 = surfaces[i*3+2];

			float sx1 = vertices[s1*3+0];
			float sy1 = vertices[s1*3+1];
			float sz1 = vertices[s1*3+2];

			float sx2 = vertices[s2*3+0];
			float sy2 = vertices[s2*3+1];
			float sz2 = vertices[s2*3+2];
		
			float sx3 = vertices[s3*3+0];
			float sy3 = vertices[s3*3+1];
			float sz3 = vertices[s3*3+2];

			bool s1ok = false;
			if ( (vx1==sx1 && vy1==sy1 && vz1==sz1) ||
				 (vx1==sx2 && vy1==sy2 && vz1==sz2) ||
				 (vx1==sx3 && vy1==sy3 && vz1==sz3) )
				s1ok = true;

			bool s2ok = false;
			if ( (vx2==sx1 && vy2==sy1 && vz2==sz1) ||
				 (vx2==sx2 && vy2==sy2 && vz2==sz2) ||
				 (vx2==sx3 && vy2==sy3 && vz2==sz3) )
				s2ok = true;

			if ( s1ok && s2ok )
				return i;
		}
		i++;
	};
	return -1;
};


void TMesh::CreateSurfaceGraph( int* surfaceGraph )
{
	ResetPercentage();

	for ( size_t i=0; i<numFaces; i++ )
	{
		DisplayPercentage(i,numFaces);

		size_t s1 = surfaces[i*3+0];
		size_t s2 = surfaces[i*3+1];
		size_t s3 = surfaces[i*3+2];

		int surf = FindNeighbour(i,s1,s2);
		surfaceGraph[i*3+0] = surf;

		surf = FindNeighbour(i,s2,s3);
		surfaceGraph[i*3+1] = surf;

		surf = FindNeighbour(i,s3,s1);
		surfaceGraph[i*3+2] = surf;
	};
	FinishPercentage();
};


float TMesh::Distance( size_t v1, size_t v2 )
{
	float x1 = vertices[v1*3+0];
	float y1 = vertices[v1*3+1];
	float z1 = vertices[v1*3+2];

	float x2 = vertices[v2*3+0];
	float y2 = vertices[v2*3+1];
	float z2 = vertices[v2*3+2];

	return (x1-x2)*(x1-x2) + 
		   (y1-y2)*(y1-y2) +
		   (z1-z2)*(z1-z2);
};


void TMesh::GetTriangles( size_t c, size_t t1, size_t t2, size_t t3,
						  size_t& t1s1, size_t& t1s2, size_t& t1s3,
						  size_t& t2s1, size_t& t2s2, size_t& t2s3 )
{
	size_t s1,s2,s3,s4;

	size_t v1 = surfaces[c*3+0];
	size_t v2 = surfaces[c*3+1];
	size_t v3 = surfaces[c*3+2];

	size_t v11 = surfaces[t1*3+0];
	size_t v12 = surfaces[t1*3+1];
	size_t v13 = surfaces[t1*3+2];

	size_t v21 = surfaces[t2*3+0];
	size_t v22 = surfaces[t2*3+1];
	size_t v23 = surfaces[t2*3+2];

	size_t v31 = surfaces[t3*3+0];
	size_t v32 = surfaces[t3*3+1];
	size_t v33 = surfaces[t3*3+2];

	if ( (v11==v1 && v12==v2) ||
		 (v11==v1 && v12==v3) ||
		 (v11==v2 && v12==v1) ||
		 (v11==v2 && v12==v3) ||
		 (v11==v3 && v12==v1) ||
		 (v11==v3 && v12==v2) )
		s1 = v13;

	if ( (v12==v1 && v13==v2) ||
		 (v12==v1 && v13==v3) ||
		 (v12==v2 && v13==v1) ||
		 (v12==v2 && v13==v3) ||
		 (v12==v3 && v13==v1) ||
		 (v12==v3 && v13==v2) )
		s1 = v11;

	if ( (v11==v1 && v13==v2) ||
		 (v11==v1 && v13==v3) ||
		 (v11==v2 && v13==v1) ||
		 (v11==v2 && v13==v3) ||
		 (v11==v3 && v13==v1) ||
		 (v11==v3 && v13==v2) )
		s1 = v12;


	if ( (v21==v1 && v22==v2) ||
		 (v21==v1 && v22==v3) ||
		 (v21==v2 && v22==v1) ||
		 (v21==v2 && v22==v3) ||
		 (v21==v3 && v22==v1) ||
		 (v21==v3 && v22==v2) )
		s2 = v23;

	if ( (v22==v1 && v23==v2) ||
		 (v22==v1 && v23==v3) ||
		 (v22==v2 && v23==v1) ||
		 (v22==v2 && v23==v3) ||
		 (v22==v3 && v23==v1) ||
		 (v22==v3 && v23==v2) )
		s2 = v21;

	if ( (v21==v1 && v23==v2) ||
		 (v21==v1 && v23==v3) ||
		 (v21==v2 && v23==v1) ||
		 (v21==v2 && v23==v3) ||
		 (v21==v3 && v23==v1) ||
		 (v21==v3 && v23==v2) )
		s2 = v22;


	if ( (v31==v1 && v32==v2) ||
		 (v31==v1 && v32==v3) ||
		 (v31==v2 && v32==v1) ||
		 (v31==v2 && v32==v3) ||
		 (v31==v3 && v32==v1) ||
		 (v31==v3 && v32==v2) )
		s3 = v33;

	if ( (v32==v1 && v33==v2) ||
		 (v32==v1 && v33==v3) ||
		 (v32==v2 && v33==v1) ||
		 (v32==v2 && v33==v3) ||
		 (v32==v3 && v33==v1) ||
		 (v32==v3 && v33==v2) )
		s3 = v31;

	if ( (v31==v1 && v33==v2) ||
		 (v31==v1 && v33==v3) ||
		 (v31==v2 && v33==v1) ||
		 (v31==v2 && v33==v3) ||
		 (v31==v3 && v33==v1) ||
		 (v31==v3 && v33==v2) )
		s3 = v32;

	// get areas of 3 potential triangles and
	// pick the largest
	float d1 = Distance( s1, v1 );
	float d2 = Distance( s1, v2 );
	float d3 = Distance( s1, v3 );

	if ( d1>d2 && d1>d3 )
		s4 = v1;
	else if ( d2>d1 && d2>d3 )
		s4 = v2;
	else
		s4 = v3;
	t1s1 = s1;
	t1s2 = s4;
	t1s3 = s2;
	t2s1 = s4;
	t2s2 = s3;
	t2s3 = s2;
};


void TMesh::HalfMesh( void )
{
	size_t i;

	size_t* newsurf = new size_t[numFaces*3];
	size_t index = 0;

	int* sg = new int[numFaces*3];
	bool* used = new bool[numFaces];
	for ( i=0; i<(numFaces*3); i++ )
		sg[i] = -1;
	for ( i=0; i<numFaces; i++ )
		used[i] = false;
	CreateSurfaceGraph( sg );

	for ( i=0; i<numFaces; i++ )
	{
		if ( sg[i*3+0]==-1 ||
			 sg[i*3+1]==-1 ||
			 sg[i*3+2]==-1 )
		{
			newsurf[index*3+0] = surfaces[i*3+0];
			newsurf[index*3+1] = surfaces[i*3+1];
			newsurf[index*3+2] = surfaces[i*3+2];
			index++;
		}
		else if ( !used[i] )
		{
			size_t s1 = sg[i*3+0];
			size_t s2 = sg[i*3+1];
			size_t s3 = sg[i*3+2];

			if ( !(used[s1] || used[s2] || used[s3]) )
			{
				size_t t1s1,t1s2,t1s3;
				size_t t2s1,t2s2,t2s3;

				GetTriangles( i, s1,s2,s3,
							  t1s1,t1s2,t1s3,
							  t2s1,t2s2,t2s3 );

				newsurf[index*3+0] = t1s1;
				newsurf[index*3+1] = t1s2;
				newsurf[index*3+2] = t1s3;
				index++;
				newsurf[index*3+0] = t2s1;
				newsurf[index*3+1] = t2s2;
				newsurf[index*3+2] = t2s3;
				index++;

				used[i] = true;
			}
		}
	}
	numFaces = index;
	delete []surfaces;
	surfaces = newsurf;
};

/*
bool TMesh::IntroduceProcedural( TMapHeightInfo* info, TString& errStr )
{
	// give each face a texture
	NumMaterialFaces( NumFaces() );

	// go through each face and assign a material as dictated by info
	for ( size_t i=0; i<NumFaces(); i++ )
	{
		// get triangle height
		size_t s1 = surfaces[i*3+0];
		size_t s2 = surfaces[i*3+1];
		size_t s3 = surfaces[i*3+2];

		float y = ( vertices[s1*3+1] + vertices[s2*3+1] + vertices[s3*3+1] ) * 0.333f;

		// find applicable rule
		bool found = false;
		size_t cntr = 0;
		float h;
		size_t num;
		size_t indexes[kMaxMaterialEntries];
		while ( cntr<info->Count() && !found )
		{
			info->GetHeight( cntr, h, num, indexes );
			if ( y<=h )
			{
				found = true;
			}
			else
				cntr++;
		}
		// assume we now have an array of items, pick one at random
		float r = Random() * float(num);
		size_t ind = size_t(r);
		size_t matIndex = indexes[ind];

		materialFaces[i] = matIndex;
	}
	return true;
};
*/

bool TMesh::IsColourMapped( void ) const
{
	return isColourMap;
};

void TMesh::IsColourMapped( bool cm )
{
	isColourMap = cm;
};

bool TMesh::OwnerDraw( void ) const
{
	return ownerDraw;
};

void TMesh::OwnerDraw( bool o )
{
	ownerDraw = o;
}

void TMesh::MultiplyUV( float um, float vm )
{
	float* t = textureCoords;
	for ( size_t i=0; i<numTextureCoords; i++ )
	{
		float u = t[i*2+0] * um;
		t[i*2+0] = u;
		float v = t[i*2+1] * vm;
		t[i*2+1] = v;
	}
};

void TMesh::Center( float cx, float cy, float cz )
{
	for ( size_t i=0; i<numVertices; i++ )
	{
		float x = vertices[i*3+0] - cx;
		float y = vertices[i*3+1] - cy;
		float z = vertices[i*3+2] - cz;

		vertices[i*3+0] = x;
		vertices[i*3+1] = y;
		vertices[i*3+2] = z;
	}
};

void TMesh::ResetMaterials( TBinObject* obj )
{
	if ( numMaterials>0 )
	{
		PreCond( obj!=NULL );
	    for ( size_t i=0; i<numMaterials; i++ )
	    {
    		TMaterial* mat = obj->GetMaterial( materialParentIndex[i] );
			Materials( i, materialParentIndex[i], mat );
		}
	}
};


bool TMesh::ReloadBumpmap( TString& errStr )
{
	if ( bumpMap.Used() )
	{
		if ( !bumpMap.Reload(errStr) )
		{
			return false;
		}
	}
	return true;
};


bool TMesh::Reload( TString& errStr )
{
	for ( size_t i=0; i<numMaterials; i++ )
	{
		PreCond( materials[i]!=NULL );
		if ( !materials[i]->Reload(errStr) )
		{
			return false;
		}
	}
	return ReloadBumpmap( errStr );
};

//==========================================================================


