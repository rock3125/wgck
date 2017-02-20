#ifndef __OBJECT_COMPOUND_OBJECT_H__
#define __OBJECT_COMPOUND_OBJECT_H__

//==========================================================================

#include <object/object.h>

//==========================================================================

class TCompoundObject
{
public:
	TCompoundObject( void );
	~TCompoundObject( void );
	TCompoundObject( const TCompoundObject& );

	const TCompoundObject& operator=( const TCompoundObject& );

	bool SaveBinary( const TString& name, const TString& texturePath ) const;

	bool LoadBinary( const TString& fname, TString& errStr,
					 const TString& path, const TString& texturePath );

//	TObject&	operator[] ( size_t index );
//	TObject&	operator() ( void );

	void				Draw( size_t currentObject ) const;

	const TCompoundObject&	Object( void ) const;
	TCompoundObject&		Object( void );

	// return array of TObjects
	TObject*				Objects( void ) const;

	const size_t&		NumObjects( void ) const;

	void				PauseAnimations( bool p );

	float				SizeX( size_t currentObject ) const;
	float				SizeY( size_t currentObject ) const;
	float				SizeZ( size_t currentObject ) const;

	bool Create( const TString& name, size_t cntr,
				 const TString& texturePath, 
				 const TString* names );

    float	X( size_t currentObject ) const;
    void	X( float _x );

    float	Y( size_t currentObject ) const;
    void	Y( float _y );

    float	Z( size_t currentObject ) const;
    void	Z( float _z );

    float	Xangle( size_t currentObject ) const;
    void	Xangle( float _xangle );

    float	Yangle( size_t currentObject ) const;
    void	Yangle( float _yangle );

    float	Zangle( size_t currentObject ) const;
    void	Zangle( float _zangle );

	float	CenterX( size_t currentObject ) const;
	float	CenterY( size_t currentObject ) const;
	float	CenterZ( size_t currentObject ) const;

	float	MinX( size_t currentObject ) const;
	float	MinY( size_t currentObject ) const;
	float	MinZ( size_t currentObject ) const;

	float	MaxX( size_t currentObject ) const;
	float	MaxY( size_t currentObject ) const;
	float	MaxZ( size_t currentObject ) const;

	size_t	NumMeshes( size_t currentObject ) const;
	TMesh*	GetMesh( size_t currentObject, size_t index ) const;

	bool	ValidObject( void ) const;

private:
	void ClearObject( void );

private:
	TString		signature;

	TObject*	objects;
	size_t		numObjects;
	bool		objectValid;
};

//==========================================================================

#endif

