#include <precomp_header.h>

#include <common/compatability.h>

#include <stdarg.h>
#include <stdlib.h>

//==========================================================================

TString logFile;

//==========================================================================
//
//	Throw an exception
//

Xmsg::Xmsg( char *msg )
{
	WriteString( "\n\n\nFATAL ERROR: %s\n\n\n", msg );
};

//==========================================================================

Xmsg::~Xmsg( void )
{
}

//==========================================================================

void PreCond1( bool pc, char *info, int l, char *f )
{
	char errmsg[256];
	if ( !pc )
	{
		sprintf(errmsg,"Precondition failed.\n\n%s\nfile \"%s\"\nline %d.",info,f,l);
		MessageBox( NULL, errmsg, "Exception", MB_OK | MB_ICONERROR );
		WriteString( errmsg );
		exit(-1);
	}
}

//==========================================================================

void PostCond1( bool pc, char *info, int l, char *f )
{
	char errmsg[256];
	if ( !pc )
	{
		sprintf(errmsg,"Postcondition failed.\n\n%s\nfile \"%s\"\nline %d.",info,f,l);
		MessageBox( NULL, errmsg, "Exception", MB_OK | MB_ICONERROR );
		WriteString( errmsg );
		exit(-1);
	}
}

//==========================================================================

void SetLogFile( const TString& logFileName )
{
	if ( logFileName.length()==0 )
		logFile = "";
	else
	{
		logFile = logFileName;
		FILE* fh = fopen( logFile.c_str(), "w" );
		if ( fh!=NULL )
		{
			fprintf(fh,"-- LOG --\n" );
			fclose(fh);
		}
	}
};


void WriteString( const char* fmt, ... )
{
	char buf[1024];

	va_list ap;

	va_start(ap, fmt );
    vsprintf( buf, fmt, ap );
	va_end(ap);

	printf( buf );
	if ( logFile.length()>0 )
	{
		FILE* fh = fopen( logFile.c_str(), "a" );
		if ( fh!=NULL )
		{
			fprintf( fh, buf );
			fclose( fh );
		}
	}
}

//==========================================================================

size_t prev_percentage;

void ResetPercentage( void )
{
	prev_percentage = 101;
}

void DisplayPercentage( size_t current, size_t max )
{
	float p = (float(current) / float(max)) * 100.0f;
	size_t percentage = size_t(p);
	if ( percentage!=prev_percentage )
	{
		if ( prev_percentage<=101 )
		{
			size_t ssize;
			if ( prev_percentage<10 )
				ssize = 3;
			else if ( prev_percentage<100 )
				ssize = 4;
			else
				ssize = 5;
			for ( size_t i=0; i<ssize; i++ )
				printf( "\b" );
		}
		prev_percentage = percentage;
		printf( "%d %%", percentage );
	}
};


void FinishPercentage( void )
{
	if ( prev_percentage<=101 )
	{
		size_t ssize;
		if ( prev_percentage<10 )
			ssize = 3;
		else if ( prev_percentage<100 )
			ssize = 4;
		else
			ssize = 5;
		for ( size_t i=0; i<ssize; i++ )
			printf( "\b" );
	}
};


// "Software\\PDV\\Performance", "TestCompleted"
bool GetRegistryKey( const TString& keyName, const TString& keyItem, size_t& val )
{
	// write this info to registry
	HKEY  keyRef;
	LONG res = RegOpenKeyEx( HKEY_LOCAL_MACHINE, keyName.c_str(),
							 0, KEY_READ, &keyRef );
	if ( res==ERROR_SUCCESS )
	{
		DWORD size = sizeof(size_t);
		DWORD type = REG_DWORD;
		res = RegQueryValueEx( keyRef, keyItem.c_str(), 
								0, &type, (BYTE*)&val,
								&size );
		if ( res==ERROR_SUCCESS )
		{
			RegCloseKey( keyRef );
			return true;
		}
		else
			return false;
	}
	else
		return false;
};


bool SetRegistryKey( const TString& keyName, const TString& keyItem, size_t val )
{
	// write this info to registry
	HKEY  keyRef;
	DWORD result;
	LONG res = RegCreateKeyEx( HKEY_LOCAL_MACHINE, keyName.c_str(),
							   0, NULL, REG_OPTION_NON_VOLATILE, 
							   KEY_SET_VALUE, NULL, &keyRef, &result );
	if ( res==ERROR_SUCCESS )
	{
		res = RegSetValueEx( keyRef, keyItem.c_str(), 
							 0, REG_DWORD, (const BYTE*)&val, 
							 sizeof(size_t) );
		if ( res==ERROR_SUCCESS )
		{
			RegCloseKey( keyRef );
			return true;
		}
		else
			return false;
		
	}
	else
		return false;
};



TPersist::TPersist( void )
	: fh(NULL),
	  access(0)
{
	errorStr[0] = 0;
};


TPersist::TPersist( size_t _access )
	: fh(NULL),
	  access(_access)
{
};


TPersist::TPersist( const TPersist& f )
{
	operator=(f);
};


TPersist::~TPersist( void )
{
	access = 0;
	if ( fh!=NULL )
		fclose(fh);
	fh = NULL;
};


const TPersist& TPersist::operator=( const TPersist& f )
{
	fh = f.fh;
	errorStr = f.errorStr;
	access = f.access;

	return *this;
};


bool TPersist::FileOpen( const TString& fname )
{
	errorStr = "";

	if ( access == fileRead )
	{
		fh = fopen( fname.c_str(), "rb" );
		if ( fh==NULL )
		{
			errorStr = "Could not open file '" + fname + "' for reading";
			return false;
		}
		return true;
	}
	else if ( access == fileWrite )
	{
		fh = fopen( fname.c_str(), "wb" );
		if ( fh==NULL )
		{
			errorStr = "Could not open file '" + fname + "' for writing";
			return false;
		}
		return true;
	}
	else
	{
		errorStr = "File '" + fname + "' unknown access type";
		return false;
	}
}


bool TPersist::FileRead( void* buf, size_t size )
{
	if ( fh==NULL )
	{
		errorStr = "FileRead: error, fh==NULL";
		return false;
	}
	else if ( access!=fileRead )
	{
		errorStr = "FileRead: file not opened with read access";
		return false;
	}
	if ( fread( buf, 1, size, fh )!=size )
	{
		errorStr = "FileRead: read past end of file";
		return false;
	}
	return true;
};


bool TPersist::FileWrite( const void* buf, size_t size )
{
	if ( fh==NULL )
	{
		errorStr = "FileWrite: error, fh==NULL";
		return false;
	}
	else if ( access!=fileWrite )
	{
		errorStr = "FileWrite: file not opened with write access";
		return false;
	}
	if ( fwrite( buf, 1, size, fh )!=size )
	{
		errorStr = "FileWrite: could not write to file";
		return false;
	}
	return true;
};


bool TPersist::FileClose( void )
{
	if ( fh==NULL )
	{
		errorStr = "FileClose: error, fh==NULL";
		return false;
	}
	fclose( fh );
	fh = NULL;
	return true;
};


bool TPersist::FileExists( const TString& fname )
{
	FILE* fh = fopen( fname.c_str(), "rb" );
	if ( fh==NULL )
		return false;
	else
	{
		fclose(fh);
		return true;
	}
};


const TString& TPersist::ErrorString( void ) const
{
	return errorStr;
};

//==========================================================================

void ConstructPath( TString& buf, const TString pathname, const TString fname )
{
	buf = "";
	if ( pathname.length()>0 )
    {
		buf = pathname;
        size_t index = buf.length();
        if ( buf[index-1]!='\\' )
        {
			buf = buf + "\\";
        }
    }
	buf = buf + fname;
};


TString SplitPath( const TString buf, TString& pathname, TString& fname )
{
	if ( buf.length()>0 )
    {
		size_t index = buf.length();
		while ( buf[index]!='\\' && index>0 ) index--;
		if ( index>0 )
		{
			pathname = buf.substr(0,index);
			fname = buf.substr(index+1);
		}
		else
		{
			pathname = "";
			fname = buf;
		}
    }
    return fname;
};

//==========================================================================
