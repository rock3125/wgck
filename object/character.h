#ifndef __OBJECT_CHARACTER_H_
#define __OBJECT_CHARACTER_H_

//==========================================================================

#include <object/geometry.h>
#include <object/tank.h>
#include <object/plane.h>
#include <object/vehicle.h>

#include <network/packet.h>
#include <mdllib/mdl.h>
#include <win32/params.h>

//==========================================================================

class DataParticulars;

//==========================================================================

class _EXPORT TCharacter : public TVehicle
{
public:
	TCharacter( void );
	virtual ~TCharacter( void );

	const TCharacter& operator=( const TCharacter& );
	TCharacter( const TCharacter& );

	void operator=( const DataParticulars& dp );
	void operator=( const TLandscapeObject& obj );

	bool	Load( const TString& fname, TString& errStr );

	// reset character doing anything after re-setup of system
	void	Reset( void );

	// tell the player what weapons they have available to them
	void	SetWeapons( size_t numWeapons, TBinObject** weapons );

	// move the character according to keys
	bool	LegalCharacterMove( TLandscape& landObj, TParams& params, 
								size_t& vehicleType, size_t& vehicleId );
	void	Move( size_t& _keys, float waterLevel,
				  bool _ctrlDown, float dxa, float dya );
	void	CommitMove( void );
	void	CommitAngles( void );

	void	Logic( void ); // logic processor

	void	Draw1stPerson( void );
	void	Draw( void );
	void	DrawShadow( size_t shadowLevel );

    float	BarrelAngle( void ) const;
    void	BarrelAngle( float _barrelAngle );

    float	TurretAngle( void ) const;
    void	TurretAngle( float _turretAngle );

	float	SubmergeDepth( void ) const;
	bool	Submerged( void ) const;

	void	SelectedGun( size_t gunId );
	size_t	SelectedGun( void ) const;

	float	GunXoff( void ) const;

	void	JumpIntoVehicle( void );
	void	JumpOutOfVehicle( TTank& tank );
	void	JumpOutOfVehicle( TPlane& plane );
	void	JumpOutOfVehicle( TFlak& flak );
	void	JumpOutOfVehicle( TArtillery& artillery );
	void	JumpOutOfVehicle( TMG42& mg42 );

	// is a point inside this person?  for gun fire detection
	bool InsideVehicle( const TPoint& point ) const;
	bool InsideVehicle( float _x, float _y, float _z ) const;

	// owner of character pointer?
	bool	Owner( void ) const;
	void	Owner( bool );

	// character pointer access
	MDL*	Character( void ) const;
	void	Character( MDL* );

	// used for after res changes
	bool	ReloadCharacter( TString& errStr );

	// set sound system for character
	void SetSounds( size_t _walkSound, size_t _gun1Sound,
					size_t gun2Sound, size_t _gun3Sound,
					size_t _projectileGraphic,
					TBinObject* _parachute );

	// return amount of ammo on board
	size_t Ammo( void ) const;

	float	Speed( void ) const;
	void	Speed( float _speed );

	// available weapons
	void AvailableWeapons( bool _mg, bool _bazooka, bool _dynamite );

	// calculate the impact of an explosion at (expx,expy,expz) with strength damage
	void CalculateDamage( float expx, float expy, float expz, float damage );
	void CalculateDamage( const TPoint& point, float damage );

	void GetBoundingBox( TPoint& minPoint, TPoint& maxPoint ) const;

	// re-arm character
	void Rearm( void );

	// stop all sounds associated with me
	void StopSounds( void );

	// set the character up for machine gun operation
	void SetupForMG42( void );

	// team identifier
	size_t	VehicleType( void ) const;
	void	VehicleType( size_t _vehicleType );

	size_t	VehicleId( void ) const;
	void	VehicleId( size_t _vehicleId );

	size_t	Sequence( void ) const;
	void	Sequence( size_t _sequence );

	// can it be send still?
	bool	SendOverNetwork( void ) const;
	void	SendOverNetwork( bool _sendOverNetwork );

	// get set net data
	size_t			netSet( size_t myId, size_t vid, byte* data );
	size_t			netSize( void ) const;
	size_t			netGet( const byte* data );

private:
	void SetSequence( bool changed );
	void ShowTargeting( void );

	void ClearShadow( void );
	bool LoadShadow( const TString& fname, TString& errStr );

	void GetMatrix( void );
	void GetGunDetails( TPoint& start, TPoint& dirn, size_t& range );

private:
	friend class TLandscape;

	// mdl sequences
	enum sequence  { idle=0, run, walk, shoot, crouch, jump, swim, 
					 treadWater, shot, deadBack, deadSitting, deadStomach };

	TString	originalFilename; // filename this character came from

	float	speed; // how fast can this character move
	float	characterSpeed;

	bool	sendOverNetwork; // should this character still be sending over the network?

	// are these weapons available?
	bool	mg;
	bool	bazooka;
	bool	dynamite;

	MDL*	character;
	bool	owner; // do I own the memory allocated by MDL*?
	bool	running;
	bool	ctrlDown;
	size_t	keys;

	bool	jumping;
	bool	jumpStarted;
	size_t	jumpCounter;
	float	jumpX, jumpY, jumpZ;

	size_t	walkSound;
	bool	walkSoundOn;
	size_t	gun1Sound;
	size_t	gun2Sound;
	size_t	gun3Sound;
	bool	machinegunFire;
	bool	usingMG;
	int		machineGunChannel;
	int		walkChannel;
	TPoint	prevWalkPos;

	// character weapons
	size_t			selectedWeapon;
	size_t			numWeapons;

	float			initProjectiles;
	float			initExplosives;

	TBinObject**	weapons;
	float			numRounds[10];
	size_t			backOffTime;
	size_t			projectileGraphic;

	float	barrelAngle;
	float	turretAngle;

	float	gunXoff;

	// handgun location and matrix
	TMatrix matrix;
	TPoint	handPoint1;
	TPoint	handPoint2;

	float	fwd_x, fwd_z;

	TPoint	minPoint;
	TPoint	maxPoint;

	bool		showParachute;
	TBinObject* parachute;

	size_t		vehicleType;
	size_t		vehicleId;

	// depth at which stage we're considered as being under water
	float	submergeDepth;
	float	waterLevel; // landscape's water level
	bool	submerged; // yes or no?

	// shadows
	float			s_angle;
	float			s_scale;
	size_t			s_numSurfaces;
	size_t			s_numVertices;
	float*			s_vertices;
	size_t*			s_surfaces;
};


//==========================================================================

#endif
