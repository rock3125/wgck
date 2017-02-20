#include <precomp_header.h>

#include <object/compoundObject.h>

//==========================================================================

TCompoundObject::TCompoundObject( void )
	: numObjects(0),
	  objects(NULL),
	  objectValid(false)
{
	signature = "PDVCO01";
};


TCompoundObject::~TCompoundObject( void )
{
	ClearObject();
};


TCompoundObject::TCompoundObject( const TCompoundObject& o )
{
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

	numObjects = o.numObjects;
	objectValid = o.objectValid;

	if ( numObjects>0 )
	{
		objects = new TObject[numObjects];
		PostCond( objects!=NULL );
		for ( size_t j=0; j<numObjects; j++ )
			objects[j] = o.objects[j];
	}

	return *this;
};


void TCompoundObject::ClearObject( void )
{
	objectValid = false;
//	delete objects;
	objects = NULL;
	numObjects = 0;
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

	objects = new TObject[numObjects];
	PostCond( objects!=NULL );

	size_t j;
	for ( j=0; j<numObjects; j++ )
	{
		if ( !objects[j].LoadBinary( file, errStr, path, texturePath ) )
		{
			file.FileClose();
			return false;
		}
	}
	file.FileClose();

	objectValid = true;

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
		objects[j].SaveBinary(file);
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
	file.FileWrite( &cntr, sizeof(size_t) );

	objects = new TObject[cntr];
	PostCond( objects!=NULL );

	size_t j;
	for ( j=0; j<cntr; j++ )
	{
		// strip path
		TString fname;
		TString path;
		fname = names[j];
		path = "";
		size_t index = fname.length()-1;
		while ( index>0 && fname[index]!='\\' ) index--;
		if ( index>0 )
		{
			path = fname.substr(0,index);
			fname = fname.substr(index+1);
		}

		TString errStr;
		if (!objects[j].LoadBinary( fname, errStr, path, texturePath ))
		{
			WriteString( "\nError: %s\n", errStr );
			file.FileClose();
			return false;
		}
		objects[j].SaveBinary(file);
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
	PreCond( objectValid );
	PreCond( objects!=NULL );
	for ( size_t i=0; i<numObjects; i++ )
		objects[i].PauseAnimations(p);
};


float TCompoundObject::SizeX( size_t currentObject ) const
{
	PreCond( objectValid );
	PreCond( objects!=NULL );
	return objects[currentObject].SizeX();
};


float TCompoundObject::SizeY( size_t currentObject ) const
{
	PreCond( objectValid );
	PreCond( objects!=NULL );
	return objects[currentObject].SizeY();
};


float TCompoundObject::SizeZ( size_t currentObject ) const
{
	PreCond( objectValid );
	PreCond( objects!=NULL );
	return objects[currentObject].SizeZ();
};

/*
TObject& TCompoundObject::operator[] ( size_t index )
{
	PreCond( objectValid );
	PreCond( objects!=NULL );
	PreCond( index<numObjects );
	return objects[index];
};


TObject& TCompoundObject::operator () ( void )
{
	PreCond( objectValid );
	PreCond( objects!=NULL );
	PreCond( currentObject<numObjects );
	return objects[currentObject];
};
*/

float TCompoundObject::X( size_t currentObject ) const
{
	PreCond( objectValid );
	PreCond( objects!=NULL );
	return objects[currentObject].X();
};



float TCompoundObject::Y( size_t currentObject ) const
{
	PreCond( objectValid );
	PreCond( objects!=NULL );
	return objects[currentObject].Y();
};



float TCompoundObject::Z( size_t currentObject ) const
{
	PreCond( objectValid );
	PreCond( objects!=NULL );
	return objects[currentObject].Z();
};



void TCompoundObject::X( float _x )
{
	PreCond( objectValid );
	PreCond( objects!=NULL );
	for ( size_t i=0; i<numObjects; i++ )
		objects[i].X( _x );
};



void TCompoundObject::Y( float _y )
{
	PreCond( objectValid );
	PreCond( objects!=NULL );
	for ( size_t i=0; i<numObjects; i++ )
		objects[i].Y( _y );
};



void TCompoundObject::Z( float _z )
{
	PreCond( objectValid );
	PreCond( objects!=NULL );
	for ( size_t i=0; i<numObjects; i++ )
		objects[i].Z( _z );
};



float TCompoundObject::Xangle( size_t currentObject ) const
{
	PreCond( objectValid );
	PreCond( objects!=NULL );
	return objects[currentObject].Xangle();
};



float TCompoundObject::Yangle( size_t currentObject ) const
{
	PreCond( objectValid );
	PreCond( objects!=NULL );
	return objects[currentObject].Yangle();
};



float TCompoundObject::Zangle( size_t currentObject ) const
{
	PreCond( objectValid );
	PreCond( objects!=NULL );
	return objects[currentObject].Zangle();
};



void TCompoundObject::Xangle( float _xangle )
{
	PreCond( objectValid );
	PreCond( objects!=NULL );
	for ( size_t i=0; i<numObjects; i++ )
		objects[i].Xangle( _xangle );
};



void TCompoundObject::Yangle( float _yangle )
{
	PreCond( objectValid );
	PreCond( objects!=NULL );
	for ( size_t i=0; i<numObjects; i++ )
		objects[i].Yangle( _yangle );
};



void TCompoundObject::Zangle( float _zangle )
{
	PreCond( objectValid );
	PreCond( objects!=NULL );
	for ( size_t i=0; i<numObjects; i++ )
		objects[i].Zangle( _zangle );
};


size_t TCompoundObject::NumMeshes( size_t currentObject ) const
{
	PreCond( objectValid );
	PreCond( objects!=NULL );
	return objects[currentObject].NumMeshes();
};


TMesh* TCompoundObject::GetMesh( size_t currentObject, size_t index ) const
{
	PreCond( objectValid );
	PreCond( objects!=NULL );
	return objects[currentObject].GetMesh(index);
};


float TCompoundObject::CenterX( size_t currentObject ) const
{
	PreCond( objectValid );
	PreCond( objects!=NULL );
	return objects[currentObject].CenterX();
};


float TCompoundObject::CenterY( size_t currentObject ) const
{
	PreCond( objectValid );
	PreCond( objects!=NULL );
	return objects[currentObject].CenterY();
};


float TCompoundObject::MinX( size_t currentObject ) const
{
	PreCond( objects!=NULL );
	return objects[currentObject].MinX();
}


float TCompoundObject::MinY( size_t currentObject ) const
{
	PreCond( objects!=NULL );
	return objects[currentObject].MinY();
}


float TCompoundObject::MinZ( size_t currentObject ) const
{
	PreCond( objects!=NULL );
	return objects[currentObject].MinZ();
}


float TCompoundObject::MaxX( size_t currentObject ) const
{
	PreCond( objects!=NULL );
	return objects[currentObject].MaxX();
}


float TCompoundObject::MaxY( size_t currentObject ) const
{
	PreCond( objects!=NULL );
	return objects[currentObject].MaxY();
}


float TCompoundObject::MaxZ( size_t currentObject ) const
{
	PreCond( objects!=NULL );
	return objects[currentObject].MaxZ();
}


float TCompoundObject::CenterZ( size_t currentObject ) const
{
	PreCond( objectValid );
	PreCond( objects!=NULL );
	return objects[currentObject].CenterZ();
};


void TCompoundObject::Draw( size_t currentObject ) const
{
	PreCond( objectValid );
	PreCond( objects!=NULL );
	objects[currentObject].Draw();
};


bool TCompoundObject::ValidObject( void ) const
{
	return objectValid;
};


TObject* TCompoundObject::Objects( void ) const
{
	return objects;
};

//==========================================================================


