#ifndef __LANDSCAPE_H__
#define __LANDSCAPE_H__

//==========================================================================

#include <object/vector.h>

//==========================================================================

class TObject;
typedef unsigned char byte;

//==========================================================================

//
//	The size of the world, fast maths
//
const int WORLDSIZE			= 256;							// make sure these are powers of 2
const int WORLDSIZESHIFT	= 8;							// 256 is 2 ^ 8
const int WORLDSIZEMOD		= (WORLDSIZE-1);				// 255 is the mod for 256 shift

const int ALTITUDEMAPSIZE	= (WORLDSIZE*WORLDSIZE);		// make sure these are powers of 2
const int COLOURMAPSIZE		= (WORLDSIZE*WORLDSIZE*3);		// rgb for each height map value

const int gs				= 8;	// grid size, must be power of 2
const int gshift			= 3;	// matching shift pattern

//const float camera_height	= 18.0f;  // how heigh the camera hovers over the landscape

//==========================================================================

class TLandscape
{
public:
	TLandscape( void );
	virtual ~TLandscape( void );

	void 	GetTranslation( float x, float z, float& tx, float& ty, float& tz );
	void	Draw( float yangle, float x, float z );
	bool	Load( int world_id, char* errStr ); // load a heightmap & colourmap from data\ directory for the landscape

	float	GetHeight( int xp, int yp ); // get height of landscape @ xp,yp
	float	GetFloatHeight( float xp, float zp );
	float	GetSlope( float x1, float z1, float x2, float z2 );
	void	GetColour( int x, int y, byte &r, byte &g, byte &b ); // get colour of landscape @ x,y

	float	GetXAngle( TObject& obj );
	float	GetZAngle( TObject& obj );

private:
	void	Movement( float yAngle, float& dirX, float& dirZ );
	int		BilinearFilter (int p1, int p2, int p3, int p4, int x,  int y);
	void	CalculateTriangle( float yangle, float xpos, float zpos );
	bool	InsideTriangle( float xpos, float ypos );

private:
	int		world_radius;
	int		circlewidth;
	bool*	circle;
	byte*	lpColour;
	byte*	lpAltitude;

	float	heightDivision;

	float	t1x,t1y,t2x,t2y,t3x,t3y;
};

//==========================================================================

#endif


