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


bool ValidCode( const char* _code, char* name )
{
	size_t i;
	char code[256];
	strcpy( code, _code );

	size_t len = strlen(code) - 4;

	size_t checksum[4];
	for ( i=0; i<4; i++ )
		checksum[i] = 0;

	// convert these 5 bit symbols back into 6 bit symbols
	char newCode[256];
	size_t ncIndex = 0;
	size_t index = 0;
	char ch1 = 0;
	char ch2 = CharToIndex(alphaBet,32,code[0]);
	while ( (index/5) < len )
	{
		if ( (index%6)==0 && index>0 )
		{
			newCode[ncIndex] = ch1;
			checksum[ncIndex%4] += size_t(ch1);
			name[ncIndex] = validSymbols[size_t(ch1)];
			ch1 = 0;
			ncIndex++;
		}
		if ( (index%5)==0 && index>0 )
		{
			ch2 = CharToIndex(alphaBet,32,code[index/5]);
		}
		ch1 = ch1 << 1;
		if ( (ch2&16)>0 )
			ch1 = ch1 | 1;
		ch2 = ch2 << 1;
		index++;
	}
	// ignore last byte - garbage!
	newCode[ncIndex] = ch1;
	name[ncIndex] = 0;

	// generate checksum
	bool correct = true;
	for ( i=0; i<4; i++ )
	{
		checksum[i] = (checksum[i] & 31);
		if ( checksum[i]!=CharToIndex(alphaBet,32,code[len+i]) )
			correct = false;
	}
	return correct;
};


bool IsRegistered( const char* code, char* email, char* errStr )
{
	// strip code - first two digits hex code
	if ( strlen(code) < 6 )
	{
		strcpy( errStr ,"invalid serial code, too short" );
		return false;
	}
	else
	{
		char prefixLength[3];
		prefixLength[0] = code[0];
		prefixLength[1] = code[1];
		prefixLength[2] = 0;

		size_t textLen = HexToInt( prefixLength );
		{
			const char* abc = "9BHJ0E-N45X2I76CVQ1TP+DKZOAG3WMSYFR8LU";
			const char* txt = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.@";

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
				email[i] = char(ch);
			}
			email[i] = 0;
			strlwr(email);
			size_t realTotal = HexToInt( &code[2+textLen] );
			if ( realTotal==total )
			{
				return true;
			}
			else
			{
				strcpy( errStr, "error: incorrect checksum" );
				return false;
			}
		}
	}
};


//
// argv[1] is user's full name as in the example shown
//
int main(int argc, void *argv[])
{
	// First parameter is user's full name
	char regCode[256];

	if ( argc!=2 )
	{
		printf( "testSerial usage: takes 1 parameter, valid serial number\n" );
		printf( "                  a serial number for\n" );
		printf( "            eg. testSerial serialnumber#\n" );
		return -1;
	}

	strncpy(regCode, (const char*)argv[1], 255);

	// strip code - first two digits hex code
	if ( strlen(regCode) < 6 )
	{
		printf( "invalid serial code, too short\n" );
	}
	else
	{
		char email[256];
		char errStr[256];
		if ( IsRegistered( regCode, email, errStr ) )
		{
			printf( "correct code\nregistered to: %s\n", email );
		}
		else
		{
			printf( "Incorrect serial code: %s\n", errStr );
		}
	}
	return 0;
};

