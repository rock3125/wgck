#include <precomp_header.h>

#include <object/geometry.h>

//==========================================================================

const float degToRad = 0.0174532925f;
const float radToDeg = 57.295779511f;

//==========================================================================

TPoint::TPoint( void )
{
	x = 0;
	y = 0;
	z = 0;
};

TPoint::TPoint( float _x, float _y, float _z )
{
	x = _x;
	y = _y;
	z = _z;
}

TPoint::TPoint( float* v )
{
	if ( v!=NULL )
	{
		x = v[0];
		y = v[1];
		z = v[2];
	}
	else
	{
		x = 0;
		y = 0;
		z = 0;
	}
}

TPoint::TPoint( const TPoint& _point )
{
	x = _point.x;
	y = _point.y;
	z = _point.z;
};


TPoint::~TPoint( void )
{
	x = 0;
	y = 0;
	z = 0;
};

const TPoint& TPoint::operator =( const TPoint& _point )
{
	x = _point.x;
	y = _point.y;
	z = _point.z;

	return *this;
};

const TPoint& TPoint::operator +( const TPoint& a )
{
	x += a.x;
	y += a.y;
	z += a.z;
	return *this;
};

const TPoint& TPoint::operator +=( const TPoint& a )
{
	x += a.x;
	y += a.y;
	z += a.z;
	return *this;
};

const TPoint& TPoint::operator -( const TPoint& a )
{
	x -= a.x;
	y -= a.y;
	z -= a.z;
	return *this;
};

const TPoint& TPoint::operator -=( const TPoint& a )
{
	x -= a.x;
	y -= a.y;
	z -= a.z;
	return *this;
};

const TPoint& TPoint::operator *( float scalar )
{
	x *= scalar;
	y *= scalar;
	z *= scalar;
	return *this;
};

TPoint operator+( const TPoint& p1, const TPoint& p2 )
{
	TPoint p;
	p = p1;
	p += p2;
	return p;
};

//==========================================================================


TMatrix::TMatrix( void )
{
	ismdl = false;
	Identity();
};

TMatrix::TMatrix( const TMatrix& _m )
{
	operator=(_m);
}

const TMatrix& TMatrix::operator=( const TMatrix& _m )
{
	CopyMemory( m, _m.m, 16*sizeof(float) );
	ismdl = _m.ismdl;
	return *this;
};

void TMatrix::Identity( void )
{
	ismdl = false;
	ZeroMemory( m, 16*sizeof(float) );
	m[0] = 1;
	m[5] = 1;
	m[10] = 1;
	m[15] = 1;
};

float* TMatrix::Matrix( void )
{
	return m;
};

void TMatrix::Matrix( float* _m )
{
	ismdl = false;
	CopyMemory( m, _m, 16*sizeof(float) );
};

float* TMatrix::MDLMatrix( void )
{
	if ( !ismdl )
	{
		ismdl = true;
		float temp[16];
		CopyMemory( temp, m, 16*sizeof(float) );
		size_t index = 0;
		for ( size_t i=0; i<4; i++ )
		{
			m[index++] = temp[i*4];
			m[index++] = temp[i*4+1];
			m[index++] = temp[i*4+2];
			m[index++] = temp[i*4+3];
		}
	}
	return m;
};

void TMatrix::MDLMatrix( float* _m )
{
	ismdl = true;
	CopyMemory( m, _m, 16*sizeof(float) );
};

void TMatrix::SetTranslation( const float* t )
{
	m[12] = t[0];
	m[13] = t[1];
	m[14] = t[2];
};

void TMatrix::GetTranslation( float* t )
{
	t[0] = m[12];
	t[1] = m[13];
	t[2] = m[14];
};

void TMatrix::AddTranslation( const float* t )
{
	m[12] += t[0];
	m[13] += t[1];
	m[14] += t[2];
};

void TMatrix::OpenGLTransformToMatrix( float tx, float ty, float tz,
									   float rx, float ry, float rz,
									   float sx, float sy, float sz )
{
	glPushMatrix();

	glLoadIdentity();
	glTranslatef( tx,ty,tz );
			
	glRotatef( rz, 0,0,1);
	glRotatef( ry, 0,1,0 );
	glRotatef( rx, 1,0,0 );

	glScalef( sx,sy,sz );
		
	glGetFloatv( GL_MODELVIEW_MATRIX, m );

	glPopMatrix();
};

void TMatrix::Mult( const float* v1, float* result )
{
	result[0] = (v1[0]*m[0] + v1[1]*m[4] + v1[2]*m[8] + m[12]);
	result[1] = (v1[0]*m[1] + v1[1]*m[5] + v1[2]*m[9] + m[13]);
	result[2] = (v1[0]*m[2] + v1[1]*m[6] + v1[2]*m[10] + m[14]);
};

void TMatrix::Mult( const float* v1, float scale, float* result )
{
	result[0] = (v1[0]*m[0] + v1[1]*m[4] + v1[2]*m[8])*scale + m[12];
	result[1] = (v1[0]*m[1] + v1[1]*m[5] + v1[2]*m[9])*scale + m[13];
	result[2] = (v1[0]*m[2] + v1[1]*m[6] + v1[2]*m[10])*scale + m[14];
};

void TMatrix::Mult( const TPoint& v1, TPoint& result )
{
	result.x = (v1.x*m[0] + v1.y*m[4] + v1.z*m[8] + m[12]);
	result.y = (v1.x*m[1] + v1.y*m[5] + v1.z*m[9] + m[13]);
	result.z = (v1.x*m[2] + v1.y*m[6] + v1.z*m[10] + m[14]);
};

TPoint TMatrix::Mult( const TPoint& v1 )
{
	TPoint result;
	result.x = (v1.x*m[0] + v1.y*m[4] + v1.z*m[8] + m[12]);
	result.y = (v1.x*m[1] + v1.y*m[5] + v1.z*m[9] + m[13]);
	result.z = (v1.x*m[2] + v1.y*m[6] + v1.z*m[10] + m[14]);
	return result;
};

TPoint TMatrix::Mult( const TPoint& v1, float scale )
{
	TPoint result;
	result.x = (v1.x*m[0] + v1.y*m[4] + v1.z*m[8])*scale + m[12];
	result.y = (v1.x*m[1] + v1.y*m[5] + v1.z*m[9])*scale + m[13];
	result.z = (v1.x*m[2] + v1.y*m[6] + v1.z*m[10])*scale + m[14];
	return result;
};

void TMatrix::Mult( const TPoint& v1, float scale, TPoint& result )
{
	result.x = (v1.x*m[0] + v1.y*m[4] + v1.z*m[8])*scale + m[12];
	result.y = (v1.x*m[1] + v1.y*m[5] + v1.z*m[9])*scale + m[13];
	result.z = (v1.x*m[2] + v1.y*m[6] + v1.z*m[10])*scale + m[14];
};

void TMatrix::MDLMult( const TPoint& v1, TPoint& result )
{
	result.x = (v1.x*m[0] + v1.y*m[1] + v1.z*m[2] + m[3]);
	result.y = (v1.x*m[4] + v1.y*m[5] + v1.z*m[6] + m[7]);
	result.z = (v1.x*m[8] + v1.y*m[9] + v1.z*m[10] + m[11]);
};

void TMatrix::SetTranslation( const TPoint& t )
{
	m[12] = t.x;
	m[13] = t.y;
	m[14] = t.z;
};

void TMatrix::GetTranslation( TPoint& t )
{
	t.x = m[12];
	t.y = m[13];
	t.z = m[14];
};

void TMatrix::AddTranslation( const TPoint& t )
{
	m[12] += t.x;
	m[13] += t.y;
	m[14] += t.z;
};

bool TMatrix::IsMDL( void ) const
{
	return ismdl;
};

void TMatrix::IsMDL( bool _ismdl )
{
	ismdl = _ismdl;
};

//==========================================================================

//
//  get the angle between two vectors (x1,y1) and (x2,y2) in degrees
//  direction going along (x1,y1) -> (x2,y2)
//
float VectorsToAngle( float x1, float y1, float x2, float y2 )
{
    float angle,dx,dy;

    dx = (x2-x1);
    dy = (y2-y1);
    if (dy==0)
    {
        angle = 0;
        if ( dx<0 )
            angle = 180.0f;
    }
	else if ( dx==0 )
	{
		angle = 90.0f;
		if ( dy<0 )
			angle = 270.0f;
	}
    else
    {
        angle=(float)atan(dy/dx);
        angle = angle * radToDeg;

		if ( angle<0 )
		{
			if ( dy>0 )
				angle = 180.0f + angle;
			else
				angle = 360.0f + angle;
		}
		else
		{
			if ( dy<0 )
				angle = angle + 180.0f;
		}

	    if (angle>360) angle-=360;
	    if (angle<0)   angle+=360;
    }
    return angle;
}


float AngleBetweenVectors( float t1x, float t1y, float t1z,
						   float t2x, float t2y, float t2z )
{
	float len;
	len = float( sqrt(t1x*t1x+t1y*t1y+t1z*t1z) * 
				 sqrt(t2x*t2x+t2y*t2y+t2z*t2z) );
	if ( len > 0.0f )
		return float(acos((t1x*t2x+t1y*t2y+t1z*t2z)/len));
	else
		return 0.0f;
};


// calculate normal to 3 points and return into nx,ny,nz
void Normal( float& nx, float& ny, float& nz,
			 float v1x, float v1y, float v1z,
			 float v2x, float v2y, float v2z,
			 float v3x, float v3y, float v3z )
{

	nx = v1y * ( v2z - v3z ) + v2y * ( v3z - v1z ) + v3y * ( v1z - v2z );
	ny = v1z * ( v2x - v3x ) + v2z * ( v3x - v1x ) + v3z * ( v1x - v2x );
	nz = v1x * ( v2y - v3y ) + v2x * ( v3y - v1y ) + v3x * ( v1y - v2y );
}

	
// calculate normal to 3 points and return into nx,ny,nz
void Normal( float& nx, float& ny, float& nz,
			 const float* v1, const float* v2, const float* v3 )
{

	nx = v1[1] * ( v2[2] - v3[2] ) + v2[1] * ( v3[2] - v1[2] ) + v3[1] * ( v1[2] - v2[2] );
	ny = v1[2] * ( v2[0] - v3[0] ) + v2[2] * ( v3[0] - v1[0] ) + v3[2] * ( v1[0] - v2[0] );
	nz = v1[0] * ( v2[1] - v3[1] ) + v2[0] * ( v3[1] - v1[1] ) + v3[0] * ( v1[1] - v2[1] );
}


inline	
void Normal( float* n, const float* v1, const float* v2, const float* v3 )
{

	n[0] = v1[1] * ( v2[2] - v3[2] ) + v2[1] * ( v3[2] - v1[2] ) + v3[1] * ( v1[2] - v2[2] );
	n[1] = v1[2] * ( v2[0] - v3[0] ) + v2[2] * ( v3[0] - v1[0] ) + v3[2] * ( v1[0] - v2[0] );
	n[2] = v1[0] * ( v2[1] - v3[1] ) + v2[0] * ( v3[1] - v1[1] ) + v3[0] * ( v1[1] - v2[1] );
}

	
inline
bool PointOnPlane( float x, float& y, float z,
				   const float* v1, const float* v2, const float* v3 )
{
	float n[3];

	Normal( n, v1,v2,v3 );
	if ( n[1]==0.0f )
		return false;

	y = (-n[0] * (x-v1[0]) - n[2] * (z-v1[2])) * (1/n[1]) + v1[1];
	return true;
};



//
// return the point where two lines intersect
// lines formed by PS and RQ, return results in (xi,yi,zi)
// or false if not possible
//
inline
bool IntersectLines( float sx, float sy, float sz,
					 float px, float py, float pz,
					 float qx, float qy, float qz,
					 float rx, float ry, float rz,
					 float& xi, float& yi, float& zi )
{
	float s,a,b,c;

	float sxpx = sx-px;
	float rzqz = rz-qz;
	float szpz = sz-pz;
	float pzqz = pz-qz;
	float qxpx = qx-px;
	float rxqx = rx-qx;
	float pxqx = px-qx;
	float qzpz = qz-pz;
	if ( sxpx!=0 && rzqz!=0 )
	{
		float e = 1 / rzqz;
		float f = 1 / sxpx;
		a = (pzqz) * e;
		b = ((szpz) * e) * ((qxpx) * f);
		c = ((szpz) * e) * ((rxqx) * f);

		if ( (1-c)==0 )
			return false;

		s = (a+b) / (1-c);
	}
	else if ( (szpz)!=0 && (rxqx)!=0 )
	{
		float e = 1 / rxqx;
		float f = 1 / szpz;
		a = (pxqx) * e;
		b = ((sxpx) * e) * ((qzpz) * f);
		c = ((sxpx) * e) * ((rzqz) * f);

		if ( (1-c)==0 )
			return false;

		s = (a+b) / (1-c);
	}
	else
	{
		float ryqy = ry-qy;
		float sypy = sy-py;
		float pyqy = py-qy;
		if ( sxpx!=0 && ryqy!=0 )
		{
			float e = 1 / ryqy;
			float f = 1 / sxpx;

			a = (pyqy) * e;
			b = ((sypy) * e) * ((qxpx) * f);
			c = ((sypy) * e) * ((rxqx) * f);

			if ( (1-c)==0 )
				return false;
			
			s = (a+b) / (1-c);
		}
		else if ( (szpz)!=0 && (ryqy)!=0 )
		{
			float e = 1 / ryqy;
			float f = 1 / szpz;

			a = (pyqy) * e;
			b = ((sypy) * e) * ((qzpz) * f);
			c = ((sypy) * e) * ((rzqz) * f);

			if ( (1-c)==0 )
				return false;

			s = (a+b) / (1-c);
		}
		else
			return false;
	}

	xi = qx + (rx-qx) * s;
	yi = qy + (ry-qy) * s;
	zi = qz + (rz-qz) * s;

	return true;
};


inline
bool IntersectLines( const float* s,
					 const float* p,
					 const float* q,
					 const float* r,
					 float* v )
{
	float a,b,c,d;

	float sxpx = s[0]-p[0];
	float rzqz = r[2]-q[2];
	float szpz = s[2]-p[2];
	float pzqz = p[2]-q[2];
	float qxpx = q[0]-p[0];
	float rxqx = r[0]-q[0];
	float pxqx = p[0]-q[0];
	float qzpz = q[2]-p[2];
	if ( sxpx!=0 && rzqz!=0 )
	{
		float e = 1 / rzqz;
		float f = 1 / sxpx;
		a = (pzqz) * e;
		b = ((szpz) * e) * ((qxpx) * f);
		c = ((szpz) * e) * ((rxqx) * f);

		if ( (1-c)==0 )
			return false;

		d = (a+b) / (1-c);
	}
	else if ( (szpz)!=0 && (rxqx)!=0 )
	{
		float e = 1 / rxqx;
		float f = 1 / szpz;
		a = (pxqx) * e;
		b = ((sxpx) * e) * ((qzpz) * f);
		c = ((sxpx) * e) * ((rzqz) * f);

		if ( (1-c)==0 )
			return false;

		d = (a+b) / (1-c);
	}
	else
	{
		float ryqy = r[1]-q[1];
		float sypy = s[1]-p[1];
		float pyqy = p[1]-q[1];
		if ( sxpx!=0 && ryqy!=0 )
		{
			float e = 1 / ryqy;
			float f = 1 / sxpx;
			a = (pyqy) * e;
			b = ((sypy) * e) * ((qxpx) * f);
			c = ((sypy) * e) * ((rxqx) * f);

			if ( (1-c)==0 )
				return false;
			
			d = (a+b) / (1-c);
		}
		else if ( (szpz)!=0 && (ryqy)!=0 )
		{
			float e = 1 / ryqy;
			float f = 1 / szpz;
			a = (pyqy) * e;
			b = ((sypy) * e) * ((qzpz) * f);
			c = ((sypy) * e) * ((rzqz) * f);

			if ( (1-c)==0 )
				return false;

			d = (a+b) / (1-c);
		}
		else
			return false;
	}

	v[0] = q[0] + (r[0]-q[0]) * d;
	v[1] = q[1] + (r[1]-q[1]) * d;
	v[2] = q[2] + (r[2]-q[2]) * d;

	return true;
};


inline
bool IntersectLines( const float* ps,
					 float px, float py, float pz,
					 const float* pq,
					 const float* pr,
					 float& xi, float& yi, float& zi )
{
	float s,a,b,c;

	float sxpx = ps[0]-px;
	float rzqz = pr[2]-pq[2];
	float szpz = ps[2]-pz;
	float pzqz = pz-pq[2];
	float qxpx = pq[0]-px;
	float rxqx = pr[0]-pq[0];
	float pxqx = px-pq[0];
	float qzpz = pq[2]-pz;
	float ryqy = pr[1]-pq[1];
	float pyqy = py-pq[1];
	float sypy = ps[1]-py;
	float qypy = pq[1]-py;
	if ( sxpx!=0 && rzqz!=0 )
	{
		float e = 1 / rzqz;
		float f = 1 / sxpx;
		a = (pzqz) * e;
		b = ((szpz) * e) * ((qxpx) * f);
		c = ((szpz) * e) * ((rxqx) * f);

		if ( (1-c)==0 )
			return false;

		s = (a+b) / (1-c);
	}
	else if ( szpz!=0 && rxqx!=0 )
	{
		float e = 1 / rxqx;
		float f = 1 / szpz;
		a = (pxqx) * e;
		b = ((sxpx) * e) * ((qzpz) * f);
		c = ((sxpx) * e) * ((rzqz) * f);

		if ( (1-c)==0 )
			return false;

		s = (a+b) / (1-c);
	}
	else if ( sxpx!=0 && ryqy!=0 )
	{
		float e = 1 / ryqy;
		float f = 1 / sxpx;
		a = (pyqy) * e;
		b = ((sypy) * e) * ((qxpx) * f);
		c = ((sypy) * e) * ((rxqx) * f);

		if ( (1-c)==0 )
			return false;

		s = (a+b) / (1-c);
	}
	else if ( sypy!=0 && rxqx!=0 )
	{
		float e = 1 / rxqx;
		float f = 1 / sypy;
		a = (pxqx) * e;
		b = ((sxpx) * e) * ((qypy) * f);
		c = ((sxpx) * e) * ((ryqy) * f);

		if ( (1-c)==0 )
			return false;

		s = (a+b) / (1-c);
	}
	else
	{
		float ryqy = pr[1]-pq[1];
		float sypy = ps[1]-py;
		float pyqy = py-pq[1];
		if ( sxpx!=0 && ryqy!=0 )
		{
			float e = 1 / ryqy;
			float f = 1 / sxpx;
			a = (pyqy) * e;
			b = ((sypy) * e) * ((qxpx) * f);
			c = ((sypy) * e) * ((rxqx) * f);

			if ( (1-c)==0 )
				return false;
			
			s = (a+b) / (1-c);
		}
		else if ( (szpz)!=0 && ryqy!=0 )
		{
			float e = 1 / ryqy;
			float f = 1 / szpz;
			a = (pyqy) * e;
			b = ((sypy) * e) * ((qzpz) * f);
			c = ((sypy) * e) * ((rzqz) * f);

			if ( (1-c)==0 )
				return false;

			s = (a+b) / (1-c);
		}
		else
			return false;
	}

	xi = pq[0] + (pr[0]-pq[0]) * s;
	yi = pq[1] + (pr[1]-pq[1]) * s;
	zi = pq[2] + (pr[2]-pq[2]) * s;

	return true;
};


inline
float Distance( float ax, float ay, float az,
				float bx, float by, float bz )
{
	return float( (bx-ax)*(bx-ax) + (by-ay)*(by-ay) + (bz-az)*(bz-az) );
};


inline
float Distance( const float* a, const float* b )
{
	return float( (b[0]-a[0])*(b[0]-a[0]) + (b[1]-a[1])*(b[1]-a[1]) + 
				  (b[2]-a[2])*(b[2]-a[2]) );
};


inline
bool IntersectionOK( float x, float y, float z,
					 float ax, float ay, float az,
					 float bx, float by, float bz,
					 float cx, float cy, float cz )
{
	float xi,yi,zi;

	if ( !IntersectLines( ax,ay,az, x,y,z, bx,by,bz, cx,cy,cz,  xi,yi,zi ) )
		return false;
	float a = Distance(ax,ay,az,x,y,z);
	float b = Distance(ax,ay,az,xi,yi,zi);
	if ( a > b )
		return false;

	if ( !IntersectLines( bx,by,bz, x,y,z, ax,ay,az, cx,cy,cz,  xi,yi,zi ) )
		return false;
	a = Distance(bx,by,bz,x,y,z);
	b = Distance(bx,by,bz,xi,yi,zi);
	if ( a > b )
		return false;

	if ( !IntersectLines( cx,cy,cz, x,y,z, ax,ay,az, bx,by,bz,  xi,yi,zi ) )
		return false;
	a = Distance(cx,cy,cz,x,y,z);
	b = Distance(cx,cy,cz,xi,yi,zi);
	if ( a > b )
		return false;

	return true;
};


inline
bool IntersectionOK( const float* v,
					 const float* a, const float* b, const float* c )
{
	float vi[3];

	if ( !IntersectLines( a, v, b, c,  vi ) )
		return false;
	float d1 = Distance(a,v);
	float d2 = Distance(a,vi);
	if ( d1 > d2 )
		return false;

	if ( !IntersectLines( b, v, a, c,  vi ) )
		return false;
	d1 = Distance(b,v);
	d2 = Distance(b,vi);
	if ( d1 > d2 )
		return false;

	if ( !IntersectLines( c, v, a, b,  vi ) )
		return false;
	d1 = Distance(c,v);
	d2 = Distance(c,vi);
	if ( d1 > d2 )
		return false;

	return true;
};


inline
bool IntersectionOK( float x, float y, float z,
					 const float* a, const float* b, const float* c )
{
	float xi,yi,zi;

	if ( !IntersectLines( a, x,y,z, b, c,  xi,yi,zi ) )
		return false;
	float da = Distance(a[0],a[1],a[2],x,y,z);
	float db = Distance(a[0],a[1],a[2],xi,yi,zi);
	if ( da > db )
		return false;

	if ( !IntersectLines( b, x,y,z, a, c,  xi,yi,zi ) )
		return false;
	da = Distance(b[0],b[1],b[2],x,y,z);
	db = Distance(b[0],b[1],b[2],xi,yi,zi);
	if ( da > db )
		return false;

	if ( !IntersectLines( c, x,y,z, a, b,  xi,yi,zi ) )
		return false;
	da = Distance(c[0],c[1],c[2],x,y,z);
	db = Distance(c[0],c[1],c[2],xi,yi,zi);
	if ( da > db )
		return false;

	return true;
};


// experimental:
// is p inside triangle v1,v2,v3 with normal n?
// this assumes that p is already on the plane defined by (v1,v2,v3)
//
// this method uses planes and normal equations to find whether a point p
// is on the same side of the 3 planes formed by v1,v2, and v3
//
// is p inside triangle v1,v2,v3 with normal n?
//
bool PointInsideTriangle( const float*p, const float* n, const float* _v1, 
						  const float* _v2, const float* _v3 )
{
	float nv1[3];
	float or[3];
	float v1[3], v2[3], v3[3];

	float cx,cy,cz;
	float scale = 1.1f; // adjustment to slightly enlarge triangle to make it work better

	cx = (_v1[0] + _v2[0] + _v3[0]) * 0.3333f;
	cy = (_v1[1] + _v2[1] + _v3[1]) * 0.3333f;
	cz = (_v1[2] + _v2[2] + _v3[2]) * 0.3333f;

	v1[0] = cx + ( _v1[0] - cx ) * scale;
	v1[1] = cy + ( _v1[1] - cy ) * scale;
	v1[2] = cz + ( _v1[2] - cz ) * scale;

	v2[0] = cx + ( _v2[0] - cx ) * scale;
	v2[1] = cy + ( _v2[1] - cy ) * scale;
	v2[2] = cz + ( _v2[2] - cz ) * scale;

	v3[0] = cx + ( _v3[0] - cx ) * scale;
	v3[1] = cy + ( _v3[1] - cy ) * scale;
	v3[2] = cz + ( _v3[2] - cz ) * scale;

	// nv1 is a normal, the cross product of n x v1v2
	// a x b = < a1b2 - a2b1, a2b0 - a0b2, a0b1 - a1b0 >
	float v1v2[3];
	v1v2[0] = v2[0] - v1[0];
	v1v2[1] = v2[1] - v1[1];
	v1v2[2] = v2[2] - v1[2];
	nv1[0] = n[1]*v1v2[2] - n[2]*v1v2[1];
	nv1[1] = n[2]*v1v2[0] - n[0]*v1v2[2];
	nv1[2] = n[0]*v1v2[1] - n[1]*v1v2[0];

	// plugin p into the normal equation with v1 to get an orientation
	or[0] = nv1[0] * (p[0] - v1[0]) + 
			nv1[1] * (p[1] - v1[1]) +
			nv1[2] * (p[2] - v1[2]);

	// same for v2v3
	float v2v3[3];
	v2v3[0] = v3[0] - v2[0];
	v2v3[1] = v3[1] - v2[1];
	v2v3[2] = v3[2] - v2[2];
	nv1[0] = n[1]*v2v3[2] - n[2]*v2v3[1];
	nv1[1] = n[2]*v2v3[0] - n[0]*v2v3[2];
	nv1[2] = n[0]*v2v3[1] - n[1]*v2v3[0];

	// plugin p into the normal equation with v1 to get an orientation
	or[1] = nv1[0] * (p[0] - v2[0]) + 
			nv1[1] * (p[1] - v2[1]) +
			nv1[2] * (p[2] - v2[2]);

	// early return?
	if ( (or[0]>=0 && or[1]<0) || (or[0]<=0 && or[1]>0) )
		return false;

	// same for v3v1
	float v3v1[3];
	v3v1[0] = v1[0] - v3[0];
	v3v1[1] = v1[1] - v3[1];
	v3v1[2] = v1[2] - v3[2];
	nv1[0] = n[1]*v3v1[2] - n[2]*v3v1[1];
	nv1[1] = n[2]*v3v1[0] - n[0]*v3v1[2];
	nv1[2] = n[0]*v3v1[1] - n[1]*v3v1[0];

	// plugin p into the normal equation with v1 to get an orientation
	or[2] = nv1[0] * (p[0] - v3[0]) + 
			nv1[1] * (p[1] - v3[1]) +
			nv1[2] * (p[2] - v3[2]);

	return ( ( or[0]<=0 && or[1]<=0 && or[2]<=0 ) ||
		     ( or[0]>=0 && or[1]>=0 && or[2]>=0 ) );
};


bool PointInsideTriangle( const float*p, const float* _v1, 
					      const float* _v2, const float* _v3 )
{
	float n[3];
	float nv1[3];
	float or[3];
	float v1[3], v2[3], v3[3];

	float cx,cy,cz;
	float scale = 1.1f; // adjustment to slightly enlarge triangle to make it work better

	Normal( n, _v1,_v2,_v3 );

	cx = (_v1[0] + _v2[0] + _v3[0]) * 0.3333f;
	cy = (_v1[1] + _v2[1] + _v3[1]) * 0.3333f;
	cz = (_v1[2] + _v2[2] + _v3[2]) * 0.3333f;

	v1[0] = cx + ( _v1[0] - cx ) * scale;
	v1[1] = cy + ( _v1[1] - cy ) * scale;
	v1[2] = cz + ( _v1[2] - cz ) * scale;

	v2[0] = cx + ( _v2[0] - cx ) * scale;
	v2[1] = cy + ( _v2[1] - cy ) * scale;
	v2[2] = cz + ( _v2[2] - cz ) * scale;

	v3[0] = cx + ( _v3[0] - cx ) * scale;
	v3[1] = cy + ( _v3[1] - cy ) * scale;
	v3[2] = cz + ( _v3[2] - cz ) * scale;

	// nv1 is a normal, the cross product of n x v1v2
	// a x b = < a1b2 - a2b1, a2b0 - a0b2, a0b1 - a1b0 >
	float v1v2[3];
	v1v2[0] = v2[0] - v1[0];
	v1v2[1] = v2[1] - v1[1];
	v1v2[2] = v2[2] - v1[2];
	nv1[0] = n[1]*v1v2[2] - n[2]*v1v2[1];
	nv1[1] = n[2]*v1v2[0] - n[0]*v1v2[2];
	nv1[2] = n[0]*v1v2[1] - n[1]*v1v2[0];

	// plugin p into the normal equation with v1 to get an orientation
	or[0] = nv1[0] * (p[0] - v1[0]) + 
			nv1[1] * (p[1] - v1[1]) +
			nv1[2] * (p[2] - v1[2]);

	// same for v2v3
	float v2v3[3];
	v2v3[0] = v3[0] - v2[0];
	v2v3[1] = v3[1] - v2[1];
	v2v3[2] = v3[2] - v2[2];
	nv1[0] = n[1]*v2v3[2] - n[2]*v2v3[1];
	nv1[1] = n[2]*v2v3[0] - n[0]*v2v3[2];
	nv1[2] = n[0]*v2v3[1] - n[1]*v2v3[0];

	// plugin p into the normal equation with v1 to get an orientation
	or[1] = nv1[0] * (p[0] - v2[0]) + 
			nv1[1] * (p[1] - v2[1]) +
			nv1[2] * (p[2] - v2[2]);

	// early return?
	if ( (or[0]>=0 && or[1]<0) || (or[0]<=0 && or[1]>0) )
		return false;

	// same for v3v1
	float v3v1[3];
	v3v1[0] = v1[0] - v3[0];
	v3v1[1] = v1[1] - v3[1];
	v3v1[2] = v1[2] - v3[2];
	nv1[0] = n[1]*v3v1[2] - n[2]*v3v1[1];
	nv1[1] = n[2]*v3v1[0] - n[0]*v3v1[2];
	nv1[2] = n[0]*v3v1[1] - n[1]*v3v1[0];

	// plugin p into the normal equation with v1 to get an orientation
	or[2] = nv1[0] * (p[0] - v3[0]) + 
			nv1[1] * (p[1] - v3[1]) +
			nv1[2] * (p[2] - v3[2]);

	return ( ( or[0]<=0 && or[1]<=0 && or[2]<=0 ) ||
		     ( or[0]>=0 && or[1]>=0 && or[2]>=0 ) );
};


// get the position of a point inside a triangle if possible, given its
// x and z.  y is calculated as the perpendicular projection of (x,z) onto
// the plane defined by t1,t2,t3.  If then (x,y,z) is inside the triangle
// formed by t1,t2,t3, return true.  Return false in all other cases.
bool PointInTriangle( float x, float &y, float z,
					  const float* t1, const float* t2, const float* t3 )
{
	// first find the x,z on the plane made by the triangle
	if ( PointOnPlane(x,y,z, t1,t2,t3) )
	{
		// then for two different parts of the triangle calculate the distance
		// between (xyz) and (t1+t2)/2 and where this line intersects either
		// vectors t2t3 or t1t3.  If this distance is greater - we are outside
		// the triangle by defn.
		return IntersectionOK(x,y,z, t1,t2,t3);
/*
		float p[3], n[3];
		p[0] = x;
		p[1] = y;
		p[2] = z;
		Normal( n, t1,t2,t3 );
		return PointInsideTriangle( p, n, t1,t2,t3 );
*/
	}
	return false;
};


inline
float minf( float a, float b, float c )
{
	if ( a < b )
	{
		if ( a < c )
			return a;
		else
			return c;
	}
	else if ( b < c )
	{
		return b;
	}
	else
	{
		return c;
	}
};

inline
float maxf( float a, float b, float c )
{
	if ( a > b )
	{
		if ( a > c )
			return a;
		else
			return c;
	}
	else if ( b > c )
	{
		return b;
	}
	else
	{
		return c;
	}
};


float DotProduct ( const float* v1, const float* v2 )
{
	return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
}

void ProjectPointOnPlane( float* dst, const float* p, const float* normal )
{
	float d;
	float n[3];
	float inv_denom;

	inv_denom = 1.0F / DotProduct( normal, normal );

	d = DotProduct( normal, p ) * inv_denom;

	n[0] = normal[0] * inv_denom;
	n[1] = normal[1] * inv_denom;
	n[2] = normal[2] * inv_denom;

	dst[0] = p[0] - d * n[0];
	dst[1] = p[1] - d * n[1];
	dst[2] = p[2] - d * n[2];
}


// return true if a line defined by l1,l2 intersects the triangle defined by
// t1,t2,t3
bool LineIntersectsTriangle2( const float* l1, const float* l2,
							  const float* t1, const float* t2, const float* t3 )
{
	float n[3];

	Normal( n, t1, t2, t3 );

	float div = n[0]*(l2[0]-l1[0]) + n[1]*(l2[1]-l1[1]) + n[2]*(l2[2]-l1[2]);
	if ( div==0 )
		return false;

	float top = -n[0]*l1[0] -n[1]*l1[1] -n[2]*l1[2] + n[0]*t1[0] + n[1]*t1[1] + n[2]*t1[2];

	// if normal for particular coordinate is zero,
	// make sure we pass through its plane at least
	if ( n[0]==0 )
	{
		float minx = l1[0];
		float maxx = l2[0];
		if ( l1[0] > l2[0] )
		{
			minx = l2[0];
			maxx = l1[0];
		}
		if ( !( minx <= t1[0] && t1[0] <= maxx ) )
			return false;
	}
	if ( n[1]==0 )
	{
		float miny = l1[1];
		float maxy = l2[1];
		if ( l1[1] > l2[1] )
		{
			miny = l2[1];
			maxy = l1[1];
		}
		if ( !( miny <= t1[1] && t1[1] <= maxy ) )
			return false;
	}
	if ( n[2]==0 )
	{
		float minz = l1[2];
		float maxz = l2[2];
		if ( l1[2] > l2[2] )
		{
			minz = l2[2];
			maxz = l1[2];
		}
		if ( !( minz <= t1[2] && t1[2] <= maxz ) )
			return false;
	}

	float t = top / div;

	// inside the line?
	if ( t<0 || t>1.0f )
		return false;

	// now we have a point where the two intersect - see if its
	// inside the triangle defined by t1,t2,t3
	float v[3];
	v[0] = l1[0] + (l2[0]-l1[0])*t;
	v[1] = l1[1] + (l2[1]-l1[1])*t;
	v[2] = l1[2] + (l2[2]-l1[2])*t;

	return IntersectionOK(v, t1, t2, t3);
//	return PointInsideTriangle( v, n, t1,t2,t3 );
};


// return true if a line defined by l1,l2 intersects the triangle defined by
// t1,t2,t3
bool LineIntersectsTriangle2( float l1x, float l1y, float l1z,
							 float l2x, float l2y, float l2z,
							 float* t1, float* t2, float* t3 )
{
	float n[3];

	Normal( n, t1, t2, t3 );

	// if normal for particular coordinate is zero,
	// make sure we pass through its plane at least
	if ( n[0]==0 )
	{
		float minx = l1x;
		float maxx = l2x;
		if ( l1x > l2x )
		{
			minx = l2x;
			maxx = l1x;
		}
		float mintx = t1[0];
		if ( mintx > t2[0] )
			mintx = t2[0];
		if ( mintx > t3[0] )
			mintx = t3[0];

		float maxtx = t1[0];
		if ( maxtx < t2[0] )
			maxtx = t2[0];
		if ( maxtx < t3[0] )
			maxtx = t3[0];

		if (  !( ( ( mintx<=minx && minx<=maxtx ) ||
			   ( mintx<=maxx && maxx<=maxtx ) ) ||
			   ( minx<mintx && maxx>maxtx ) ) )
			return false;
	}
	if ( n[1]==0 )
	{
		float miny = l1y;
		float maxy = l2y;
		if ( l1y > l2y )
		{
			miny = l2y;
			maxy = l1y;
		}
		
		float minty = t1[1];
		if ( minty > t2[1] )
			minty = t2[1];
		if ( minty > t3[1] )
			minty = t3[1];

		float maxty = t1[1];
		if ( maxty < t2[1] )
			maxty = t2[1];
		if ( maxty < t3[1] )
			maxty = t3[1];

		if (  !( ( ( minty<=miny && miny<=maxty ) ||
			   ( minty<=maxy && maxy<=maxty ) ) ||
			   ( miny<minty && maxy>maxty ) ) )
			return false;
	}
	if ( n[2]==0 )
	{
		float minz = l1z;
		float maxz = l2z;
		if ( l1z > l2z )
		{
			minz = l2z;
			maxz = l1z;
		}

		float mintz = t1[2];
		if ( mintz > t2[2] )
			mintz = t2[2];
		if ( mintz > t3[2] )
			mintz = t3[2];

		float maxtz = t1[2];
		if ( maxtz < t2[2] )
			maxtz = t2[2];
		if ( maxtz < t3[2] )
			maxtz = t3[2];

		if (  !( ( ( mintz<=minz && minz<=maxtz ) ||
			   ( mintz<=maxz && maxz<=maxtz ) ) ||
			   ( minz<mintz && maxz>maxtz ) ) )
			return false;
	}

	float div = n[0]*(l2x-l1x) + n[1]*(l2y-l1y) + n[2]*(l2z-l1z);
	if ( div==0 )
		return false;

	float top = -n[0]*l1x -n[1]*l1y -n[2]*l1z + n[0]*t1[0] + n[1]*t1[1] + n[2]*t1[2];
	float t = top / div;

	// inside the line?
	if ( t<0 || t>1.0f )
		return false;

	// now we have a point where the two intersect - see if its
	// inside the triangle defined by t1,t2,t3
/*
	float p[3];
	p[0] = l1x + (l2x-l1x)*t;
	p[1] = l1y + (l2y-l1y)*t;
	p[2] = l1z + (l2z-l1z)*t;
*/
	float x,y,z;
	x = l1x + (l2x-l1x)*t;
	y = l1y + (l2y-l1y)*t;
	z = l1z + (l2z-l1z)*t;

	return IntersectionOK(x,y,z, t1, t2, t3 );
//	return PointInsideTriangle( p, n, t1,t2,t3 );
};


// return true if a line defined by l1,l2 intersects the triangle defined by
// t1,t2,t3
bool LineIntersectsTriangle( float l1x, float l1y, float l1z,
							 float l2x, float l2y, float l2z,
							 float t1x, float t1y, float t1z,
							 float t2x, float t2y, float t2z,
							 float t3x, float t3y, float t3z,
							 float& y )
{
	float nx,ny,nz;
	float x,z;

	Normal( nx,ny,nz, t1x,t1y,t1z, t2x,t2y,t2z, t3x,t3y,t3z );

	float div = nx*(l2x-l1x) + ny*(l2y-l1y) + nz*(l2z-l1z);
	if ( div==0 )
		return false;

	float top = -nx*l1x -ny*l1y -nz*l1z + nx*t1x + ny*t1y + nz*t1z;

	float t = top / div;

	// inside the line?
	if ( t<0 || t>1.0f )
		return false;

	// now we have a point where the two intersect - see if its
	// inside the triangle defined by t1,t2,t3
	x = l1x + (l2x-l1x)*t;
	y = l1y + (l2y-l1y)*t;
	z = l1z + (l2z-l1z)*t;

	// invert ok into collision
	return IntersectionOK(x,y,z, t1x,t1y,t1z, t2x,t2y,t2z, t3x,t3y,t3z);
};


// return true if a line defined by l1,l2 intersects the triangle defined by
// t1,t2,t3
bool LineIntersectsTriangle( float l1x, float l1y, float l1z,
							 float l2x, float l2y, float l2z,
							 float* t1, float* t2, float* t3,
							 float& y )
{
	float nx,ny,nz;
	float x,z;

	Normal( nx,ny,nz, t1, t2, t3 );

	float div = nx*(l2x-l1x) + ny*(l2y-l1y) + nz*(l2z-l1z);
	if ( div==0 )
		return false;

	float top = -nx*l1x -ny*l1y -nz*l1z + nx*t1[0] + ny*t1[1] + nz*t1[2];

	float t = top / div;

	// inside the line?
	if ( t<0 || t>1.0f )
		return false;

	// now we have a point where the two intersect - see if its
	// inside the triangle defined by t1,t2,t3
	x = l1x + (l2x-l1x)*t;
	y = l1y + (l2y-l1y)*t;
	z = l1z + (l2z-l1z)*t;

	// invert ok into collision
	return IntersectionOK(x,y,z, t1, t2, t3 );
};


void DrawBoundingBox( float minX, float minY, float minZ,
					  float maxX, float maxY, float maxZ,
					  float r, float g, float b )
{
	glEnable( GL_COLOR_MATERIAL );
	glColor3f(r,g,b);
	glBegin( GL_LINES );

		glVertex3f( minX, minY, minZ );
		glVertex3f( maxX, minY, minZ );

		glVertex3f( maxX, minY, minZ );
		glVertex3f( maxX, maxY, minZ );

		glVertex3f( maxX, maxY, minZ );
		glVertex3f( minX, maxY, minZ );

		glVertex3f( minX, maxY, minZ );
		glVertex3f( minX, minY, minZ );

		glVertex3f( minX, minY, maxZ );
		glVertex3f( maxX, minY, maxZ );

		glVertex3f( maxX, minY, maxZ );
		glVertex3f( maxX, maxY, maxZ );

		glVertex3f( maxX, maxY, maxZ );
		glVertex3f( minX, maxY, maxZ );

		glVertex3f( minX, maxY, maxZ );
		glVertex3f( minX, minY, maxZ );

		glVertex3f( minX, minY, minZ );
		glVertex3f( minX, minY, maxZ );

		glVertex3f( maxX, minY, minZ );
		glVertex3f( maxX, minY, maxZ );

		glVertex3f( maxX, maxY, minZ );
		glVertex3f( maxX, maxY, maxZ );

		glVertex3f( minX, maxY, minZ );
		glVertex3f( minX, maxY, maxZ );

	glEnd();
};


void DrawBoundingBox( float x1, float y1, float z1,
					  float x2, float y2, float z2,
					  float x3, float y3, float z3,
					  float x4, float y4, float z4,
					  float x5, float y5, float z5,
					  float x6, float y6, float z6,
					  float x7, float y7, float z7,
					  float x8, float y8, float z8,
					  float r, float g, float b )
{
	glEnable( GL_COLOR_MATERIAL );
	glColor3f(r,g,b);
	glBegin( GL_LINES );
		glVertex3f( x1,y1,z1 );
		glVertex3f( x3,y3,z3 );

		glVertex3f( x3,y3,z3 );
		glVertex3f( x4,y4,z4 );

		glVertex3f( x4,y4,z4 );
		glVertex3f( x2,y2,z2 );

		glVertex3f( x2,y2,z2 );
		glVertex3f( x1,y1,z1 );

		glVertex3f( x1,y1,z1 );
		glVertex3f( x5,y5,z5 );

		glVertex3f( x5,y5,z5 );
		glVertex3f( x6,y6,z6 );

		glVertex3f( x6,y6,z6 );
		glVertex3f( x2,y2,z2 );

		glVertex3f( x6,y6,z6 );
		glVertex3f( x8,y8,z8 );

		glVertex3f( x8,y8,z8 );
		glVertex3f( x7,y7,z7 );

		glVertex3f( x7,y7,z7 );
		glVertex3f( x5,y5,z5 );

		glVertex3f( x7,y7,z7 );
		glVertex3f( x3,y3,z3 );

		glVertex3f( x8,y8,z8 );
		glVertex3f( x4,y4,z4 );
	glEnd();
};


bool PointInsideBox( float x, float y,
					 float x1, float y1, float x2, float y2,
					 float x3, float y3, float x4, float y4 )
{
	float v1[3],v2[3],v3[3],v4[3];

	v1[0] = x1;
	v1[1] = 0;
	v1[2] = y1;

	v2[0] = x2;
	v2[1] = 0;
	v2[2] = y2;

	v3[0] = x3;
	v3[1] = 0;
	v3[2] = y3;

	v4[0] = x4;
	v4[1] = 0;
	v4[2] = y4;

	float dummyy = 0;
	if ( PointInTriangle( x, dummyy, y, v1,v2,v3 ) ||
		 PointInTriangle( x, dummyy, y, v3,v4,v1 ) )
		return true;
	return false;
}


void AddVertex( float* vertices, size_t& count, float x, float y, float z )
{
	vertices[count*3+0] = x;
	vertices[count*3+1] = y;
	vertices[count*3+2] = z;
	count++;
};

void AddSurface( size_t* surfaces, size_t& count, size_t s1, size_t s2, size_t s3 )
{
	surfaces[count*3+0] = s1;
	surfaces[count*3+1] = s2;
	surfaces[count*3+2] = s3;
	count++;
};

bool PointInsideBox( float* b, float* v1, float* v2, float* v3, float* point )
{
	// calculate midpoints of 6 faces
	float m1[3], m2[3], m3[3];
	float m4[3], m5[3], m6[3];
	m1[0] = b[0] + (v1[0] + v2[0]) * 0.5f;
	m1[1] = b[1] + (v1[1] + v2[1]) * 0.5f;
	m1[2] = b[2] + (v1[2] + v2[2]) * 0.5f;

	m2[0] = b[0] + (v2[0] + v3[0]) * 0.5f;
	m2[1] = b[1] + (v2[1] + v3[1]) * 0.5f;
	m2[2] = b[2] + (v2[2] + v3[2]) * 0.5f;

	m3[0] = b[0] + (v1[0] + v3[0]) * 0.5f;
	m3[1] = b[1] + (v1[1] + v3[1]) * 0.5f;
	m3[2] = b[2] + (v1[2] + v3[2]) * 0.5f;

	m4[0] = m1[0] + v3[0];
	m4[1] = m1[1] + v3[1];
	m4[2] = m1[2] + v3[2];

	m5[0] = m2[0] + v1[0];
	m5[1] = m2[1] + v1[1];
	m5[2] = m2[2] + v1[2];

	m6[0] = m3[0] + v2[0];
	m6[1] = m3[1] + v2[1];
	m6[2] = m3[2] + v2[2];

	// calculate distances from 6 middles to their extremes
	float d1 = (v1[0] + v2[0]) * 0.5f - v3[0];
	float d2 = (v1[1] + v2[1]) * 0.5f - v3[1];
	float d3 = (v1[2] + v2[2]) * 0.5f - v3[2];
	float s1 = d1*d1 + d2*d2 + d3*d3;

	d1 = (v3[0] + v2[0]) * 0.5f - v1[0];
	d2 = (v3[1] + v2[1]) * 0.5f - v1[1];
	d3 = (v3[2] + v2[2]) * 0.5f - v1[2];
	float s2 = d1*d1 + d2*d2 + d3*d3;

	d1 = (v1[0] + v3[0]) * 0.5f - v2[0];
	d2 = (v1[1] + v3[1]) * 0.5f - v2[1];
	d3 = (v1[2] + v3[2]) * 0.5f - v2[2];
	float s3 = d1*d1 + d2*d2 + d3*d3;

	// if distance(point, mx) <= s for all - then inside box
	d1 = m1[0] - point[0];
	d2 = m1[1] - point[1];
	d3 = m1[2] - point[2];
	float dst = d1 * d1 + d2 * d2 + d3 * d3;
	if ( dst <= s1 )
	{
		d1 = m2[0] - point[0];
		d2 = m2[1] - point[1];
		d3 = m2[2] - point[2];
		dst = d1 * d1 + d2 * d2 + d3 * d3;
		if ( dst <= s2 )
		{
			d1 = m3[0] - point[0];
			d2 = m3[1] - point[1];
			d3 = m3[2] - point[2];
			dst = d1 * d1 + d2 * d2 + d3 * d3;
			if ( dst <= s3 )
			{
				d1 = m4[0] - point[0];
				d2 = m4[1] - point[1];
				d3 = m4[2] - point[2];
				dst = d1 * d1 + d2 * d2 + d3 * d3;
				if ( dst <= s1 )
				{
					d1 = m5[0] - point[0];
					d2 = m5[1] - point[1];
					d3 = m5[2] - point[2];
					dst = d1 * d1 + d2 * d2 + d3 * d3;
					if ( dst <= s2 )
					{
						d1 = m6[0] - point[0];
						d2 = m6[1] - point[1];
						d3 = m6[2] - point[2];
						dst = d1 * d1 + d2 * d2 + d3 * d3;
						if ( dst <= s3 )
						{
							return true;
						}
					}
				}
			}
		}
	}
	return false;
};

bool PointInsideBox( const TPoint& p1, const TPoint& p2, 
					 const TPoint& point )
{
	TPoint v1, v2, v3;
	v1.x = p2.x - p1.x;
	v2.y = p2.y - p1.y;
	v3.z = p2.z - p1.z;
	return PointInsideBox( p1, v1,v2,v3, point );
};

bool PointInsideBox( const TPoint& b, 
					 const TPoint& v1, const TPoint& v2, const TPoint& v3, 
					 const TPoint& point )
{
	// calculate midpoints of 6 faces
	TPoint m1, m2, m3;
	TPoint m4, m5, m6;
	m1 = b + (v1 + v2) * 0.5f;
	m2 = b + (v2 + v3) * 0.5f;
	m3 = b + (v1 + v3) * 0.5f;

	m4 = m1 + v3;
	m5 = m2 + v1;
	m6 = m3 + v2;

	// calculate distances from 6 middles to their opposite
	// vector corners
	float d1 = (v1.x + v2.x) * 0.5f - v3.x;
	float d2 = (v1.y + v2.y) * 0.5f - v3.y;
	float d3 = (v1.z + v2.z) * 0.5f - v3.z;
	float s1 = d1*d1 + d2*d2 + d3*d3;

	d1 = (v3.x + v2.x) * 0.5f - v1.x;
	d2 = (v3.y + v2.y) * 0.5f - v1.y;
	d3 = (v3.z + v2.z) * 0.5f - v1.z;
	float s2 = d1*d1 + d2*d2 + d3*d3;

	d1 = (v1.x + v3.x) * 0.5f - v2.x;
	d2 = (v1.y + v3.y) * 0.5f - v2.y;
	d3 = (v1.z + v3.z) * 0.5f - v2.z;
	float s3 = d1*d1 + d2*d2 + d3*d3;

	// if distance(point, mx) <= s for all - then inside box
	d1 = m1.x - point.x;
	d2 = m1.y - point.y;
	d3 = m1.z - point.z;
	float dst = d1 * d1 + d2 * d2 + d3 * d3;
	if ( dst <= s1 )
	{
		d1 = m2.x - point.x;
		d2 = m2.y - point.y;
		d3 = m2.z - point.z;
		dst = d1 * d1 + d2 * d2 + d3 * d3;
		if ( dst <= s2 )
		{
			d1 = m3.x - point.x;
			d2 = m3.y - point.y;
			d3 = m3.z - point.z;
			dst = d1 * d1 + d2 * d2 + d3 * d3;
			if ( dst <= s3 )
			{
				d1 = m4.x - point.x;
				d2 = m4.y - point.y;
				d3 = m4.z - point.z;
				dst = d1 * d1 + d2 * d2 + d3 * d3;
				if ( dst <= s1 )
				{
					d1 = m5.x - point.x;
					d2 = m5.y - point.y;
					d3 = m5.z - point.z;
					dst = d1 * d1 + d2 * d2 + d3 * d3;
					if ( dst <= s2 )
					{
						d1 = m6.x - point.x;
						d2 = m6.y - point.y;
						d3 = m6.z - point.z;
						dst = d1 * d1 + d2 * d2 + d3 * d3;
						if ( dst <= s3 )
						{
							// last check:
							// check if inside min/max volume
							// of object (just in case its on the
							// other side of a valid inside)
							TPoint min = b;
							TPoint max = b + v1 + v2 + v3;
							float temp;
							if ( min.x > max.x )
							{
								temp = min.x;
								min.x = max.x;
								max.x = temp;
							}
							if ( min.y > max.y )
							{
								temp = min.y;
								min.y = max.y;
								max.y = temp;
							}
							if ( min.z > max.z )
							{
								temp = min.z;
								min.z = max.z;
								max.z = temp;
							}
							if ( min.x <= point.x && point.x <= max.x )
							if ( min.y <= point.y && point.y <= max.y )
							if ( min.z <= point.z && point.z <= max.z )
								return true;
						}
					}
				}
			}
		}
	}
	return false;
};

//==========================================================================

void DrawCrosshair( float size, const TPoint& p, float angle )
{
	glPushMatrix();
		glEnable(GL_COLOR_MATERIAL );
		glColor3ub( 0,0,0 );
		glTranslatef( p.x,p.y,p.z );
		glRotatef( angle, 0,1,0 );
		glBegin( GL_LINES );

			glVertex3f( -size, 0, 0 );
			glVertex3f( -size*0.25f, 0, 0 );

			glVertex3f( size, 0, 0 );
			glVertex3f( size*0.25f, 0, 0 );

			glVertex3f( 0, -size, 0 );
			glVertex3f( 0, -size*0.25f, 0 );

			glVertex3f( 0, size, 0 );
			glVertex3f( 0, size*0.25f, 0 );

		glEnd();
	glPopMatrix();
};

void ShowMachineGunFire( const TPoint& start, const TPoint& dirn, 
						 float gunRange, size_t counter )
{
	glEnable(GL_COLOR_MATERIAL );
/*
	int val;
	glGetIntegerv( GL_LINE_SMOOTH, &val );
	if ( val==1 )
	{
		glDisable( GL_LINE_SMOOTH );
	}
*/
	float pos = 0.25f * float(counter%4);

	glColor3ub( 0,0,0 );
	glBegin( GL_POINTS );

	for ( size_t i=0; i<size_t(gunRange); i++ )
	{
		glVertex3f( start.x + (dirn.x * pos),
					start.y + (dirn.y * pos),
					start.z + (dirn.z * pos) );

		pos = pos + 1;
	}

	glEnd();
/*
	if ( val==1 )
	{
		glEnable( GL_LINE_SMOOTH );
	}
*/
};

