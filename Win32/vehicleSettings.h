#ifndef __TANK_VEHICLESETTINGS_H__
#define __TANK_VEHICLESETTINGS_H__

//==========================================================================

#include <win32/win32.h>

//==========================================================================

class _EXPORT TVehicleSettings
{
public:
	TVehicleSettings( void );
	TVehicleSettings( const TString& objName, size_t iconId, float barrelXadj, float barrelYadj,
					  float barrelZadj, size_t gunRange, size_t reloadTime, float hover,
					  size_t _engineSound, size_t _gunSound,
					  float _minBarrelAngle, float _maxBarrelAngle, float _speed,
					  float _fuelConsumption );
	TVehicleSettings( const TString& objName, size_t iconId, float restAngle, float response,
					  float takeoffSpeed, size_t gunRange,
					  float ceiling, float hover,
					  size_t _engineSound, size_t _gunSound, float _fuelConsumption );
	TVehicleSettings( const TString& _objName, size_t iconId, 
					  size_t _gunRange, 
					  float _hover, 
					  size_t _engineSound, 
					  size_t _gunSound );
	TVehicleSettings( const TString& _objName, size_t iconId,
					  size_t _gunRange, 
					  float _hover, 
					  size_t _engineSound, 
					  size_t _gunSound,
					  float _fuelConsumption,
					  float _speed,
					  float _minBarrelAngle, float _maxBarrelAngle );
	TVehicleSettings( const TString& _objName, size_t iconId,
					  size_t _gunRange, 
					  float _hover, 
					  size_t _gunSound );
	TVehicleSettings( const TString& _objName, 
					  size_t _engineSound, 
					  float _hover );

	~TVehicleSettings( void );

	const TVehicleSettings& operator=( const TVehicleSettings& );
	TVehicleSettings( const TVehicleSettings& );

	bool	IsPlane( void ) const;
	bool	IsTank( void ) const;
	bool	IsFlak( void ) const;
	bool	IsArtillery( void ) const;
	bool	IsMG42( void ) const;
	bool	IsV2( void ) const;

	size_t	IconId( void ) const;

	float	BarrelXadj( void ) const;
	float	BarrelYadj( void ) const;
	float	BarrelZadj( void ) const;

	size_t	GunRange( void ) const;
	size_t	ReloadTime( void ) const;

	float	RestAngle( void ) const;
	float	Response( void ) const;
	float	TakeoffSpeed( void ) const;
	float	Ceiling( void ) const;
	float	Hover( void ) const;
	float	FuelConsumption( void ) const;

	size_t	EngineSound( void ) const;
	size_t	GunSound( void ) const;

	float	MinBarrelAngle( void ) const;
	float	MaxBarrelAngle( void ) const;

	float	Speed( void ) const;

private:
	friend class TVehicleList;

	TString	objName;

	float barrelXadj;
	float barrelYadj;
	float barrelZadj;

	size_t iconId;
	size_t gunRange;
	size_t reloadTime;
	size_t vehicleType;

	float restAngle;
	float response;
	float takeoffSpeed;
	float ceiling;

	float hover;
	float fuelConsumption;

	float minBarrelAngle;
	float maxBarrelAngle;

	float speed;

	size_t engineSound;
	size_t gunSound;
};

//==========================================================================

class _EXPORT TVehicleList
{
	const TVehicleList& operator=( const TVehicleList& );
	TVehicleList( const TVehicleList& );
public:
	TVehicleList( void );
	~TVehicleList( void );

	TVehicleSettings*	Find( const TString& objName );

private:
	TVehicleSettings	list[kMaxDifferentVehicles];
	size_t				count;
};

//==========================================================================

#endif
