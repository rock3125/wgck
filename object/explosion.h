#ifndef _OBJECT_EXPLOSION_H_
#define _OBJECT_EXPLOSION_H_

//==========================================================================

#include <object/binobject.h>
#include <object/geometry.h>
#include <common/sounds.h>
#include <object/camera.h>

//==========================================================================

enum explosionType { etFull, etMaterial, etFire };

//==========================================================================

class _EXPORT TExplosion
{
public:
	TExplosion( void );

	const TExplosion& operator=( const TExplosion& );
	TExplosion( const TExplosion& );

	~TExplosion( void );

	void	SetXYZ( float x, float y, float z );
	void	SetXYZ( const TPoint& point );
	void	GetXYZ( float& x, float& y, float& z );

	void	Logic( void );
	void	Draw( float cameraAngle, bool transp ) const;

	void	Start( size_t _explosionType );
	bool	Finished( void ) const;

	size_t	Frame( void ) const;
	void	Frame( size_t _frame );

	float	Scale( void ) const;
	void	Scale( float _scale );

	void	Explosion( TSound* _soundSystem, size_t _explosionSound,
					   TBinObject* _explosion, TTexture** _explosionF );

	size_t	ExplosionType( void ) const;
	void	ExplosionType( size_t _explosionType );

private:
	size_t	frame;
	size_t	numFrames;
	size_t	explosionSound;
	float	scale;
	bool	started;
	size_t	explosionType;

	float	x,y,z;

	TSound*		soundSystem;
	TBinObject*	explosion;
	TTexture**	explosionFlame;
};

//==========================================================================

#endif
