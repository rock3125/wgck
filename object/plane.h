#ifndef __OBJECT_PLANE_H_
#define __OBJECT_PLANE_H_

//==========================================================================

#include <object/geometry.h>
#include <object/compoundObject.h>
#include <object/vehicle.h>

#include <win32/vehicleSettings.h>
#include <win32/params.h>

#include <common/sounds.h>

//==========================================================================

class TLandscape;
class DataParticulars;
class TPlane;
class TApp;
class TCamera;

//==========================================================================

class _EXPORT TPlane : public TVehicle
{
public:
	TPlane( void );
	virtual ~TPlane( void );

	TPlane( const TPlane& );
	const TPlane& operator = (const TPlane&);
	void operator = ( const TLandscapeObject& o );
	void operator=( TCharacter& dp );
	void operator=( TVehicleSettings& vs );
	void operator=( DataParticulars& dp );

	virtual void Draw( bool transp );
	virtual void Draw1stPerson( void );

	// move the vehicle according to key
	virtual void Move( size_t& key, bool ctrlDown, float dxa, float dya,
					   TCamera* cam );

	// commit the previous call to Move for real or abort
	void CommitMove( void );

	bool	LegalVehicleMove( TLandscape* landObj, TParams& params );

	float	GetXZRadius( void ) const;

	bool	Collision( float x, float y, float z,
					   float x1, float y1, float z1 ) const;

	void Bounds( float& x1, float& z1, float& y1,
				 float& x2, float& z2, float& y2,
				 float& x3, float& z3, float& y3,
				 float& x4, float& z4, float& y4 ) const;

	// after loading
	void SetupObject( void );

	float SubmergeDepth( void ) const;
	void  SubmergeDepth( float smd );

	float Speed( void ) const;

    float	Y( void ) const;
    float	NY( void ) const;
    void	Y( float _y );

	float	AngleAdjust( void ) const;
	void	AngleAdjust( float _angleAdjust );

	void	PauseAnimations( bool p );

	bool InsideVehicle( float x, float y, float z ) const;
	bool InsideVehicle( const TPoint& point ) const;

	void	SelectedGun( size_t _gunId );
	size_t	SelectedGun( void ) const;

	// scale of plane
	float	BarrelAngle( void ) const;
	void	BarrelAngle( float ba );

	float	TurretAngle( void ) const;
	void	TurretAngle( float ta );

	float	FlyingY( void ) const;
	void	FlyingY( float _flyingY );

	float	TailAngle( void ) const;
	void	TailAngle( float _tailAngle );

	bool	Flying( void ) const;
	void	Flying( bool _flying );

	// return amount of ammo on board
	size_t Ammo( void ) const;

	// set bomb graphic
	void SetProjectile( size_t _projectile );

	// return health status of plane
	size_t Strength( void ) const;
	void Strength( size_t _strength );

	// explode plane (stop drawing it - shields==0)
	void Explode( void );

	// calculate the impact of an explosion at (expx,expy,expz) with strength damage
	void CalculateDamage( float expx, float expy, float expz, float damage );
	void CalculateDamage( const TPoint& point, float damage );

	// landscape object builder settings
	float			Fuel( void ) const;
	void			Fuel( float _fuel );

	void			Rearm( void );
	void			Refuel( void );
	void			Repair( void );

	void GetTailProp( float& tx, float& ty, float& tz, 
					  float& px, float& py, float& pz );
	void SetTailProp( float tx, float ty, float tz, 
					  float px, float py, float pz );

	// stop all sounds associated with me
	void StopSounds( void );

	// can I enter this plane?
	bool CanEnter( void ) const;

	// weapons range
	float			WeaponsRange( void ) const;

	// is the plane upsidedown?
	bool UpsideDown( void ) const;

	// network read/write
	size_t netSet( size_t myId, size_t vid, byte* data );
	size_t netSize( void ) const;
	size_t netGet( const byte* data );

protected:
	void UpdatePlaneMatrix( void );
	void ShowTargeting( void );
	void GetBarrelMatrix( void );
	void GetGunDetails( TPoint& _start, TPoint& _dirn, size_t& _gunRange );
	void GetGunDetails( void );

private:
	friend class AIPlane;

	TMesh* tail;
	TMesh* prop;

	size_t gunRange; // weapons fire distance

	// sounds
	size_t			engineSound;
	bool			engineSoundOn;
	int				engineChannel;
	TPoint			prevEnginePos;

	size_t			gunSound;
	bool			gunSoundOn;
	int				machineGunChannel;
	TPoint			prevMachineGunPos;

	size_t			projectile; // plane's bomb

	// location of vehicle stuff
	float barrelAngle;
	float turretAngle;
	float ceiling;

	float flyingAngle;
	float flyingOffset;
	float restAngle;
	bool  flying;
	float speed;
	float takeoffSpeed;
	float response;
	float maxSpeed;
	float yoff;

	float hover; // hover height (graphic adjustment)

	size_t	gunId;
	bool	showTarget;
	size_t	numRounds;
	size_t	numBombs;
	size_t	numMissiles;

	size_t	initNumMissiles;
	size_t	initNumBombs;

	float	fuel;
	float	fuelConsumption;

	size_t	logicCounter;

	float flyingY, n_flyingY;

	float angleAdjust;
	float n_angleAdjust;

	float propAngle;
	float tailAngle;

	size_t reloadTime;

	float matrix[16];

	mutable TPoint	start;
	TPoint			dirn;
	TMatrix			gunMatrix;

	// these can only be done once per plane model
	float	propX, propY, propZ;
	float	tailX, tailY, tailZ;

	size_t	counter;

	// distance stuff
	bool	isaDistanceObject;
	float	distanceFromCamera;
	float	switchDistance;
};

//==========================================================================

#endif

