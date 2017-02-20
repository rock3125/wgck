#ifndef __CONTROLSCLASS_TJPEG_H__
#define __CONTROLSCLASS_TJPEG_H__

//--------------------------------------------------------
//
//	CAREFUL: this code is not re-entrant at this point in time
//	you can not call this code at the same time from two
//	different threads!  It uses global variables in the C
//	modules associated with load JPEG and overwrites standard
//	C error callback hooks
//
//--------------------------------------------------------

#include <jpeglib/jpeg_interface.h>

//--------------------------------------------------------

#ifndef _MSVC
class _export TJpeg
#else
class TJpeg
#endif
{
	TJpeg( const TJpeg& );
    const TJpeg& operator=(const TJpeg&);
public:
	TJpeg( void ); // ordinary constructor
	virtual ~TJpeg( void );

    // load JPEG from file, return T|F and errstring if false
	virtual bool 	Load( const char* fileName, char* errstr );
	virtual bool	Load( HINSTANCE instance, LPCTSTR id, char* errstr );

    // draw inside a rectangle on a DC
	virtual void 			Draw( const HDC& dc, const RECT& rect ) const;

    // draw at (x,y) on a DC
	virtual void 			Draw( const HDC& dc, const int& left, const int& top ) const;

    // get Width, Height, RGB bits
    const int&		Width( void ) const;
    const int&		Height( void ) const;
    unsigned char*	Bits( void ) const;
    const size_t	ColourDepth( void ) const;
    const bool&		OK( void ) const;

private:
    virtual void	Width( const int& _width );
    virtual void	Height( const int& _height );
    virtual void	Bits( unsigned char* _rgb );

    unsigned char* 	MakeDwordAlignedBuf(unsigned char* dataBuf );
	bool 			BGRFromRGB( unsigned char*buf );
	bool 			VertFlipBuf( unsigned char* inbuf );
    int  			WidthBytes( int pixels ) const;

	virtual const BITMAPINFOHEADER& BitmapInfoHeader( void ) const;

private:
	unsigned char* 		rgb;
	int					width;
    int 				height;
    BITMAPINFOHEADER	bmiHeader;
    bool				properInit;
};

//--------------------------------------------------------

inline
void TJpeg::Width( const int& _width )
{
	width = _width;
};

//--------------------------------------------------------

inline
const int& TJpeg::Width( void ) const
{
	return width;
};

//--------------------------------------------------------

inline
void TJpeg::Height( const int& _height )
{
	height = _height;
};

//--------------------------------------------------------

inline
const int& TJpeg::Height( void ) const
{
	return height;
};

//--------------------------------------------------------

inline
unsigned char* TJpeg::Bits( void ) const
{
	return rgb;
};

//--------------------------------------------------------

inline
void TJpeg::Bits( unsigned char* _rgb )
{
	rgb = _rgb;
};

//--------------------------------------------------------

inline
int TJpeg::WidthBytes( int pixels ) const
{
	return (((pixels) + 31) / 32 * 4);
};

//--------------------------------------------------------

inline
const BITMAPINFOHEADER& TJpeg::BitmapInfoHeader( void ) const
{
	return bmiHeader;
}

//--------------------------------------------------------

inline
const size_t TJpeg::ColourDepth( void ) const
{
	return 24;
};

//--------------------------------------------------------

inline
const bool& TJpeg::OK( void ) const
{
	return properInit;
};

//--------------------------------------------------------

#endif

