#ifndef __PNGLIB_READPNG_H__
#define __PNGLIB_READPNG_H__

extern "C"
{
/* is file fileName a PNG file (checks signature) - returns 1 if so */
int CheckPNG( const char *file_name );

/*
	Read a PNG file given a filename
    returns: width, height, bitDepth, rgbReturn
    		and byteSize (total size of memory block in bytes)

    returns 0 & errStr on failure or
    returns 1 and valid return values for width, height, bitDepth and rgbReturn on success
*/
int ReadPNG( const char* fileName,
			 size_t* width, size_t* height, int* bitDepth,
			 unsigned char** rgbReturn, size_t* byteSize,
			 char* errStr );

/*
	Read a PNG file from a memory buffer (eg. resource) - starting at ptr, ending at end
	returns: width, height, bitDepth, rgbReturn
			and byteSize (total size of memory block in bytes)

	returns 0 & errStr on failure or
	returns 1 and valid return values for width, height, bitDepth and rgbReturn on success
*/
int ReadPNGBuffer( char* ptr, char* end,
				   size_t* width, size_t* height, int* bitDepth,
				   unsigned char** rgbReturn, size_t* byteSize,
				   char* errStr );

/*
	free the buffer returned by the ReadPNG and ReadPNGBuffer
*/

void FreePNGBuffer(unsigned char* rgbReturn);

}

#endif

