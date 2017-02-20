#ifndef __DXPARSER_OBJECT_H_
#define __DXPARSER_OBJECT_H_

//==========================================================================

#include <object/vector.h>
#include <object/binobject.h>

//==========================================================================

class TLandscapeGraph;
class TVehicle;

//==========================================================================

class TObject : public TVector, public TBinObject
{
public:
	TObject( void );
	~TObject( void );

	TObject( const TObject& );
	const TObject& operator=( const TObject& );

    bool LoadBinary( const TString& fname, TString& errStr, 
					 const TString& pathname,
					 const TString& texturePath );
	bool LoadBinary( TPersist& file, TString& errStr, 
					 const TString& pathname,
					 const TString& texturePath );

	virtual void Draw( bool showBoundingBox = false ) const;

	void DrawBoundingBox( void ) const;

	void GetBoudingBox( float& minX, float& minY, float& minZ,
						float& maxX, float& maxY, float& maxZ ) const;

	void SetLocationToObjectLocation( TObject& obj );
	void GetCenter( float& cx, float& cy, float &cz ) const;
	void Resize( void );

	float X( void ) const;
	float Y( void ) const;
	float Z( void ) const;

	void X( float );
	void Y( float );
	void Z( float );

	float MinX( void ) const;
	float MinY( void ) const;
	float MinZ( void ) const;

	float MaxX( void ) const;
	float MaxY( void ) const;
	float MaxZ( void ) const;

	float CenterX( void ) const;
	float CenterY( void ) const;
	float CenterZ( void ) const;

	float SizeX( void ) const;
	float SizeY( void ) const;
	float SizeZ( void ) const;

	void SizeX( float );
	void SizeY( float );
	void SizeZ( float );

	void MinX( float );
	void MinY( float );
	void MinZ( float );

	void MaxX( float );
	void MaxY( float );
	void MaxZ( float );

	void CenterX( float );
	void CenterY( float );
	void CenterZ( float );

	bool Loaded( void ) const;

private:
	friend TVehicle;

	bool loaded;
	float x,y,z;
	float minX, minY, minZ;
	float maxX, maxY, maxZ;
	float centerX, centerY, centerZ;
	float xSize, ySize, zSize;
};

//==========================================================================

#endif

