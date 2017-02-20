#include <precomp_header.h>

#include <win32/events.h>
#include <win32/win32.h>

#include <object/recordSystem.h>

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
	blockSize = 4096;

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
				else
				{
					*_d = 0;
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
				else
				{
					*_d = 0;
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
				else
				{
					*_d = 0;
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
				else
				{
					*_d = 0;
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
				else
				{
					*_d = 0;
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
	  buttons(dtbyte),
	  deltaXAngle(dtfloat),
	  deltaYAngle(dtfloat)
{
};


TTankRecord::~TTankRecord( void )
{
};


void TTankRecord::Add( size_t _keys, bool _ctrlDown, byte _buttons, 
					   float _dxa, float _dya )
{
	keys.Add( (void*)&_keys );
	ctrlDown.Add( (void*)&_ctrlDown );
	buttons.Add( (void*)&_buttons );
	deltaXAngle.Add( (void*)&_dxa );
	deltaYAngle.Add( (void*)&_dya );
};


void TTankRecord::Get( size_t& _keys, bool& _ctrlDown, byte& _buttons, 
					   float& _dxa, float& _dya )
{
	keys.Get( (void*)&_keys );
	ctrlDown.Get( (void*)&_ctrlDown );
	buttons.Get( (void*)&_buttons );
	deltaXAngle.Get( (void*)&_dxa );
	deltaYAngle.Get( (void*)&_dya );
};


bool TTankRecord::SaveBinary( TString fname, TString& errStr )
{
	TPersist file(fileWrite);

	if ( !file.FileOpen( fname ) )
	{
		errStr = file.ErrorString();
		return false;
	}

	keys.SaveBinary( file );
	ctrlDown.SaveBinary( file );
	buttons.SaveBinary( file );
	deltaXAngle.SaveBinary( file );
	deltaYAngle.SaveBinary( file );
	file.FileClose();
	return true;
};


void TTankRecord::LoadBinary( TString fname )
{
	TPersist file(fileRead);

	if ( file.FileOpen( fname ) )
	{
		keys.LoadBinary( file );
		ctrlDown.LoadBinary( file );
		buttons.LoadBinary( file );
		deltaXAngle.LoadBinary( file );
		deltaYAngle.LoadBinary( file );

		file.FileClose();
	};

};


