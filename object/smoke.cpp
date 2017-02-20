#include <precomp_header.h>

#include <object/smoke.h>

//=============================================================

TSmoke TSmoke::list[kMaxSmoke];

//=============================================================

bool TSmoke::Add( float x, float y, float z, 
				  float dx, float dy, float dz )
{
	size_t i=0;
	bool found = false;
	while (i<kMaxSmoke && !found)
	{
		if ( !list[i].InUse() )
		{
			list[i].SetPos(x,y,z);
			list[i].SetDirection(dx,dy,dz);
			list[i].SetTTL( kTimeToLive );
			list[i].col = (unsigned char)(Random()*64.0f);
			found = true;
		}
		else
			i++;
	}
	return found;
};


void TSmoke::Draw( void )
{
	glEnable( GL_COLOR_MATERIAL );
	for ( size_t i=0; i<kMaxSmoke; i++ )
		if ( list[i].InUse() )
			list[i].DrawSingle();
	glDisable( GL_COLOR_MATERIAL );
};


void TSmoke::Logic( void )
{
	for ( size_t i=0; i<kMaxSmoke; i++ )
		if ( list[i].InUse() )
		{
			list[i].Animate();
			list[i].ttl--;
		}
};

//=============================================================

TSmoke::TSmoke( void )
{
	ttl = 0;
	col = 0;

	size = 0;
	cntr = 0;

	x = y = z = 0;
	dx = dy = dz = 0;
};


TSmoke::~TSmoke( void )
{
	ttl = 0;
};


void TSmoke::DrawSingle( void )
{
	glPushMatrix();
		glTranslatef(x,y,z);
		glColor3ub(col,col,col);
		glBegin( GL_TRIANGLES );
			if ( (cntr%2)==0 )
			{
				glVertex3f(-size,-size,0);
				glVertex3f( size,-size,0);
				glVertex3f(0,size,0);

				glVertex3f(0,size,0);
				glVertex3f( size,-size,0);
				glVertex3f(-size,-size,0);

				glVertex3f(0,-size,-size);
				glVertex3f(0,size,-size);
				glVertex3f(0,0,size);

				glVertex3f(0,0,size);
				glVertex3f(0,size,-size);
				glVertex3f(0,-size,-size);
			}
			else
			{
				glVertex3f( size, size,0);
				glVertex3f(-size, size,0);
				glVertex3f(0,-size,0);

				glVertex3f(0,-size,0);
				glVertex3f(-size, size,0);
				glVertex3f( size, size,0);

				glVertex3f(0,size,size);
				glVertex3f(0,-size,size);
				glVertex3f(0,0,-size);

				glVertex3f(0,0,-size);
				glVertex3f(0,-size,size);
				glVertex3f(0,size,size);
			}
		glEnd();
	glPopMatrix();
};


void TSmoke::Animate( void )
{
	x += (dx*0.1f);
	y += (dy*0.1f);
	z += (dz*0.1f);
	size = size - 0.0025f;
	cntr++;
	ttl--;
};


void TSmoke::SetTTL( size_t _ttl )
{
	ttl = _ttl;
	cntr = 0;
	size = 0.08f;
};


void TSmoke::SetPos( float _x, float _y, float _z )
{
	x = _x + Random()*0.4f;
	y = _y + Random()*0.4f;
	z = _z + Random()*0.4f;
};


void TSmoke::SetDirection( float _dx, float _dy, float _dz )
{
	dx = _dx;
	dy = _dy;
	dz = _dz;
};


bool TSmoke::InUse( void )
{
	return (ttl>0);
};

//=============================================================

