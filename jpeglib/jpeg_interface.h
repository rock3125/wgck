#ifndef __JPEGINTERFACE_H__
#define __JPEGINTERFACE_H__

// the return is to a new char buffer that is the simple bitmap
// pixels. has to be deallocated by caller
extern "C"
{

unsigned char* JpegFileToRGB( const char* fileName,	// source
							  int& width, 			// jpeg bmp width
							  int& height,          // jpeg bmp height
							  char* errstr );

unsigned char* JpegBufferToRGB( char* start,		// 1st char in source buffer
								char* last,			// last char in source buffer
								int& width,			// jpeg bmp width
								int& height,        // jpeg bmp height
								char* errstr );
}

#endif

