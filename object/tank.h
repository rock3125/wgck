#ifndef __OBJECT_TANK_H_
#define __OBJECT_TANK_H_

//==========================================================================

#include <object/camera.h>
#include <object/vector.h>
#include <object/compoundObject.h>
#include <object/anim.h>
#include <object/landscape.h>
#include <object/recordSystem.h>
#include <object/vehicle.h>

#include <win32/vehicleSettings.h>
#include <win32/params.h>

//==========================================================================

class _EXPORT TTank : public TVehicle
{
public:
	TTank( void );
	~TTank( void );

	void operator=( TLandscapeObject& );
	void operator=( TCharacter& dp );
	void operator=( TVehicleSettings& vs );

	// examine the mesh and setup the turrets etc.
	bool SetupTank( TString& errStr );

	// recording system
	void SetupRecording( const TString& filename );
	bool StopRecording( TString& errSr );

	bool SetupPlayback( const TString& filename, TString& errSr );
	void StopPlayback( void );

	bool LegalVehicleMove( TLandscape* landObj, TParams& params );

	bool Move( TLandscape& landObj, size_t& key, bool showTarget,
			   float dxa, float dya, class TCamera* );

	// after loading
	void CommitMove( void );

	void Draw1stPerson( void );
	void Draw( bool transp );

	float			SubmergeDepth( void ) const;
	void			SubmergeDepth( float smd );

	bool			Collision( float x, float y, float z,
							   float x1, float y1, float z1 ) const;

	// overwritten y - has different value from default
	float Y( void ) const;
	void  Y( float _y );
	
	float	GetXZRadius( void ) const;

	void	PauseAnimations( bool p );

	// vehicles height above landscape
	void  HoverHeight( float hh );
	float HoverHeight( void ) const;

	const float&	TurretAngle( void ) const;
	void			TurretAngle( const float& );

	const float&	BarrelAngle( void ) const;
	void			BarrelAngle( const float& );

	bool InsideVehicle( float x, float y, float z ) const;
	bool InsideVehicle( const TPoint& point ) const;

	// no turret on tank design?
	bool TurretLess( void ) const;

	// person inside the tank?
	bool Occupied( void ) const;
	void Occupied( bool o );

	// setup firing projectile
	void SetProjectile( size_t _projectile );

	// return amount of ammo on board
	size_t Ammo( void ) const;

	// return health status of tank
	void   Strength( size_t _strength );
	size_t Strength( void ) const;

	// explode the vehicle
	void Explode( void ) ;

	// calculate the impact of an explosion at (expx,expy,expz) with strength damage
	void CalculateDamage( float expx, float expy, float expz, float damage );
	void CalculateDamage( const TPoint& point, float damage );

	float			Fuel( void ) const;
	void			Fuel( float _fuel );

	void			Rearm( void );
	void			Refuel( void );
	void			Repair( void );

	// can enter vehicle?
	bool			CanEnter( void ) const;

	void GetTurretBarrel( float& tx, float& ty, float& tz, 
						  float& bx, float& by, float& bz );
	void SetTurretBarrel( float tx, float ty, float tz, 
						  float bx, float by, float bz );

	// stop all sounds associated with me
	void StopSounds( void );

	// for ai calcs
	void GetGunDetails( TPoint& _start, TPoint& _dirn, size_t& _gunRange );

	float Speed( void ) const;

	// get set net data
	size_t	netSet( size_t myId, size_t vid, byte* data );
	size_t	netSize( void ) const;
	size_t	netGet( const byte* data );

private:
	void ShowTargeting( void );

	// update the barrel's matrix
	void GetBarrelMatrix( void );
	void GetGunDetails( void );

private:
	// the three meshes that make a tank
	TMesh*	body[2];
	TMesh*	barrel[2];
	TMesh*	turret[2];

	// sound system
	size_t	engineSound;
	size_t	gunSound;
	bool	engineSoundOn;
	bool	gunSoundOn;
	int		engineChannel;
	TPoint	prevEnginePos;

	// gun
	long	reloadTime;
	bool	readyToFire;

	size_t	gunRange;	// gun's range

	float	fuel;
	float	fuelConsumption;

	// offsets for these meshes
	float turretX;
	float turretY;
	float turretZ;

	float barrelX,barrelY,barrelZ;
	float barrelXadj, barrelYadj, barrelZadj;

	// angles for these meshes
	bool  turretLess;
	float turretAngle;
	float barrelAngle;

	bool  occupied;

	// range of ammo
	float	shellRange;
	float	speed; // no prizes for guessing this one
	float	actualSpeed; // movement speed (if applicable)

	bool initialised;	// object loaded ok?
	bool showTarget;	// display targetting info?
	bool prevEngineOn;	// engine on previously?
	bool prevFireOn;
	bool selfDestruct;	// blow yourself up

	size_t		projectile;	// the projectile graphic fired from the tank

	// record tank movements?
	bool			playback;
	bool			record;
	TTankRecord*	recording;
	TString			recordingFname;

	float minBarrelAngle, maxBarrelAngle;

	// vehicle status
	size_t	numShells;
	size_t	initNumShells;
	size_t	kReloadTime;

	size_t	moveCounter;

	mutable TPoint	start;
	TPoint			dirn;
	TMatrix			matrix;

	// leave tracks behind when moving
	bool	moving;
	size_t	trackIndex;
	size_t	numTracks;
	float	tracks[kMaxTracks*6];

	float hoverHeight;	// how high above the landscape does this vehicle sit
	float submergeDepth; // how deep into the water can I go?  (+ve number)

	float diff;

	float	distanceFromCamera;
	bool	isaDistanceObject;
	float	switchDistance;


	size_t dirnkey;
};

#endif
