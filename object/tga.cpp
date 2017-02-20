#include <precomp_header.h>

#include <object/tga.h>

//==========================================================================

TGA::TGA( void )
	: m_pixels( NULL )
{
	m_pixelDepth = 0;
	m_type = tgaUndefined;
	m_alphaBits = 0;
	m_width = 0;
	m_height = 0;
	m_loaded = false;
};

TGA::TGA( const TGA& tga )
	: m_pixels( NULL )
{
	operator=( tga );
};

const TGA& TGA::operator=( const TGA& tga )
{
	m_pixelDepth = tga.m_pixelDepth;
	m_type = tga.m_type;
	m_alphaBits = tga.m_alphaBits;
	m_width = tga.m_width;
	m_height = tga.m_height;
	m_loaded = tga.m_loaded;

	if ( m_pixels!=NULL )
	{
		delete[] m_pixels;
	}
	m_pixels = NULL;

	if ( m_width>0 && m_height>0 && m_pixelDepth>=8 )
	{
		size_t size = m_width*m_height*(m_pixelDepth/8);
		m_pixels = new byte[size];
		for ( size_t i=0; i<size; i++ )
		{
			m_pixels[i] = tga.m_pixels[i];
		}
	}
	return *this;
};

bool TGA::isRGB( void ) const
{
	return (m_type == tgaRGB);
};

bool TGA::isRGBA( void ) const
{
	return (m_type == tgaRGBA);
};

size_t TGA::Width( void ) const
{
	return m_width;
};

size_t TGA::Height( void ) const
{
	return m_height;
};

byte* TGA::Rgb( void ) const
{
	return m_pixels;
};

size_t TGA::Size( void ) const
{
	return m_width*m_height*(m_pixelDepth/8);
};

bool TGA::LoadBinary( const TString& fname, const TString& path, TString& errStr )
{
	TString filename;
	::ConstructPath( filename, path, fname );
	return LoadBinary( filename, errStr );
};

bool TGA::LoadBinary( const TString& filename, TString& errStr )
{
	bool rle = false;
	bool truecolor = false;
	int  currentPixel = 0;

	byte ch_buf1, ch_buf2;
	byte buf1[1024];

	byte idLength;
	byte idColorMapType;
	byte idImageType;

	TPersist file(fileRead);
	if ( !file.FileOpen( filename ) )
	{
		errStr = file.ErrorString();
		return false;
	}

	file.FileRead( &idLength, 1 );
	file.FileRead( &idColorMapType, 1 );
	if (idColorMapType == 1)
	{
		errStr = "TGA: 256 colour types or less are not supported";
		return false;
	}

	file.FileRead( &idImageType, 1 );

	switch (idImageType)
	{
		case 2:
			{
				truecolor = true;
				break;
			}
		case 3:
			{
				m_type = tgaGreyScale;
				break;
			}
		case 10:
			{
				rle = true;
				truecolor = true;
				break;
			}
		case 11:
			{
				rle = true;
				m_type = tgaGreyScale;
				break;
			}
		default:
			{
				errStr = "TGA: unsupported type";
				return false;
			}
	}

	// skip 9 - colour map data(5) & X and Y origin data(4)
	file.FileRead( buf1, 9 );

	file.FileRead( &m_width, sizeof(short) );
	file.FileRead( &m_height, sizeof(short) );
	file.FileRead( &m_pixelDepth, 1 );

	if ( !((m_pixelDepth == 8) || (m_pixelDepth ==  24) ||
		   (m_pixelDepth == 16) || (m_pixelDepth == 32)) )
	{
		errStr = "TGA: pixel depth not supported";
		return false;
	}

	file.FileRead( &ch_buf1, 1 );
	ch_buf2 = 15; //00001111;
	m_alphaBits = ch_buf1 & ch_buf2;

	if ( ! ((m_alphaBits == 0) || (m_alphaBits == 8)) )
	{
		errStr = "TGA: alpha bitplane depth not supported";
		return false;
	}

	if (truecolor)
	{
		m_type = tgaRGB;
		if (m_pixelDepth == 32)
		{
			m_type = tgaRGBA;
		}
	}

	if ( m_type == tgaUndefined )
	{
		errStr = "TGA: type undefined";
		return false;
	};

	if ( m_type == tgaGreyScale )
	{
		errStr = "TGA: greyscales not allowed";
		return false;
	};

	// skip idLength
	file.FileRead( buf1, idLength );

	size_t mult = m_pixelDepth/8;
	m_pixels = new byte[m_width*m_height*mult];
	if ( m_pixels==NULL )
	{
		errStr = "TGA: not enough memory to load TGA";
		return false;
	}

	if ( !rle )
	{
		if ( !file.FileRead(m_pixels, m_width*m_height*mult) )
		{
			delete[] m_pixels;
			errStr = "TGA: reached eof before end";
			return false;
		}
	}
	else
	{
		while ( currentPixel < m_width*m_height -1 )
		{
			file.FileRead( &ch_buf1, 1 );
			if ((ch_buf1 & 128) == 128)
			{   // this is an rle packet
				ch_buf2 = (byte)((ch_buf1 & 127) + 1);   // how many pixels are encoded using this packet
				if ( !file.FileRead(buf1, mult) )
				{
					delete[] m_pixels;
					errStr = "TGA: reached eof before end in RLE";
					return false;
				}
				for ( int i=currentPixel; i<currentPixel+ch_buf2; i++ )
				for ( size_t j=0; j<mult; j++ )
				{
					m_pixels[i*mult+j] = buf1[j];
				}
				currentPixel += ch_buf2;
			}
			else
			{   // this is a raw packet
				ch_buf2 = (byte)((ch_buf1 & 127) + 1);
				if ( !file.FileRead(buf1, mult*ch_buf2) )
				{
					delete[] m_pixels;
					errStr = "TGA: reading data from file failed";
					return false;
				}
				for (int i=currentPixel; i<currentPixel+ch_buf2; i++)
				for (size_t j=0; j<mult; j++)
					m_pixels[i*mult+j] =  buf1[(i-currentPixel)*mult+j];
				currentPixel += ch_buf2;
			}
		}
	}

	m_loaded = true;

	// swap BGR(A) to RGB(A)
	byte temp;
	if ( (m_type == tgaRGB) || (m_type == tgaRGBA) )
	if ( (m_pixelDepth == 24) || (m_pixelDepth == 32) )
	for (int i= 0; i<m_width*m_height; i++)
	{
		temp = m_pixels[i*mult];
		m_pixels[i*mult] = m_pixels[i*mult+2];
		m_pixels[i*mult+2] = temp;
	}
	return true;
}

//==========================================================================

