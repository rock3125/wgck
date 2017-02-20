#include <precomp_header.h>

#include <win32/win32.h>
#include <object/explosion.h>
#include <object/camera.h>

//==========================================================================

const float degToRad = 0.01745329252033f;

//==========================================================================

const TExplosion& TExplosion::operator=( const TExplosion& ex )
{
	frame = ex.frame;
	numFrames = ex.numFrames;
	scale = ex.scale;
	started = ex.started;

	explosionSound = ex.explosionSound;
	soundSystem = ex.soundSystem;

	x = ex.x;
	y = ex.y;
	z = ex.z;

	explosion = ex.explosion;
	explosionFlame = ex.explosionFlame;

	explosionType = ex.explosionType;

	return *this;
};

 TExplosion::TExplosion( const TExplosion& ex )
	: explosion( NULL ),
	  explosionFlame( NULL ),
	  soundSystem( NULL )
{
	operator=(ex);
};

TExplosion::TExplosion( void )
	: explosion( NULL ),
	  explosionFlame( NULL ),
	  soundSystem( NULL )
{
	x = 0;
	y = 0;
	z = 0;

	scale = 1;
	frame = 0;
	started = false;

	numFrames = kNumExplosionAnimations;

	explosionType = etFull;
	explosionSound = 0;
};

TExplosion::~TExplosion( void )
{
	explosion = NULL;
};

void TExplosion::SetXYZ( float _x, float _y, float _z )
{
	x = _x;
	y = _y;
	z = _z;
};

void TExplosion::SetXYZ( const TPoint& point )
{
	x = point.x;
	y = point.y;
	z = point.z;
};

void TExplosion::GetXYZ( float& _x, float& _y, float& _z )
{
	_x = x;
	_y = y;
	_z = z;
};

void TExplosion::Draw( float cameraAngle, bool transp ) const
{
	if ( explosion!=NULL && !transp && (explosionType==etFull || explosionType==etMaterial) )
	{
		glPushMatrix();
	
			glTranslatef( x,y,z );
			glScalef( scale,scale,scale );

			explosion->DrawExplosion( frame, numFrames );
	
		glPopMatrix();
	}
	if ( explosionFlame!=NULL && transp && 
		 (explosionType==etFull || explosionType==etFire) )
	{
		glEnable( GL_COLOR_MATERIAL );
		glColor4ub( 255,255,255,255 );

		glPushMatrix();
	
		glTranslatef( x,y,z );
		glRotatef( cameraAngle, 0,1,0 );

		glScalef( scale,scale,scale );

//		glDisable(GL_DEPTH_TEST);

		explosionFlame[frame]->Draw( -0.5f,-0.5f,0.5f,0.5f );
		
//		glEnable(GL_DEPTH_TEST);

		glPopMatrix();
	}
};

size_t TExplosion::Frame( void ) const
{
	return frame;
};

void TExplosion::Frame( size_t _frame )
{
	frame = _frame;
};

float TExplosion::Scale( void ) const
{
	return scale;
};

void TExplosion::Scale( float _scale )
{
	scale = _scale;
};

void TExplosion::Start( size_t _explosionType )
{
	frame = 0;
	explosionType = _explosionType;
	started = true;

	if ( soundSystem!=NULL )
	{
		soundSystem->Play( explosionSound, TPoint(x,y,z) );
	}
};

bool TExplosion::Finished( void ) const
{
	return (!started);
};

void TExplosion::Logic( void )
{
	if ( started )
	{
		frame++;
		if ( frame>=numFrames )
		{
			frame = 0;
			started = false;
		}
	}
};

void TExplosion::Explosion( TSound* _soundSystem, size_t _explosionSound,
							TBinObject* _explosion, TTexture** _explosionF )
{
	explosion = _explosion;
	explosionFlame = _explosionF;
	soundSystem = _soundSystem;
	explosionSound = _explosionSound;
};

size_t TExplosion::ExplosionType( void ) const
{
	return explosionType;
};

void TExplosion::ExplosionType( size_t _explosionType )
{
	explosionType = _explosionType;
};

//==========================================================================
