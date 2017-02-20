#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

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


void MixRoutine( void )
{
	char* mix = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

	for ( size_t i=0; i<10; i++ )
	{
		size_t len = strlen(mix);
		for ( size_t j=0; j<1000; j++ )
		{
			int index1 = rand() % len;
			int index2 = rand() % len;
			char ch1 = mix[index1];
			char ch2 = mix[index2];
			mix[index2] = ch1;
			mix[index1] = ch2;
		}
		printf( "%s\n", mix );
	}
};


// given an alphabet and an alphabet size - return its index
size_t CharToIndex( const char* abc, size_t size, char ch )
{
	for ( size_t i=0; i<size; i++ )
		if ( abc[i]==ch )
			return i;
	return 0;
};

// given a name, return a variable size code
// in 'code' - nb. code has a 255 limit, name must
// be << 255.  The code is roughly 1.2 x the original
void CalculateReg( const char* name, char* code )
{
	size_t i;
	char process[256];
	strcpy( process, name );
	strupr( process );
/*
	// scramble symbols
	srand( (int)time(NULL) );
	char str[256];
	strcpy( str, validSymbols );
	for ( i=0; i<255; i++ )
	{
		int index1 = rand() % 64;
		int index2 = rand() % 64;
		char ch1 = str[index1];
		char ch2 = str[index2];
		str[index2] = ch1;
		str[index1] = ch2;
	};
	printf( "\n\"%s\"\n\n", str );
	exit(0);
*/
	// initialise postfix checksum
	size_t checksum[4];
	for ( i=0; i<4; i++ )
		checksum[i] = 0;

	// apply replacement alphabet 1
	size_t len = strlen(process);
	for ( i=0; i<len; i++ )
	{
		code[i] = CharToIndex( validSymbols,64,process[i]);
		checksum[i%4] += size_t(code[i]);
	}
	code[len] = 0;
	len++;

	// convert these 6 bit symbols to 5 bit symbols
	char newCode[256];
	size_t ncIndex = 0;
	size_t index = 0;
	char ch1 = 0;
	char ch2 = code[0];
	while ( (index/6) < len )
	{
		if ( (index%5)==0 && index>0 )
		{
			newCode[ncIndex++] = ch1;
			ch1 = 0;
		}
		if ( (index%6)==0 && index>0 )
		{
			ch2 = code[index/6];
		}
		ch1 = ch1 << 1;
		if ( (ch2&32)>0 )
			ch1 = ch1 | 1;
		ch2 = ch2 << 1;
		index++;
	}
	newCode[ncIndex++] = ch1;

	// use newCode to generate the actual string of alphaBet characters
	// use code to return this value
	for ( i=0; i<256; i++ )
		code[i] = 0;
	for ( i=0; i<ncIndex; i++ )
	{
		code[i] = char(alphaBet[newCode[i]]);
	}

	// generate checksum & append to code
	for ( i=0; i<4; i++ )
	{
		checksum[i] = (checksum[i] & 31);
		code[ncIndex++] = alphaBet[checksum[i]];
	}
	code[ncIndex] = 0;
};


//
// argv[1] is user's full name as in the example shown
//
int main(int argc, void *argv[])
{
	MixRoutine();
/*
   // First parameter is user's full name
   char userName[256];
   char regCode[256];

   if ( argc!=2 )
   {
	   printf( "generateSerial usage: takes 1 parameter, name of person to generate\n" );
	   printf( "                      a serial number for\n" );
	   printf( "            eg. generateSerial bob\n" );
	   printf( "                generateSerial \"billy bob\"\n" );
	   return -1;
   }

   strncpy(userName, (const char*)argv[1], 255);
   
   // Code or function calls to
   // place registration code into 
   // regCode variable
   CalculateReg( userName, regCode );

   printf("%s\n",regCode);
*/
   return 0;
};

