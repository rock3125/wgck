/*
*
*	readpng.c - an example of using read functionality of libpng
*
*
*/
#include <stdlib.h>
#include <pnglib/png.h>

typedef unsigned char	byte;
typedef byte			FAR * bytep;
typedef byte        	FAR * FAR * bytepp;


struct _ReadStruct
{
	char* ptr;
	char* end;
};

/*
*	Is file file_name a png file?
*	return 1 if yes, 0 if not
*/
int CheckPNG( const char *file_name )
{
	char 	buf[8];
	int		ret;
	FILE*	fh;

	fh = fopen(file_name, "rb");
	if ( fh==NULL )
		return 0;
	ret = fread(buf, 1, 8, fh);
	if ( ret!=8 )
	{
		fclose(fh);
		return 0;
	}

	/* Check the signature starting at byte 0, and check all 8 bytes */
	ret = png_check_sig((bytep)buf, 8);
	fclose(fh);
	return ret;
}

/*
*	Example user defined read function
*
*/
void user_read_fn( png_structp png_ptr, bytep data, png_size_t length )
{
	struct _ReadStruct*	io_ptr;
	size_t	   			i;

	io_ptr = (struct _ReadStruct*)CVT_PTR(png_ptr->io_ptr);
	for ( i=0; i<length; i++ )
	{
		data[i] = *io_ptr->ptr;
		io_ptr->ptr++;
		if ( io_ptr->ptr > io_ptr->end+1 )		// ?? error in read routine - 1 byte short
		{
			png_error(png_ptr, "read Error (passed eof)");
		}
	}
}

png_voidp PNGInstanceAllocate(png_structp ignore, png_size_t size)
{
	return malloc(size);
}

void PNGInstanceDeallocate(png_structp ignore, png_voidp ptr)
{
	free(ptr);
};

void FreePNGBuffer( unsigned char* ptr)
{
	free(ptr);
};

/*
*	Read a png file, returns 0 on failure, 1 on success
*
*/
int ReadPNG( const char* fileName,
			 size_t* width, size_t* height, int* bitDepth,
			 unsigned char** rgbReturn, size_t* byteSize,
			 char* errStr )
{
	png_structp		png_ptr;
	png_infop		info_ptr;
	FILE*			fh;
	png_color_16	my_background;
	float			screen_gamma = 2.2f;
	bytepp 			rgb; /* PNG read receive buffer */
	unsigned char*	m_buf;

	int color_type;
	int interlace_type, compression_type, filter_type;
	size_t row,i,j;

	/* set return RGB set to NULL */
	*rgbReturn = NULL;

	if ( CheckPNG(fileName)==0 )
	{
		sprintf( errStr, "Error: File %s is not a PNG file", fileName );
		return 0;
	}

	if ( (fh=fopen(fileName, "rb"))==NULL )
	{
		sprintf( errStr, "Error: Could not open file %s", fileName );
		return 0;
	}

	/*
		Create and initialize the png_struct with the desired error handler
		functions.  If you want to use the default stderr and longjump method,
		you can supply NULL for the last three parameters.  We also check that
		the header file is compatible with the library version.
		We use the version that allows us to put in the memory creators
		to fit with the rest of the code...
	*/
	png_ptr = png_create_read_struct_2(PNG_LIBPNG_VER_STRING,
									   (void *)NULL,
									   NULL,
									   NULL,
									   NULL,	// don't need the mem pointer
									   PNGInstanceAllocate,
									   PNGInstanceDeallocate);
	if (!png_ptr)
	{
		fclose(fh);
		sprintf( errStr, "Error: Incompatible library version" );
		return 0;
	}

	/*
		set our memory system in place - note we don't need the variable
		so we set it to NULL
	*/
	png_set_mem_fn(png_ptr, NULL, PNGInstanceAllocate, PNGInstanceDeallocate);

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		fclose(fh);
		png_destroy_read_struct( &png_ptr, (png_infopp)NULL, (png_infopp)NULL );
		sprintf( errStr, "Error: Could not create info structure for PNG %s", fileName );
		return 0;
	}

	/* set error handling if you are using the setjmp/longjmp method */
	if (setjmp(png_ptr->jmpbuf))
	{
		/* Free all of the memory associated with the png_ptr and info_ptr */
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		fclose(fh);
		/* If we get here, we had a problem reading the file */
    	return 0;
	}

	/* set up the input control if you are using standard C streams */
	png_init_io(png_ptr, fh);

	/*
    	The call to png_read_info() gives us all of the information
    	from the PNG file before the first IDAT (image data chunk).
    */
	png_read_info(png_ptr, info_ptr);

	/* set up the transformations you want.  Note that these are
      all optional.  Only call them if you want them */

	/* expand paletted colors into true RGB triplets */
	if (info_ptr->color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_expand(png_ptr);

	/* expand grayscale images to the full 8 bits */
	if (info_ptr->color_type == PNG_COLOR_TYPE_GRAY && info_ptr->bit_depth < 8)
		png_set_expand(png_ptr);

	/* expand paletted or RGB images with transparency to full alpha channels
	 * so the data will be available as RGBA quartets
	 */
	if (info_ptr->valid & PNG_INFO_tRNS)
		png_set_expand(png_ptr);

	/*
		Set the background color to draw transparent and alpha
		images over.  It is possible to set the red, green, and blue
		components directly for paletted images.
	*/
	if (info_ptr->valid & PNG_INFO_bKGD)
		png_set_background( png_ptr, &(info_ptr->background),
							PNG_BACKGROUND_GAMMA_FILE, 1, 1.0);
	else
		png_set_background( png_ptr, &my_background,
							PNG_BACKGROUND_GAMMA_SCREEN, 0, 1.0);

	/*
		tell libpng to handle the gamma conversion for you.  We only
		need the second call if the screen_gamma isn't the usual 2.2
		or if it is controllable by the user.  It may also be a good
		idea to allow the user to set the file gamma if it is unknown.
	*/
	if ( info_ptr->valid & PNG_INFO_gAMA )
		png_set_gamma(png_ptr, screen_gamma, info_ptr->gamma);
	else
		png_set_gamma(png_ptr, screen_gamma, 0.45);

	/* tell libpng to strip 16 bit/color files down to 8 bits/color */
	if (info_ptr->bit_depth == 16)
		png_set_strip_16(png_ptr);

	png_read_update_info( png_ptr, info_ptr );

	/*
		allocate the memory to hold the image using the fields
		of png_info.
	*/
	if ( !png_get_IHDR( png_ptr, info_ptr, (png_uint_32*)width, (png_uint_32*)height, bitDepth,
						&color_type, &interlace_type, &compression_type, &filter_type) )
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		fclose(fh);
		sprintf( errStr, "Error: could not get header information from file %s", fileName );
		return 0;
	}

	/* the easiest way to read the image */
	*byteSize = *height * info_ptr->rowbytes;
	rgb = (bytepp)malloc( *height * sizeof(bytep) );
	if ( rgb==NULL )
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		fclose(fh);
        sprintf( errStr, "Error: could not allocate memory to load file %s", fileName );
    	return 0;
    }
    for ( row=0; row<*height; row++ )
    {
		rgb[row] = (bytep)malloc(info_ptr->rowbytes);
        if ( rgb[row]==NULL )
        {
			png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
    		fclose(fh);
			sprintf( errStr, "Error: could not allocate memory to load file %s", fileName );
	    	return 0;
        }
    };
    png_read_image(png_ptr, (bytepp)rgb);

    /* convert this strange memory block into a normal rgb set */
    m_buf = (unsigned char*)malloc(*byteSize);
	if ( m_buf==NULL )
	{
		// free memory allocated by malloc in m_buf
		for ( row=0; row<*height; row++ )
			free( rgb[row] );
        free( rgb );

       	sprintf( errStr, "Error: could not allocate memory to convert file %s", fileName );
		return 0;
    }

	for ( i=0; i<*height; i++ )
   	for ( j=0; j<info_ptr->rowbytes; j++ )
       	m_buf[i*info_ptr->rowbytes+j] = rgb[*height-(i+1)][j]; // flipped it - PDV
//       	m_buf[i*info_ptr->rowbytes+j] = rgb[i][j];

    /* free memory allocated by malloc in rgb */
   	for ( i=0; i<*height; i++ )
		free( rgb[i] );
    free( rgb );

    *rgbReturn = m_buf;

	/* read the rest of the file, getting any additional chunks in info_ptr */
	png_read_end(png_ptr, info_ptr);

	/* clean up after the read, and free any memory allocated */
	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

	/* close the file */
	fclose(fh);

	return 1;
}

/*
*	Read a png buffer, returns 0 on failure, 1 on success
*
*/
int ReadPNGBuffer( char* _ptr, char* _end,
				   size_t* width, size_t* height, int* bitDepth,
				   unsigned char** rgbReturn, size_t* byteSize,
				   char* errStr )
{
	png_structp		png_ptr;
	png_infop		info_ptr;
	png_color_16	my_background;
	float			screen_gamma = 2.2f;
	bytepp 			rgb; /* PNG read receive buffer */
	unsigned char*	m_buf;

	int color_type;
	int interlace_type, compression_type, filter_type;
	size_t row,i,j;
	struct _ReadStruct	ReadStruct;

	/* we need to copy the data to use the png destruction system */
	if ( _ptr==NULL || _ptr >= _end || (_end - _ptr) < 8)
	{
		sprintf( errStr, "Error: Resource block is NULL" );
		return 0;
	}
	ReadStruct.ptr = malloc(_end - _ptr);
	ReadStruct.end = ReadStruct.ptr + (_end - _ptr);
	memcpy(ReadStruct.ptr, _ptr, _end - _ptr + 1);

	/* set return RGB set to NULL */
	*rgbReturn = NULL;
	if ( png_check_sig((bytep)ReadStruct.ptr, 8)==0 )
	{
		sprintf( errStr, "Error: Resource is not a PNG file" );
		return 0;
	}

	/*
		Create and initialize the png_struct with the desired error handler
		functions.  If you want to use the default stderr and longjump method,
		you can supply NULL for the last three parameters.  We also check that
		the header file is compatible with the library version.
		We use the version that allows us to put in the memory creators
		to fit with the rest of the code...
	*/
	png_ptr = png_create_read_struct_2(PNG_LIBPNG_VER_STRING,
									   (void *)NULL,
									   NULL,
									   NULL,
									   NULL,	// don't need the mem pointer
									   PNGInstanceAllocate,
									   PNGInstanceDeallocate);
	if (!png_ptr)
	{
		sprintf( errStr, "Error: Incompatible library version" );
		return 0;
	}

	/*
		set our memory system in place - note we don't need the variable
		so we set it to NULL
	*/
	png_set_mem_fn(png_ptr, NULL, PNGInstanceAllocate, PNGInstanceDeallocate);

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		png_destroy_read_struct( &png_ptr, (png_infopp)NULL, (png_infopp)NULL );
    	sprintf( errStr, "Error: Could not create info structure for Resource PNG" );
		return 0;
	}

	/* set error handling if you are using the setjmp/longjmp method */
	if (setjmp(png_ptr->jmpbuf))
	{
    	/* Free all of the memory associated with the png_ptr and info_ptr */
    	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
    	/* If we get here, we had a problem reading the file */
    	return 0;
	}

	/* set up the input control if you are using standard C streams */
	png_set_read_fn( png_ptr, (void *)&ReadStruct, user_read_fn);

	/*
    	The call to png_read_info() gives us all of the information
    	from the PNG file before the first IDAT (image data chunk).
    */
	png_read_info(png_ptr, info_ptr);

	/* set up the transformations you want.  Note that these are
      all optional.  Only call them if you want them */

	/* expand paletted colors into true RGB triplets */
	if (info_ptr->color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_expand(png_ptr);

	/* expand grayscale images to the full 8 bits */
	if (info_ptr->color_type == PNG_COLOR_TYPE_GRAY && info_ptr->bit_depth < 8)
    	png_set_expand(png_ptr);

	/* expand paletted or RGB images with transparency to full alpha channels
	 * so the data will be available as RGBA quartets
     */
	if (info_ptr->valid & PNG_INFO_tRNS)
    	png_set_expand(png_ptr);

	/*
    	Set the background color to draw transparent and alpha
    	images over.  It is possible to set the red, green, and blue
    	components directly for paletted images.
    */
	if (info_ptr->valid & PNG_INFO_bKGD)
    	png_set_background( png_ptr, &(info_ptr->background),
        					PNG_BACKGROUND_GAMMA_FILE, 1, 1.0);
	else
    	png_set_background( png_ptr, &my_background,
        					PNG_BACKGROUND_GAMMA_SCREEN, 0, 1.0);

	/*
    	tell libpng to handle the gamma conversion for you.  We only
    	need the second call if the screen_gamma isn't the usual 2.2
    	or if it is controllable by the user.  It may also be a good
    	idea to allow the user to set the file gamma if it is unknown.
    */
	if ( info_ptr->valid & PNG_INFO_gAMA )
    	png_set_gamma(png_ptr, screen_gamma, info_ptr->gamma);
	else
    	png_set_gamma(png_ptr, screen_gamma, 0.45);

	/* tell libpng to strip 16 bit/color files down to 8 bits/color */
	if (info_ptr->bit_depth == 16)
    	png_set_strip_16(png_ptr);

	png_read_update_info( png_ptr, info_ptr );

	/*
    	allocate the memory to hold the image using the fields
    	of png_info.
    */
    if ( !png_get_IHDR( png_ptr, info_ptr, (png_uint_32*)width, (png_uint_32*)height, bitDepth,
          			    &color_type, &interlace_type, &compression_type, &filter_type) )
    {
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
        sprintf( errStr, "Error: could not get header information from Resource" );
    	return 0;
    }

	/* the easiest way to read the image */
    *byteSize = *height * info_ptr->rowbytes;
    rgb = (bytepp)malloc( *height * sizeof(bytep) );
    if ( rgb==NULL )
    {
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
        sprintf( errStr, "Error: could not allocate memory to load resource" );
    	return 0;
    }
    for ( row=0; row<*height; row++ )
    {
    	rgb[row] = (bytep)malloc(info_ptr->rowbytes);
        if ( rgb[row]==NULL )
        {
			png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
        	sprintf( errStr, "Error: could not allocate memory to load resource" );
	    	return 0;
        }
    };
    png_read_image(png_ptr, (bytepp)rgb);

    /* convert this strange memory block into a normal rgb set */
    m_buf = (unsigned char*)malloc(*byteSize);
    if ( m_buf==NULL )
    {
		// free memory allocated by malloc in m_buf
		for ( row=0; row<*height; row++ )
			free( rgb[row] );
        free( rgb );

       	sprintf( errStr, "Error: could not allocate memory to convert resource" );
    	return 0;
    }

    for ( i=0; i<*height; i++ )
   	for ( j=0; j<info_ptr->rowbytes; j++ )
       	m_buf[i*info_ptr->rowbytes+j] = rgb[*height-(i+1)][j];

	/* free memory allocated by malloc in rgb */
	for ( i=0; i<*height; i++ )
		free( rgb[i] );
    free( rgb );

    *rgbReturn = m_buf;

	/* read the rest of the file, getting any additional chunks in info_ptr */
	png_read_end(png_ptr, info_ptr);

	/* clean up after the read, and free any memory allocated */
	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

	return 1;
}

