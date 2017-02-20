#ifndef __DXPARSER_OBJECT_ANIM_H_
#define __DXPARSER_OBJECT_ANIM_H_

//==========================================================================

#include <object/object.h>

class TVehicle;

//==========================================================================

class TAnimation : public TObject
{
public:
	TAnimation( void );
	~TAnimation( void );

	TAnimation( const TAnimation& );
	const TAnimation& operator=( const TAnimation& );

    bool LoadBinary( const TString& fname, TString& errStr, 
					 const TString& pathname,
					 const TString& texturePath );
	bool SaveBinary( const TString& fname );

	virtual void Draw( bool showBoundingBox = false );

	bool CreateAnimation( const TString& animFname, const TString& texturePath,
						  size_t numAnims, TString& errStr );

	void SetOrientationToVehicle( TVehicle& obj );
	void SetPos( float x, float y, float z );

	// animation set for meshes
	virtual void	StartAnimation( size_t index=0, size_t fps=20 );
	virtual void	PauseAnimation( bool tf );
	virtual bool	AnimationPaused( void ) const;
	virtual bool	AnimationDone( void ) const; // is it?
	virtual void	AnimationDone( bool _ad );

	virtual int		AnimationType( void );
	virtual void	AnimationType( int _type );
	
	virtual size_t	AnimationIndex( void ) const;
	virtual size_t	NextAnimation( void );
	virtual void	NextAnimation( size_t next );
	virtual size_t	PreviousAnimation( void );

	virtual size_t	AnimationCount( void );

	virtual void	AnimationFPS( size_t anim );
	virtual size_t	AnimationFPS( void );

	void	MX( const float& _mx );
	void	MY( const float& _my );
	void	MZ( const float& _mz );

	const TString&	Name( void ) const;

private:
	TString		signature;
	TString		name;
	bool		initialised;

	float		mx,my,mz;

	float x1,y1,z1;
	float x2,y2,z2;
	float x3,y3,z3;
	float x4,y4,z4;

	// anim vars
	bool			animationDone;			// type 0 animation not cyclic
	int				animationType;			// type 0,1,2,-1,-2
	int				animationDirection;		// internal use only
	size_t			animationCount;			// number of animations
	size_t			animationIndex;			// current animation index
	size_t			animationFPS;			// fps of animation (sets animationMS)
	long			animationMS;			// anim speed in ms (internal use only)
	long			animationTickCount;		// internal timer
	bool			animationPaused;		// paused yes/no?
};


//==========================================================================

#endif
