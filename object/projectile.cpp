#include <precomp_header.h>

#include <object/projectile.h>
#include <object/geometry.h>

#include <tank/tankapp.h>

//==========================================================================

const float rad2Deg = 57.295779511273f;

//==========================================================================

TProjectile::TProjectile( void )
	: graphic( NULL ),
	  app( NULL )
{
	damage = 0;
	range = 0;
	scale = 0.1f;
	graphicId = 0;

	yangle = 0;
	xangle = 0;

	firstTime = true;
	exploding = false;
	inUse = false;

	explosion.Scale(10);
};

TProjectile::TProjectile( const TProjectile& p )
	: graphic( NULL )
{
	explosion.Scale(10);
	operator=(p);
};

TProjectile::~TProjectile( void )
{
	graphic = NULL;
};

const TProjectile& TProjectile::operator=( const TProjectile& p )
{
	app = p.app;

	graphicId = p.graphicId;

	start = p.start;
	dirn = p.dirn;
	prev = p.prev;

	yangle = p.yangle;
	xangle = p.xangle;

	damage = p.damage;
	range = p.range;

	exploding = p.exploding;
	inUse = p.inUse;
	scale = p.scale;

	firstTime = p.firstTime;

	graphic = p.graphic;
	explosion = p.explosion;

	return *this;
};

float TProjectile::X( void ) const
{
	return start.x;
};

float TProjectile::Y( void ) const
{
	return start.y;
};

float TProjectile::Z( void ) const
{
	return start.z;
};

float TProjectile::Damage( void ) const
{
	return damage;
};

void TProjectile::Damage( float _damage )
{
	damage = _damage;
};

size_t TProjectile::Range( void ) const
{
	return range;
};

void TProjectile::Range( size_t _range )
{
	range = _range;
};

float TProjectile::Scale( void ) const
{
	return scale;
};

void TProjectile::Scale( float _scale )
{
	scale = _scale;
};

TBinObject* TProjectile::Graphic( void ) const
{
	return graphic;
};

void TProjectile::Graphic( TBinObject* _graphic )
{
	graphic = _graphic;
};

size_t TProjectile::GraphicId( void ) const
{
	return graphicId;
};

void TProjectile::GraphicId( size_t _graphicId )
{
	graphicId = _graphicId;
};

bool TProjectile::InUse( void ) const
{
	return inUse;
};

void TProjectile::Start( void )
{
	inUse = true;
	exploding = false;
	firstTime = true;
};

void TProjectile::Explosion( TSound* _soundSystem, size_t _explosionSound,
							 TBinObject* _explosion, TTexture** _explosionF )
{
	explosion.Explosion( _soundSystem, _explosionSound,
						 _explosion, _explosionF );
};

bool TProjectile::Explode( void ) const
{
	return exploding;
};

void TProjectile::Explode( bool _explode, size_t _explosionType )
{
	exploding = _explode;
	if ( exploding )
	{
		explosion.Start( _explosionType );
		if ( app!=NULL )
			app->CalculateExplosionDamage( prev, damage );
	}
};

void TProjectile::SetApp( TApp* _app )
{
	app = _app;
};

void TProjectile::Logic( TLandscape* landObj, TParams& params )
{
	if ( inUse )
	{
		if ( !exploding )
		{
			size_t cntr = 3;
			while ( cntr>0 && range>0 )
			{
				// check collisions with both landscape
				// and objects
				if ( !firstTime )
				{
					size_t objId;
					landObj->ObjCollision( prev, start, objId );
					if ( objId>0 )
					{
						range = 0;
						exploding = true;
						explosion.Start(etFull);
						if ( app!=NULL )
						{
							app->CalculateExplosionDamage( prev, damage );
						}
					}
					else
					{
						float tempY = start.y;
						landObj->GetY( start.x,tempY,start.z);
						if ( tempY > start.y )
						{
							range = 0;
							exploding = true;
							explosion.Start(etFire);
							if ( app!=NULL )
							{
								app->CalculateExplosionDamage( prev, damage );
							}
						}
						else
						{

							size_t i;

							TPlane* planes = params.planes;
							size_t numPlanes = params.numPlanes;
							for ( i=0; i<numPlanes; i++ )
							{
								if ( planes[i].Strength()>0 )
								{
									if ( planes[i].InsideVehicle( start ) )
									{
										range = 0;
										planes[i].Strength( planes[i].Strength() - 1 );
									}
								}
							}

							TTank* tanks = params.tanks;
							size_t numTanks = params.numTanks;
							for ( i=0; i<numTanks; i++ )
							{
								if ( tanks[i].Strength()>0 )
								{
									if ( tanks[i].InsideVehicle( start ) )
									{
										range = 0;
										tanks[i].Strength( tanks[i].Strength() - 1 );
									}
								}
							}

							TFlak* flak = params.flak;
							size_t numFlak = params.numFlak;
							for ( i=0; i<numFlak; i++ )
							{
								if ( flak[i].Strength()>0 )
								{
									if ( flak[i].InsideVehicle( start ) )
									{
										range = 0;
										flak[i].Strength( flak[i].Strength() - 1 );
									}
								}
							}
						
							TArtillery* artillery = params.artillery;
							size_t numArtillery = params.numArtillery;
							for ( i=0; i<numArtillery; i++ )
							{
								if ( artillery[i].Strength()>0 )
								{
									if ( artillery[i].InsideVehicle( start ) )
									{
										range = 0;
										artillery[i].Strength( artillery[i].Strength() - 1 );
									}
								}
							}

						}
					}
				}

				prev = start;
				if ( range>0 )
					range--;
				if ( range==0 )
				{
					exploding = true;
					explosion.Start(etFire); // not a full explosion
					if ( app!=NULL )
						app->CalculateExplosionDamage( prev, damage );
				}
				else
				{
					start = start + dirn;
					firstTime = false;
				}
				cntr--;
			}
		}
		else
		{
			explosion.Logic();
			if ( explosion.Finished() )
			{
				inUse = false;
			}
		}
	}
};

void TProjectile::Draw( float cameraAngle, bool transp )
{
	if ( !exploding )
	{
		if ( graphic!=NULL )
		{
			glPushMatrix();
				glTranslatef( start.x, start.y, start.z );
				glScalef( scale, scale, scale );
				glRotatef( yangle, 0,1,0 );
				glRotatef( xangle, 1,0,0 );
				graphic->Draw();
			glPopMatrix();
		}
	}
	else
	{
		explosion.SetXYZ( prev );
		explosion.Draw( cameraAngle, transp );
	}
};

TPoint& TProjectile::StartPoint( void )
{
	return start;
};

void TProjectile::StartPoint( const TPoint& _start )
{
	start = _start;
	prev = _start;
};

TPoint& TProjectile::Dirn( void )
{
	return dirn;
};

void TProjectile::Dirn( const TPoint& _dirn, float cameraAngle )
{
	dirn = _dirn;

	// calculate y and x angles from dirn vector
	yangle = cameraAngle;
	xangle = 0;
	if ( dirn.x==0 && dirn.z==0 )
	{
		if ( dirn.y > 0 )
		{
			xangle = 90;
		}
		else if ( dirn.y < 0 )
		{
			xangle = -90;
		}
	}
	else
	{
		float xzsize = float(sqrt(dirn.x*dirn.x+dirn.z*dirn.z));
		if ( xzsize>0 )
		{
			xangle = float(atan( dirn.y / xzsize )) * rad2Deg;
		}
	}
};

size_t TProjectile::netSet( size_t myId, byte* buf ) const
{
	PreCond( app!=NULL );
	PreCond( buf!=NULL );

	buf[0] = id_projectile;

	buf[1] = myId;
	buf[2] = graphicId;
	buf[3] = range;

	float* f = (float*)&buf[4];
	f[0] = start.x;
	f[1] = start.y;
	f[2] = start.z;

	f[3] = dirn.x;
	f[4] = dirn.y;
	f[5] = dirn.z;

	// zero for now - so we don't damage -
	// and use the server to retransmit actual
	// damages
//	f[6] = damage;
	f[6] = 0;

	return netSize();
};

size_t TProjectile::netSize( void ) const
{
	return (4 + sizeof(float)*7);
};

size_t TProjectile::netGet( const byte* buf )
{
	PreCond( app!=NULL );
	PreCond( buf!=NULL );
	PreCond( buf[0]==id_projectile );

	graphicId = buf[2];
	graphic = app->ProjectileNameToBin( graphicId );
	range = buf[3];

	float* f = (float*)&buf[4];
	start.x = f[0];
	start.y = f[1];
	start.z = f[2];

	dirn.x = f[3];
	dirn.y = f[4];
	dirn.z = f[5];

	float damage = f[6];

	return netSize();
}

//==========================================================================

