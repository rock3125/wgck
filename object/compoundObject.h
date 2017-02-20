#ifndef __OBJECT_COMPOUND_OBJECT_H__
#define __OBJECT_COMPOUND_OBJECT_H__

//==========================================================================

#include <object/binobject.h>
#include <object/geometry.h>

//==========================================================================

class _EXPORT TCompoundObject
{
public:
	TCompoundObject( void );
	~TCompoundObject( void );
	TCompoundObject( const TCompoundObject& );

	const TCompoundObject& operator=( const TCompoundObject& );

	bool SaveBinary( const TString& name, const TString& texturePath ) const;

	bool LoadBinary( const TString& fname, TString& errStr,
					 const TString& path, const TString& texturePath );

	// object shadow management
	bool				WriteShadow( const TString& fname,
									 float angle, float scale,
									 size_t numVertices, float* vertices,
									 size_t numSurfaces, size_t* surfaces,
									 TString& errStr );

	bool				LoadShadow( const TString& fname,
									TString& errStr );

	float				ShadowAngle( void ) const;
	float				ShadowScale( void ) const;
	size_t				NumShadowFaces( void ) const;
	size_t				NumShadowVertices( void ) const;
	size_t*				ShadowFaces( void );
	float*				ShadowVertices( void );

	void				Draw( size_t currentObject ) const;
	void				Draw( size_t currentObject, bool drawTransp ) const;

	// reload textures of materials after res-reset
	virtual bool		Reload( TString& errStr );

	const TCompoundObject&	Object( void ) const;
	TCompoundObject&		Object( void );

	// return array of TObjects
	TBinObject**		Objects( void ) const;

	const size_t&		NumObjects( void ) const;

	void				PauseAnimations( bool p );

	float				SizeX( size_t currentObject ) const;
	float				SizeY( size_t currentObject ) const;
	float				SizeZ( size_t currentObject ) const;

	bool Create( const TString& name, size_t cntr,
				 const TString& texturePath, 
				 const TString* names );

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

	// calculate the objects boundaries
	void	CalculateBoundaries( size_t currentObject );

	bool	ValidObject( void ) const;

	// collision detection on object
	bool Collision( size_t currentObject,
					float l1x, float l1y, float l1z,
					float l2x, float l2y, float l2z ) const;
	bool Collision( size_t currentObject, const float* matrix,
					float l1x, float l1y, float l1z,
					float l2x, float l2y, float l2z ) const;
	bool Collision( size_t currentObject, TMatrix& matrix,
					float l1x, float l1y, float l1z,
					float l2x, float l2y, float l2z );
	bool Collision( size_t currentObject,
					const float* matrix,
					float l1x, float l1z,
					float l2x, float l2z ) const;

	// surface graph mapping using this object
	bool GetLORTriangle( size_t currentObject, 
						 float xp, float yp, float zp, 
						 float size, float& y );

	bool GetLORTriangle( size_t currentObject,  const float* matrix,
						 float xp, float yp, float zp, 
						 float size, float& y );

	void GetBounds( size_t currentObject,
					float& minx, float& miny, float& minz,
					float& maxx, float& maxy, float& maxz );
	void GetCenterSize( size_t currentObject,
						float& cx, float& cy, float& cz,
						float& sx, float& sy, float& sz );

	// proper normalise - takes into account all objects
	// and does a global normalise on all items inside the object
	void Normalise( void );

	// center axis as indicated by flags
	void CenterAxes( bool cx, bool cy, bool cz );

	// detail switching settings
	float		DistanceFromCamera( void ) const;
	void		DistanceFromCamera( float _distanceFromCamera );

	float		SwitchDistance( void ) const;
	void		SwitchDistance( float _switchDistance );

	bool		IsaSwitchDetailObject( void ) const;
	void		IsaSwitchDetailObject( bool _isaSwitchDetailObject );

private:
	void ClearObject( void );
	void ClearShadow( void );

private:
	TString			signature;

	TBinObject**	objects;
	size_t			numObjects;
	bool			objectValid;
	bool			owner;

	// auto detail switching
	bool			isaSwitchDetailObject;
	float			distanceFromCamera;
	float			switchDistance;

	// shadows
	float			s_angle;
	float			s_scale;
	size_t			s_numSurfaces;
	size_t			s_numVertices;
	float*			s_vertices;
	size_t*			s_surfaces;
};

//==========================================================================

#endif

