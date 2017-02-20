#ifndef __OBJECT_ARTILLERY_H_
#define __OBJECT_ARTILLERY_H_

//==========================================================================

#include <object/geometry.h>
#include <object/compoundObject.h>
#include <object/vehicle.h>

#include <win32/vehicleSettings.h>
#include <win32/params.h>

#include <common/sounds.h>

//==========================================================================

class TLandscape;
class TArtillery;
class TApp;
class TCharacter;

//==========================================================================

class _EXPORT TArtillery : public TVehicle
{
public:
	TArtillery( void );
	virtual ~TArtillery( void );

	TArtillery( const TArtillery& );
	const TArtillery& operator = (const TArtillery&);
	void operator = ( const TLandscapeObject& o );
	void operator=( TCharacter& dp );
	void operator=( TVehicleSettings& vs );

	virtual void Draw( bool transp );
	virtual void Draw1stPerson( void );

	// move the vehicle according to key
	virtual void Move( size_t& key, bool ctrlDown, float dxa, float dya );

	// commit the previous call to Move for real or abort
	virtual void CommitMove( void );

	bool LegalVehicleMove( TLandscape* landObj, TParams& params );

	bool	Collision( float x, float y, float z,
					   float x1, float y1, float z1 ) const;

	void Bounds( float& x1, float& z1, float& y1,
				 float& x2, float& z2, float& y2,
				 float& x3, float& z3, float& y3,
				 float& x4, float& z4, float& y4 ) const;

	float Speed( void ) const;

    float	Y( void ) const;
    float	NY( void ) const;
    void	Y( float _y );

	void	PauseAnimations( bool p );

	bool InsideVehicle( float x, float y, float z ) const;
	bool InsideVehicle( const TPoint& point ) const;

	float	BarrelAngle( void ) const;
	void	BarrelAngle( float ba );

	float	TurretAngle( void ) const;
	void	TurretAngle( float ta );

	// return amount of ammo on board
	size_t Ammo( void ) const;

	// set shell graphic
	void SetProjectile( size_t _projectile );

	// return health status of plane
	size_t Strength( void ) const;
	void Strength( size_t _strength );

	// explode plane (stop drawing it - shields==0)
	void Explode( void );

	// calculate the impact of an explosion at (expx,expy,expz) with strength damage
	void CalculateDamage( float expx, float expy, float expz, float damage );
	void CalculateDamage( const TPoint& point, float damage );

	float			Fuel( void ) const;
	void			Fuel( float _fuel );

	void			Rearm( void );
	void			Refuel( void );
	void			Repair( void );

	// stop all sounds associated with me
	void StopSounds( void );

	// can I enter this plane?
	bool CanEnter( void ) const;

	// get set net data
	size_t			netSet( size_t myId, size_t vid, byte* data );
	size_t			netSize( void ) const;
	size_t			netGet( const byte* data );

	void			SetSoldier( TCharacter* _soldier );
	void			RemoveSoldier( void );
	void			CharacterOwner( bool _characterOwner );

protected:
	void UpdateArtilleryMatrix( void );
	void ShowTargeting( void );
	void GetBarrelMatrix( void );
	void GetGunDetails( TPoint& _start, TPoint& _dirn, size_t& _gunRange );
	void GetGunDetails( void );

private:
	friend class AIArtillery;

	size_t gunRange; // weapons fire distance

	// sounds
	size_t			engineSound;
	bool			engineSoundOn;
	int				engineChannel;
	TPoint			prevEnginePos;

	size_t			gunSound;

	size_t			projectile; // artillery's shell

	float			barrelAngle;
	float			turretAngle;

	float			hover; // hover height (graphic adjustment)

	bool	showTarget;
	size_t	numShells;
	size_t	initNumShells;

	float	fuel;
	float	fuelConsumption;

	size_t	logicCounter;

	float speed;
	float actualSpeed;
	float maxBarrelAngle;
	float minBarrelAngle;

	size_t reloadTime;

	float matrix[16];

	TCharacter*		soldier;
	bool			characterOwner; // do we need to take care of the soldier?

	mutable TPoint	start;
	TPoint			dirn;
	TMatrix			gunMatrix;
};

//==========================================================================

#endif

