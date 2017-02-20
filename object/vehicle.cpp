#include <precomp_header.h>

#include <win32/events.h>
#include <object/vehicle.h>

//#include <object/shadows.h>

//==========================================================================

const float radToDeg = 57.29577951f;
const float degToRad = 0.017453293f;

//==========================================================================

TVehicle::TVehicle( void )
	: soundSystem( NULL ),
	  app( NULL )
{
	type = vtInvalid;
	x = y = z = 0;
	n_x = n_y = n_z = 0;
	xangle = yangle = zangle = 0;
	n_xangle = n_yangle = n_zangle = 0;
	scalex = scaley = scalez = 1;

	strength = 100;
	strengthUpdated = true;
	currentIndex = 0;
	gameImportance = 0;
	isaAI = false;
	aiType = 0;
	aiId = 0;
	iconId = 0;
	team = 0;

	xzRadius = 0;
	xzAngle = 0;

	float h = 500;
	x1 = y1 = z1 = h;
	x2 = y2 = z2 = h;
	x3 = y3 = z3 = h;
	x4 = y4 = z4 = h;

	n_x1 = n_y1 = n_z1 = h;
	n_x2 = n_y2 = n_z2 = h;
	n_x3 = n_y3 = n_z3 = h;
	n_x4 = n_y4 = n_z4 = h;
};

TVehicle::TVehicle( size_t vt )
	: soundSystem( NULL ),
	  app( NULL )
{
	type = vt;
	x = y = z = 0;
	n_x = n_y = n_z = 0;
	xangle = yangle = zangle = 0;
	n_xangle = n_yangle = n_zangle = 0;
	scalex = scaley = scalez = 1;

	strength = 100;
	strengthUpdated = true;
	currentIndex = 0;
	gameImportance = 0;
	isaAI = false;
	aiType = 0;
	aiId = 0;
	iconId = 0;
	team = 0;

	xzRadius = 0;
	xzAngle = 0;

	float h = 500;

	x1 = y1 = z1 = h;
	x2 = y2 = z2 = h;
	x3 = y3 = z3 = h;
	x4 = y4 = z4 = h;

	n_x1 = n_y1 = n_z1 = h;
	n_x2 = n_y2 = n_z2 = h;
	n_x3 = n_y3 = n_z3 = h;
	n_x4 = n_y4 = n_z4 = h;
};

TVehicle::TVehicle( const TVehicle& v )
	: soundSystem( NULL ),
	  app( NULL )
{
	operator=( v );
};

const TVehicle& TVehicle::operator = ( const TVehicle& v )
{
	type = v.type;

	app = v.app;
	soundSystem = v.soundSystem;

	x = v.x;
	y = v.y;
	z = v.z;

	n_x = v.n_x;
	n_y = v.n_y;
	n_z = v.n_z;

	xangle = v.xangle;
	yangle = v.yangle;
	zangle = v.zangle;

	n_xangle = v.n_xangle;
	n_yangle = v.n_yangle;
	n_zangle = v.n_zangle;

	scalex = v.scalex;
	scaley = v.scaley;
	scalez = v.scalez;
	scale = v.scale;

	strength = v.strength;
	strengthUpdated = v.strengthUpdated;
	gameImportance = v.gameImportance;
	isaAI = v.isaAI;
	aiType = v.aiType;
	aiId = v.aiId;
	iconId = v.iconId;
	team = v.team;

	currentIndex = v.currentIndex;
	object = v.object;
	name = v.name;

	xzRadius = v.xzRadius;
	xzAngle = v.xzAngle;

	x1 = v.x1;
	x2 = v.x2;
	x3 = v.x3;
	x4 = v.x4;

	y1 = v.y1;
	y2 = v.y2;
	y3 = v.y3;
	y4 = v.y4;

	z1 = v.z1;
	z2 = v.z2;
	z3 = v.z3;
	z4 = v.z4;

	n_x1 = v.n_x1;
	n_x2 = v.n_x2;
	n_x3 = v.n_x3;
	n_x4 = v.n_x4;

	n_y1 = v.n_y1;
	n_y2 = v.n_y2;
	n_y3 = v.n_y3;
	n_y4 = v.n_y4;

	n_z1 = v.n_z1;
	n_z2 = v.n_z2;
	n_z3 = v.n_z3;
	n_z4 = v.n_z4;

	return *this;
};

TVehicle::~TVehicle( void )
{
	type = vtInvalid;
};

size_t TVehicle::VehicleType( void ) const
{
	return type;
};

void TVehicle::VehicleType( size_t vt )
{
	type = vt;
};

float TVehicle::X( void ) const
{
	return x;
};

void TVehicle::X( float _x )
{
	n_x = _x;
	x = _x;
};

float TVehicle::Y( void ) const
{
	return y;
};

void TVehicle::Y( float _y )
{
	n_y = _y;
	y = _y;
};

float TVehicle::Z( void ) const
{
	return z;
};

void TVehicle::Z( float _z )
{
	n_z = _z;
	z = _z;
};

float TVehicle::Xangle( void ) const
{
	return xangle;
};

void TVehicle::Xangle( float _xangle )
{
	n_xangle = _xangle;
	xangle = _xangle;
};

float TVehicle::Yangle( void ) const
{
	return yangle;
};

void TVehicle::Yangle( float _yangle )
{
	n_yangle = _yangle;
	yangle = _yangle;
};

float TVehicle::Zangle( void ) const
{
	return zangle;
};

void TVehicle::Zangle( float _zangle )
{
	n_zangle = _zangle;
	zangle = _zangle;
};

void TVehicle::CommitMove( void )
{
	x = n_x;
	y = n_y;
	z = n_z;

	xangle = n_xangle;
	yangle = n_yangle;
	zangle = n_zangle;

	x1 = n_x1;
	y1 = n_y1;
	z1 = n_z1;

	x2 = n_x2;
	y2 = n_y2;
	z2 = n_z2;

	x3 = n_x3;
	y3 = n_y3;
	z3 = n_z3;

	x4 = n_x4;
	y4 = n_y4;
	z4 = n_z4;
};

void TVehicle::CommitAngles( void )
{
	xangle = n_xangle;
	yangle = n_yangle;
	zangle = n_zangle;
};

void TVehicle::AbortMove( void )
{
	n_x = x;
	n_y = y;
	n_z = z;

	n_xangle = xangle;
	n_yangle = yangle;
	n_zangle = zangle;

	n_x1 = x1;
	n_y1 = y1;
	n_y1 = y1;

	n_x2 = x2;
	n_y2 = y2;
	n_y2 = y2;

	n_x3 = x3;
	n_y3 = y3;
	n_y3 = y3;

	n_x4 = x4;
	n_y4 = y4;
	n_y4 = y4;
};

float TVehicle::ScaleX( void ) const
{
	return scalex;
};

void TVehicle::ScaleX( float _scalex )
{
	scalex = _scalex;
};

float TVehicle::ScaleY( void ) const
{
	return scaley;
};

void TVehicle::ScaleY( float _scaley )
{
	scaley = _scaley;
};

float TVehicle::ScaleZ( void ) const
{
	return scalez;
};

void TVehicle::ScaleZ( float _scalez )
{
	scalez = _scalez;
};

float TVehicle::SizeX( void ) const
{
	if ( object.ValidObject() )
		return object.SizeX(currentIndex) * scalex;
	else
		return 1;
};


float TVehicle::SizeY( void ) const
{
	if ( object.ValidObject() )
		return object.SizeY(currentIndex) * scaley;
	else
		return 1;
};


float TVehicle::SizeZ( void ) const
{
	if ( object.ValidObject() )
		return object.SizeZ(currentIndex) * scalez;
	else
		return 1;
};

bool TVehicle::IsaAI( void ) const
{
	return isaAI;
};


void TVehicle::IsaAI( bool d )
{
	isaAI = d;
};

size_t TVehicle::AIType( void ) const
{
	return aiType;
};


void TVehicle::AIType( size_t d )
{
	aiType = d;
};

size_t TVehicle::GameImportance( void ) const
{
	return gameImportance;
};

void TVehicle::GameImportance( size_t d )
{
	gameImportance = d;
};

void TVehicle::SetSoundSystem( TSound* _soundSystem )
{
	soundSystem = _soundSystem;
}

void TVehicle::SetApp( TEvent* _app )
{
	app = _app;
}

size_t TVehicle::AIID( void ) const
{
	return aiId;
};

void TVehicle::AIID( size_t _aiId )
{
	aiId = _aiId;
};

size_t TVehicle::IconId( void ) const
{
	return iconId;
};

void TVehicle::IconId( size_t _iconId )
{
	iconId = _iconId;
};

size_t TVehicle::Team( void ) const
{
	return team;
};

void TVehicle::Team( size_t _team )
{
	team = _team;
};

size_t TVehicle::CurrentIndex( void ) const
{
	return currentIndex;
};

void TVehicle::CurrentIndex( size_t _currentIndex )
{
	currentIndex = _currentIndex;
};

void TVehicle::operator=( TCompoundObject& o )
{
	object = o;
};

void TVehicle::operator=( const TLandscapeObject& o )
{
	// can be null in case of characters
	if ( o.Object()!=NULL )
		object = *o.Object();

	strength = 100;
	strengthUpdated = true;

	X( o.TX() );
	Y( o.TY() );
	Z( o.TZ() );
	
	Xangle( o.RX() );
	Yangle( o.RY() );
	Zangle( o.RZ() );

	n_yangle = yangle;

	scalex = o.ScaleX();
	scaley = o.ScaleY();
	scalez = o.ScaleZ();
	scale = scalex;
	if ( scaley > scale )
		scale = scaley;
	if ( scalez > scale )
		scale = scalez;

	name = o.Name();

	isaAI = o.IsaAI();
	aiType = o.AIType();
	gameImportance = o.GameImportance();

	SetupObject();
	GetCornerPoints();
};

void TVehicle::SetupObject( void )
{
	if ( object.ValidObject() )
	{
		float xSize = object.SizeX(currentIndex);
		float zSize = object.SizeZ(currentIndex);
		xzRadius = float(sqrt(xSize*xSize+zSize*zSize));
		if ( xSize!=0.0 )
			xzAngle = float(atan(fabs(xSize)/fabs(zSize))) * radToDeg;
		else
			xzAngle = 90.0f;
	}
	else
	{
		xzRadius = 1;
		xzAngle = 0;
	}
};

void TVehicle::GetCornerPoints( void )
{
	float yangle1;
	float offsetX, offsetZ;
	float adjust = 0.95f;
	float rad1 = xzRadius*adjust*scalex;
	float rad2 = xzRadius*adjust*scalez;

    yangle1 = (-n_yangle+xzAngle)*degToRad;
	offsetX = -float(sin(yangle1)) * rad1;
	offsetZ =  float(cos(yangle1)) * rad2;

	n_x1 = n_x + offsetX;
	n_z1 = n_z + offsetZ;
	n_x3 = n_x - offsetX;
	n_z3 = n_z - offsetZ;

    yangle1 = (-n_yangle-xzAngle)*degToRad;
	offsetX = -float(sin(yangle1)) * rad1;
	offsetZ =  float(cos(yangle1)) * rad2;

	n_x2 = n_x + offsetX;
	n_z2 = n_z + offsetZ;
	n_x4 = n_x - offsetX;
	n_z4 = n_z - offsetZ;
};

size_t TVehicle::Strength( void ) const
{
	return strength;
};

void TVehicle::Strength( size_t _strength )
{
	strength = _strength;
};

bool TVehicle::StrengthUpdated( void ) const
{
	return strengthUpdated;
};

void TVehicle::StrengthUpdated( bool _strengthUpdated )
{
	strengthUpdated = _strengthUpdated;
};

const TString& TVehicle::Name( void ) const
{
	return name;
};

void TVehicle::Name( const TString& _name )
{
	name = _name;
};

TCompoundObject& TVehicle::Object( void )
{
	return object;
}

size_t TVehicle::netSet( size_t myId, size_t vid, byte* data )
{
	PreCond( data!=NULL );

	data[0] = id_vehicle;
	data[1] = myId;
	data[2] = vid;
	data[3] = strength;
	data[4] = aiId;

	float* f = (float*)&data[5];
	f[0] = x;
	f[1] = y;
	f[2] = z;

	f[3] = xangle;
	f[4] = yangle;
	f[5] = zangle;

	return netSize();
};

size_t TVehicle::netSize( void ) const
{
	return (5 + 6*sizeof(float));
};

size_t TVehicle::netGet( const byte* data )
{
	PreCond( data!=NULL );

	if ( strength!=data[3] )
	{
		strengthUpdated = true;
	}
	strength = data[3];
	aiId = data[4];

	float* f = (float*)&data[5];
	x = f[0];
	y = f[1];
	z = f[2];

	xangle = f[3];
	yangle = f[4];
	zangle = f[5];

	return netSize();
}

void TVehicle::DrawShadow( size_t shadowLevel )
{
	if ( shadowLevel==0 )
	{
		return;
	};

	float height = 0.1f;

	if ( object.NumShadowFaces() > 0 )
	{
		size_t numFaces = object.NumShadowFaces();
		size_t numVertices = object.NumShadowVertices();
		size_t* surfaces = object.ShadowFaces();
		float*  vertices = object.ShadowVertices();
		float   sscale = object.ShadowScale();
		TLandscape* landObj = app->LandObj();

		glPushMatrix();

			glEnable(GL_BLEND);
			glEnable(GL_COLOR_MATERIAL);
			glColor4ub( 0,0,0,150 );

			float m[16];
			glPushMatrix();
				glLoadIdentity();
				glTranslatef( x, 0, z );
				glRotatef( yangle + object.ShadowAngle(), 0,1,0 );
				glGetFloatv( GL_MODELVIEW_MATRIX, m );
			glPopMatrix();

			glBegin( GL_TRIANGLES );

			for ( size_t i=0; i<numFaces; i++ )
			{
				size_t s[3];
				s[0] = surfaces[i*3+0];
				s[1] = surfaces[i*3+1];
				s[2] = surfaces[i*3+2];

				float v1[3][3];
				float vt1[3][3];

				for ( size_t j=0; j<3; j++ )
				{

					vt1[j][0] = vertices[s[j]*2+0] * scale * sscale;
					vt1[j][1] = 0;
					vt1[j][2] = vertices[s[j]*2+1] * scale * sscale;

					MatrixMult( m, vt1[j], v1[j] );

					v1[j][1] = Y() + 1;

					if ( shadowLevel==1 )
					{
						landObj->GetY( v1[j][0], v1[j][1], v1[j][2] );
					}
					else
					{
						landObj->GetBestY( v1[j][0], v1[j][1], v1[j][2] );
					}
					v1[j][1] += height;
				}

				glVertex3fv( v1[0] );
				glVertex3fv( v1[1] );
				glVertex3fv( v1[2] );

				glVertex3fv( v1[2] );
				glVertex3fv( v1[1] );
				glVertex3fv( v1[0] );
			}

			glEnd();

			glDisable(GL_BLEND);

		glPopMatrix();

	}
};

//==========================================================================

