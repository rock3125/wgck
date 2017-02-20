#ifndef __COMPATABILITY_H__
#define __COMPATABILITY_H__

//==========================================================================

#include <common/tstring.h>

//==========================================================================

void SetLogFile( const TString& logFileName );
void WriteString( const char* fmt, ... );

//==========================================================================

void PreCond1( bool pc, char *info, int l, char *f );
void PostCond1( bool pc, char *info, int l, char *f );

#define PreCond(cond)		{ PreCond1((cond),#cond,__LINE__,__FILE__); }
#define PostCond(cond)		{ PostCond1((cond),#cond,__LINE__,__FILE__); }

//==========================================================================

class Xmsg
{
public:
	Xmsg( char *str );
	~Xmsg( void );
};

//==========================================================================

void ResetPercentage( void );
void DisplayPercentage( size_t current, size_t max );
void FinishPercentage( void );

bool GetRegistryKey( const TString& keyName, const TString& keyItem, size_t& val );
bool SetRegistryKey( const TString& keyName, const TString& keyItem, size_t val );

//==========================================================================

enum FileAccessInfo { none, fileRead, fileWrite, resourceRead };

class TPersist
{
public:
	TPersist( void );
	TPersist( size_t access );
	TPersist( const TPersist& );
	~TPersist( void );

	const TPersist& operator=( const TPersist& );

	// file based operations
	bool FileOpen( const TString& fname );
	bool FileRead( void* buf, size_t size );
	bool FileWrite( const void* buf, size_t size );
	bool FileClose( void );

	static bool FileExists( const TString& fname );

	const TString& ErrorString( void ) const;

private:
	FILE* fh;
	TString errorStr;
	size_t access;
};

//==========================================================================

void ConstructPath( TString& buf, const TString pathname, const TString fname );
TString SplitPath( const TString buf, TString& pathname, TString& fname );

//==========================================================================

#endif


