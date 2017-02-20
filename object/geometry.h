#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

//==========================================================================

class _EXPORT TPoint
{
public:
	TPoint( void );
	TPoint( float x, float y, float z );
	TPoint( float* v );
	TPoint( const TPoint& _point );

	~TPoint( void );

	const TPoint& operator =( const TPoint& );
	const TPoint& operator +( const TPoint& );
	const TPoint& operator +=( const TPoint& );
	const TPoint& operator -( const TPoint& );
	const TPoint& operator -=( const TPoint& );

	const TPoint& operator *( float scalar );

	float x,y,z;
};

TPoint operator+( const TPoint&, const TPoint& );

//==========================================================================

class _EXPORT TMatrix
{
public:
	TMatrix( void );
	TMatrix( const TMatrix& );

	const TMatrix& operator=( const TMatrix& );

	void Identity( void ); // turn matrix into an identity matrix

	float*	Matrix( void ); // get-set matrix
	void	Matrix( float* _m );

	float*	MDLMatrix( void ); // get-set mdl matrix
	void	MDLMatrix( float* _m );

	void	SetTranslation( const float* t );
	void	GetTranslation( float* t );
	void	AddTranslation( const float* t );

	void	Mult( const float* v1, float* result );
	void	Mult( const float* v1, float scale, float* result );
	void	Mult( const TPoint& v1, TPoint& result );
	void	Mult( const TPoint& v1, float scale, TPoint& result );
	void	MDLMult( const TPoint& v1, TPoint& result );
	TPoint	Mult( const TPoint& v1 );
	TPoint	Mult( const TPoint& v1, float scale );

	void	SetTranslation( const TPoint& t );
	void	GetTranslation( TPoint& t );
	void	AddTranslation( const TPoint& t );

	bool	IsMDL( void ) const;
	void	IsMDL( bool _ismdl );

	void	OpenGLTransformToMatrix( float tx, float ty, float tz,
									 float rx, float ry, float rz,
									 float sx=1, float sy=1, float sz=1 );
private:
	float m[16];
	bool  ismdl;
};

//==========================================================================

// box formed by point b[] and vectors v1,v2,v3
bool _EXPORT PointInsideBox( float* b, float* v1, float* v2, float* v3, float* point );

bool _EXPORT PointInsideBox( const TPoint& b, const TPoint& v1, const TPoint& v2, 
					 const TPoint& v3, const TPoint& point );

bool _EXPORT PointInsideBox( const TPoint& p1, const TPoint& p2, 
					 const TPoint& point );

float _EXPORT AngleBetweenVectors( float t1x, float t1y, float t1z,
						   float t2x, float t2y, float t2z );
float _EXPORT VectorsToAngle( float x1, float y1, float x2, float y2 );

void _EXPORT AddVertex( float* vertices, size_t& count, float x, float y, float z );
void _EXPORT AddSurface( size_t* surfaces, size_t& count, size_t s1, size_t s2, size_t s3 );

bool _EXPORT PointInTriangle( float x, float &y, float z,
							  const float* v1, const float* v2, const float* v3 );

bool _EXPORT LineIntersectsTriangle( float l1x, float l1y, float l1z,
							 float l2x, float l2y, float l2z,
							 float t1x, float t1y, float t1z,
							 float t2x, float t2y, float t2z,
							 float t3x, float t3y, float t3z );

bool _EXPORT LineIntersectsTriangle( float l1x, float l1y, float l1z,
							 float l2x, float l2y, float l2z,
							 const float* t1, const float* t2, const float* t3 );

bool LineIntersectsTriangle2( const float* l1, const float* l2,
							  const float* t1, const float* t2, const float* t3 );

bool _EXPORT LineIntersectsTriangle2( float l1x, float l1y, float l1z,
							 float l2x, float l2y, float l2z,
							 float t1x, float t1y, float t1z,
							 float t2x, float t2y, float t2z,
							 float t3x, float t3y, float t3z );

bool _EXPORT LineIntersectsTriangle2( float l1x, float l1y, float l1z,
							 float l2x, float l2y, float l2z,
							 float* t1, float* t2, float* t3 );

bool _EXPORT LineIntersectsTriangle( float l1x, float l1y, float l1z,
							 float l2x, float l2y, float l2z,
							 float t1x, float t1y, float t1z,
							 float t2x, float t2y, float t2z,
							 float t3x, float t3y, float t3z,
							 float& y );

bool _EXPORT LineIntersectsTriangle( float l1x, float l1y, float l1z,
							 float l2x, float l2y, float l2z,
							 float* t1, float* t2, float* t3,
							 float& y );

bool _EXPORT PointOnPlane( float x, float& y, float z,
				   float v1x, float v1y, float v1z,
				   float v2x, float v2y, float v2z,
				   float v3x, float v3y, float v3z );

bool _EXPORT PointInsideBox( float x, float y,
					 float x1, float y1, float x2, float y2,
					 float x3, float y3, float x4, float y4 );

bool _EXPORT IntersectLines( float ax, float ay, float az,
					 float bx, float by, float bz,
					 float cx, float cy, float cz,
					 float dx, float dy, float dz,
					 float& xi, float& yi, float& zi );
void _EXPORT Normal( float& nx, float& ny, float& nz,
			 float v1x, float v1y, float v1z,
			 float v2x, float v2y, float v2z,
			 float v3x, float v3y, float v3z );

void _EXPORT DrawBoundingBox( float minX, float minY, float minZ,
					  float maxX, float maxY, float maxZ,
					  float r, float g, float b );

void _EXPORT DrawBoundingBox( float x1, float y1, float z1,
					  float x2, float y2, float z2,
					  float x3, float y3, float z3,
					  float x4, float y4, float z4,
					  float x5, float y5, float z5,
					  float x6, float y6, float z6,
					  float x7, float y7, float z7,
					  float x8, float y8, float z8,
					  float r, float g, float b );

//==========================================================================

inline
void MatrixMult( const float* matrix, 
				 float& x, float& y, float& z )
{
	float a,b,c;
	a = x*matrix[0] + y*matrix[4] + z*matrix[8] + matrix[12];
	b = x*matrix[1] + y*matrix[5] + z*matrix[9] + matrix[13];
	c = x*matrix[2] + y*matrix[6] + z*matrix[10] + matrix[14];
	x = a;
	y = b;
	z = c;
}


inline
void MDLMatrixMult( const float* matrix, const float* v1, float* v2 )
{
	v2[0] = (v1[0]*matrix[0] + v1[1]*matrix[1] + v1[2]*matrix[2] + matrix[3]);
	v2[1] = (v1[0]*matrix[4] + v1[1]*matrix[5] + v1[2]*matrix[6] + matrix[7]);
	v2[2] = (v1[0]*matrix[8] + v1[1]*matrix[9] + v1[2]*matrix[10] + matrix[11]);
};

inline
void MatrixMult( const float* matrix, const float* v1, float* v2 )
{
	v2[0] = (v1[0]*matrix[0] + v1[1]*matrix[4] + v1[2]*matrix[8] + matrix[12]);
	v2[1] = (v1[0]*matrix[1] + v1[1]*matrix[5] + v1[2]*matrix[9] + matrix[13]);
	v2[2] = (v1[0]*matrix[2] + v1[1]*matrix[6] + v1[2]*matrix[10] + matrix[14]);
};

inline
void MultiplyMatrices( const float* m1, const float* m2, float *m3 )
{
	m3[0]  = m1[0]*m2[0] + m1[4]*m2[1] + m1[8]*m2[2]  + m1[12]*m2[3];
	m3[1]  = m1[1]*m2[0] + m1[5]*m2[1] + m1[9]*m2[2]  + m1[13]*m2[3];
	m3[2]  = m1[2]*m2[0] + m1[6]*m2[1] + m1[10]*m2[2] + m1[14]*m2[3];
//	m3[3]  = m1[3]*m2[0] + m1[7]*m2[1] + m1[11]*m2[2] + m1[15]*m2[3];
	m3[3] = 0;

	m3[4]  = m1[0]*m2[4] + m1[4]*m2[5] + m1[8]*m2[6]  + m1[12]*m2[7];
	m3[5]  = m1[1]*m2[4] + m1[5]*m2[5] + m1[9]*m2[6]  + m1[13]*m2[7];
	m3[6]  = m1[2]*m2[4] + m1[6]*m2[5] + m1[10]*m2[6] + m1[14]*m2[7];
//	m3[7]  = m1[3]*m2[4] + m1[7]*m2[5] + m1[11]*m2[6] + m1[15]*m2[7];
	m3[7] = 0;

	m3[8]  = m1[0]*m2[8] + m1[4]*m2[9] + m1[8]*m2[10]  + m1[12]*m2[11];
	m3[9]  = m1[1]*m2[8] + m1[5]*m2[9] + m1[9]*m2[10]  + m1[13]*m2[11];
	m3[10] = m1[2]*m2[8] + m1[6]*m2[9] + m1[10]*m2[10] + m1[14]*m2[11];
//	m3[11] = m1[3]*m2[8] + m1[7]*m2[9] + m1[11]*m2[10] + m1[15]*m2[11];
	m3[11] = 0;

	m3[12] = m1[0]*m2[12] + m1[4]*m2[13] + m1[8]*m2[14]  + m1[12]*m2[15];
	m3[13] = m1[1]*m2[12] + m1[5]*m2[13] + m1[9]*m2[14]  + m1[13]*m2[15];
	m3[14] = m1[2]*m2[12] + m1[6]*m2[13] + m1[10]*m2[14] + m1[14]*m2[15];
//	m3[15] = m1[3]*m2[12] + m1[7]*m2[13] + m1[11]*m2[14] + m1[15]*m2[15];
	m3[15] = 1;
};

inline
void MatrixMult( const float* matrix, const float* v1, float scale, float* v2 )
{
	v2[0] = (v1[0]*matrix[0] + v1[1]*matrix[4] + v1[2]*matrix[8])*scale + matrix[12];
	v2[1] = (v1[0]*matrix[1] + v1[1]*matrix[5] + v1[2]*matrix[9])*scale + matrix[13];
	v2[2] = (v1[0]*matrix[2] + v1[1]*matrix[6] + v1[2]*matrix[10])*scale + matrix[14];
};

//==========================================================================

void _EXPORT DrawCrosshair( float size, const TPoint& p, float angle );
void _EXPORT ShowMachineGunFire( const TPoint& start, const TPoint& dirn, 
						 float gunRange, size_t counter );

//==========================================================================

#endif
