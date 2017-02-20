#include <precomp_header.h>

#include <win32/win32.h>
#include <object/object.h>
#include <object/tank.h>
#include <object/geometry.h>

//==========================================================================

const float kPI = 3.1415927f;
const float degToRad = 0.01745329252033f;
const float radToDeg = 57.2957795112730f;

//==========================================================================

TTank::TTank( const float _shellRange )
	: body(NULL),
	  turret(NULL),
	  barrel(NULL),
	  exhaust(NULL)
{
	initialised = false;
	turretAngle = 0;
	barrelAngle = 0;
	shellRange = _shellRange;

	explode = false;

	targetX = 0;
	targetY = 0;
	targetZ = 0;
};


TTank::~TTank( void )
{
	body = NULL;
	turret = NULL;
	barrel = NULL;

	initialised = false;
};


bool TTank::LoadBinary( const TString& fname, TString& errStr, 
						const TString& pathname,
						const TString& texturePath,
						TAnimation* _exhaust )
{
	exhaust = _exhaust;

	bool success = TVehicle::LoadBinary(fname,errStr,pathname,texturePath);

	if ( success )
	{
		if ( object.NumMeshes(currentIndex)!=3 )
		{
			errStr = "Tank object must have 3 meshes";
			return false;
		}
		bool foundBody = false;
		bool foundBarrel = false;
		bool foundTurret = false;
		size_t i;
		for ( i=0; i<3; i++ )
		{
			TMesh* m = object.GetMesh(currentIndex,i);
			PostCond( m!=NULL );
			PostCond( m->Name()!=NULL );
			if ( stricmp(m->Name().c_str(),"body")==0 )
			{
				foundBody = true;
				body = m;
			}
			if ( stricmp(m->Name().c_str(),"barrel")==0 )
			{
				foundBarrel = true;
				barrel = m;
			}
			if ( stricmp(m->Name().c_str(),"turret")==0 )
			{
				foundTurret = true;
				turret = m;
			}
		}
		if ( !foundBody || !foundBarrel || !foundTurret )
		{
			errStr = "Tank object missing 'body', 'barrel', or 'turret'";
			return false;
		}

		float mix,miy,miz,max,may,maz;
		turret->GetBoundaries( mix,miy,miz,max,may,maz );
		turretX = (mix+max) * 0.5f;
		turretY = (miy+may) * 0.5f;
		turretZ = (miz+maz) * 0.5f;

		// re-offset the turret to 0,0,0 relative to turretX,Y,Z
		float *vs = turret->Vertices();
		size_t cnt = turret->NumVertices();
		for ( i=0; i<cnt; i++ )
		{
			vs[i*3+0] = vs[i*3+0] - turretX;
			vs[i*3+1] = vs[i*3+1] - turretY;
			vs[i*3+2] = vs[i*3+2] - turretZ;
		}

		barrel->GetBoundaries( mix,miy,miz,max,may,maz );
		barrelX = (mix+max) * 0.5f;
		barrelY = (miy+may) * 0.5f;
		barrelZ = maz; //(miz+maz) * 0.5f;

		// re-offset the barrel to 0,0,0 relative to barrelX,Y,Z
		vs = barrel->Vertices();
		cnt = barrel->NumVertices();
		for ( i=0; i<cnt; i++ )
		{
			vs[i*3+0] = vs[i*3+0] - (barrelX+turretX);
			vs[i*3+1] = vs[i*3+1] - (barrelY+turretY);
			vs[i*3+2] = vs[i*3+2] - (barrelZ+turretZ);
		}

		initialised = true;
	}
	return success;
};


void TTank::Move( TLandscape& landObj, 
				  size_t key, float speed,
				  bool canFire, bool _showTarget )
{
	TVehicle::Move(landObj,key,speed);

	showTarget = _showTarget;

	// precalculate angles and mutlipliers
	yang = -(-object.Yangle(currentIndex) + 90.0f + turretAngle + 180.0f) * degToRad;

	float dy = ((y1+y2)*0.5f-(y4+y3)*0.5f);
	xang = float( -atan(dy/(object.SizeX(currentIndex)*2)) * cos(yang) ) + barrelAngle*degToRad;
	dy = y1-y2;
	zang = float( atan(dy/(object.SizeZ(currentIndex)*2)) *sin(yang) ) + barrelAngle*degToRad;

	xmult = float(cos(yang));
	ymult = float(-sin(xang));
	zmult = float(sin(yang));

	// fire?  => calculate target position
	if ( (key&aFire)>0 && canFire )
	{
		GetTarget( landObj,targetX, targetY, targetZ );
	}

	if ( exhaust!=NULL )
	{
		bool engineOn = ((key&15)!=0);
		if ( engineOn && exhaust->AnimationDone() )
		{
			exhaust->StartAnimation(4);
		}
		else if ( !engineOn )
			exhaust->AnimationDone( true );

		// get exhaust position
		exhaust->SetOrientationToVehicle( *this );
		exhaust->Yangle( exhaust->Yangle()+90.0f );
	}
};


void TTank::Draw( bool exploding ) const
{
	if ( !initialised )
		return;

	if ( exploding )
	{
		TVehicle::Draw();
	}
	else
	{
		// obj can be considered to be at 0,0 at this stage
		glPushMatrix();

			glTranslatef( mx,
						  my,
 						  mz );

			glRotatef( ya, 0,1,0);
			glRotatef( xa, 0,0,1 );
			glRotatef( za, 1,0,0 );

			body->Draw();

			glPushMatrix();
			glTranslatef( turretX, turretY, turretZ );
			glRotatef( turretAngle, 0,1,0 );
			turret->Draw();
			glRotatef( barrelAngle, 0,0,1 );
			glTranslatef( barrelX, barrelY, barrelZ );
			barrel->Draw();


			glPopMatrix();

		glPopMatrix();

		// draw the tank's exhaust
		if ( exhaust!=NULL )
		{
			exhaust->MX( EX3() );
			exhaust->MZ( EZ3() );
			exhaust->MY( Y3() + 1.0f );
			exhaust->Draw();

			exhaust->MX( EX4() );
			exhaust->MZ( EZ4() );
			exhaust->MY( Y4() + 1.0f );
			exhaust->Draw();
		}

		if ( showTarget )
			ShowTargeting();
	}
};


void TTank::ShowTargeting( void ) const
{
	float x,y,z;

	// plot trajectory course
	glColor3ub(255,255,255);
	float gravity = 0.09f;
	glBegin( GL_POINTS );
		float initDist = 2.0f;
		float dist = initDist;
		float h = 0.0f;
		size_t cntr = 33;
		while ( cntr>0 )
		{
			x = mx+dist*xmult;
			y = my+turretY+dist*ymult;
			z = mz+dist*zmult;

			glVertex3f( x,y+h,z );
			dist += shellRange*4;
			h = h - gravity*gravity*(dist-initDist)*4;
			cntr--;
		}
	glEnd();
};


void TTank::GetTarget( TLandscape& land,
					   float& x, float& y, float& z ) const
{
	size_t bulletLoc = location1;
	float gravity = 0.09f;
	float initDist = 2.0f;
	float dist = initDist;
	float h = 0.0f;
	bool found = false;
	size_t cntr = 0;
	while ( !found && cntr<1000 )
	{
		x = mx+dist*xmult;
		y = my+turretY+dist*ymult + h;
		z = mz+dist*zmult;

		float height;
		if ( land.WaterLevel()>=y )
		{
			y = land.WaterLevel();
			found = true;
		}
		else if ( land.InsideObstacle(x,y,z) )
		{
			found = true;
		}
		else if ( land.GetLORTriangle( x, height, z, bulletLoc ) )
		{
			bulletLoc = land.lorReference;
			if ( height>=y )
			{
				y = height;
				found = true;
			}
		}
		dist += shellRange*4;
		h = h - gravity*gravity*(dist-initDist)*4;
		cntr++;
	}
	PostCond( found );
};


const float& TTank::TurretAngle( void ) const
{
	return turretAngle;
};


void TTank::TurretAngle( const float& a)
{
	turretAngle = a;
};


const float& TTank::BarrelAngle( void ) const
{
	return barrelAngle;
};


void TTank::BarrelAngle( const float& a)
{
	barrelAngle = a;
};


const float& TTank::TargetX( void ) const
{
	return targetX;
};


void TTank::TargetX( const float& t )
{
	targetX = t;
};


const float& TTank::TargetY( void ) const
{
	return targetY;
};


const float& TTank::TargetZ( void ) const
{
	return targetZ;
};


