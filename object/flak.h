#ifndef __OBJECT_FLAK_H_
#define __OBJECT_FLAK_H_

//==========================================================================

#include <object/geometry.h>
#include <object/compoundObject.h>
#include <object/vehicle.h>

#include <win32/vehicleSettings.h>

//==========================================================================

class TLandscape;
class DataParticulars;
class TFlak;
class TApp;

//==========================================================================

class _EXPORT TFlak : public TVehicle
{
public:
	TFlak( void );
	virtual ~TFlak( void );

	TFlak( const TFlak& );
	const TFlak& operator = (const TFlak&);

	void operator = ( const TLandscapeObject& o );
	void operator=( TCharacter& dp );
	void operator=( TVehicleSettings& vs );
	void operator=( DataParticulars& dp );

	void Draw1stPerson( void );
	virtual void Draw( bool transp );

	// move the vehicle according to key
	virtual void Move( size_t& key, bool ctrlDown, float dxa, float dya );

	float	GetXZRadius( void ) const;

	bool	Collision( float x, float y, float z,
					   float x1, float y1, float z1 );

	void Bounds( float& x1, float& z1, float& y1,
				 float& x2, float& z2, float& y2,
				 float& x3, float& z3, float& y3,
				 float& x4, float& z4, float& y4 ) const;

    float	Y( void ) const;
    void	Y( float _y );

	void	PauseAnimations( bool p );

	bool InsideVehicle( float x, float y, float z ) const;
	bool InsideVehicle( const TPoint& point ) const;

	void	SelectedGun( size_t _gunId );
	size_t	SelectedGun( void ) const;

	float	BarrelAngle( void ) const;
	void	BarrelAngle( float ba );

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

	void			Rearm( void );
	void			Refuel( void );
	void			Repair( void );

	// stop all sounds associated with me
	void StopSounds( void );

	// can I enter this flak cannon?
	bool CanEnter( void ) const;

	// get set net data
	size_t			netSet( size_t myId, size_t vid, byte* data );
	size_t			netSize( void ) const;
	size_t			netGet( const byte* data );

protected:
	void UpdateMatrix( void );
	void ShowTargeting( void );
	void GetMatrix( void );
	void GetGunDetails( TPoint& _start, TPoint& _dirn, size_t& _gunRange );
	void GetGunDetails( void );

private:
	friend class AIFlak;

	size_t			gunRange; // weapons fire distance

	// sounds
	size_t			engineSound;
	bool			engineSoundOn;
	int				engineChannel;

	size_t			gunSound;
	bool			gunSoundOn;

	size_t			projectile; // flak's shell

	float			barrelAngle;

	float			hover; // hover height (graphic adjustment)

	size_t			gunId;
	bool			showTarget;

	size_t			numRounds;
	size_t			initNumProjectiles;

	size_t			logicCounter;

	float flyingY, n_flyingY;

	float angleAdjust;
	float n_angleAdjust;

	size_t reloadTime;

	mutable TPoint	start;
	TPoint			dirn;
	TMatrix			matrix;
	TMatrix			gunMatrix;
};

//==========================================================================

#endif

