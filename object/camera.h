#ifndef __DXPARSER_OBJECT_CAMERA_H_
#define __DXPARSER_OBJECT_CAMERA_H_

//==========================================================================

#include <object/landscape.h>
#include <object/tank.h>
#include <object/plane.h>
#include <object/tank.h>
#include <object/flak.h>
#include <object/mg42.h>
#include <object/character.h>
#include <object/recordSystem.h>

#include <win32/params.h>

//==========================================================================

class TExplosion;

//==========================================================================

class _EXPORT TCamera
{
public:
	TCamera(void);
	~TCamera(void);

	void	SetVehicleCamera( TLandscape& landObj,
							  TTank& vehicle, 
							  TParams& params,
							  bool ctrlDown, 
							  float deltaXAngle,
							  float deltaYAngle,
							  bool showMap);

	void	SetVehicleCamera( TLandscape& landObj, 
							  TPlane& plane, 
							  TParams& params,
							  bool ctrlDown,
							  float dxa, float dya,
							  bool showMap );

	void	SetVehicleCamera( TLandscape& landObj, 
							  TFlak& vehicle, 
							  TParams& params,
							  bool ctrlDown,
							  float dxa, float dya,
							  bool showMap );
	void	SetVehicleCamera( TLandscape& landObj, 
							  TArtillery& vehicle, 
							  TParams& params,
							  bool ctrlDown,
							  float dxa, float dya,
							  bool showMap );
	void	SetVehicleCamera( TLandscape& landObj, 
							  TMG42& vehicle, 
							  TParams& params,
							  bool ctrlDown,
							  float dxa, float dya,
							  bool showMap );

	void	SetPersonCamera( TLandscape& landObj, const TCharacter& ch,
							 TParams& params, bool ctrlDown, 
							 float deltaXAngle, float deltaYAngle, 
							 bool showMap );

	void	SetCamera( float ox, float oy, float oz,
					   float yangle,
					   float dxa, float dya );

	void	SetCamera( float ox, float oy, float oz,
					   float oyangle,
					   float dxa, float dya, float scale );

//	void	Draw( float x, float z, TLandscape& landObj );

	float	CameraX( void ) const;
	float	CameraY( void ) const;
	float	CameraZ( void ) const;

	float	ListenX( void ) const;
	float	ListenY( void ) const;
	float	ListenZ( void ) const;

	float	XAngle( void ) const;
	float	YAngle( void ) const;
	float	ZAngle( void ) const;

	// recording system
	void Record( size_t selectedCamera );
	void Playback( size_t& selectedCamera );

	void SetupRecording( const TString& filename );
	bool StopRecording( TString& errSr );

	bool SetupPlayback( const TString& filename, TString& errSr );
	void StopPlayback( void );

	void	Type( size_t t );
	size_t	Type( void ) const;

	float	FixedCameraRotation( void ) const;

	float	CameraAngle( void ) const;

private:
	void CheckCameraHeight( TParams& params, float lx, float& ly, float lz );

private:
	float	radius;
	float	yAngle, xAngle, zAngle;
	float	minRadius, maxRadius;
	float	lx, ly, lz; // listener pos
	float	sx, sy, sz; // sound pos
	float	py; // previous y

	float	vx,vz;

	size_t	type; // camera type
	float	fixedCameraRotation; // camera fixed rotation

	TString			recordingFname;
	TCameraRecord*	recording;
};

//==========================================================================

#endif
