#include <string.h>
#include <stdio.h>



size_t HexToInt( const char* str )
{
	size_t res = 0;
	for ( size_t i=0; i<strlen(str); i++ )
	{
		size_t ch = (size_t)str[i];
		if ( ch>='0' && ch<='9' )
		{
			ch = ch - '0';
		}
		else if ( ch>='A' && ch<='F' )
		{
			ch = (ch - 'A') + 10;
		}
		else
		{
			return 0;
		};
		res = res * 16 + ch;
	}
	return res;
};

//
// user name is preferably 8 characters -
// if its more - its truncated, chopped off,
// if its less - its filled in with alphabet codes
// making it less secure - which is no problem
// this code is only used as a deterrent, nothing more
//

// $.GHZ5N\\%+@6LA<MQ^S&(YJD >O;W-,*X1:IKU48/T'#7!|`3R0E)FV9=CB\"~?2P
// *;U2DQ4#C\"|)THF9$Y0V>~3^\\(GIN5=EA,Z!R.1-`WMKL/8@?OJ<P%&+B:6XS7 '
// H^V=XGIL'`(#+<968&W\"ERQC\\TYSP2O%AKM */5U.Z0-D1BF@:~7),|;?J>3N!$4
// .`Q/L;5<1VKO^N|E@Z8),'0*JXHP:2YC\\M(4$7-&#%\"+S=TAI3FR>6D ?BU9G!W~
// <J47=~5K;BS^&|NTY0Z`Q#/-L$*FV>X:.HC?\\)RAW\"UGDI'O2 %1+6@,MP8E(!93

// list of 64 valid symbols
const char* validSymbols = "<J47=~5K;BS^&|NTY0Z`Q#/-L$*FV>X:.HC?\\)RAW\"UGDI'O2 %1+6@,MP8E(!93";
// list of 32 encode symbols
const char* alphaBet = "AEIU7B1CD9NM23YT465JK8LSZFGHPQRV";

// given an alphabet and an alphabet size - return its index
size_t CharToIndex( const char* abc, size_t size, char ch )
{
	for ( size_t i=0; i<size; i++ )
		if ( abc[i]==ch )
			return i;
	return 0;
};


void GenerateSerial( const char* _email, char* code )
{
	char email[256];
	strcpy( email, _email );
	strupr( email );

	size_t code1Len = strlen( email );
	if ( code1Len > 35 )
	{
		email[35] = 0;
	}

	char storeData[256];
	strcpy( storeData, email );

	size_t total = 0;
	size_t len = strlen(storeData);
	code[0] = 0;
	size_t codeIndex = 0;
	size_t i;

	for ( i=0; i<len; i++ )
	{
		char letter = storeData[i];
		char ascii = letter;
		bool valid = false;
		if ( ascii >= 65 )
		{
			if ( ascii <= 91 )
			{
				valid = true;
			}
		}
		if ( ascii >= 48 )
		{
			if ( ascii <= 58 )
			{
				valid = true;
			}
		}
		if ( ascii == 46 )
		{
			valid = true;
		}
		if ( ascii == 64 )
		{
			valid = true;
		}
		if ( valid )
		{
			total = total + ascii;
			code[codeIndex++] = ascii;
		}
	}
	code[codeIndex] = 0;

	// encode the id string - this is the actual coding part
	// encode 0..9 A..Z string into "secret code"
	// . = 0x2e = 46
	// @ = 0x40 = 64

	const char* abc = "9BHJ0E-N45X2I76CVQ1TP+DKZOAG3WMSYFR8LU";
	code1Len = strlen(code);
	int perm = 3;
	char code2[256];
	code2[0] = 0;
	size_t code2Index = 0;
	for ( i=0; i < code1Len; i++ )
	{
		char letter = code[i];
		char ascii = letter;
		if ( ascii >= 65 )
		{
			ascii = ascii - 65;
		}
		if ( ascii >= 48 )
		{
			if ( ascii <= 58 )
			{
				ascii = (ascii - 48) + 26;
			}
		}
		if ( ascii == 46 )
		{
			ascii = (ascii - 46) + 36;
		}
		if ( ascii == 64 )
		{
			ascii = (ascii - 64) + 37;
		}
		ascii = ascii + perm;
		if ( perm == 3 )
		{
			perm = -3;
		}
		else
		{
			perm = 3;
		}
		if ( ascii < 0 )
		{
			ascii = ascii + 38;
		}
		if ( ascii > 37 )
		{
			ascii = ascii - 38;
		}
		code2[code2Index++] = abc[ascii];
	}
	code2[code2Index] = 0;

	// prefix code with hex length
	char prefix[256];
	sprintf( prefix, "%02X", code1Len );

	// postfix code is hex checksum
	char postfix[256];
	sprintf( postfix, "%04X", total );

	// finale code is all together
	strcpy( code, prefix );
	strcat( code, code2 );
	strcat( code, postfix );
};


//
// argv[1] is user's full name as in the example shown
//
int main(int argc, void *argv[])
{
	// First parameter is user's full name
	char email[256];

	if ( argc!=2 )
	{
		printf( "genSerial usage: takes 1 parameter, email address\n" );
		printf( "             eg. genSerial email@test.com\n" );
		return -1;
	}

	strncpy(email, (const char*)argv[1], 255);
	char code[256];

	GenerateSerial( email, code );
	printf( "generated code: %s\n", code );

	return 0;
};

