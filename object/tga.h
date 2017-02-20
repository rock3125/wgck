#ifndef _OBJECTS_TGA_H_
#define _OBJECTS_TGA_H_

//==========================================================================

enum imageType
{
	tgaUndefined,
	tgaGreyScale,
	tgaRGB,
	tgaRGBA
};

class _EXPORT TGA
{
public:
	TGA( void );
	TGA( const TGA& );

	const TGA& operator=( const TGA& );

	bool LoadBinary( const TString& fname, const TString& path, TString& errStr );
	bool LoadBinary( const TString& filename, TString& errStr );

	bool isRGB( void ) const;
	bool isRGBA( void ) const;

	size_t	Width( void ) const;
	size_t	Height( void ) const;
	size_t	Size( void ) const;

	byte*	Rgb( void ) const;

private:
	byte	m_pixelDepth;
	byte	m_type;
	byte	m_alphaBits;
	short	m_width;
	short	m_height;
	byte*	m_pixels;
	bool	m_loaded;
};

//==========================================================================

#endif
