#include <precomp_header.h>

#include <Commdlg.h>

#include <object/deffile.h>

//==========================================================================

const size_t defFileVersion = 2;

//==========================================================================

TConnections::TConnections( void )
{
	x = y = z = 0;
	numConnections = 0;
	recursionCounter = 0;
	visited = false;
	for ( size_t i=0; i<4; i++ )
	{
		neighbours[i] = 0;
	}
};


const TConnections& TConnections::operator=( const TConnections& c )
{
	x = c.x;
	y = c.y;
	z = c.z;
	visited = c.visited;
	numConnections = c.numConnections;
	recursionCounter = c.recursionCounter;
	neighbours[0] = c.neighbours[0];
	neighbours[1] = c.neighbours[1];
	neighbours[2] = c.neighbours[2];
	neighbours[3] = c.neighbours[3];

	return *this;
};


TConnections::TConnections( const TConnections& c )
{
	x = y = z = 0;
	numConnections = 0;
	recursionCounter = 0;
	visited = false;
	for ( size_t i=0; i<4; i++ )
	{
		neighbours[i] = 0;
	}

	operator=(c);
};


TConnections::~TConnections( void )
{
};

void TConnections::Save( TPersist& file )
{
	file.FileWrite( &x, sizeof(float) );
	file.FileWrite( &y, sizeof(float) );
	file.FileWrite( &z, sizeof(float) );

	file.FileWrite( &numConnections, sizeof(size_t) );
	for ( size_t i=0; i<numConnections; i++ )
	{
		file.FileWrite( &neighbours[i], sizeof(size_t) );
	}
};

void TConnections::Load( TPersist& file )
{
	file.FileRead( &x, sizeof(float) );
	file.FileRead( &y, sizeof(float) );
	file.FileRead( &z, sizeof(float) );

	file.FileRead( &numConnections, sizeof(size_t) );
	for ( size_t i=0; i<numConnections; i++ )
	{
		file.FileRead( &neighbours[i], sizeof(size_t) );
	}
};

//==========================================================================

TDefFile::TDefFile( void )
	: landscapeObjects(NULL),
	  aiLocations(NULL),
	  patrolLocations(NULL),
	  sequenceLocations(NULL)
{
	signature = "PDVILSE06";

	timeOfDay = 0;
	soundTrack = 0;

	numLandscapeObjects = 0;
	numLocations = 0;
	numPatrolLocations = 0;
	numSequenceLocations = 0;
	water = 10;

	buffer1Size = 1;
	buffer2Size = 1;
	buffer3Size = 1;
	buffer4Size = 1;

	landscapeObjects = new TLandscapeObject[buffer1Size];
	aiLocations = new TConnections[buffer2Size];
	patrolLocations = new TConnections[buffer3Size];
	sequenceLocations = new TConnections[buffer4Size];
};


TDefFile::TDefFile( const TDefFile& d )
	: landscapeObjects(NULL),
	  aiLocations(NULL),
	  patrolLocations(NULL),
	  sequenceLocations(NULL)
{
	signature = "PDVILSE06";

	buffer1Size = 1;
	buffer2Size = 1;
	buffer3Size = 1;
	buffer4Size = 1;

	numLandscapeObjects = 0;
	numPatrolLocations = 0;
	numLocations = 0;
	numSequenceLocations = 0;
	water = 10;

	operator=(d);
};


TDefFile::~TDefFile( void )
{
	numLandscapeObjects = 0;
	numLocations = 0;
	numPatrolLocations = 0;
	numSequenceLocations = 0;

	if ( landscapeObjects!=NULL )
		delete []landscapeObjects;
	landscapeObjects = NULL;

	if ( aiLocations!=NULL )
		delete []aiLocations;
	aiLocations = NULL;

	if ( patrolLocations!=NULL )
		delete []patrolLocations;
	patrolLocations = NULL;

	if ( sequenceLocations!=NULL )
		delete []sequenceLocations;
	sequenceLocations = NULL;
};


const TDefFile& TDefFile::operator=( const TDefFile& d )
{
	size_t i;

	islandName = d.islandName;
	binName = d.binName;
	strip = d.strip;
	creator = d.creator;
	material = d.material;
	timeOfDay = d.timeOfDay;
	soundTrack = d.soundTrack;
	water = d.water;

	buffer1Size = d.buffer1Size;
	buffer2Size = d.buffer2Size;
	buffer3Size = d.buffer3Size;
	buffer4Size = d.buffer4Size;

	if ( landscapeObjects!=NULL )
		delete []landscapeObjects;
	landscapeObjects = new TLandscapeObject[buffer1Size];
	numLandscapeObjects = d.numLandscapeObjects;
	for ( i=0; i<numLandscapeObjects; i++ )
		landscapeObjects[i] = d.landscapeObjects[i];

	numLocations = d.numLocations;
	if ( aiLocations!=NULL )
		delete []aiLocations;
	aiLocations = new TConnections[buffer2Size];
	for ( i=0; i<numLocations; i++ )
	{
		aiLocations[i] = d.aiLocations[i];
	}

	numPatrolLocations = d.numPatrolLocations;
	if ( patrolLocations!=NULL )
		delete []patrolLocations;
	patrolLocations = new TConnections[buffer3Size];
	for ( i=0; i<numPatrolLocations; i++ )
	{
		patrolLocations[i] = d.patrolLocations[i];
	}

	numSequenceLocations = d.numSequenceLocations;
	if ( sequenceLocations!=NULL )
		delete []sequenceLocations;
	sequenceLocations = new TConnections[buffer4Size];
	for ( i=0; i<numSequenceLocations; i++ )
	{
		sequenceLocations[i] = d.sequenceLocations[i];
	}

	return *this;
};


bool TDefFile::LoadBinaryAs( HWND parent, TString& fname )
{
	OPENFILENAME ofn;
	memset( &ofn,0,sizeof(OPENFILENAME) );
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = parent;
	ofn.lpstrFilter = "Map defn file\0*.def\0All files\0*.*\0\0";
	char temp[256];
	temp[0] = 0;
	ofn.lpstrFile = temp;
	ofn.lpstrTitle = "Load map definition file";
	ofn.nMaxFileTitle = 256;
	ofn.nMaxFile = 256;
	ofn.Flags = OFN_EXPLORER;
	if ( GetOpenFileName( &ofn )==TRUE )
	{
		fname = ofn.lpstrFile;
		return true;
	};
	return false;
};


bool TDefFile::LoadBinary( const TString& fname, const TString& path, TString& errStr,
						   bool calculateBackwards )
{
	WriteString( "Loading .DEF file\n" );

	TPersist file(fileRead);
	TString fileName;
	ConstructPath( fileName, path, fname );

	if ( !file.FileOpen( fileName ) )
	{
		errStr = "Error: could not open \"" + fileName + "\"";
		return false;
	}

	if ( !LoadBinary( file, errStr, calculateBackwards ) )
		return false;

	file.FileClose();
	return true;
};


bool TDefFile::LoadBinary( TPersist& file, TString& errStr, bool calculateBackwards )
{
	size_t i;

	size_t version = 0;
	if ( defFileVersion>0 )
	{
		file.FileRead( &version, sizeof(size_t) );
	}

	// check signature and get island name
	char ch;
	TString sig;
	bool eof;
	do
	{
		eof = !file.FileRead( &ch,1 );
		if ( ch>0 )
			sig = sig + TString(ch);
	}
	while ( ch!=0 && !eof );

	bool oldfile = false;
	if ( sig!=signature )
	{
		errStr = "Error: not a valid island file or has the wrong version";
		return false;
	}

	islandName = "";
	do
	{
		file.FileRead( &ch,1 );
		if ( ch>0 )
			islandName = islandName + TString(ch);
	}
	while ( ch!=0 );

	binName = "";
	do
	{
		file.FileRead( &ch,1 );
		if ( ch>0 )
			binName = binName + TString(ch);
	}
	while ( ch!=0 );
//	binName = "story02.bmp";

	creator = "";
	do
	{
		file.FileRead( &ch,1 );
		if ( ch>0 )
			creator = creator + TString(ch);
	}
	while ( ch!=0 );

	strip = "";
	do
	{
		file.FileRead( &ch,1 );
		if ( ch>0 )
			strip = strip + TString(ch);
	}
	while ( ch!=0 );

	if ( !oldfile )
	{
		material = "";
		do
		{
			file.FileRead( &ch,1 );
			if ( ch>0 )
				material = material + TString(ch);
		}
		while ( ch!=0 );
	}

	file.FileRead( &timeOfDay, sizeof(size_t) );
	if ( version>0 )
	{
		file.FileRead( &soundTrack, sizeof(size_t) );
	}

	float temp;
	file.FileRead( &temp, sizeof(float) );
	water = size_t(temp);

	file.FileRead( &numLandscapeObjects, sizeof(size_t) );
	if ( landscapeObjects!=NULL )
		delete []landscapeObjects;
	landscapeObjects = new TLandscapeObject[numLandscapeObjects];
	buffer1Size = numLandscapeObjects;
	for ( i=0; i<numLandscapeObjects; i++ )
	{
		if ( !landscapeObjects[i].LoadBinary( file, errStr ) )
		{
			numLandscapeObjects = 0;
			file.FileClose();
			return false;
		}
	}

	file.FileRead(&numLocations,sizeof(size_t));
	if ( aiLocations!=NULL )
		delete []aiLocations;
	if ( numLocations>0 )
	{
		aiLocations = new TConnections[numLocations];
		buffer2Size = numLocations;
	}
	else
	{
		aiLocations = new TConnections[buffer2Size];
	}
	for ( i=0; i<numLocations; i++ )
		aiLocations[i].Load( file );

	if ( calculateBackwards )
	{
		// make backwards connections
		for ( i=0; i<numLocations; i++ )
		{
			TConnections& conn = aiLocations[i];
			for ( size_t j=0; j<conn.numConnections; j++ )
			{
				TConnections& c2 = aiLocations[conn.neighbours[j]];
				if ( conn.neighbours[j]>i )
				if ( c2.numConnections<4 )
				{
					c2.neighbours[c2.numConnections++] = i;
				}
			}
		}
	}

	file.FileRead(&numPatrolLocations,sizeof(size_t));
	if ( patrolLocations!=NULL )
		delete []patrolLocations;
	if ( numPatrolLocations>0 )
	{
		patrolLocations = new TConnections[numPatrolLocations];
		buffer3Size = numPatrolLocations;
	}
	else
	{
		patrolLocations = new TConnections[buffer3Size];
	}
	for ( i=0; i<numPatrolLocations; i++ )
		patrolLocations[i].Load( file );

	if ( calculateBackwards )
	{
		// make backwards connections
		for ( i=0; i<numPatrolLocations; i++ )
		{
			TConnections& conn = patrolLocations[i];
			for ( size_t j=0; j<conn.numConnections; j++ )
			{
				TConnections& c2 = patrolLocations[conn.neighbours[j]];
				if ( conn.neighbours[j]>i )
				if ( c2.numConnections<4 )
				{
					c2.neighbours[c2.numConnections++] = i;
				}
			}
		}
	}

	if ( version>1 )
	{
		file.FileRead(&numSequenceLocations,sizeof(size_t));
		if ( sequenceLocations!=NULL )
			delete []sequenceLocations;
		if ( numSequenceLocations>0 )
		{
			sequenceLocations = new TConnections[numSequenceLocations];
			buffer4Size = numSequenceLocations;
		}
		else
		{
			sequenceLocations = new TConnections[buffer4Size];
		}
		for ( i=0; i<numSequenceLocations; i++ )
			sequenceLocations[i].Load( file );
	}

	return true;
};


bool TDefFile::LoadObjects( size_t numObjects, TString* objectNames, TCompoundObject** objects,
						    TString& errStr )
{
	// assign actual objects to landscape objects
	size_t removed = 0;
	size_t index = 0;
	for ( size_t i=0; i<numLandscapeObjects; i++ )
	{
		bool found = false;
		size_t j=0;
		while ( !found && j<numObjects )
		{
			if ( landscapeObjects[index].Name()==objectNames[j] )
			{
				found = true;
				landscapeObjects[index].Object( objects[j] );
			}
			j++;
		}
		if ( !found )
		{
//			numLandscapeObjects = 0;
//			errStr = "Error: object \"" + landscapeObjects[i].Name();
//			errStr = errStr + "\" not found in available objects";
//			return false;
			// remove this landscape object
			removed++;
		}
		else
		{
			index++;
		}
	}
	numLandscapeObjects -= removed;
	return true;
}


void TDefFile::SaveBinary( const TString& fname )
{
	TPersist file(fileWrite);
	file.FileOpen( fname );

	SaveBinary( file );

	file.FileClose();
};


void TDefFile::SaveBinary( TPersist& file )
{
	size_t i;

	if ( defFileVersion>0 )
	{
		file.FileWrite( &defFileVersion, sizeof(size_t) );
	}

	file.FileWrite( signature.c_str(), signature.length()+1 );
	file.FileWrite( islandName.c_str(), islandName.length()+1 );
	file.FileWrite( binName.c_str(), binName.length()+1 );
	file.FileWrite( creator.c_str(), creator.length()+1 );
	file.FileWrite( strip.c_str(), strip.length()+1 );
	file.FileWrite( material.c_str(), material.length()+1 );

	file.FileWrite( &timeOfDay, sizeof(size_t) );
	if ( defFileVersion>0 )
	{
		file.FileWrite( &soundTrack, sizeof(size_t) );
	}

	float temp = float(water);
	file.FileWrite( &temp, sizeof(float) );

	file.FileWrite( &numLandscapeObjects, sizeof(size_t) );
	for ( i=0; i<numLandscapeObjects; i++ )
	{
		landscapeObjects[i].SaveBinary( file );
	}

	file.FileWrite( &numLocations, sizeof(size_t) );
	for ( i=0; i<numLocations; i++ )
	{
		aiLocations[i].Save( file );
	}

	file.FileWrite( &numPatrolLocations, sizeof(size_t) );
	for ( i=0; i<numPatrolLocations; i++ )
	{
		patrolLocations[i].Save( file );
	}

	if ( defFileVersion>1 )
	{
		file.FileWrite( &numSequenceLocations, sizeof(size_t) );
		for ( i=0; i<numSequenceLocations; i++ )
		{
			sequenceLocations[i].Save( file );
		}
	}
};

const TString& TDefFile::IslandName( void ) const
{
	return islandName;
};

void TDefFile::IslandName( const TString& _islandName )
{
	islandName = _islandName;
};

const TString& TDefFile::BMPName( void ) const
{
	return binName;
};

void TDefFile::BMPName( const TString& _binName )
{
	binName = _binName;
};

const TString& TDefFile::BMPStrip( void ) const
{
	return strip;
};

void TDefFile::BMPStrip( const TString& _strip )
{
	strip = _strip;
};

const TString& TDefFile::Material( void ) const
{
	return material;
};

void TDefFile::Material( const TString& _material )
{
	material = _material;
};

const TString& TDefFile::Creator( void ) const
{
	return creator;
};

void TDefFile::Creator( const TString& _creator )
{
	creator = _creator;
};

size_t TDefFile::NumLandscapeObjects( void ) const
{
	return numLandscapeObjects;
};

void TDefFile::NumLandscapeObjects( size_t _numLandscapeObjects )
{
	numLandscapeObjects = _numLandscapeObjects;
};

TLandscapeObject TDefFile::LandscapeObjects( size_t index )
{
	PreCond( index<numLandscapeObjects );
	return landscapeObjects[index];
};

void TDefFile::LandscapeObjects( size_t index, TLandscapeObject lo )
{
	PreCond( index<numLandscapeObjects );
	landscapeObjects[index] = lo;
};

size_t TDefFile::AddLandscapeObject( const TLandscapeObject& lo )
{
	if ( numLandscapeObjects>=buffer1Size )
	{
		buffer1Size += 5;
		TLandscapeObject* temp = new TLandscapeObject[buffer1Size];
		for ( size_t i=0; i<numLandscapeObjects; i++ )
			temp[i] = landscapeObjects[i];
		if ( landscapeObjects!=NULL )
			delete []landscapeObjects;
		landscapeObjects = temp;
	};

	landscapeObjects[numLandscapeObjects] = lo;
	numLandscapeObjects++;

	return numLandscapeObjects;
};

size_t TDefFile::AddAILocation( int prev, float x, float y, float z )
{
	TConnections c;
	c.x = x;
	c.y = y;
	c.z = z;

	if ( prev>=0 )
	{
		if ( aiLocations[prev].numConnections<4 )
		{
			size_t index = aiLocations[prev].numConnections;
			aiLocations[prev].neighbours[index] = numLocations;
			aiLocations[prev].numConnections++;
		}
	}

	if ( numLocations>=buffer2Size )
	{
		buffer2Size += 5;
		TConnections* temp = new TConnections[buffer2Size];
		for ( size_t i=0; i<numLocations; i++ )
			temp[i] = aiLocations[i];
		if ( aiLocations!=NULL )
			delete []aiLocations;
		aiLocations = temp;
	};

	aiLocations[numLocations] = c;
	numLocations++;
	return numLocations;
};

void TDefFile::RemoveAILocation( size_t index )
{
	size_t i;

	PreCond( index<numLocations );
	for ( i=index; i<(numLocations-1); i++ )
	{
		aiLocations[i] = aiLocations[i+1];
	}
	numLocations--;

	// remove references to this point, and renumber
	// references after this index
	for ( i=0; i<numLocations; i++ )
	{
		size_t cnt = aiLocations[i].numConnections;
		for ( size_t j=0; j<cnt; j++ )
		{
			if ( aiLocations[i].neighbours[j]==index )
			{
				for ( size_t k=j; k<(cnt-1); k++ )
				{
					aiLocations[i].neighbours[k] = aiLocations[i].neighbours[k+1];
				}
				aiLocations[i].numConnections--;
			}
			else if ( aiLocations[i].neighbours[j]>=index && 
					  aiLocations[i].neighbours[j]>0 )
			{
				aiLocations[i].neighbours[j]--;
			}
		}
	}
};

void TDefFile::GetAILocation( size_t index, float& x, float&y, float& z ) const
{
	PreCond( index < numLocations );
	x = aiLocations[index].x;
	y = aiLocations[index].y;
	z = aiLocations[index].z;
};

const TConnections& TDefFile::Connection( size_t index) const
{
	PreCond( index < numLocations );
	return aiLocations[index];
};


void TDefFile::Connection( size_t index, const TConnections& conn )
{
	PreCond( index < numLocations );
	aiLocations[index] = conn;
};


size_t TDefFile::NumLocations( void ) const
{
	return numLocations;
};

void TDefFile::NumLocations( size_t _numLocations )
{
	numLocations = _numLocations;
};

size_t TDefFile::AddAIPatrolLocation( int prev, float x, float y, float z )
{
	TConnections c;
	c.x = x;
	c.y = y;
	c.z = z;

	if ( prev>=0 )
	{
		if ( patrolLocations[prev].numConnections<4 )
		{
			size_t index = patrolLocations[prev].numConnections;
			patrolLocations[prev].neighbours[index] = numPatrolLocations;
			patrolLocations[prev].numConnections++;
		}
	}

	if ( numPatrolLocations>=buffer3Size )
	{
		buffer3Size += 5;
		TConnections* temp = new TConnections[buffer3Size];
		for ( size_t i=0; i<numPatrolLocations; i++ )
			temp[i] = patrolLocations[i];
		if ( patrolLocations!=NULL )
			delete []patrolLocations;
		patrolLocations = temp;
	};

	patrolLocations[numPatrolLocations] = c;
	numPatrolLocations++;
	return numPatrolLocations;
};


void TDefFile::RemoveAIPatrolLocation( size_t index )
{
	size_t i;

	PreCond( index<numPatrolLocations );
	for ( i=index; i<(numPatrolLocations-1); i++ )
	{
		patrolLocations[i] = patrolLocations[i+1];
	}
	numPatrolLocations--;

	// remove references to this point, and renumber
	// references after this index
	for ( i=0; i<numPatrolLocations; i++ )
	{
		size_t cnt = patrolLocations[i].numConnections;
		for ( size_t j=0; j<cnt; j++ )
		{
			if ( patrolLocations[i].neighbours[j]==index )
			{
				for ( size_t k=j; k<(cnt-1); k++ )
				{
					patrolLocations[i].neighbours[k] = patrolLocations[i].neighbours[k+1];
				}
				patrolLocations[i].numConnections--;
			}
			else if ( patrolLocations[i].neighbours[j]>=index &&
					  patrolLocations[i].neighbours[j]>0 )
			{
				patrolLocations[i].neighbours[j]--;
			}
		}
	}
};

void TDefFile::GetAIPatrolLocation( size_t index, float& x, float&y, float& z ) const
{
	PreCond( index < numPatrolLocations );
	x = patrolLocations[index].x;
	y = patrolLocations[index].y;
	z = patrolLocations[index].z;
};

const TConnections& TDefFile::PatrolConnection( size_t index) const
{
	if ( index >= numPatrolLocations )
		index = numPatrolLocations;

	PreCond( index < numPatrolLocations );
	return patrolLocations[index];
};


void TDefFile::PatrolConnection( size_t index, const TConnections& conn )
{
	PreCond( index < numPatrolLocations );
	patrolLocations[index] = conn;
};

size_t TDefFile::NumPatrolLocations( void ) const
{
	return numPatrolLocations;
};

void TDefFile::NumPatrolLocations( size_t _numPatrolLocations )
{
	numPatrolLocations = _numPatrolLocations;
};

size_t TDefFile::AddSequenceLocation( float x, float y, float z )
{
	TConnections c;
	c.x = x;
	c.y = y;
	c.z = z;

	if ( numSequenceLocations>=buffer4Size )
	{
		buffer4Size += 5;
		TConnections* temp = new TConnections[buffer4Size];
		for ( size_t i=0; i<numSequenceLocations; i++ )
			temp[i] = sequenceLocations[i];
		if ( sequenceLocations!=NULL )
			delete []sequenceLocations;
		sequenceLocations = temp;
	};

	sequenceLocations[numSequenceLocations] = c;
	numSequenceLocations++;
	return numSequenceLocations;
};

void TDefFile::RemoveSequenceLocation( size_t index )
{
	size_t i;

	PreCond( index<numSequenceLocations );
	for ( i=index; i<(numSequenceLocations-1); i++ )
	{
		sequenceLocations[i] = sequenceLocations[i+1];
	}
	numSequenceLocations--;
};

void TDefFile::GetSequenceLocation( size_t index, float& x, float&y, float& z ) const
{
	PreCond( index < numSequenceLocations );
	x = sequenceLocations[index].x;
	y = sequenceLocations[index].y;
	z = sequenceLocations[index].z;
};

const TConnections& TDefFile::SequenceConnection( size_t index) const
{
	if ( index >= numSequenceLocations )
		index = numSequenceLocations;

	PreCond( index < numSequenceLocations );
	return sequenceLocations[index];
};


void TDefFile::SequenceConnection( size_t index, const TConnections& conn )
{
	PreCond( index < numSequenceLocations );
	sequenceLocations[index] = conn;
};

size_t TDefFile::NumSequenceLocations( void ) const
{
	return numSequenceLocations;
};

void TDefFile::NumSequenceLocations( size_t _numSequenceLocations )
{
	numSequenceLocations = _numSequenceLocations;
};


size_t TDefFile::WaterLevel( void ) const
{
	return water;
};

void TDefFile::WaterLevel( size_t _water )
{
	water = _water;
};

void TDefFile::New( const TString& _binName, const TString& _islandName, 
				    const TString& _creator, const TString& _strip,
					const TString& _material,
					size_t _water )
{
	NumLandscapeObjects( 0 );
	binName = _binName;
	islandName = _islandName;
	creator = _creator;
	material = _material;
	strip = _strip;
	water = _water;
};


TMapPlayerInfo TDefFile::GetPlayerInfo( void )
{
	TMapPlayerInfo info;
	size_t i;

	for ( i=0; i<kMaxTeams; i++ )
	{
		info.teamCount[i] = 0;
	}

	for ( i=0; i<numLandscapeObjects; i++ )
	{
		TString name = landscapeObjects[i].Name().lcase();
		if ( name=="redflag.obj" )
		{
			info.teams[0][info.teamCount[0]++] = landscapeObjects[i];
			PostCond( info.teamCount[0]<16 );
		}
		else if ( name=="greenflag.obj" )
		{
			info.teams[1][info.teamCount[1]++] = landscapeObjects[i];
			PostCond( info.teamCount[1]<16 );
		}
		else if ( name=="blueflag.obj" )
		{
			info.teams[2][info.teamCount[2]++] = landscapeObjects[i];
			PostCond( info.teamCount[2]<16 );
		}
		else if ( name=="yellowflag.obj" )
		{
			info.teams[3][info.teamCount[3]++] = landscapeObjects[i];
			PostCond( info.teamCount[3]<16 );
		}
	}

	info.numTeams = 0;
	for ( i=0; i<kMaxTeams; i++ )
	{
		if ( info.teamCount[i]>0 )
		{
			info.numTeams++;
		}
	}

	// don't align - keep the r,g,b,y order for team selections
/*
	// align data
	for ( i=0; i<info.numTeams; i++ )
	{
		if ( info.teamCount[i]==0 )
		{
			size_t j = i + 1;
			while ( info.teamCount[j]==0 && j<kMaxTeams ) j++;
			PostCond( info.teamCount[j]>0 );
			info.teamCount[i] = info.teamCount[j];
			size_t k;
			for ( k=0; k<info.teamCount[i]; k++ )
			{
				info.teams[i][k] = info.teams[j][k];
			}
			info.teamCount[j] = 0;
		}
	}
*/
	return info;
};

bool TDefFile::CheckMap( TString& errStr ) const
{
	bool isok = false;
	for ( size_t i=0; i<numLandscapeObjects && !isok; i++ )
	{
		if ( landscapeObjects[i].GameImportance()>0 )
			isok = true;
	}

	// check ai
	if ( isok )
	{
		// make sure the patrol circuit is a circular entity
		if ( numPatrolLocations>0 )
		{
			size_t i = 0;
			size_t recursionCounter = 1;
			while ( i<numPatrolLocations && isok )
			{
				if ( patrolLocations[i].numConnections==0 )
				{
					errStr = "Warning: patrol circuit is not circular";
					isok = false;
				}
				i++;
			}
		}
	}
	else
	{
		errStr = "Warning: this map has no mission objectives.\n\n";
		errStr = errStr + "Consequently - this map will not work in a game\n";
	}

	return isok;
};

void TDefFile::TimeOfDay( size_t _timeOfDay )
{
	timeOfDay = _timeOfDay;
}

size_t TDefFile::TimeOfDay( void ) const
{
	return timeOfDay;
};

void TDefFile::SoundTrack( size_t _soundTrack )
{
	soundTrack = _soundTrack;
}

size_t TDefFile::SoundTrack( void ) const
{
	return soundTrack;
};

//==========================================================================

TMapPlayerInfo::TMapPlayerInfo( void )
{
	numTeams = 0;
};


TMapPlayerInfo::TMapPlayerInfo( const TMapPlayerInfo& pi )
{
	numTeams = 0;
	operator=(pi);
};


TMapPlayerInfo::~TMapPlayerInfo( void )
{
	Clear();
};


void TMapPlayerInfo::Clear( void )
{
	numTeams = 0;
	for ( size_t i=0; i<kMaxTeams; i++ )
	{
		teamCount[i] = 0;
	};
}


const TMapPlayerInfo& TMapPlayerInfo::operator = ( const TMapPlayerInfo& pi )
{
	Clear();
	numTeams = pi.numTeams;

	for ( size_t i=0; i<kMaxTeams; i++ )
	{
		teamCount[i] = pi.teamCount[i];
		size_t np = teamCount[i];
		for ( size_t j=0; j<np; j++ )
		{
			teams[i][j] = pi.teams[i][j];
		}
	}
	return *this;
};

//==========================================================================

