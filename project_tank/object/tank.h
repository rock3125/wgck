#ifndef __OBJECT_TANK_H_
#define __OBJECT_TANK_H_

//==========================================================================

#include <object/vehicle.h>
#include <object/anim.h>
#include <object/landscape.h>

class TTank : public TVehicle
{
public:
	TTank( float shellRange );
	~TTank( void );

    bool LoadBinary( const TString& fname, TString& errStr, 
					 const TString& pathname,
					 const TString& texturePath,
					 TAnimation* exhaust=NULL );

	void Move( TLandscape& landObj, size_t key, float speed, 
			   bool canFire, bool showTarget );

	void Draw( bool exploding ) const;
	
	const float&	TurretAngle( void ) const;
	void			TurretAngle( const float& );

	const float&	BarrelAngle( void ) const;
	void			BarrelAngle( const float& );

	const float&	TargetX( void ) const;
	const float&	TargetY( void ) const;
	const float&	TargetZ( void ) const;

	void			TargetX( const float& t );

private:
	void ShowTargeting( void ) const;
	void GetTarget( TLandscape& land, 
					float& x, float& y, float& z ) const;

private:
	// the three meshes that make a tank
	TMesh* body;
	TMesh* barrel;
	TMesh* turret;

	// offsets for these meshes
	float turretX,turretY,turretZ;
	float barrelX,barrelY,barrelZ;

	// angles for these meshes
	float turretAngle;
	float barrelAngle;

	// range of ammo
	float shellRange;

	// only used for internal optimisations
	mutable float xmult,ymult,zmult;

	bool initialised;	// object loaded ok?
	bool showTarget;	// display targetting info?
	bool explode;		// exploding?

	// the exhaust(s) of this tank
	mutable TAnimation* exhaust;
	mutable float xang,yang,zang;

	// target's fire location
	float targetX,targetY,targetZ;
};

#endif
