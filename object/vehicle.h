#ifndef _OBJECT_VEHICLE_H_
#define _OBJECT_VEHICLE_H_

//==========================================================================

#include <win32/win32.h>
#include <common/sounds.h>

//==========================================================================

class TVehicle
{
public:
	TVehicle( void );
	TVehicle( size_t type );
	TVehicle( const TVehicle& );
	const TVehicle& operator = ( const TVehicle& );

	virtual ~TVehicle( void );

	virtual void	DrawShadow( size_t shadowLevel );

	virtual void operator=( TCompoundObject& o );
	virtual void operator=( const TLandscapeObject& o );

	virtual size_t	VehicleType( void ) const;
	virtual void	VehicleType( size_t vt );

	virtual float	X( void ) const;
	virtual void	X( float _x );

	virtual float	Y( void ) const;
	virtual void	Y( float _y );

	virtual float	Z( void ) const;
	virtual void	Z( float _z );

	virtual float	Xangle( void ) const;
	virtual void	Xangle( float _xangle );

	virtual float	Yangle( void ) const;
	virtual void	Yangle( float _yangle );

	virtual float	Zangle( void ) const;
	virtual void	Zangle( float _zangle );

	virtual float	ScaleX( void ) const;
	virtual void	ScaleX( float _scalex );

	virtual float	ScaleY( void ) const;
	virtual void	ScaleY( float _scaley );

	virtual float	ScaleZ( void ) const;
	virtual void	ScaleZ( float _scalez );

	virtual float	SizeX( void ) const;
	virtual float	SizeY( void ) const;
	virtual float	SizeZ( void ) const;

	virtual void	CommitMove( void );
	virtual void	CommitAngles( void );
	virtual void	AbortMove( void );

	virtual bool	IsaAI( void ) const;
	virtual void	IsaAI( bool d );

	virtual size_t	AIType( void ) const;
	virtual void	AIType( size_t d );

	virtual size_t	AIID( void ) const;
	virtual void	AIID( size_t _aiId );

	virtual size_t	IconId( void ) const;
	virtual void	IconId( size_t _iconId );

	virtual size_t	GameImportance( void ) const;
	virtual void	GameImportance( size_t d );

	virtual size_t	Team( void ) const;
	virtual void	Team( size_t _team );

	virtual void	SetSoundSystem( TSound* _soundSystem );
	virtual void	SetApp( TEvent* _app );

	virtual size_t	CurrentIndex( void ) const;
	virtual void	CurrentIndex( size_t _currentIndex );

	virtual TCompoundObject& Object( void );

	virtual const TString&	Name( void ) const;
	virtual void			Name( const TString& );

	// setup diameters and corner point algorithm
	virtual void	SetupObject( void );
	virtual void	GetCornerPoints( void );

	// return health status of vehicle
	virtual size_t	Strength( void ) const;
	virtual void	Strength( size_t _strength );

	virtual bool	StrengthUpdated( void ) const;
	virtual void	StrengthUpdated( bool _strengthUpdated );

	// get set net data
	virtual size_t	netSet( size_t myId, size_t vid, byte* data );
	virtual size_t	netSize( void ) const;
	virtual size_t	netGet( const byte* data );

protected:
	friend class TLandscape;

	size_t type;

	float x,y,z;
	float n_x, n_y, n_z;
	float xangle, yangle, zangle;
	float n_xangle, n_yangle, n_zangle;
	float scalex, scaley, scalez;

	// corner points
	float x1, x2, x3, x4;
	float y1, y2, y3, y4;
	float z1, z2, z3, z4;

	float n_x1, n_x2, n_x3, n_x4;
	float n_y1, n_y2, n_y3, n_y4;
	float n_z1, n_z2, n_z3, n_z4;

	size_t	strength;
	bool	strengthUpdated;
	size_t	gameImportance;
	bool	isaAI;
	size_t	aiType;
	size_t	aiId;
	size_t	iconId;
	size_t	team;
	float	scale;

	TString name;
	TCompoundObject object;
	size_t currentIndex;

	float xzRadius;
	float xzAngle;

	TSound*	soundSystem;
	TEvent* app;
};

//==========================================================================

#endif
