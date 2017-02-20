#ifndef __CONTROLSCLASS_TPNG_H__
#define __CONTROLSCLASS_TPNG_H__

//--------------------------------------------------------
//
//	CAREFUL: this code is not re-entrant at this point in time
//	you can not call this code at the same time from two
//	different threads!  It uses global variables in the C
//	modules associated with load JPEG and overwrites standard
//	C error callback hooks
//
//--------------------------------------------------------

#define _CLASSEXPORT __declspec(dllexport)

class _CLASSEXPORT TPNG
{
	TPNG( const TPNG& );
	const TPNG& operator=(const TPNG&);
public:
	TPNG( void ); // ordinary constructor
	virtual ~TPNG( void );

	// load JPEG from file, return T|F and errstring if false
	const bool Load( const char* fileName, char* errstr );

	// draw inside a rectangle on a DC
	void Draw( const HDC& dc, const RECT& rect, bool stretch=true ) const;

	// draw at (x,y) on a DC
	void Draw( const HDC& dc, const int& left, const int& top ) const;

	// get Width, Height, RGB bits
	const int&		Width( void ) const;
	const int&		Height( void ) const;
	unsigned char*	Bits( void ) const;
	const size_t	ColourDepth( void ) const;
	const bool&		OK( void ) const;

	virtual const BITMAPINFOHEADER& BitmapInfoHeader( void ) const;

protected:
	// internal converter from primary jpeg result to bitmap
	const bool TranscribeLoad( unsigned char* m_buf, char* errstr );

	void Width( const int& _width );
	void Height( const int& _height );
	void Bits( unsigned char* _rgb );

private:
	unsigned char* 	MakeDwordAlignedBuf(unsigned char* dataBuf );
	bool 			BGRFromRGB( unsigned char*buf );
	bool 			VertFlipBuf( unsigned char* inbuf );
	int  			WidthBytes( int pixels ) const;

private:
	unsigned char* 		rgb;
	int					width;
	int					height;
	BITMAPINFOHEADER	bmiHeader;
	bool				properInit;
};

#endif

