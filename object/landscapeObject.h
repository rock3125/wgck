#ifndef __LANDSCAPE_OBJECT_H_
#define __LANDSCAPE_OBJECT_H_

//==========================================================================

#include <object/compoundObject.h>
#include <object/landscapeObject.h>

//==========================================================================

class TCompoundObject;
class TApp;

//==========================================================================

class _EXPORT TLandscapeObject
{
public:
	TLandscapeObject( const TString& name, size_t id,
					  float tx, float ty, float tz,
					  float rx, float ry, float rz );

	TLandscapeObject( void );
	TLandscapeObject( const TLandscapeObject& );
	~TLandscapeObject( void );

	const TLandscapeObject& operator=( const TLandscapeObject& );

	size_t			Id( void ) const;
	void			Id( size_t );

	void			Draw( void ) const;
	void			Draw( float scaleX, float scaleY, float scaleZ ) const;
	void			Draw( bool ) const;

	const TString&	Name( void ) const;
	void			Name( const TString& _name );

	void			SaveBinary( TPersist& file ) const;
	bool			LoadBinary( TPersist& file, TString& errStr );

	const TCompoundObject* Object( void ) const;
	void			Object( const TCompoundObject* obj );

	void			SetTranslation( float x, float y, float z );
	void			GetTranslation( float& x, float& y, float& z ) const;

	void			SetRotation( float x, float y, float z );
	void			GetRotation( float& x, float& y, float& z ) const;

	void			SetScale( float x, float y, float z );
	void			GetScale( float& x, float& y, float& z ) const;

	bool			InsideObstacle( float x, float y, float z ) const;
//	bool			InsideObstacle( float x, float z ) const;

	const float&	TX( void ) const;
	const float&	TY( void ) const;
	const float&	TZ( void ) const;

	void			TX( float _tx );
	void			TY( float _ty );
	void			TZ( float _tz );

	float			SizeX( void ) const;
	float			SizeY( void ) const;
	float			SizeZ( void ) const;

	float			RX( void ) const;
	float			RY( void ) const;
	float			RZ( void ) const;

	void			RX( float r );
	void			RY( float r );
	void			RZ( float r );

	float			ScaleX( void ) const;
	float			ScaleY( void ) const;
	float			ScaleZ( void ) const;

	void			ScaleX( float s );
	void			ScaleY( float s );
	void			ScaleZ( float s );

	size_t			MaxIndex( void ) const;

	size_t			CurrentIndex( void ) const;
	void			CurrentIndex( size_t _currentIndex );

	bool			IsaAI( void ) const;
	void			IsaAI( bool );

	size_t			AIType( void ) const;
	void			AIType( size_t _aiType );

	size_t			AITeam( void ) const;
	void			AITeam( size_t _aiTeam );

	size_t			GameImportance( void ) const;
	void			GameImportance( size_t _gameImportance );

	size_t			Strength( void ) const;
	void			Strength( size_t _strength );

	size_t			Points( void ) const;
	void			Points( size_t _points );

	size_t			Bombs( void ) const;
	void			Bombs( size_t _bombs );

	size_t			V2Countdown( void ) const;
	void			V2Countdown( size_t d );

	size_t			Shells( void ) const;
	void			Shells( size_t _shells );

	size_t			CharacterType( void ) const;
	void			CharacterType( size_t _characterType );

	// cheap and fast is the objects footprint on the landscape
	// intersected by this line across the landscape (all at y=0)
//	bool LineInsideObject( float x1, float z1, float x2, float z2 ) const;
	bool InsideObject( float x1, float z1 ) const;
	bool InsideObject( float* x1, float* z1 ) const;

	// collision detection using this object
	bool			Collision( float l1x, float l1y, float l1z, 
							   float l2x, float l2y, float l2z ) const;
	bool			Collision( float l1x, float l1z,
							   float l2x, float l2z ) const;

	// surface graphing using this object
	bool			GetLORTriangle( float xp, float yp, float zp, 
									float size, float& y );

	// get physical position of item
	void			GetBounds( float* min, float* max ) const;
	void			GetBounds( float& minx, float& miny, float& minz,
							   float& maxx, float& maxy, float& maxz ) const;
	void			GetGround( float&x1, float& z1, float& x2, float& z2,
							   float&x3, float& z3, float& x4, float& z4 ) const;
	void			GetCorners( float& x1, float& y1, float& z1, 
								float& x2, float& y2, float& z2,
								float& x3, float& y3, float& z3,
								float& x4, float& y4, float& z4,
								float& x5, float& y5, float& z5,
								float& x6, float& y6, float& z6,
								float& x7, float& y7, float& z7,
								float& x8, float& y8, float& z8 ) const;
								
	void			DrawBoundingBox( float r, float g, float b );

	bool			Destructable( void ) const;
	void			Destructable( bool );

	bool			Armory( void ) const;
	void			Armory( bool );

	bool			Repairs( void ) const;
	void			Repairs( bool );

	bool			RefuelStation( void ) const;
	void			RefuelStation( bool );

	bool			StrengthUpdated( void ) const;
	void			StrengthUpdated( bool );

	// refresh internally cached transformation matrices
	void			Refresh( void );

	// object id
	static bool IsPlant( const TString& name );
	static bool IsStructure( const TString& name );
	static bool IsTeam( const TString& name );
	static bool IsFlak( const TString& name );
	static bool IsArtillery( const TString& name );
	static bool IsV2( const TString& name );
	static bool IsV2TargetMarker( const TString& name );
	static TString FlagNameToTeamName( const TString& name );

	// calculate & process damage of projectile exploding close to me
	void CalculateDamage( TApp* app, size_t damage );
	void CalculateDamage( TApp* app, float expx, float expy, float expz, 
						  float damage );

private:
	// update matrix for transformation purposes
	void GetTransformationMatrix( void );

	// pre-post calculate bounds
	void CalculateBounds( void );

	// turn strength rating into a currentIndex <-> maxIndex combo
	void StrengthToIndex( void );

private:
	friend class TLandscapeObjectDraw;

	TString				name;			// mesh common name

	float tx,ty,tz;
	float rx,ry,rz;
	float scaleX,scaleY,scaleZ;

	float b1x,b1y,b1z;
	float b2x,b2y,b2z;
	float b3x,b3y,b3z;
	float b4x,b4y,b4z;
	float b5x,b5y,b5z;
	float b6x,b6y,b6z;
	float b7x,b7y,b7z;
	float b8x,b8y,b8z;

	float minx,miny,minz;
	float maxx,maxy,maxz;

	size_t				id;

	mutable size_t		currentIndex;
	size_t				maxIndex;
	bool				destructable;		// can I be destroyed?
	bool				isaAI;				// am I an AI?
	bool				strengthUpdated;	// strength updated?
	size_t				aiType;				// ai type for vehicles
	size_t				aiTeam;				// ai team (0==hostile, 1==friendly)
	size_t				gameImportance;		// my role in the game
	size_t				strength;			// my strength
	size_t				points;				// # of points for destroying me
	size_t				bombs;				// # of bombs on planes
	size_t				shells;				// # of rockets on planes, shells on tanks
	size_t				characterType;		// character type for flags/teams

	// is this item one of these?
	bool				armory;
	bool				refuelStation;
	bool				repairs;

	float				matrix[16];

	const TCompoundObject* obj;
};

//==========================================================================

class _EXPORT TLandscapeObjectDraw
{
public:
	TLandscapeObjectDraw( TLandscapeObject& obj );
	TLandscapeObjectDraw( void );
	TLandscapeObjectDraw( const TLandscapeObjectDraw& );

	const TLandscapeObjectDraw& operator=( const TLandscapeObjectDraw& );

	~TLandscapeObjectDraw( void );

	void Draw( void ) const;
	void Draw( bool drawTransp ) const;

	TLandscapeObjectDraw*	Next( void ) const;
	void		Next( TLandscapeObjectDraw* _next );

	bool InsideObject( float x1, float y1, float z1 );
	bool InsideObject( float x1, float z1 );
	bool InsideObject( float x1, float y1, float z1,
					   float x2, float y2, float z2 );
	bool InsideObject( float* x1, float* z1 );

	bool GetLORTriangle( float xp, float yp, float zp, float size, float& y );

	bool Collision( float l1x, float l1y, float l1z,
				    float l2x, float l2y, float l2z ) const;

	void GetBounds( float& minx, float& miny, float& minz,
					float& maxx, float& maxy, float& maxz );
	void DrawBoundingBox( float r, float g, float b );

	const TString& Name( void ) const;

private:
	friend class TLandscape;

	TLandscapeObject* obj;
	TLandscapeObjectDraw* next;
};

//==========================================================================

#endif

