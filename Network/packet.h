#ifndef _NETWORK_PACKET_H_
#define _NETWORK_PACKET_H_

//==========================================================================

#include <win32/win32.h>
//#include <object/tank.h>
//#include <object/plane.h>
//#include <object/character.h>
//#include <object/flak.h>
//#include <object/artillery.h>
//#include <ai/v2ai.h>
#include <winsock.h>

//==========================================================================

class TCharacter;

//==========================================================================

#define kMaxQueueSize		100
#define kNetworkVersion		1.15f
#define kMaxSingleCoordSize	128

enum packetId  { id_canijoin = 1,
				 id_icanjoin,
				 id_start,
				 id_quit,
				 id_myparticulars,
				 id_fire,

				 id_youcan,
				 id_globalcoords,
				 id_welcome,
				 id_otherplayerjoined,
				 id_otherplayerquit,
				 id_explode,
				 id_landscapeStatus,

				 id_vehicle,
				 id_planeData,
				 id_flakData,
				 id_tankData,
				 id_mg42Data,
				 id_characterData,
				 id_artilleryData,
				 id_v2Data,

				 id_projectile,
				 id_strengthUpdate,
				 id_buildingStrengthUpdate,
				 id_explosive

				};

//==========================================================================

class _EXPORT DataScores
{
public:
	DataScores( void );
	~DataScores( void );
	DataScores( const DataScores& );
	const DataScores& operator=( const DataScores& );

	size_t	Size( void ) const;
	size_t	Set( byte* data );
	size_t	Get( const byte* data );

	size_t	scores[kMaxPlayers];
};

//==========================================================================

class _EXPORT DataStrengthUpdate
{
public:
	DataStrengthUpdate( void );
	~DataStrengthUpdate( void );
	DataStrengthUpdate( const DataStrengthUpdate& );
	const DataStrengthUpdate& operator=( const DataStrengthUpdate& );

	bool	Add( size_t vehicleType, size_t vehicleId, size_t strength );

	size_t	Size( void ) const;
	size_t	Set( byte* data );
	size_t	Get( const byte* data );

	size_t	cntr;
	byte	id;
	byte	strength[kMaxPlayers*4];
	byte	vehicleType[kMaxPlayers*4];
	byte	vehicleId[kMaxPlayers*4];
};

//==========================================================================

class _EXPORT DataBuildingStrengthUpdate
{
public:
	DataBuildingStrengthUpdate( void );
	~DataBuildingStrengthUpdate( void );
	DataBuildingStrengthUpdate( const DataBuildingStrengthUpdate& );
	const DataBuildingStrengthUpdate& operator=( const DataBuildingStrengthUpdate& );

	bool	Add( size_t buildingId, size_t strength );

	size_t	Size( void ) const;
	size_t	Set( byte* data );
	size_t	Get( const byte* data );

	size_t	cntr;
	byte	id;
	byte	buildingId[kMaxPlayers];
	byte	strength[kMaxPlayers];
};

//==========================================================================

class _EXPORT DataLandscapeStatus
{
public:
	DataLandscapeStatus( void );
	~DataLandscapeStatus( void );
	DataLandscapeStatus( const DataLandscapeStatus& );
	const DataLandscapeStatus& operator=( const DataLandscapeStatus& );

	void SetData( size_t _size, const byte* array );

	size_t	Set( byte* data );
	size_t	Get( const byte* data );

	byte	id;
	size_t	size;
	byte*	data;
};

//==========================================================================

// revolving fifo network queue
class _EXPORT DataPacketQueue
{
	DataPacketQueue( const DataPacketQueue& );
	const DataPacketQueue& operator=( const DataPacketQueue& );
public:
	DataPacketQueue( void );
	~DataPacketQueue( void );

	bool	Add( SOCKET sock, size_t size, const byte* data );
	size_t	Count( void ) const;

	// GetNext gets the message, but doesn't remove it from the front of the queue
	bool	GetNext( SOCKET& s, size_t& size, byte* data );

	// Remove message works in conjunction with GetNext, it does the actual remove
	void	RemoveMessage( void );

private:
	void	Clear( void );

private:
	size_t	head;
	size_t	tail;

	size_t	size[kMaxQueueSize];
	SOCKET	sock[kMaxQueueSize];
	byte*	data[kMaxQueueSize];
};

//==========================================================================

class _EXPORT DataCanIJoin
{
public:
	DataCanIJoin( const TString& name );
	DataCanIJoin( void );
	~DataCanIJoin( void );
	DataCanIJoin( const DataCanIJoin& );
	const DataCanIJoin& operator=( const DataCanIJoin& );

	size_t	Set( byte* data );
	size_t	Get( const byte* data );

	byte	id;
	float	version;
	TString	name;
};


class _EXPORT DataYouCan
{
public:
	DataYouCan( const bool& accept, const byte& playerId, 
				const byte& map, const TString& reason );
	DataYouCan( void );
	~DataYouCan( void );
	DataYouCan( const DataYouCan& );
	const DataYouCan& operator=( const DataYouCan& );

	size_t	Set( byte* data );
	size_t	Get( const byte* data );

	byte	id;
	byte	playerId;
	byte	map;
	bool	accept;
	TString	reason;
};


class _EXPORT DataQuit
{
public:
	DataQuit( byte playerId, const TString& response );
	DataQuit( void );
	~DataQuit( void );
	DataQuit( const DataQuit& );
	const DataQuit& operator=( const DataQuit& );

	size_t	Set( byte* data );
	size_t	Get( const byte* data );

	byte	id;
	byte	playerId;
	TString	response;
};


class _EXPORT DataClientStart
{
public:
	DataClientStart( const byte& playerId );
	DataClientStart( void );
	~DataClientStart( void );
	DataClientStart( const DataClientStart& );
	const DataClientStart& operator=( const DataClientStart& );

	size_t	Set( byte* data );
	size_t	Get( const byte* data );

	byte	id;
	byte	playerId;
	byte	team;
	byte	character;
	TString	name;
};


class _EXPORT DataFire
{
public:
	DataFire( const byte& playerId );
	DataFire( void );
	~DataFire( void );
	DataFire( const DataFire& );
	const DataFire& operator=( const DataFire& );

	size_t	Set( byte* data );
	size_t	Get( const byte* data );

	size_t	Size( void ) const;

	byte			id;
	byte			playerId;
	byte			range;
	byte			strength;
};


class _EXPORT DataParticulars
{
public:
	DataParticulars( void );
	DataParticulars( size_t _playerId, size_t _team, 
					 size_t _vehicleType, size_t _vehicleId );
	~DataParticulars( void );
	DataParticulars( const DataParticulars& );
	const DataParticulars& operator=( const DataParticulars& );

	size_t	Size( void ) const;
	size_t	Set( byte* data );
	size_t	Get( const byte* data );

	byte		id;
	byte		team;
	byte		playerId;
	byte		vehicleId;		// id of vehicle 0..kMaxWhatever
	byte		vehicleType;	// type of vehicle vtNone..vtFlak
};


class _EXPORT DataICanJoin
{
public:
	DataICanJoin( const byte& playerId, const bool& accept,
				  const byte& team );
	DataICanJoin( byte playerId );
	~DataICanJoin( void );
	DataICanJoin( const DataICanJoin& );
	const DataICanJoin& operator=( const DataICanJoin& );

	size_t	Set( byte* data );
	size_t	Get( const byte* data );

	byte			id;
	byte			playerId;
	byte			team;
	byte			character;
	TString			name;
	bool			accept;
};


class _EXPORT DataGlobalCoords
{
public:
	DataGlobalCoords( void );
	~DataGlobalCoords( void );
	DataGlobalCoords( const DataGlobalCoords& );
	const DataGlobalCoords& operator=( const DataGlobalCoords& );

	void Add( size_t vehicleType, size_t vehicleId, size_t size, const byte* data );
	void Read( size_t index, size_t& vehicleType, size_t& vehicleId, 
			   size_t& _size, byte* _data );

	void SetScores( const DataScores& );

	size_t	Set( byte* data );
	size_t	Get( const byte* data );

	byte				id;
	byte				containsScores;
	DataScores			scores;

	// count and data buffer for compressed coordinates
	size_t				cntr;
	byte				dat[kMaxPlayers*2][kMaxSingleCoordSize]; // kMaxPlayers players and kMaxPlayers AIs
	size_t				size[kMaxPlayers*2];
	byte				vehicleType[kMaxPlayers*2];
	byte				vehicleId[kMaxPlayers*2];
};


class _EXPORT DataAICoords
{
public:
	DataAICoords( void );
	~DataAICoords( void );
	DataAICoords( const DataAICoords& );
	const DataAICoords& operator=( const DataAICoords& );

	void Add( const DataParticulars& );

	size_t	Set( byte* data );
	size_t	Get( const byte* data );

	byte				id;
	size_t				cntr;
	DataParticulars		data[kMaxPlayers];
};


class _EXPORT DataWelcome
{
public:
	DataWelcome( void );
	~DataWelcome( void );
	DataWelcome( const DataWelcome& );
	const DataWelcome& operator=( const DataWelcome& );

	void Add( size_t _playerId, const TString& _name, byte _team, byte _character );

	size_t	Set( byte* data );
	size_t	Get( const byte* data );

	byte				id;
	byte				lighting;
	byte				gameType;
	byte				map;
	size_t				maxPoints;

	size_t				cntr;
	byte				playerId[kMaxPlayers];
	TString				name[kMaxPlayers];
	byte				team[kMaxPlayers];
	byte				character[kMaxPlayers];
};


class _EXPORT DataOtherPlayerJoined
{
public:
	DataOtherPlayerJoined( void );
	DataOtherPlayerJoined( size_t playerId, const TString& name, 
						   byte team, byte character );
	~DataOtherPlayerJoined( void );
	DataOtherPlayerJoined( const DataOtherPlayerJoined& );
	const DataOtherPlayerJoined& operator=( const DataOtherPlayerJoined& );

	size_t	Set( byte* data );
	size_t	Get( const byte* data );

	byte				id;
	byte				team;
	byte				character;
	byte				playerId;
	TString				name;
};


class _EXPORT DataOtherPlayerQuit
{
public:
	DataOtherPlayerQuit( const byte& playerId );
	DataOtherPlayerQuit( void );
	~DataOtherPlayerQuit( void );
	DataOtherPlayerQuit( const DataOtherPlayerQuit& );
	const DataOtherPlayerQuit& operator=( const DataOtherPlayerQuit& );

	size_t	Set( byte* data );
	size_t	Get( const byte* data );

	byte	id;
	byte	playerId;
};

class _EXPORT DataExplosion
{
public:
	DataExplosion( void );
	~DataExplosion( void );

	DataExplosion( const DataExplosion& );
	const DataExplosion& operator=( const DataExplosion& );

	size_t	Set( byte* data );
	size_t	Get( const byte* data );

	byte   id;
	byte   playerId;
	float  x,y,z;
	byte   shield[kMaxPlayers];
	size_t scores[kMaxPlayers];
	size_t objectId;
};

#endif
