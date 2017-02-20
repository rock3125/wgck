#include <windows.h>
#include <stdio.h>

#include <pnglib/tpng.h>
#include <pnglib/readpng.h>

//==========================================================================

TPNG::TPNG( void )
	: rgb(NULL)
{
	width = 0;
	height = 0;
	properInit = false;
}

TPNG::~TPNG( void )
{
	if ( rgb!=NULL )
		delete []rgb;
	rgb = NULL;
	properInit = false;
};

const bool TPNG::Load( const char* fileName, char* errstr)
{
	if ( fileName==NULL )
		return false;

	if ( errstr!=NULL )
		errstr[0] = 0;

	width = 0;
	height = 0;
	properInit = false;

	// free existing bitmap?
	if ( rgb!=NULL )
		delete []rgb;
	rgb = NULL;

	// read to buffer tmp
	unsigned char* m_buf = NULL;
	size_t byteSize = 0;
	int bitDepth = 0;
	size_t w = 0, h = 0;

	char err[256];
	::memset(err, 0, sizeof(err));
	if ( ::ReadPNG( fileName, &w, &h, &bitDepth, &m_buf, &byteSize, err )==0 )
	{
		if ( errstr!=NULL )
		{
			strcpy( errstr, err );
		}
		return false;
	}
	width = int(w);
	height = int(h);
	return TranscribeLoad(m_buf, errstr);
};

const bool TPNG::TranscribeLoad( unsigned char* m_buf, char* errstr )
{
	if ( m_buf == NULL )
	{
		if ( errstr!=NULL )
		{
			strcpy( errstr, "NULL return buffer" );
		}
		return false;
	}

	if ( !BGRFromRGB(m_buf) )
	{
		if ( errstr!=NULL )
		{
			strcpy( errstr, "BGRFromRGB: ran out of memory" );
		}
		::FreePNGBuffer(m_buf);
		return false;
	}

	// DWORD-align for display
	rgb = MakeDwordAlignedBuf(m_buf);
	if ( rgb==NULL )
	{
		errstr = "MakeDwordAlignedBuf: ran out of memory";
		::FreePNGBuffer(m_buf);
		return false;
	}

	::FreePNGBuffer(m_buf);

	//
	// set up a bitmap header, internal use for blitting
	//
	bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmiHeader.biWidth = Width();
	bmiHeader.biHeight = Height();
	bmiHeader.biPlanes = 1;
	bmiHeader.biBitCount = 24;
	bmiHeader.biCompression = BI_RGB;
	bmiHeader.biSizeImage = (long)((long)WidthBytes(width * 24) * (long)height);
	bmiHeader.biXPelsPerMeter = 0;
	bmiHeader.biYPelsPerMeter = 0;
	bmiHeader.biClrUsed = 0;
	bmiHeader.biClrImportant = 0;
	properInit = true;

	return true;
};


void TPNG::Draw( const HDC& dc, const RECT& rect, bool stretch ) const
{
	// if we don't have an image, get out of here
	if ( !properInit )
		return;

	if (dc!=NULL)
	{
		int width, height;
		if ( stretch )
		{
			width = Width();
			height = Height();
		}
		else
		{
			width = rect.right - rect.left;
			height = rect.bottom - rect.top;
		}
		::StretchDIBits(dc,
						rect.left, rect.top,
						rect.right-rect.left, rect.bottom-rect.top,
						0,0,
                        width,
                        height,
                        Bits(),
                        (LPBITMAPINFO)&BitmapInfoHeader(),
                        DIB_RGB_COLORS,
                        SRCCOPY );
	}
}

void TPNG::Draw( const HDC& dc, const int& left, const int& top ) const
{
	RECT r;
	r.left = left;
	r.top = top;
	r.right = left + Width();
	r.bottom = top + Height();
	Draw( dc, r );
}

//--------------------------------------------------------
//--------------------------------------------------------
//--------------------------------------------------------
//--------------------------------------------------------

unsigned char* TPNG::MakeDwordAlignedBuf( unsigned char* dataBuf )
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

bool TPNG::BGRFromRGB( unsigned char*buf )
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

bool TPNG::VertFlipBuf( unsigned char* inbuf )
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

void TPNG::Width( const int& _width )
{
	width = _width;
};

const int& TPNG::Width( void ) const
{
	return width;
};

void TPNG::Height( const int& _height )
{
	height = _height;
};

const int& TPNG::Height( void ) const
{
	return height;
};

unsigned char* TPNG::Bits( void ) const
{
	return rgb;
};

void TPNG::Bits( unsigned char* _rgb )
{
	rgb = _rgb;
};

int TPNG::WidthBytes( int pixels ) const
{
	return (((pixels) + 31) / 32 * 4);
};

const BITMAPINFOHEADER& TPNG::BitmapInfoHeader( void ) const
{
	return bmiHeader;
}

const size_t TPNG::ColourDepth( void ) const
{
	return 24;
};

const bool& TPNG::OK( void ) const
{
	return properInit;
};

//--------------------------------------------------------

