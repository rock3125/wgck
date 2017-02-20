#ifndef _COMMON_FONT_H__
#define _COMMON_FONT_H__

//==========================================================================

#include <common/string.h>

#define kMaxSizes 40

//==========================================================================

//==========================================================================

//
//	The font parent class and its load and write methods
//
class _EXPORT Font
{
public:
	Font( void );
	virtual ~Font( void );

	// load the font from file
	bool Load( const TString& fname, TString& errStr );

	void Write( float x, float y, float z, const TString& text );  // write text to openGL viewport @ x,y,z
	void WriteReverse( float x, float y, float z, const TString& text );
	void Write( float x, float y, float z, float wdth, const TString& text );

	size_t	GetWidth( const TString& text );
	size_t	GetCharWidth( char ch );

private:
	void FontWriteLetter( size_t ch, float x, float y, float z );
	void FontWriteLetterReverse( size_t ch, float x, float y, float z );

private:
	TString		signature;

	size_t		textureId;

	size_t		cellWidth;
	size_t		cellHeight;
	size_t		widths[256];
};

//==========================================================================

#endif


