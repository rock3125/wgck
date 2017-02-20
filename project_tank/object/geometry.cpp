#include <precomp_header.h>

#include <object/geometry.h>

//==========================================================================

const float degToRad = 0.01745329252033f;
const float radToDeg = 57.295779511273f;

//==========================================================================

TPoint::TPoint( void )
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
};



TPoint::TPoint( float _x, float _y, float _z )
{
	x = _x;
	y = _y;
	z = _z;
}



TPoint::TPoint( const TPoint& _point )
{
	x = _point.x;
	y = _point.y;
	z = _point.z;
};



TPoint::~TPoint( void )
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
};



TPoint& TPoint::operator =( const TPoint& _point )
{
	x = _point.x;
	y = _point.y;
	z = _point.z;

	return *this;
};


//==========================================================================


TRect::TRect( void )
{
	x1 = 0.0f;
	y1 = 0.0f;
	x2 = 0.0f;
	y2 = 0.0f;
}



TRect::TRect( const TRect& _rect )
{
	x1 = _rect.x1;
	y1 = _rect.y1;
	x2 = _rect.x2;
	y2 = _rect.y2;
};



TRect::TRect( float _x1, float _y1, float _x2, float _y2 )
{
	x1 = _x1;
	y1 = _y1;
	x2 = _x2;
	y2 = _y2;
}



TRect::~TRect( void )
{
	x1 = 0.0f;
	y1 = 0.0f;
	x2 = 0.0f;
	y2 = 0.0f;
};



TRect& TRect::operator =( const TRect& _rect )
{
	x1 = _rect.x1;
	y1 = _rect.y1;
	x2 = _rect.x2;
	y2 = _rect.y2;

	return *this;
};


bool TRect::Contains( const TPoint& point )
{
	return ( (point.x>=x1 && point.x<=x2) && (point.y>=y1 && point.y<=y2) );
};


//==========================================================================


TPolygon::TPolygon( void )
{
	numPoints = 0;
};



TPolygon::TPolygon( const TPolygon& _poly )
{
	numPoints = _poly.numPoints;
	for ( size_t i=0; i<numPoints; i++ )
		points[i] = _poly.points[i];
};



TPolygon::~TPolygon( void )
{
	numPoints = 0;
};



TPolygon& TPolygon::operator =( const TPolygon& _poly )
{
	numPoints = _poly.numPoints;
	for ( size_t i=0; i<numPoints; i++ )
		points[i] = _poly.points[i];

	return *this;
};



void TPolygon::AddPoint( const TPoint& point )
{
	if ( numPoints<10 )
		points[numPoints++] = point;
};



bool TPolygon::Contains( const TPoint& ptTest )
{
	TPoint	pt1, pt2;
	size_t	wnumintsct=0;

	TRect prbound;
	if (!PtInPolyRect(ptTest,prbound))
		return false;

	pt1 = ptTest;
	pt2 = ptTest;
	pt2.x = prbound.x2 + 50;

	// Now go through each of the lines in the polygon and see if it
	// intersects
	for ( size_t i=0; i<numPoints-1; i++)
	{
		if (Intersect( ptTest, pt2, points[i], points[i+1] ) )
			wnumintsct++ ;
	}

	// And the last line
	if (Intersect(ptTest, pt2, points[numPoints-1], points[0]))
		wnumintsct++;

	return (wnumintsct&1);
};


//
// FUNCTION:   PtInPolyRect
//
//   PURPOSE
//	 This routine determines if a point is within the smallest rectangle
//	 that encloses a polygon.
//	
//	 RETURN VALUE
//	 (BOOL) TRUE or FALSE depending on whether the point is in the rect or
//   not.
//
bool TPolygon::PtInPolyRect( const TPoint& ptTest, TRect& prbound )
{
	// If a bounding rect has not been passed in, calculate it
	float	xmin, xmax, ymin, ymax;
	size_t  i;

	xmin = xmax = points[0].x;
	ymin = ymax = points[0].y;

	for ( i=0; i<numPoints; i++ )
	{
		if (points[i].x < xmin)
			xmin = points[i].x;
		if (points[i].x > xmax)
			xmax = points[i].x;
		if (points[i].y < ymin)
			ymin = points[i].y;
		if (points[i].y > ymax)
			ymax = points[i].y;
	}

	TRect r(xmin,ymin,xmax,ymax);
	prbound = r;
	return r.Contains( ptTest );
}


//
//	FUNCTION:   Intersect
//
//	PURPOSE
//	Given two line segments, determine if they intersect.
//
//	RETURN VALUE
//	TRUE if they intersect, FALSE if not.
//
bool TPolygon::Intersect( const TPoint& p1, const TPoint& p2, const TPoint& p3, const TPoint& p4 )
{
	return ((( CCW(p1, p2, p3) * CCW(p1, p2, p4)) <= 0)
			&& (( CCW(p3, p4, p1) * CCW(p3, p4, p2)  <= 0) )) ;
}


//	FUNCTION:   CCW (CounterClockWise)
//
//	PURPOSE
//	Determines, given three points, if when travelling from the first to
//	the second to the third, we travel in a counterclockwise direction.
//
//	RETURN VALUE
//	(int) 1 if the movement is in a counterclockwise direction, -1 if
//	not.
//
int TPolygon::CCW( const TPoint& p0, const TPoint& p1, const TPoint& p2 )
{
	float dx1, dx2;
	float dy1, dy2;

	dx1 = p1.x - p0.x ; dx2 = p2.x - p0.x;
	dy1 = p1.y - p0.y ; dy2 = p2.y - p0.y;

	// This is basically a slope comparison: we don't do divisions because
	// of divide by zero possibilities with pure horizontal and pure
    // vertical lines.
	return ((dx1 * dy2 > dy1 * dx2) ? 1 : -1);
}


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

	
inline
bool PointOnPlane( float x, float& y, float z,
				   float v1x, float v1y, float v1z,
				   float v2x, float v2y, float v2z,
				   float v3x, float v3y, float v3z )
{
	float nx,ny,nz;

	Normal( nx,ny,nz, v1x,v1y,v1z, v2x,v2y,v2z, v3x,v3y,v3z );
	if ( ny==0.0f )
		return false;

	y = (-nx * (x-v1x) - nz * (z-v1z)) * (1/ny) + v1y;
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

	if ( (sx-px)!=0 && (rz-qz)!=0 )
	{
		a = (pz-qz) / (rz-qz);
		b = ((sz-pz) / (rz-qz)) * ((qx-px) / (sx-px));
		c = ((sz-pz) / (rz-qz)) * ((rx-qx) / (sx-px));

		if ( (1-c)==0 )
			return false;

		s = (a+b) / (1-c);
	}
	else if ( (sz-pz)!=0 && (rx-qx)!=0 )
	{
		a = (px-qx) / (rx-qx);
		b = ((sx-px) / (rx-qx)) * ((qz-pz) / (sz-pz));
		c = ((sx-px) / (rx-qx)) * ((rz-qz) / (sz-pz));

		if ( (1-c)==0 )
			return false;

		s = (a+b) / (1-c);
	}
	else
		return false;

	xi = qx + (rx-qx) * s;
	yi = qy + (ry-qy) * s;
	zi = qz + (rz-qz) * s;

	return true;
};


inline
bool SameSense( float a, float b )
{
	return ( (a>0 && b>0) || (a<0 && b<0) || (a==b) );
};


inline
bool SameDirection( float ax, float ay, float az,
				    float bx, float by, float bz ) 
{
	return ( SameSense(ax,bx) && SameSense(ay,by) && SameSense(az,bz) );
};


inline
float Distance( float ax, float ay, float az,
				float bx, float by, float bz )
{
	return float( (bx-ax)*(bx-ax) + (by-ay)*(by-ay) + (bz-az)*(bz-az) );
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


bool PointInTriangle( float x, float &y, float z,
					  float t1x, float t1y, float t1z,
					  float t2x, float t2y, float t2z,
					  float t3x, float t3y, float t3z )
{
	// first find the x,z on the plane made by the triangle
	if ( PointOnPlane(x,y,z, t1x,t1y,t1z, t2x,t2y,t2z, t3x,t3y,t3z) )
	{
		// then for two different parts of the triangle calculate the distance
		// between (xyz) and (t1+t2)/2 and where this line intersects either
		// vectors t2t3 or t1t3.  If this distance is greater - we are outside
		// the triangle by defn.
		if ( IntersectionOK(x,y,z, t1x,t1y,t1z, t2x,t2y,t2z, t3x,t3y,t3z) )
			return true;
	}
	return false;
};


bool AngleInsideSlice( float angle, float minA, float maxA )
{
	if ( minA>180.0f ) 
		minA = minA - 360.0f;
	if ( maxA>180.0f ) 
		maxA = maxA - 360.0f;
	if ( angle>180.0f ) 
		angle = angle - 360.0f;

	if ( minA > maxA )
	{
		float temp = minA;
		minA = maxA;
		maxA = temp;
		return !(minA<angle && angle<maxA);
	}
	return (minA<angle && angle<maxA);
};


// multiply the openGL matrix with x,y,z
void MatrixMult( float& x, float& y, float& z, float* m )
{
	float x1,y1,z1;

	x1 = x*m[0] + y*m[4] + z*m[8]  + m[12];
	y1 = x*m[1] + y*m[5] + z*m[9]  + m[13];
	z1 = x*m[2] + y*m[6] + z*m[10] + m[14];

	x = x1;
	y = y1;
	z = z1;
};

//==========================================================================

