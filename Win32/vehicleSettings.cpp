#include <precomp_header.h>

#include <win32/vehicleSettings.h>

//==========================================================================

TVehicleSettings::TVehicleSettings( void )
{
	vehicleType = vtNone;
};

TVehicleSettings::TVehicleSettings( const TString& _objName, size_t _iconId, float _barrelXadj, float _barrelYadj,
									float _barrelZadj, size_t _gunRange, size_t _reloadTime, 
									float _hover, size_t _engineSound, size_t _gunSound,
									float _minBarrelAngle, float _maxBarrelAngle,
									float _speed, float _fuelConsumption )
{
	vehicleType = vtTank;

	iconId = _iconId;
	barrelXadj = _barrelXadj;
	barrelYadj = _barrelYadj;
	barrelZadj = _barrelZadj;
	gunRange   = _gunRange;
	reloadTime = _reloadTime;
	hover = _hover;
	engineSound = _engineSound;
	gunSound = _gunSound;

	objName = _objName;

	maxBarrelAngle = _maxBarrelAngle;
	minBarrelAngle = _minBarrelAngle;
	speed = _speed;
	fuelConsumption = _fuelConsumption;
};

TVehicleSettings::TVehicleSettings( const TString& _objName, size_t _iconId, float _restAngle, float _response,
									float _takeoffSpeed, size_t _gunRange,
									float _ceiling, float _hover, size_t _engineSound, 
									size_t _gunSound, float _fuelConsumption )
{
	vehicleType = vtPlane;

	iconId = _iconId;
	restAngle  = _restAngle;
	gunRange   = _gunRange;
	response   = _response;
	takeoffSpeed = _takeoffSpeed;
	ceiling = _ceiling;
	hover = _hover;
	engineSound = _engineSound;
	gunSound = _gunSound;
	fuelConsumption = _fuelConsumption;

	objName = _objName;

	minBarrelAngle = 0;
	maxBarrelAngle = 0;
};

TVehicleSettings::TVehicleSettings( const TString& _objName, 
									size_t _iconId, 
									size_t _gunRange, 
									float _hover, 
									size_t _engineSound, 
									size_t _gunSound )
{
	vehicleType = vtFlak;

	iconId = _iconId;
	gunRange   = _gunRange;
	hover = _hover;
	engineSound = _engineSound;
	gunSound = _gunSound;

	objName = _objName;
};

TVehicleSettings::TVehicleSettings( const TString& _objName, 
									size_t _iconId, 
									size_t _gunRange, 
									float _hover, 
									size_t _gunSound )
{
	vehicleType = vtMG42;

	iconId = _iconId;
	gunRange   = _gunRange;
	hover = _hover;
	gunSound = _gunSound;

	objName = _objName;
};

TVehicleSettings::TVehicleSettings( const TString& _objName, 
									size_t _iconId, 
									size_t _gunRange, 
									float _hover, 
									size_t _engineSound, 
									size_t _gunSound,
									float _fuelConsumption,
									float _speed,
									float _minBarrelAngle, 
									float _maxBarrelAngle )
{
	vehicleType = vtArtillery;

	iconId = _iconId;
	gunRange   = _gunRange;
	hover = _hover;
	engineSound = _engineSound;
	gunSound = _gunSound;
	speed = _speed;
	objName = _objName;
	fuelConsumption = _fuelConsumption;
	maxBarrelAngle = _maxBarrelAngle;
	minBarrelAngle = _minBarrelAngle;
};

TVehicleSettings::TVehicleSettings( const TString& _objName, 
									size_t _engineSound, 
									float _hover )
{
	vehicleType = vtV2;
	iconId = 0;

	hover = _hover;
	engineSound = _engineSound;
	objName = _objName;
};

TVehicleSettings::~TVehicleSettings( void )
{
	vehicleType = vtNone;
};

const TVehicleSettings& TVehicleSettings::operator=( const TVehicleSettings& v )
{
	vehicleType = v.vehicleType;

	iconId = v.iconId;

	barrelXadj = v.barrelXadj;
	barrelYadj = v.barrelYadj;
	barrelZadj = v.barrelZadj;
	gunRange = v.gunRange;
	reloadTime = v.reloadTime;

	engineSound = v.engineSound;
	gunSound = v.gunSound;

	restAngle = v.restAngle;
	response = v.response;
	takeoffSpeed = v.takeoffSpeed;
	ceiling = v.ceiling;

	objName = v.objName;

	hover = v.hover;

	minBarrelAngle = v.minBarrelAngle;
	maxBarrelAngle = v.maxBarrelAngle;

	speed = v.speed;
	fuelConsumption = v.fuelConsumption;

	return *this;
};

TVehicleSettings::TVehicleSettings( const TVehicleSettings& v )
{
	operator=(v);
};

bool TVehicleSettings::IsPlane( void ) const
{
	return (vehicleType==vtPlane);
};

bool TVehicleSettings::IsTank( void ) const
{
	return (vehicleType==vtTank);
};

bool TVehicleSettings::IsFlak( void ) const
{
	return (vehicleType==vtFlak);
};

bool TVehicleSettings::IsArtillery( void ) const
{
	return (vehicleType==vtArtillery);
};

bool TVehicleSettings::IsMG42( void ) const
{
	return (vehicleType==vtMG42);
};

bool TVehicleSettings::IsV2( void ) const
{
	return (vehicleType==vtV2);
};

float TVehicleSettings::BarrelXadj( void ) const
{
	return barrelXadj;
};

float TVehicleSettings::BarrelYadj( void ) const
{
	return barrelYadj;
};

float TVehicleSettings::BarrelZadj( void ) const
{
	return barrelZadj;
};

size_t TVehicleSettings::GunRange( void ) const
{
	return gunRange;
};

size_t TVehicleSettings::ReloadTime( void ) const
{
	return reloadTime;
};

size_t TVehicleSettings::IconId( void ) const
{
	return iconId;
};

float TVehicleSettings::RestAngle( void ) const
{
	return restAngle;
};

float TVehicleSettings::Response( void ) const
{
	return response;
};

float TVehicleSettings::TakeoffSpeed( void ) const
{
	return takeoffSpeed;
};

float TVehicleSettings::Ceiling( void ) const
{
	return ceiling;
};

float TVehicleSettings::Hover( void ) const
{
	return hover;
};

size_t TVehicleSettings::EngineSound( void ) const
{
	return engineSound;
};

size_t TVehicleSettings::GunSound( void ) const
{
	return gunSound;
};

float TVehicleSettings::MinBarrelAngle( void ) const
{
	return minBarrelAngle;
};

float TVehicleSettings::MaxBarrelAngle( void ) const
{
	return maxBarrelAngle;
};

float TVehicleSettings::Speed( void ) const
{
	return speed;
};

float TVehicleSettings::FuelConsumption( void ) const
{
	return fuelConsumption;
};

//==========================================================================

TVehicleList::TVehicleList( void )
{
	count = 0;

	TPersist file(fileRead);
	if ( !file.FileOpen( "data\\vehicles.bin" ) )
	{
		WriteString( "\nError loading data\\vehicles.bin\n" );
		return;
	}
	else
	{
		TString temp;

		size_t vehicleType = vtNone;
		bool eof = false;
		while ( !eof )
		{
			if ( count >= kMaxDifferentVehicles )
			{
				WriteString( "\nError loading data\\vehicles.bin - too many vehicles listed, increase kMaxDifferentVehicles\n" );
				return;
			}

			eof = !file.ReadLine(temp);
			if ( !eof )
			{
				if ( temp.length()>0 )
				{
					if ( temp=="[tanks]" )
					{
						vehicleType = vtTank;
					}
					else if ( temp=="[planes]" )
					{
						vehicleType = vtPlane;
					}
					else if ( temp=="[flak]" )
					{
						vehicleType = vtFlak;
					}
					else if ( temp=="[artillery]" )
					{
						vehicleType = vtArtillery;
					}
					else if ( temp=="[mg42]" )
					{
						vehicleType = vtMG42;
					}
					else if ( temp=="[v2]" )
					{
						vehicleType = vtV2;
					}
					else if ( temp[0]=='/' && temp[1]=='/' ) 
					{
					}
					else // descriptive line
					{
						if ( vehicleType==vtNone )
						{
							WriteString( "\nError: parsing vehicles.bin\n" );
							return;
						}
						else if ( vehicleType==vtTank )
						{
							TString objName;
							size_t iconId;
							float bxadj, byadj, bzadj, hover;
							float speed;
							size_t engineSound, gunSound;
							size_t range, reloadTime;
							float minBarrelAngle, maxBarrelAngle;
							float fuelConsumption;
							objName = temp.GetItem('=',0);
							TString rest = temp.GetItem('=',1);
							if ( rest.length()==0 )
							{
								WriteString( "\nError: parsing vehicles: tank rest=0\n" );
								return;
							}
							iconId = size_t(atoi( rest.GetItem(',',0).c_str() ));
							bxadj = float(atof( rest.GetItem(',',1).c_str() ));
							byadj = float(atof( rest.GetItem(',',2).c_str() ));
							bzadj = float(atof( rest.GetItem(',',3).c_str() ));
							range = size_t(atof( rest.GetItem(',',4).c_str() ));
							reloadTime = size_t(atof(rest.GetItem(',',5).c_str() ));
							hover = float(atof( rest.GetItem(',',6).c_str() ));
							engineSound = size_t(atoi( rest.GetItem(',',7).c_str() ));
							gunSound = size_t(atoi( rest.GetItem(',',8).c_str() ));
							minBarrelAngle = float(atof( rest.GetItem(',',9).c_str() ));
							maxBarrelAngle = float(atof( rest.GetItem(',',10).c_str() ));
							speed = float(atof( rest.GetItem(',',11).c_str() ));
							fuelConsumption = float(atof( rest.GetItem(',',12).c_str() ));
							list[count++] = TVehicleSettings( objName, iconId,bxadj, byadj, bzadj, 
												range, reloadTime, hover, engineSound, gunSound,
												minBarrelAngle, maxBarrelAngle, speed,
												fuelConsumption);
						}
						else if ( vehicleType==vtPlane )
						{
							TString objName;
							size_t iconId;
							float restAngle,response,takeoffSpeed,ceiling, hover;
							size_t engineSound, gunSound;
							size_t gunRange;
							float fuelConsumption;
							objName = temp.GetItem('=',0);
							TString rest = temp.GetItem('=',1);
							if ( rest.length()==0 )
							{
								WriteString( "\nError: parsing vehicles: plane rest=0\n" );
								return;
							}
							iconId = size_t(atoi( rest.GetItem(',',0).c_str() ));
							restAngle = float(atof( rest.GetItem(',',1).c_str() ));
							response = float(atof( rest.GetItem(',',2).c_str() ));
							takeoffSpeed = float(atof( rest.GetItem(',',3).c_str() ));
							gunRange = size_t(atof( rest.GetItem(',',4).c_str() ));
							ceiling = float(atof( rest.GetItem(',',5).c_str() ));
							hover = float(atof( rest.GetItem(',',6).c_str() ));
							engineSound = size_t(atoi( rest.GetItem(',',7).c_str() ));
							gunSound = size_t(atoi( rest.GetItem(',',8).c_str() ));
							fuelConsumption = float(atof( rest.GetItem(',',9).c_str() ));
							list[count++] = TVehicleSettings( objName, iconId, restAngle, response, 
															  takeoffSpeed, gunRange, ceiling, 
															  hover, engineSound, gunSound,
															  fuelConsumption);
						}
						else if ( vehicleType==vtFlak )
						{
							TString objName;
							size_t iconId;
							size_t gunRange;
							float  hover;
							size_t engineSound;
							size_t gunSound;

							objName = temp.GetItem('=',0);
							TString rest = temp.GetItem('=',1);

							iconId = size_t(atoi( rest.GetItem(',',0).c_str() ));
							gunRange = size_t(atof( rest.GetItem(',',1).c_str() ));
							hover = float(atof( rest.GetItem(',',2).c_str() ));
							engineSound = size_t(atoi( rest.GetItem(',',3).c_str() ));
							gunSound = size_t(atoi( rest.GetItem(',',4).c_str() ));

							list[count++] = TVehicleSettings( objName, iconId, gunRange, 
															  hover, engineSound, 
															  gunSound );
						}
						else if ( vehicleType==vtArtillery )
						{
							TString objName;
							size_t iconId;
							size_t gunRange;
							float  hover;
							size_t engineSound;
							size_t gunSound;
							float  fuelConsumption;
							float  speed;
							float  minBarrelAngle, maxBarrelAngle;

							objName = temp.GetItem('=',0);
							TString rest = temp.GetItem('=',1);

							iconId = size_t(atoi( rest.GetItem(',',0).c_str() ));
							gunRange = size_t(atof( rest.GetItem(',',1).c_str() ));
							hover = float(atof( rest.GetItem(',',2).c_str() ));
							engineSound = size_t(atoi( rest.GetItem(',',3).c_str() ));
							gunSound = size_t(atoi( rest.GetItem(',',4).c_str() ));
							fuelConsumption = float(atof(rest.GetItem(',',5).c_str() ));
							speed = float(atof(rest.GetItem(',',6).c_str() ));
							minBarrelAngle = float(atof( rest.GetItem(',',7).c_str() ));
							maxBarrelAngle = float(atof( rest.GetItem(',',8).c_str() ));

							list[count++] = TVehicleSettings( objName, iconId, gunRange, 
															  hover, engineSound, 
															  gunSound, fuelConsumption,
															  speed, minBarrelAngle, 
															  maxBarrelAngle );
						}
						else if ( vehicleType==vtMG42 )
						{
							TString objName;
							size_t iconId;
							size_t gunRange;
							float  hover;
							size_t gunSound;

							objName = temp.GetItem('=',0);
							TString rest = temp.GetItem('=',1);

							iconId = size_t(atoi( rest.GetItem(',',0).c_str() ));
							gunRange = size_t(atof( rest.GetItem(',',1).c_str() ));
							hover = float(atof( rest.GetItem(',',2).c_str() ));
							gunSound = size_t(atoi( rest.GetItem(',',3).c_str() ));

							list[count++] = TVehicleSettings( objName, iconId, gunRange, 
															  hover, gunSound );
						}
						else if ( vehicleType==vtV2 )
						{
							TString objName;
							size_t  engineSound;
							float   hover;

							objName = temp.GetItem('=',0);
							TString rest = temp.GetItem('=',1);
							engineSound = size_t(atof( rest.GetItem(',',0).c_str() ));
							hover = float(atof( rest.GetItem(',',1).c_str() ));

							list[count++] = TVehicleSettings( objName, engineSound, hover );
						}
					}
				}
			}
		}
		file.FileClose();
	}
};

TVehicleList::~TVehicleList( void )
{
	count = 0;
};


TVehicleSettings* TVehicleList::Find( const TString& objName )
{
	for ( size_t i=0; i<count; i++ )
	{
		if ( list[i].objName==objName )
			return &list[i];
	}
	return NULL;
};

//==========================================================================
