#ifndef _OBJECT_PROJECTILE_H_
#define _OBJECT_PROJECTILE_H_

//==========================================================================

#include <object/binobject.h>
#include <object/explosion.h>
#include <object/landscape.h>
#include <win32/params.h>

//==========================================================================

class _EXPORT TProjectile
{
public:
	TProjectile( void );
	TProjectile( const TProjectile& );
	~TProjectile( void );

	const TProjectile& operator=( const TProjectile& );

	void		Logic( TLandscape* landObj, TParams& params );
	void		Draw( float cameraAngle, bool transp );

	void		SetApp( TApp* app );

	// fire!
	void		Start( void );

	float		X( void ) const;
	float		Y( void ) const;
	float		Z( void ) const;
	
	float		Damage( void ) const;
	void		Damage( float _damage );
	
	size_t		Range( void ) const;
	void		Range( size_t _range );
	
	TPoint&		StartPoint( void );
	void		StartPoint( const TPoint& _start );
	
	TPoint&		Dirn( void );
	void		Dirn( const TPoint& _dirn, float cameraAngle );
	
	float		Scale( void ) const;
	void		Scale( float _scale );
	
	TBinObject*	Graphic( void ) const;
	void		Graphic( TBinObject* _graphic );

	// get/set only - no effect on actual object
	size_t		GraphicId( void ) const;
	void		GraphicId( size_t _graphicId );

	bool		InUse( void ) const;

	bool		Explode( void ) const;
	void		Explode( bool _explode, size_t _explosionType );

	void		Explosion( TSound* _soundSystem, size_t _explosionSound,
						   TBinObject* _explosion, TTexture** _explosionF );

	// for sending over networks
	size_t netSet( size_t myId, byte* data ) const;
	size_t netSize( void ) const;
	size_t netGet( const byte* data );

private:
	TApp*	app;

	TPoint	start;
	TPoint	dirn;
	TPoint	prev;

	float	damage;
	size_t	range;

	bool	firstTime;
	bool	inUse;
	bool	exploding;
	float	scale;

	float	yangle;
	float	xangle;

	size_t	graphicId;  // for reloading the graphic

	TBinObject* graphic;
	TExplosion	explosion;
};

//==========================================================================

#endif

