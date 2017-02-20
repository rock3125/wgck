#ifndef __OBJECT_VEHICLE_H_
#define __OBJECT_VEHICLE_H_

//==========================================================================

#include <object/vector.h>
#include <object/compoundObject.h>

//==========================================================================

class TLandscape;

//==========================================================================

class TVehicle
{
public:
	TVehicle( float hoverHeight, float submergeDepth );
	virtual ~TVehicle( void );

	TVehicle( const TVehicle& );
	const TVehicle& operator = (const TVehicle&);

	virtual void Draw( void ) const;

	// move the vehicle according to key
	virtual void Move( size_t key, float speed );

	// commit the previous call to Move for real or abort
	virtual void CommitMove( void );
	virtual void AbortMove( void );

	bool	Collision( float x, float y, float z,
					   float x1, float y1, float z1 ) const;

	// after loading
	void SetupObject( void );

	bool LoadBinary( const TString& fname, TString& errStr, 
					 const TString& pathname,
					 const TString& texturePath );

	void SetOrientationToVehicle( TVehicle& obj );

	float SubmergeDepth( void ) const;
	void  SubmergeDepth( float smd );

	float MX( void ) const;
	float MY( void ) const;
	float MZ( void ) const;

	float GetXZRadius( void ) const;

	// get 4 points from the middle around this object
	void GetCornerPoints( void );

	void GetCornerPoints( float _yangle,
						  float &x1, float &z1,
						  float &x2, float &z2,
						  float &x3, float &z3,
						  float &x4, float &z4 ) const;

	void GetCornerPoints( float _yangle,
						  float &x1, float &z1,
						  float &x2, float &z2 ) const;

	void SetLocations( int _location1, int _location2, 
					   int _location3, int _location4 );
	void SetObjectLocations( int _locationObj1, int _locationObj2, 
							 int _locationObj3, int _locationObj4 );
	void SetNewY( float _n_y1, float _n_y2, float _n_y3, float _n_y4 );

	void SetInitialPosition( TLandscape& landObj, float x, float z );
	void ResetPosition( void ); // reset vehicle to initial position

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

	virtual float	SizeX( void ) const;
	virtual float	SizeY( void ) const;
	virtual float	SizeZ( void ) const;

	virtual bool InsideVehicle( float x, float y, float z ) const;

	TCompoundObject& Object( void );

	// are two vehicles colliding?
	bool Collision( TVehicle& otherVehicle ) const;

	// vehicles height above landscape
	void HoverHeight( float hh );
	float HoverHeight( void ) const;

protected:
	void UpdateVehicleMatrix( void );

private:
	friend class TAnimation;
	friend class TLandscape;
	friend class TTank;
	friend class TRaceCar;
	friend class TCamera;
	friend class AITank;
	friend class AIPlane;
	friend class AIFlak;
	friend class AIMG42;

	TCompoundObject object;

	size_t currentIndex;

	// location of vehicle stuff
	float x,y,z;
	float xangle,yangle,zangle;
	mutable float xa,ya,za;
	float mx,my,mz;

	// reset initial position
	float initX, initY, initZ;

	float scaleX, scaleY, scaleZ;

	// new locations - these together witht he old
	// make an undo-able movement vector
	float n_x,n_y,n_z;
	float n_xangle,n_yangle,n_zangle;
	float n_xa, n_ya, n_za;
	float n_mx, n_my, n_mz;

	// corner points of vehicle on landscape
	// exhaust points
	float x1,y1,z1;
	float x2,y2,z2;
	float x3,y3,z3;
	float x4,y4,z4;

//	float xo1, zo1, xo2, zo2;

	float px,py,pz,pa;
	float px1,py1,pz1;
	float px2,py2,pz2;
	float px3,py3,pz3;
	float px4,py4,pz4;

	float xzRadius;
	float xzAngle;

	float hoverHeight;	// how high above the landscape does this vehicle sit
	float submergeDepth; // how deep into the water can I go?  (+ve number)

	int   dirn;
	float diff;

	float matrix[16];
};

//==========================================================================

#endif

