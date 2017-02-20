#ifndef __TANK_TANKAPP_H__
#define __TANK_TANKAPP_H__

//==========================================================================

#include <object/binobject.h>
#include <object/landscape.h>
#include <win32/events.h>

#include <object/character.h>

#include <network/packet.h>

#include <win32/params.h>

#include <ai/tankai.h>
#include <ai/planeai.h>
#include <ai/landscapeai.h>
#include <ai/flakai.h>
#include <ai/artilleryai.h>
#include <ai/mg42ai.h>
#include <ai/v2ai.h>

#include <dialogs/controlsDialog.h>
#include <win32/vehicleSettings.h>
#include <object/projectile.h>
#include <object/explosive.h>
#include <object/soundTrack.h>

//==========================================================================

class TTank;
class TMessage;

//==========================================================================

void DrawFlag( float x, float y, float z, byte r, byte g, byte b );
void DrawArrow( float x, float y, float z, byte r, byte g, byte b );

//==========================================================================

enum ProjectileGraphics { grNone, grShell, grBomb };

//==========================================================================

class TApp : public TEvent
{
public:
	TApp( HINSTANCE, TString );
    ~TApp( void );

	//==========================================================================
	// network processing
	void ServerNetworkLogic( void );
	void ClientNetworkLogic( void );
	bool CheckPlayerName( const TString& _name, TString& errStr );
	void SetupGuest( const TString& _playerName, size_t port, const TString& _hostName );
	// can a person join a team on a specified map (depends on gameType)
	bool CanJoinMap( size_t playerId, size_t map, TString& errStr );
	bool CanJoinTeam( size_t playerId, size_t _team, TString& errStr );
	void AddPlayer( size_t playerId, size_t _team, size_t _character, TString name );
	void SendClientStartMessageToServer( void );
	// fire a projectile on behalf of a client or server
	void NetworkFireProjectile( const TProjectile& proj );
	void NetworkFireProjectile( size_t playerId, const TProjectile& proj );
	void NetworkDropExplosive( const TExplosive& expl );
	void NetworkDropExplosive( size_t playerId, const TExplosive& expl );
	// update system wide strengths for clients
	void UpdateStrengths( void );
	// get update packet for newly joined players
	size_t GetStrengthUpdatePacket( byte* buf );

	// see if a vehicle is already occupied by a player or not
	// defined in tankapp_logic.cpp
	bool Occupied( size_t vehicleType, size_t vehicleId );

	//==========================================================================
	// startup: load menu stuff, and once off stuff like sounds
	bool StartupSystem( TString& errStr );

	// main logic hook, runs at logic fps
	void Logic( void );

	TLandscape* LandObj( void );

	// render hook, runs at max fps
	void Render( void );

	// new level - load all & setup initials
	bool ReloadObjects( TString& errStr );

	// resolution reset - get all objects anew
	bool ReloadGraphics( TString& errStr );

	// minimal reload for system
	bool ReloadMenu( TString& errStr );

	// event callbacks
	void MouseDown( int button, int x, int y);
	void MouseUp( int button, int, int );
	void MouseMove( int x, int y );

	void MouseDown( int button, int x, int y, int dx, int dy );
	void MouseUp( int button, int, int, int, int );
	void MouseMove( int x, int y, int, int );

	void KeyDown( size_t kd );
	void KeyUp( size_t ku );
	void KeyPress( size_t key );
	void DoMenu( int menu );

	// destroy window callback
	void Destroy( void );

	// read setup info from registry
	bool GetRegistrySettings( void );

	// return compound object from all loaded objects by searching by name
	// used by landscape to create a map with objects on it
	TCompoundObject*	GetObjectByName( const TString& name, size_t objectIndex=0, 
										 size_t numObjects=0 );

	// interface for join dialog
	bool	ClientReceivedWelcome( void ) const;
	bool	ClientReceivedStart( void ) const;
	bool	ClientError( void ) const;
	void	ClientError( bool );
	TString ClientErrorStr( void ) const;

	// add a means of transportation
	bool AddTransport( TLandscapeObject& v2, TLandscapeObject& target );
	bool AddTransport( TLandscapeObject& obj );

	// firing system management
	// an object Fires, after which the projectile explodes
	// at a location.  The system always needs to draw
	// projectiles and explosions for the game itself
	TProjectile* FireProjectile( TPoint& start, TPoint& dirn,
								 size_t graphicName, size_t range, 
								 float damage );
	TProjectile* FireProjectile( const byte* buf );

	// special case of fire projectile - direct fire
	void Fire( TPoint& start, TPoint& dirn, size_t range, size_t damage );

	void ExplodeAt( const TPoint& point, size_t explosionType, float damage );

	void ProjectileLogic( void );
	void DrawProjectiles( TCamera& cam, bool transp );

	// a shell explodes of size damageFactor - tell everyone to calculate
	// damages
	void CalculateExplosionDamage( const TPoint& point, float damageFactor );

	// game-over mission-accomplish tests
	void ResetGameObjects( void );
	void AddGameObject( TLandscapeObject* obj );
	void GameStatus( bool& gameOver, bool& missionAccomplished ) const;

	// drop explosive by player
	void DropExplosive( float x, float y, float z, size_t timer );
	TExplosive* DropExplosive( const byte* buf );

	// access to these items
	TPlane*		Planes( void );
	size_t		NumPlanes( void );

	TTank*		Tanks( void );
	size_t		NumTanks( void );
	
	size_t		NumFlak( void );
	TFlak*		Flak( void );

	size_t		NumArtillery( void );
	TArtillery* Artillery( void );

	size_t		NumMG42s( void );
	TMG42*		MG42s( void );

	AITank*		AITanks( void );
	AIPlane*	AIPlanes( void );
	AIFlak*		AIFlaks( void );
	AIArtillery*AIArtilleries( void );
	AIMG42*		AIMG42s( void );

	// music
	void StopMusic( void );
	void StartMusic( size_t track );

	// stop all game sounds
	void StopSounds( void );

	// network assistants for transmitting projectile information
	TBinObject* ProjectileNameToBin( size_t graphicName ) const;
	size_t		ProjectileBinToName( TBinObject* graphic ) const;

	// reload menu or game objects - depends on status
	void	ReloadAsRequired( void );

private:
	bool	ResetObjects( TString& errStr );

	bool	LoadAboutScreen( TString& errStr );

	bool	ShowScores( void ) const;
	void	ShowScores( bool );

	void	ShowTimeout( size_t );
	size_t	ShowTimeout( void ) const;

	bool	ShowMap( void ) const;
	void	ShowMap( bool );

	void	MapTimeout( size_t );
	size_t	MapTimeout( void ) const;

	// returns true if firing - logic sub
	void LogicSinglePlayer( void );
	bool LogicSinglePlayer( TLandscape& landObj, TCharacter& player, 
							size_t& keys, bool ctrlDown, bool showTarget );

	void LogicGameMenu( void );

	// render-sub
	void RenderGameMenu( void );

	void RenderSinglePlayer( void );
	void RenderSinglePlayer( TLandscape& landObj, TCharacter& player );

	// load a tank given a position
	bool LoadTank( TTank& tank, TString& errStr );
	bool LoadMap( byte map, TLandscape& landObj, TString& errStr );

	bool LegalVehicleMove( TPlane& plane );

	void DrawScores( void );

	bool LoadBackgroundPicture( TString& errStr );

	// get all .obj files for reference
	size_t	NumObjects( void ) const;
	TString GetObjectName( size_t index ) const;

	// is dist (x1,z1) to (x2,z2) inside render circle?
	bool InRange( float x1, float z1, float x2, float z2 ) const;

	// draw status bar for health, fuel, etc
	void DrawBar( float x, float y, float z, size_t amount, float size );

private:
	TLandscapeAI	landscapeAI;					// ai system
	TVehicleList	vehicles;						// vehicle list

	TCompoundObject*objects[kMaxObjects];			// available objects
	TString			objectNames[kMaxObjects];		// names
	size_t			numObjects;						// count

	TBinObject*		parachute;						// parachute

	TCharacter		characters[kMaxModels];			// available loadable models
	byte			character[kMaxPlayers];			// which character is the player?

	size_t			numModels;						// count
	size_t			operatorId;

	TTank			tanks[kMaxTanks];				// tanks on landscape
	size_t			numTanks;
	// count
	AITank			aitanks[kMaxTanks];
	size_t			numAITanks;

	AIPlane			aiplanes[kMaxTanks];
	size_t			numAIPlanes;

	TPlane			planes[kMaxTanks];				// planes on landscape
	size_t			numPlanes;						// count

	TFlak			flak[kMaxTanks];				// flak
	size_t			numFlak;
	AIFlak			aiflak[kMaxTanks];
	size_t			numAIFlak;

	TArtillery		artillery[kMaxTanks];			// artillery
	size_t			numArtillery;
	AIArtillery		aiartillery[kMaxTanks];
	size_t			numAIArtillery;

	TMG42			mg42[kMaxTanks];				// Machine gun 42
	size_t			numMG42s;
	AIMG42			aimg42[kMaxTanks];
	size_t			numAIMG42;

	V2				v2[kMaxTanks];					// AI v2 rockets
	size_t			numV2;

	TParams			params;

	TBinObject*		weapons[kMaxWeapons];			// weapons
	size_t			numWeapons;						// count
	TBinObject*		bomb;							// bomb graphic for dropping from planes
	TBinObject*		shell;							// shell graphic for tanks shooting

	TLandscape*		landObj;						// the landscape

	// main system menu vars
	TTexture*		mainMenuBackground;				// background screen for menu
	TTexture*		about;							// about screen jpg
	TTexture*		downloadScreen;					// download screen jpg
	TBinObject*		aboutLogo;						// about 3d text
	bool			showAbout;						// show about screen?
	bool			showDownload;					// show download screen?
	float			aboutAngle;						// rotational angle

	bool			leftButtonDown;

	TBinObject*		title;							// 3D title text
	TBinObject*		missionControl;					// missionControl text
	TBinObject*		gameOverText;					// 3D game over text
	TBinObject*		missionAccomplishedText;		// 3D mission accomplished text
	float			animCount;						// animation counter for 3D text
	TBinObject*		sky;							// the sky
	TBinObject*		explosion;						// explosion object - used for all

//	TTexture*		explosionCircle;				// explosion circle graphic
	TTexture*		explosionFlame[20];				// explosion flame graphics

	TMessage*		msg;							// msg system
	TTexture*		cloudTexture;					// background

	// misc background instrument display items
	TString			iconPath;

	TTexture*		iconPerson[6];
	TTexture*		iconWeapon[7];
	TTexture*		iconVehicle[15];

	TString			iconPersonName[6];
	TString			iconWeaponName[7];
	TString			iconVehicleName[15];

    TString			path;							// data path
	TString			texturePath;					// texture path

	UserKeys		userKeys;						// this user's predefined keys

	TCamera			cam;							// game cam

	size_t			numGameObjects;					// game-over / mission accomplished tests
	TLandscapeObject**	gameObjects;

	TString			backgroundBitmap;

	size_t			backgroundSound;				// atmospheric sound
	bool			backgroundSoundPlaying;
	int				backgroundSoundChannel;

    // from registry
	TString		playerName;
	TString		hostName;
	TString		gameName;

	// misc
	bool		showBoundingBox;
    bool 		leftDown;
	bool		rightDown;
	bool		lazy;

	float		deltaXAngle;
	float		deltaYAngle;

	size_t		keys;

	size_t		cameraType;

	// for join dialog functionality
	bool		clientReceivedWelcome;
	bool		clientReceivedStart;
	bool		clientError;
	TString		clientErrorStr;

	// map logic
	size_t		showTimeout;
	size_t		mapTimeout;
	bool		showScores;
	bool		showMap;
	float		minMapScale;
	float		maxMapScale;
	float		mapScaleIncrease;
	float		mapScale;

	// music
	bool		mp3playing;
	HANDLE		hMusicThread;
	DWORD		musicThreadId;

	// my particulars - not to be transmitted
	byte		myTeam;
	byte		myCharacter;

	bool		gameOver;						// is it?
	bool		missionAccomplished;

	// vehicle vars for this player
	bool	scoreUpdated;

	// single player mission flag
	bool	singlePlayerMission;
	bool	clientGame;
	bool	serverGame;
	
	bool	showTarget;							// show targetting system?

	// all possible projectiles in the game
	TProjectile			projectiles[kMaxPlayers];

	// all possible explosives in the game
	TExplosive			explosives[kMaxPlayers];

	// network vars
	// all details send & collected for server & client
	byte				team[kMaxPlayers];				// whose team is the player on?
	size_t				scores[kMaxPlayers];			// scoring
	size_t				maxPoints;						// # points before game-over

	// progress bar assist
	size_t				lastProgress;
	size_t				progressSize;

	size_t				numResources;					// refuel, rearm, and repair stations
	TLandscapeObject**	resources;						// cache

	// sequence story lining
	size_t				numSequenceLocations;			// sequence locationing
	TConnections*		sequenceLocations;

	// sound track list
	SoundTrack			soundTracks;

	// multi-player settings
	bool			multiplayerSlotInUse[kMaxPlayers];	// used slots
	bool			multiplayerPlaying[kMaxPlayers];	// set after c_start received
	TString			multiplayerNames[kMaxPlayers];		// names for multiplayer game
														// only used for server data send flag

	bool			tipsShown;
};

//==========================================================================

#endif

