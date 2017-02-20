#ifndef _OBJECT_EXPLOSIVE_H_
#define _OBJECT_EXPLOSIVE_H_

//==========================================================================

#include <object/binobject.h>
#include <object/geometry.h>
#include <common/sounds.h>
#include <object/camera.h>

//==========================================================================

class _EXPORT TExplosive
{
public:
	TExplosive( void );

	const TExplosive& operator=( const TExplosive& );
	TExplosive( const TExplosive& );

	~TExplosive( void );

	void	SetXYZ( float x, float y, float z );
	void	SetXYZ( const TPoint& point );
	void	GetXYZ( float& x, float& y, float& z );

	void	SetApp( TApp* app );
	void	Logic( void );
	void	Draw( void ) const;

	void	StartTimer( size_t time ); // start in timer ms
	void	StartTimer( void ); // start - assumes explosionTime has been set!

	bool	Ready( void ) const;
	void	InUse( bool _inUse );

	float	Scale( void ) const;
	void	Scale( float _scale );

	void	Explosion( TBinObject* _explosive, float scale );

	size_t	ExplosionType( void ) const;
	void	ExplosionType( size_t _explosionType );

	void CalculateDamage( const TPoint& point, float damage );
	void CalculateDamage( float expx, float expy, float expz, float damage );

	// networking transmission of this object
	size_t netSet( size_t myId, byte* buf ) const;
	size_t netSize( void ) const;
	size_t netGet( const byte* buf );

private:
	TApp*	app;
	bool	inUse;
	size_t	explosionSound;
	float	scale;
	size_t	explosionType;
	size_t	explosionTime;

	float	x,y,z;

	TSound*		soundSystem;
	TBinObject*	explosive;
};

//==========================================================================

#endif
