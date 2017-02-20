#include <precomp_header.h>

#include <win32/win32.h>
#include <object/binobject.h>
#include <object/material.h>
#include <object/landscape.h>
#include <object/mesh.h>
#include <object/geometry.h>

//==========================================================================

TLandscape::TLandscape( void )
	: divisions(NULL),
	  noGoDivisions(NULL),
	  divx(0),
	  divy(0),
	  divSizeX(0.0f),
	  divSizeY(0.0f),
	  gsurfaces(NULL),
	  gvertices(NULL),
	  gnumSurfaces(0),
	  gnumVertices(0),
	  visitedValue(0),
	  landscapeMesh(NULL),
	  surfaceDivisionCounts(NULL),
	  surfaceDivisions(NULL),
	  surfaceMaterialFaces(NULL),
	  circle(NULL),
	  surfaceGraph(NULL),
	  bridges(NULL),
	  bridgeMeshes(NULL),
	  objectNames(NULL),
	  objectCache(NULL),
	  objectCount(0),
	  initialised(false)
{
	signature = "PDVLND10";
	renDepth = 0;
	numBridges = 0;
	recursionCounter = 0;
	waterLevel = 0.0f;

	landscapeminX = 0;
	landscapeminZ = 0;

	bridgeMeshes = new TMesh*[kMaxNumBridges];

	size_t i;
	for ( i=0; i<kMaxNumBridges; i++ )
		bridgeMeshes[i] = NULL;
};


TLandscape::~TLandscape( void )
{
	if ( circle!=NULL )
		delete circle;
	circle = NULL;

	initialised = false;
	DeleteDivisions();
};


void TLandscape::DeleteDivisions( void )
{
	if ( landscapeMesh!=NULL )
		delete landscapeMesh;
	landscapeMesh = NULL;

	if ( divisions!=NULL )
	{
		for ( size_t x=0; x<divx; x++ )
		for ( size_t y=0; y<divy; y++ )
			if ( divisions[y*divx+x]!=NULL )
			{
				delete divisions[y*divx+x];
				divisions[y*divx+x] = NULL;
			}
		delete []divisions;
		divisions = NULL;
	}

	if ( noGoDivisions!=NULL )
	{
		for ( size_t x=0; x<divx; x++ )
		for ( size_t y=0; y<divy; y++ )
			if ( noGoDivisions[y*divx+x]!=NULL )
			{
				delete noGoDivisions[y*divx+x];
				noGoDivisions[y*divx+x] = NULL;
			}
		delete []noGoDivisions;
		noGoDivisions = NULL;
	}

	if ( surfaceDivisionCounts!=NULL )
		delete surfaceDivisionCounts;
	surfaceDivisionCounts = NULL;

	if ( surfaceDivisions!=NULL )
	{
		for ( size_t x=0; x<divx; x++ )
		for ( size_t y=0; y<divy; y++ )
			if ( surfaceDivisions[y*divx+x]!=NULL )
			{
				delete surfaceDivisions[y*divx+x];
				surfaceDivisions[y*divx+x] = NULL;
			}
		delete []surfaceDivisions;
		surfaceDivisions = NULL;
	}

	if ( surfaceMaterialFaces!=NULL )
	{
		for ( size_t x=0; x<divx; x++ )
		for ( size_t y=0; y<divy; y++ )
			if ( surfaceMaterialFaces[y*divx+x]!=NULL )
			{
				delete surfaceMaterialFaces[y*divx+x];
				surfaceMaterialFaces[y*divx+x] = NULL;
			}
		delete []surfaceMaterialFaces;
		surfaceMaterialFaces = NULL;
	}

	if ( bridgeMeshes!=NULL )
	{
		for ( size_t i=0; i<numBridges; i++ )
			if ( bridgeMeshes[i]!=NULL )
			{
				delete bridgeMeshes[i];
				bridgeMeshes[i] = NULL;
			}
		delete []bridgeMeshes;
		bridgeMeshes = NULL;
	}
	if ( bridges!=NULL )
		delete bridges;
	numBridges = 0;


	gsurfaces = NULL;
	gnumSurfaces = 0;
	gvertices = NULL;
	gnumVertices = 0;

	if ( surfaceGraph!=NULL )
		delete surfaceGraph;
	surfaceGraph = NULL;
}


bool TLandscape::SaveBinary( const TString& fname )
{
	size_t x,y,i;
	TPersist file(fileWrite);

	if ( !initialised )
		return false;

	if ( !file.FileOpen( fname ) )
		return false;

    // write signature to file
    int index = 0;
    while ( signature[index]!=0 )
    {
    	file.FileWrite( &signature[index], 1 );
    	index++;
    }

    int num = NumMaterials();
    file.FileWrite( &num, sizeof(int) );
	num = divx;
    file.FileWrite( &num, sizeof(int) );
	num = divy;
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
	for ( y=0; y<divy; y++ )
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
	PreCond( landscapeMesh!=NULL );
	landscapeMesh->SaveBinary( file, this );

	// save landscape surface divisions
	for ( y=0; y<divy; y++ )
	for ( x=0; x<divx; x++ )
    {
		file.FileWrite( &surfaceDivisionCounts[y*divx+x], sizeof(size_t) );
		if ( surfaceDivisionCounts[y*divx+x]>0 )
		{
			file.FileWrite( surfaceDivisions[y*divx+x], surfaceDivisionCounts[y*divx+x]*3*sizeof(size_t) );
			file.FileWrite( surfaceMaterialFaces[y*divx+x], surfaceDivisionCounts[y*divx+x]*sizeof(size_t) );
		}
    }

	// save surfaceGraph, first cut it back to 3* surfaces
	int* newSurfaceGraph = new int[3*gnumSurfaces];
	PostCond( newSurfaceGraph!=NULL );
	for ( i=0; i<gnumSurfaces; i++ )
	{
		newSurfaceGraph[i*3+0] = surfaceGraph[i*4+0];
		newSurfaceGraph[i*3+1] = surfaceGraph[i*4+1];
		newSurfaceGraph[i*3+2] = surfaceGraph[i*4+2];
	}
	file.FileWrite( newSurfaceGraph, 3*gnumSurfaces*sizeof(int) );

	// save bridges
	file.FileWrite( &numBridges, sizeof(size_t) );
	file.FileWrite( bridges, numBridges*kBridgeSize*sizeof(float) );
	for ( i=0; i<numBridges; i++ )
	{
		PreCond( bridgeMeshes[i]!=NULL );
		bridgeMeshes[i]->SaveBinary( file, NULL );
	}

	// landscape objects
	TLandscapeObjectList* first = objectList.Next();
	i=0;
	while ( first!=NULL )
	{
		i++;
		first = first->Next();
	}
	file.FileWrite( &i, sizeof(size_t) );

	first = objectList.Next();
	while ( first!=NULL )
	{
		first->Object().SaveBinary( file );
		first = first->Next();
	}

	// sea
	file.FileWrite( &waterLevel, sizeof(float) );

    file.FileClose();
    return true;
};


bool TLandscape::LoadBinary( const TString& fname, TString& errStr, 
								   const TString& pathname,
								   const TString& texturePath )
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
    	errStr = "Error incorrect file type or version \"" + openName;
		errStr = errStr + "\"";
    	return false;
    }

    int numMaterials;
    file.FileRead( &numMaterials, sizeof(int) );
    file.FileRead( &divx, sizeof(int) );
    file.FileRead( &divy, sizeof(int) );

	// calculate min and max render depths
	float depth = float(divx + divy) * 0.5f;
	kMinRenderDepth = size_t( depth / 8.0f );
	if ( kMinRenderDepth<3 )
		kMinRenderDepth =3;
	kMaxRenderDepth = size_t( depth / 6.0f );
	if ( kMaxRenderDepth<4 )
		kMaxRenderDepth = 4;
	renderDepth = kMaxRenderDepth;
	RenderDepth( renderDepth );
    
	file.FileRead(&landscapeminX,sizeof(float));
	file.FileRead(&landscapeminZ,sizeof(float));
	file.FileRead(&divSizeX,sizeof(float));
	file.FileRead(&divSizeY,sizeof(float));

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
	divisions = new TMesh*[divx*divy];
	PostCond( divisions!=NULL );
	noGoDivisions = new TLandscapeObjectDraw*[divx*divy];
	PostCond( noGoDivisions!=NULL );

	size_t valid;
	objectSize = 0;
	for ( y=0; y<divy; y++ )
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

	for ( y=0; y<divy; y++ )
	for ( x=0; x<divx; x++ )
    {
		noGoDivisions[y*divx+x] = NULL;
    }

	// load landscape mesh
	landscapeMesh = new TMesh();
	PostCond( landscapeMesh!=NULL );
	if ( !landscapeMesh->LoadBinary( file, this ) )
	{
		errStr = "Error loading landscapeMesh in object \"" + openName;
		errStr = errStr + "\n";
		return false;
	}
	objectSize += sizeof(TMeshList);
	objectSize += landscapeMesh->ObjectSize();

	gvertices = landscapeMesh->Vertices();
	gnumVertices = landscapeMesh->NumVertices();
	gsurfaces = landscapeMesh->Surfaces();
	gnumSurfaces = landscapeMesh->NumFaces();

	// load landscape surface divisions
	surfaceDivisionCounts = new size_t[divx*divy];
	PostCond( surfaceDivisionCounts!=NULL );
	surfaceDivisions = new size_t*[divx*divy];
	PostCond( surfaceDivisions!=NULL );
	surfaceMaterialFaces = new size_t*[divx*divy];
	PostCond( surfaceMaterialFaces!=NULL );

	for ( y=0; y<divy; y++ )
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

			ptr = new size_t[size];
			PostCond( ptr!=NULL );
			file.FileRead( ptr, size*sizeof(size_t) );
			surfaceMaterialFaces[y*divx+x] = ptr;
		}
		else
		{
			surfaceDivisions[y*divx+x] = NULL;
			surfaceMaterialFaces[y*divx+x] = NULL;
		}
    }

	// load surfaceGraph
	if ( surfaceGraph!=NULL )
		delete surfaceGraph;
	surfaceGraph = new int[gnumSurfaces*3];
	PostCond( surfaceGraph!=NULL );
	file.FileRead( surfaceGraph, 3*gnumSurfaces*sizeof(int) );

	// spread surfaceGraph into a 4*sizeof(int) structure
	int* newSurfaceGraph = new int[gnumSurfaces*4];
	PostCond( newSurfaceGraph!=NULL );
	for ( i=0; i<gnumSurfaces; i++ )
	{
		newSurfaceGraph[i*4+0] = surfaceGraph[i*3+0];
		newSurfaceGraph[i*4+1] = surfaceGraph[i*3+1];
		newSurfaceGraph[i*4+2] = surfaceGraph[i*3+2];
		newSurfaceGraph[i*4+3] = 0;
	}
	delete surfaceGraph;
	surfaceGraph = newSurfaceGraph;

	// load bridges
	file.FileRead( &numBridges, sizeof(size_t) );
	if ( bridges!=NULL )
		delete bridges;
	if ( numBridges>0 )
	{
		bridges = new float[numBridges*kBridgeSize];
		PostCond( bridges!=NULL );
		file.FileRead( bridges, numBridges*kBridgeSize*sizeof(float) );
	}
	else
		bridges = NULL;

	for ( i=0; i<numBridges; i++ )
	{
		bridgeMeshes[i] = new TMesh();
		bridgeMeshes[i]->LoadBinary( file, NULL );
	}

	// read landscape objects
	size_t cnt;
	file.FileRead( &cnt, sizeof(size_t) );
	for ( i=0; i<cnt; i++ )
	{
		TLandscapeObject* lo = new TLandscapeObject();
		if ( !lo->LoadBinary( file, errStr ) )
			return false;

		TString str;
		str = lo->Name();
		TCompoundObject* obj = LoadLandscapeObject( str, errStr, 
													pathname, 
													texturePath );
		if ( obj==NULL )
			return false;
		lo->Object( obj );

		PreCond( obj->Objects()!=NULL );
		AddLandscapeObject( lo );
	}

	// sea
	file.FileRead( &waterLevel, sizeof(float) );

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


void TLandscape::RemoveDuplicates( size_t numVertices, float* vertices,
										 size_t numFaces, size_t* surfaces )
{
	// kill duplicates from the vertices & surfaces
	for ( size_t i=0; i<numVertices; i++ )
	{
	DisplayPercentage( i, numVertices );
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
		};
	}
	}
}


bool TLandscape::IsObjectName( const TString& str ) const
{
	size_t index = 0;
	while ( str[index]!=0 && str[index]!='.' ) index++;
	if ( str[index]=='.' )
	if ( strnicmp( &str[index], ".obj", 4 )==0 )
		return true;
	return false;
};


TCompoundObject* TLandscape::LoadLandscapeObject( TString& fname, TString& errStr,
												  const TString& path,
												  const TString& texturePath )
{
	size_t i;

	// get name proper from objname
	i=0;
	while ( fname[i]!=0 && fname[i]!='.' ) i++;
	if ( fname[i]!='.' )
	{
		errStr = "Error: " + fname;
		errStr = errStr + " is not an object name\n";
		return NULL;
	}
	if ( strnicmp( &fname[i], ".obj", 4 )!=0 )
	{
		errStr = "Error: " + fname;
		errStr = errStr + " is not an object name\n";
		return NULL;
	}
	fname[i+4] = 0;

	// setup cache if necessairy
	if ( objectNames==NULL )
	{
		objectNames = new TString[256];
		PostCond( objectNames!=NULL );
		objectCache = new TCompoundObject[256];
		PostCond( objectCache!=NULL );
		objectCount = 0;
	}

	// find object in cache if exists
	for ( i=0; i<objectCount; i++ )
	{
		if ( objectNames[i] == fname )
			return &objectCache[i];
	}

	// not found - load it into cache
	if ( objectCount>=255 )
	{
		errStr = "Error: cache object overflow\n";
		return NULL;
	}

	objectNames[objectCount] = fname;
	if ( !objectCache[objectCount].LoadBinary( fname, errStr, path, texturePath ) )
		return NULL;

	return &objectCache[objectCount++];
};


bool TLandscape::DivideMeshes( TObject& landscape, 
							   size_t xdiv, 
							   size_t ydiv, 
							   TString& errStr, 
							   const TString& fileName,
							   const TString& path, 
							   const TString& texturePath )
{
	size_t i;

	errStr = "";

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
	divy = ydiv;

	PreCond( divx>1 && divy>1 );
	numBridges = 0;

	DeleteDivisions();
	divisions = new TMesh*[divx*divy];
	surfaceDivisionCounts = new size_t[divx*divy];
	surfaceDivisions = new size_t*[divx*divy];
	surfaceMaterialFaces = new size_t*[divx*divy];
	noGoDivisions = new TLandscapeObjectDraw*[divx*divy];
	PostCond( divisions!=NULL );
	PostCond( surfaceDivisions!=NULL );
	PostCond( surfaceDivisionCounts!=NULL );
	PostCond( noGoDivisions!=NULL );
	for ( i=0; i<(divx*divy); i++ )
	{
		surfaceDivisionCounts[i] = 0;
		divisions[i] = NULL;
		surfaceDivisions[i] = NULL;
		surfaceMaterialFaces[i] = NULL;
		noGoDivisions[i] = NULL;
	}


	WriteString( "\n" );

	if ( bridgeMeshes==NULL )
	{
		bridgeMeshes = new TMesh*[kMaxNumBridges];
		for ( size_t i=0; i<kMaxNumBridges; i++ )
			bridgeMeshes[i] = NULL;
	}

	landscapeminX = landscape.MinX();
	landscapeminZ = landscape.MinZ();

	// Create a landscape definition file from all the objects
	// inside the landscape file
	size_t numObjects = 0;
	TString defFilename;
	defFilename = fileName;
	i=0;
	while ( defFilename[i]!='.' && defFilename[i]!=0 ) i++;
	if ( defFilename[i]=='.' )
	{
		defFilename = defFilename.substr(0,i);
		defFilename = defFilename + ".def";
	}
	else
		defFilename = defFilename + ".def";

	TString name;
	ConstructPath( name, path, defFilename );
	TPersist file( fileWrite );
	file.FileOpen( name );
	for ( i=0; i<landscape.NumMeshes(); i++ )
	{
		TMesh* mesh = landscape.GetMesh(i);
		if ( mesh->Name()!=NULL )
		if ( IsObjectName( mesh->Name() ) )
		{
			numObjects++;

			// load the equivalent object for comparison
			TString objName;
			objName = mesh->Name();
			TCompoundObject* obj = LoadLandscapeObject( objName, errStr, path, texturePath );
			if ( obj==NULL )
			{
				file.FileClose();
				return false;
			}

			// get center of this particular mesh
			float mnx,mny,mnz,mxx,mxy,mxz;
			mesh->GetBoundaries( mnx,mny,mnz, mxx,mxy,mxz );
			float cx,cy,cz;

			cx = (mnx+mxx) * 0.5f;
			cy = (mny+mxy) * 0.5f;
			cz = (mnz+mxz) * 0.5f;
			
			// obj is centered at (0,0,0) - cx,cy,cz are the required
			// translation to move this screen object to the real object
			TString str;
			str = FormatString( "\nObject %s\n{\n", objName );
			file.FileWrite( str.c_str(), str.length() );
			str = FormatString( "T(%2.4f,%2.4f,%2.4f)\n", cx,cy,cz );
			file.FileWrite( str.c_str(), str.length() );

			// figure out rotation of the object
			str = FormatString( "R(%2.4f,%2.4f,%2.4f)\n", 0.0f,0.0f,0.0f );
			file.FileWrite( str.c_str(), str.length() );

			file.FileWrite( "}\n", 2 );

			AddLandscapeObject( new TLandscapeObject( objName, cx,cy,cz, 0,0,0 ) );
		}
	}
	file.FileClose();
	WriteString( "found %d objects\n", numObjects );

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
			}
		}
		i++;
	};

	if ( !found )
	{
		errStr = "Could not find any meshes labelled \"landscapeMesh\".\nCan't process this file.";
		return false;
	};

	i = 0;
	found = false;
	while ( i<landscape.NumMeshes() )
	{
		TMesh* mesh = landscape.GetMesh(i);
		if ( mesh->Name()!=NULL )
		{
			if ( stricmp( mesh->Name().c_str(), "seamesh" )==0 )
			{
				found = true;
			}
		}
		i++;
	};

	if ( !found )
	{
		errStr = "Could not find any meshes labelled \"seaMesh\".\nCan't process this file.";
		return false;
	};

	// for each mesh
	for ( size_t cntr=0; cntr<landscape.NumMeshes(); cntr++ )
	{
		TMesh* mland = landscape.GetMesh(cntr);
		PostCond( mland!=NULL );
		PostCond( mland->Name()!=NULL );

		bool isBridgeLimit = (strnicmp( mland->Name().c_str(), "bridgelimit", 11 )==0);

		WriteString( "processing mesh \"%s\" (%d of %d)\n",
					 mland->Name().c_str(), cntr+1, landscape.NumMeshes() );

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

		if ( mland->NumNormalFaces()==0 && !isBridgeLimit )
		{
			errStr = "Landscape must have normals to function";
			return false;
		}

		// must be true for this to work properly
		if ( numSurfaces!=mland->NumNormalFaces() && !isBridgeLimit )
		{
			errStr = "MESH error: \"" + mland->Name();
			errStr = errStr + "\" numSurfaces!=numNormalFaces()\n";
			return false;
		};

		// make sure this never happens
		if ( numMaterialFaces!=numSurfaces && !isBridgeLimit )
		{
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
		if ( !isBridgeLimit )
		{
			PreCond( numSurfaces>0 );

			processed = new bool[numSurfaces];
			PostCond( processed!=NULL );

			for ( i=0; i<numSurfaces; i++ )
				processed[i] = false;
		}

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

			// setup a cache for fast lookup
			surfaceGraph = new int[gnumSurfaces*4];
			PostCond( surfaceGraph!=NULL );
			for ( i=0; i<gnumSurfaces; i++ )
			{
				surfaceGraph[i*4+0] = -1;
				surfaceGraph[i*4+1] = -1;
				surfaceGraph[i*4+2] = -1;
				surfaceGraph[i*4+3] = 0;
			}

			WriteString( "Processing landscapeMesh - creating graph\n" );
			CreateSurfaceGraph();

			//
			// create a new Mesh at (x,y) with newSurfaceCount surfaces
			//
			landscapeMesh = new TMesh();
			PostCond( landscapeMesh!=NULL );
	
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
				landscapeMesh->Materials( i, mland->Materials(i) );
		}
		else if ( isBridgeLimit )
		{
			doAdd = false;
			WriteString( "Processing Bridge Surface object\n" );
			AddBridge( mland );
		}
		else if ( strnicmp( mland->Name().c_str(), "seamesh", 7 )==0 )
		{
			WriteString( "Processing sea object\n" );
			float minx,miny,minz,maxx,maxy,maxz;
			mland->GetBoundaries( minx,miny,minz, maxx,maxy,maxz );
			waterLevel = maxy;
		}
		else if ( IsObjectName(mland->Name()) )
		{
			WriteString( "object already processed\n" );
			doAdd = false;
		}
		else if ( strnicmp( mland->Name().c_str(),"leaf",4)==0 )
		{
			WriteString( "not adding leaf object - obsoleted\n" );
			doAdd = false;
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
					mesh->NumTextureCoords( newSurfaceCount*3 );
					mesh->NumMaterialFaces( newSurfaceCount );
					mesh->NumNormals( newSurfaceCount*3 );
					mesh->NumNormalFaces( newSurfaceCount );

					// copy material references
					size_t nms = mland->NumMaterials();
					mesh->NumMaterials( nms );
					for ( i=0; i<nms; i++ )
						mesh->Materials( i, mland->Materials(i) );
	
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
							newTextureCoords[vertexIndex*2+0] = textureCoords[surf*2+0];
							newTextureCoords[vertexIndex*2+1] = textureCoords[surf*2+1];
	
							surf = surfaces[i*3+1];
							vertexIndex = GetVertex( newVertices, &vertices[surf*3], newVertexCount );
							newFaces[newSurfaceCount*3+1] = vertexIndex;
							newTextureCoords[vertexIndex*2+0] = textureCoords[surf*2+0];
							newTextureCoords[vertexIndex*2+1] = textureCoords[surf*2+1];

							surf = surfaces[i*3+2];
							vertexIndex = GetVertex( newVertices, &vertices[surf*3], newVertexCount );
							newFaces[newSurfaceCount*3+2] = vertexIndex;
							newTextureCoords[vertexIndex*2+0] = textureCoords[surf*2+0];
							newTextureCoords[vertexIndex*2+1] = textureCoords[surf*2+1];
				
							// copy material faces
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

					ptr = new size_t[newSurfaceCount];
					surfaceMaterialFaces[y*divx+x] = ptr;
					PostCond( surfaceMaterialFaces[y*divx+x]!=NULL );

					size_t cntr = 0;
					size_t matCntr = 0;
					size_t* newSurfaces = surfaceDivisions[y*divx+x];
					size_t* matFaces = surfaceMaterialFaces[y*divx+x];
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
							matFaces[matCntr++] = materialFaces[i];
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


void TLandscape::Draw( float x, float y, float z, 
					   float yAngle, float xAngle, 
					   float vx, float vz,
					   bool showBoundingBox )
{
	if ( initialised )
	{
		PreCond( circle!=NULL );

		int cX = int((-x-MinX()) / divSizeX);
		int cZ = int((-z-MinZ()) / divSizeY);

		yAngle -= 90.0f;
		float sliceSize = 80.0f + (xAngle / 2.0f); // tilt determines view optimisation
		float minAngle = yAngle - sliceSize;
		float maxAngle = yAngle + sliceSize;
		while ( minAngle < 0.0f ) minAngle += 360.0f;
		while ( maxAngle > 360.0f ) maxAngle -= 360.0f;

		int rd = int(renDepth);
		for ( int x1=-rd; x1<rd; x1++ )	// only draw up to renderDepth radius
		for ( int y1=-rd; y1<rd; y1++ )
		if ( circle[(y1+rd)*2*rd+(x1+rd)] ) // only draw inside the circle
		{
			// only draw view angles 0 to 180
			bool doDraw = true;
			if (!( x1>-3 && x1<3 && y1>-3 && y1<3 ))
			{
				float angle = VectorsToAngle( 0,0, float(x1), float(y1) );
				if ( !AngleInsideSlice( angle, minAngle, maxAngle ) )
					doDraw = false;
			}
			if ( doDraw )
			{
				int newX = (cX+x1);
				int newY = (cZ+y1);
				if ( newX>0 && newX<int(divx) )
				if ( newY>0 && newY<int(divy) )
				{
					if ( x1==0 && y1==0 )
						x1 = 0;

					if ( surfaceDivisions[newY*divx+newX]!=NULL )
						landscapeMesh->Draw( surfaceDivisionCounts[newY*divx+newX],
											 surfaceDivisions[newY*divx+newX],
											 surfaceMaterialFaces[newY*divx+newX] );

					if ( divisions[newY*divx+newX]!=NULL )
						divisions[newY*divx+newX]->Draw();

					if ( noGoDivisions[newY*divx+newX]!=NULL )
					{
						TLandscapeObjectDraw* ngd = noGoDivisions[newY*divx+newX];
						while ( ngd!=NULL )
						{
							ngd->Draw();
							ngd = ngd->Next();
						};
					}
				}
			}
		}

		if ( showBoundingBox )
			DrawBoundingBox();
	}
};


bool TLandscape::InNoGoZone( float x1, float z1,
							 float x2, float z2,
							 float x3, float z3,
							 float x4, float z4 ) const
{
	float cx,cz;
	cx = (x1+x2+x3+x4)*0.25f;
	cz = (z1+z2+z3+z4)*0.25f;

	cx = (cx-landscapeminX) / divSizeX;
	cz = (cz-landscapeminZ) / divSizeY;

	size_t x = size_t(cx);
	size_t z = size_t(cz);

	TLandscapeObjectDraw* ngd = noGoDivisions[z*divx+x];
	while ( ngd!=NULL )
	{
		if ( ngd->InNoGoZone(x1,z1,x2,z2,x3,z3,x4,z4) )
			return true;
		ngd = ngd->Next();
	};
	return false;
}


//
//	Given two vertices, find another surface to orgSurface that has these vertices
//	or return -1 if not
//
int TLandscape::FindNeighbour( size_t orgSurface, size_t vert1, size_t vert2 )
{
	size_t i = 0;
	while ( i<gnumSurfaces )
	{
		if ( i!=orgSurface )
		{
			size_t v1 = gsurfaces[i*3+0];
			size_t v2 = gsurfaces[i*3+1];
			size_t v3 = gsurfaces[i*3+2];
			if ( ( v1==vert1 || v2==vert1 || v3==vert1 ) &&
				 ( v1==vert2 || v2==vert2 || v3==vert2 ) )
				return i;
		}
		i++;
	};

	// if we couldn't find it the simple way - exhaustively search
	// the vertices to see if two points aren't connected after all
	float vx1 = gvertices[vert1*3+0];
	float vy1 = gvertices[vert1*3+1];
	float vz1 = gvertices[vert1*3+2];

	float vx2 = gvertices[vert2*3+0];
	float vy2 = gvertices[vert2*3+1];
	float vz2 = gvertices[vert2*3+2];

	i = 0;
	while ( i<gnumSurfaces )
	{
		if ( i!=orgSurface )
		{
			size_t s1 = gsurfaces[i*3+0];
			size_t s2 = gsurfaces[i*3+1];
			size_t s3 = gsurfaces[i*3+2];

			float sx1 = gvertices[s1*3+0];
			float sy1 = gvertices[s1*3+1];
			float sz1 = gvertices[s1*3+2];

			float sx2 = gvertices[s2*3+0];
			float sy2 = gvertices[s2*3+1];
			float sz2 = gvertices[s2*3+2];
		
			float sx3 = gvertices[s3*3+0];
			float sy3 = gvertices[s3*3+1];
			float sz3 = gvertices[s3*3+2];

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


void TLandscape::CreateSurfaceGraph( void )
{
	ResetPercentage();

	for ( size_t i=0; i<gnumSurfaces; i++ )
	{
		DisplayPercentage(i,gnumSurfaces);

		size_t s1 = gsurfaces[i*3+0];
		size_t s2 = gsurfaces[i*3+1];
		size_t s3 = gsurfaces[i*3+2];

		int surf = FindNeighbour(i,s1,s2);
		surfaceGraph[i*4+0] = surf;

		surf = FindNeighbour(i,s2,s3);
		surfaceGraph[i*4+1] = surf;

		surf = FindNeighbour(i,s3,s1);
		surfaceGraph[i*4+2] = surf;
	};

	FinishPercentage();
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


void TLandscape::GetTriangleCenter( size_t surf,
										  float& vx,
										  float& vy,
										  float& vz )
{
	vx = 0.0f;
	vy = 0.0f;
	vz = 0.0f;

	size_t s1 = gsurfaces[surf*3+0];
	vx = vx + gvertices[s1*3+0];
	vy = vy + gvertices[s1*3+1];
	vz = vz + gvertices[s1*3+2];

	s1 = gsurfaces[surf*3+1];
	vx = vx + gvertices[s1*3+0];
	vy = vy + gvertices[s1*3+1];
	vz = vz + gvertices[s1*3+2];

	s1 = gsurfaces[surf*3+2];
	vx = vx + gvertices[s1*3+0];
	vy = vy + gvertices[s1*3+1];
	vz = vz + gvertices[s1*3+2];

	vx = vx * 0.33333f;
	vy = vy * 0.33333f;
	vz = vz * 0.33333f;
};


bool TLandscape::InsideMeshSurface( float x, float &y, float z,
										  TMesh* mesh, size_t surface )
{
	size_t* gsurfaces = mesh->Surfaces();
	float*  gvertices = mesh->Vertices();

	size_t s1 = gsurfaces[surface*3+0];
	size_t s2 = gsurfaces[surface*3+1];
	size_t s3 = gsurfaces[surface*3+2];

	float vx1 = gvertices[s1*3+0];
	float vy1 = gvertices[s1*3+1];
	float vz1 = gvertices[s1*3+2];

	float vx2 = gvertices[s2*3+0];
	float vy2 = gvertices[s2*3+1];
	float vz2 = gvertices[s2*3+2];

	float vx3 = gvertices[s3*3+0];
	float vy3 = gvertices[s3*3+1];
	float vz3 = gvertices[s3*3+2];

	float maxx = vx1;
	float minx = vx1;
	if ( vx2>maxx )
		maxx = vx2;
	if ( vx3>maxx )
		maxx = vx3;
	if ( vx2<minx ) 
		minx = vx2;
	if ( vx3<minx )
		minx = vx3;

	float maxz = vz1;
	float minz = vz1;
	if ( vz2>maxz )
		maxz = vz2;
	if ( vz3>maxz )
		maxz = vz3;
	if ( vz2<minz ) 
		minz = vz2;
	if ( vz3<minz )
		minz = vz3;

	if ( minx<=x && x<=maxx )
	if ( minz<=z && z<=maxz )
	if ( PointInTriangle( x,y,z, vx1,vy1,vz1, vx2,vy2,vz2, vx3,vy3,vz3 ) )
	{
		return true;
	}
	return false;
};


bool TLandscape::InsideSurface( float x, float &y, float z, size_t surface )
{
	size_t s1 = gsurfaces[surface*3+0];
	size_t s2 = gsurfaces[surface*3+1];
	size_t s3 = gsurfaces[surface*3+2];

	float vx1 = gvertices[s1*3+0];
	float vy1 = gvertices[s1*3+1];
	float vz1 = gvertices[s1*3+2];

	float vx2 = gvertices[s2*3+0];
	float vy2 = gvertices[s2*3+1];
	float vz2 = gvertices[s2*3+2];

	float vx3 = gvertices[s3*3+0];
	float vy3 = gvertices[s3*3+1];
	float vz3 = gvertices[s3*3+2];

	float maxx = vx1;
	float minx = vx1;
	if ( vx2>maxx )
		maxx = vx2;
	if ( vx3>maxx )
		maxx = vx3;
	if ( vx2<minx ) 
		minx = vx2;
	if ( vx3<minx )
		minx = vx3;

	float maxz = vz1;
	float minz = vz1;
	if ( vz2>maxz )
		maxz = vz2;
	if ( vz3>maxz )
		maxz = vz3;
	if ( vz2<minz ) 
		minz = vz2;
	if ( vz3<minz )
		minz = vz3;

	if ( minx<=x && x<=maxx )
	if ( minz<=z && z<=maxz )
	if ( PointInTriangle( x,y,z, vx1,vy1,vz1, vx2,vy2,vz2, vx3,vy3,vz3 ) )
	{
		lorReference = surface;
		return true;
	}
	return false;
};


bool TLandscape::GetExhaustiveSearchTriangle( float x, float& y, float z )
{
	lorReference = -1;
	for ( size_t i=0; i<gnumSurfaces; i++ )
	{
		if ( InsideSurface( x,y,z, i ) )
			return true;
	}
	return false;
};


bool TLandscape::GetLORTriangle( float x, float& y, float z,
									   int surface )
{
	recursionCounter++;
	float prevy = y;
	return GetLORTriangle( x,y,z, surface, 0 );
};


bool TLandscape::GetLORTriangle(  float x, float &y, float z,
										int surface, size_t rdepth )
{
	// initial value - do exhaustive search
	if ( surface==-1 )
		return GetExhaustiveSearchTriangle( x,y,z );

	if ( rdepth>10 )
		return false;

	if ( InsideSurface( x,y,z, surface ) )
	{
		return true;
	}
	else
	{
		bool found = false;
		int* lor = &surfaceGraph[surface*4];
		if ( lor[3]!=recursionCounter )
		{
			// heuristically search the closest triangles
			float cx,cy,cz;
			float d1 = 999999999.0f;
			float d2 = 999999999.0f;
			float d3 = 999999999.0f;

			if ( lor[0]!=-1 )
			{
				GetTriangleCenter(lor[0],cx,cy,cz);
				d1 = ( (cx-x)*(cx-x) + (cz-z)*(cz-z) );
			}
			if ( lor[1]!=-1 )
			{
				GetTriangleCenter(lor[1],cx,cy,cz);
				d2 = ( (cx-x)*(cx-x) + (cz-z)*(cz-z) );
			}
			if ( lor[2]!=-1 )
			{
				GetTriangleCenter(lor[2],cx,cy,cz);
				d3 = ( (cx-x)*(cx-x) + (cz-z)*(cz-z) );
			}
			lor[3] = recursionCounter;

			if ( d1 < d2 && d1 < d3 )
			{
				if ( lor[0]!=-1 )
					found = GetLORTriangle(x,y,z,lor[0],rdepth+1);
				if ( d2 < d3 && !found )
				{
					if ( !found && lor[1]!=-1 )
						found = GetLORTriangle(x,y,z,lor[1],rdepth+1);
					if ( !found && lor[2]!=-1 )
						found = GetLORTriangle(x,y,z,lor[2],rdepth+1);
				}
				else if ( !found )
				{
					if ( !found && lor[2]!=-1 )
						found = GetLORTriangle(x,y,z,lor[2],rdepth+1);
					if ( !found && lor[1]!=-1 )
						found = GetLORTriangle(x,y,z,lor[1],rdepth+1);
				}
			}
			else if ( d2 < d1 && d2 < d3 )
			{
				if ( lor[1]!=-1 )
					found = GetLORTriangle(x,y,z,lor[1],rdepth+1);
				if ( d1 < d3 && !found )
				{
					if ( !found && lor[0]!=-1 )
						found = GetLORTriangle(x,y,z,lor[0],rdepth+1);
					if ( !found && lor[2]!=-1 )
						found = GetLORTriangle(x,y,z,lor[2],rdepth+1);
				}
				else if ( !found )
				{
					if ( !found && lor[2]!=-1 )
						found = GetLORTriangle(x,y,z,lor[2],rdepth+1);
					if ( !found && lor[0]!=-1 )
						found = GetLORTriangle(x,y,z,lor[0],rdepth+1);
				}
			}
			else
			{
				if ( lor[2]!=-1 )
					found = GetLORTriangle(x,y,z,lor[2],rdepth+1);
				if ( d1 < d2 && !found )
				{
					if ( !found && lor[0]!=-1 )
						found = GetLORTriangle(x,y,z,lor[0],rdepth+1);
					if ( !found && lor[1]!=-1 )
						found = GetLORTriangle(x,y,z,lor[1],rdepth+1);
				}
				else if ( !found )
				{
					if ( !found && lor[1]!=-1 )
						found = GetLORTriangle(x,y,z,lor[1],rdepth+1);
					if ( !found && lor[0]!=-1 )
						found = GetLORTriangle(x,y,z,lor[0],rdepth+1);
				}
			}
		}
		return found;
	}
};


void TLandscape::DrawTriangle( int surface )
{
	if ( surface==-1 )
		return;

	size_t s1 = gsurfaces[surface*3+0];
	size_t s2 = gsurfaces[surface*3+1];
	size_t s3 = gsurfaces[surface*3+2];

	float vx1 = gvertices[s1*3+0];
	float vy1 = gvertices[s1*3+1];
	float vz1 = gvertices[s1*3+2];

	float vx2 = gvertices[s2*3+0];
	float vy2 = gvertices[s2*3+1];
	float vz2 = gvertices[s2*3+2];

	float vx3 = gvertices[s3*3+0];
	float vy3 = gvertices[s3*3+1];
	float vz3 = gvertices[s3*3+2];

	glColor3f(1,1,0);
	glBegin( GL_LINES );

		glVertex3f( vx1,vy1,vz1 );
		glVertex3f( vx2,vy2,vz2 );

		glVertex3f( vx2,vy2,vz2 );
		glVertex3f( vx3,vy3,vz3 );

		glVertex3f( vx3,vy3,vz3 );
		glVertex3f( vx1,vy1,vz1 );

	glEnd();
};


void TLandscape::AddBridge( TMesh* mesh )
{
	PreCond( mesh!=NULL );
	PreCond( numBridges<kMaxNumBridges );

	float minx,miny,minz,maxx,maxy,maxz;
	mesh->GetBoundaries( minx,miny,minz, maxx,maxy,maxz );

	if ( numBridges==0 )
	{
		bridges = new float[kBridgeSize];
		bridges[0] = minx;
		bridges[1] = maxx;
		bridges[2] = minz;
		bridges[3] = maxz;
		bridgeMeshes[numBridges] = mesh;
		numBridges++;
	}
	else
	{
		float* newbridges = new float[(numBridges+1)*kBridgeSize];
		size_t i;
		for ( i=0; i<numBridges*kBridgeSize; i++ )
			newbridges[i] = bridges[i];

		newbridges[0+(numBridges*kBridgeSize)] = minx;
		newbridges[1+(numBridges*kBridgeSize)] = maxx;
		newbridges[2+(numBridges*kBridgeSize)] = minz;
		newbridges[3+(numBridges*kBridgeSize)] = maxz;
		bridgeMeshes[numBridges] = mesh;
		numBridges++;
		delete bridges;
		bridges = newbridges;
	}
};


bool TLandscape::BridgeHeight( float x, float& y, float z )
{
	for ( size_t i=0; i<numBridges; i++ )
	{
		TMesh* mesh = bridgeMeshes[i];
		PostCond( mesh!=NULL );
		size_t index = i*kBridgeSize;
		if ( x>=bridges[index+0] && x<=bridges[index+1] )
		if ( z>=bridges[index+2] && z<=bridges[index+3] )
		{
			for ( size_t j=0; j<mesh->NumFaces(); j++ )
			{
				if ( InsideMeshSurface( x,y,z, mesh, j ) )
					return true;
			}
		}
	};
	return false;
};


float TLandscape::WaterLevel( void ) const
{
	return waterLevel;
};


void TLandscape::AddLandscapeObject( TLandscapeObject* lnd )
{
	objectList.Append( *lnd );

	float tx,ty,tz;
	lnd->GetTranslation( tx,ty,tz );

	float xpos = (tx - landscapeminX) / divSizeX;
	float zpos = (tz - landscapeminZ) / divSizeY;

	size_t x = size_t(xpos);
	size_t z = size_t(zpos);

	PreCond( lnd->Object()->Objects()!=NULL );
	if ( noGoDivisions[z*divx+x]==NULL )
	{
		noGoDivisions[z*divx+x] = new TLandscapeObjectDraw(*lnd);
	}
	else
	{
		TLandscapeObjectDraw* ngdll = noGoDivisions[z*divx+x];
		while ( ngdll->next!=NULL )
			ngdll = ngdll->next;
		ngdll->next = new TLandscapeObjectDraw(*lnd);
	}
};


void TLandscape::RemoveLandscapeObject( TLandscapeObjectDraw*& list, 
									    TLandscapeObjectDraw* remove )
{
	TLandscapeObjectDraw* ngdll = list;
	TLandscapeObjectDraw* prev = NULL;
	bool found = false;
	while ( ngdll->next!=NULL && !found )
	{
		if ( ngdll==remove )
		{
			found = true;
			if ( prev==NULL )
			{
				list = list->next;
				ngdll->next = NULL;
			}
			else
			{
				prev->next = ngdll->next;
				ngdll->next = NULL;
			}
		}
		else
		{
			prev = ngdll;
			ngdll = ngdll->next;
		}
	}
};


bool TLandscape::InsideObstacle( float xp, float yp, float zp )
{
	float xpos = (xp-landscapeminX) / divSizeX;
	float zpos = (zp-landscapeminZ) / divSizeY;

	size_t x = size_t(xpos);
	size_t z = size_t(zpos);

	if ( noGoDivisions[z*divx+x]!=NULL )
	{
		TLandscapeObjectDraw* ngd = noGoDivisions[z*divx+x];
		while ( ngd!=NULL )
		{
			if ( ngd->InsideObject(xp,yp,zp) )
			{
				TLandscapeObject* obj = ngd->obj;
				obj->CurrentIndex( obj->CurrentIndex()+1 );
				if ( obj->CurrentIndex() >= obj->MaxIndex() )
				{
					RemoveLandscapeObject( noGoDivisions[z*divx+x], ngd );
				}
				return true;
			}
			ngd = ngd->Next();
		};
	}
	return false;
};


TLandscapeObject* TLandscape::GetLandscapeObject( const TString& name ) const
{
	return objectList.GetObject( name );
};

//==========================================================================

TLandscapeObjectDraw::TLandscapeObjectDraw( TLandscapeObject& obj )
	: obj(&obj),
	  next(NULL)
{
};


TLandscapeObjectDraw::TLandscapeObjectDraw( void )
	: obj(NULL),
	  next(NULL)
{
}


TLandscapeObjectDraw::TLandscapeObjectDraw( const TLandscapeObjectDraw& o )
	: obj(NULL),
	  next(NULL)
{
	operator=(o);
};


const TLandscapeObjectDraw& TLandscapeObjectDraw::operator=( const TLandscapeObjectDraw& o )
{
	obj = o.obj;
	next = o.next;
	return *this;
};


TLandscapeObjectDraw::~TLandscapeObjectDraw( void )
{
	obj = NULL;
	delete next;
};


void TLandscapeObjectDraw::Draw( void ) const
{
	PreCond( obj!=NULL );
	obj->Draw();
};


bool TLandscapeObjectDraw::InNoGoZone( float x1, float z1,
									   float x2, float z2,
									   float x3, float z3,
									   float x4, float z4 ) const
{
	if ( obj->Object()->Objects()==NULL )
		return false;

	float x = (x1+x2+x3+x4) * 0.25f;
	float z = (z1+z2+z3+z4) * 0.25f;

	if ( fabs(obj->TX()-x) < obj->SizeX() &&
		 fabs(obj->TZ()-z) < obj->SizeZ() )
	{
		if ( obj->InsideObstacle(x1,z1) ||
			 obj->InsideObstacle(x2,z2) ||
			 obj->InsideObstacle(x3,z3) ||
			 obj->InsideObstacle(x4,z4) )
			return true;
	}
	return false;
};


bool TLandscapeObjectDraw::InsideObject( float x1, float y1, float z1 )
{
	if ( obj->InsideObstacle(x1,y1,z1) )
	{
		return true;
	}
	return false;
};


TLandscapeObjectDraw* TLandscapeObjectDraw::Next( void ) const
{
	return next;
};


void TLandscapeObjectDraw::Next( TLandscapeObjectDraw* _next )
{
	next = _next;
};

//==========================================================================

