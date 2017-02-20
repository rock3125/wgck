#ifndef _OBJECT_RECORDSYSTEM_H__
#define _OBJECT_RECORDSYSTEM_H__

typedef unsigned char byte;

enum DataType { dtsize_t, dtbool, dtfloat, dtint, dtbyte };

class TTank;
//class TRaceCar;

//==========================================================================

class _EXPORT TData
{
public:
	TData( DataType type );
	~TData( void );
	TData( const TData& );

	const TData& operator=( const TData& );

	void Add( void* );
	void Get( void* );

	void SaveBinary( TPersist& file );
	void LoadBinary( TPersist& file );

	DataType	type;
	size_t		count;
	size_t		index;
	size_t		blockSize;

	byte*		RLE;
	byte*		Data;
};

//==========================================================================

class _EXPORT TTankRecord
{
public:
	TTankRecord( void );
	~TTankRecord( void );
	TTankRecord( const TTankRecord& );

	const TTankRecord& operator=( const TTankRecord& );

	void Add( TTank* tank, size_t keys, bool ctrlDown, float dxa, float dya );

	// returns tank motions and false if finished playback
	bool Get( TTank* tank, size_t& keys, bool& ctrlDown, float& dxa, float& dya );

	bool PlaybackFinished( void ) const;

	bool SaveBinary( TString fname, TString& errStr );
	bool LoadBinary( TString fname, TString& errStr );

private:
	TString	signature;

	// movement data
	TData	keys;
	TData	ctrlDown;
	TData	deltaXAngle;
	TData	deltaYAngle;

	// exact data
	TData	x;
	TData	z;
	TData	yangle;
	TData	barrelAngle;
	TData	turretAngle;
};

//==========================================================================

class _EXPORT TCameraRecord
{
public:
	TCameraRecord( void );
	~TCameraRecord( void );
	TCameraRecord( const TCameraRecord& );

	const TCameraRecord& operator=( const TCameraRecord& );

	void Add( float x, float y, float z, float xangle, float yangle, size_t sel );

	// returns tank motions and false if finished playback
	bool Get( float& x, float& y, float& z, float& xangle, float& yangle, size_t& sel );

	bool PlaybackFinished( void ) const;

	bool SaveBinary( TString fname, TString& errStr );
	bool LoadBinary( TString fname, TString& errStr );

private:
	TString	signature;

	TData	x;
	TData	y;
	TData	z;
	TData	xangle;
	TData	yangle;
	TData	selectedCamera;
};

//==========================================================================
/*
class TCarRecord
{
public:
	TCarRecord( void );
	~TCarRecord( void );
	TCarRecord( const TCarRecord& );

	const TCarRecord& operator=( const TCarRecord& );

	void Add( TRaceCar* car, size_t keys, bool ctrlDown, float dxa, float dya );

	// returns tank motions and false if finished playback
	bool Get( TRaceCar* tank, size_t& keys, bool& ctrlDown, float& dxa, float& dya );

	bool PlaybackFinished( void ) const;

	bool SaveBinary( TString fname, TString& errStr );
	bool LoadBinary( TString fname, TString& errStr );

private:
	TString	signature;

	// movement data
	TData	keys;
	TData	ctrlDown;
	TData	deltaXAngle;
	TData	deltaYAngle;

	// exact data
	TData	x;
	TData	z;
	TData	yangle;
	TData	speed;
};
*/
//==========================================================================

#endif


