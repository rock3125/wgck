//==========================================================================
//
//	Written by Peter de Vocht
//
//==========================================================================

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <common/tstring.h>
#pragma hdrstop

//==========================================================================

#include <stdarg.h>
#include <common/compatability.h>

//==========================================================================
//
//	My string class
//

TString::TString( void )
	: str(NULL),
	  len(0)
{
	nullStr[0] = 0;
};



TString::TString( const TString& _str )
	: str(NULL),
	  len(0)
{
	nullStr[0] = 0;
	*this = _str;
};



TString::TString( const char* _str )
	: str(NULL),
	  len(0)
{
	nullStr[0] = 0;
	*this = _str;
};



TString::TString( char _ch )
	: str(NULL),
	  len(0)
{
	char temp[2];
	nullStr[0] = 0;
	temp[0] = _ch;
	temp[1] = 0;
	*this = temp;
};



TString::TString( long num )
	: str(NULL),
	  len(0)
{
	nullStr[0] = 0;
	char buf[256];
	itoa(num,buf,10);
	*this = buf;	
};



TString::TString( double num )
	: str(NULL),
	  len(0)
{
	nullStr[0] = 0;
	char buf[256];
	sprintf( buf, "%f", num );
	*this = buf;
};



TString::~TString( void )
{
	if ( str!=NULL )
		delete []str;
	str = NULL;
	len = 0;
};



const TString& TString::operator = ( const TString& _str )
{
	*this = _str.c_str();
	return *this;
};



const TString& TString::operator = ( const char* _str )
{
	if ( str!=NULL )
		delete []str;

	len = strlen(_str);
	str = new char[len+1];
	PostCond( str!=NULL );
	for ( size_t i=0; i<=len; i++ )
		str[i] = _str[i];

	return *this;
};



char& TString::operator [] ( size_t index )
{
	PreCond( str!=NULL );
	PreCond( index<=length() );
	return str[index];
};


const char& TString::operator [] ( size_t index ) const
{
	PreCond( str!=NULL );
	PreCond( index<=length() );
	return str[index];
};


const char*	TString::c_str( void ) const
{
	if ( str==NULL )
		return nullStr;
	return str;
};



size_t TString::length( void ) const
{
	if ( str==NULL )
		return 0;
	return strlen( str );
};



TString TString::substr( size_t index, size_t len ) const
{
	PreCond( index<length() );
	
	if ( len==0 )
		return TString( &str[index] );
	else
	{
		TString temp = &str[index];
		if ( len<temp.length() )
			temp[len] = 0;
		return TString( temp );
	}
};


TString TString::lcase( void ) const
{
	TString temp = str;
	return TString( strlwr(const_cast<char*>(temp.c_str())) );
};



TString	TString::ucase( void ) const
{
	TString temp = str;
	return TString( strupr(const_cast<char*>(temp.c_str())) );
};



TString operator + ( const TString& str1, const TString& str2 )
{
	int len = str1.length() + str2.length();
	char* buf = new char[len+1];
	PostCond( buf!=NULL );
	strcpy( buf, str1.c_str() );
	strcat( buf, str2.c_str() );
	TString temp(buf);
	delete []buf;
	return temp;
};



TString operator + ( const TString& str1, const char* str2 )
{
	return str1 + TString(str2);
};



bool operator == ( const TString& str1, const TString& str2 )
{
	if ( str1.c_str()==NULL || str2.c_str()==NULL )
		return false;
	return ( strcmp(str1.c_str(),str2.c_str())==0 );
};



bool operator == ( const TString& str1, const char* str2 )
{
	if ( str1.c_str()==NULL || str2==NULL )
		return false;
	return ( strcmp(str1.c_str(),str2)==0 );
};



bool operator != ( const TString& str1, const TString& str2 )
{
	if ( str1.c_str()==NULL || str2.c_str()==NULL )
		return true;
	return ( strcmp(str1.c_str(),str2.c_str())!=0 );
};



bool operator != ( const TString& str1, const char* str2 )
{
	if ( str1.c_str()==NULL || str2==NULL )
		return true;
	return ( strcmp(str1.c_str(),str2)!=0 );
};



TString IntToString( long num )
{
	return TString(num);
};



long StringToInt( const TString& str )
{
	return atoi(str.c_str());
};



TString FormatString( const char *format, ... )
{
	va_list ap;
	char buf[4096];

	va_start(ap, format );
    vsprintf( buf, format, ap );
	va_end(ap);

	return TString(buf);
};



TString FloatToString( size_t precis1, size_t precis2, const double& num )
{
	char buf[256];
	char fmt[256];
	sprintf( fmt, "%%%d.%df", precis1, precis2 );
	sprintf( buf, fmt, num );
	return TString(buf);
};


size_t TString::NumItems( const char& seperator ) const
{
	size_t cntr = 1;
	size_t len = length();
	for ( size_t i=0; i<len; i++ )
	{
		if ( str[i]==seperator )
			cntr++;
	}
	return cntr;
};


TString TString::GetItem( const char& seperator, size_t index ) const
{
	if ( index==0 )
	{
		size_t i = 0;
		while ( str[i]!=seperator && str[i]!=0 ) i++;
		TString temp;
		temp = str;
		if ( str[i]==seperator )
		{
			temp[i]=0;
			return temp;
		}
		else
			return temp;
	}
	else
	{
		size_t prevChar = 0;
		size_t cntr = 0;
		size_t len = length();
		for ( size_t i=0; i<len; i++ )
		{
			if ( str[i]==seperator )
			{
				cntr++;
			}

			if ( (cntr-1)==index )
			{
				TString temp;
				temp = &str[prevChar];
				temp[i-prevChar] = 0;
				return temp;
			}
			else if ( cntr>index )
			{
				return TString();
			}

			if ( str[i]==seperator )
			{
				prevChar = i+1;
			}
		}
		if ( cntr==index )
		{
			TString temp;
			temp = &str[prevChar];
			return temp;
		}
	}
	return TString();
};

//==========================================================================

