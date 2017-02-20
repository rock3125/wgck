//--------------------------------------------------------
//
//	Some of this code is a bit messy due to the unfortunate nature
//	of the JPEG library which is entirely written in C and uses some
//  arcane stuff to do things - but it works - so don't fix it unless
//  you want to spend the next two weeks trying to read it all.
//

//----------------------------------------------------------------------------

#include <stdio.h>
#include <memory.h>
//#include <jpeglib/jpeg_interface.h>

extern "C"
{
	#include <jpeglib/jpeglib.h>

	// prototype to set up a buffer only source
	// datastart points to the first byte in a range of source
	// datastart points to the last byte in a range of source
	// source in source/jpeglib/jmemsrc.c
	GLOBAL(void)
	jpeg_buffer_src (j_decompress_ptr cinfo, JOCTET* datastart, JOCTET* dataend );
}

#include <jpeglib/jpeg_interface.h>

//----------------------------------------------------------------------------

//
// <setjmp.h> is used for the optional error recovery mechanism shown in
// the second part of the example.
//
#include <setjmp.h>


void j_putRGBScanline(unsigned char* jpegline, int widthPix, unsigned char* outBuf, int row );
void j_putGrayScanlineToRGB(unsigned char* jpegline, int widthPix, unsigned char* outBuf, int row );

//
// error handler, to avoid those pesky exit(0)'s
//
struct my_error_mgr
{
  struct jpeg_error_mgr pub;	// "public" fields
  jmp_buf setjmp_buffer;		// for return to caller
};

typedef struct my_error_mgr * my_error_ptr;
METHODDEF(void)my_error_exit(j_common_ptr cinfo);

//
//	to handle fatal errors.
//	the original JPEG code will just exit(0). can't really
//	do that in Windows....
//
char temp_error_buffer[256]; // this global - messy but necessairy

METHODDEF(void)my_error_exit(j_common_ptr cinfo)
{
	// cinfo->err really points to a my_error_mgr struct, so coerce pointer
	my_error_ptr myerr = (my_error_ptr)cinfo->err;

	// Create the message
    temp_error_buffer[0] = 0;
	(*cinfo->err->format_message)(cinfo, temp_error_buffer);

	// Return control to the setjmp point
	longjmp(myerr->setjmp_buffer, 1);
}

//----------------------------------------------------------------------------

//
//	read a JPEG file
//	returns NULL and errstr on failure
//
unsigned char* JpegFileToRGB( const char* fileName, int& width, int& height, char* errstr )
{
	// basic code from IJG Jpeg Code v6 example.c

	// This struct contains the JPEG decompression parameters and pointers to
	// working space (which is allocated as needed by the JPEG library).
	//
	struct jpeg_decompress_struct cinfo;
	// We use our private extension JPEG error handler.
	// Note that this struct must live as long as the main JPEG parameter
	// struct, to avoid dangling-pointer problems.
	//

	struct my_error_mgr jerr;
	// More stuff
	FILE * infile;		// source file

	JSAMPARRAY buffer;		// Output row buffer
	int row_stride;		// physical row width in output buffer

	// In this example we want to open the input file before doing anything else,
	// so that the setjmp() error recovery below can assume the file is open.
   	// VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
	// requires it in order to read binary files.
	//
    unsigned char* dataBuf = NULL;
    
	width=0;
	height=0;

	if ((infile = fopen(fileName, "rb")) == NULL)
    {
		sprintf(errstr, "JPEG :\nCan't open %s\n", fileName);
		return NULL;
	}

	// Step 1: allocate and initialize JPEG decompression object

	// We set up the normal JPEG error routines, then override error_exit.
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;


	// Establish the setjmp return context for my_error_exit to use.
	if (setjmp(jerr.setjmp_buffer))
    {
    	//
		// If we get here, the JPEG code has signaled an error.
		// We need to clean up the JPEG object, close the input file, and return.
        //
		jpeg_destroy_decompress(&cinfo);

		if (infile!=NULL)
			fclose(infile);

        if ( dataBuf!=NULL )
        	delete []dataBuf;

		sprintf( errstr, "%s", temp_error_buffer );

		return NULL;
	}

	// Now we can initialize the JPEG decompression object.
	jpeg_create_decompress(&cinfo);

	// Step 2: specify data source (eg, a file)
	jpeg_stdio_src(&cinfo, infile);

	// Step 3: read file parameters with jpeg_read_header()
	(void) jpeg_read_header(&cinfo, TRUE);

	// We can ignore the return value from jpeg_read_header since
	//   (a) suspension is not possible with the stdio data source, and
	//   (b) we passed TRUE to reject a tables-only JPEG file as an error.
	// See libjpeg.doc for more info.
	//
	// Step 4: set parameters for decompression

	// In this example, we don't need to change any of the defaults set by
	// jpeg_read_header(), so we do nothing here.
	//

	// Step 5: Start decompressor
	(void) jpeg_start_decompress(&cinfo);

	// We can ignore the return value since suspension is not possible
	// with the stdio data source.
	//

	// We may need to do some setup of our own at this point before reading
	// the data.  After jpeg_start_decompress() we have the correct scaled
	// output image dimensions available, as well as the output colormap
	// if we asked for color quantization.
	// In this example, we need to make an output work buffer of the right size.
	//

	////////////////////////////////////////////////////////////
	// alloc and open our new buffer
	dataBuf=(unsigned char *)new char[cinfo.output_width * 3 * cinfo.output_height];
	if (dataBuf==NULL)
    {
		sprintf(errstr,"JpegFile :\nOut of memory");
		jpeg_destroy_decompress(&cinfo);

		fclose(infile);

		return NULL;
	}

	// how big is this thing gonna be?
	width = cinfo.output_width;
	height = cinfo.output_height;

	// JSAMPLEs per row in output buffer
	row_stride = cinfo.output_width * cinfo.output_components;

	// Make a one-row-high sample array that will go away when done with image
	buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

	// Step 6: while (scan lines remain to be read)
	//           jpeg_read_scanlines(...);

	// Here we use the library's state variable cinfo.output_scanline as the
	// loop counter, so that we don't have to keep track ourselves.
	//
	while (cinfo.output_scanline < cinfo.output_height)
    {
		// jpeg_read_scanlines expects an array of pointers to scanlines.
		// Here the array is only one element long, but you could ask for
		// more than one scanline at a time if that's more convenient.
        //
		(void) jpeg_read_scanlines(&cinfo, buffer, 1);

		// Assume put_scanline_someplace wants a pointer and sample count.
		// asuumer all 3-components are RGBs
		if (cinfo.out_color_components==3)
        {
			j_putRGBScanline(buffer[0],
								width,
								dataBuf,
								cinfo.output_scanline-1);

		}
        else if (cinfo.out_color_components==1)
        {
			// assume all single component images are grayscale
			j_putGrayScanlineToRGB(buffer[0],
								width,
								dataBuf,
								cinfo.output_scanline-1);
		}
	}

	// Step 7: Finish decompression

	(void) jpeg_finish_decompress(&cinfo);
	// We can ignore the return value since suspension is not possible
	// with the stdio data source.

	// Step 8: Release JPEG decompression object

	// This is an important step since it will release a good deal of memory.
	jpeg_destroy_decompress(&cinfo);

	// After finish_decompress, we can close the input file.
	// Here we postpone it until after no more JPEG errors are possible,
	// so as to simplify the setjmp error logic above.  (Actually, I don't
	// think that jpeg_destroy can do an error exit, but why assume anything...)
	fclose(infile);

	// At this point you may want to check to see whether any corrupt-data
	// warnings occurred (test whether jerr.pub.num_warnings is nonzero).
	return dataBuf;
}

//	read a JPEG file
//	returns NULL and errstr on failure
unsigned char* JpegBufferToRGB( char* start,
								char* last,
								int& width,
								int& height,
								char* errstr )
{
	struct jpeg_decompress_struct cinfo;

	// We use our private extension JPEG error handler.
	// Note that this struct must live as long as the main JPEG parameter
	// struct, to avoid dangling-pointer problems.
	struct my_error_mgr jerr;

	JSAMPARRAY buffer;		// Output row buffer
	int row_stride;		// physical row width in output buffer

	unsigned char* dataBuf = NULL;

	width=0;
	height=0;

	// Step 1: allocate and initialize JPEG decompression object

	// We set up the normal JPEG error routines, then override error_exit.
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;

	// Establish the setjmp return context for my_error_exit to use.
	if (setjmp(jerr.setjmp_buffer))
	{
		// If we get here, the JPEG code has signaled an error.
		// We need to clean up the JPEG object, and return.
		jpeg_destroy_decompress(&cinfo);

		if ( dataBuf!=NULL )
			delete []dataBuf;

		sprintf( errstr, "%s", temp_error_buffer );

		return NULL;
	}

	jpeg_create_decompress(&cinfo);
	jpeg_buffer_src(&cinfo, (JOCTET*) start, (JOCTET*) last);
	(void) jpeg_read_header(&cinfo, TRUE);

	(void) jpeg_start_decompress(&cinfo);

	// alloc and open our new buffer
	dataBuf=(unsigned char *)new char[cinfo.output_width * 3 * cinfo.output_height];
	if (dataBuf==NULL)
	{
		sprintf(errstr,"JpegBuffer :\nOut of memory");
		jpeg_destroy_decompress(&cinfo);
		return NULL;
	}

	// how big is this thing gonna be?
	width = cinfo.output_width;
	height = cinfo.output_height;

	// JSAMPLEs per row in output buffer
	row_stride = cinfo.output_width * cinfo.output_components;

	// Make a one-row-high sample array that will go away when done with image
	buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

	while (cinfo.output_scanline < cinfo.output_height)
	{
		(void) jpeg_read_scanlines(&cinfo, buffer, 1);
		if (cinfo.out_color_components==3)
		{
			j_putRGBScanline(buffer[0],
								width,
								dataBuf,
								cinfo.output_scanline-1);

		}
		else if (cinfo.out_color_components==1)
		{
			// assume all single component images are grayscale
			j_putGrayScanlineToRGB(buffer[0],
								width,
								dataBuf,
								cinfo.output_scanline-1);
		}
	}

	(void) jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	// At this point you may want to check to see whether any corrupt-data
	// warnings occurred (test whether jerr.pub.num_warnings is nonzero).
	return dataBuf;
}

//----------------------------------------------------------------------------

//
//	stash a scanline
//
void j_putRGBScanline(unsigned char* jpegline, int widthPix,
					  unsigned char* outBuf, int row )
{
	int offset = row * widthPix * 3;
	int count;
	for (count=0;count<widthPix;count++)
	{
		*(outBuf + offset + count * 3 + 0) = *(jpegline + count * 3 + 0);
		*(outBuf + offset + count * 3 + 1) = *(jpegline + count * 3 + 1);
		*(outBuf + offset + count * 3 + 2) = *(jpegline + count * 3 + 2);
	}
}

//----------------------------------------------------------------------------

//
//	stash a gray scanline
//
void j_putGrayScanlineToRGB(unsigned char* jpegline, int widthPix,
							unsigned char* outBuf, int row )
{
	int offset = row * widthPix * 3;
	int count;
	for (count=0;count<widthPix;count++) {

		unsigned char iGray;

		// get our grayscale value
		iGray = *(jpegline + count);

		*(outBuf + offset + count * 3 + 0) = iGray;
		*(outBuf + offset + count * 3 + 1) = iGray;
		*(outBuf + offset + count * 3 + 2) = iGray;
	}
}

//----------------------------------------------------------------------------


