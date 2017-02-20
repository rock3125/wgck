#ifndef __COMPATABILITY_H__
#define __COMPATABILITY_H__

//==========================================================================

#include <common/string.h>

//==========================================================================

typedef unsigned char byte;
typedef unsigned long ulong;

class TEvent;

//==========================================================================

void _EXPORT SetLogFile( const TString& logFileName );
void _EXPORT WriteString( const char* fmt, ... );

// message loop handling
void _EXPORT ProcessMessages( TEvent* app, MSG& msg, size_t& fpsTimer );

// for message box display as we know it
int _EXPORT Message( TEvent* app, const TString& msg, const TString& title, size_t flags );

// return a number between [0..1]
float _EXPORT Random( void );

//==========================================================================

void _EXPORT PreCond1( bool pc, char *info, int l, char *f );
void _EXPORT PostCond1( bool pc, char *info, int l, char *f );

#define PreCond(cond)		{ PreCond1((cond),#cond,__LINE__,__FILE__); }
#define PostCond(cond)		{ PostCond1((cond),#cond,__LINE__,__FILE__); }

//==========================================================================

class _EXPORT Xmsg
{
public:
	Xmsg( char *str );
	~Xmsg( void );
};

//==========================================================================

void _EXPORT ResetPercentage( void );
void _EXPORT DisplayPercentage( size_t current, size_t max );
void _EXPORT FinishPercentage( void );

bool _EXPORT GetRegistryKey( const TString& keyName, const TString& keyItem, size_t& val );
bool _EXPORT GetRegistryKey( const TString& keyName, const TString& keyItem, TString& str );

bool _EXPORT SetRegistryKey( const TString& keyName, const TString& keyItem, size_t val );
bool _EXPORT SetRegistryKey( const TString& keyName, const TString& keyItem, const TString& str );

//==========================================================================

enum FileAccessInfo { none, fileRead, fileWrite, resourceRead };

class _EXPORT TPersist
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
	
	bool ReadLine( TString& line );
	bool ReadString( TString& line );

	bool FileWrite( const void* buf, size_t size );
	bool FileClose( void );
	long FileSize( void );

	static bool FileExists( const TString& fname );

	const TString& ErrorString( void ) const;
	const TString& FileName( void ) const;

private:
	FILE* fh;
	TString errorStr;
	TString filename;
	size_t access;
};

//==========================================================================

void _EXPORT ConstructPath( TString& buf, const TString pathname, const TString fname );
TString _EXPORT SplitPath( const TString buf, TString& pathname, TString& fname );

//==========================================================================
//
// verify serial#
//
size_t _EXPORT CharToIndex( const TString& abc, size_t size, char ch );
size_t _EXPORT HexToInt( const TString& str );

inline
bool IsRegistered( TString& name, TString& email )
{
	TString code;
	if ( !::GetRegistryKey( "Software\\PDV\\Performance", "RegoCode", code ) )
		return false;

	if ( !::GetRegistryKey( "Software\\PDV\\Performance", "RegoName", name ) )
		return false;

	// strip code - first two digits hex code
	if ( code.length() < 6 )
	{
	   // "invalid serial code, too short"
		return false;
	}
	else
	{
		TString prefixLength = code.substr(0,2);

		size_t textLen = HexToInt( prefixLength );
		if ( textLen==0 || textLen > code.length() )
		{
		   // "invalid serial code, prefix length incorrect"
			return false;
		}
		else
		{
			TString abc = "9BHJ0E-N45X2I76CVQ1TP+DKZOAG3WMSYFR8LU";
			TString txt = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.@";

			size_t index = 0;
			size_t total = 0;
			int ch;
			int perm = -3;
			for ( size_t i=0; i<textLen; i++ )
			{
				ch = int(CharToIndex(abc,38,code[i+2])) + perm;
				if ( ch<0 )
				{
					ch += 38;
				}
				else if ( ch>37 )
				{
					ch -= 38;
				}
				perm = perm * -1;
				ch = txt[ ch ];
				total = total + size_t(ch);
				email = email + TString(char(ch));
			}
			email = email.lcase();
			size_t realTotal = HexToInt( &code[2+textLen] );
			return realTotal==total;
		}
	}
};


inline
bool ValidCode( const TString& code )
{
	// strip code - first two digits hex code
	if ( code.length() < 6 )
	{
	   // "invalid serial code, too short"
		return false;
	}
	else
	{
		TString prefixLength = code.substr(0,2);

		size_t textLen = HexToInt( prefixLength );
		if ( textLen==0 || textLen > code.length() )
		{
		   // "invalid serial code, prefix length incorrect"
			return false;
		}
		else
		{
			TString abc = "9BHJ0E-N45X2I76CVQ1TP+DKZOAG3WMSYFR8LU";
			TString txt = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.@";

			size_t index = 0;
			size_t total = 0;
			int ch;
			int perm = -3;
			for ( size_t i=0; i<textLen; i++ )
			{
				ch = int(CharToIndex(abc,38,code[i+2])) + perm;
				if ( ch<0 )
				{
					ch += 38;
				}
				else if ( ch>37 )
				{
					ch -= 38;
				}
				perm = perm * -1;
				ch = txt[ ch ];
				total = total + size_t(ch);
			}
			size_t realTotal = HexToInt( &code[2+textLen] );
			return realTotal==total;
		}
	}
};


size_t _EXPORT GetNumDaysBeforeExpire( void );
bool _EXPORT HasExpired( void );

#ifdef _ISADMIN
TString _EXPORT GenerateCode( const TString& name );
#endif

//==========================================================================

TString _EXPORT KeyToString( size_t languageId, size_t key );
#ifdef _USEGUI
size_t _EXPORT GetUserDefinedKey( TEvent* app );
#endif

//==========================================================================

#endif


