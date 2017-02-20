#ifndef __OBJECT_VEHICLE_H_
#define __OBJECT_VEHICLE_H_

//==========================================================================

#include <object/vector.h>
#include <object/compoundObject.h>

//==========================================================================

enum actions { aUp=1, 
			   aDown=2, 
			   aLeft=4, 
			   aRight=8, 
			   aFire=16, 
			   aExplode=32 
			};

class TVehicle
{
public:
	TVehicle( void );
	virtual ~TVehicle( void );

	TVehicle( const TVehicle& );
	const TVehicle& operator = (const TVehicle&);

	virtual void Draw( void ) const;

	// move the vehicle according to key
	virtual void Move( TLandscape& landObj, size_t key, float speed );

	bool LoadBinary( const TString& fname, TString& errStr, 
					 const TString& pathname,
					 const TString& texturePath );

	void SetOrientationToVehicle( TVehicle& obj );

	int	 Location1( void ) const;
	void Location1( int );

	int	 Location2( void ) const;
	void Location2( int );

	int	 Location3( void ) const;
	void Location3( int );

	int	 Location4( void ) const;
	void Location4( int );

	float MX( void ) const;
	float MY( void ) const;
	float MZ( void ) const;

	float GetXZRadius( void ) const;

	// get 4 points from the middle around this object
	void GetCornerPoints( float lx, float lz,
						  float &x1, float &z1,
						  float &x2, float &z2,
						  float &x3, float &z3,
						  float &x4, float &z4 ) const;

	void GetCornerPoints( float _yangle,
						  float &x1, float &z1,
						  float &x2, float &z2,
						  float &x3, float &z3,
						  float &x4, float &z4 ) const;

	void GetExhaustPoints( float lx, float lz,
						   float &x3, float &z3,
						   float &x4, float &z4 ) const;

	float X1( void ) const;
	float X2( void ) const;
	float X3( void ) const;
	float X4( void ) const;

	float Y1( void ) const;
	float Y2( void ) const;
	float Y3( void ) const;
	float Y4( void ) const;

	float Z1( void ) const;
	float Z2( void ) const;
	float Z3( void ) const;
	float Z4( void ) const;

	float EX3( void ) const;
	float EX4( void ) const;
	float EZ3( void ) const;
	float EZ4( void ) const;

    float	X( void ) const;
    void	X( float _x );

    float	Y( void ) const;
    void	Y( float _y );

    float	Z( void ) const;
    void	Z( float _z );

    float	Xangle( void ) const;
    void	Xangle( float _xangle );

    float	Yangle( void ) const;
    void	Yangle( float _yangle );

    float	Zangle( void ) const;
    void	Zangle( float _zangle );

	size_t	CurrentIndex( void ) const;
	void	CurrentIndex( size_t c );

	void	PauseAnimations( bool p );

	float	SizeX( void ) const;
	float	SizeY( void ) const;
	float	SizeZ( void ) const;

private:
	friend class TAnimation;
	friend class TTank;

	TCompoundObject object;

	size_t currentIndex;

	int location1;
	int location2;
	int location3;
	int location4;

	// mid point and normal of vehicle
	float mx,my,mz;

	// corner points of vehicle on landscape
	float x1,y1,z1;
	float x2,y2,z2;
	float x3,y3,z3;
	float x4,y4,z4;

	// exhaust points
	float ex3,ez3,ex4,ez4;

	// undo movement capability
	float previousX;
	float previousZ;
	float previousYangle;

	float xzRadius;
	float xzAngle;

	int   dirn;
	float diff;

	// only used for internal optimisations
	mutable float xa,ya,za;
};

//==========================================================================

#endif

