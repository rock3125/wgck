#include <precomp_header.h>

#include <network/packet.h>

//==========================================================================

DataStrengthUpdate::DataStrengthUpdate( void )
{
	id = id_strengthUpdate;
	cntr = 0;
};

DataStrengthUpdate::~DataStrengthUpdate( void )
{
	cntr = 0;
};

DataStrengthUpdate::DataStrengthUpdate( const DataStrengthUpdate& d )
{
	operator=(d);
};

const DataStrengthUpdate& DataStrengthUpdate::operator=( const DataStrengthUpdate& d )
{
	cntr = d.cntr;
	for ( size_t i=0; i<cntr; i++ )
	{
		strength[i] = d.strength[i];
		vehicleId[i] = d.vehicleId[i];
		vehicleType[i] = d.vehicleType[i];
	}
	return *this;
};

bool DataStrengthUpdate::Add( size_t _vehicleType, size_t _vehicleId, size_t _strength )
{
	if ( cntr < (kMaxPlayers*4) )
	{
		strength[cntr] = _strength;
		vehicleId[cntr] = _vehicleId;
		vehicleType[cntr] = _vehicleType;
		cntr++;
		return true;
	}
	return false;
};

size_t DataStrengthUpdate::Size( void ) const
{
	return (2 + sizeof(byte)*3*cntr );
};

size_t DataStrengthUpdate::Set( byte* data )
{
	data[0] = id;
	data[1] = cntr;
	size_t index = 2;
	for ( size_t i=0; i<cntr; i++ )
	{
		data[index++] = strength[i];
		data[index++] = vehicleId[i];
		data[index++] = vehicleType[i];
	}
	return Size();
};

size_t DataStrengthUpdate::Get( const byte* data )
{
	PreCond( data[0]==id_strengthUpdate );

	cntr = data[1];
	size_t index = 2;
	for ( size_t i=0; i<cntr; i++ )
	{
		strength[i] = data[index++];
		vehicleId[i] = data[index++];
		vehicleType[i] = data[index++];
	}
	return Size();
};

//==========================================================================

DataBuildingStrengthUpdate::DataBuildingStrengthUpdate( void )
{
	id = id_buildingStrengthUpdate;
	cntr = 0;
};

DataBuildingStrengthUpdate::~DataBuildingStrengthUpdate( void )
{
	cntr = 0;
};

DataBuildingStrengthUpdate::DataBuildingStrengthUpdate( const DataBuildingStrengthUpdate& d )
{
	operator=(d);
};

const DataBuildingStrengthUpdate& DataBuildingStrengthUpdate::operator=( const DataBuildingStrengthUpdate& d )
{
	cntr = d.cntr;
	for ( size_t i=0; i<cntr; i++ )
	{
		buildingId[i] = d.buildingId[i];
		strength[i] = d.strength[i];
	}
	return *this;
};

bool DataBuildingStrengthUpdate::Add( size_t _buildingId, size_t _strength )
{
	if ( cntr < kMaxPlayers )
	{
		buildingId[cntr] = _buildingId;
		strength[cntr] = _strength;
		cntr++;
		return true;
	}
	return false;
};

size_t DataBuildingStrengthUpdate::Size( void ) const
{
	return (2 + (sizeof(size_t)+sizeof(byte))*cntr );
};

size_t DataBuildingStrengthUpdate::Set( byte* data )
{
	data[0] = id;
	data[1] = cntr;
	size_t index = 2;
	for ( size_t i=0; i<cntr; i++ )
	{
		size_t* d = (size_t*)&data[index];
		d[0] = buildingId[i];
		index += sizeof(size_t);
		data[index++] = strength[i];
	}
	return Size();
};

size_t DataBuildingStrengthUpdate::Get( const byte* data )
{
	PreCond( data[0]==id_buildingStrengthUpdate );

	cntr = data[1];
	size_t index = 2;
	for ( size_t i=0; i<cntr; i++ )
	{
		size_t* d = (size_t*)&data[index];
		buildingId[i] = d[0];
		index += sizeof(size_t);
		strength[i] = data[index++];
	}
	return Size();
};

//==========================================================================

DataPacketQueue::DataPacketQueue( void )
{
	head = 0;
	tail = 0;
	for ( size_t i=0; i<kMaxQueueSize; i++ )
	{
		size[i] = 0;
		sock[i] = INVALID_SOCKET;
		data[i] = NULL;
	}
};

DataPacketQueue::~DataPacketQueue( void )
{
	Clear();
};

bool DataPacketQueue::Add( SOCKET _sock, size_t _size, const byte* _data )
{
	// make sure we're not going to get an overflow (can only hold kQueueSize-1 items)
	size_t temp = head+1;
	if ( temp>=kMaxQueueSize )
		temp = 0;
	if ( temp==tail )
		return false;

	PreCond( _data!=NULL && _size>0 );

	sock[head] = _sock;
	size[head] = _size;
	data[head] = new byte[_size];
	PostCond( data[head]!=NULL );
	for ( size_t i=0; i<_size; i++ )
		data[head][i] = _data[i];
	head++;
	if ( head>=kMaxQueueSize )
		head=0;
	return true;
};

void DataPacketQueue::Clear( void )
{
	for ( size_t i=0; i<kMaxQueueSize; i++ )
	{
		size[i] = 0;
		sock[i] = INVALID_SOCKET;
		if ( data[i]!=NULL )
			delete data[i];
		data[i] = NULL;
	}
	head = 0;
	tail = 0;
};

size_t DataPacketQueue::Count( void ) const
{
	if ( head>tail )
	{
		return (head-tail);
	}
	else if ( head<tail )
	{
		return (tail+head);
	}
	return 0;
};

bool DataPacketQueue::GetNext( SOCKET& s, size_t& _size, byte* buf )
{
	if ( head==tail )
		return false;
	
	PreCond( buf!=NULL );
	for ( size_t i=0; i<size[tail]; i++ )
	{
		buf[i] = data[tail][i];
	}

	_size = size[tail];
	s = sock[tail];
	return true;
};

void DataPacketQueue::RemoveMessage( void )
{
	if ( head==tail )
		return;
	
	delete data[tail];
	data[tail] = NULL;
	sock[tail] = INVALID_SOCKET;
	size[tail] = 0;

	tail++;
	if ( tail>=kMaxQueueSize )
		tail = 0;
};

//==========================================================================


DataLandscapeStatus::DataLandscapeStatus( void )
	: data(NULL)
{
	id = id_landscapeStatus;
};

DataLandscapeStatus::~DataLandscapeStatus( void )
{
	if ( data!=NULL )
		delete data;
	data = NULL;
};

DataLandscapeStatus::DataLandscapeStatus( const DataLandscapeStatus& ls )
	: data(NULL)
{
	id = id_landscapeStatus;
	operator=(ls);
};

const DataLandscapeStatus& DataLandscapeStatus::operator=( const DataLandscapeStatus& ls )
{
	id = ls.id;
	size = ls.size;
	if ( data!=NULL )
		delete data;
	data = new byte[size];
	for ( size_t i=0; i<size; i++ )
		data[i] = ls.data[i];
	return *this;
};


void DataLandscapeStatus::SetData( size_t _size, const byte* array )
{
	size = _size;

	if ( data!=NULL )
		delete data;
	data = new byte[size];

	for ( size_t i=0; i<size; i++ )
		data[i] = array[i];
};


size_t DataLandscapeStatus::Set( byte* _data )
{
	_data[0] = id;
	size_t* sz = (size_t*)&_data[1];
	sz[0] = size;

	for ( size_t i=0; i<size; i++ )
		_data[i+1+sizeof(size_t)] = data[i];

	return (sizeof(size_t) + size + 1);
};

size_t DataLandscapeStatus::Get( const byte* _data )
{
	id = _data[0];
	size_t* sz = (size_t*)&_data[1];
	size = sz[0];

	if ( data!=NULL )
		delete data;
	data = new byte[size];

	for ( size_t i=0; i<size; i++ )
		data[i] = _data[i+1+sizeof(size_t)];

	return (sizeof(size_t) + size + 1);
};

//==========================================================================

DataScores::DataScores( void )
{
	for ( size_t i=0; i<kMaxPlayers; i++ )
		scores[i] = 0;
};

DataScores::~DataScores( void )
{
};

DataScores::DataScores( const DataScores& s )
{
	operator=(s);
};

const DataScores& DataScores::operator=( const DataScores& s )
{
	for ( size_t i=0; i<kMaxPlayers; i++ )
		scores[i] = s.scores[i];
	return *this;
};

size_t DataScores::Set( byte* data )
{
	size_t* sc = (size_t*)data;
	for ( size_t i=0; i<kMaxPlayers; i++ )
		sc[i] = scores[i];
	return Size();
};

size_t DataScores::Get( const byte* data )
{
	size_t* sc = (size_t*)data;
	for ( size_t i=0; i<kMaxPlayers; i++ )
		scores[i] = sc[i];
	return Size();
};


size_t DataScores::Size( void ) const
{
	return kMaxPlayers*sizeof(size_t);
};

//==========================================================================

DataExplosion::DataExplosion( void )
{
	id = id_explode;
	playerId = 0;
	x = 0;
	y = 0;
	z = 0;
	for ( size_t i=0; i<kMaxPlayers; i++ )
	{
		shield[i] = 0;
		scores[i] = 0;
	}
	objectId = 0;
};

DataExplosion::~DataExplosion( void )
{
};

DataExplosion::DataExplosion( const DataExplosion& e )
{
	operator=(e);
};

const DataExplosion& DataExplosion::operator=( const DataExplosion& e )
{
	x = e.x;
	y = e.y;
	z = e.z;
	objectId = e.objectId;
	for ( size_t i=0; i<kMaxPlayers; i++ )
	{
		shield[i] = e.shield[i];
		scores[i] = e.scores[i];
	}
	return *this;
};

size_t DataExplosion::Set( byte* data )
{
	size_t i;

	data[0] = id;
	data[1] = playerId;
	float* f = (float*)&data[2];
	f[0] = x;
	f[1] = y;
	f[2] = z;
	size_t indx = sizeof(float)*3 + 2;
	
	for ( i=0; i<kMaxPlayers; i++ )
		data[i+indx] = shield[i];
	indx += kMaxPlayers;

	size_t* sc = (size_t*)&data[indx];
	for ( i=0; i<kMaxPlayers; i++ )
		sc[i] = scores[i];
	indx += kMaxPlayers*sizeof(size_t);

	size_t* p = (size_t*)&data[indx];
	p[0] = objectId;

	return indx+sizeof(size_t);
};

size_t DataExplosion::Get( const byte* data )
{
	size_t i;

	id = data[0];
	playerId = data[1];
	float* f = (float*)&data[2];
	x = f[0];
	y = f[1];
	z = f[2];
	size_t indx = sizeof(float)*3 + 2;

	for ( i=0; i<kMaxPlayers; i++ )
		shield[i] = data[i+indx];
	indx += kMaxPlayers;
	
	size_t* sc = (size_t*)&data[indx];
	for ( i=0; i<kMaxPlayers; i++ )
		scores[i] = sc[i];
	indx += kMaxPlayers*sizeof(size_t);
	
	size_t* p = (size_t*)&data[indx];
	objectId = p[0];
	return indx + sizeof(size_t);
};

//==========================================================================

DataCanIJoin::DataCanIJoin( const TString& _name )
{
	id = id_canijoin;
	version = kNetworkVersion;
	name = _name;
};

DataCanIJoin::DataCanIJoin( void )
{
	id = id_canijoin;
	version = kNetworkVersion;
};

DataCanIJoin::~DataCanIJoin( void )
{
	id = 0;
};

DataCanIJoin::DataCanIJoin( const DataCanIJoin& d )
{
	operator=(d);
};

const DataCanIJoin& DataCanIJoin::operator=( const DataCanIJoin& d )
{
	id = d.id;
	version = d.version;
	name = d.name;
	return *this;
};

size_t DataCanIJoin::Set( byte* data )
{
	PreCond( data!=NULL );

	data[0] = id;
	float* v = (float*)&data[1];
	v[0] = version;
	size_t index = 1 + sizeof(float);
	strcpy( (char*)&data[index], name.c_str() );

	return (name.length()+index+1);
};

size_t DataCanIJoin::Get( const byte* data )
{
	PreCond( data!=NULL );

	id = data[0];
	float* v = (float*)&data[1];
	version = v[0];
	size_t index = 1 + sizeof(float);
	char temp[256];
	strcpy( temp, (const char*)&data[index] );
	name = temp;

	return (name.length()+index+1);
};

//==========================================================================

DataYouCan::DataYouCan( const bool& _accept, const byte& _playerId, 
					    const byte& _map, const TString& _reason )
{
	id = id_youcan;
	playerId = _playerId;
	accept = _accept;
	map = _map;
	reason = _reason;
};

DataYouCan::DataYouCan( void )
{
	id = id_youcan;
	map = 0;
	playerId = 0;
	accept = false;
	reason = "bad packet - not initialised";
};

DataYouCan::~DataYouCan( void )
{
	id = 0;
};

DataYouCan::DataYouCan( const DataYouCan& d )
{
	operator=(d);
};

const DataYouCan& DataYouCan::operator=( const DataYouCan& d )
{
	id = d.id;
	playerId = d.playerId;
	accept = d.accept;
	reason = d.reason;
	map = d.map;
	return *this;
};

size_t DataYouCan::Set( byte* data )
{
	PreCond( data!=NULL );

	data[0] = id;
	data[1] = playerId;
	if ( accept )
		data[2] = 1;
	else
		data[2] = 0;
	data[3] = map;
	strcpy( (char*)&data[4], reason.c_str() );

	return (reason.length()+5);
};

size_t DataYouCan::Get( const byte* data )
{
	PreCond( data!=NULL );

	id = data[0];
	PostCond( id==id_youcan );
	playerId = data[1];
	if ( data[2]==1 )
		accept = true;
	else
		accept = false;
	map = data[3];
	char temp[256];
	strcpy( temp, (const char*)&data[4] );
	reason = temp;

	return (reason.length()+5);
};

//==========================================================================

DataICanJoin::DataICanJoin( const byte& _playerId, const bool& _accept,
						    const byte& _team )
{
	id = id_icanjoin;
	playerId = _playerId;
	accept = _accept;
	team = _team;
	character = 0;
};

DataICanJoin::DataICanJoin( byte _playerId )
{
	id = id_icanjoin;
	playerId = _playerId;
	team = 0;
	accept = false;
	character = 0;
};

DataICanJoin::~DataICanJoin( void )
{
	id = 0;
};

DataICanJoin::DataICanJoin( const DataICanJoin& d )
{
	operator=(d);
};

const DataICanJoin& DataICanJoin::operator=( const DataICanJoin& d )
{
	id = d.id;
	playerId = d.playerId;
	accept = d.accept;
	team = d.team;
	name = d.name;
	character = d.character;
	return *this;
};

size_t DataICanJoin::Set( byte* _data )
{
	PreCond( _data!=NULL );

	_data[0] = id;
	if ( accept )
		_data[1] = 1;
	else
		_data[1] = 0;
	_data[2] = playerId;
	_data[3] = team;
	_data[4] = character;
	memcpy( &_data[5], name.c_str(), name.length()+1 );
	return (name.length()+6);
};

size_t DataICanJoin::Get( const byte* _data )
{
	PreCond( _data!=NULL );

	id = _data[0];
	if ( _data[1]==1 )
		accept = true;
	else
		accept = false;
	playerId = _data[2];
	team = _data[3];
	character = _data[4];
	name = (const char*)&_data[5];
	return (name.length()+6);
};

//==========================================================================

DataQuit::DataQuit( byte _playerId, const TString& _response )
{
	id = id_quit;
	playerId = _playerId;
	response = _response;
};

DataQuit::DataQuit( void )
{
	id = id_quit;
};

DataQuit::~DataQuit( void )
{
	id = 0;
};

DataQuit::DataQuit( const DataQuit& d )
{
	operator=(d);
};

const DataQuit& DataQuit::operator=( const DataQuit& d )
{
	id = d.id;
	playerId = d.playerId;
	response = d.response;
	return *this;
};

size_t DataQuit::Set( byte* data )
{
	PreCond( data!=NULL );

	data[0] = id;
	data[1] = playerId;
	data[2] = 0;
	if ( response.length() > 0 )
	{
		strcpy( (char*)&data[2], response.c_str() );
	}
	return 3 + response.length();
};

size_t DataQuit::Get( const byte* data )
{
	PreCond( data!=NULL );

	id = data[0];
	playerId = data[1];
	response = (char*)&data[2];
	return 3 + response.length();
};

//==========================================================================

DataClientStart::DataClientStart( const byte& _playerId )
{
	id = id_start;
	playerId = _playerId;
	team = 0;
	character = 0;
};

DataClientStart::DataClientStart( void )
{
	id = id_start;
	playerId = 0;
	team = 0;
	character = 0;
};

DataClientStart::~DataClientStart( void )
{
	id = 0;
};

DataClientStart::DataClientStart( const DataClientStart& d )
{
	operator=(d);
};

const DataClientStart& DataClientStart::operator=( const DataClientStart& d )
{
	id = d.id;
	playerId = d.playerId;
	team = d.team;
	character = d.character;
	name = d.name;
	return *this;
};

size_t DataClientStart::Set( byte* data )
{
	PreCond( data!=NULL );

	data[0] = id;
	data[1] = playerId;
	data[2] = team;
	data[3] = character;
	strcpy( (char*)&data[4], name.c_str() );
	return name.length() + 5;
};

size_t DataClientStart::Get( const byte* data )
{
	PreCond( data!=NULL );

	id = data[0];
	playerId = data[1];
	team = data[2];
	character = data[3];
	name = (const char*)&data[4];
	return name.length() + 5;
};

//==========================================================================

DataParticulars::DataParticulars( void )
{
	id = id_myparticulars;

	playerId = 0;
	team = 0;
	vehicleId = 0;
	vehicleType = 0;
};

DataParticulars::DataParticulars( size_t _playerId, size_t _team, 
								  size_t _vehicleType, size_t _vehicleId )
{
	id = id_myparticulars;

	playerId = _playerId;
	team = _team;
	vehicleId = _vehicleId;
	vehicleType = _vehicleType;
};

DataParticulars::~DataParticulars( void )
{
	id = 0;
};


DataParticulars::DataParticulars( const DataParticulars& d )
{
	operator=(d);
};

const DataParticulars& DataParticulars::operator=( const DataParticulars& d )
{
	id = d.id;

	playerId = d.playerId;
	vehicleId = d.vehicleId;
	vehicleType = d.vehicleType;
	team = d.team;

	return *this;
};

size_t DataParticulars::Set( byte* data )
{
	PreCond( data!=NULL );
	PreCond( playerId<kMaxPlayers );

	data[0] = id;
	data[1] = playerId;
	data[2] = vehicleId;
	data[3] = vehicleType;

	return Size();
};


size_t DataParticulars::Size( void ) const
{
	return (4);
};


size_t DataParticulars::Get( const byte* data )
{
	PreCond( data!=NULL );

	id = data[0];
	playerId = data[1];
	vehicleId = data[2];
	vehicleType = data[3];
	
	return Size();
}

//==========================================================================

DataFire::DataFire( const byte& _playerId )
{
	id = id_fire;
	playerId = _playerId;
	strength = 0;
	range = 0;
};

DataFire::DataFire( void )
{
	id = id_fire;
	playerId = 0;
	strength = 0;
	range = 0;
};

DataFire::~DataFire( void )
{
	id = 0;
};

DataFire::DataFire( const DataFire& df )
{
	operator=(df);
};

const DataFire& DataFire::operator=( const DataFire& df )
{
	id = df.id;
	playerId = df.playerId;
	range = df.range;
	strength = df.strength;

	return *this;
};

size_t DataFire::Set( byte* _data )
{
	_data[0] = id;
	_data[1] = playerId;
	_data[2] = strength;
	_data[3] = range;
	return Size();
};

size_t DataFire::Size( void ) const
{
	return 4;
};

size_t DataFire::Get( const byte* _data )
{
	PreCond( _data!=NULL );

	id = _data[0];
	playerId = _data[1];
	strength = _data[2];
	range = _data[3];
	return Size();
};

//==========================================================================

DataGlobalCoords::DataGlobalCoords( void )
{
	id = id_globalcoords;
	containsScores = 0;
	cntr = 0;
};


DataGlobalCoords::~DataGlobalCoords( void )
{
	id = id_globalcoords;
	containsScores = 0;
	cntr = 0;
};


DataGlobalCoords::DataGlobalCoords( const DataGlobalCoords& d )
{
	operator=(d);
};

const DataGlobalCoords& DataGlobalCoords::operator=( const DataGlobalCoords& d )
{
	id = d.id;
	cntr = d.cntr;
	containsScores = d.containsScores;
	scores = d.scores;

	for ( size_t i=0; i<cntr; i++ )
	{
		size[i] = d.size[i];
		vehicleType[i] = d.vehicleType[i];
		vehicleId[i] = d.vehicleId[i];
		PreCond( size[i]<kMaxSingleCoordSize );
		memcpy( dat[i], d.dat[i], size[i] );
	}

	return *this;
};


void DataGlobalCoords::Add( size_t _vehicleType, size_t _vehicleId, 
						    size_t _size, const byte* _data )
{
	PreCond( _size<kMaxSingleCoordSize );

	if ( cntr < (kMaxPlayers*2) )
	{
		vehicleType[cntr] = _vehicleType;
		vehicleId[cntr] = _vehicleId;
		size[cntr] = _size;
		memcpy( dat[cntr], _data, _size );
		cntr++;
	}
};


void DataGlobalCoords::Read( size_t index, size_t& _vehicleType, 
							 size_t& _vehicleId, size_t& _size, 
							 byte* _data )
{
	PreCond( index < (kMaxPlayers*2) );
	_vehicleType = vehicleType[index];
	_vehicleId = vehicleId[index];
	_size = size[index];
	memcpy( _data, dat[index], size[index] );
};


void DataGlobalCoords::SetScores( const DataScores& s )
{
	containsScores = 1;
	scores = s;
};


size_t DataGlobalCoords::Set( byte* _data )
{
	PreCond( _data!=NULL );

	_data[0] = id;
	_data[1] = containsScores;
	size_t* d = (size_t*)&_data[2];
	d[0] = cntr;
	size_t index = 2 + sizeof(size_t);
	for ( size_t i=0; i<cntr; i++ )
	{
		size_t* d = (size_t*)&_data[index];
		d[0] = size[i];
		index += sizeof(size_t);
		memcpy( &_data[index], dat[i], size[i] );
		index += size[i];
		_data[index++] = vehicleType[i];
		_data[index++] = vehicleId[i];
	}
	if ( containsScores==1 )
	{
		index += scores.Set( &_data[index] );
	}
	return index;
};


size_t DataGlobalCoords::Get( const byte* _data )
{
	PreCond( _data!=NULL );

	id = _data[0];
	containsScores = _data[1];
	size_t* d = (size_t*)&_data[2];
	cntr = d[0];
	size_t index = 2 + sizeof(size_t);
	for ( size_t i=0; i<cntr; i++ )
	{
		size_t* d = (size_t*)&_data[index];
		size[i] = d[0];
		index += sizeof(size_t);
		memcpy( dat[i], &_data[index], size[i] );
		index += size[i];
		vehicleType[i] = _data[index++];
		vehicleId[i] = _data[index++];
	}
	if ( containsScores==1 )
	{
		index += scores.Get( &_data[index] );
	}
	return index;
};

//==========================================================================

DataWelcome::DataWelcome( void )
{
	id = id_welcome;
	lighting = 1;
	gameType = 0;
	maxPoints = 0;
	map = 0;
	cntr = 0;
};


DataWelcome::~DataWelcome( void )
{
	id = 0;
	lighting = 1;
};


DataWelcome::DataWelcome( const DataWelcome& d )
{
	operator=(d);
};


const DataWelcome& DataWelcome::operator=( const DataWelcome& d )
{
	id = d.id;
	gameType = d.gameType;
	lighting = d.lighting;
	maxPoints = d.maxPoints;
	map = d.map;
	cntr = d.cntr;

	for ( size_t i=0; i<cntr; i++ )
	{
		playerId[i] = d.playerId[i];
		name[i] = d.name[i];
		team[i] = d.team[i];
		character[i] = d.character[i];
	}
	return *this;
};

void DataWelcome::Add( size_t _playerId, const TString& _name, byte _team, byte _character )
{
	if ( cntr < kMaxPlayers )
	{
		playerId[cntr] = _playerId;
		team[cntr] = _team;
		name[cntr] = _name;
		character[cntr] = _character;
		cntr++;
	}
};

size_t DataWelcome::Set( byte* _data )
{
	PreCond( _data!=NULL );

	_data[0] = id;
	_data[1] = lighting;
	_data[2] = gameType;
	_data[3] = map;
	size_t* d = (size_t*)&_data[4];
	d[0] = maxPoints;
	d[1] = cntr;
	size_t index = 4 + sizeof(size_t)*2;
	for ( size_t i=0; i<cntr; i++ )
	{
		_data[index++] = playerId[i];
		_data[index++] = team[i];
		_data[index++] = character[i];
		strcpy( (char*)&_data[index], name[i].c_str() );
		index = index + name[i].length() + 1;
	}
	return index;
};


size_t DataWelcome::Get( const byte* _data )
{
	PreCond( _data!=NULL );

	id = _data[0];
	lighting = _data[1];
	gameType = _data[2];
	map = _data[3];
	size_t* d = (size_t*)&_data[4];
	maxPoints = d[0];
	cntr = d[1];
	size_t index = 4 + sizeof(size_t)*2;
	for ( size_t i=0; i<cntr; i++ )
	{
		playerId[i] = _data[index++];
		team[i] = _data[index++];
		character[i] = _data[index++];
		name[i] = (char*)&_data[index];
		index = index + name[i].length() + 1;
	}
	return index;
};

//==========================================================================

DataOtherPlayerJoined::DataOtherPlayerJoined( void )
{
	id = id_otherplayerjoined;
	playerId = 0;
	team = 0;
	character = 0;
};

DataOtherPlayerJoined::DataOtherPlayerJoined( size_t _playerId, const TString& _name, 
											  byte _team, byte _character )
{
	id = id_otherplayerjoined;
	playerId = _playerId;
	name = _name;
	team = _team;
	character = _character;
};

DataOtherPlayerJoined::~DataOtherPlayerJoined( void )
{
	id = 0;
};

DataOtherPlayerJoined::DataOtherPlayerJoined( const DataOtherPlayerJoined& d )
{
	operator=(d);
};

const DataOtherPlayerJoined& DataOtherPlayerJoined::operator=( const DataOtherPlayerJoined& d )
{
	id = d.id;
	playerId = d.playerId;
	name = d.name;
	team = d.team;
	character = d.character;
	return *this;
};

size_t DataOtherPlayerJoined::Set( byte* _data )
{
	PreCond( _data!=NULL );

	_data[0] = id;
	_data[1] = team;
	_data[2] = character;
	_data[3] = playerId;
	strcpy( (char*)&_data[4], name.c_str() );
	size_t index = name.length() + 5;
	return index;
};

size_t DataOtherPlayerJoined::Get( const byte* _data )
{
	PreCond( _data!=NULL );

	id = _data[0];
	team = _data[1];
	character = _data[2];
	playerId = _data[3];
	name = (char*)&_data[4];
	size_t index = name.length() + 5;
	return index;
};

//==========================================================================

DataOtherPlayerQuit::DataOtherPlayerQuit( const byte& _playerId )
{
	id = id_otherplayerquit;
	playerId = _playerId;
};

DataOtherPlayerQuit::DataOtherPlayerQuit( void )
{
	id = id_otherplayerquit;
};

DataOtherPlayerQuit::~DataOtherPlayerQuit( void )
{
	id = 0;
};

DataOtherPlayerQuit::DataOtherPlayerQuit( const DataOtherPlayerQuit& d )
{
	operator=(d);
};

const DataOtherPlayerQuit& DataOtherPlayerQuit::operator=( const DataOtherPlayerQuit& d )
{
	id = d.id;
	playerId = d.playerId;
	return *this;
};

size_t DataOtherPlayerQuit::Set( byte* data )
{
	PreCond( data!=NULL );

	data[0] = id;
	data[1] = playerId;
	return 2;
};

size_t DataOtherPlayerQuit::Get( const byte* data )
{
	PreCond( data!=NULL );

	id = data[0];
	playerId = data[1];
	return 2;
};

//==========================================================================

class TPacket
{
public:
	TPacket( size_t id, byte* msg, size_t size );
	TPacket( const TPacket& );
	~TPacket( void );

	const TPacket& operator=( const TPacket& );

private:
	size_t	id;
	byte*	msg;
	size_t	size;
	ulong	timestamp;
};

//==========================================================================

TPacket::TPacket( size_t _id, byte* _msg, size_t _size )
	: msg(NULL)
{
	id = _id;
	size = _size;

	PreCond( _msg!=NULL );
	msg = new byte[_size];
	PostCond( msg!=NULL );
	PreCond( _size>0 );

	for ( size_t i=0; i<_size; i++ )
		msg[i] = _msg[i];
	timestamp = ::GetTickCount();
};


TPacket::TPacket( const TPacket& m )
	: msg(NULL)
{
	operator=(m);
};


TPacket::~TPacket( void )
{
	if ( msg!=NULL )
		delete msg;
	msg = NULL;
	size = 0;
	id = 0;
	timestamp = 0;
};


const TPacket& TPacket::operator=( const TPacket& m )
{
	PreCond( m.msg!=NULL );

	id = m.id;
	size = m.size;

	if ( msg!=NULL )
		delete msg;
	msg = new byte[size];
	PostCond( msg!=NULL );

	for ( size_t i=0; i<size; i++ )
		msg[i] = m.msg[i];

	timestamp = m.timestamp;

	return *this;
};

//==========================================================================

