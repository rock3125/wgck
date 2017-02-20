#include <precomp_header.h>

#include <object/geometry.h>

//==========================================================================

const double kPI = 3.14159265;

//==========================================================================

#include <math.h>
#include <landscape/landscape.h>
#include <object/object.h>
#include <win32/events.h>

//==========================================================================

TLandscape::TLandscape( void )
	: circle(NULL),
	  lpAltitude(NULL),
	  lpColour(NULL),
	  heightDivision(20.0f)
{
	world_radius = WORLDSIZE * 1; // world viewing distance size

	//
	//	Allocate circle map for drawing, optimisation by me
	//	we only draw anything within a fixed radius (world_radius)
	//	but sqrt calcs. are expensive, so we pre-store the information
	//	in a 2d array.
	//
	int i,j;
	int width = (((int)world_radius * 2) / gs) + 3;
	circlewidth = width;
	circle = new bool[width*width];
	for (i=0;i<width;i++)
	for (j=0;j<width;j++)
		circle[i*width+j] = false;

	//
	//	Circle mathematics
	//
	float cx  = (float)width / 2.0f;  // center x
	float cy  = cx;         // center y
	float rad = cx;         // radius
	for (i=0;i<width;i++)
	for (j=0;j<width;j++)
	{
		float y = (float)i;
		float x = (float)j;

		float dist = (float)sqrt( (y-cy)*(y-cy) + (x-cx)*(x-cx) ); // distance from center
		if ( dist<=rad )
			circle[i*width+j] = true;
	}

	lpAltitude = new byte[ALTITUDEMAPSIZE];
	PostCond( lpAltitude!=NULL );

	lpColour = new byte[COLOURMAPSIZE];
	PostCond( lpColour!=NULL );
}


//==========================================================================

TLandscape::~TLandscape( void )
{
	if ( lpAltitude!=NULL )
		delete []lpAltitude;
	lpAltitude=NULL;

	if ( lpColour!=NULL )
		delete []lpColour;
	lpColour=NULL;

	if ( circle!=NULL )
		delete []circle;
	circle=NULL;
}

//==========================================================================

bool TLandscape::Load( int world_id, char* errStr )
{
	FILE *fh;
	char filename[256];

	sprintf(filename,"data\\world_%02d_cm.bin",world_id);
	fh = fopen( filename, "rb" );
	if ( fh==NULL )
    {
    	sprintf( errStr, "Error opening file \"%s\"", filename );
		return false;
    }
	fread((void *)lpColour,WORLDSIZE,WORLDSIZE*3,fh);
	fclose(fh);

	sprintf(filename,"data\\world_%02d_hm.bin",world_id);
	fh = fopen( filename, "rb" );
	if ( fh==NULL )
    {
    	sprintf( errStr, "Error opening file \"%s\"", filename );
		return false;
    }
	fread((void *)lpAltitude,WORLDSIZE,WORLDSIZE,fh);
	fclose(fh);

	return true;
}

//==========================================================================

void TLandscape::GetTranslation( float x, float z, float& tx, float& ty, float& tz )
{
	//
    //	Calculate grid x & z for two dimensional offsets
    //	into the landscape
    //
    if ( x<0 )
        tx = -float(fmod(-x,gs));
    else
		tx = float(fmod(x,gs));

    if ( z<0 )
        tz = -float(fmod(-z,gs));
    else
		tz = float(fmod(z,gs));

	ty = -float(GetHeight(int(x),int(z)));
};

//==========================================================================

void TLandscape::CalculateTriangle( float yangle, float xpos, float zpos )
{
	float dirx1,dirz1;
	float dirx2,dirz2;
	Movement(-yangle-45.0f,dirx1,dirz1);
	Movement(-yangle+45.0f,dirx2,dirz2);

	t1x = xpos;
	t1y = zpos;

	t2x = t1x + dirx1*world_radius;
	t2y = t1y + dirz1*world_radius;

	t3x = t1x + dirx2*world_radius;
	t3y = t1y + dirz2*world_radius;
}

//==========================================================================

bool TLandscape::InsideTriangle( float xpos, float ypos )
{
	TPolygon poly;
	poly.AddPoint( TPoint(t1x,t1y) );
	poly.AddPoint( TPoint(t2x,t2y) );
	poly.AddPoint( TPoint(t3x,t3y) );

	if ( poly.Contains( TPoint(xpos,ypos) ) )
		return true;
	return false;
}

//==========================================================================

//
//	Render my landscape
//
void TLandscape::Draw( float yangle, float xpos, float zpos )
{
//	CalculateTriangle( yangle, xpos, zpos ); // for culling landscape

	glEnable(GL_COLOR_MATERIAL);

	//
	// get partial grid movement for smooth between gs movement
	//
    int usex, usez; // array indexes for the game itself

	//
    //	Calculate grid x & z for two dimensional offsets
    //	into the landscape
    //
    if ( xpos<0 )
    {
		usex = int(-xpos) >> gshift;
		usex = usex << gshift;
        usex = -usex;
    }
    else
    {
		usex = int(xpos) >> gshift;
		usex = usex<<gshift;
    }

    if ( zpos<0 )
    {
		usez = int(-zpos) >> gshift;
		usez = usez << gshift;
        usez = -usez;
    }
    else
    {
		usez = int(zpos) >> gshift;
		usez = usez<<gshift;
    }

	//
	//	Draw map using triangle strips, don't draw anything outside
	//	the circle located at Location().X(),Location().Z() with radius world_radius
	//
	float z = float(-world_radius);
	int circlez = 0;

	while (z<world_radius)
	{

		glEnable(GL_COLOR);
		glColor3f(0.5f,0.5f,0.5f);
		glBegin(GL_TRIANGLE_STRIP);

		float x = float(-world_radius-gs);
		int circlex = 0;
		while (x<=(world_radius+gs))
		{
			//
			// is this poly inside the circlular visible landscape?
			//
			if ( circle[circlez*circlewidth+circlex] )
			//if ( InsideTriangle( float(circlex), float(circlez) ) )
			{
				// get heightmap height
				float my1  = GetHeight(int(usex+x),int(usez+z));
				float my2  = GetHeight(int(usex+x),int(usez+z+gs));

				// get colourmap colours
				byte r1,g1,b1;
				byte r2,g2,b2;

				GetColour(int(usex+x),int(usez+z),r1,g1,b1);
				GetColour(int(usex+x),int(usez+z+gs),r2,g2,b2);

				glColor3ub(r1,g1,b1);
				glVertex3f((float)x,my1,(float)z);

				glColor3ub(r2,g2,b2);
				glVertex3f((float)x,my2,(float)(z+gs));
			}

			circlex++;
			x += gs;
		}

		glEnd();

		circlez++;
		z += gs;
	}
	glDisable(GL_COLOR_MATERIAL);
}

//==========================================================================

float TLandscape::GetSlope( float x1, float z1, float x2, float z2 )
{
	float height1 = float(GetHeight( int(x1), int(z1) ));
	float height2 = float(GetHeight( int(x2), int(z2) ));

	float dist = float(sqrt( (x1-x2)*(x1-x2) + (z1-z2)*(z1-z2) ) );

	float angle = 0.0f;
	return angle;
};

//==========================================================================
//
//	Get landscape height at world coordinates xp and yp
//
inline
float TLandscape::GetHeight( int xp, int yp )
{
	int x1,y1,x2,y2,x3,y3,x4,y4;

	// translate & bilinear height
	x1 = (xp >> gshift) + WORLDSIZE;
	y1 = (yp >> gshift) + WORLDSIZE;
	x2 = x1 + 1;
	y2 = y1;
	x3 = x1 + 1;
	y3 = y1 + 1;
	x4 = x1;
	y4 = y1 + 1;

	float height = float(BilinearFilter( lpAltitude[(y1 % WORLDSIZEMOD) * WORLDSIZE + (x1 % WORLDSIZEMOD)],
						   lpAltitude[(y2 % WORLDSIZEMOD) * WORLDSIZE + (x2 % WORLDSIZEMOD)],
						   lpAltitude[(y3 % WORLDSIZEMOD) * WORLDSIZE + (x3 % WORLDSIZEMOD)],
						   lpAltitude[(y4 % WORLDSIZEMOD) * WORLDSIZE + (x4 % WORLDSIZEMOD)],
						   (xp+WORLDSIZE) % gs, (yp+WORLDSIZE) % gs ));
	height = height / heightDivision;
	return height;
}

//==========================================================================

float TLandscape::GetFloatHeight( float xp, float zp )
{
	return -float(GetHeight(int(xp),int(zp)));
}

//==========================================================================

//
//	Get landscape colour coordinates at x,y
//
inline
void TLandscape::GetColour( int x, int y, byte &r, byte &g, byte &b )
{
	int x1 = (x >> gshift) + WORLDSIZE;
	int y1 = (y >> gshift) + WORLDSIZE;

	r = lpColour[(y1 % WORLDSIZEMOD) * WORLDSIZE*3 + (x1 % WORLDSIZEMOD)*3 + 0];
	g = lpColour[(y1 % WORLDSIZEMOD) * WORLDSIZE*3 + (x1 % WORLDSIZEMOD)*3 + 1];
	b = lpColour[(y1 % WORLDSIZEMOD) * WORLDSIZE*3 + (x1 % WORLDSIZEMOD)*3 + 2];
}

//==========================================================================

//
// Bilinear interpolation between four values given x,y in range [0..gs>
//
inline 
int TLandscape::BilinearFilter (int p1, int p2, int p3, int p4, int x,  int y)
{
    int p5 = ((p1<<gshift) + x * (p2 - p1));
    int p6 = ((p3<<gshift) + x * (p4 - p3));
    return ((p5<<gshift) + y * (p6 - p5)) >> gshift;
}

//==========================================================================

void TLandscape::Movement( float yAngle, float& dirX, float& dirZ )
{
    // movement
    float yangle = float(( yAngle / 180.0f) * kPI);
	dirZ = -(float)cos(yangle);
	dirX = (float)sin(yangle);
}

//==========================================================================

float TLandscape::GetXAngle( TObject& obj )
{
	float cx,cy,cz;
	float dirx, dirz;
	float angle = 0.0f;

	obj.GetCenter(cx,cy,cz);
	Movement( obj.Xangle(), dirx, dirz );

	float xpos = (obj.X()+cx) * dirx;
	float zpos = (obj.Z()+obj.MinZ()) * dirz;
	float height1 = GetHeight(int(xpos),int(zpos));

	zpos = (obj.Z()+obj.MaxZ()) * dirz;
	float height2 = GetHeight(int(xpos),int(zpos));

	float dist = obj.MaxZ()*dirz - obj.MinZ()*dirz;
	if ( dist!=0 )
	{
		height2 = height2 - height1;
		angle = float((atan( height2 / dist ) / kPI) * 180.0f);
	}

	return angle;
};

//==========================================================================

float TLandscape::GetZAngle( TObject& obj )
{
	float cx,cy,cz;
	float dirx, dirz;
	float angle = 0.0f;

	obj.GetCenter(cx,cy,cz);

	obj.GetCenter(cx,cy,cz);
	Movement( obj.Xangle(), dirx, dirz );

	float xpos = (obj.X()+obj.MinX()) * dirx;
	float zpos = (obj.Z()+cz) * dirz;
	float height1 = GetHeight(int(xpos),int(zpos));

	xpos = (obj.X()+obj.MaxX()) * dirx;
	float height2 = GetHeight(int(xpos),int(zpos));

	float dist = obj.MaxX()*dirx - obj.MinX()*dirx;
	if ( dist!=0 )
	{
		height2 = height2 - height1;
		angle = float((atan( height2 / dist ) / kPI) * 180.0f);
	}

	return angle;
};

//==========================================================================
