#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

//==========================================================================

void MatrixMult( float& x, float& y, float& z, float* matrix );
float VectorsToAngle( float x1, float y1, float x2, float y2 );
bool PointInTriangle( float x, float &y, float z,
					  float t1x, float t1y, float t1z,
					  float t2x, float t2y, float t2z,
					  float t3x, float t3y, float t3z );
bool PointOnPlane( float x, float& y, float z,
				   float v1x, float v1y, float v1z,
				   float v2x, float v2y, float v2z,
				   float v3x, float v3y, float v3z );
bool AngleInsideSlice( float angle, float minA, float maxA );
bool IntersectLines( float ax, float ay, float az,
					 float bx, float by, float bz,
					 float cx, float cy, float cz,
					 float dx, float dy, float dz,
					 float& xi, float& yi, float& zi );
void Normal( float& nx, float& ny, float& nz,
			 float v1x, float v1y, float v1z,
			 float v2x, float v2y, float v2z,
			 float v3x, float v3y, float v3z );

//==========================================================================

class TPoint
{
public:
	TPoint( void );
	TPoint( float x, float y, float z = 0.0f );
	TPoint( const TPoint& _point );

	~TPoint( void );

	TPoint& operator =( const TPoint& );

	float x,y,z;
};

//==========================================================================

class TRect
{
public:
	TRect( void );
	TRect( const TRect& _rect );
	TRect( float x1, float y1, float x2, float y2 );

	~TRect( void );

	TRect& operator =( const TRect& );

	bool Contains( const TPoint& point );

	float x1,y1;
	float x2,y2;
};

//==========================================================================

class TPolygon
{
public:
	TPolygon( void );
	TPolygon( const TPolygon& _rect );

	~TPolygon( void );

	TPolygon& operator =( const TPolygon& );

	void AddPoint( const TPoint& point );
	bool Contains( const TPoint& point );

	size_t	numPoints;
	TPoint	points[10];

private:
	bool	PtInPolyRect( const TPoint& ptTest, TRect& prbound );
	bool	Intersect( const TPoint& p1, const TPoint& p2, const TPoint& p3, const TPoint& p4 );
	int		CCW( const TPoint& p0, const TPoint& p1, const TPoint& p2 );

};

//==========================================================================

#endif
