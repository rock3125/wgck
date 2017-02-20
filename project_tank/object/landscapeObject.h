#ifndef __LANDSCAPE_OBJECT_H_
#define __LANDSCAPE_OBJECT_H_

//==========================================================================

#include <object/compoundObject.h>
#include <object/landscapeObject.h>

//==========================================================================

class TLandscapeObject
{
public:
	TLandscapeObject( const TString& name, 
					  float tx, float ty, float tz,
					  float rx, float ry, float rz );

	TLandscapeObject( void );
	TLandscapeObject( const TLandscapeObject& );
	~TLandscapeObject( void );

	const TLandscapeObject& operator=( const TLandscapeObject& );

	void			Draw( void ) const;

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

	bool			InsideObstacle( float x, float y, float z );
	bool			InsideObstacle( float x, float z ) const;

	const float&	TX( void ) const;
	const float&	TY( void ) const;
	const float&	TZ( void ) const;

	float			SizeX( void ) const;
	float			SizeY( void ) const;
	float			SizeZ( void ) const;

	size_t			MaxIndex( void ) const;

	size_t			CurrentIndex( void ) const;
	void			CurrentIndex( size_t _currentIndex );

private:
	TString				name;			// mesh common name

	float tx,ty,tz;
	float rx,ry,rz;

	size_t				currentIndex;
	size_t				maxIndex;
	float				armour;
	float				damage;

	const TCompoundObject* obj;
};

//==========================================================================

class TLandscapeObjectList
{
public:
	TLandscapeObjectList( const TLandscapeObject& obj );
	TLandscapeObjectList( void );
	~TLandscapeObjectList( void );

	const TLandscapeObject& Object( void ) const;
	void					Object( const TLandscapeObject& obj );

	TLandscapeObjectList*	Next( void ) const;
	void					Next( TLandscapeObjectList* _next );

	void					Append( const TLandscapeObject& obj );
	void					Remove( const TLandscapeObject& obj );

	void					Clear( void );

	TLandscapeObject*		GetObject( const TString& name ) const;

private:
	const TLandscapeObject*	obj;
	TLandscapeObjectList*	next;
};

//==========================================================================

#endif

