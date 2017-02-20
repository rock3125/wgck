#include <precomp_header.h>

#include <object/smoke.h>

//=============================================================

TSmoke TSmoke::list[kMaxSmoke];

//=============================================================

bool TSmoke::AddNewParticle( float x, float y, float z, 
							 float dx, float dy )
{
	size_t i=0;
	bool found = false;
	while (i<kMaxSmoke && !found)
	{
		if ( !list[i].InUse() )
		{
			list[i].SetPos(x,y,z);
			list[i].SetDirection(dx,dy);
			list[i].SetTTL( kTimeToLive );
			list[i].col = (unsigned char)(float(rand())/float(RAND_MAX)*64.0f);
			found = true;
		}
		else
			i++;
	}
	return found;
};


void TSmoke::DrawSmoke( void )
{
	glEnable( GL_COLOR_MATERIAL );
	for ( size_t i=0; i<kMaxSmoke; i++ )
		if ( list[i].InUse() )
			list[i].Draw();
	glDisable( GL_COLOR_MATERIAL );
};


void TSmoke::AnimateSmoke( void )
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
};


TSmoke::~TSmoke( void )
{
	ttl = 0;
};


void TSmoke::Draw( void )
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
	x += (dx*0.01f);
	z += (dz*0.01f);
	y += 0.1f;
	size = size - 0.0025f;
	cntr++;
	ttl--;
};


void TSmoke::SetTTL( size_t _ttl )
{
	ttl = _ttl;
	cntr = 0;
	size = 0.1f;
};


void TSmoke::SetPos( float _x, float _y, float _z )
{
	x = _x + (float(rand())/float(RAND_MAX))*0.4f;
	y = _y + (float(rand())/float(RAND_MAX))*0.4f;
	z = _z + (float(rand())/float(RAND_MAX))*0.4f;
};


void TSmoke::SetDirection( float _dx, float _dz )
{
	dx = _dx;
	dz = _dz;
};


bool TSmoke::InUse( void )
{
	return (ttl>0);
};

//=============================================================

