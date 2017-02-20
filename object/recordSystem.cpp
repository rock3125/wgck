#include <precomp_header.h>

#include <win32/events.h>
#include <win32/win32.h>

#include <object/recordSystem.h>

#include <object/tank.h>

//==========================================================================

size_t DataSize( DataType type )
{
	switch ( type )
	{
		case dtsize_t:
			{
				return sizeof(size_t);
			}

		case dtbool:
			{
				return sizeof(bool);
			}

		case dtfloat:
			{
				return sizeof(float);
			}

		case dtint:
			{
				return sizeof(int);
			}

		case dtbyte:
			{
				return sizeof(byte);
			}
		default:
			{
				return 1;
			};
	}
};

//==========================================================================

TData::TData( DataType _type )
	: RLE(NULL),
	  Data(NULL),
	  type( _type )
{
	count = 0;
	index = 0;
	blockSize = 1024;

	RLE = new byte[blockSize];
	Data = new byte[DataSize(type)*blockSize];

	for ( size_t i=0; i<blockSize; i++ )
	{
		RLE[i] = 0;
		switch ( type )
		{
		case dtsize_t:
			{
				((size_t*)Data)[i] = 0;
				break;
			}

		case dtbool:
			{
				((bool*)Data)[i] = false;
				break;
			}

		case dtfloat:
			{
				((float*)Data)[i] = 0;
				break;
			}

		case dtint:
			{
				((int*)Data)[i] = 0;
				break;
			}

		case dtbyte:
			{
				((byte*)Data)[i] = 0;
				break;
			}
		}
	}
};


TData::TData( const TData& d )
	: RLE(NULL),
	  Data(NULL)
{
	operator=(d);
};


const TData& TData::operator=( const TData& d )
{
	type = d.type;

	count = d.count;
	index = d.index;
	blockSize = d.blockSize;

	if ( RLE!=NULL )
		delete RLE;
	if ( Data!=NULL )
		delete Data;

	size_t ts = DataSize(type);
	byte* RLE = new byte[blockSize];
	byte* Data = new byte[blockSize*ts];

	for ( size_t i=0; i<blockSize; i++ )
	{
		RLE[i] = d.RLE[i];
		switch ( type )
		{
		case dtsize_t:
			{
				((size_t*)Data)[i] = ((size_t*)d.Data)[i];
				break;
			}
		case dtbool:
			{
				((bool*)Data)[i] = ((bool*)d.Data)[i];
				break;
			}
		case dtfloat:
			{
				((float*)Data)[i] = ((float*)d.Data)[i];
				break;
			}
		case dtint:
			{
				((int*)Data)[i] = ((int*)d.Data)[i];
				break;
			}
		case dtbyte:
			{
				((byte*)Data)[i] = ((byte*)d.Data)[i];
				break;
			}
		}
	}
	return *this;
};

	
TData::~TData( void )
{
	if ( Data!=NULL )
		delete Data;
	Data = NULL;
	
	if ( RLE!=NULL )
		delete RLE;
	RLE = NULL;
};


void TData::Add( void* _d )
{
	switch ( type )
	{
		case dtsize_t:
			{
				size_t ts = DataSize(type);
				size_t p = ((size_t*)Data)[count];
				size_t d = *(size_t*)_d;
				if ( p!=d )
				{
					if ( RLE[count]==0 )
						RLE[count] = 1;
					count++;
					((size_t*)Data)[count] = d;
					RLE[count] = 1;
				}
				else
				{
					RLE[count]++;
					if ( RLE[count]==255 )
					{
						count++;
						((size_t*)Data)[count] = d;
						RLE[count] = 1;
					}
				}

				if ( count>=(blockSize-1) )
				{
					size_t old = blockSize;
					blockSize = blockSize * 2;
				
					byte* _RLE = new byte[blockSize];
					byte* _Data = new byte[blockSize*ts];
					for ( size_t i=0; i<blockSize; i++ )
					{
						if ( i<old )
						{
							_RLE[i] = RLE[i];
							((size_t*)_Data)[i] = ((size_t*)Data)[i];
						}
						else
						{
							_RLE[i] = 0;
							((size_t*)_Data)[i] = 0;
						}
					}
					delete RLE;
					delete Data;
					RLE = _RLE;
					Data = _Data;
				}

				break;
			}

		case dtbool:
			{
				size_t ts = DataSize(type);
				bool p = ((bool*)Data)[count];
				bool d = *(bool*)_d;
				if ( p!=d )
				{
					if ( RLE[count]==0 )
						RLE[count] = 1;
					count++;
					((bool*)Data)[count] = d;
					RLE[count] = 1;
				}
				else
				{
					RLE[count]++;
					if ( RLE[count]==255 )
					{
						count++;
						((bool*)Data)[count] = d;
						RLE[count] = 1;
					}
				}

				if ( count>=(blockSize-1) )
				{
					size_t old = blockSize;
					blockSize = blockSize * 2;
				
					byte* _RLE = new byte[blockSize];
					byte* _Data = new byte[blockSize*ts];
					for ( size_t i=0; i<blockSize; i++ )
					{
						if ( i<old )
						{
							_RLE[i] = RLE[i];
							((bool*)_Data)[i] = ((bool*)Data)[i];
						}
						else
						{
							_RLE[i] = 0;
							((bool*)_Data)[i] = 0;
						}
					}
					delete RLE;
					delete Data;
					RLE = _RLE;
					Data = _Data;
				}

				break;
			}

		case dtfloat:
			{
				size_t ts = DataSize(type);
				float p = ((float*)Data)[count];
				float d = *(float*)_d;
				if ( p!=d )
				{
					if ( RLE[count]==0 )
						RLE[count] = 1;
					count++;
					((float*)Data)[count] = d;
					RLE[count] = 1;
				}
				else
				{
					RLE[count]++;
					if ( RLE[count]==255 )
					{
						count++;
						((float*)Data)[count] = d;
						RLE[count] = 1;
					}
				}

				if ( count>=(blockSize-1) )
				{
					size_t old = blockSize;
					blockSize = blockSize * 2;
				
					byte* _RLE = new byte[blockSize];
					byte* _Data = new byte[blockSize*ts];
					for ( size_t i=0; i<blockSize; i++ )
					{
						if ( i<old )
						{
							_RLE[i] = RLE[i];
							((float*)_Data)[i] = ((float*)Data)[i];
						}
						else
						{
							_RLE[i] = 0;
							((float*)_Data)[i] = 0;
						}
					}
					delete RLE;
					delete Data;
					RLE = _RLE;
					Data = _Data;
				}

				break;
			}

		case dtint:
			{
				size_t ts = DataSize(type);
				int p = ((int*)Data)[count];
				int d = *(int*)_d;
				if ( p!=d )
				{
					if ( RLE[count]==0 )
						RLE[count] = 1;
					count++;
					((int*)Data)[count] = d;
					RLE[count] = 1;
				}
				else
				{
					RLE[count]++;
					if ( RLE[count]==255 )
					{
						count++;
						((int*)Data)[count] = d;
						RLE[count] = 1;
					}
				}

				if ( count>=(blockSize-1) )
				{
					size_t old = blockSize;
					blockSize = blockSize * 2;
				
					byte* _RLE = new byte[blockSize];
					byte* _Data = new byte[blockSize*ts];
					for ( size_t i=0; i<blockSize; i++ )
					{
						if ( i<old )
						{
							_RLE[i] = RLE[i];
							((int*)_Data)[i] = ((int*)Data)[i];
						}
						else
						{
							_RLE[i] = 0;
							((int*)_Data)[i] = 0;
						}
					}
					delete RLE;
					delete Data;
					RLE = _RLE;
					Data = _Data;
				}

				break;
			}

		case dtbyte:
			{
				size_t ts = DataSize(type);
				byte p = ((byte*)Data)[count];
				byte d = *(byte*)_d;
				if ( p!=d )
				{
					if ( RLE[count]==0 )
						RLE[count] = 1;
					count++;
					((byte*)Data)[count] = d;
					RLE[count] = 1;
				}
				else
				{
					RLE[count]++;
					if ( RLE[count]==255 )
					{
						count++;
						((byte*)Data)[count] = d;
						RLE[count] = 1;
					}
				}

				if ( count>=(blockSize-1) )
				{
					size_t old = blockSize;
					blockSize = blockSize * 2;
				
					byte* _RLE = new byte[blockSize];
					byte* _Data = new byte[blockSize*ts];
					for ( size_t i=0; i<blockSize; i++ )
					{
						if ( i<old )
						{
							_RLE[i] = RLE[i];
							((byte*)_Data)[i] = ((byte*)Data)[i];
						}
						else
						{
							_RLE[i] = 0;
							((byte*)_Data)[i] = 0;
						}
					}
					delete RLE;
					delete Data;
					RLE = _RLE;
					Data = _Data;
				}

				break;
			}
	}
};


void TData::Get( void* d )
{
	switch ( type )
	{
		case dtsize_t:
			{
				size_t* _d =(size_t*)d;
				if ( index<count )
				{
					*_d = ((size_t*)Data)[index];
				
					RLE[index]--;
					if ( RLE[index]==0 )
						index++;
				}
				break;
			}

		case dtbool:
			{
				bool* _d =(bool*)d;
				if ( index<count )
				{
					*_d = ((bool*)Data)[index];
				
					RLE[index]--;
					if ( RLE[index]==0 )
						index++;
				}
				break;
			}

		case dtfloat:
			{
				float* _d =(float*)d;
				if ( index<count )
				{
					*_d = ((float*)Data)[index];
				
					RLE[index]--;
					if ( RLE[index]==0 )
						index++;
				}
				break;
			}

		case dtint:
			{
				int* _d =(int*)d;
				if ( index<count )
				{
					*_d = ((int*)Data)[index];
				
					RLE[index]--;
					if ( RLE[index]==0 )
						index++;
				}
				break;
			}

		case dtbyte:
			{
				byte* _d =(byte*)d;
				if ( index<count )
				{
					*_d = ((byte*)Data)[index];
				
					RLE[index]--;
					if ( RLE[index]==0 )
						index++;
				}
				break;
			}
	}
};


void TData::SaveBinary( TPersist& file )
{
	file.FileWrite( &count, sizeof(size_t) );
	file.FileWrite( &type, sizeof(DataType) );
	file.FileWrite( RLE, sizeof(byte)*count );
	file.FileWrite( Data, DataSize(type)*count );
};


void TData::LoadBinary( TPersist& file )
{
	index = 0;

	if ( Data!=NULL )
		delete Data;
	if ( RLE!=NULL )
		delete RLE;

	file.FileRead( &count, sizeof(size_t) );
	file.FileRead( &type, sizeof(DataType) );

	if ( count>0 )
	{
		RLE = new byte[count];
		file.FileRead( RLE, sizeof(byte)*count);
		Data = new byte[count*DataSize(type)];
		file.FileRead( Data, DataSize(type)*count );
	}
	else
	{
		RLE = NULL;
		Data = NULL;
	}
};

//==========================================================================

TTankRecord::TTankRecord( void )
	: keys(dtsize_t),
	  ctrlDown(dtbool),
	  deltaXAngle(dtfloat),
	  deltaYAngle(dtfloat),

	  x(dtfloat),
	  z(dtfloat),
	  yangle(dtfloat),
	  barrelAngle(dtfloat),
	  turretAngle(dtfloat)
{
	signature = "PDVTNKRCRD01";
};


TTankRecord::~TTankRecord( void )
{
};


TTankRecord::TTankRecord( const TTankRecord& t )
	: keys(dtsize_t),
	  ctrlDown(dtbool),
	  deltaXAngle(dtfloat),
	  deltaYAngle(dtfloat),

	  x(dtfloat),
	  z(dtfloat),
	  yangle(dtfloat),
	  barrelAngle(dtfloat),
	  turretAngle(dtfloat)
{
	operator=(t);
};


const TTankRecord& TTankRecord::operator=( const TTankRecord& t )
{
	keys = t.keys;
	ctrlDown = t.ctrlDown;
	deltaXAngle = t.deltaXAngle;
	deltaYAngle = t.deltaYAngle;

	x = t.x;
	z = t.z;
	yangle = t.yangle;
	barrelAngle = t.barrelAngle;
	turretAngle = t.turretAngle;

	return *this;
};


void TTankRecord::Add( TTank* tank, size_t _keys, bool _ctrlDown, float _dxa, float _dya )
{
	keys.Add( (void*)&_keys );
	ctrlDown.Add( (void*)&_ctrlDown );
	deltaXAngle.Add( (void*)&_dxa );
	deltaYAngle.Add( (void*)&_dya );

	float tx = tank->X();
	float tz = tank->Z();
	float tya = tank->Yangle();
	float tba = tank->BarrelAngle();
	float tta = tank->TurretAngle();
	x.Add( &tx );
	z.Add( &tz );
	yangle.Add( &tya );
	barrelAngle.Add( &tba );
	turretAngle.Add( &tta );
};


bool TTankRecord::PlaybackFinished( void ) const
{
	return (keys.index >= keys.count);
};


bool TTankRecord::Get( TTank* tank, size_t& _keys, bool& _ctrlDown, float& _dxa, float& _dya )
{
	keys.Get( (void*)&_keys );
	ctrlDown.Get( (void*)&_ctrlDown );
	deltaXAngle.Get( (void*)&_dxa );
	deltaYAngle.Get( (void*)&_dya );

	float tx = tank->X();
	float tz = tank->Z();
	float tya = tank->Yangle();
	float tba = tank->BarrelAngle();
	float tta = tank->TurretAngle();
	x.Get( &tx );
	z.Get( &tz );
	yangle.Get( &tya );
	barrelAngle.Get( &tba );
	turretAngle.Get( &tta );

	tank->X( tx );
	tank->Z( tz );
	tank->Yangle( tya );
	tank->BarrelAngle( tba );
	tank->TurretAngle( tta );

	// finished playback?
	if ( keys.index >= keys.count )
	{
		_keys = 0;
		return false;
	}
	return true;
};


bool TTankRecord::SaveBinary( TString fname, TString& errStr )
{
	TPersist file(fileWrite);

	if ( !file.FileOpen( fname ) )
	{
		errStr = file.ErrorString();
		return false;
	}

	file.FileWrite( signature.c_str(), signature.length()+1 );

	keys.SaveBinary( file );
	ctrlDown.SaveBinary( file );
	deltaXAngle.SaveBinary( file );
	deltaYAngle.SaveBinary( file );

	x.SaveBinary( file );
	z.SaveBinary( file );
	yangle.SaveBinary( file );
	barrelAngle.SaveBinary( file );
	turretAngle.SaveBinary( file );

	file.FileClose();

	return true;
};


bool TTankRecord::LoadBinary( TString fname, TString& errStr )
{
	TPersist file(fileRead);

	if ( file.FileOpen( fname ) )
	{
		// check signature
		char ch;
		size_t index = 0;
		TString fileSignature;
		do
		{
			file.FileRead( &ch, 1 );
			if ( ch!=0 )
				fileSignature = fileSignature + TString(ch);
		}
		while ( ch!=0 && index<255 );

		if ( fileSignature!=signature )
		{
			file.FileClose();
			errStr = "Error: " + fname;
			errStr = errStr + " incorrect version or file-type";
			return false;
		}

		keys.LoadBinary( file );
		ctrlDown.LoadBinary( file );
		deltaXAngle.LoadBinary( file );
		deltaYAngle.LoadBinary( file );

		x.LoadBinary( file );
		z.LoadBinary( file );
		yangle.LoadBinary( file );
		barrelAngle.LoadBinary( file );
		turretAngle.LoadBinary( file );

		file.FileClose();
		return true;
	}
	else
	{
		errStr = file.ErrorString();
		return false;
	}
};

//==========================================================================

TCameraRecord::TCameraRecord( void )
	: x(dtfloat),
	  y(dtfloat),
	  z(dtfloat),
	  xangle(dtfloat),
	  yangle(dtfloat),
	  selectedCamera(dtsize_t)
{
	signature = "PDVCAMRCRD02";
};


TCameraRecord::~TCameraRecord( void )
{
};


TCameraRecord::TCameraRecord( const TCameraRecord& t )
	: x(dtfloat),
	  y(dtfloat),
	  z(dtfloat),
	  xangle(dtfloat),
	  yangle(dtfloat),
	  selectedCamera(dtsize_t)
{
	operator=(t);
};


const TCameraRecord& TCameraRecord::operator=( const TCameraRecord& t )
{
	x = t.x;
	y = t.y;
	z = t.z;
	xangle = t.xangle;
	yangle = t.yangle;
	selectedCamera = t.selectedCamera;

	return *this;
};


void TCameraRecord::Add( float _x, float _y, float _z, 
						 float _xangle, float _yangle, 
						 size_t _sel )
{
	x.Add( (void*)&_x );
	y.Add( (void*)&_y );
	z.Add( (void*)&_z );
	xangle.Add( (void*)&_xangle );
	yangle.Add( (void*)&_yangle );
	selectedCamera.Add( (void*)&_sel );
};


bool TCameraRecord::PlaybackFinished( void ) const
{
	return (x.index >= x.count);
};


bool TCameraRecord::Get( float& _x, float& _y, float& _z, 
						 float& _xangle, float& _yangle, 
						 size_t& _sel )
{
	x.Get( &_x );
	y.Get( &_y );
	z.Get( &_z );
	xangle.Get( &_xangle );
	yangle.Get( &_yangle );
	selectedCamera.Get( &_sel );

	// finished playback?
	if ( x.index >= x.count )
	{
		return false;
	}
	return true;
};


bool TCameraRecord::SaveBinary( TString fname, TString& errStr )
{
	TPersist file(fileWrite);

	if ( !file.FileOpen( fname ) )
	{
		errStr = file.ErrorString();
		return false;
	}

	file.FileWrite( signature.c_str(), signature.length()+1 );

	x.SaveBinary( file );
	y.SaveBinary( file );
	z.SaveBinary( file );
	xangle.SaveBinary( file );
	yangle.SaveBinary( file );
	selectedCamera.SaveBinary( file );

	file.FileClose();

	return true;
};


bool TCameraRecord::LoadBinary( TString fname, TString& errStr )
{
	TPersist file(fileRead);

	if ( file.FileOpen( fname ) )
	{
		// check signature
		char ch;
		size_t index = 0;
		TString fileSignature;
		do
		{
			file.FileRead( &ch, 1 );
			if ( ch!=0 )
				fileSignature = fileSignature + TString(ch);
		}
		while ( ch!=0 && index<255 );

		if ( fileSignature!=signature )
		{
			file.FileClose();
			errStr = "Error: " + fname;
			errStr = errStr + " incorrect version or file-type";
			return false;
		}

		x.LoadBinary( file );
		y.LoadBinary( file );
		z.LoadBinary( file );
		xangle.LoadBinary( file );
		yangle.LoadBinary( file );
		selectedCamera.LoadBinary( file );
		file.FileClose();
		return true;
	}
	else
	{
		errStr = file.ErrorString();
		return false;
	}
};

//==========================================================================
/*
TCarRecord::TCarRecord( void )
	: keys(dtsize_t),
	  ctrlDown(dtbool),
	  deltaXAngle(dtfloat),
	  deltaYAngle(dtfloat),

	  x(dtfloat),
	  z(dtfloat),
	  yangle(dtfloat),
	  speed(dtfloat)
{
	signature = "PDVTNKRCRD01";
};


TCarRecord::~TCarRecord( void )
{
};


TCarRecord::TCarRecord( const TCarRecord& t )
	: keys(dtsize_t),
	  ctrlDown(dtbool),
	  deltaXAngle(dtfloat),
	  deltaYAngle(dtfloat),

	  x(dtfloat),
	  z(dtfloat),
	  yangle(dtfloat),
	  speed(dtfloat)
{
	operator=(t);
};


const TCarRecord& TCarRecord::operator=( const TCarRecord& t )
{
	keys = t.keys;
	ctrlDown = t.ctrlDown;
	deltaXAngle = t.deltaXAngle;
	deltaYAngle = t.deltaYAngle;

	x = t.x;
	z = t.z;
	yangle = t.yangle;
	speed = t.speed;

	return *this;
};


void TCarRecord::Add( TRaceCar* car, size_t _keys, bool _ctrlDown, float _dxa, float _dya )
{
	keys.Add( (void*)&_keys );
	ctrlDown.Add( (void*)&_ctrlDown );
	deltaXAngle.Add( (void*)&_dxa );
	deltaYAngle.Add( (void*)&_dya );

	float tx = car->X();
	float tz = car->Z();
	float tya = car->Yangle();
	float spd = car->Speed();
	x.Add( &tx );
	z.Add( &tz );
	yangle.Add( &tya );
	speed.Add( &spd );
};


bool TCarRecord::PlaybackFinished( void ) const
{
	return (keys.index >= keys.count);
};


bool TCarRecord::Get( TRaceCar* car, size_t& _keys, bool& _ctrlDown, float& _dxa, float& _dya )
{
	keys.Get( (void*)&_keys );
	ctrlDown.Get( (void*)&_ctrlDown );
	deltaXAngle.Get( (void*)&_dxa );
	deltaYAngle.Get( (void*)&_dya );

	float tx = car->X();
	float tz = car->Z();
	float tya = car->Yangle();
	float _speed = car->Speed();
	x.Get( &tx );
	z.Get( &tz );
	yangle.Get( &tya );
	speed.Get( &_speed );

	car->X( tx );
	car->Z( tz );
	car->Yangle( tya );
	car->Speed( _speed );

	// finished playback?
	if ( keys.index >= keys.count )
	{
		_keys = 0;
		return false;
	}
	return true;
};


bool TCarRecord::SaveBinary( TString fname, TString& errStr )
{
	TPersist file(fileWrite);

	if ( !file.FileOpen( fname ) )
	{
		errStr = file.ErrorString();
		return false;
	}

	file.FileWrite( signature.c_str(), signature.length()+1 );

	keys.SaveBinary( file );
	ctrlDown.SaveBinary( file );
	deltaXAngle.SaveBinary( file );
	deltaYAngle.SaveBinary( file );

	x.SaveBinary( file );
	z.SaveBinary( file );
	yangle.SaveBinary( file );
	speed.SaveBinary( file );

	file.FileClose();

	return true;
};


bool TCarRecord::LoadBinary( TString fname, TString& errStr )
{
	TPersist file(fileRead);

	if ( file.FileOpen( fname ) )
	{
		// check signature
		char ch;
		size_t index = 0;
		TString fileSignature;
		do
		{
			file.FileRead( &ch, 1 );
			if ( ch!=0 )
				fileSignature = fileSignature + TString(ch);
		}
		while ( ch!=0 && index<255 );

		if ( fileSignature!=signature )
		{
			file.FileClose();
			errStr = "Error: " + fname;
			errStr = errStr + " incorrect version or file-type";
			return false;
		}

		keys.LoadBinary( file );
		ctrlDown.LoadBinary( file );
		deltaXAngle.LoadBinary( file );
		deltaYAngle.LoadBinary( file );

		x.LoadBinary( file );
		z.LoadBinary( file );
		yangle.LoadBinary( file );
		speed.LoadBinary( file );

		file.FileClose();
		return true;
	}
	else
	{
		errStr = file.ErrorString();
		return false;
	}
};
*/
//==========================================================================
