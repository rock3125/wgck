#ifndef _OBJECT_DEFFILE_H_
#define _OBJECT_DEFFILE_H_

//==========================================================================

#include <win32/win32.h>

#include <object/compoundobject.h>
#include <object/landscapeobject.h>

#define kMaxLandscapeObjects		10000

//==========================================================================

class _EXPORT TConnections
{
public:
	TConnections( void );
	~TConnections( void );

	const TConnections& operator=( const TConnections& );
	TConnections( const TConnections& );

	void Save( TPersist& file );
	void Load( TPersist& file );

	float x,y,z;
	size_t numConnections;
	size_t recursionCounter;
	size_t neighbours[4];

	bool   visited; // for sequences only - don't save!
};

//==========================================================================

enum tteams { redTeam, greenTeam, blueTeam, yellowTeam };

class _EXPORT TMapPlayerInfo
{
public:
	TMapPlayerInfo( void );
	TMapPlayerInfo( const TMapPlayerInfo& );
	~TMapPlayerInfo( void );

	const TMapPlayerInfo& operator = ( const TMapPlayerInfo& );

	void Clear( void );

	size_t				numTeams;
	size_t				teamCount[kMaxTeams];
	TLandscapeObject	teams[kMaxTeams][kMaxPlayers];
};

//==========================================================================

class _EXPORT TDefFile
{
public:
	TDefFile( void );
	~TDefFile( void );

	TDefFile( const TDefFile& );
	const TDefFile& operator=( const TDefFile& );

	// load def file from file
	bool LoadBinary( const TString& fname, const TString& path, TString& errStr,
					 bool calculateBackwards );
	bool LoadBinary( TPersist& file, TString& errStr, bool calculateBackwards=true );
	bool LoadBinaryAs( HWND parent, TString& fname );

	// save defn to file
	void SaveBinary( const TString& fname );
	void SaveBinary( TPersist& file );

	// load compound object references into the landscapeobjects
	bool LoadObjects(  size_t numObjects, TString* objectNames, 
					   TCompoundObject** objects, TString& errStr );

	const TString&	IslandName( void ) const;
	void			IslandName( const TString& _islandName );

	const TString&	BMPName( void ) const;
	void			BMPName( const TString& _binName );

	const TString&	BMPStrip( void ) const;
	void			BMPStrip( const TString& _strip );

	const TString&	Material( void ) const;
	void			Material( const TString& _material );

	const TString&	Creator( void ) const;
	void			Creator( const TString& _creator );

	size_t			NumLandscapeObjects( void ) const;
	void			NumLandscapeObjects( size_t );

	size_t			NumLocations( void ) const;
	void			NumLocations( size_t _numLocations );

	size_t			NumPatrolLocations( void ) const;
	void			NumPatrolLocations( size_t _numPatrolLocations );
	
	TLandscapeObject	LandscapeObjects( size_t index );
	void				LandscapeObjects( size_t index, TLandscapeObject lo );

	size_t			AddLandscapeObject( const TLandscapeObject& lo );
	size_t			AddAILocation( int prev, float x, float y, float z );
	void			RemoveAILocation( size_t index );

	const TConnections&	Connection( size_t index) const;
	void				Connection( size_t index, const TConnections& );

	void			GetAILocation( size_t index, float& x, float&y, float& z ) const;

	size_t		AddAIPatrolLocation( int prev, float x, float y, float z );
	void		RemoveAIPatrolLocation( size_t index );
	void		GetAIPatrolLocation( size_t index, float& x, float&y, float& z ) const;

	size_t				AddSequenceLocation( float x, float y, float z );
	void				RemoveSequenceLocation( size_t index );
	void				GetSequenceLocation( size_t index, float& x, float&y, float& z ) const;
	const TConnections& SequenceConnection( size_t index) const;
	void				SequenceConnection( size_t index, const TConnections& conn );
	size_t				NumSequenceLocations( void ) const;
	void				NumSequenceLocations( size_t _numSequenceLocations );

	const TConnections&	PatrolConnection( size_t index) const;
	void				PatrolConnection( size_t index, const TConnections& conn );

	size_t			WaterLevel( void ) const;
	void			WaterLevel( size_t _water );

	void			TimeOfDay( size_t _timeOfDay );
	size_t			TimeOfDay( void ) const;

	void			SoundTrack( size_t _soundTrack );
	size_t			SoundTrack( void ) const;

	// do new, delete all items and clear the slate
	void			New( const TString& _binName, const TString& _islandName, 
						 const TString& _creator, const TString& _strip,
						 const TString& _material,
						 size_t waterLevel );

	// for this map get the player's positions and numbers
	TMapPlayerInfo	GetPlayerInfo( void );

	// check mission integrity of the map
	bool CheckMap( TString& errStr ) const;

private:
	TString		signature;
	TString		islandName;
	TString		binName;
	TString		strip;
	TString		material;
	TString		creator;

	size_t		water;
	size_t		timeOfDay;
	size_t		soundTrack;

	size_t				numLandscapeObjects;
	TLandscapeObject*	landscapeObjects;

	size_t				numLocations;
	size_t				numPatrolLocations;
	size_t				numSequenceLocations;

	TConnections*		aiLocations;
	TConnections*		patrolLocations;
	TConnections*		sequenceLocations;

	size_t				buffer1Size;
	size_t				buffer2Size;
	size_t				buffer3Size;
	size_t				buffer4Size;
};

//==========================================================================

#endif
