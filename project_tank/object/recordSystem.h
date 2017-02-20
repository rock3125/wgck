#ifndef _OBJECT_RECORDSYSTEM_H__
#define _OBJECT_RECORDSYSTEM_H__

typedef unsigned char byte;

enum DataType { dtsize_t, dtbool, dtfloat, dtint, dtbyte };

//==========================================================================

class TData
{
public:
	TData( DataType type );
	~TData( void );

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

class TTankRecord
{
	TTankRecord( const TTankRecord& );
	const TTankRecord& operator=( const TTankRecord& );
public:
	TTankRecord( void );
	~TTankRecord( void );

	void Add( size_t keys, bool ctrlDown, byte buttons, float dxa, float dya );
	void Get( size_t& keys, bool& ctrlDown, byte& buttons, float& dxa, float& dya );

	bool SaveBinary( TString fname, TString& errStr );
	void LoadBinary( TString fname );

private:
	TData	keys;
	TData	ctrlDown;
	TData	buttons;
	TData	deltaXAngle;
	TData	deltaYAngle;
};

#endif

//==========================================================================

