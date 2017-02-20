#include <precomp_header.h>

#include <win32/win32.h>
#include <object/binobject.h>
#include <object/mesh.h>
#include <object/anim.h>
#include <object/tank.h>

//==========================================================================

const float kPI = 3.1415927f;
const float degToRad = 0.01745329252033f;
const float radToDeg = 57.2957795112730f;

//==========================================================================

class AnimationCache
{
public:
	AnimationCache( void );
	AnimationCache( const TAnimation& );
	~AnimationCache( void );

	void AddAnimation( const TAnimation& );
	const TAnimation* GetAnimation( const TString& fname ) const;

private:
	AnimationCache*		next;
	const TAnimation	animation;

public:
	static AnimationCache first;
};


AnimationCache AnimationCache::first;

AnimationCache::AnimationCache( void )
	: next(NULL)
{
};


AnimationCache::AnimationCache( const TAnimation& t )
	: next(NULL),
	  animation( t )
{
};


AnimationCache::~AnimationCache( void )
{
	next = NULL;
};


const TAnimation* AnimationCache::GetAnimation( const TString& fname ) const
{
	AnimationCache* list = next;
	while ( list!=NULL )
	{
		if ( stricmp(fname.c_str(),list->animation.Name().c_str())==0 )
			return &list->animation;
		list = list->next;
	}
	return NULL;
};

	
void AnimationCache::AddAnimation( const TAnimation& t )
{
	if ( next==NULL )
	{
		next = new AnimationCache( t );
	}
	else
	{
		AnimationCache* list = next;
		bool found = false;
		while ( list->next!=NULL && !found )
		{
			if ( stricmp(t.Name().c_str(),list->animation.Name().c_str())==0 )
			{
				found = true;
			}
			else
				list = list->next;
		}
		if ( !found )
			list->next = new AnimationCache( t );
	}
};

//==========================================================================

TAnimation::TAnimation( void )
	: object(NULL)
{
	signature = "PDVANM02";
	initialised = false;

	animationType = 0;
	animationDirection = 1;
	animationCount = 0;
	animationIndex = 0;
	animationFPS = 0;
	animationMS = 0;
	animationTickCount = 0;
	animationPaused = false;
	animationDone = true;

	mx = 0.0f;
	my = 0.0f;
	mz = 0.0f;

	x1 = 0;
	y1 = 0;
	z1 = 0;
	x2 = 0;
	y2 = 0;
	z2 = 0;
	x3 = 0;
	y3 = 0;
	z3 = 0;
	x4 = 0;
	y4 = 0;
	z4 = 0;
};


TAnimation::TAnimation( const TAnimation& a )
	: object(NULL)
{
	signature = "PDVANM01";
	initialised = false;
	operator=(a);
};


const TAnimation& TAnimation::operator=( const TAnimation& a )
{
	initialised = a.initialised;
	name = a.name;

	x1 = a.x1;
	y1 = a.y1;
	z1 = a.z1;
	x2 = a.x2;
	y2 = a.y2;
	z2 = a.z2;
	x3 = a.x3;
	y3 = a.y3;
	z3 = a.z3;
	x4 = a.x4;
	y4 = a.y4;
	z4 = a.z4;

	// anim vars
	animationDone = a.animationDone;
	animationType = a.AnimationType;
	animationDirection = a.animationDirection;
	animationCount = a.animationCount;
	animationIndex = a.animationIndex;
	animationFPS = a.animationFPS;
	animationMS = a.animationMS;
	animationTickCount = a.animationTickCount;
	animationPaused = a.animationPaused;

	object = a.object;

	return *this;
};
	
	
TAnimation::~TAnimation( void )
{
	object = NULL; // will have been destructed by the cache
	initialised = false;
};


bool TAnimation::LoadBinary( const TString& fname, TString& errStr, 
							 const TString& pathname,
							 const TString& texturePath )
{
	object = TBinObject::GetObject( fname, errStr, pathname, texturePath );
	if ( object!=NULL )
	{
		SizeX( object->SizeX() );
		SizeY( object->SizeY() );
		SizeZ( object->SizeZ() );
		initialised = true;
		animationCount = object->NumMeshes();
	}
	return initialised;
};


bool TAnimation::SaveBinary( const TString& fname )
{
	PreCond( object!=NULL );
	return object->SaveBinary( fname );
};


void TAnimation::SetPos( float x, float y, float z )
{
	mx = x;
	my = y;
	mz = z;
}


void TAnimation::Draw( bool showBoundingBox )
{
	if ( !initialised || animationDone )
		return;

	// animate?
	long tc = ::GetTickCount();
	if ( tc>animationTickCount && !animationPaused )
	{
		NextAnimation();
		animationTickCount = tc + animationMS;
	}

	// get animationIndex-th mesh
	TMesh* mesh = object->GetMesh( size_t(animationIndex) );
	PostCond( mesh!=NULL );

	glPushMatrix();
		glTranslatef( mx,
					  my,
					  mz );
/*
		float dy = ((y1+y2)*0.5f-(y4+y3)*0.5f);
		float xa = 0;
		if ( SizeX()!=0 )
			xa = -float(atan( dy / (SizeX()*2) )) * radToDeg;
		else
			xa = Xangle();
		dy = y1-y2;
		float za = 0;
		if ( SizeZ()!=0 )
			za = float(atan( dy / (SizeZ()*2) )) * radToDeg;
		else
			za = Zangle();
*/
		glRotatef( -Yangle() + 90.0f , 0,1,0);
		glRotatef( Xangle(), 0,0,1 );
		glRotatef( Zangle(), 1,0,0 );
		
		mesh->Draw();

	glPopMatrix();
};


size_t TAnimation::NextAnimation( void )
{
	if ( animationDone )
		return animationIndex;

	switch ( animationType )
	{
		case 0:
			{
				animationIndex++;
				if ( animationIndex>=animationCount )
				{
					animationDone = true;
					animationIndex = animationCount - 1;
				}
			}
			break;

		case 1:
			{
				animationIndex++;
				if ( animationIndex>=animationCount )
				animationIndex = 0;
			}
			break;
		case -2:
		case 2:
			{
				animationIndex += animationDirection;
				if ( animationDirection==1 && animationIndex>=animationCount )
				{
					animationIndex = animationCount - 2;
					animationDirection = -1;
				}
				else if ( animationDirection==-1 && animationIndex==0 )
				{
					animationDirection = 1;
				}
			}
			break;

		case -1:
			{
				animationIndex--;
				if ( (animationIndex+1)==0 )
				{
					animationIndex = animationCount - 1;
				}
			}
			break;
	};
	return animationIndex;
};


size_t TAnimation::PreviousAnimation( void )
{
	if ( animationDone )
		return animationIndex;

	animationIndex--;
	if ( (animationIndex+1)==0 )
	{
		animationIndex = animationCount - 1;
	}
	return animationIndex;
};


void TAnimation::NextAnimation( size_t next )
{
	animationIndex = next;
};


int	TAnimation::AnimationType( void )
{
	return animationType;
};


void TAnimation::AnimationType( int _type )
{
	animationType = _type;
	switch ( _type )
	{
		case 0:
		case 1:
		case 2:
			{
				animationDirection = 1;
				animationDone = false;
			}
			break;

		default:
			animationDirection = -1;
	};
};


void TAnimation::AnimationFPS( size_t animFPS )
{
	PreCond( animFPS!=0 );

	animationFPS = animFPS;
	animationMS = 1000 / animFPS;
};


size_t TAnimation::AnimationFPS( void )
{
	return animationFPS;
};


size_t TAnimation::AnimationCount( void )
{
	return animationCount;
};


void TAnimation::PauseAnimation( bool tf )
{
	animationPaused = tf;
};


bool TAnimation::AnimationPaused( void ) const
{
	return animationPaused;
};


void TAnimation::StartAnimation( size_t index, size_t fps )
{
	NextAnimation( index );
	AnimationFPS( fps );
	AnimationType( 0 );
	animationPaused = false;
	animationDone = false;
};


bool TAnimation::AnimationDone( void ) const
{
	return animationDone;
};


void TAnimation::AnimationDone( bool _ad )
{
	animationDone = _ad;
};


size_t TAnimation::AnimationIndex( void ) const
{
	return animationIndex;
};


bool TAnimation::CreateAnimation( const TString& animFname, const TString& texturePath,
								  size_t numAnims, TString& errStr )
{
	if ( object!=NULL )
		delete object;
	object = new TBinObject();

	TString path,animTemplate;
    size_t len = animFname.length();
    while ( animFname[len]!='\\' && len>0 ) len--;
    if ( animFname[len]=='\\' )
    {
        path = const_cast<TString&>(animFname).substr(0,len);		
		animTemplate = &animFname[len+1];
    }
    else
	{
    	path = "";
		animTemplate = animFname;
	}

	for ( size_t i=0; i<numAnims; i++ )
	{
		TString fname;
		fname = FormatString( animTemplate.c_str(), i );
		TBinObject temp;
		if ( !temp.LoadBinary( fname,errStr,path,texturePath) )
			return false;
		
		// copy materials
		for ( size_t j=0; j<temp.NumMaterials(); j++ )
		{
			TMaterial* mat = temp.GetMaterial(j);
			PostCond( mat!=NULL );
			PostCond( mat->MaterialName()!=NULL );
			TMaterial* exists = object->GetMaterial(mat->MaterialName());
			if ( exists==NULL )
			{
				object->AddMaterial( *mat );
			}
		}
		temp.MaterialList( NULL );

		// copy single mesh
		TMesh* m0 = temp.GetMesh(size_t(0));
		PostCond( m0!=NULL );
		object->AddMesh( *m0 );
		WriteString( "Adding mesh(0) of %s\n", fname );
		temp.meshList = NULL; // prevent from being deleted
	}

	initialised = true;
	return true;
};


void TAnimation::MX( const float& _mx )
{
	mx = _mx;
};


void TAnimation::MY( const float& _my )
{
	my = _my;
};


void TAnimation::MZ( const float& _mz )
{
	mz = _mz;
};


const TString& TAnimation::Name( void ) const
{
	return name;
}


float TAnimation::SizeX( void ) const
{
	return xSize;
};


float TAnimation::SizeY( void ) const
{
	return ySize;
};


float TAnimation::SizeZ( void ) const
{
	return zSize;
};


void TAnimation::SizeX( float sx )
{
	xSize = sx;
};


void TAnimation::SizeY( float sy )
{
	ySize = sy;
};


void TAnimation::SizeZ( float sz)
{
	zSize = sz;
};

//==========================================================================

