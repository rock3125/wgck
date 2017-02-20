#include <precomp_header.h>

#include <object/compoundObject.h>

//==========================================================================

TCompoundObject::TCompoundObject( void )
	: numObjects(0),
	  objects(NULL),
	  objectValid(false)
{
	signature = "PDVCO02";
	owner = false;

	s_angle = 0;
	s_scale = 1;
	s_numSurfaces = 0;
	s_numVertices = 0;
	s_vertices = NULL;
	s_surfaces = NULL;

	isaSwitchDetailObject = false;
	distanceFromCamera = 0;
	switchDistance = 0;
};


TCompoundObject::~TCompoundObject( void )
{
	ClearObject();
};


TCompoundObject::TCompoundObject( const TCompoundObject& o )
	: numObjects(0),
	  objects(NULL),
	  objectValid(false)
{
	s_angle = 0;
	s_scale = 1;
	s_numSurfaces = 0;
	s_numVertices = 0;
	s_vertices = NULL;
	s_surfaces = NULL;

	operator=( o );
};


const TCompoundObject& TCompoundObject::Object( void ) const
{
	return *this;
};


TCompoundObject& TCompoundObject::Object( void )
{
	return *this;
};


const TCompoundObject& TCompoundObject::operator=( const TCompoundObject& o )
{
	ClearObject();

	s_angle = o.s_angle;
	s_scale = o.s_scale;
	s_numSurfaces = o.s_numSurfaces;
	s_numVertices = o.s_numVertices;

	if ( s_numSurfaces > 0 )
	{
		size_t size = 2*sizeof(float)*s_numVertices;
		s_vertices = new float[size];
		for ( size_t i=0; i<s_numVertices*2; i++ )
		{
			s_vertices[i] = o.s_vertices[i];
		}
	}

	if ( s_numVertices > 0 )
	{
		size_t size = 3*sizeof(size_t)*s_numSurfaces;
		s_surfaces = new size_t[size];
		for ( size_t i=0; i<s_numSurfaces*3; i++ )
		{
			s_surfaces[i] = o.s_surfaces[i];
		}
	}

	numObjects = o.numObjects;
	objectValid = o.objectValid;
	owner = false;

	isaSwitchDetailObject = o.isaSwitchDetailObject;
	distanceFromCamera = o.distanceFromCamera;
	switchDistance = o.switchDistance;

	if ( numObjects>0 )
	{
		objects = new TBinObject*[numObjects];
		PostCond( objects!=NULL );
		for ( size_t j=0; j<numObjects; j++ )
			objects[j] = o.objects[j];
	}

	return *this;
};


void TCompoundObject::ClearObject( void )
{
	objectValid = false;

	if ( owner )
	for ( size_t i=0; i<numObjects; i++ )
	{
		if ( objects[i]!=NULL )
			delete objects[i];
		objects[i] = NULL;
	}
	if ( objects!=NULL )
		delete []objects;
	objects = NULL;
	numObjects = 0;

	ClearShadow();
};

void TCompoundObject::ClearShadow( void )
{
	s_angle = 0;
	s_scale = 1;
	s_numSurfaces = 0;
	s_numVertices = 0;

	if ( s_vertices!=NULL )
	{
		delete[] s_vertices;
		s_vertices = NULL;
	}

	if ( s_surfaces!=NULL )
	{
		delete[] s_surfaces;
		s_surfaces = NULL;
	}
};

bool TCompoundObject::LoadBinary( const TString& fname,
								  TString& errStr,
								  const TString& path,
								  const TString& texturePath )
{
	TPersist file(fileRead);

	ClearObject();

	TString name;
	ConstructPath( name, path, fname );
	if ( !file.FileOpen( name ) )
	{
		errStr = "\nError opening file " + name;
		errStr = errStr + "for read\n";
		return false;
	}

	// check file signature
	TString temp;
	size_t index = 0;
	char ch;
	do
	{
		file.FileRead(&ch,1);
		temp = temp + TString(ch);
		index++;
	}
	while ( index<255 && ch!=0 );
	if ( signature!=temp )
	{
		file.FileClose();
		errStr = "\nFile " + name;
		errStr = errStr +" is either of incorrect version or not a CompoundObject\n";
		return false;
	};

	temp = "";
	index = 0;
	do
	{
		file.FileRead(&ch,1);
		temp = temp + TString(ch);
		index++;
	}
	while ( index<255 && ch!=0 );

	if ( index==255 )
	{
		file.FileClose();
		errStr = "\nFile " + name;
		errStr = errStr + "is corrupt - texturePath not found\n";
		return false;
	};

	file.FileRead( &numObjects, sizeof(size_t) );

	owner = true;
	objects = new TBinObject*[numObjects];
	PostCond( objects!=NULL );
	size_t j;
	for ( j=0; j<numObjects; j++ )
		objects[j] = NULL;

	for ( j=0; j<numObjects; j++ )
	{
		objects[j] = new TBinObject();
		if ( !objects[j]->LoadBinary( file, errStr, path, texturePath ) )
		{
			file.FileClose();
			return false;
		}
	}
	file.FileClose();

	objectValid = true;

	// try and load a shadow file if possible
	TString shadowName = fname.GetItem( '.', 0 ) + ".shd";
	ConstructPath( name, path, shadowName );
	LoadShadow( name, errStr );
//	{
//		TString shadowName2 = shadowName + "_";
//		WriteShadow( shadowName2, 180, 1, s_numVertices, s_vertices, 
//					 s_numSurfaces, s_surfaces, errStr );
//	};

	return true;
};


bool TCompoundObject::SaveBinary( const TString& name, const TString& texturePath ) const
{
	TPersist file(fileWrite);

	if ( !objectValid )
	{
		WriteString( "\nError: Trying to save invalid object as %s\n", name );
		return false;
	};

	if ( !file.FileOpen( name ) )
	{
		WriteString( "\nError opening file %s for writing\n", name );
		return false;
	}

	file.FileWrite( signature.c_str(), signature.length()+1 );
	file.FileWrite( texturePath.c_str(), texturePath.length()+1 );
	file.FileWrite( &numObjects, sizeof(size_t) );

	size_t j;
	for ( j=0; j<numObjects; j++ )
	{
		objects[j]->SaveBinary(file);
	}
	file.FileClose();
	return true;
};


bool TCompoundObject::Create( const TString& name, 
							  size_t cntr,
							  const TString& texturePath, 
							  const TString* names )
{
	TPersist file(fileWrite);

	numObjects = cntr;
	if ( !file.FileOpen( name ) )
	{
		WriteString( "\nError opening file %s for writing\n", name );
		return false;
	}

	ClearObject();

	file.FileWrite( signature.c_str(), signature.length()+1 );
	file.FileWrite( texturePath.c_str(), texturePath.length()+1 );

	owner = true;
	objects = new TBinObject*[cntr];
	PostCond( objects!=NULL );
	size_t j;
	for ( j=0; j<cntr; j++ )
	{
		objects[j] = NULL;
	};

	bool flipem = false;
	for ( j=0; j<cntr; j++ )
	{
		if ( names[j]=="-flipuv" )
		{
			WriteString( "\nFlipping UVs\n" );
			flipem = true;
		}
	}

	if ( flipem )
	{
		size_t count = cntr-1;
		file.FileWrite( &count, sizeof(size_t) );
	}
	else
	{
		file.FileWrite( &cntr, sizeof(size_t) );
	}

	size_t idx = 0;
	for ( j=0; j<cntr; j++ )
	{
		// strip path
		TString fname;
		TString path;
		fname = names[j];
		path = "";

		if ( fname[0]!='-' )
		{
			size_t index = fname.length()-1;
			while ( index>0 && fname[index]!='\\' ) index--;
			if ( index>0 )
			{
				path = fname.substr(0,index);
				fname = fname.substr(index+1);
			}

			TString errStr;
			objects[idx] = new TBinObject();
			if (!objects[idx]->LoadBinary( fname, errStr, path, texturePath ))
			{
				WriteString( "\nError: %s\n", errStr );
				file.FileClose();
				return false;
			}

			if ( flipem )
				objects[idx]->FlipUV();

			objects[idx]->SaveBinary(file);
			idx++;
		}
	}
	file.FileClose();

	objectValid = true;
	return true;
};


const size_t& TCompoundObject::NumObjects( void ) const
{
	return numObjects;
};


void TCompoundObject::PauseAnimations( bool p )
{
	if ( !objectValid )
		return;
	PreCond( objects!=NULL );
	for ( size_t i=0; i<numObjects; i++ )
	{
		PreCond( objects[i]!=NULL );
		objects[i]->PauseAnimations(p);
	}
};



bool TCompoundObject::GetLORTriangle( size_t currentObject,
									  float xp, float yp, float zp, 
									  float size, float& y )
{
	PreCond( objects[currentObject]!=NULL );
	return objects[currentObject]->GetLORTriangle( xp,yp,zp, size, y );
};


bool TCompoundObject::GetLORTriangle( size_t currentObject,
									  const float* matrix,
									  float xp, float yp, float zp, 
									  float size, float& y )
{
	PreCond( objects[currentObject]!=NULL );
	return objects[currentObject]->GetLORTriangle( matrix, xp,yp,zp, size, y );
};


float TCompoundObject::SizeX( size_t currentObject ) const
{
	PreCond( objectValid );
	PreCond( objects!=NULL );
	PreCond( objects[currentObject]!=NULL );
	return objects[currentObject]->SizeX();
};


float TCompoundObject::SizeY( size_t currentObject ) const
{
	PreCond( objectValid );
	PreCond( objects!=NULL );
	PreCond( objects[currentObject]!=NULL );
	return objects[currentObject]->SizeY();
};


float TCompoundObject::SizeZ( size_t currentObject ) const
{
	PreCond( objectValid );
	PreCond( objects!=NULL );
	PreCond( objects[currentObject]!=NULL );
	return objects[currentObject]->SizeZ();
};


size_t TCompoundObject::NumMeshes( size_t currentObject ) const
{
	PreCond( objectValid );
	PreCond( objects!=NULL );
	PreCond( objects[currentObject]!=NULL );
	return objects[currentObject]->NumMeshes();
};


TMesh* TCompoundObject::GetMesh( size_t currentObject, size_t index ) const
{
	PreCond( objectValid );
	PreCond( objects!=NULL );
	PreCond( objects[currentObject]!=NULL );
	return objects[currentObject]->GetMesh(index);
};


float TCompoundObject::CenterX( size_t currentObject ) const
{
	PreCond( objectValid );
	PreCond( objects!=NULL );
	PreCond( objects[currentObject]!=NULL );
	return objects[currentObject]->CenterX();
};


float TCompoundObject::CenterY( size_t currentObject ) const
{
	PreCond( objectValid );
	PreCond( objects!=NULL );
	PreCond( objects[currentObject]!=NULL );
	return objects[currentObject]->CenterY();
};


float TCompoundObject::MinX( size_t currentObject ) const
{
	PreCond( objects!=NULL );
	PreCond( objects[currentObject]!=NULL );
	return objects[currentObject]->MinX();
}


float TCompoundObject::MinY( size_t currentObject ) const
{
	PreCond( objects!=NULL );
	PreCond( objects[currentObject]!=NULL );
	return objects[currentObject]->MinY();
}


float TCompoundObject::MinZ( size_t currentObject ) const
{
	PreCond( objects!=NULL );
	PreCond( objects[currentObject]!=NULL );
	return objects[currentObject]->MinZ();
}


float TCompoundObject::MaxX( size_t currentObject ) const
{
	PreCond( objects!=NULL );
	PreCond( objects[currentObject]!=NULL );
	return objects[currentObject]->MaxX();
}


float TCompoundObject::MaxY( size_t currentObject ) const
{
	PreCond( objects!=NULL );
	PreCond( objects[currentObject]!=NULL );
	return objects[currentObject]->MaxY();
}


float TCompoundObject::MaxZ( size_t currentObject ) const
{
	PreCond( objects!=NULL );
	PreCond( objects[currentObject]!=NULL );
	return objects[currentObject]->MaxZ();
}


float TCompoundObject::CenterZ( size_t currentObject ) const
{
	PreCond( objectValid );
	PreCond( objects!=NULL );
	PreCond( objects[currentObject]!=NULL );
	return objects[currentObject]->CenterZ();
};


void TCompoundObject::Draw( size_t currentObject ) const
{
	PreCond( objectValid );
	PreCond( objects!=NULL );
	PreCond( objects[currentObject]!=NULL );

	size_t objIndex = currentObject;
	if ( isaSwitchDetailObject && numObjects==2 )
	{
		objIndex = 0;
		if ( distanceFromCamera > switchDistance )
		{
			objIndex = 1;
		}
	}
	objects[objIndex]->Draw();
};


void TCompoundObject::Draw( size_t currentObject, bool drawTransp ) const
{
	PreCond( objectValid );
	PreCond( objects!=NULL );
	PreCond( objects[currentObject]!=NULL );

	size_t objIndex = currentObject;
	if ( isaSwitchDetailObject && numObjects==2 )
	{
		objIndex = 0;
		if ( distanceFromCamera > switchDistance )
		{
			objIndex = 1;
		}
	}
	objects[objIndex]->Draw(drawTransp);
};


bool TCompoundObject::ValidObject( void ) const
{
	return objectValid;
};


TBinObject** TCompoundObject::Objects( void ) const
{
	return objects;
};


void TCompoundObject::GetBounds( size_t currentObject,
								 float& minx, float& miny, float& minz,
								 float& maxx, float& maxy, float& maxz )
{
	PreCond( objects!=NULL );
	PreCond( objects[currentObject]!=NULL );
	minx = objects[currentObject]->MinX();
	miny = objects[currentObject]->MinY();
	minz = objects[currentObject]->MinZ();
	maxx = objects[currentObject]->MaxX();
	maxy = objects[currentObject]->MaxY();
	maxz = objects[currentObject]->MaxZ();
};


void TCompoundObject::GetCenterSize( size_t currentObject,
									 float& cx, float& cy, float& cz,
									 float& sx, float& sy, float& sz )
{
	PreCond( objects!=NULL );
	PreCond( objects[currentObject]!=NULL );
	cx = objects[currentObject]->CenterX();
	cy = objects[currentObject]->CenterY();
	cz = objects[currentObject]->CenterZ();
	sx = objects[currentObject]->SizeX();
	sy = objects[currentObject]->SizeY();
	sz = objects[currentObject]->SizeZ();
};


bool TCompoundObject::Collision( size_t currentObject,
								 float l1x, float l1y, float l1z,
								 float l2x, float l2y, float l2z ) const
{
	PreCond( objects[currentObject]!=NULL );
	return objects[currentObject]->Collision( l1x, l1y, l1z, l2x, l2y, l2z );
}


bool TCompoundObject::Collision( size_t currentObject,
								 const float* matrix,
								 float l1x, float l1y, float l1z,
								 float l2x, float l2y, float l2z ) const
{
	PreCond( objects[currentObject]!=NULL );
	return objects[currentObject]->Collision( matrix, l1x, l1y, l1z, l2x, l2y, l2z );
}


bool TCompoundObject::Collision( size_t currentObject,
								 TMatrix& matrix,
								 float l1x, float l1y, float l1z,
								 float l2x, float l2y, float l2z )
{
	PreCond( objects[currentObject]!=NULL );
	return objects[currentObject]->Collision( matrix, l1x, l1y, l1z, l2x, l2y, l2z );
}


bool TCompoundObject::Collision( size_t currentObject,
								 const float* matrix,
								 float l1x, float l1z,
								 float l2x, float l2z ) const
{
	PreCond( objects[currentObject]!=NULL );
	return objects[currentObject]->Collision( matrix, l1x, l1z, l2x, l2z );
}


void TCompoundObject::CenterAxes( bool cx, bool cy, bool cz )
{
	PreCond( objectValid );
	PreCond( objects!=NULL );
	for ( size_t i=0; i<numObjects; i++ )
	{
		PreCond( objects[i]!=NULL );
		objects[i]->CenterAxes(cx,cy,cz);
	}
};

void TCompoundObject::CalculateBoundaries( size_t currentObject )
{
	PreCond( objects[currentObject]!=NULL );
	objects[currentObject]->CalculateBoundaries();
};

void TCompoundObject::Normalise( void )
{
	PreCond( objects!=NULL );

	size_t i;
	float minX,minY,minZ, maxX,maxY,maxZ;
	objects[0]->GetBounds( minX, minY, minZ, maxX, maxY, maxZ );
	for ( i=0; i<numObjects; i++ )
	{
		objects[i]->Resize( minX, minY, minZ, maxX, maxY,maxZ );
	}

	// make a common center and recenter all objects
	objects[0]->GetBounds( minX, minY, minZ, maxX, maxY, maxZ );
	float cx = (minX + maxX) * 0.5f;
	float cy = (minY + maxY) * 0.5f;
	float cz = (minZ + maxZ) * 0.5f;
	for ( i=0; i<numObjects; i++ )
	{
		objects[i]->Center( cx,cy,cz );
	}
};

bool TCompoundObject::Reload( TString& errStr )
{
	for ( size_t i=0; i<numObjects; i++ )
	{
		PreCond( objects[i]!=NULL );
		if ( !objects[i]->Reload(errStr) )
		{
			return false;
		}
	}
	return true;
};

float TCompoundObject::DistanceFromCamera( void ) const
{
	return distanceFromCamera;
};

void TCompoundObject::DistanceFromCamera( float _distanceFromCamera )
{
	distanceFromCamera = _distanceFromCamera;
};

float TCompoundObject::SwitchDistance( void ) const
{
	return switchDistance;
};

void TCompoundObject::SwitchDistance( float _switchDistance )
{
	switchDistance = _switchDistance;
};

bool TCompoundObject::IsaSwitchDetailObject( void ) const
{
	return isaSwitchDetailObject;
};

void TCompoundObject::IsaSwitchDetailObject( bool _isaSwitchDetailObject )
{
	isaSwitchDetailObject = _isaSwitchDetailObject;
};

bool TCompoundObject::WriteShadow( const TString& fname,
								   float angle, float scale,
								   size_t numVertices, float* vertices,
								   size_t numSurfaces, size_t* surfaces,
								   TString& errStr )
{
	TPersist file(fileWrite);
	if ( !file.FileOpen( fname ) )
	{
		errStr = file.ErrorString();
		return false;
	}
	else
	{
		TString sig = "PDV01SHD";
		file.FileWrite( sig.c_str(), sig.length()+1 );
		file.FileWrite( &angle, sizeof(float) );
		file.FileWrite( &scale, sizeof(float) );
		file.FileWrite( &numVertices, sizeof(size_t) );
		file.FileWrite( vertices, sizeof(float) * numVertices * 2 );
		file.FileWrite( &numSurfaces, sizeof(size_t) );
		file.FileWrite( surfaces, sizeof(size_t) * numSurfaces * 3 );

		file.FileClose();
	}
	return true;
};

bool TCompoundObject::LoadShadow( const TString& fname,
								  TString& errStr )
{
	ClearShadow();

	TPersist file(fileRead);
	if ( !file.FileOpen( fname ) )
	{
		errStr = file.ErrorString();
		return false;
	}
	else
	{
		TString sig = "PDV01SHD";
		TString signature;

		file.ReadString( signature );

		if ( signature!=sig )
		{
			errStr = "Shadow file \"" + fname + "\" incorrect version (";
			errStr = errStr + signature + ")";
			file.FileClose();
			return false;
		}

		file.FileRead( &s_angle, sizeof(float) );
		file.FileRead( &s_scale, sizeof(float) );

		file.FileRead( &s_numVertices, sizeof(size_t) );
		s_vertices = new float[s_numVertices*2];
		file.FileRead( s_vertices, sizeof(float) * s_numVertices * 2 );

		file.FileRead( &s_numSurfaces, sizeof(size_t) );
		s_surfaces = new size_t[s_numSurfaces*3];
		file.FileRead( s_surfaces, sizeof(size_t) * s_numSurfaces * 3 );

		file.FileClose();
	}
	return true;
};

float TCompoundObject::ShadowAngle( void ) const
{
	return s_angle;
};

float TCompoundObject::ShadowScale( void ) const
{
	return s_scale;
};

size_t TCompoundObject::NumShadowFaces( void ) const
{
	return s_numSurfaces;
};

size_t TCompoundObject::NumShadowVertices( void ) const
{
	return s_numVertices;
};

size_t* TCompoundObject::ShadowFaces( void )
{
	return s_surfaces;
};

float* TCompoundObject::ShadowVertices( void )
{
	return s_vertices;
};

//==========================================================================


