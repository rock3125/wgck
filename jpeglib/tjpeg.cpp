#include <precomp_header.h>

#ifndef _MSVC
#include <mem.h>
#endif
#include <jpeglib/tjpeg.h>

//----------------------------------------------------------------------------

TJpeg::TJpeg( void )
	: rgb(NULL)
{
	width = 0;
    height = 0;
    properInit = false;
}

//----------------------------------------------------------------------------

TJpeg::~TJpeg( void )
{
	if ( rgb!=NULL )
    	delete []rgb;
    rgb = NULL;
    properInit = false;
}

//----------------------------------------------------------------------------

bool TJpeg::Load( const char* fileName, char* errstr )
{
	errstr[0] = 0;
    width = 0;
    height = 0;
    properInit = false;

    // free existing bitmap?
    if ( rgb!=NULL )
    	delete []rgb;
    rgb = NULL;

	// read to buffer tmp
    unsigned char* m_buf = JpegFileToRGB( fileName, width, height, errstr );
    if ( m_buf==NULL )
    	return false;

	if ( !BGRFromRGB(m_buf) )
    {
    	strcpy(errstr, "BGRFromRGB: ran out of memory" );
        delete []m_buf;
        return false;
    }

    if ( !VertFlipBuf(m_buf) )
    {
    	strcpy(errstr, "VertFlipBuf: ran out of memory" );
        delete []m_buf;
        return false;
    }

	// DWORD-align for display
    rgb = MakeDwordAlignedBuf(m_buf);
    if ( rgb==NULL )
    {
    	strcpy(errstr, "MakeDwordAlignedBuf: ran out of memory" );
        delete []m_buf;
        return false;
    }

	delete []m_buf;

    //
    // set up a bitmap header, internal use for blitting
    //
    bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmiHeader.biWidth = Width();
    bmiHeader.biHeight = Height();
    bmiHeader.biPlanes = 1;
    bmiHeader.biBitCount = 24;
    bmiHeader.biCompression = BI_RGB;
    bmiHeader.biSizeImage = 0;
    bmiHeader.biXPelsPerMeter = 0;
    bmiHeader.biYPelsPerMeter = 0;
    bmiHeader.biClrUsed = 0;
    bmiHeader.biClrImportant = 0;
    properInit = true;

    return true;
}

//----------------------------------------------------------------------------

bool TJpeg::Load( HINSTANCE instance, LPCTSTR id, char* errstr)
{
	errstr = "";
	width = 0;
	height = 0;
	properInit = false;

	// free existing bitmap?
	if ( rgb!=NULL )
		delete [] rgb;
	rgb = NULL;

	// load up our buffer from resource
	// First, load the resource into a global memory block.
	HANDLE handle = NULL;
	HRSRC  resHandle = ::FindResource(instance, id, "TEMPLATE1" );
	if ( resHandle==0 )
	{
		sprintf(errstr,"Resource handle (%s) null", id );
		return false;
	};

	int size = ::SizeofResource(instance, resHandle);
	if ( size==0 )
	{
		sprintf(errstr,"Resource size (%s) ==0", id );
		return false;
	};

	handle = resHandle ? ::LoadResource(instance, resHandle) : 0;
	if ( handle==NULL )
	{
		sprintf( errstr, "Can't load resource %s", id );
		return false;
	}

	try
	{
		char* ptr = (char*) ::LockResource(handle);
		char* end = ptr + (size-1);
		try
		{
			// read to buffer tmp
			unsigned char* m_buf = ::JpegBufferToRGB( ptr, end, width, height, errstr );
			if ( m_buf==NULL )
			{
				UnlockResource(ptr);
				::FreeResource(handle);
				return false;
			}
			UnlockResource(ptr);
			::FreeResource(handle);

			if ( !BGRFromRGB(m_buf) )
		    {
		    	strcpy(errstr, "BGRFromRGB: ran out of memory" );
		        delete []m_buf;
		        return false;
		    }

			if ( !VertFlipBuf(m_buf) )
		    {
				strcpy(errstr, "VertFlipBuf: ran out of memory" );
			    delete []m_buf;
			    return false;
		    }

			// DWORD-align for display
		    rgb = MakeDwordAlignedBuf(m_buf);
		    if ( rgb==NULL )
		    {
		    	strcpy(errstr, "MakeDwordAlignedBuf: ran out of memory" );
		        delete []m_buf;
		        return false;
		    }
			delete []m_buf;

		    //
		    // set up a bitmap header, internal use for blitting
		    //
		    bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		    bmiHeader.biWidth = Width();
		    bmiHeader.biHeight = Height();
		    bmiHeader.biPlanes = 1;
		    bmiHeader.biBitCount = 24;
		    bmiHeader.biCompression = BI_RGB;
		    bmiHeader.biSizeImage = 0;
		    bmiHeader.biXPelsPerMeter = 0;
		    bmiHeader.biYPelsPerMeter = 0;
		    bmiHeader.biClrUsed = 0;
		    bmiHeader.biClrImportant = 0;
		    properInit = true;

			return true;
		}
		catch (...)
		{
			UnlockResource(ptr);
			throw;
		}
	}
	catch(...)
	{
		::FreeResource(handle);
		throw;
	}
	return false;
};

//----------------------------------------------------------------------------

void TJpeg::Draw( const HDC& dc, const RECT& rect ) const
{
	// if we don't have an image, get out of here
	if ( !properInit )
    	return;

	if (dc!=NULL)
    {
		::StretchDIBits(dc,
        			  rect.left, rect.top,
                      rect.right - rect.left, rect.bottom - rect.top,
					  0,0,
					  Width(),
					  Height(),
					  Bits(),
					  (LPBITMAPINFO)&BitmapInfoHeader(),
					  DIB_RGB_COLORS,
					  SRCCOPY );
	}
}

//--------------------------------------------------------

void TJpeg::Draw( const HDC& dc, const int& left, const int& top ) const
{
	RECT myRect;
    ::SetRect( &myRect, left, top, left+Width(), top+Height() );
	Draw( dc, myRect );
}

//--------------------------------------------------------
//--------------------------------------------------------
//--------------------------------------------------------
//--------------------------------------------------------

unsigned char* TJpeg::MakeDwordAlignedBuf( unsigned char* dataBuf )
{
	int widthPix = width;
	////////////////////////////////////////////////////////////
	// what's going on here? this certainly means trouble
	if (dataBuf==NULL)
		return NULL;

	////////////////////////////////////////////////////////////
	// how big is the smallest DWORD-aligned buffer that we can use?
	int uiWidthBytes;
	uiWidthBytes = WidthBytes(widthPix * 24);

	long dwNewsize=(long)((long)uiWidthBytes * (long)height);
	unsigned char* pNew;

	////////////////////////////////////////////////////////////
	// alloc and open our new buffer
	pNew=(unsigned char *)new char[dwNewsize];
	if (pNew==NULL)
		return NULL;

	////////////////////////////////////////////////////////////
	// copy row-by-row
	int uiInWidthBytes = widthPix * 3;
	int uiCount;
	for (uiCount=0;uiCount < height;uiCount++) {
		unsigned char* bpInAdd;
		unsigned char* bpOutAdd;
		unsigned long lInOff;
		unsigned long lOutOff;

		lInOff=uiInWidthBytes * uiCount;
		lOutOff=uiWidthBytes * uiCount;

		bpInAdd= dataBuf + lInOff;
		bpOutAdd= pNew + lOutOff;

		::memcpy(bpOutAdd,bpInAdd,uiInWidthBytes);
	}

	return pNew;
}

//----------------------------------------------------------------------------

bool TJpeg::BGRFromRGB( unsigned char*buf )
{
	int widthPix = width;
	if (buf==NULL)
		return false;

	int col, row;
	for (row=0;row<height;row++)
    {
		for (col=0;col<widthPix;col++)
        {
			unsigned char *pRed, *pBlu;
			pRed = buf + row * widthPix * 3 + col * 3;
			pBlu = buf + row * widthPix * 3 + col * 3 + 2;

			// swap red and blue
			unsigned char tmp;
			tmp = *pRed;
			*pRed = *pBlu;
			*pBlu = tmp;
		}
	}
	return true;
}

//----------------------------------------------------------------------------

bool TJpeg::VertFlipBuf( unsigned char* inbuf )
{
	unsigned char* tb1;
	unsigned char* tb2;
    int widthBytes = width * 3;

	if (inbuf==NULL)
		return false;

	int bufsize;

	bufsize=widthBytes;
	tb1= (unsigned char *)new char[bufsize];
	if (tb1==NULL)
		return false;

	tb2= (unsigned char*)new char[bufsize];
	if (tb2==NULL)
    {
		delete []tb1;
		return false;
	}

	int row_cnt;
	unsigned long off1;
	unsigned long off2;
	for (row_cnt=0;row_cnt<(height+1)/2;row_cnt++)
    {
		off1=row_cnt*bufsize;
		off2=((height-1)-row_cnt)*bufsize;

		memcpy(tb1,inbuf+off1,bufsize);
		memcpy(tb2,inbuf+off2,bufsize);
		memcpy(inbuf+off1,tb2,bufsize);
		memcpy(inbuf+off2,tb1,bufsize);
	}

	delete []tb1;
	delete []tb2;

	return true;
}

//----------------------------------------------------------------------------


